#include "dashboardwindow.h"

#include "apptheme.h"

#include <QAbstractItemView>
#include <QBoxLayout>
#include <QCheckBox>
#include <QCloseEvent>
#include <QComboBox>
#include <QFrame>
#include <QHBoxLayout>
#include <QKeySequenceEdit>
#include <QLabel>
#include <QLayout>
#include <QMouseEvent>
#include <QPushButton>
#include <QScrollArea>
#include <QScrollBar>
#include <QSignalBlocker>
#include <QSizePolicy>
#include <QStyle>
#include <QVBoxLayout>
#include <QWheelEvent>

namespace {
void refreshDynamicStyle(QWidget *widget)
{
    widget->style()->unpolish(widget);
    widget->style()->polish(widget);
    widget->update();
}

class NoWheelComboBox : public QComboBox
{
public:
    explicit NoWheelComboBox(QWidget *parent = nullptr)
        : QComboBox(parent)
    {
    }

protected:
    void wheelEvent(QWheelEvent *event) override
    {
        if (view() != nullptr && view()->isVisible()) {
            QComboBox::wheelEvent(event);
            return;
        }

        event->ignore();
    }
};
}

DashboardWindow::DashboardWindow(MemoStore *store, QWidget *parent)
    : QWidget(parent)
    , store(store)
    , questionVisibilityButton(nullptr)
    , todoVisibilityButton(nullptr)
    , questionTopCheck(nullptr)
    , todoTopCheck(nullptr)
    , autostartCheck(nullptr)
    , hideInputAfterSaveCheck(nullptr)
    , themeCombo(nullptr)
    , hotkeyEdit(nullptr)
    , questionCountLabel(nullptr)
    , todoCountLabel(nullptr)
    , questionRecordsLayout(nullptr)
    , todoRecordsLayout(nullptr)
    , statusLabel(nullptr)
    , recordsPanel(nullptr)
    , sidePanel(nullptr)
{
    setupUi();
    connect(store, &MemoStore::recordsChanged, this, &DashboardWindow::refresh);
    connect(store, &MemoStore::settingsChanged, this, &DashboardWindow::refresh);
    connect(store, &MemoStore::windowStateChanged, this, &DashboardWindow::refresh);
    refresh();
}

void DashboardWindow::refresh()
{
    refreshMemoControls(MemoType::Question);
    refreshMemoControls(MemoType::Todo);

    {
        const QSignalBlocker blocker(autostartCheck);
        autostartCheck->setChecked(store->autostartEnabled());
    }

    {
        const QSignalBlocker blocker(hideInputAfterSaveCheck);
        hideInputAfterSaveCheck->setChecked(store->hideInputAfterSave());
    }

    {
        const QSignalBlocker blocker(themeCombo);
        themeCombo->setCurrentIndex(themeCombo->findData(static_cast<int>(store->themeMode())));
    }

    {
        const QSignalBlocker blocker(hotkeyEdit);
        hotkeyEdit->setKeySequence(QKeySequence(store->hotkey()));
    }

    refreshRecords();
}

void DashboardWindow::setStatusMessage(const QString &message)
{
    statusLabel->setText(message);
}

void DashboardWindow::applyTheme(ThemeMode mode)
{
    setStyleSheet(AppTheme::dashboardStyleSheet(mode));
    AppTheme::applyElevation(recordsPanel, mode, ElevationLevel::E2);
    AppTheme::applyElevation(sidePanel, mode, ElevationLevel::E2);
}

