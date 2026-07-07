#include "dashboardwindow.h"

#include "theme/apptext.h"
#include "theme/apptheme.h"

#include <QAbstractItemView>
#include <QBoxLayout>
#include <QCheckBox>
#include <QCloseEvent>
#include <QComboBox>
#include <QFrame>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QKeyEvent>
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
#include <QToolButton>
#include <QTimer>
#include <QVBoxLayout>
#include <QWheelEvent>

#include <functional>

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

class CollapsibleSettingsGroup : public QFrame
{
public:
    explicit CollapsibleSettingsGroup(QWidget *parent = nullptr, bool expandedByDefault = false)
        : QFrame(parent)
        , expanded(expandedByDefault)
        , header(new QWidget(this))
        , arrowButton(new QToolButton(header))
        , title(new QLabel(header))
        , summary(new QLabel(header))
        , content(new QWidget(this))
        , contentBox(new QVBoxLayout(content))
    {
        setObjectName("SettingsGroup");
        setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);

        auto *rootLayout = new QVBoxLayout(this);
        rootLayout->setContentsMargins(0, 0, 0, 0);
        rootLayout->setSpacing(0);
        rootLayout->setSizeConstraint(QLayout::SetMinimumSize);

        header->setObjectName("SettingsGroupHeader");
        header->setCursor(Qt::PointingHandCursor);
        header->installEventFilter(this);

        auto *headerLayout = new QHBoxLayout(header);
        headerLayout->setContentsMargins(12, 10, 12, 10);
        headerLayout->setSpacing(8);

        title->setObjectName("SettingGroupTitle");
        title->setAttribute(Qt::WA_TransparentForMouseEvents);

        summary->setObjectName("SettingGroupSummary");
        summary->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
        summary->setMinimumWidth(0);
        summary->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
        summary->setAttribute(Qt::WA_TransparentForMouseEvents);

        arrowButton->setObjectName("CollapseArrow");
        arrowButton->setAutoRaise(true);
        arrowButton->setCursor(Qt::PointingHandCursor);
        arrowButton->setFixedSize(20, 20);
        connect(arrowButton, &QToolButton::clicked, this, [this]() {
            setExpanded(!expanded);
        });

        headerLayout->addWidget(title);
        headerLayout->addWidget(summary, 1);
        headerLayout->addWidget(arrowButton);

        content->setObjectName("SettingsGroupContent");
        content->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);
        contentBox->setContentsMargins(12, 4, 12, 12);
        contentBox->setSpacing(10);
        contentBox->setSizeConstraint(QLayout::SetMinimumSize);

        rootLayout->addWidget(header);
        rootLayout->addWidget(content);

        setExpanded(expanded);
    }

    QLabel *titleLabel() const
    {
        return title;
    }

    QBoxLayout *contentLayout() const
    {
        return contentBox;
    }

    void setSummaryText(const QString &text)
    {
        summary->setText(text);
        summary->setToolTip(text);
    }

    void setCollapseCallback(std::function<void()> callback)
    {
        collapseCallback = callback;
    }

    void setExpanded(bool value)
    {
        const bool wasExpanded = expanded;
        if (wasExpanded && !value && collapseCallback) {
            collapseCallback();
        }

        expanded = value;
        content->setVisible(expanded);
        arrowButton->setArrowType(expanded ? Qt::DownArrow : Qt::RightArrow);
        setProperty("expanded", expanded);
        refreshMinimumHeight();
        refreshDynamicStyle(this);
        refreshDynamicStyle(header);
    }

    void refreshMinimumHeight()
    {
        if (!expanded) {
            setMinimumHeight(44);
            setMaximumHeight(44);
            return;
        }

        setMaximumHeight(QWIDGETSIZE_MAX);
        const int expandedHeight = header->sizeHint().height() + content->sizeHint().height();
        setMinimumHeight(qMax(64, expandedHeight));
    }

protected:
    bool eventFilter(QObject *object, QEvent *event) override
    {
        if (object == header && event->type() == QEvent::MouseButtonRelease) {
            auto *mouseEvent = static_cast<QMouseEvent *>(event);
            if (mouseEvent->button() == Qt::LeftButton && header->rect().contains(mouseEvent->pos())) {
                setExpanded(!expanded);
                return true;
            }
        }

        return QFrame::eventFilter(object, event);
    }

