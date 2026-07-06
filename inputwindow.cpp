#include "inputwindow.h"

#include "apptheme.h"
#include "apptext.h"

#include <QAbstractAnimation>
#include <QCursor>
#include <QEasingCurve>
#include <QFrame>
#include <QGuiApplication>
#include <QHideEvent>
#include <QHBoxLayout>
#include <QKeyEvent>
#include <QLabel>
#include <QLineEdit>
#include <QParallelAnimationGroup>
#include <QPoint>
#include <QPropertyAnimation>
#include <QPushButton>
#include <QScreen>
#include <QStyle>
#include <QTimer>

namespace {
constexpr int kShowAnimationDurationMs = 120;
constexpr int kHideAnimationDurationMs = 90;
constexpr int kShowOffsetY = 8;
constexpr int kHideOffsetY = 6;
constexpr int kSuccessFeedbackDurationMs = 1200;
constexpr int kWarningFeedbackDurationMs = 1400;
constexpr int kAutoHideAfterSaveDelayMs = 800;

void refreshDynamicStyle(QWidget *widget)
{
    widget->style()->unpolish(widget);
    widget->style()->polish(widget);
    widget->update();
    if (widget->isVisible()) {
        widget->repaint();
    }
}
}

InputWindow::InputWindow(QWidget *parent)
    : QWidget(parent)
    , inputPanel(nullptr)
    , input(nullptr)
    , typeButton(nullptr)
    , enterHint(nullptr)
    , escHint(nullptr)
    , feedbackLabel(nullptr)
    , feedbackTimer(nullptr)
    , autoHideTimer(nullptr)
    , inputAnimation(nullptr)
    , opacityAnimation(nullptr)
    , positionAnimation(nullptr)
    , visiblePosition()
    , activeType(MemoType::Question)
    , appLanguage(AppLanguage::ZhCn)
    , questionCategoryName(QStringLiteral("Idea"))
    , todoCategoryName(QStringLiteral("ToDo"))
    , hideAfterSave(false)
    , captureOpen(false)
    , hidingWithAnimation(false)
{
    setupUi();
}

MemoType InputWindow::currentType() const
{
    return activeType;
}

bool InputWindow::isOpenForCapture() const
{
    return captureOpen;
}

void InputWindow::setCurrentType(MemoType type)
{
    if (activeType == type) {
        return;
    }

    activeType = type;
    clearFeedback();
    updateTypeButton();
    emit currentTypeChanged(activeType);
}

void InputWindow::setHideAfterSave(bool enabled)
{
    hideAfterSave = enabled;
}

void InputWindow::setLanguage(AppLanguage language)
{
    if (appLanguage == language) {
        return;
    }

    appLanguage = language;
    clearFeedback();
    retranslateUi();
}

void InputWindow::setCategoryName(MemoType type, const QString &name)
{
    QString &target = type == MemoType::Question ? questionCategoryName : todoCategoryName;
    if (target == name) {
        return;
    }

    target = name;
    clearFeedback();
    updateTypeButton();
}

void InputWindow::toggleCurrentType()
{
    setCurrentType(activeType == MemoType::Question ? MemoType::Todo : MemoType::Question);
}

void InputWindow::showAndFocus()
{
    const bool shouldAnimate = !isVisible();

    if (shouldAnimate) {
        const QScreen *screen = QGuiApplication::screenAt(QCursor::pos());
        if (screen == nullptr) {
            screen = QGuiApplication::primaryScreen();
        }
        if (screen != nullptr) {
            const QRect area = screen->availableGeometry();
            move(area.center() - rect().center());
        }
    }

    if (inputAnimation->state() == QAbstractAnimation::Running) {
        stopInputAnimation();
    }

    captureOpen = true;
    visiblePosition = pos();

    if (shouldAnimate) {
        setWindowOpacity(0.0);
        move(visiblePosition + QPoint(0, kShowOffsetY));
    } else {
        setWindowOpacity(1.0);
    }

    show();
    raise();
    activateWindow();
    input->setFocus();

    if (shouldAnimate) {
        startShowAnimation();
    }
}

void InputWindow::applyTheme(ThemeMode mode, FontSizeMode fontSize, DensityMode density)
{
    setStyleSheet(AppTheme::inputWindowStyleSheet(mode, fontSize, density));
    AppTheme::applyElevation(inputPanel, mode, ElevationLevel::E3);
    updateTypeButton();
}

bool InputWindow::eventFilter(QObject *object, QEvent *event)
{
    if (object == input || object == typeButton) {
        if (event->type() == QEvent::FocusIn || event->type() == QEvent::FocusOut) {
            const bool focused = event->type() == QEvent::FocusIn
                                     || (object == input && typeButton->hasFocus())
                                     || (object == typeButton && input->hasFocus());
            inputPanel->setProperty("focused", focused);
            refreshDynamicStyle(inputPanel);
        }
    }

    return QWidget::eventFilter(object, event);
}