bool DashboardWindow::eventFilter(QObject *object, QEvent *event)
{
    const QVariant memoId = object->property("memoId");
    if (!memoId.isValid()) {
        return QWidget::eventFilter(object, event);
    }

    auto *widget = qobject_cast<QWidget *>(object);
    if (widget == nullptr) {
        return QWidget::eventFilter(object, event);
    }

    if (event->type() == QEvent::MouseButtonPress) {
        auto *mouseEvent = static_cast<QMouseEvent *>(event);
        if (mouseEvent->button() == Qt::LeftButton) {
            widget->setProperty("pressed", true);
            refreshDynamicStyle(widget);
            return true;
        }
    }

    if (event->type() == QEvent::MouseButtonRelease) {
        auto *mouseEvent = static_cast<QMouseEvent *>(event);
        widget->setProperty("pressed", false);
        refreshDynamicStyle(widget);

        if (mouseEvent->button() == Qt::LeftButton && widget->rect().contains(mouseEvent->pos())) {
            const MemoType type = MemoStore::typeFromString(widget->property("memoKind").toString());
            emit recordDeleteRequested(memoId.toString(), type);
            setStatusMessage(QStringLiteral("已删除一条%1。").arg(MemoStore::displayName(type)));
        }
        return true;
    }

    if (event->type() == QEvent::Leave) {
        widget->setProperty("pressed", false);
        refreshDynamicStyle(widget);
    }

    return QWidget::eventFilter(object, event);
}

void DashboardWindow::closeEvent(QCloseEvent *event)
{
    event->ignore();
    hide();
}

