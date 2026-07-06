#include "dashboardwindow.h"

#include "apptheme.h"
#include "apptext.h"

#include <QAbstractItemView>
#include <QBoxLayout>
#include <QCheckBox>
#include <QCloseEvent>
#include <QComboBox>
#include <QFrame>
#include <QGridLayout>
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
    , languageCombo(nullptr)
    , themeCombo(nullptr)
    , fontSizeCombo(nullptr)
    , densityCombo(nullptr)
    , memoStartupDisplayCombo(nullptr)
    , defaultInputTypeCombo(nullptr)
    , recordClickActionCombo(nullptr)
    , recordSortOrderCombo(nullptr)
    , hotkeyEdit(nullptr)
    , applyHotkeyButton(nullptr)
    , exportJsonButton(nullptr)
    , importJsonButton(nullptr)
    , recordsTitleLabel(nullptr)
    , memoSectionTitleLabel(nullptr)
    , settingsSectionTitleLabel(nullptr)
    , questionColumnTitleLabel(nullptr)
    , todoColumnTitleLabel(nullptr)
    , questionCountLabel(nullptr)
    , todoCountLabel(nullptr)
    , questionControlTitleLabel(nullptr)
    , todoControlTitleLabel(nullptr)
    , hotkeyGroupTitleLabel(nullptr)
    , hotkeyFieldLabel(nullptr)
    , personalizationGroupTitleLabel(nullptr)
    , languageFieldLabel(nullptr)
    , themeFieldLabel(nullptr)
    , fontSizeFieldLabel(nullptr)
    , densityFieldLabel(nullptr)
    , memoDisplayGroupTitleLabel(nullptr)
    , memoStartupDisplayFieldLabel(nullptr)
    , inputGroupTitleLabel(nullptr)
    , defaultInputTypeFieldLabel(nullptr)
    , recordGroupTitleLabel(nullptr)
    , recordClickActionFieldLabel(nullptr)
    , recordSortOrderFieldLabel(nullptr)
    , dataGroupTitleLabel(nullptr)
    , systemGroupTitleLabel(nullptr)
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
    retranslateUi();
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
        const QSignalBlocker blocker(languageCombo);
        languageCombo->setCurrentIndex(languageCombo->findData(static_cast<int>(store->language())));
    }

    {
        const QSignalBlocker blocker(themeCombo);
        themeCombo->setCurrentIndex(themeCombo->findData(static_cast<int>(store->themeMode())));
    }

    {
        const QSignalBlocker blocker(fontSizeCombo);
        fontSizeCombo->setCurrentIndex(fontSizeCombo->findData(static_cast<int>(store->fontSizeMode())));
    }

    {
        const QSignalBlocker blocker(densityCombo);
        densityCombo->setCurrentIndex(densityCombo->findData(static_cast<int>(store->densityMode())));
    }

    {
        const QSignalBlocker blocker(memoStartupDisplayCombo);
        memoStartupDisplayCombo->setCurrentIndex(
            memoStartupDisplayCombo->findData(static_cast<int>(store->memoStartupDisplayMode())));
    }

    {
        const QSignalBlocker blocker(defaultInputTypeCombo);
        defaultInputTypeCombo->setCurrentIndex(
            defaultInputTypeCombo->findData(static_cast<int>(store->defaultInputTypeMode())));
    }

    {
        const QSignalBlocker blocker(recordClickActionCombo);
        recordClickActionCombo->setCurrentIndex(
            recordClickActionCombo->findData(static_cast<int>(store->recordClickAction())));
    }

    {
        const QSignalBlocker blocker(recordSortOrderCombo);
        recordSortOrderCombo->setCurrentIndex(
            recordSortOrderCombo->findData(static_cast<int>(store->recordSortOrder())));
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

void DashboardWindow::applyTheme(ThemeMode mode, FontSizeMode fontSize, DensityMode density)
{
    setStyleSheet(AppTheme::dashboardStyleSheet(mode, fontSize, density));
    AppTheme::applyElevation(recordsPanel, mode, ElevationLevel::E2);
    AppTheme::applyElevation(sidePanel, mode, ElevationLevel::E1);
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
            emit recordClickRequested(memoId.toString(), type);
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
    setWindowTitle(AppText::dashboardWindowTitle(store->language()));
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

    recordsTitleLabel = new QLabel(recordsTitleRow);
    recordsTitleLabel->setObjectName("PageTitle");
    recordsTitleLayout->addWidget(recordsTitleLabel);
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
    sidePanel->setFixedWidth(280);
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
    sideLayout->setContentsMargins(10, 10, 10, 10);
    sideLayout->setSpacing(8);
    sideLayout->setSizeConstraint(QLayout::SetMinimumSize);

    memoSectionTitleLabel = new QLabel(sideContent);
    memoSectionTitleLabel->setObjectName("SideSectionTitle");
    sideLayout->addWidget(memoSectionTitleLabel);
    sideLayout->addWidget(createMemoControls(MemoType::Question, sideContent));
    sideLayout->addWidget(createMemoControls(MemoType::Todo, sideContent));

    settingsSectionTitleLabel = new QLabel(sideContent);
    settingsSectionTitleLabel->setObjectName("SideSectionTitle");
    sideLayout->addSpacing(2);
    sideLayout->addWidget(settingsSectionTitleLabel);

    QBoxLayout *hotkeyGroupLayout = nullptr;
    auto *hotkeyGroup = createSettingsGroup(&hotkeyGroupTitleLabel, sideContent, &hotkeyGroupLayout);
    hotkeyFieldLabel = new QLabel(hotkeyGroup);
    hotkeyFieldLabel->setObjectName("FieldLabel");
    hotkeyEdit = new QKeySequenceEdit(hotkeyGroup);
    hotkeyEdit->setMinimumHeight(34);

    applyHotkeyButton = new QPushButton(hotkeyGroup);
    applyHotkeyButton->setObjectName("PrimaryButton");
    applyHotkeyButton->setMinimumSize(68, 34);
    connect(applyHotkeyButton, &QPushButton::clicked, this, [this]() {
        emit hotkeyChangeRequested(hotkeyEdit->keySequence());
    });

    auto *hotkeyInputRow = new QWidget(hotkeyGroup);
    auto *hotkeyInputLayout = new QHBoxLayout(hotkeyInputRow);
    hotkeyInputLayout->setContentsMargins(0, 0, 0, 0);
    hotkeyInputLayout->setSpacing(6);
    hotkeyInputLayout->addWidget(hotkeyEdit, 1);
    hotkeyInputLayout->addWidget(applyHotkeyButton);

    hotkeyGroupLayout->addWidget(hotkeyFieldLabel);
    hotkeyGroupLayout->addWidget(hotkeyInputRow);
    hotkeyGroup->setMinimumHeight(100);

    QBoxLayout *personalizationGroupLayout = nullptr;
    auto *personalizationGroup = createSettingsGroup(&personalizationGroupTitleLabel,
                                                     sideContent,
                                                     &personalizationGroupLayout);
    auto *personalizationGrid = new QGridLayout();
    personalizationGrid->setContentsMargins(0, 0, 0, 0);
    personalizationGrid->setHorizontalSpacing(8);
    personalizationGrid->setVerticalSpacing(6);

    languageFieldLabel = new QLabel(personalizationGroup);
    languageFieldLabel->setObjectName("FieldLabel");
    themeFieldLabel = new QLabel(personalizationGroup);
    themeFieldLabel->setObjectName("FieldLabel");
    fontSizeFieldLabel = new QLabel(personalizationGroup);
    fontSizeFieldLabel->setObjectName("FieldLabel");
    densityFieldLabel = new QLabel(personalizationGroup);
    densityFieldLabel->setObjectName("FieldLabel");

    languageCombo = new NoWheelComboBox(personalizationGroup);
    languageCombo->setObjectName("LanguageCombo");
    languageCombo->setCursor(Qt::PointingHandCursor);
    languageCombo->setMinimumHeight(30);
    themeCombo = new NoWheelComboBox(personalizationGroup);
    themeCombo->setObjectName("ThemeCombo");
    themeCombo->setCursor(Qt::PointingHandCursor);
    themeCombo->setMinimumHeight(30);
    fontSizeCombo = new NoWheelComboBox(personalizationGroup);
    fontSizeCombo->setObjectName("FontSizeCombo");
    fontSizeCombo->setCursor(Qt::PointingHandCursor);
    fontSizeCombo->setMinimumHeight(30);
    densityCombo = new NoWheelComboBox(personalizationGroup);
    densityCombo->setObjectName("DensityCombo");
    densityCombo->setCursor(Qt::PointingHandCursor);
    densityCombo->setMinimumHeight(30);

    personalizationGrid->addWidget(languageFieldLabel, 0, 0);
    personalizationGrid->addWidget(languageCombo, 0, 1);
    personalizationGrid->addWidget(themeFieldLabel, 1, 0);
    personalizationGrid->addWidget(themeCombo, 1, 1);
    personalizationGrid->addWidget(fontSizeFieldLabel, 2, 0);
    personalizationGrid->addWidget(fontSizeCombo, 2, 1);
    personalizationGrid->addWidget(densityFieldLabel, 3, 0);
    personalizationGrid->addWidget(densityCombo, 3, 1);
    personalizationGrid->setColumnStretch(1, 1);
    personalizationGroupLayout->addLayout(personalizationGrid);
    personalizationGroup->setMinimumHeight(172);

    connect(languageCombo, &QComboBox::currentIndexChanged, this, [this](int index) {
        const QVariant data = languageCombo->itemData(index);
        if (!data.isValid()) {
            return;
        }
        emit languageChangeRequested(static_cast<AppLanguage>(data.toInt()));
    });
    connect(themeCombo, &QComboBox::currentIndexChanged, this, [this](int index) {
        const QVariant data = themeCombo->itemData(index);
        if (!data.isValid()) {
            return;
        }
        emit themeChangeRequested(static_cast<ThemeMode>(data.toInt()));
    });
    connect(fontSizeCombo, &QComboBox::currentIndexChanged, this, [this](int index) {
        const QVariant data = fontSizeCombo->itemData(index);
        if (!data.isValid()) {
            return;
        }
        emit fontSizeChangeRequested(static_cast<FontSizeMode>(data.toInt()));
    });
    connect(densityCombo, &QComboBox::currentIndexChanged, this, [this](int index) {
        const QVariant data = densityCombo->itemData(index);
        if (!data.isValid()) {
            return;
        }
        emit densityChangeRequested(static_cast<DensityMode>(data.toInt()));
    });

    QBoxLayout *memoDisplayGroupLayout = nullptr;
    auto *memoDisplayGroup = createSettingsGroup(&memoDisplayGroupTitleLabel,
                                                 sideContent,
                                                 &memoDisplayGroupLayout);
    auto *memoDisplayGrid = new QGridLayout();
    memoDisplayGrid->setContentsMargins(0, 0, 0, 0);
    memoDisplayGrid->setHorizontalSpacing(8);
    memoDisplayGrid->setVerticalSpacing(6);

    memoStartupDisplayFieldLabel = new QLabel(memoDisplayGroup);
    memoStartupDisplayFieldLabel->setObjectName("FieldLabel");
    memoStartupDisplayCombo = new NoWheelComboBox(memoDisplayGroup);
    memoStartupDisplayCombo->setObjectName("MemoStartupDisplayCombo");
    memoStartupDisplayCombo->setCursor(Qt::PointingHandCursor);
    memoStartupDisplayCombo->setMinimumHeight(30);

    memoDisplayGrid->addWidget(memoStartupDisplayFieldLabel, 0, 0);
    memoDisplayGrid->addWidget(memoStartupDisplayCombo, 0, 1);
    memoDisplayGrid->setColumnStretch(1, 1);
    memoDisplayGroupLayout->addLayout(memoDisplayGrid);
    memoDisplayGroup->setMinimumHeight(76);

    connect(memoStartupDisplayCombo, &QComboBox::currentIndexChanged, this, [this](int index) {
        const QVariant data = memoStartupDisplayCombo->itemData(index);
        if (!data.isValid()) {
            return;
        }
        emit memoStartupDisplayChangeRequested(static_cast<MemoStartupDisplayMode>(data.toInt()));
    });

    QBoxLayout *inputGroupLayout = nullptr;
    auto *inputGroup = createSettingsGroup(&inputGroupTitleLabel, sideContent, &inputGroupLayout);
    auto *inputGrid = new QGridLayout();
    inputGrid->setContentsMargins(0, 0, 0, 0);
    inputGrid->setHorizontalSpacing(8);
    inputGrid->setVerticalSpacing(6);

    defaultInputTypeFieldLabel = new QLabel(inputGroup);
    defaultInputTypeFieldLabel->setObjectName("FieldLabel");
    defaultInputTypeCombo = new NoWheelComboBox(inputGroup);
    defaultInputTypeCombo->setObjectName("DefaultInputTypeCombo");
    defaultInputTypeCombo->setCursor(Qt::PointingHandCursor);
    defaultInputTypeCombo->setMinimumHeight(30);

    inputGrid->addWidget(defaultInputTypeFieldLabel, 0, 0);
    inputGrid->addWidget(defaultInputTypeCombo, 0, 1);
    inputGrid->setColumnStretch(1, 1);
    inputGroupLayout->addLayout(inputGrid);

    hideInputAfterSaveCheck = new QCheckBox(inputGroup);
    hideInputAfterSaveCheck->setObjectName("InputToggle");
    hideInputAfterSaveCheck->setMinimumHeight(28);
    connect(defaultInputTypeCombo, &QComboBox::currentIndexChanged, this, [this](int index) {
        const QVariant data = defaultInputTypeCombo->itemData(index);
        if (!data.isValid()) {
            return;
        }
        emit defaultInputTypeChangeRequested(static_cast<DefaultInputTypeMode>(data.toInt()));
    });
    connect(hideInputAfterSaveCheck, &QCheckBox::toggled, this, &DashboardWindow::inputAutoHideChanged);
    inputGroupLayout->addWidget(hideInputAfterSaveCheck);
    inputGroup->setMinimumHeight(104);

    QBoxLayout *recordGroupLayout = nullptr;
    auto *recordGroup = createSettingsGroup(&recordGroupTitleLabel, sideContent, &recordGroupLayout);
    auto *recordGrid = new QGridLayout();
    recordGrid->setContentsMargins(0, 0, 0, 0);
    recordGrid->setHorizontalSpacing(8);
    recordGrid->setVerticalSpacing(6);

    recordClickActionFieldLabel = new QLabel(recordGroup);
    recordClickActionFieldLabel->setObjectName("FieldLabel");
    recordSortOrderFieldLabel = new QLabel(recordGroup);
    recordSortOrderFieldLabel->setObjectName("FieldLabel");

    recordClickActionCombo = new NoWheelComboBox(recordGroup);
    recordClickActionCombo->setObjectName("RecordClickActionCombo");
    recordClickActionCombo->setCursor(Qt::PointingHandCursor);
    recordClickActionCombo->setMinimumHeight(30);
    recordSortOrderCombo = new NoWheelComboBox(recordGroup);
    recordSortOrderCombo->setObjectName("RecordSortOrderCombo");
    recordSortOrderCombo->setCursor(Qt::PointingHandCursor);
    recordSortOrderCombo->setMinimumHeight(30);

    recordGrid->addWidget(recordClickActionFieldLabel, 0, 0);
    recordGrid->addWidget(recordClickActionCombo, 0, 1);
    recordGrid->addWidget(recordSortOrderFieldLabel, 1, 0);
    recordGrid->addWidget(recordSortOrderCombo, 1, 1);
    recordGrid->setColumnStretch(1, 1);
    recordGroupLayout->addLayout(recordGrid);
    recordGroup->setMinimumHeight(104);

    connect(recordClickActionCombo, &QComboBox::currentIndexChanged, this, [this](int index) {
        const QVariant data = recordClickActionCombo->itemData(index);
        if (!data.isValid()) {
            return;
        }
        emit recordClickActionChangeRequested(static_cast<RecordClickAction>(data.toInt()));
    });
    connect(recordSortOrderCombo, &QComboBox::currentIndexChanged, this, [this](int index) {
        const QVariant data = recordSortOrderCombo->itemData(index);
        if (!data.isValid()) {
            return;
        }
        emit recordSortOrderChangeRequested(static_cast<RecordSortOrder>(data.toInt()));
    });

    QBoxLayout *dataGroupLayout = nullptr;
    auto *dataGroup = createSettingsGroup(&dataGroupTitleLabel, sideContent, &dataGroupLayout);
    auto *dataButtonRow = new QWidget(dataGroup);
    auto *dataButtonLayout = new QHBoxLayout(dataButtonRow);
    dataButtonLayout->setContentsMargins(0, 0, 0, 0);
    dataButtonLayout->setSpacing(6);

    exportJsonButton = new QPushButton(dataGroup);
    exportJsonButton->setObjectName("SecondaryButton");
    exportJsonButton->setCursor(Qt::PointingHandCursor);
    exportJsonButton->setMinimumHeight(30);
    importJsonButton = new QPushButton(dataGroup);
    importJsonButton->setObjectName("SecondaryButton");
    importJsonButton->setCursor(Qt::PointingHandCursor);
    importJsonButton->setMinimumHeight(30);
    connect(exportJsonButton, &QPushButton::clicked, this, &DashboardWindow::exportJsonRequested);
    connect(importJsonButton, &QPushButton::clicked, this, &DashboardWindow::importJsonRequested);

    dataButtonLayout->addWidget(exportJsonButton, 1);
    dataButtonLayout->addWidget(importJsonButton, 1);
    dataGroupLayout->addWidget(dataButtonRow);
    dataGroup->setMinimumHeight(76);

    QBoxLayout *systemGroupLayout = nullptr;
    auto *systemGroup = createSettingsGroup(&systemGroupTitleLabel, sideContent, &systemGroupLayout);
    autostartCheck = new QCheckBox(systemGroup);
    autostartCheck->setObjectName("SystemToggle");
    autostartCheck->setMinimumHeight(28);
    connect(autostartCheck, &QCheckBox::toggled, this, &DashboardWindow::autostartChanged);
    systemGroupLayout->addWidget(autostartCheck);
    systemGroup->setMinimumHeight(68);

    sideLayout->addWidget(hotkeyGroup);
    sideLayout->addWidget(personalizationGroup);
    sideLayout->addWidget(memoDisplayGroup);
    sideLayout->addWidget(inputGroup);
    sideLayout->addWidget(recordGroup);
    sideLayout->addWidget(dataGroup);
    sideLayout->addWidget(systemGroup);
    sideLayout->addStretch(1);

    sideScrollArea->setWidget(sideContent);
    sidePanelLayout->addWidget(sideScrollArea);

    contentLayout->addWidget(recordsPanel, 1);
    contentLayout->addWidget(sidePanel);

    auto *statusBar = new QFrame(this);
    statusBar->setObjectName("StatusBar");
    auto *statusLayout = new QHBoxLayout(statusBar);
    statusLayout->setContentsMargins(16, 0, 16, 0);
    statusLabel = new QLabel(AppText::ready(store->language()), statusBar);
    statusLabel->setObjectName("StatusLabel");
    statusLayout->addWidget(statusLabel, 1);

    rootLayout->addWidget(content, 1);
    rootLayout->addWidget(statusBar);

    retranslateUi();
    applyTheme(ThemeMode::Light, FontSizeMode::Default, DensityMode::Comfortable);
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

    auto *titleLabel = new QLabel(store->categoryName(type), header);
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
        questionColumnTitleLabel = titleLabel;
        questionCountLabel = countLabel;
        questionRecordsLayout = listLayout;
    } else {
        todoColumnTitleLabel = titleLabel;
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
    card->setToolTip(store->recordClickAction() == RecordClickAction::Complete
                         ? AppText::clickToComplete(store->language())
                         : AppText::clickToDelete(store->language()));
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
    container->setMinimumHeight(56);
    container->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);

    auto *layout = new QVBoxLayout(container);
    layout->setContentsMargins(8, 8, 8, 8);
    layout->setSpacing(0);

    auto *topRow = new QWidget(container);
    topRow->setMinimumHeight(34);
    auto *topRowLayout = new QHBoxLayout(topRow);
    topRowLayout->setContentsMargins(0, 0, 0, 0);
    topRowLayout->setSpacing(6);

    auto *label = new QLabel(store->categoryName(type), topRow);
    label->setObjectName("MemoControlTitle");

    auto *visibilityButton = new QPushButton(topRow);
    visibilityButton->setObjectName("SecondaryButton");
    visibilityButton->setCursor(Qt::PointingHandCursor);
    visibilityButton->setMinimumSize(56, 30);
    auto *topCheck = new QCheckBox(AppText::pin(store->language()), topRow);
    topCheck->setObjectName("PinToggle");
    topCheck->setProperty("memoKind", MemoStore::typeToString(type));
    topCheck->setCursor(Qt::PointingHandCursor);
    topCheck->setToolTip(AppText::pinTooltip(store->language()));
    topCheck->setMinimumHeight(30);

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
        questionControlTitleLabel = label;
        questionVisibilityButton = visibilityButton;
        questionTopCheck = topCheck;
    } else {
        todoControlTitleLabel = label;
        todoVisibilityButton = visibilityButton;
        todoTopCheck = topCheck;
    }

    topRowLayout->addWidget(label);
    topRowLayout->addStretch(1);
    topRowLayout->addWidget(topCheck);
    topRowLayout->addWidget(visibilityButton);

    layout->addWidget(topRow);
    return container;
}

