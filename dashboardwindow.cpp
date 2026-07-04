#include "dashboardwindow.h"

#include "apptheme.h"

#include <QBoxLayout>
#include <QCheckBox>
#include <QCloseEvent>
#include <QComboBox>
#include <QFrame>
#include <QHBoxLayout>
#include <QKeySequenceEdit>
#include <QLabel>
#include <QPushButton>
#include <QScrollArea>
#include <QSignalBlocker>
#include <QStyle>
#include <QStringList>
#include <QVBoxLayout>

namespace {
void refreshDynamicStyle(QWidget *widget)
{
    widget->style()->unpolish(widget);
    widget->style()->polish(widget);
    widget->update();
}
}

DashboardWindow::DashboardWindow(MemoStore *store, QWidget *parent)
    : QWidget(parent)
    , store(store)
    , questionVisibilityButton(nullptr)
    , todoVisibilityButton(nullptr)
    , questionWindowStatusLabel(nullptr)
    , todoWindowStatusLabel(nullptr)
    , questionTopCheck(nullptr)
    , todoTopCheck(nullptr)
    , autostartCheck(nullptr)
    , themeCombo(nullptr)
    , hotkeyEdit(nullptr)
    , hotkeyPreviewLayout(nullptr)
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
        const QSignalBlocker blocker(themeCombo);
        themeCombo->setCurrentIndex(themeCombo->findData(static_cast<int>(store->themeMode())));
    }

    {
        const QSignalBlocker blocker(hotkeyEdit);
        hotkeyEdit->setKeySequence(QKeySequence(store->hotkey()));
    }
    refreshHotkeyPreview(QKeySequence(store->hotkey()));

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
    sidePanel->setFixedWidth(288);
    auto *sidePanelLayout = new QVBoxLayout(sidePanel);
    sidePanelLayout->setContentsMargins(0, 0, 0, 0);
    sidePanelLayout->setSpacing(0);

    auto *sideScrollArea = new QScrollArea(sidePanel);
    sideScrollArea->setObjectName("SideContentScrollArea");
    sideScrollArea->setWidgetResizable(true);
    sideScrollArea->setFrameShape(QFrame::NoFrame);
    sideScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    sideScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    auto *sideContent = new QWidget(sideScrollArea);
    sideContent->setObjectName("SideContent");
    sideContent->setMinimumHeight(720);

    auto *sideLayout = new QVBoxLayout(sideContent);
    sideLayout->setContentsMargins(16, 16, 16, 16);
    sideLayout->setSpacing(16);

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

    auto *applyHotkeyButton = new QPushButton(QStringLiteral("应用"), hotkeyGroup);
    applyHotkeyButton->setObjectName("PrimaryButton");
    applyHotkeyButton->setToolTip(QStringLiteral("应用新的全局快捷键"));
    connect(applyHotkeyButton, &QPushButton::clicked, this, [this]() {
        emit hotkeyChangeRequested(hotkeyEdit->keySequence());
    });
    connect(hotkeyEdit, &QKeySequenceEdit::keySequenceChanged,
            this, &DashboardWindow::refreshHotkeyPreview);

    auto *hotkeyRow = new QWidget(hotkeyGroup);
    auto *hotkeyRowLayout = new QHBoxLayout(hotkeyRow);
    hotkeyRowLayout->setContentsMargins(0, 0, 0, 0);
    hotkeyRowLayout->setSpacing(8);
    hotkeyRowLayout->addWidget(hotkeyEdit, 1);
    hotkeyRowLayout->addWidget(applyHotkeyButton);

    auto *hotkeyPreview = new QWidget(hotkeyGroup);
    hotkeyPreview->setObjectName("HotkeyPreview");
    hotkeyPreviewLayout = new QHBoxLayout(hotkeyPreview);
    hotkeyPreviewLayout->setContentsMargins(0, 0, 0, 0);
    hotkeyPreviewLayout->setSpacing(6);

    hotkeyGroupLayout->addWidget(hotkeyLabel);
    hotkeyGroupLayout->addWidget(hotkeyRow);
    hotkeyGroupLayout->addWidget(hotkeyPreview);

    QBoxLayout *appearanceGroupLayout = nullptr;
    auto *appearanceGroup = createSettingsGroup(QStringLiteral("外观"), sideContent, &appearanceGroupLayout);
    auto *themeLabel = new QLabel(QStringLiteral("主题"), appearanceGroup);
    themeLabel->setObjectName("FieldLabel");
    themeCombo = new QComboBox(appearanceGroup);
    themeCombo->setObjectName("ThemeCombo");
    themeCombo->setCursor(Qt::PointingHandCursor);
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

    QBoxLayout *systemGroupLayout = nullptr;
    auto *systemGroup = createSettingsGroup(QStringLiteral("系统"), sideContent, &systemGroupLayout);
    autostartCheck = new QCheckBox(QStringLiteral("开机自启"), systemGroup);
    autostartCheck->setToolTip(QStringLiteral("开机后自动启动 Quick Memo"));
    connect(autostartCheck, &QCheckBox::toggled, this, &DashboardWindow::autostartChanged);
    systemGroupLayout->addWidget(autostartCheck);

    auto *exitButton = new QPushButton(QStringLiteral("退出程序"), sideContent);
    exitButton->setObjectName("DangerButton");
    exitButton->setToolTip(QStringLiteral("保存状态并退出程序"));
    connect(exitButton, &QPushButton::clicked, this, &DashboardWindow::exitRequested);

    sideLayout->addWidget(hotkeyGroup);
    sideLayout->addWidget(appearanceGroup);
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

