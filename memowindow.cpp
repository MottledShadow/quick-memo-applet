#include "memowindow.h"

#include "apptheme.h"

#include <QBoxLayout>
#include <QColor>
#include <QCloseEvent>
#include <QCursor>
#include <QEasingCurve>
#include <QFrame>
#include <QHideEvent>
#include <QLabel>
#include <QMouseEvent>
#include <QPaintEvent>
#include <QPainter>
#include <QParallelAnimationGroup>
#include <QPushButton>
#include <QPropertyAnimation>
#include <QResizeEvent>
#include <QScrollArea>
#include <QSequentialAnimationGroup>
#include <QShowEvent>
#include <QSizeGrip>
#include <QStyle>
#include <QStyleOption>
#include <QVariant>
#include <QVariantAnimation>

namespace {
constexpr int kPaperLineSpacing = 32;
constexpr int kPaperContentTop = 18;
constexpr int kPaperTextLineOffset = 25;
constexpr int kStrikeAnimationDurationMs = 130;
constexpr int kCollapseAnimationDurationMs = 120;
constexpr int kStrikeLineHeight = 2;

void refreshDynamicStyle(QWidget *widget);

class LinedMemoPaper : public QFrame
{
public:
    explicit LinedMemoPaper(QWidget *parent = nullptr)
        : QFrame(parent)
    {
        setAttribute(Qt::WA_StyledBackground, true);
    }

protected:
    void paintEvent(QPaintEvent *event) override
    {
        Q_UNUSED(event)

        QStyleOption option;
        option.initFrom(this);

        QPainter painter(this);
        style()->drawPrimitive(QStyle::PE_Widget, &option, &painter, this);
        drawPaperDetails(&painter);
        drawAccentTape(&painter);
        drawRuleLines(&painter);
    }

private:
    QColor accentColor() const
    {
        const QVariant value = property("accentColor");
        if (value.canConvert<QColor>()) {
            QColor color = value.value<QColor>();
            if (color.isValid()) {
                return color;
            }
        }

        return QColor(37, 99, 235);
    }

    QColor ruleLineColor() const
    {
        const QVariant value = property("paperLineColor");
        if (value.canConvert<QColor>()) {
            QColor color = value.value<QColor>();
            if (color.isValid()) {
                color.setAlphaF(0.58);
                return color;
            }
        }

        return QColor(227, 214, 155, 148);
    }

    void drawPaperDetails(QPainter *painter) const
    {
        if (width() <= 4 || height() <= 4) {
            return;
        }

        painter->save();
        painter->setRenderHint(QPainter::Antialiasing, true);

        QPen innerHighlight(QColor(255, 255, 255, 118));
        innerHighlight.setWidthF(1.0);
        painter->setPen(innerHighlight);
        painter->setBrush(Qt::NoBrush);
        painter->drawRoundedRect(QRectF(1.5, 1.5, width() - 3.0, height() - 3.0), 7.0, 7.0);

        QPen warmEdge(QColor(141, 112, 38, 36));
        warmEdge.setWidthF(1.0);
        painter->setPen(warmEdge);
        painter->drawLine(QPointF(6.0, height() - 2.5), QPointF(width() - 8.0, height() - 2.5));
        painter->drawLine(QPointF(width() - 2.5, 10.0), QPointF(width() - 2.5, height() - 10.0));

        painter->restore();
    }

    void drawAccentTape(QPainter *painter) const
    {
        QColor fill = accentColor();
        fill.setAlphaF(0.22);

        QColor edge = fill;
        edge.setAlphaF(0.30);

        painter->save();
        painter->setRenderHint(QPainter::Antialiasing, true);
        painter->translate(20.0, 7.0);
        painter->rotate(-4.0);
        painter->setPen(QPen(edge, 1.0));
        painter->setBrush(fill);
        painter->drawRoundedRect(QRectF(0.0, 0.0, 52.0, 9.0), 3.0, 3.0);
        painter->restore();
    }

