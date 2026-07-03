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
#include <QVBoxLayout>

DashboardWindow::DashboardWindow(MemoStore *store, QWidget *parent)
    : QWidget(parent)
    , store(store)
    , questionVisibilityButton(nullptr)
    , todoVisibilityButton(nullptr)
    , questionTopCheck(nullptr)
    , todoTopCheck(nullptr)
    , autostartCheck(nullptr)
    , themeCombo(nullptr)
    , hotkeyEdit(nullptr)
    , questionCountLabel(nullptr)
    , todoCountLabel(nullptr)
    , questionRecordsLayout(nullptr)
    , todoRecordsLayout(nullptr)
    , statusLabel(nullptr)
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

    refreshRecords();
}

void DashboardWindow::setStatusMessage(const QString &message)
{
    statusLabel->setText(message);
}

void DashboardWindow::applyTheme(ThemeMode mode)
{
    setStyleSheet(AppTheme::dashboardStyleSheet(mode));
}

void DashboardWindow::closeEvent(QCloseEvent *event)
{
    event->ignore();
    hide();
}

void DashboardWindow::setupUi()
{
    setWindowTitle(QStringLiteral("Quick Memo 后台"));
    resize(980, 640);
    setMinimumSize(860, 520);

    auto *rootLayout = new QVBoxLayout(this);
    rootLayout->setContentsMargins(16, 16, 16, 12);
    rootLayout->setSpacing(10);

    auto *content = new QWidget(this);
    auto *contentLayout = new QHBoxLayout(content);
    contentLayout->setContentsMargins(0, 0, 0, 0);
    contentLayout->setSpacing(14);

    auto *recordsPanel = new QFrame(content);
    recordsPanel->setObjectName("RecordsPanel");
    auto *recordsPanelLayout = new QVBoxLayout(recordsPanel);
    recordsPanelLayout->setContentsMargins(16, 14, 16, 16);
    recordsPanelLayout->setSpacing(12);

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
    columnsLayout->setSpacing(12);
    columnsLayout->addWidget(createRecordsColumn(MemoType::Question, columns), 1);
    columnsLayout->addWidget(createRecordsColumn(MemoType::Todo, columns), 1);

    recordsPanelLayout->addWidget(recordsTitleRow);
    recordsPanelLayout->addWidget(columns, 1);

    auto *sidePanel = new QFrame(content);
    sidePanel->setObjectName("SidePanel");
    sidePanel->setFixedWidth(286);
    auto *sideLayout = new QVBoxLayout(sidePanel);
    sideLayout->setContentsMargins(14, 14, 14, 14);
    sideLayout->setSpacing(12);

    auto *memoSectionTitle = new QLabel(QStringLiteral("便签窗口"), sidePanel);
    memoSectionTitle->setObjectName("SideSectionTitle");
    sideLayout->addWidget(memoSectionTitle);
    sideLayout->addWidget(createMemoControls(MemoType::Question, sidePanel));
    sideLayout->addWidget(createMemoControls(MemoType::Todo, sidePanel));

    auto *settingsSectionTitle = new QLabel(QStringLiteral("设置"), sidePanel);
    settingsSectionTitle->setObjectName("SideSectionTitle");
    sideLayout->addSpacing(4);
    sideLayout->addWidget(settingsSectionTitle);

    auto *hotkeyLabel = new QLabel(QStringLiteral("全局快捷键"), sidePanel);
    hotkeyLabel->setObjectName("FieldLabel");
    hotkeyEdit = new QKeySequenceEdit(sidePanel);

    auto *applyHotkeyButton = new QPushButton(QStringLiteral("应用"), sidePanel);
    applyHotkeyButton->setObjectName("PrimaryButton");
    connect(applyHotkeyButton, &QPushButton::clicked, this, [this]() {
        emit hotkeyChangeRequested(hotkeyEdit->keySequence());
    });

    auto *hotkeyRow = new QWidget(sidePanel);
    auto *hotkeyRowLayout = new QHBoxLayout(hotkeyRow);
    hotkeyRowLayout->setContentsMargins(0, 0, 0, 0);
    hotkeyRowLayout->setSpacing(8);
    hotkeyRowLayout->addWidget(hotkeyEdit, 1);
    hotkeyRowLayout->addWidget(applyHotkeyButton);

    auto *themeLabel = new QLabel(QStringLiteral("主题"), sidePanel);
    themeLabel->setObjectName("FieldLabel");
    themeCombo = new QComboBox(sidePanel);
    themeCombo->addItem(MemoStore::themeDisplayName(ThemeMode::Light), static_cast<int>(ThemeMode::Light));
    themeCombo->addItem(MemoStore::themeDisplayName(ThemeMode::Dark), static_cast<int>(ThemeMode::Dark));
    connect(themeCombo, &QComboBox::currentIndexChanged, this, [this](int index) {
        const QVariant data = themeCombo->itemData(index);
        if (!data.isValid()) {
            return;
        }
        emit themeChangeRequested(static_cast<ThemeMode>(data.toInt()));
    });

    autostartCheck = new QCheckBox(QStringLiteral("开机自启"), sidePanel);
    connect(autostartCheck, &QCheckBox::toggled, this, &DashboardWindow::autostartChanged);

    auto *exitButton = new QPushButton(QStringLiteral("退出程序"), sidePanel);
    exitButton->setObjectName("DangerButton");
    connect(exitButton, &QPushButton::clicked, this, &DashboardWindow::exitRequested);

    sideLayout->addWidget(hotkeyLabel);
    sideLayout->addWidget(hotkeyRow);
    sideLayout->addWidget(themeLabel);
    sideLayout->addWidget(themeCombo);
    sideLayout->addWidget(autostartCheck);
    sideLayout->addStretch(1);
    sideLayout->addWidget(exitButton);

    contentLayout->addWidget(recordsPanel, 1);
    contentLayout->addWidget(sidePanel);

    auto *statusBar = new QFrame(this);
    statusBar->setObjectName("StatusBar");
    auto *statusLayout = new QHBoxLayout(statusBar);
    statusLayout->setContentsMargins(10, 0, 10, 0);
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

    auto *layout = new QVBoxLayout(column);
    layout->setContentsMargins(12, 12, 12, 12);
    layout->setSpacing(10);

    auto *header = new QWidget(column);
    auto *headerLayout = new QHBoxLayout(header);
    headerLayout->setContentsMargins(0, 0, 0, 0);

    auto *titleLabel = new QLabel(MemoStore::displayName(type), header);
    titleLabel->setObjectName("RecordColumnTitle");

    auto *countLabel = new QLabel(QStringLiteral("0"), header);
    countLabel->setObjectName("CountBadge");
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

    auto *layout = new QVBoxLayout(card);
    layout->setContentsMargins(12, 10, 12, 10);
    layout->setSpacing(6);

    auto *textLabel = new QLabel(memo.text, card);
    textLabel->setObjectName("DashboardRecordText");
    textLabel->setWordWrap(true);
    textLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);

    auto *timeLabel = new QLabel(memo.createdAt.toString("yyyy-MM-dd HH:mm"), card);
    timeLabel->setObjectName("DashboardRecordTime");

    layout->addWidget(textLabel);
    layout->addWidget(timeLabel);
    return card;
}