QWidget *DashboardWindow::createRecordCard(const MemoItem &memo, QWidget *parent) const
{
    auto *card = new QFrame(parent);
    card->setObjectName("DashboardRecordCard");
    card->setProperty("memoKind", MemoStore::typeToString(memo.type));

    auto *layout = new QHBoxLayout(card);
    layout->setContentsMargins(12, 10, 12, 10);
    layout->setSpacing(10);

    auto *accentDot = new QFrame(card);
    accentDot->setObjectName("RecordAccentDot");
    accentDot->setProperty("memoKind", MemoStore::typeToString(memo.type));
    accentDot->setFixedSize(7, 7);

    auto *contentLayout = new QVBoxLayout();
    contentLayout->setContentsMargins(0, 0, 0, 0);
    contentLayout->setSpacing(6);

    auto *textLabel = new QLabel(memo.text, card);
    textLabel->setObjectName("DashboardRecordText");
    textLabel->setWordWrap(true);
    textLabel->setMinimumWidth(0);
    textLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);

    auto *timeLabel = new QLabel(memo.createdAt.toString("yyyy-MM-dd HH:mm"), card);
    timeLabel->setObjectName("DashboardRecordTime");
    timeLabel->setMinimumWidth(0);
    timeLabel->setAlignment(Qt::AlignRight);

    contentLayout->addWidget(textLabel);
    contentLayout->addWidget(timeLabel);
    layout->addWidget(accentDot, 0, Qt::AlignTop);
    layout->addLayout(contentLayout, 1);
    return card;
}

QWidget *DashboardWindow::createEmptyState(MemoType type, QWidget *parent) const
{
    auto *emptyState = new QFrame(parent);
    emptyState->setObjectName("EmptyState");
    emptyState->setProperty("memoKind", MemoStore::typeToString(type));

    auto *layout = new QVBoxLayout(emptyState);
    layout->setContentsMargins(18, 18, 18, 18);
    layout->setSpacing(10);

    auto *label = new QLabel(type == MemoType::Question
                                 ? QStringLiteral("这里还没有问题")
                                 : QStringLiteral("这里还没有待办"),
                             emptyState);
    label->setObjectName("EmptyStateText");
    label->setAlignment(Qt::AlignCenter);

    for (int i = 0; i < 3; ++i) {
        auto *rule = new QFrame(emptyState);
        rule->setObjectName("EmptyStateRule");
        rule->setProperty("memoKind", MemoStore::typeToString(type));
        layout->addWidget(rule);
    }

    layout->addWidget(label);

    return emptyState;
}