void DashboardWindow::setupUi()
{
    setWindowTitle(QStringLiteral("Quick Memo 后台"));
    resize(984, 640);
    setMinimumSize(864, 520);

    auto *rootLayout = new QVBoxLayout(this);
    rootLayout->setContentsMargins(16, 16, 16, 16);
    rootLayout->setSpacing(8);

    auto *content = new QWidget(this);
    auto *contentLayout = new QHBoxLayout(content);
    contentLayout->setContentsMargins(8, 8, 8, 8);
    contentLayout->setSpacing(16);

    recordsPanel = new QFrame(content);
    recordsPanel->setObjectName("RecordsPanel");
    auto *recordsPanelLayout = new QVBoxLayout(recordsPanel);
    recordsPanelLayout->setContentsMargins(16, 16, 16, 16);
    recordsPanelLayout->setSpacing(16);

    auto *recordsTitleRow = new QWidget(recordsPanel);
    auto *recordsTitleLayout = new QHBoxLayout(recordsTitleRow);
    recordsTitleLayout->setContentsMargins(0, 0, 0, 0);

    auto *recordsTitle = new QLabel(QStringLiteral("全部记录"), recordsTitleRow);
    recordsTitle->setObjectName("PageTitle");
    recordsTitleLayout->addWidget(recordsTitle);
    recordsTitleLayout->addStretch();

    auto *columns = new QWidget(recordsPanel);
    auto *columnsLayout = new QHBoxLayout(columns);
    columnsLayout->setContentsMargins(0, 0, 0, 0);
    columnsLayout->setSpacing(16);
    columnsLayout->addWidget(createRecordsColumn(MemoType::Question, columns), 1);
    columnsLayout->addWidget(createRecordsColumn(MemoType::Todo, columns), 1);

    recordsPanelLayout->addWidget(recordsTitleRow);
    recordsPanelLayout->addWidget(columns, 1);

    sidePanel = new QFrame(content);
    sidePanel->setObjectName("SidePanel");
    sidePanel->setFixedWidth(320);
    auto *sidePanelLayout = new QVBoxLayout(sidePanel);
    sidePanelLayout->setContentsMargins(0, 0, 0, 0);
    sidePanelLayout->setSpacing(0);

    auto *sideScrollArea = new QScrollArea(sidePanel);
    sideScrollArea->setObjectName("SideContentScrollArea");
    sideScrollArea->setWidgetResizable(true);
    sideScrollArea->setFrameShape(QFrame::NoFrame);
    configureScrollArea(sideScrollArea);

    auto *sideContent = new QWidget(sideScrollArea);
    sideContent->setObjectName("SideContent");

    auto *sideLayout = new QVBoxLayout(sideContent);
    sideLayout->setContentsMargins(16, 16, 16, 16);
    sideLayout->setSpacing(16);
    sideLayout->setSizeConstraint(QLayout::SetMinimumSize);

    auto *memoSectionTitle = new QLabel(QStringLiteral("便签窗口"), sideContent);
    memoSectionTitle->setObjectName("SideSectionTitle");
    sideLayout->addWidget(memoSectionTitle);
    sideLayout->addWidget(createMemoControls(MemoType::Question, sideContent));
    sideLayout->addWidget(createMemoControls(MemoType::Todo, sideContent));

    auto *settingsSectionTitle = new QLabel(QStringLiteral("设置"), sideContent);
    settingsSectionTitle->setObjectName("SideSectionTitle");
    sideLayout->addSpacing(8);
    sideLayout->addWidget(settingsSectionTitle);

    QBoxLayout *hotkeyGroupLayout = nullptr;
    auto *hotkeyGroup = createSettingsGroup(QStringLiteral("快捷键"), sideContent, &hotkeyGroupLayout);
    auto *hotkeyLabel = new QLabel(QStringLiteral("全局快捷键"), hotkeyGroup);
    hotkeyLabel->setObjectName("FieldLabel");
    hotkeyEdit = new QKeySequenceEdit(hotkeyGroup);
    hotkeyEdit->setMinimumHeight(38);

    auto *applyHotkeyButton = new QPushButton(QStringLiteral("应用"), hotkeyGroup);
    applyHotkeyButton->setObjectName("PrimaryButton");
    applyHotkeyButton->setMinimumSize(76, 34);
    applyHotkeyButton->setToolTip(QStringLiteral("应用新的全局快捷键"));
    connect(applyHotkeyButton, &QPushButton::clicked, this, [this]() {
        emit hotkeyChangeRequested(hotkeyEdit->keySequence());
    });

    auto *hotkeyActionRow = new QWidget(hotkeyGroup);
    auto *hotkeyActionLayout = new QHBoxLayout(hotkeyActionRow);
    hotkeyActionLayout->setContentsMargins(0, 0, 0, 0);
    hotkeyActionLayout->addStretch(1);
    hotkeyActionLayout->addWidget(applyHotkeyButton);

    hotkeyGroupLayout->addWidget(hotkeyLabel);
    hotkeyGroupLayout->addWidget(hotkeyEdit);
    hotkeyGroupLayout->addWidget(hotkeyActionRow);
    hotkeyGroup->setFixedHeight(156);

    QBoxLayout *appearanceGroupLayout = nullptr;
    auto *appearanceGroup = createSettingsGroup(QStringLiteral("外观"), sideContent, &appearanceGroupLayout);
    auto *themeLabel = new QLabel(QStringLiteral("主题"), appearanceGroup);
    themeLabel->setObjectName("FieldLabel");
    themeCombo = new NoWheelComboBox(appearanceGroup);
    themeCombo->setObjectName("ThemeCombo");
    themeCombo->setCursor(Qt::PointingHandCursor);
    themeCombo->setMinimumHeight(38);
    themeCombo->setToolTip(QStringLiteral("切换亮色或暗色主题"));
    themeCombo->addItem(MemoStore::themeDisplayName(ThemeMode::Light), static_cast<int>(ThemeMode::Light));
    themeCombo->addItem(MemoStore::themeDisplayName(ThemeMode::Dark), static_cast<int>(ThemeMode::Dark));
    connect(themeCombo, &QComboBox::currentIndexChanged, this, [this](int index) {
        const QVariant data = themeCombo->itemData(index);
        if (!data.isValid()) {
            return;
        }
        emit themeChangeRequested(static_cast<ThemeMode>(data.toInt()));
    });
    appearanceGroupLayout->addWidget(themeLabel);
    appearanceGroupLayout->addWidget(themeCombo);
    appearanceGroup->setFixedHeight(124);

    QBoxLayout *inputGroupLayout = nullptr;
    auto *inputGroup = createSettingsGroup(QStringLiteral("输入"), sideContent, &inputGroupLayout);
    hideInputAfterSaveCheck = new QCheckBox(QStringLiteral("保存后自动收起输入框"), inputGroup);
    hideInputAfterSaveCheck->setObjectName("InputToggle");
    hideInputAfterSaveCheck->setMinimumHeight(28);
    hideInputAfterSaveCheck->setToolTip(QStringLiteral("保存成功后短暂显示反馈并自动收起输入框"));
    connect(hideInputAfterSaveCheck, &QCheckBox::toggled, this, &DashboardWindow::inputAutoHideChanged);
    inputGroupLayout->addWidget(hideInputAfterSaveCheck);
    inputGroup->setFixedHeight(112);

    QBoxLayout *systemGroupLayout = nullptr;
    auto *systemGroup = createSettingsGroup(QStringLiteral("系统"), sideContent, &systemGroupLayout);
    autostartCheck = new QCheckBox(QStringLiteral("开机自启"), systemGroup);
    autostartCheck->setObjectName("SystemToggle");
    autostartCheck->setMinimumHeight(28);
    autostartCheck->setToolTip(QStringLiteral("开机后自动启动 Quick Memo"));
    connect(autostartCheck, &QCheckBox::toggled, this, &DashboardWindow::autostartChanged);
    systemGroupLayout->addWidget(autostartCheck);
    systemGroup->setFixedHeight(112);

    auto *exitButton = new QPushButton(QStringLiteral("退出程序"), sideContent);
    exitButton->setObjectName("DangerButton");
    exitButton->setToolTip(QStringLiteral("保存状态并退出程序"));
    connect(exitButton, &QPushButton::clicked, this, &DashboardWindow::exitRequested);

    sideLayout->addWidget(hotkeyGroup);
    sideLayout->addWidget(appearanceGroup);
    sideLayout->addWidget(inputGroup);
    sideLayout->addWidget(systemGroup);
    sideLayout->addStretch(1);
    sideLayout->addWidget(exitButton);

    sideScrollArea->setWidget(sideContent);
    sidePanelLayout->addWidget(sideScrollArea);

    contentLayout->addWidget(recordsPanel, 1);
    contentLayout->addWidget(sidePanel);

    auto *statusBar = new QFrame(this);
    statusBar->setObjectName("StatusBar");
    auto *statusLayout = new QHBoxLayout(statusBar);
    statusLayout->setContentsMargins(16, 0, 16, 0);
    statusLabel = new QLabel(QStringLiteral("就绪"), statusBar);
    statusLabel->setObjectName("StatusLabel");
    statusLayout->addWidget(statusLabel, 1);

    rootLayout->addWidget(content, 1);
    rootLayout->addWidget(statusBar);

    applyTheme(ThemeMode::Light);
}

