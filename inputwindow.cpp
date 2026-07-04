#include "inputwindow.h"

#include "apptheme.h"

#include <QCursor>
#include <QFrame>
#include <QGuiApplication>
#include <QHBoxLayout>
#include <QKeyEvent>
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
}
}

InputWindow::InputWindow(QWidget *parent)
    : QWidget(parent)
    , inputPanel(nullptr)
    , input(nullptr)
    , typeButton(nullptr)
    , activeType(MemoType::Question)
{
    setupUi();
}

MemoType InputWindow::currentType() const
{
    return activeType;
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

    show();
    raise();
    activateWindow();
    input->setFocus();
}

void InputWindow::applyTheme(ThemeMode mode)
{
    setStyleSheet(AppTheme::inputWindowStyleSheet(mode));
    AppTheme::applyElevation(inputPanel, mode, ElevationLevel::E3);
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
    setFixedSize(480, 72);

    auto *outerLayout = new QHBoxLayout(this);
    outerLayout->setContentsMargins(12, 12, 12, 12);
    outerLayout->setSpacing(0);

    inputPanel = new QFrame(this);
    inputPanel->setObjectName("InputPanel");

    auto *panelLayout = new QHBoxLayout(inputPanel);
    panelLayout->setContentsMargins(12, 8, 12, 8);
    panelLayout->setSpacing(8);

    typeButton = new QPushButton(inputPanel);
    typeButton->setObjectName("TypeButton");
    typeButton->setCursor(Qt::PointingHandCursor);
    typeButton->setToolTip(QStringLiteral("切换记录类型"));
    connect(typeButton, &QPushButton::clicked, this, &InputWindow::toggleCurrentType);

    input = new QLineEdit(inputPanel);
    input->setClearButtonEnabled(true);
    input->setToolTip(QStringLiteral("输入后按 Enter 保存"));
    input->setPlaceholderText(QStringLiteral("输入内容，Enter 保存，Esc 收起"));
    connect(input, &QLineEdit::returnPressed, this, [this]() {
        const QString text = input->text().trimmed();
        if (text.isEmpty()) {
            return;
        }
        emit memoSubmitted(activeType, text);
        input->clear();
    });

    panelLayout->addWidget(typeButton);
    panelLayout->addWidget(input, 1);
    outerLayout->addWidget(inputPanel);

    updateTypeButton();
    applyTheme(ThemeMode::Light);
}

void InputWindow::updateTypeButton()
{
    typeButton->setText(MemoStore::displayName(activeType));
    typeButton->setProperty("memoKind", MemoStore::typeToString(activeType));
    refreshDynamicStyle(typeButton);
    input->setPlaceholderText(QStringLiteral("记录到「%1」；Enter 保存，Esc 收起")
                                  .arg(MemoStore::displayName(activeType)));
}