    void drawRuleLines(QPainter *painter) const
    {
        const QWidget *header = findChild<QWidget *>(QStringLiteral("TitleBar"));
        if (header == nullptr) {
            return;
        }

        const int lineLeft = 24;
        const int lineRight = width() - 24;
        const int firstLineY = header->geometry().bottom() + kPaperContentTop + kPaperTextLineOffset;
        const int lineStep = kPaperLineSpacing;
        const int maxY = height() - 28;

        if (lineRight <= lineLeft || maxY <= firstLineY) {
            return;
        }

        QPen pen(ruleLineColor());
        pen.setWidthF(1.0);
        pen.setCapStyle(Qt::FlatCap);
        pen.setStyle(Qt::DashLine);

        painter->save();
        painter->setRenderHint(QPainter::Antialiasing, false);
        painter->setPen(pen);

        for (int y = firstLineY; y <= maxY; y += lineStep) {
            const qreal crispY = y + 0.5;
            painter->drawLine(QPointF(lineLeft, crispY), QPointF(lineRight, crispY));
        }

        painter->restore();
    }
};

class MemoRecordCard : public QFrame
{
public:
    explicit MemoRecordCard(QWidget *parent = nullptr)
        : QFrame(parent)
        , textLabel(nullptr)
        , marker(nullptr)
        , strikeLine(new QFrame(this))
        , strikeProgressValue(0.0)
    {
        setAttribute(Qt::WA_StyledBackground, true);
        strikeLine->setObjectName("StrikeLine");
        strikeLine->setAttribute(Qt::WA_TransparentForMouseEvents);
        strikeLine->hide();
    }

    void setTextLabel(QLabel *label)
    {
        textLabel = label;
        updateStrikeGeometry();
    }

    void setMarker(QFrame *markerFrame)
    {
        marker = markerFrame;
    }

    void setMemoKind(const QString &memoKind)
    {
        setProperty("memoKind", memoKind);
        strikeLine->setProperty("memoKind", memoKind);
        refreshDynamicStyle(strikeLine);
    }

    void setStrikeProgress(qreal progress)
    {
        if (progress < 0.0) {
            progress = 0.0;
        } else if (progress > 1.0) {
            progress = 1.0;
        }

        if (qFuzzyCompare(strikeProgressValue, progress)) {
            return;
        }

        strikeProgressValue = progress;
        updateStrikeGeometry();
    }

    void setDeleting(bool deleting)
    {
        setProperty("deleting", deleting);
        refreshDynamicStyle(this);

        if (marker != nullptr) {
            marker->setProperty("deleting", deleting);
            refreshDynamicStyle(marker);
        }
    }

protected:
    void resizeEvent(QResizeEvent *event) override
    {
        QFrame::resizeEvent(event);
        updateStrikeGeometry();
    }

private:
    void updateStrikeGeometry()
    {
        if (textLabel == nullptr || strikeLine == nullptr) {
            return;
        }

        const QRect textGeometry = textLabel->geometry();
        const int lineWidth = static_cast<int>((textGeometry.width() * strikeProgressValue) + 0.5);
        const int lineY = textGeometry.y() + (textGeometry.height() / 2) - (kStrikeLineHeight / 2);

        strikeLine->setGeometry(textGeometry.x(), lineY, lineWidth, kStrikeLineHeight);
        strikeLine->setVisible(lineWidth > 0);
        strikeLine->raise();
    }

