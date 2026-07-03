#include "inputwindow.h"

#include <QCursor>
#include <QGuiApplication>
#include <QHBoxLayout>
#include <QKeyEvent>
#include <QLineEdit>
#include <QPushButton>
#include <QScreen>

InputWindow::InputWindow(QWidget *parent)
    : QWidget(parent)
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
    setFixedSize(460, 52);
    setStyleSheet(
        "InputWindow { background: #f7fbff; border: 1px solid #88a9c7; border-radius: 6px; }"
        "QLineEdit { border: 1px solid #a9bdd0; border-radius: 4px; padding: 7px; font-size: 13px; }"
        "QPushButton { border: 1px solid #88a9c7; border-radius: 4px; background: #e8f2fb; padding: 6px 10px; }"
        "QPushButton:hover { background: #d6e9f9; }");

    auto *layout = new QHBoxLayout(this);
    layout->setContentsMargins(10, 8, 10, 8);
    layout->setSpacing(8);

    typeButton = new QPushButton(this);
    connect(typeButton, &QPushButton::clicked, this, &InputWindow::toggleCurrentType);

    input = new QLineEdit(this);
    input->setPlaceholderText(QStringLiteral("输入内容，Enter 保存，Esc 收起"));
    connect(input, &QLineEdit::returnPressed, this, [this]() {
        const QString text = input->text().trimmed();
        if (text.isEmpty()) {
            return;
        }
        emit memoSubmitted(activeType, text);
        input->clear();
    });

    layout->addWidget(typeButton);
    layout->addWidget(input, 1);
    updateTypeButton();
}

void InputWindow::updateTypeButton()
{
    typeButton->setText(MemoStore::displayName(activeType));
    input->setPlaceholderText(QStringLiteral("记录到「%1」；Enter 保存，Esc 收起")
                                  .arg(MemoStore::displayName(activeType)));
}
