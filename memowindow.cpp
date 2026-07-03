#include "memowindow.h"

#include "apptheme.h"

#include <QBoxLayout>
#include <QCloseEvent>
#include <QCursor>
#include <QFrame>
#include <QHideEvent>
#include <QLabel>
#include <QMouseEvent>
#include <QPushButton>
#include <QResizeEvent>
#include <QScrollArea>
#include <QShowEvent>
#include <QSizeGrip>

MemoWindow::MemoWindow(MemoType type, QWidget *parent)
    : QWidget(parent)
    , type(type)
    , titleBar(nullptr)
    , titleLabel(nullptr)
    , topButton(nullptr)
    , listLayout(nullptr)
    , alwaysOnTop(true)
    , dragging(false)
{
    setupUi();
    updateWindowFlags(false);
}

MemoType MemoWindow::memoType() const
{
    return type;
}

MemoWindowState MemoWindow::currentState() const
{
    MemoWindowState state;
    state.geometry = geometry();
    state.visible = isVisible();
    state.alwaysOnTop = alwaysOnTop;
    return state;
}

void MemoWindow::setRecords(const QVector<MemoItem> &records)
{
    currentRecords = records;
    rebuildList();
}

void MemoWindow::restoreState(const MemoWindowState &state)
{
    alwaysOnTop = state.alwaysOnTop;
    updateWindowFlags(false);
    setGeometry(state.geometry);

    if (state.visible) {
        show();
    } else {
        hide();
    }
}

void MemoWindow::setAlwaysOnTop(bool enabled)
{
    if (alwaysOnTop == enabled) {
        return;
    }

    alwaysOnTop = enabled;
    updateWindowFlags(true);
    emitStateChanged();
}

void MemoWindow::applyTheme(ThemeMode mode)
{
    setStyleSheet(AppTheme::memoWindowStyleSheet(mode));
}

bool MemoWindow::eventFilter(QObject *object, QEvent *event)
{
    if (object == titleBar) {
        if (event->type() == QEvent::MouseButtonPress) {
            auto *mouseEvent = static_cast<QMouseEvent *>(event);
            if (mouseEvent->button() == Qt::LeftButton) {
                dragging = true;
                dragOffset = mouseEvent->globalPosition().toPoint() - frameGeometry().topLeft();
                return true;
            }
        }

        if (event->type() == QEvent::MouseMove && dragging) {
            auto *mouseEvent = static_cast<QMouseEvent *>(event);
            move(mouseEvent->globalPosition().toPoint() - dragOffset);
            return true;
        }

        if (event->type() == QEvent::MouseButtonRelease) {
            dragging = false;
            return true;
        }
    }

    const QVariant memoId = object->property("memoId");
    if (memoId.isValid() && event->type() == QEvent::MouseButtonRelease) {
        auto *mouseEvent = static_cast<QMouseEvent *>(event);
        if (mouseEvent->button() == Qt::LeftButton) {
            emit memoClicked(memoId.toString());
            return true;
        }
    }

    return QWidget::eventFilter(object, event);
}

void MemoWindow::moveEvent(QMoveEvent *event)
{
    QWidget::moveEvent(event);
    emitStateChanged();
}

void MemoWindow::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    emitStateChanged();
}

void MemoWindow::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);
    emitStateChanged();
}

void MemoWindow::hideEvent(QHideEvent *event)
{
    QWidget::hideEvent(event);
    emitStateChanged();
}

void MemoWindow::closeEvent(QCloseEvent *event)
{
    event->ignore();
    hide();
}

void MemoWindow::setupUi()
{
    setMinimumSize(240, 220);
    setWindowTitle(MemoStore::displayName(type));
    setAttribute(Qt::WA_DeleteOnClose, false);

    auto *rootLayout = new QVBoxLayout(this);
    rootLayout->setContentsMargins(10, 8, 10, 8);
    rootLayout->setSpacing(8);

    titleBar = new QWidget(this);
    titleBar->setObjectName("TitleBar");
    titleBar->installEventFilter(this);
    titleBar->setCursor(Qt::SizeAllCursor);

    auto *titleLayout = new QHBoxLayout(titleBar);
    titleLayout->setContentsMargins(0, 0, 0, 0);

    titleLabel = new QLabel(MemoStore::displayName(type), titleBar);
    titleLabel->setObjectName("TitleLabel");

    topButton = new QPushButton(titleBar);
    connect(topButton, &QPushButton::clicked, this, [this]() {
        setAlwaysOnTop(!alwaysOnTop);
    });

    auto *hideButton = new QPushButton("X", titleBar);
    hideButton->setToolTip(QStringLiteral("隐藏便签"));
    hideButton->setFixedWidth(32);
    connect(hideButton, &QPushButton::clicked, this, &MemoWindow::hide);

    titleLayout->addWidget(titleLabel);
    titleLayout->addStretch();
    titleLayout->addWidget(topButton);
    titleLayout->addWidget(hideButton);

    auto *scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);

    auto *listWidget = new QWidget(scrollArea);
    listLayout = new QVBoxLayout(listWidget);
    listLayout->setContentsMargins(0, 0, 0, 0);
    listLayout->setSpacing(7);
    scrollArea->setWidget(listWidget);

    auto *bottomBar = new QWidget(this);
    auto *bottomLayout = new QHBoxLayout(bottomBar);
    bottomLayout->setContentsMargins(0, 0, 0, 0);
    bottomLayout->addStretch();
    bottomLayout->addWidget(new QSizeGrip(bottomBar));

    rootLayout->addWidget(titleBar);
    rootLayout->addWidget(scrollArea, 1);
    rootLayout->addWidget(bottomBar);

    applyTheme(ThemeMode::Light);
}

void MemoWindow::rebuildList()
{
    while (QLayoutItem *item = listLayout->takeAt(0)) {
        delete item->widget();
        delete item;
    }

    for (const MemoItem &memo : currentRecords) {
        auto *recordFrame = new QFrame(this);
        recordFrame->setProperty("record", true);
        recordFrame->setProperty("memoId", memo.id);
        recordFrame->setCursor(Qt::PointingHandCursor);
        recordFrame->installEventFilter(this);

        auto *recordLayout = new QVBoxLayout(recordFrame);
        recordLayout->setContentsMargins(10, 8, 10, 8);
        recordLayout->setSpacing(4);

        auto *textLabel = new QLabel(memo.text, recordFrame);
        textLabel->setObjectName("RecordText");
        textLabel->setWordWrap(true);
        textLabel->setAttribute(Qt::WA_TransparentForMouseEvents);

        auto *timeLabel = new QLabel(memo.createdAt.toString("yyyy-MM-dd HH:mm"), recordFrame);
        timeLabel->setObjectName("RecordTime");
        timeLabel->setAttribute(Qt::WA_TransparentForMouseEvents);

        recordLayout->addWidget(textLabel);
        recordLayout->addWidget(timeLabel);
        listLayout->addWidget(recordFrame);
    }

    listLayout->addStretch();
}

void MemoWindow::updateWindowFlags(bool keepVisible)
{
    const bool wasVisible = isVisible();
    Qt::WindowFlags flags = Qt::Tool | Qt::FramelessWindowHint;
    if (alwaysOnTop) {
        flags |= Qt::WindowStaysOnTopHint;
    }
    setWindowFlags(flags);

    topButton->setText(alwaysOnTop ? QStringLiteral("置顶") : QStringLiteral("普通"));

    if (keepVisible && wasVisible) {
        show();
        raise();
    }
}

void MemoWindow::emitStateChanged()
{
    emit stateChanged(type, currentState());
}