    QLabel *textLabel;
    QFrame *marker;
    QFrame *strikeLine;
    qreal strikeProgressValue;
};

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
    , titleCountLabel(nullptr)
    , topButton(nullptr)
    , resizeGrip(nullptr)
    , listLayout(nullptr)
    , deletingMemoId()
    , alwaysOnTop(true)
    , dragging(false)
    , deleteAnimationActive(false)
    , rebuildPending(false)
{
    setProperty("memoKind", MemoStore::typeToString(type));
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
    if (deleteAnimationActive) {
        pendingRecords = records;
        rebuildPending = true;
        return;
    }

    rebuildPending = false;
    pendingRecords.clear();
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
    panel->setProperty("accentColor", QVariant::fromValue(AppTheme::memoAccentColor(type, mode)));
    panel->setProperty("paperLineColor", QVariant::fromValue(AppTheme::memoPaperLineColor(mode)));
    panel->update();
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
            if (deleteAnimationActive) {
                return true;
            }

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
            if (!deleteAnimationActive && widget != nullptr) {
                startDeleteAnimation(widget, memoId.toString());
            }
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

    panel = new LinedMemoPaper(this);
    panel->setObjectName("MemoPanel");
    auto *panelLayout = new QVBoxLayout(panel);
    panelLayout->setContentsMargins(0, 0, 0, 0);
    panelLayout->setSpacing(0);

    titleBar = new QWidget(panel);
    titleBar->setObjectName("TitleBar");
    titleBar->installEventFilter(this);
    titleBar->setCursor(Qt::SizeAllCursor);

    auto *titleLayout = new QHBoxLayout(titleBar);
    titleLayout->setContentsMargins(18, 14, 12, 4);
    titleLayout->setSpacing(8);

    titleLabel = new QLabel(MemoStore::displayName(type), titleBar);
    titleLabel->setObjectName("TitleLabel");

    titleCountLabel = new QLabel(QStringLiteral("0"), titleBar);
    titleCountLabel->setObjectName("TitleCount");
    titleCountLabel->setAlignment(Qt::AlignCenter);

    topButton = new QPushButton(titleBar);
    topButton->setObjectName("TopButton");
    topButton->setCursor(Qt::PointingHandCursor);
    topButton->setFixedSize(24, 24);
    connect(topButton, &QPushButton::clicked, this, [this]() {
        setAlwaysOnTop(!alwaysOnTop);
    });

    auto *hideButton = new QPushButton(QStringLiteral("×"), titleBar);
    hideButton->setObjectName("HideButton");
    hideButton->setToolTip(QStringLiteral("隐藏便签"));
    hideButton->setFixedSize(24, 24);
    connect(hideButton, &QPushButton::clicked, this, &MemoWindow::hide);

    titleLayout->addWidget(titleLabel);
    titleLayout->addWidget(titleCountLabel, 0, Qt::AlignVCenter);
    titleLayout->addStretch(1);
    titleLayout->addWidget(topButton);
    titleLayout->addWidget(hideButton);

    auto *scrollArea = new QScrollArea(panel);
    scrollArea->setObjectName("MemoScrollArea");
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);
    scrollArea->setAutoFillBackground(false);
    scrollArea->setAttribute(Qt::WA_TranslucentBackground);
    scrollArea->viewport()->setAutoFillBackground(false);
    scrollArea->viewport()->setAttribute(Qt::WA_TranslucentBackground);

    auto *listWidget = new QWidget(scrollArea);
    listWidget->setObjectName("MemoList");
    listWidget->setAutoFillBackground(false);
    listWidget->setAttribute(Qt::WA_TranslucentBackground);
    listLayout = new QVBoxLayout(listWidget);
    listLayout->setContentsMargins(24, kPaperContentTop, 24, 20);
    listLayout->setSpacing(0);
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
    titleLabel->setText(MemoStore::displayName(type));
    titleCountLabel->setText(QString::number(currentRecords.size()));

    while (QLayoutItem *item = listLayout->takeAt(0)) {
        delete item->widget();
        delete item;
    }

    if (currentRecords.isEmpty()) {
        listLayout->addStretch();
        return;
    }

