#include "dashboardwindow.h"

#include <QAbstractItemView>
#include <QCheckBox>
#include <QCloseEvent>
#include <QFormLayout>
#include <QGroupBox>
#include <QHeaderView>
#include <QHBoxLayout>
#include <QKeySequenceEdit>
#include <QLabel>
#include <QPushButton>
#include <QSignalBlocker>
#include <QTableWidget>
#include <QVBoxLayout>

DashboardWindow::DashboardWindow(MemoStore *store, QWidget *parent)
    : QWidget(parent)
    , store(store)
    , questionVisibilityButton(nullptr)
    , todoVisibilityButton(nullptr)
    , questionTopCheck(nullptr)
    , todoTopCheck(nullptr)
    , autostartCheck(nullptr)
    , hotkeyEdit(nullptr)
    , questionRecordsTable(nullptr)
    , todoRecordsTable(nullptr)
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
        const QSignalBlocker blocker(hotkeyEdit);
        hotkeyEdit->setKeySequence(QKeySequence(store->hotkey()));
    }

    refreshRecords();
}

void DashboardWindow::setStatusMessage(const QString &message)
{
    statusLabel->setText(message);
}

void DashboardWindow::closeEvent(QCloseEvent *event)
{
    event->ignore();
    hide();
}

void DashboardWindow::setupUi()
{
    setWindowTitle(QStringLiteral("Quick Memo 后台"));
    resize(900, 560);

    auto *rootLayout = new QVBoxLayout(this);
    rootLayout->setContentsMargins(14, 14, 14, 14);
    rootLayout->setSpacing(12);

    auto *windowGroup = new QGroupBox(QStringLiteral("便签窗口"), this);
    auto *windowLayout = new QVBoxLayout(windowGroup);
    windowLayout->addWidget(createMemoControls(MemoType::Question));
    windowLayout->addWidget(createMemoControls(MemoType::Todo));

    auto *settingsGroup = new QGroupBox(QStringLiteral("设置"), this);
    auto *settingsLayout = new QFormLayout(settingsGroup);

    hotkeyEdit = new QKeySequenceEdit(settingsGroup);
    auto *applyHotkeyButton = new QPushButton(QStringLiteral("应用快捷键"), settingsGroup);
    connect(applyHotkeyButton, &QPushButton::clicked, this, [this]() {
        emit hotkeyChangeRequested(hotkeyEdit->keySequence());
    });

    auto *hotkeyRow = new QWidget(settingsGroup);
    auto *hotkeyRowLayout = new QHBoxLayout(hotkeyRow);
    hotkeyRowLayout->setContentsMargins(0, 0, 0, 0);
    hotkeyRowLayout->addWidget(hotkeyEdit, 1);
    hotkeyRowLayout->addWidget(applyHotkeyButton);

    autostartCheck = new QCheckBox(QStringLiteral("开机自启"), settingsGroup);
    connect(autostartCheck, &QCheckBox::toggled, this, &DashboardWindow::autostartChanged);

    settingsLayout->addRow(QStringLiteral("全局快捷键"), hotkeyRow);
    settingsLayout->addRow(QString(), autostartCheck);

    auto *recordsGroup = new QGroupBox(QStringLiteral("全部记录"), this);
    auto *recordsLayout = new QHBoxLayout(recordsGroup);
    recordsLayout->setSpacing(12);

    auto *questionGroup = new QGroupBox(MemoStore::displayName(MemoType::Question), recordsGroup);
    auto *questionLayout = new QVBoxLayout(questionGroup);
    questionRecordsTable = createRecordsTable(questionGroup);
    questionLayout->addWidget(questionRecordsTable);

    auto *todoGroup = new QGroupBox(MemoStore::displayName(MemoType::Todo), recordsGroup);
    auto *todoLayout = new QVBoxLayout(todoGroup);
    todoRecordsTable = createRecordsTable(todoGroup);
    todoLayout->addWidget(todoRecordsTable);

    recordsLayout->addWidget(questionGroup);
    recordsLayout->addWidget(todoGroup);

    statusLabel = new QLabel(this);
    statusLabel->setText(QStringLiteral("就绪"));

    auto *exitButton = new QPushButton(QStringLiteral("退出程序"), this);
    connect(exitButton, &QPushButton::clicked, this, &DashboardWindow::exitRequested);

    auto *bottomLayout = new QHBoxLayout();
    bottomLayout->addWidget(statusLabel, 1);
    bottomLayout->addWidget(exitButton);

    rootLayout->addWidget(windowGroup);
    rootLayout->addWidget(settingsGroup);
    rootLayout->addWidget(recordsGroup, 1);
    rootLayout->addLayout(bottomLayout);
}

QWidget *DashboardWindow::createMemoControls(MemoType type)
{
    auto *container = new QWidget(this);
    auto *layout = new QHBoxLayout(container);
    layout->setContentsMargins(0, 0, 0, 0);

    auto *label = new QLabel(MemoStore::displayName(type), container);
    auto *visibilityButton = new QPushButton(container);
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

    layout->addWidget(label);
    layout->addStretch();
    layout->addWidget(topCheck);
    layout->addWidget(visibilityButton);
    return container;
}

QTableWidget *DashboardWindow::createRecordsTable(QWidget *parent) const
{
    auto *table = new QTableWidget(parent);
    table->setColumnCount(2);
    table->setHorizontalHeaderLabels({
        QStringLiteral("内容"),
        QStringLiteral("创建时间")
    });
    table->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    table->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table->setSelectionBehavior(QAbstractItemView::SelectRows);
    return table;
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
    fillRecordsTable(questionRecordsTable, store->records(MemoType::Question));
    fillRecordsTable(todoRecordsTable, store->records(MemoType::Todo));
}

void DashboardWindow::fillRecordsTable(QTableWidget *table, const QVector<MemoItem> &records)
{
    table->setRowCount(records.size());

    for (int row = 0; row < records.size(); ++row) {
        const MemoItem &memo = records.at(row);
        table->setItem(row, 0, new QTableWidgetItem(memo.text));
        table->setItem(row, 1, new QTableWidgetItem(memo.createdAt.toString("yyyy-MM-dd HH:mm")));
    }
}
