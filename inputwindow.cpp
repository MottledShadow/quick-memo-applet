#include "inputwindow.h"

#include "apptheme.h"

#include <QCursor>
#include <QFrame>
#include <QGuiApplication>
#include <QHideEvent>
#include <QHBoxLayout>
#include <QKeyEvent>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QScreen>
#include <QStyle>

namespace {
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
    , activeType(MemoType::Question)
    , captureOpen(false)
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
    updateTypeButton();
    emit currentTypeChanged(activeType);
}

void InputWindow::toggleCurrentType()
{
    setCurrentType(activeType == MemoType::Question ? MemoType::Todo : MemoType::Question);
}

void InputWindow::showAndFocus()
{
    if (!isVisible()) {
        const QScreen *screen = QGuiApplication::screenAt(QCursor::pos());
        if (screen == nullptr) {
            screen = QGuiApplication::primaryScreen();
        }
        if (screen != nullptr) {
            const QRect area = screen->availableGeometry();
            move(area.center() - rect().center());
        }
    }

    captureOpen = true;
    show();
    raise();
    activateWindow();
    input->setFocus();
}

void InputWindow::applyTheme(ThemeMode mode)
{
    setStyleSheet(AppTheme::inputWindowStyleSheet(mode));
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
    captureOpen = false;
    QWidget::hideEvent(event);
}

void InputWindow::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Escape) {
        hide();
        event->accept();
        return;
    }

    QWidget::keyPressEvent(event);
}

void InputWindow::setupUi()
{
    setWindowTitle(QStringLiteral("快速记录"));
    setWindowFlags(Qt::Tool | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    setAttribute(Qt::WA_TranslucentBackground, true);
    setAutoFillBackground(false);
    setFixedSize(560, 64);

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
    typeButton->setToolTip(QStringLiteral("切换记录类型"));
    typeButton->installEventFilter(this);
    connect(typeButton, &QPushButton::clicked, this, &InputWindow::toggleCurrentType);

    input = new QLineEdit(inputPanel);
    input->setClearButtonEnabled(true);
    input->setToolTip(QStringLiteral("输入后按 Enter 保存"));
    input->setPlaceholderText(QStringLiteral("记下一个问题..."));
    input->installEventFilter(this);
    connect(input, &QLineEdit::returnPressed, this, [this]() {
        const QString text = input->text().trimmed();
        if (text.isEmpty()) {
            return;
        }
        emit memoSubmitted(activeType, text);
        input->clear();
    });

    enterHint = new QLabel(QStringLiteral("Enter 保存"), inputPanel);
    enterHint->setObjectName("ShortcutHint");
    enterHint->setAttribute(Qt::WA_TransparentForMouseEvents);

    escHint = new QLabel(QStringLiteral("Esc 收起"), inputPanel);
    escHint->setObjectName("ShortcutHint");
    escHint->setAttribute(Qt::WA_TransparentForMouseEvents);

    panelLayout->addWidget(typeButton);
    panelLayout->addWidget(input, 1);
    panelLayout->addWidget(enterHint, 0, Qt::AlignVCenter);
    panelLayout->addWidget(escHint, 0, Qt::AlignVCenter);
    outerLayout->addWidget(inputPanel);

    updateTypeButton();
    applyTheme(ThemeMode::Light);
}

void InputWindow::updateTypeButton()
{
    typeButton->setText(QStringLiteral("%1 %2")
                            .arg(QString(QChar(0x25CF)), MemoStore::displayName(activeType)));
    typeButton->setProperty("memoKind", MemoStore::typeToString(activeType));
    inputPanel->setProperty("memoKind", MemoStore::typeToString(activeType));
    input->setPlaceholderText(activeType == MemoType::Question
                                  ? QStringLiteral("记下一个问题...")
                                  : QStringLiteral("写下一件待办..."));
    refreshDynamicStyle(typeButton);
    refreshDynamicStyle(inputPanel);
    input->update();
    if (input->isVisible()) {
        input->repaint();
    }
}
