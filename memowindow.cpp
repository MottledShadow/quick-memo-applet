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
#include <QStyle>

namespace {
void refreshDynamicStyle(QWidget *widget)
{
    widget->style()->unpolish(widget);
    widget->style()->polish(widget);
    widget->update();
}
}

MemoWindow::MemoWindow(MemoType type, QWidget *parent)
    : QWidget(parent)
    , type(type)
    , panel(nullptr)
    , titleBar(nullptr)
    , titleLabel(nullptr)
    , topButton(nullptr)
    , resizeGrip(nullptr)
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
    AppTheme::applyElevation(panel, mode, ElevationLevel::E3);
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
    if (memoId.isValid() && event->type() == QEvent::MouseButtonPress) {
        auto *mouseEvent = static_cast<QMouseEvent *>(event);
        if (mouseEvent->button() == Qt::LeftButton) {
            auto *widget = qobject_cast<QWidget *>(object);
            if (widget != nullptr) {
                widget->setProperty("pressed", true);
                refreshDynamicStyle(widget);
            }
            return true;
        }
    }

    if (memoId.isValid() && event->type() == QEvent::MouseButtonRelease) {
        auto *mouseEvent = static_cast<QMouseEvent *>(event);
        auto *widget = qobject_cast<QWidget *>(object);
        if (widget != nullptr) {
            widget->setProperty("pressed", false);
            refreshDynamicStyle(widget);
        }
        if (mouseEvent->button() == Qt::LeftButton) {
            emit memoClicked(memoId.toString());
            return true;
        }
    }

    if (memoId.isValid() && event->type() == QEvent::Leave) {
        auto *widget = qobject_cast<QWidget *>(object);
        if (widget != nullptr) {
            widget->setProperty("pressed", false);
            refreshDynamicStyle(widget);
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
    updateResizeGripGeometry();
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
    setMinimumSize(256, 240);
    setWindowTitle(MemoStore::displayName(type));
    setProperty("memoKind", MemoStore::typeToString(type));
    setAttribute(Qt::WA_DeleteOnClose, false);
    setAttribute(Qt::WA_TranslucentBackground, true);
    setAutoFillBackground(false);

    auto *rootLayout = new QVBoxLayout(this);
    rootLayout->setContentsMargins(8, 8, 8, 8);
    rootLayout->setSpacing(0);

    panel = new QFrame(this);
    panel->setObjectName("MemoPanel");
    auto *panelLayout = new QVBoxLayout(panel);
    panelLayout->setContentsMargins(0, 0, 0, 0);
    panelLayout->setSpacing(0);

    titleBar = new QWidget(panel);
    titleBar->setObjectName("TitleBar");
    titleBar->installEventFilter(this);
    titleBar->setCursor(Qt::SizeAllCursor);

    auto *titleLayout = new QHBoxLayout(titleBar);
    titleLayout->setContentsMargins(8, 8, 8, 8);
    titleLayout->setSpacing(8);

    titleLabel = new QLabel(MemoStore::displayName(type), titleBar);
    titleLabel->setObjectName("TitleLabel");

    topButton = new QPushButton(titleBar);
    topButton->setObjectName("TopButton");
    topButton->setCursor(Qt::PointingHandCursor);
    connect(topButton, &QPushButton::clicked, this, [this]() {
        setAlwaysOnTop(!alwaysOnTop);
    });

    auto *hideButton = new QPushButton(QStringLiteral("×"), titleBar);
    hideButton->setObjectName("HideButton");
    hideButton->setToolTip(QStringLiteral("隐藏便签"));
    connect(hideButton, &QPushButton::clicked, this, &MemoWindow::hide);

    titleLayout->addWidget(titleLabel, 1);
    titleLayout->addWidget(topButton);
    titleLayout->addWidget(hideButton);

    auto *scrollArea = new QScrollArea(panel);
    scrollArea->setObjectName("MemoScrollArea");
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);

    auto *listWidget = new QWidget(scrollArea);
    listWidget->setObjectName("MemoList");
    listLayout = new QVBoxLayout(listWidget);
    listLayout->setContentsMargins(8, 8, 8, 16);
    listLayout->setSpacing(8);
    scrollArea->setWidget(listWidget);

    resizeGrip = new QSizeGrip(panel);
    resizeGrip->setObjectName("ResizeGrip");
    resizeGrip->setFixedSize(16, 16);

    panelLayout->addWidget(titleBar);
    panelLayout->addWidget(scrollArea, 1);
    rootLayout->addWidget(panel);

    applyTheme(ThemeMode::Light);
    updateResizeGripGeometry();
}

void MemoWindow::rebuildList()
{
    titleLabel->setText(QStringLiteral("%1 · %2")
                            .arg(MemoStore::displayName(type))
                            .arg(currentRecords.size()));

    while (QLayoutItem *item = listLayout->takeAt(0)) {
        delete item->widget();
        delete item;
    }

    if (currentRecords.isEmpty()) {
        auto *emptyFrame = new QFrame(this);
        emptyFrame->setObjectName("EmptyState");
        emptyFrame->setProperty("memoKind", MemoStore::typeToString(type));

        auto *emptyLayout = new QVBoxLayout(emptyFrame);
        emptyLayout->setContentsMargins(16, 24, 16, 24);

        auto *emptyLabel = new QLabel(QStringLiteral("暂无记录"), emptyFrame);
        emptyLabel->setObjectName("EmptyStateText");
        emptyLabel->setAlignment(Qt::AlignCenter);
        emptyLayout->addWidget(emptyLabel);

        listLayout->addWidget(emptyFrame);
        listLayout->addStretch();
        return;
    }

    for (const MemoItem &memo : currentRecords) {
        auto *recordFrame = new QFrame(this);
        recordFrame->setObjectName("MemoRecordCard");
        recordFrame->setProperty("record", true);
        recordFrame->setProperty("memoId", memo.id);
        recordFrame->setProperty("memoKind", MemoStore::typeToString(memo.type));
        recordFrame->setCursor(Qt::PointingHandCursor);
        recordFrame->setToolTip(QStringLiteral("点击删除"));
        recordFrame->installEventFilter(this);

        auto *recordLayout = new QVBoxLayout(recordFrame);
        recordLayout->setContentsMargins(12, 8, 12, 8);
        recordLayout->setSpacing(4);

        auto *textLabel = new QLabel(memo.text, recordFrame);
        textLabel->setObjectName("RecordText");
        textLabel->setWordWrap(true);
        textLabel->setMinimumWidth(0);
        textLabel->setAttribute(Qt::WA_TransparentForMouseEvents);

        auto *timeLabel = new QLabel(memo.createdAt.toString("yyyy-MM-dd HH:mm"), recordFrame);
        timeLabel->setObjectName("RecordTime");
        timeLabel->setMinimumWidth(0);
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
    topButton->setProperty("active", alwaysOnTop);
    refreshDynamicStyle(topButton);

    if (keepVisible && wasVisible) {
        show();
        raise();
    }
}

void MemoWindow::updateResizeGripGeometry()
{
    if (resizeGrip == nullptr || panel == nullptr) {
        return;
    }

    const int x = panel->width() - resizeGrip->width() - 4;
    const int y = panel->height() - resizeGrip->height() - 4;
    resizeGrip->move(x < 0 ? 0 : x, y < 0 ? 0 : y);
    resizeGrip->raise();
}

void MemoWindow::emitStateChanged()
{
    emit stateChanged(type, currentState());
}