QWidget *DashboardWindow::createRecordsColumn(MemoType type, QWidget *parent)
{
    auto *column = new QFrame(parent);
    column->setObjectName("RecordColumn");
    column->setProperty("memoKind", MemoStore::typeToString(type));

    auto *layout = new QVBoxLayout(column);
    layout->setContentsMargins(12, 16, 12, 16);
    layout->setSpacing(8);

    auto *header = new QWidget(column);
    auto *headerLayout = new QHBoxLayout(header);
    headerLayout->setContentsMargins(0, 0, 0, 0);

    auto *titleLabel = new QLabel(MemoStore::displayName(type), header);
    titleLabel->setObjectName("RecordColumnTitle");

    auto *countLabel = new QLabel(QStringLiteral("0"), header);
    countLabel->setObjectName("CountBadge");
    countLabel->setProperty("memoKind", MemoStore::typeToString(type));
    countLabel->setAlignment(Qt::AlignCenter);

    headerLayout->addWidget(titleLabel);
    headerLayout->addWidget(countLabel);
    headerLayout->addStretch();

    auto *scrollArea = new QScrollArea(column);
    scrollArea->setObjectName("RecordsScrollArea");
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);
    configureScrollArea(scrollArea);

    auto *listWidget = new QWidget(scrollArea);
    listWidget->setObjectName("RecordsList");
    auto *listLayout = new QVBoxLayout(listWidget);
    listLayout->setContentsMargins(0, 0, 0, 0);
    listLayout->setSpacing(8);
    scrollArea->setWidget(listWidget);

    layout->addWidget(header);
    layout->addWidget(scrollArea, 1);

    if (type == MemoType::Question) {
        questionCountLabel = countLabel;
        questionRecordsLayout = listLayout;
    } else {
        todoCountLabel = countLabel;
        todoRecordsLayout = listLayout;
    }

    return column;
}