QFrame *DashboardWindow::createSettingsGroup(QLabel **titleLabel, QWidget *parent, QBoxLayout **contentLayout) const
{
    auto *group = new QFrame(parent);
    group->setObjectName("SettingsGroup");
    group->setMinimumHeight(64);
    group->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);

    auto *layout = new QVBoxLayout(group);
    layout->setContentsMargins(8, 8, 8, 8);
    layout->setSpacing(5);

    auto *label = new QLabel(group);
    label->setObjectName("SettingGroupTitle");
    layout->addWidget(label);

    if (titleLabel != nullptr) {
        *titleLabel = label;
    }

    if (contentLayout != nullptr) {
        *contentLayout = layout;
    }

    return group;
}

void DashboardWindow::retranslateUi()
{
    const AppLanguage language = store->language();

    setWindowTitle(AppText::dashboardWindowTitle(language));
    recordsTitleLabel->setText(AppText::recordsTitle(language));
    memoSectionTitleLabel->setText(AppText::memoWindowsTitle(language));
    settingsSectionTitleLabel->setText(AppText::settingsTitle(language));
    questionColumnTitleLabel->setText(store->categoryName(MemoType::Question));
    todoColumnTitleLabel->setText(store->categoryName(MemoType::Todo));
    questionControlTitleLabel->setText(store->categoryName(MemoType::Question));
    todoControlTitleLabel->setText(store->categoryName(MemoType::Todo));

    hotkeyGroupTitleLabel->setText(AppText::hotkeyTitle(language));
    hotkeyFieldLabel->setText(AppText::globalHotkeyLabel(language));
    applyHotkeyButton->setText(AppText::applyButton(language));
    applyHotkeyButton->setToolTip(AppText::applyHotkeyTooltip(language));

    personalizationGroupTitleLabel->setText(AppText::personalizationTitle(language));
    languageFieldLabel->setText(AppText::languageLabel(language));
    themeFieldLabel->setText(AppText::themeLabel(language));
    fontSizeFieldLabel->setText(AppText::fontSizeLabel(language));
    densityFieldLabel->setText(AppText::densityLabel(language));
    themeCombo->setToolTip(AppText::themeTooltip(language));
    fontSizeCombo->setToolTip(AppText::fontSizeTooltip(language));
    densityCombo->setToolTip(AppText::densityTooltip(language));

    memoDisplayGroupTitleLabel->setText(AppText::memoStartupDisplayTitle(language));
    memoStartupDisplayFieldLabel->setText(AppText::memoStartupDisplayLabel(language));
    memoStartupDisplayCombo->setToolTip(AppText::memoStartupDisplayTooltip(language));

    inputGroupTitleLabel->setText(AppText::inputTitle(language));
    defaultInputTypeFieldLabel->setText(AppText::defaultInputTypeLabel(language));
    defaultInputTypeCombo->setToolTip(AppText::defaultInputTypeTooltip(language));
    hideInputAfterSaveCheck->setText(AppText::hideInputAfterSave(language));
    hideInputAfterSaveCheck->setToolTip(AppText::hideInputAfterSaveTooltip(language));

    recordGroupTitleLabel->setText(AppText::recordTitle(language));
    recordClickActionFieldLabel->setText(AppText::recordClickActionLabel(language));
    recordClickActionCombo->setToolTip(AppText::recordClickActionTooltip(language));
    recordSortOrderFieldLabel->setText(AppText::recordSortOrderLabel(language));
    recordSortOrderCombo->setToolTip(AppText::recordSortOrderTooltip(language));

    dataGroupTitleLabel->setText(AppText::dataTitle(language));
    exportJsonButton->setText(AppText::exportJson(language));
    exportJsonButton->setToolTip(AppText::exportJsonTooltip(language));
    importJsonButton->setText(AppText::importJson(language));
    importJsonButton->setToolTip(AppText::importJsonTooltip(language));

    systemGroupTitleLabel->setText(AppText::systemTitle(language));
    autostartCheck->setText(AppText::autostart(language));
    autostartCheck->setToolTip(AppText::autostartTooltip(language));
    questionTopCheck->setText(AppText::pin(language));
    questionTopCheck->setToolTip(AppText::pinTooltip(language));
    todoTopCheck->setText(AppText::pin(language));
    todoTopCheck->setToolTip(AppText::pinTooltip(language));

    rebuildComboLabels();
}