private:
    bool expanded;
    QWidget *header;
    QToolButton *arrowButton;
    QLabel *title;
    QLabel *summary;
    QWidget *content;
    QVBoxLayout *contentBox;
    std::function<void()> collapseCallback;
};

QString onOffText(bool enabled, AppLanguage language)
{
    if (language == AppLanguage::English) {
        return enabled ? QStringLiteral("On") : QStringLiteral("Off");
    }

    return enabled ? QString::fromUtf8("开启") : QString::fromUtf8("关闭");
}

QString statusKindName(DashboardWindow::DashboardStatusKind kind)
{
    switch (kind) {
    case DashboardWindow::DashboardStatusKind::Ready:
        return QStringLiteral("ready");
    case DashboardWindow::DashboardStatusKind::Success:
        return QStringLiteral("success");
    case DashboardWindow::DashboardStatusKind::Error:
        return QStringLiteral("error");
    case DashboardWindow::DashboardStatusKind::Info:
        return QStringLiteral("info");
    }

    return QStringLiteral("info");
}

void setGroupSummary(QFrame *frame, const QString &summary)
{
    auto *group = static_cast<CollapsibleSettingsGroup *>(frame);
    group->setSummaryText(summary);
}

void refreshGroupHeight(QFrame *frame)
{
    auto *group = static_cast<CollapsibleSettingsGroup *>(frame);
    group->refreshMinimumHeight();
}
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
    , hotkeyFeedbackLabel(nullptr)
    , hotkeyFeedbackTimer(nullptr)
    , exportJsonButton(nullptr)
    , importJsonButton(nullptr)
    , personalizationGroupFrame(nullptr)
    , inputGroupFrame(nullptr)
    , recordGroupFrame(nullptr)
    , dataGroupFrame(nullptr)
    , recordsTitleLabel(nullptr)
    , memoSectionTitleLabel(nullptr)
    , settingsSectionTitleLabel(nullptr)
    , questionColumnTitleLabel(nullptr)
    , todoColumnTitleLabel(nullptr)
    , questionCountLabel(nullptr)
    , todoCountLabel(nullptr)
    , questionControlTitleLabel(nullptr)
    , todoControlTitleLabel(nullptr)
    , hotkeyFieldLabel(nullptr)
    , personalizationGroupTitleLabel(nullptr)
    , languageFieldLabel(nullptr)
    , themeFieldLabel(nullptr)
    , fontSizeFieldLabel(nullptr)
    , densityFieldLabel(nullptr)
    , memoStartupDisplayFieldLabel(nullptr)
    , inputGroupTitleLabel(nullptr)
    , defaultInputTypeFieldLabel(nullptr)
    , recordGroupTitleLabel(nullptr)
    , recordClickActionFieldLabel(nullptr)
    , recordSortOrderFieldLabel(nullptr)
    , dataGroupTitleLabel(nullptr)
    , questionRecordsLayout(nullptr)
    , todoRecordsLayout(nullptr)
    , statusLabel(nullptr)
    , recordsPanel(nullptr)
    , sidePanel(nullptr)
    , storedHotkeySequence()
    , hotkeyState(HotkeyEditState::Idle)
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
        const QKeySequence savedHotkey(store->hotkey());
        const bool keepDraft = hotkeyState == HotkeyEditState::Recording || hotkeyState == HotkeyEditState::Pending;
        storedHotkeySequence = savedHotkey;
        if (!keepDraft) {
            const QSignalBlocker blocker(hotkeyEdit);
            hotkeyEdit->setKeySequence(savedHotkey);
        }
        updateHotkeyApplyState();
    }

    refreshRecords();
}

void DashboardWindow::setStatusMessage(const QString &message, DashboardStatusKind kind)
{
    statusLabel->setText(message);
    statusLabel->setProperty("statusKind", statusKindName(kind));
    refreshDynamicStyle(statusLabel);
}

void DashboardWindow::applyTheme(ThemeMode mode, FontSizeMode fontSize, DensityMode density)
{
    setStyleSheet(AppTheme::dashboardStyleSheet(mode, fontSize, density));
    AppTheme::applyElevation(recordsPanel, mode, ElevationLevel::E2);
    AppTheme::applyElevation(sidePanel, mode, ElevationLevel::E1);
}