QWidget *DashboardWindow::createMemoControls(MemoType type, QWidget *parent)
{
    auto *container = new QFrame(parent);
    container->setObjectName("MemoControlCard");
    container->setProperty("memoKind", MemoStore::typeToString(type));
    container->setMinimumHeight(96);

    auto *layout = new QVBoxLayout(container);
    layout->setContentsMargins(12, 12, 12, 12);
    layout->setSpacing(8);

    auto *topRow = new QWidget(container);
    topRow->setMinimumHeight(30);
    auto *topRowLayout = new QHBoxLayout(topRow);
    topRowLayout->setContentsMargins(0, 0, 0, 0);
    topRowLayout->setSpacing(8);

    auto *label = new QLabel(MemoStore::displayName(type), topRow);
    label->setObjectName("MemoControlTitle");

    auto *statusLabel = new QLabel(topRow);
    statusLabel->setObjectName("WindowStatusBadge");
    statusLabel->setProperty("memoKind", MemoStore::typeToString(type));
    statusLabel->setAlignment(Qt::AlignCenter);

    auto *visibilityButton = new QPushButton(topRow);
    visibilityButton->setObjectName("SecondaryButton");
    visibilityButton->setCursor(Qt::PointingHandCursor);
    auto *topCheck = new QCheckBox(QStringLiteral("置顶"), container);
    topCheck->setObjectName("PinToggle");
    topCheck->setProperty("memoKind", MemoStore::typeToString(type));
    topCheck->setCursor(Qt::PointingHandCursor);
    topCheck->setToolTip(QStringLiteral("保持便签窗口置顶"));
    topCheck->setMinimumHeight(34);

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
        questionWindowStatusLabel = statusLabel;
        questionTopCheck = topCheck;
    } else {
        todoVisibilityButton = visibilityButton;
        todoWindowStatusLabel = statusLabel;
        todoTopCheck = topCheck;
    }

    topRowLayout->addWidget(label);
    topRowLayout->addWidget(statusLabel);
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
    group->setMinimumHeight(72);

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
    QLabel *statusLabel = type == MemoType::Question ? questionWindowStatusLabel : todoWindowStatusLabel;
    QCheckBox *topCheck = type == MemoType::Question ? questionTopCheck : todoTopCheck;

    visibilityButton->setText(state.visible ? QStringLiteral("隐藏") : QStringLiteral("显示"));
    visibilityButton->setToolTip(state.visible ? QStringLiteral("隐藏便签窗口")
                                                : QStringLiteral("显示便签窗口"));
    visibilityButton->setProperty("active", state.visible);
    refreshDynamicStyle(visibilityButton);

    statusLabel->setText(state.visible ? QStringLiteral("已显示") : QStringLiteral("已隐藏"));
    statusLabel->setProperty("active", state.visible);
    refreshDynamicStyle(statusLabel);

    const QSignalBlocker blocker(topCheck);
    topCheck->setChecked(state.alwaysOnTop);
    refreshDynamicStyle(topCheck);
}

void DashboardWindow::refreshHotkeyPreview(const QKeySequence &sequence)
{
    if (hotkeyPreviewLayout == nullptr) {
        return;
    }

    clearLayout(hotkeyPreviewLayout);

    const QString text = sequence.toString(QKeySequence::NativeText).trimmed();
    QWidget *parent = hotkeyPreviewLayout->parentWidget();
    if (parent == nullptr) {
        return;
    }

    if (text.isEmpty()) {
        auto *emptyLabel = new QLabel(QStringLiteral("未设置"), parent);
        emptyLabel->setObjectName("Keycap");
        emptyLabel->setProperty("empty", true);
        emptyLabel->setAlignment(Qt::AlignCenter);
        hotkeyPreviewLayout->addWidget(emptyLabel);
        hotkeyPreviewLayout->addStretch(1);
        return;
    }

    const QStringList parts = text.split(QLatin1Char('+'), Qt::SkipEmptyParts);
    for (const QString &part : parts) {
        auto *keycap = new QLabel(part.trimmed(), parent);
        keycap->setObjectName("Keycap");
        keycap->setAlignment(Qt::AlignCenter);
        hotkeyPreviewLayout->addWidget(keycap);
    }
    hotkeyPreviewLayout->addStretch(1);
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

    if (records.isEmpty()) {
        layout->addWidget(createEmptyState(type, layout->parentWidget()));
    } else {
        for (const MemoItem &memo : records) {
            layout->addWidget(createRecordCard(memo, layout->parentWidget()));
        }
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