QWidget *DashboardWindow::createEmptyState(QWidget *parent) const
{
    auto *emptyState = new QFrame(parent);
    emptyState->setObjectName("EmptyState");

    auto *layout = new QVBoxLayout(emptyState);
    layout->setContentsMargins(12, 28, 12, 28);

    auto *label = new QLabel(QStringLiteral("暂无记录"), emptyState);
    label->setObjectName("EmptyStateText");
    label->setAlignment(Qt::AlignCenter);
    layout->addWidget(label);

    return emptyState;
}

QWidget *DashboardWindow::createMemoControls(MemoType type, QWidget *parent)
{
    auto *container = new QFrame(parent);
    container->setObjectName("MemoControlCard");

    auto *layout = new QVBoxLayout(container);
    layout->setContentsMargins(10, 10, 10, 10);
    layout->setSpacing(8);

    auto *topRow = new QWidget(container);
    auto *topRowLayout = new QHBoxLayout(topRow);
    topRowLayout->setContentsMargins(0, 0, 0, 0);
    topRowLayout->setSpacing(8);

    auto *label = new QLabel(MemoStore::displayName(type), topRow);
    label->setObjectName("MemoControlTitle");

    auto *visibilityButton = new QPushButton(topRow);
    visibilityButton->setObjectName("SecondaryButton");
    auto *topCheck = new QCheckBox(QStringLiteral("置顶"), container);

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

    topRowLayout->addWidget(label, 1);
    topRowLayout->addWidget(visibilityButton);

    layout->addWidget(topRow);
    layout->addWidget(topCheck);
    return container;
}

void DashboardWindow::refreshMemoControls(MemoType type)
{
    const MemoWindowState state = store->windowState(type);
    QPushButton *visibilityButton = type == MemoType::Question ? questionVisibilityButton : todoVisibilityButton;
    QCheckBox *topCheck = type == MemoType::Question ? questionTopCheck : todoTopCheck;

    visibilityButton->setText(state.visible ? QStringLiteral("隐藏") : QStringLiteral("显示"));

    const QSignalBlocker blocker(topCheck);
    topCheck->setChecked(state.alwaysOnTop);
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
        layout->addWidget(createEmptyState(layout->parentWidget()));
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