void DashboardWindow::setHotkeyChangeSucceeded(const QKeySequence &sequence)
{
    storedHotkeySequence = sequence;
    {
        const QSignalBlocker blocker(hotkeyEdit);
        hotkeyEdit->setKeySequence(sequence);
    }

    applyHotkeyButton->setEnabled(false);
    updateSettingsGroupSummaries();
    setHotkeyEditState(HotkeyEditState::Success,
                       AppText::hotkeyUpdated(sequence.toString(QKeySequence::NativeText), store->language()));
    hotkeyFeedbackTimer->start(1400);
}

void DashboardWindow::setHotkeyChangeFailed(const QString &message)
{
    {
        const QSignalBlocker blocker(hotkeyEdit);
        hotkeyEdit->setKeySequence(storedHotkeySequence);
    }

    applyHotkeyButton->setEnabled(false);
    setHotkeyEditState(HotkeyEditState::Error, message);
    hotkeyFeedbackTimer->start(2600);
}

void DashboardWindow::setHotkeyEditState(HotkeyEditState state, const QString &message)
{
    hotkeyState = state;
    if (hotkeyFeedbackTimer != nullptr) {
        hotkeyFeedbackTimer->stop();
    }

    QString feedbackKind = QStringLiteral("idle");
    QString editState;
    QString feedbackText = message;

    switch (state) {
    case HotkeyEditState::Idle:
        feedbackText = feedbackText.isEmpty() ? AppText::hotkeyIdleHint(store->language()) : feedbackText;
        break;
    case HotkeyEditState::Recording:
        feedbackKind = QStringLiteral("recording");
        editState = QStringLiteral("recording");
        feedbackText = feedbackText.isEmpty() ? AppText::hotkeyRecordingHint(store->language()) : feedbackText;
        break;
    case HotkeyEditState::Pending:
        feedbackKind = QStringLiteral("pending");
        editState = QStringLiteral("pending");
        feedbackText = feedbackText.isEmpty() ? AppText::hotkeyPendingHint(store->language()) : feedbackText;
        break;
    case HotkeyEditState::Success:
        feedbackKind = QStringLiteral("success");
        editState = QStringLiteral("success");
        break;
    case HotkeyEditState::Error:
        feedbackKind = QStringLiteral("error");
        editState = QStringLiteral("error");
        break;
    }

    hotkeyEdit->setProperty("captureState", editState);
    hotkeyFeedbackLabel->setProperty("feedbackKind", feedbackKind);
    hotkeyFeedbackLabel->setText(feedbackText);
    refreshDynamicStyle(hotkeyEdit);
    refreshDynamicStyle(hotkeyFeedbackLabel);
    refreshDynamicStyle(applyHotkeyButton);
    refreshGroupHeight(inputGroupFrame);
}

void DashboardWindow::updateHotkeyApplyState()
{
    const QKeySequence sequence = hotkeyEdit->keySequence();
    const bool hasPendingChange = !sequence.isEmpty() && sequence != storedHotkeySequence;

    applyHotkeyButton->setEnabled(hasPendingChange);
    if (hasPendingChange) {
        setHotkeyEditState(HotkeyEditState::Pending);
        return;
    }

    setHotkeyEditState(hotkeyEdit->hasFocus() ? HotkeyEditState::Recording : HotkeyEditState::Idle);
}

void DashboardWindow::resetHotkeyEdit()
{
    {
        const QSignalBlocker blocker(hotkeyEdit);
        hotkeyEdit->setKeySequence(storedHotkeySequence);
    }

    applyHotkeyButton->setEnabled(false);
    setHotkeyEditState(HotkeyEditState::Idle);
}