QWidget *DashboardWindow::createRecordCard(const MemoItem &memo, QWidget *parent)
{
    auto *card = new QFrame(parent);
    card->setObjectName("DashboardRecordCard");
    card->setProperty("memoKind", MemoStore::typeToString(memo.type));
    card->setProperty("memoId", memo.id);
    card->setProperty("pressed", false);
    card->setCursor(Qt::PointingHandCursor);
    card->setToolTip(QStringLiteral("点击删除"));
    card->installEventFilter(this);

    auto *layout = new QHBoxLayout(card);
    layout->setContentsMargins(12, 10, 12, 10);
    layout->setSpacing(10);

    auto *accentDot = new QFrame(card);
    accentDot->setObjectName("RecordAccentDot");
    accentDot->setProperty("memoKind", MemoStore::typeToString(memo.type));
    accentDot->setFixedSize(7, 7);
    accentDot->setAttribute(Qt::WA_TransparentForMouseEvents);

    auto *contentLayout = new QVBoxLayout();
    contentLayout->setContentsMargins(0, 0, 0, 0);
    contentLayout->setSpacing(6);

    auto *textLabel = new QLabel(memo.text, card);
    textLabel->setObjectName("DashboardRecordText");
    textLabel->setWordWrap(true);
    textLabel->setMinimumWidth(0);
    textLabel->setAttribute(Qt::WA_TransparentForMouseEvents);

    auto *timeLabel = new QLabel(memo.createdAt.toString("yyyy-MM-dd HH:mm"), card);
    timeLabel->setObjectName("DashboardRecordTime");
    timeLabel->setMinimumWidth(0);
    timeLabel->setAlignment(Qt::AlignRight);
    timeLabel->setAttribute(Qt::WA_TransparentForMouseEvents);

    contentLayout->addWidget(textLabel);
    contentLayout->addWidget(timeLabel);
    layout->addWidget(accentDot, 0, Qt::AlignTop);
    layout->addLayout(contentLayout, 1);
    return card;
}

QWidget *DashboardWindow::createMemoControls(MemoType type, QWidget *parent)
{
    auto *container = new QFrame(parent);
    container->setObjectName("MemoControlCard");
    container->setProperty("memoKind", MemoStore::typeToString(type));
    container->setMinimumHeight(112);
    container->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);

    auto *layout = new QVBoxLayout(container);
    layout->setContentsMargins(12, 12, 12, 12);
    layout->setSpacing(8);

    auto *topRow = new QWidget(container);
    topRow->setMinimumHeight(38);
    auto *topRowLayout = new QHBoxLayout(topRow);
    topRowLayout->setContentsMargins(0, 0, 0, 0);
    topRowLayout->setSpacing(8);

    auto *label = new QLabel(MemoStore::displayName(type), topRow);
    label->setObjectName("MemoControlTitle");

    auto *visibilityButton = new QPushButton(topRow);
    visibilityButton->setObjectName("SecondaryButton");
    visibilityButton->setCursor(Qt::PointingHandCursor);
    visibilityButton->setMinimumSize(72, 34);
    auto *topCheck = new QCheckBox(QStringLiteral("置顶"), container);
    topCheck->setObjectName("PinToggle");
    topCheck->setProperty("memoKind", MemoStore::typeToString(type));
    topCheck->setCursor(Qt::PointingHandCursor);
    topCheck->setToolTip(QStringLiteral("保持便签窗口置顶"));
    topCheck->setMinimumHeight(36);

    connect(visibilityButton, &QPushButton::clicked, this, [this, type]() {
        const MemoWindowState state = store->windowState(type);
        if (state.visible) {
            emit hideMemoRequested(type);
        } else {
            emit showMemoRequested(type);
        }
    });
    connect(topCheck, &QCheckBox::toggled, this, [this, type](bool enabled) {
        emit alwaysOnTopChanged(type, enabled);
    });

    if (type == MemoType::Question) {
        questionVisibilityButton = visibilityButton;
        questionTopCheck = topCheck;
    } else {
        todoVisibilityButton = visibilityButton;
        todoTopCheck = topCheck;
    }

    topRowLayout->addWidget(label);
    topRowLayout->addStretch(1);
    topRowLayout->addWidget(visibilityButton);

    layout->addWidget(topRow);
    layout->addWidget(topCheck);
    return container;
}