void InputWindow::hideEvent(QHideEvent *event)
{
    clearFeedback();
    captureOpen = false;
    QWidget::hideEvent(event);
}

void InputWindow::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Escape) {
        startHideAnimation();
        event->accept();
        return;
    }

    QWidget::keyPressEvent(event);
}

void InputWindow::setupUi()
{
    setWindowTitle(AppText::inputWindowTitle(appLanguage));
    setWindowFlags(Qt::Tool | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    setAttribute(Qt::WA_TranslucentBackground, true);
    setAutoFillBackground(false);
    setFixedSize(560, 64);

    inputAnimation = new QParallelAnimationGroup(this);
    opacityAnimation = new QPropertyAnimation(this, "windowOpacity");
    positionAnimation = new QPropertyAnimation(this, "pos");
    inputAnimation->addAnimation(opacityAnimation);
    inputAnimation->addAnimation(positionAnimation);
    connect(inputAnimation, &QParallelAnimationGroup::finished, this, [this]() {
        if (hidingWithAnimation) {
            const QPoint restorePosition = visiblePosition;
            hidingWithAnimation = false;
            hide();
            move(restorePosition);
            setWindowOpacity(1.0);
            return;
        }

        move(visiblePosition);
        setWindowOpacity(1.0);
    });

    feedbackTimer = new QTimer(this);
    feedbackTimer->setSingleShot(true);
    connect(feedbackTimer, &QTimer::timeout, this, &InputWindow::clearFeedback);

    autoHideTimer = new QTimer(this);
    autoHideTimer->setSingleShot(true);
    connect(autoHideTimer, &QTimer::timeout, this, &InputWindow::startHideAnimation);

    auto *outerLayout = new QHBoxLayout(this);
    outerLayout->setContentsMargins(8, 8, 8, 8);
    outerLayout->setSpacing(0);

    inputPanel = new QFrame(this);
    inputPanel->setObjectName("InputPanel");
    inputPanel->setProperty("focused", false);

    auto *panelLayout = new QHBoxLayout(inputPanel);
    panelLayout->setContentsMargins(12, 7, 12, 7);
    panelLayout->setSpacing(10);

    typeButton = new QPushButton(inputPanel);
    typeButton->setObjectName("TypeButton");
    typeButton->setCursor(Qt::PointingHandCursor);
    typeButton->setToolTip(AppText::switchTypeTooltip(appLanguage));
    typeButton->installEventFilter(this);
    connect(typeButton, &QPushButton::clicked, this, &InputWindow::toggleCurrentType);

    input = new QLineEdit(inputPanel);
    input->setClearButtonEnabled(true);
    input->setToolTip(AppText::inputTooltip(appLanguage));
    input->setPlaceholderText(AppText::questionPlaceholder(appLanguage));
    input->installEventFilter(this);
    connect(input, &QLineEdit::returnPressed, this, [this]() {
        const QString text = input->text().trimmed();
        if (text.isEmpty()) {
            showFeedback(AppText::emptyInputWarning(appLanguage),
                         QStringLiteral("warning"),
                         kWarningFeedbackDurationMs,
                         false);
            return;
        }
        emit memoSubmitted(activeType, text);
        input->clear();
        showFeedback(AppText::savedTo(categoryName(activeType), appLanguage),
                     QStringLiteral("success"),
                     kSuccessFeedbackDurationMs,
                     hideAfterSave);
    });

    enterHint = new QLabel(AppText::enterSave(appLanguage), inputPanel);
    enterHint->setObjectName("ShortcutHint");
    enterHint->setAttribute(Qt::WA_TransparentForMouseEvents);

    escHint = new QLabel(AppText::escHide(appLanguage), inputPanel);
    escHint->setObjectName("ShortcutHint");
    escHint->setAttribute(Qt::WA_TransparentForMouseEvents);

    feedbackLabel = new QLabel(inputPanel);
    feedbackLabel->setObjectName("FeedbackLabel");
    feedbackLabel->setAlignment(Qt::AlignCenter);
    feedbackLabel->setAttribute(Qt::WA_TransparentForMouseEvents);
    feedbackLabel->hide();

    auto *hintArea = new QWidget(inputPanel);
    hintArea->setFixedWidth(136);
    auto *hintLayout = new QHBoxLayout(hintArea);
    hintLayout->setContentsMargins(0, 0, 0, 0);
    hintLayout->setSpacing(6);
    hintLayout->addWidget(enterHint);
    hintLayout->addWidget(escHint);
    hintLayout->addWidget(feedbackLabel, 1);

    panelLayout->addWidget(typeButton);
    panelLayout->addWidget(input, 1);
    panelLayout->addWidget(hintArea, 0, Qt::AlignVCenter);
    outerLayout->addWidget(inputPanel);

    retranslateUi();
    applyTheme(ThemeMode::Light, FontSizeMode::Default, DensityMode::Comfortable);
}

void InputWindow::retranslateUi()
{
    setWindowTitle(AppText::inputWindowTitle(appLanguage));
    typeButton->setToolTip(AppText::switchTypeTooltip(appLanguage));
    input->setToolTip(AppText::inputTooltip(appLanguage));
    enterHint->setText(AppText::enterSave(appLanguage));
    escHint->setText(AppText::escHide(appLanguage));
    updateTypeButton();
}

QString InputWindow::categoryName(MemoType type) const
{
    return type == MemoType::Question ? questionCategoryName : todoCategoryName;
}

void InputWindow::updateTypeButton()
{
    typeButton->setText(QStringLiteral("%1 %2")
                            .arg(QString(QChar(0x25CF)), categoryName(activeType)));
    typeButton->setProperty("memoKind", MemoStore::typeToString(activeType));
    inputPanel->setProperty("memoKind", MemoStore::typeToString(activeType));
    input->setPlaceholderText(activeType == MemoType::Question
                                  ? AppText::questionPlaceholder(appLanguage)
                                  : AppText::todoPlaceholder(appLanguage));
    refreshDynamicStyle(typeButton);
    refreshDynamicStyle(inputPanel);
    input->update();
    if (input->isVisible()) {
        input->repaint();
    }
}

void InputWindow::showFeedback(const QString &message, const QString &kind, int durationMs, bool hideAfterDelay)
{
    if (feedbackLabel == nullptr || enterHint == nullptr || escHint == nullptr) {
        return;
    }

    feedbackTimer->stop();
    autoHideTimer->stop();

    feedbackLabel->setText(message);
    feedbackLabel->setProperty("feedbackKind", kind);
    refreshDynamicStyle(feedbackLabel);

    enterHint->hide();
    escHint->hide();
    feedbackLabel->show();
    input->setFocus();

    feedbackTimer->start(durationMs);
    if (hideAfterDelay) {
        autoHideTimer->start(kAutoHideAfterSaveDelayMs);
    }
}

void InputWindow::clearFeedback()
{
    if (feedbackTimer != nullptr) {
        feedbackTimer->stop();
    }
    if (autoHideTimer != nullptr) {
        autoHideTimer->stop();
    }

    if (feedbackLabel == nullptr || enterHint == nullptr || escHint == nullptr) {
        return;
    }

    feedbackLabel->hide();
    feedbackLabel->clear();
    feedbackLabel->setProperty("feedbackKind", QString());
    refreshDynamicStyle(feedbackLabel);
    enterHint->show();
    escHint->show();
}

void InputWindow::startShowAnimation()
{
    hidingWithAnimation = false;

    opacityAnimation->setDuration(kShowAnimationDurationMs);
    opacityAnimation->setEasingCurve(QEasingCurve::OutCubic);
    opacityAnimation->setStartValue(windowOpacity());
    opacityAnimation->setEndValue(1.0);

    positionAnimation->setDuration(kShowAnimationDurationMs);
    positionAnimation->setEasingCurve(QEasingCurve::OutCubic);
    positionAnimation->setStartValue(pos());
    positionAnimation->setEndValue(visiblePosition);

    inputAnimation->start();
}

void InputWindow::startHideAnimation()
{
    if (!isVisible()) {
        return;
    }

    if (inputAnimation->state() == QAbstractAnimation::Running) {
        stopInputAnimation();
    }

    captureOpen = false;
    hidingWithAnimation = true;
    visiblePosition = pos();

    opacityAnimation->setDuration(kHideAnimationDurationMs);
    opacityAnimation->setEasingCurve(QEasingCurve::InCubic);
    opacityAnimation->setStartValue(windowOpacity());
    opacityAnimation->setEndValue(0.0);

    positionAnimation->setDuration(kHideAnimationDurationMs);
    positionAnimation->setEasingCurve(QEasingCurve::InCubic);
    positionAnimation->setStartValue(pos());
    positionAnimation->setEndValue(visiblePosition + QPoint(0, kHideOffsetY));

    inputAnimation->start();
}

void InputWindow::stopInputAnimation()
{
    if (inputAnimation->state() == QAbstractAnimation::Running) {
        inputAnimation->stop();
    }

    hidingWithAnimation = false;
    move(visiblePosition);
    setWindowOpacity(1.0);
}