bool DashboardWindow::eventFilter(QObject *object, QEvent *event)
{
    if (object == hotkeyEdit) {
        if (event->type() == QEvent::FocusIn) {
            updateHotkeyApplyState();
            return QWidget::eventFilter(object, event);
        }

        if (event->type() == QEvent::FocusOut) {
            updateHotkeyApplyState();
            return QWidget::eventFilter(object, event);
        }

        if (event->type() == QEvent::KeyPress) {
            auto *keyEvent = static_cast<QKeyEvent *>(event);
            if (keyEvent->key() == Qt::Key_Escape) {
                resetHotkeyEdit();
                hotkeyEdit->clearFocus();
                return true;
            }
        }

        return QWidget::eventFilter(object, event);
    }

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
    resize(984, 760);
    setMinimumSize(864, 760);

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
    sideLayout->setContentsMargins(12, 12, 12, 12);
    sideLayout->setSpacing(10);
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

    QBoxLayout *inputGroupLayout = nullptr;
    inputGroupFrame = createSettingsGroup(&inputGroupTitleLabel, sideContent, &inputGroupLayout, true);
    static_cast<CollapsibleSettingsGroup *>(inputGroupFrame)->setCollapseCallback([this]() {
        resetHotkeyEdit();
    });
    auto *inputGroup = inputGroupFrame;
    hotkeyFieldLabel = new QLabel(inputGroup);
    hotkeyFieldLabel->setObjectName("FieldLabel");
    hotkeyEdit = new QKeySequenceEdit(inputGroup);
    hotkeyEdit->setMinimumHeight(34);
    hotkeyEdit->installEventFilter(this);
    connect(hotkeyEdit, &QKeySequenceEdit::keySequenceChanged, this, [this]() {
        updateHotkeyApplyState();
    });

    hotkeyFeedbackTimer = new QTimer(this);
    hotkeyFeedbackTimer->setSingleShot(true);
    connect(hotkeyFeedbackTimer, &QTimer::timeout, this, [this]() {
        updateHotkeyApplyState();
    });

    applyHotkeyButton = new QPushButton(inputGroup);
    applyHotkeyButton->setObjectName("PrimaryButton");
    applyHotkeyButton->setMinimumSize(68, 34);
    applyHotkeyButton->setEnabled(false);
    connect(applyHotkeyButton, &QPushButton::clicked, this, [this]() {
        if (applyHotkeyButton->isEnabled()) {
            emit hotkeyChangeRequested(hotkeyEdit->keySequence());
        }
    });

    hotkeyFeedbackLabel = new QLabel(inputGroup);
    hotkeyFeedbackLabel->setObjectName("HotkeyFeedbackLabel");
    hotkeyFeedbackLabel->setWordWrap(true);

    auto *hotkeyInputRow = new QWidget(inputGroup);
    auto *hotkeyInputLayout = new QHBoxLayout(hotkeyInputRow);
    hotkeyInputLayout->setContentsMargins(0, 0, 0, 0);
    hotkeyInputLayout->setSpacing(6);
    hotkeyInputLayout->addWidget(hotkeyEdit, 1);
    hotkeyInputLayout->addWidget(applyHotkeyButton);

    inputGroupLayout->addWidget(hotkeyFieldLabel);
    inputGroupLayout->addWidget(hotkeyInputRow);
    inputGroupLayout->addWidget(hotkeyFeedbackLabel);

    auto *inputGrid = new QGridLayout();
    inputGrid->setContentsMargins(0, 0, 0, 0);
    inputGrid->setHorizontalSpacing(10);
    inputGrid->setVerticalSpacing(8);

    defaultInputTypeFieldLabel = new QLabel(inputGroup);
    defaultInputTypeFieldLabel->setObjectName("FieldLabel");
    defaultInputTypeCombo = new NoWheelComboBox(inputGroup);
    defaultInputTypeCombo->setObjectName("DefaultInputTypeCombo");
    defaultInputTypeCombo->setCursor(Qt::PointingHandCursor);
    defaultInputTypeCombo->setMinimumHeight(30);

    hideInputAfterSaveCheck = new QCheckBox(inputGroup);
    hideInputAfterSaveCheck->setObjectName("InputToggle");
    hideInputAfterSaveCheck->setMinimumHeight(28);

    inputGrid->addWidget(defaultInputTypeFieldLabel, 0, 0);
    inputGrid->addWidget(defaultInputTypeCombo, 0, 1);
    inputGrid->setColumnStretch(1, 1);
    inputGroupLayout->addLayout(inputGrid);
    inputGroupLayout->addWidget(hideInputAfterSaveCheck);

    connect(defaultInputTypeCombo, &QComboBox::currentIndexChanged, this, [this](int index) {
        const QVariant data = defaultInputTypeCombo->itemData(index);
        if (!data.isValid()) {
            return;
        }
        emit defaultInputTypeChangeRequested(static_cast<DefaultInputTypeMode>(data.toInt()));
    });
    connect(hideInputAfterSaveCheck, &QCheckBox::toggled, this, &DashboardWindow::inputAutoHideChanged);

    QBoxLayout *personalizationGroupLayout = nullptr;
    personalizationGroupFrame = createSettingsGroup(&personalizationGroupTitleLabel,
                                                    sideContent,
                                                    &personalizationGroupLayout);
    auto *personalizationGroup = personalizationGroupFrame;
    auto *personalizationGrid = new QGridLayout();
    personalizationGrid->setContentsMargins(0, 0, 0, 0);
    personalizationGrid->setHorizontalSpacing(10);
    personalizationGrid->setVerticalSpacing(12);

    languageFieldLabel = new QLabel(personalizationGroup);
    languageFieldLabel->setObjectName("FieldLabel");
    themeFieldLabel = new QLabel(personalizationGroup);
    themeFieldLabel->setObjectName("FieldLabel");
    fontSizeFieldLabel = new QLabel(personalizationGroup);
    fontSizeFieldLabel->setObjectName("FieldLabel");
    densityFieldLabel = new QLabel(personalizationGroup);
    densityFieldLabel->setObjectName("FieldLabel");
    memoStartupDisplayFieldLabel = new QLabel(personalizationGroup);
    memoStartupDisplayFieldLabel->setObjectName("FieldLabel");

    languageCombo = new NoWheelComboBox(personalizationGroup);
    languageCombo->setObjectName("LanguageCombo");
    languageCombo->setCursor(Qt::PointingHandCursor);
    languageCombo->setMinimumHeight(34);
    themeCombo = new NoWheelComboBox(personalizationGroup);
    themeCombo->setObjectName("ThemeCombo");
    themeCombo->setCursor(Qt::PointingHandCursor);
    themeCombo->setMinimumHeight(34);
    fontSizeCombo = new NoWheelComboBox(personalizationGroup);
    fontSizeCombo->setObjectName("FontSizeCombo");
    fontSizeCombo->setCursor(Qt::PointingHandCursor);
    fontSizeCombo->setMinimumHeight(34);
    densityCombo = new NoWheelComboBox(personalizationGroup);
    densityCombo->setObjectName("DensityCombo");
    densityCombo->setCursor(Qt::PointingHandCursor);
    densityCombo->setMinimumHeight(34);
    memoStartupDisplayCombo = new NoWheelComboBox(personalizationGroup);
    memoStartupDisplayCombo->setObjectName("MemoStartupDisplayCombo");
    memoStartupDisplayCombo->setCursor(Qt::PointingHandCursor);
    memoStartupDisplayCombo->setMinimumHeight(34);

    personalizationGrid->addWidget(languageFieldLabel, 0, 0);
    personalizationGrid->addWidget(languageCombo, 0, 1);
    personalizationGrid->addWidget(themeFieldLabel, 1, 0);
    personalizationGrid->addWidget(themeCombo, 1, 1);
    personalizationGrid->addWidget(fontSizeFieldLabel, 2, 0);
    personalizationGrid->addWidget(fontSizeCombo, 2, 1);
    personalizationGrid->addWidget(densityFieldLabel, 3, 0);
    personalizationGrid->addWidget(densityCombo, 3, 1);
    personalizationGrid->addWidget(memoStartupDisplayFieldLabel, 4, 0);
    personalizationGrid->addWidget(memoStartupDisplayCombo, 4, 1);
    personalizationGrid->setColumnStretch(1, 1);
    for (int row = 0; row < 5; ++row) {
        personalizationGrid->setRowMinimumHeight(row, 38);
    }
    personalizationGroupLayout->addLayout(personalizationGrid);

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
    connect(memoStartupDisplayCombo, &QComboBox::currentIndexChanged, this, [this](int index) {
        const QVariant data = memoStartupDisplayCombo->itemData(index);
        if (!data.isValid()) {
            return;
        }
        emit memoStartupDisplayChangeRequested(static_cast<MemoStartupDisplayMode>(data.toInt()));
    });

    QBoxLayout *recordGroupLayout = nullptr;
    recordGroupFrame = createSettingsGroup(&recordGroupTitleLabel, sideContent, &recordGroupLayout);
    auto *recordGroup = recordGroupFrame;
    auto *recordGrid = new QGridLayout();
    recordGrid->setContentsMargins(0, 0, 0, 0);
    recordGrid->setHorizontalSpacing(10);
    recordGrid->setVerticalSpacing(8);

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
    dataGroupFrame = createSettingsGroup(&dataGroupTitleLabel, sideContent, &dataGroupLayout);
    auto *dataGroup = dataGroupFrame;
    auto *dataButtonRow = new QWidget(dataGroup);
    auto *dataButtonLayout = new QHBoxLayout(dataButtonRow);
    dataButtonLayout->setContentsMargins(0, 0, 0, 0);
    dataButtonLayout->setSpacing(8);

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

    autostartCheck = new QCheckBox(dataGroup);
    autostartCheck->setObjectName("SystemToggle");
    autostartCheck->setMinimumHeight(28);
    connect(autostartCheck, &QCheckBox::toggled, this, &DashboardWindow::autostartChanged);
    dataGroupLayout->addWidget(autostartCheck);

    sideLayout->addWidget(inputGroup);
    sideLayout->addWidget(personalizationGroup);
    sideLayout->addWidget(recordGroup);
    sideLayout->addWidget(dataGroup);
    sideLayout->addStretch(1);

    refreshGroupHeight(inputGroupFrame);
    refreshGroupHeight(personalizationGroupFrame);
    refreshGroupHeight(recordGroupFrame);
    refreshGroupHeight(dataGroupFrame);

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
    statusLabel->setProperty("statusKind", statusKindName(DashboardStatusKind::Ready));
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

QFrame *DashboardWindow::createSettingsGroup(QLabel **titleLabel,
                                             QWidget *parent,
                                             QBoxLayout **contentLayout,
                                             bool expandedByDefault) const
{
    auto *group = new CollapsibleSettingsGroup(parent, expandedByDefault);

    if (titleLabel != nullptr) {
        *titleLabel = group->titleLabel();
    }

    if (contentLayout != nullptr) {
        *contentLayout = group->contentLayout();
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

    autostartCheck->setText(AppText::autostart(language));
    autostartCheck->setToolTip(AppText::autostartTooltip(language));
    questionTopCheck->setText(AppText::pin(language));
    questionTopCheck->setToolTip(AppText::pinTooltip(language));
    todoTopCheck->setText(AppText::pin(language));
    todoTopCheck->setToolTip(AppText::pinTooltip(language));

    rebuildComboLabels();
    updateSettingsGroupSummaries();
    refreshGroupHeight(inputGroupFrame);
    refreshGroupHeight(personalizationGroupFrame);
    refreshGroupHeight(recordGroupFrame);
    refreshGroupHeight(dataGroupFrame);
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

void DashboardWindow::updateSettingsGroupSummaries()
{
    const AppLanguage language = store->language();
    const QString questionName = store->categoryName(MemoType::Question);
    const QString todoName = store->categoryName(MemoType::Todo);

    setGroupSummary(personalizationGroupFrame,
                    QStringLiteral("%1 / %2 / %3")
                        .arg(AppText::languageDisplayName(language),
                             AppText::themeDisplayName(store->themeMode(), language),
                             AppText::memoStartupDisplayName(store->memoStartupDisplayMode(), language)));
    setGroupSummary(inputGroupFrame,
                    QStringLiteral("%1 / %2")
                        .arg(QKeySequence(store->hotkey()).toString(QKeySequence::NativeText),
                             AppText::defaultInputTypeName(store->defaultInputTypeMode(),
                                                           questionName,
                                                           todoName,
                                                           language)));
    setGroupSummary(recordGroupFrame,
                    QStringLiteral("%1 / %2")
                        .arg(AppText::recordClickActionName(store->recordClickAction(), language),
                             AppText::recordSortOrderName(store->recordSortOrder(), language)));
    setGroupSummary(dataGroupFrame,
                    QStringLiteral("%1 / %2 / %3")
                        .arg(AppText::exportJson(language),
                             AppText::importJson(language),
                             onOffText(store->autostartEnabled(), language)));
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