QFrame *DashboardWindow::createSettingsGroup(const QString &title, QWidget *parent, QBoxLayout **contentLayout) const
{
    auto *group = new QFrame(parent);
    group->setObjectName("SettingsGroup");
    group->setMinimumHeight(92);
    group->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);

    auto *layout = new QVBoxLayout(group);
    layout->setContentsMargins(12, 12, 12, 12);
    layout->setSpacing(8);

    auto *titleLabel = new QLabel(title, group);
    titleLabel->setObjectName("SettingGroupTitle");
    layout->addWidget(titleLabel);

    if (contentLayout != nullptr) {
        *contentLayout = layout;
    }

    return group;
}

void DashboardWindow::refreshMemoControls(MemoType type)
{
    const MemoWindowState state = store->windowState(type);
    QPushButton *visibilityButton = type == MemoType::Question ? questionVisibilityButton : todoVisibilityButton;
    QCheckBox *topCheck = type == MemoType::Question ? questionTopCheck : todoTopCheck;

    visibilityButton->setText(state.visible ? QStringLiteral("隐藏") : QStringLiteral("显示"));
    visibilityButton->setToolTip(state.visible ? QStringLiteral("隐藏便签窗口")
                                                : QStringLiteral("显示便签窗口"));
    visibilityButton->setProperty("active", state.visible);
    refreshDynamicStyle(visibilityButton);

    const QSignalBlocker blocker(topCheck);
    topCheck->setChecked(state.alwaysOnTop);
    refreshDynamicStyle(topCheck);
}

void DashboardWindow::refreshRecords()
{
    refreshRecordColumn(MemoType::Question, store->records(MemoType::Question));
    refreshRecordColumn(MemoType::Todo, store->records(MemoType::Todo));
}

void DashboardWindow::refreshRecordColumn(MemoType type, const QVector<MemoItem> &records)
{
    QBoxLayout *layout = type == MemoType::Question ? questionRecordsLayout : todoRecordsLayout;
    QLabel *countLabel = type == MemoType::Question ? questionCountLabel : todoCountLabel;

    countLabel->setText(QString::number(records.size()));
    clearLayout(layout);

    for (const MemoItem &memo : records) {
        layout->addWidget(createRecordCard(memo, layout->parentWidget()));
    }

    layout->addStretch();
}

void DashboardWindow::clearLayout(QBoxLayout *layout) const
{
    while (QLayoutItem *item = layout->takeAt(0)) {
        delete item->widget();
        delete item;
    }
}

void DashboardWindow::configureScrollArea(QScrollArea *scrollArea) const
{
    if (scrollArea == nullptr) {
        return;
    }

    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    scrollArea->verticalScrollBar()->setSingleStep(32);
    scrollArea->verticalScrollBar()->setPageStep(160);
}