void DashboardWindow::rebuildComboLabels()
{
    const AppLanguage language = store->language();

    {
        const QSignalBlocker blocker(languageCombo);
        languageCombo->clear();
        languageCombo->addItem(AppText::languageDisplayName(AppLanguage::ZhCn),
                               static_cast<int>(AppLanguage::ZhCn));
        languageCombo->addItem(AppText::languageDisplayName(AppLanguage::English),
                               static_cast<int>(AppLanguage::English));
        languageCombo->setCurrentIndex(languageCombo->findData(static_cast<int>(store->language())));
    }

    {
        const QSignalBlocker blocker(themeCombo);
        themeCombo->clear();
        themeCombo->addItem(AppText::themeDisplayName(ThemeMode::System, language),
                            static_cast<int>(ThemeMode::System));
        themeCombo->addItem(AppText::themeDisplayName(ThemeMode::Light, language),
                            static_cast<int>(ThemeMode::Light));
        themeCombo->addItem(AppText::themeDisplayName(ThemeMode::Dark, language),
                            static_cast<int>(ThemeMode::Dark));
        themeCombo->setCurrentIndex(themeCombo->findData(static_cast<int>(store->themeMode())));
    }

    {
        const QSignalBlocker blocker(fontSizeCombo);
        fontSizeCombo->clear();
        fontSizeCombo->addItem(AppText::fontSizeDisplayName(FontSizeMode::Small, language),
                               static_cast<int>(FontSizeMode::Small));
        fontSizeCombo->addItem(AppText::fontSizeDisplayName(FontSizeMode::Default, language),
                               static_cast<int>(FontSizeMode::Default));
        fontSizeCombo->addItem(AppText::fontSizeDisplayName(FontSizeMode::Large, language),
                               static_cast<int>(FontSizeMode::Large));
        fontSizeCombo->setCurrentIndex(fontSizeCombo->findData(static_cast<int>(store->fontSizeMode())));
    }

    {
        const QSignalBlocker blocker(densityCombo);
        densityCombo->clear();
        densityCombo->addItem(AppText::densityDisplayName(DensityMode::Comfortable, language),
                              static_cast<int>(DensityMode::Comfortable));
        densityCombo->addItem(AppText::densityDisplayName(DensityMode::Compact, language),
                              static_cast<int>(DensityMode::Compact));
        densityCombo->setCurrentIndex(densityCombo->findData(static_cast<int>(store->densityMode())));
    }

    {
        const QSignalBlocker blocker(memoStartupDisplayCombo);
        memoStartupDisplayCombo->clear();
        memoStartupDisplayCombo->addItem(
            AppText::memoStartupDisplayName(MemoStartupDisplayMode::Restore, language),
            static_cast<int>(MemoStartupDisplayMode::Restore));
        memoStartupDisplayCombo->addItem(
            AppText::memoStartupDisplayName(MemoStartupDisplayMode::ShowAll, language),
            static_cast<int>(MemoStartupDisplayMode::ShowAll));
        memoStartupDisplayCombo->addItem(
            AppText::memoStartupDisplayName(MemoStartupDisplayMode::HideAll, language),
            static_cast<int>(MemoStartupDisplayMode::HideAll));
        memoStartupDisplayCombo->setCurrentIndex(
            memoStartupDisplayCombo->findData(static_cast<int>(store->memoStartupDisplayMode())));
    }

    {
        const QSignalBlocker blocker(defaultInputTypeCombo);
        defaultInputTypeCombo->clear();
        defaultInputTypeCombo->addItem(
            AppText::defaultInputTypeName(DefaultInputTypeMode::LastUsed,
                                          store->categoryName(MemoType::Question),
                                          store->categoryName(MemoType::Todo),
                                          language),
            static_cast<int>(DefaultInputTypeMode::LastUsed));
        defaultInputTypeCombo->addItem(
            AppText::defaultInputTypeName(DefaultInputTypeMode::Question,
                                          store->categoryName(MemoType::Question),
                                          store->categoryName(MemoType::Todo),
                                          language),
            static_cast<int>(DefaultInputTypeMode::Question));
        defaultInputTypeCombo->addItem(
            AppText::defaultInputTypeName(DefaultInputTypeMode::Todo,
                                          store->categoryName(MemoType::Question),
                                          store->categoryName(MemoType::Todo),
                                          language),
            static_cast<int>(DefaultInputTypeMode::Todo));
        defaultInputTypeCombo->setCurrentIndex(
            defaultInputTypeCombo->findData(static_cast<int>(store->defaultInputTypeMode())));
    }

    {
        const QSignalBlocker blocker(recordClickActionCombo);
        recordClickActionCombo->clear();
        recordClickActionCombo->addItem(AppText::recordClickActionName(RecordClickAction::Delete, language),
                                        static_cast<int>(RecordClickAction::Delete));
        recordClickActionCombo->addItem(AppText::recordClickActionName(RecordClickAction::Complete, language),
                                        static_cast<int>(RecordClickAction::Complete));
        recordClickActionCombo->addItem(AppText::recordClickActionName(RecordClickAction::DashboardOnly, language),
                                        static_cast<int>(RecordClickAction::DashboardOnly));
        recordClickActionCombo->setCurrentIndex(
            recordClickActionCombo->findData(static_cast<int>(store->recordClickAction())));
    }

    {
        const QSignalBlocker blocker(recordSortOrderCombo);
        recordSortOrderCombo->clear();
        recordSortOrderCombo->addItem(AppText::recordSortOrderName(RecordSortOrder::NewestFirst, language),
                                      static_cast<int>(RecordSortOrder::NewestFirst));
        recordSortOrderCombo->addItem(AppText::recordSortOrderName(RecordSortOrder::OldestFirst, language),
                                      static_cast<int>(RecordSortOrder::OldestFirst));
        recordSortOrderCombo->setCurrentIndex(
            recordSortOrderCombo->findData(static_cast<int>(store->recordSortOrder())));
    }
}

void DashboardWindow::refreshMemoControls(MemoType type)
{
    const MemoWindowState state = store->windowState(type);
    QPushButton *visibilityButton = type == MemoType::Question ? questionVisibilityButton : todoVisibilityButton;
    QCheckBox *topCheck = type == MemoType::Question ? questionTopCheck : todoTopCheck;

    visibilityButton->setText(state.visible ? AppText::hide(store->language())
                                            : AppText::show(store->language()));
    visibilityButton->setToolTip(state.visible ? AppText::hideMemoTooltip(store->language())
                                                : AppText::showMemoTooltip(store->language()));
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