    for (const MemoItem &memo : currentRecords) {
        auto *recordFrame = new MemoRecordCard(listLayout->parentWidget());
        recordFrame->setObjectName("MemoRecordCard");
        recordFrame->setProperty("record", true);
        recordFrame->setProperty("memoId", memo.id);
        recordFrame->setMemoKind(MemoStore::typeToString(memo.type));
        recordFrame->setProperty("deleting", false);
        recordFrame->setCursor(Qt::PointingHandCursor);
        recordFrame->setToolTip(QStringLiteral("点击删除"));
        recordFrame->installEventFilter(this);
        recordFrame->setFixedHeight(kPaperLineSpacing);

        auto *recordLayout = new QHBoxLayout(recordFrame);
        recordLayout->setContentsMargins(0, 0, 0, 0);
        recordLayout->setSpacing(8);

        auto *marker = new QFrame(recordFrame);
        marker->setObjectName("RecordMarker");
        marker->setProperty("memoKind", MemoStore::typeToString(memo.type));
        marker->setProperty("deleting", false);
        marker->setFixedSize(8, 8);
        marker->setAttribute(Qt::WA_TransparentForMouseEvents);
        recordFrame->setMarker(marker);
        recordLayout->addWidget(marker, 0, Qt::AlignVCenter);

        auto *textLabel = new QLabel(memo.text, recordFrame);
        textLabel->setObjectName("RecordText");
        textLabel->setWordWrap(true);
        textLabel->setMinimumWidth(0);
        textLabel->setFixedHeight(kPaperLineSpacing);
        textLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        textLabel->setAttribute(Qt::WA_TransparentForMouseEvents);

        recordLayout->addWidget(textLabel);
        recordFrame->setTextLabel(textLabel);
        listLayout->addWidget(recordFrame);
    }

    listLayout->addStretch();
}

void MemoWindow::startDeleteAnimation(QWidget *recordWidget, const QString &id)
{
    if (deleteAnimationActive || recordWidget == nullptr || id.isEmpty()) {
        return;
    }

    auto *record = static_cast<MemoRecordCard *>(recordWidget);
    deleteAnimationActive = true;
    rebuildPending = false;
    pendingRecords.clear();
    deletingMemoId = id;

    record->setProperty("pressed", false);
    record->setDeleting(true);
    record->setStrikeProgress(0.0);

    auto *strikeAnimation = new QVariantAnimation();
    strikeAnimation->setDuration(kStrikeAnimationDurationMs);
    strikeAnimation->setEasingCurve(QEasingCurve::OutCubic);
    strikeAnimation->setStartValue(0.0);
    strikeAnimation->setEndValue(1.0);
    connect(strikeAnimation, &QVariantAnimation::valueChanged, record, [record](const QVariant &value) {
        record->setStrikeProgress(value.toReal());
    });

    auto *collapseGroup = new QParallelAnimationGroup();
    auto *minHeightAnimation = new QPropertyAnimation(record, "minimumHeight");
    minHeightAnimation->setDuration(kCollapseAnimationDurationMs);
    minHeightAnimation->setEasingCurve(QEasingCurve::InCubic);
    minHeightAnimation->setStartValue(kPaperLineSpacing);
    minHeightAnimation->setEndValue(0);

    auto *maxHeightAnimation = new QPropertyAnimation(record, "maximumHeight");
    maxHeightAnimation->setDuration(kCollapseAnimationDurationMs);
    maxHeightAnimation->setEasingCurve(QEasingCurve::InCubic);
    maxHeightAnimation->setStartValue(kPaperLineSpacing);
    maxHeightAnimation->setEndValue(0);

    collapseGroup->addAnimation(minHeightAnimation);
    collapseGroup->addAnimation(maxHeightAnimation);

    auto *deleteAnimation = new QSequentialAnimationGroup(this);
    deleteAnimation->addAnimation(strikeAnimation);
    deleteAnimation->addAnimation(collapseGroup);
    connect(deleteAnimation, &QSequentialAnimationGroup::finished, this, [this, deleteAnimation]() {
        const QString id = deletingMemoId;
        deleteAnimationActive = false;
        deletingMemoId.clear();
        deleteAnimation->deleteLater();

        emit memoClicked(id);

        if (rebuildPending) {
            currentRecords = pendingRecords;
            pendingRecords.clear();
            rebuildPending = false;
            rebuildList();
        }
    });

    deleteAnimation->start();
}

void MemoWindow::updateWindowFlags(bool keepVisible)
{
    const bool wasVisible = isVisible();
    Qt::WindowFlags flags = Qt::Tool | Qt::FramelessWindowHint;
    if (alwaysOnTop) {
        flags |= Qt::WindowStaysOnTopHint;
    }
    setWindowFlags(flags);

    topButton->setText(QString(QChar(alwaysOnTop ? 0x25CF : 0x25CB)));
    topButton->setToolTip(alwaysOnTop ? QStringLiteral("取消置顶")
                                      : QStringLiteral("保持置顶"));
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
