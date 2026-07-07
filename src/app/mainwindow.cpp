#include "mainwindow.h"

#include "platform/hotkeymanager.h"
#include "theme/apptext.h"
#include "theme/apptheme.h"
#include "ui/dashboardwindow.h"
#include "ui/inputwindow.h"
#include "ui/memowindow.h"
#include "ui/traycontroller.h"

#include <QAbstractAnimation>
#include <QApplication>
#include <QCoreApplication>
#include <QDir>
#include <QEasingCurve>
#include <QFileDialog>
#include <QFileInfo>
#include <QKeySequence>
#include <QMessageBox>
#include <QPropertyAnimation>
#include <QPushButton>
#include <QSettings>
#include <QTimer>
#include <QWidget>

namespace {
using StatusKind = DashboardWindow::DashboardStatusKind;

constexpr int kThemeTransitionDurationMs = 180;
constexpr qreal kThemeTransitionStartOpacity = 0.82;
constexpr qreal kThemeTransitionMinimumIdleOpacity = 0.99;
constexpr int kStartupMessageDelayMs = 700;
}

MainWindow::MainWindow(QObject *parent)
    : QObject(parent)
    , store(new MemoStore(this))
    , inputWindow(nullptr)
    , questionWindow(nullptr)
    , todoWindow(nullptr)
    , dashboardWindow(nullptr)
    , hotkeyManager(new HotkeyManager(this))
    , trayController(new TrayController(this))
    , appliedTheme(ThemeMode::System)
    , appliedFontSize(FontSizeMode::Default)
    , appliedDensity(DensityMode::Comfortable)
{
    store->load();

    inputWindow = new InputWindow();
    questionWindow = new MemoWindow(MemoType::Question);
    todoWindow = new MemoWindow(MemoType::Todo);
    dashboardWindow = new DashboardWindow(store);

    questionWindow->setRecords(store->records(MemoType::Question));
    todoWindow->setRecords(store->records(MemoType::Todo));
    inputWindow->setCurrentType(store->currentType());
    inputWindow->setHideAfterSave(store->hideInputAfterSave());
    syncLanguage();
    syncRecordClickAction();
    applyAppearance(false);

    restoreWindows();
    setupConnections();
    dashboardWindow->refresh();
    registerStoredHotkey();
    applyAutostart(store->autostartEnabled());

    const bool launchedFromAutostart = QCoreApplication::arguments().contains(QStringLiteral("--autostart"));
    if (!launchedFromAutostart && store->firstRunCreated()) {
        QTimer::singleShot(kStartupMessageDelayMs, trayController, [this]() {
            trayController->showStartupMessage(QKeySequence(store->hotkey()).toString(QKeySequence::NativeText));
        });
    }
}

MainWindow::~MainWindow()
{
    delete dashboardWindow;
    delete todoWindow;
    delete questionWindow;
    delete inputWindow;
}

void MainWindow::setupConnections()
{
    connect(store, &MemoStore::recordsChanged, this, [this]() {
        refreshRecordViews();
    });

    connect(store, &MemoStore::settingsChanged, this, [this]() {
        const bool appearanceChanged = store->themeMode() != appliedTheme
                                       || store->fontSizeMode() != appliedFontSize
                                       || store->densityMode() != appliedDensity;
        inputWindow->setCurrentType(store->currentType());
        inputWindow->setHideAfterSave(store->hideInputAfterSave());
        syncLanguage();
        syncRecordClickAction();
        if (appearanceChanged) {
            applyAppearance(true);
        }
    });

    connect(questionWindow, &MemoWindow::memoClicked, this, [this](const QString &id) {
        handleRecordClick(id, MemoType::Question, false);
    });
    connect(todoWindow, &MemoWindow::memoClicked, this, [this](const QString &id) {
        handleRecordClick(id, MemoType::Todo, false);
    });
    connect(questionWindow, &MemoWindow::categoryNameChangeRequested, this, [this](MemoType type, const QString &name) {
        store->setCategoryName(type, name);
        dashboardWindow->setStatusMessage(AppText::categoryNameChanged(store->categoryName(type), store->language()),
                                          StatusKind::Success);
    });
    connect(todoWindow, &MemoWindow::categoryNameChangeRequested, this, [this](MemoType type, const QString &name) {
        store->setCategoryName(type, name);
        dashboardWindow->setStatusMessage(AppText::categoryNameChanged(store->categoryName(type), store->language()),
                                          StatusKind::Success);
    });

    connect(questionWindow, &MemoWindow::stateChanged, store, &MemoStore::setWindowState);
    connect(todoWindow, &MemoWindow::stateChanged, store, &MemoStore::setWindowState);

    connect(inputWindow, &InputWindow::memoSubmitted, store, &MemoStore::addMemo);
    connect(inputWindow, &InputWindow::currentTypeChanged, store, &MemoStore::setCurrentType);

    connect(hotkeyManager, &HotkeyManager::activated, this, [this]() {
        if (inputWindow->isOpenForCapture()) {
            inputWindow->toggleCurrentType();
        } else {
            applyDefaultInputType();
        }
        inputWindow->showAndFocus();
    });

    connect(trayController, &TrayController::openDashboardRequested, this, [this]() {
        dashboardWindow->show();
        dashboardWindow->raise();
        dashboardWindow->activateWindow();
    });
    connect(trayController, &TrayController::exitRequested, this, &MainWindow::quitApplication);

    connect(dashboardWindow, &DashboardWindow::showMemoRequested, this, [this](MemoType type) {
        MemoWindow *window = memoWindow(type);
        window->show();
        window->raise();
        window->activateWindow();
    });

    connect(dashboardWindow, &DashboardWindow::hideMemoRequested, this, [this](MemoType type) {
        memoWindow(type)->hide();
    });

    connect(dashboardWindow, &DashboardWindow::recordClickRequested, this, [this](const QString &id, MemoType type) {
        handleRecordClick(id, type, true);
    }, Qt::QueuedConnection);

    connect(dashboardWindow, &DashboardWindow::alwaysOnTopChanged, this, [this](MemoType type, bool enabled) {
        memoWindow(type)->setAlwaysOnTop(enabled);
    });

    connect(dashboardWindow, &DashboardWindow::hotkeyChangeRequested, this, [this](const QKeySequence &sequence) {
        const QString oldHotkey = store->hotkey();
        QString error;
        if (!hotkeyManager->registerHotkey(sequence, &error)) {
            hotkeyManager->registerHotkey(QKeySequence(oldHotkey));
            dashboardWindow->setHotkeyChangeFailed(error);
            dashboardWindow->setStatusMessage(error, StatusKind::Error);
            return;
        }

        store->setHotkey(sequence.toString(QKeySequence::PortableText));
        const QString message = AppText::hotkeyUpdated(sequence.toString(QKeySequence::NativeText),
                                                       store->language());
        dashboardWindow->setHotkeyChangeSucceeded(sequence);
        dashboardWindow->setStatusMessage(message, StatusKind::Success);
    });

    connect(dashboardWindow, &DashboardWindow::themeChangeRequested, this, [this](ThemeMode mode) {
        store->setThemeMode(mode);
        dashboardWindow->setStatusMessage(AppText::themeChanged(mode, store->language()), StatusKind::Success);
    });

    connect(dashboardWindow, &DashboardWindow::languageChangeRequested, this, [this](AppLanguage language) {
        store->setLanguage(language);
        dashboardWindow->setStatusMessage(AppText::ready(store->language()), StatusKind::Ready);
    });

    connect(dashboardWindow, &DashboardWindow::fontSizeChangeRequested, this, [this](FontSizeMode mode) {
        store->setFontSizeMode(mode);
        dashboardWindow->setStatusMessage(AppText::fontSizeChanged(mode, store->language()), StatusKind::Success);
    });

    connect(dashboardWindow, &DashboardWindow::densityChangeRequested, this, [this](DensityMode mode) {
        store->setDensityMode(mode);
        dashboardWindow->setStatusMessage(AppText::densityChanged(mode, store->language()), StatusKind::Success);
    });

    connect(dashboardWindow, &DashboardWindow::memoStartupDisplayChangeRequested, this, [this](MemoStartupDisplayMode mode) {
        store->setMemoStartupDisplayMode(mode);
        dashboardWindow->setStatusMessage(AppText::memoStartupDisplayChanged(mode, store->language()),
                                          StatusKind::Success);
    });

    connect(dashboardWindow, &DashboardWindow::defaultInputTypeChangeRequested, this, [this](DefaultInputTypeMode mode) {
        store->setDefaultInputTypeMode(mode);
        dashboardWindow->setStatusMessage(AppText::defaultInputTypeChanged(mode,
                                                                           store->categoryName(MemoType::Question),
                                                                           store->categoryName(MemoType::Todo),
                                                                           store->language()),
                                          StatusKind::Success);
    });

    connect(dashboardWindow, &DashboardWindow::recordClickActionChangeRequested, this, [this](RecordClickAction action) {
        store->setRecordClickAction(action);
        dashboardWindow->setStatusMessage(AppText::recordClickActionChanged(action, store->language()),
                                          StatusKind::Success);
    });

    connect(dashboardWindow, &DashboardWindow::recordSortOrderChangeRequested, this, [this](RecordSortOrder order) {
        store->setRecordSortOrder(order);
        dashboardWindow->setStatusMessage(AppText::recordSortOrderChanged(order, store->language()),
                                          StatusKind::Success);
    });

    connect(dashboardWindow, &DashboardWindow::exportJsonRequested, this, &MainWindow::exportJson);
    connect(dashboardWindow, &DashboardWindow::importJsonRequested, this, &MainWindow::importJson);

    connect(dashboardWindow, &DashboardWindow::inputAutoHideChanged, this, [this](bool enabled) {
        store->setHideInputAfterSave(enabled);
        dashboardWindow->setStatusMessage(enabled ? AppText::inputAutoHideEnabled(store->language())
                                                  : AppText::inputAutoHideDisabled(store->language()),
                                          StatusKind::Success);
    });

    connect(dashboardWindow, &DashboardWindow::autostartChanged, this, [this](bool enabled) {
        if (!applyAutostart(enabled)) {
            dashboardWindow->setStatusMessage(AppText::autostartFailed(store->language()), StatusKind::Error);
            dashboardWindow->refresh();
            return;
        }

        store->setAutostartEnabled(enabled);
        dashboardWindow->setStatusMessage(enabled ? AppText::autostartEnabled(store->language())
                                                  : AppText::autostartDisabled(store->language()),
                                          StatusKind::Success);
    });

}

void MainWindow::restoreWindows()
{
    MemoWindowState questionState = store->windowState(MemoType::Question);
    MemoWindowState todoState = store->windowState(MemoType::Todo);

    switch (store->memoStartupDisplayMode()) {
    case MemoStartupDisplayMode::ShowAll:
        questionState.visible = true;
        todoState.visible = true;
        break;
    case MemoStartupDisplayMode::HideAll:
        questionState.visible = false;
        todoState.visible = false;
        break;
    case MemoStartupDisplayMode::Restore:
        break;
    }

    store->setWindowState(MemoType::Question, questionState);
    store->setWindowState(MemoType::Todo, todoState);
    questionWindow->restoreState(questionState);
    todoWindow->restoreState(todoState);
}

void MainWindow::registerStoredHotkey()
{
    QString error;
    if (!hotkeyManager->registerHotkey(QKeySequence(store->hotkey()), &error)) {
        dashboardWindow->setStatusMessage(error, StatusKind::Error);
    }
}

void MainWindow::syncLanguage()
{
    const AppLanguage language = store->language();
    inputWindow->setLanguage(language);
    inputWindow->setCategoryName(MemoType::Question, store->categoryName(MemoType::Question));
    inputWindow->setCategoryName(MemoType::Todo, store->categoryName(MemoType::Todo));
    questionWindow->setLanguage(language);
    questionWindow->setCategoryName(store->categoryName(MemoType::Question));
    todoWindow->setLanguage(language);
    todoWindow->setCategoryName(store->categoryName(MemoType::Todo));
    hotkeyManager->setLanguage(language);
    trayController->setLanguage(language);
}

void MainWindow::syncRecordClickAction()
{
    questionWindow->setRecordClickAction(store->recordClickAction());
    todoWindow->setRecordClickAction(store->recordClickAction());
}

void MainWindow::refreshRecordViews()
{
    questionWindow->setRecords(store->records(MemoType::Question));
    todoWindow->setRecords(store->records(MemoType::Todo));
}

void MainWindow::applyDefaultInputType()
{
    switch (store->defaultInputTypeMode()) {
    case DefaultInputTypeMode::Question:
        inputWindow->setCurrentType(MemoType::Question);
        break;
    case DefaultInputTypeMode::Todo:
        inputWindow->setCurrentType(MemoType::Todo);
        break;
    case DefaultInputTypeMode::LastUsed:
        inputWindow->setCurrentType(store->currentType());
        break;
    }
}

void MainWindow::handleRecordClick(const QString &id, MemoType type, bool fromDashboard)
{
    const RecordClickAction action = store->recordClickAction();
    if (action == RecordClickAction::DashboardOnly && !fromDashboard) {
        return;
    }

    if (action == RecordClickAction::Complete) {
        store->completeMemo(id);
        dashboardWindow->setStatusMessage(AppText::completedRecord(store->categoryName(type), store->language()),
                                          StatusKind::Success);
        return;
    }

    store->deleteMemo(id);
    dashboardWindow->setStatusMessage(AppText::deletedRecord(store->categoryName(type), store->language()),
                                      StatusKind::Success);
}

void MainWindow::exportJson()
{
    QString filePath = QFileDialog::getSaveFileName(dashboardWindow,
                                                    AppText::exportJson(store->language()),
                                                    QDir::home().filePath("quick-memo-export.json"),
                                                    QStringLiteral("JSON (*.json);;All Files (*)"));
    if (filePath.isEmpty()) {
        return;
    }

    if (!filePath.endsWith(QStringLiteral(".json"), Qt::CaseInsensitive)) {
        filePath.append(QStringLiteral(".json"));
    }

    QString error;
    if (!store->exportToFile(filePath, &error)) {
        dashboardWindow->setStatusMessage(AppText::exportJsonFailed(error, store->language()), StatusKind::Error);
        return;
    }

    dashboardWindow->setStatusMessage(AppText::exportJsonSuccess(QFileInfo(filePath).fileName(), store->language()),
                                      StatusKind::Success);
}

void MainWindow::importJson()
{
    const QString filePath = QFileDialog::getOpenFileName(dashboardWindow,
                                                          AppText::importJson(store->language()),
                                                          QDir::homePath(),
                                                          QStringLiteral("JSON (*.json);;All Files (*)"));
    if (filePath.isEmpty()) {
        return;
    }

    const QFileInfo importFile(filePath);
    QMessageBox confirm(QMessageBox::Warning,
                        AppText::importJsonConfirmTitle(store->language()),
                        AppText::importJsonConfirmBody(importFile.fileName(), store->language()),
                        QMessageBox::Cancel,
                        dashboardWindow);
    QPushButton *importButton = confirm.addButton(AppText::importJsonConfirmButton(store->language()),
                                                  QMessageBox::AcceptRole);
    confirm.setDefaultButton(QMessageBox::Cancel);
    confirm.exec();
    if (confirm.clickedButton() != importButton) {
        return;
    }

    QString error;
    if (!store->importFromFile(filePath, &error)) {
        dashboardWindow->setStatusMessage(AppText::importJsonFailed(error, store->language()), StatusKind::Error);
        return;
    }

    inputWindow->setCurrentType(store->currentType());
    inputWindow->setHideAfterSave(store->hideInputAfterSave());
    refreshRecordViews();
    syncLanguage();
    syncRecordClickAction();
    applyAppearance(false);
    restoreWindows();
    registerStoredHotkey();
    applyAutostart(store->autostartEnabled());
    dashboardWindow->refresh();
    dashboardWindow->setStatusMessage(AppText::importJsonSuccess(store->language()), StatusKind::Success);
}

void MainWindow::applyAppearance(bool animate)
{
    const ThemeMode mode = store->themeMode();
    const FontSizeMode fontSize = store->fontSizeMode();
    const DensityMode density = store->densityMode();

    qApp->setStyleSheet(AppTheme::applicationStyleSheet(mode, fontSize, density));
    inputWindow->applyTheme(mode, fontSize, density);
    questionWindow->applyTheme(mode, fontSize, density);
    todoWindow->applyTheme(mode, fontSize, density);
    dashboardWindow->applyTheme(mode, fontSize, density);
    appliedTheme = mode;
    appliedFontSize = fontSize;
    appliedDensity = density;

    if (!animate) {
        return;
    }

    playThemeTransition(inputWindow);
    playThemeTransition(questionWindow);
    playThemeTransition(todoWindow);
    playThemeTransition(dashboardWindow);
}

void MainWindow::playThemeTransition(QWidget *widget)
{
    if (widget == nullptr || !widget->isVisible()) {
        return;
    }

    if (widget->windowOpacity() < kThemeTransitionMinimumIdleOpacity) {
        return;
    }

    widget->setWindowOpacity(kThemeTransitionStartOpacity);

    auto *animation = new QPropertyAnimation(widget, "windowOpacity", widget);
    animation->setDuration(kThemeTransitionDurationMs);
    animation->setEasingCurve(QEasingCurve::OutCubic);
    animation->setStartValue(kThemeTransitionStartOpacity);
    animation->setEndValue(1.0);
    animation->start(QAbstractAnimation::DeleteWhenStopped);
}

bool MainWindow::applyAutostart(bool enabled)
{
#ifdef Q_OS_WIN
    QSettings runKey(QStringLiteral("HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Run"),
                     QSettings::NativeFormat);
    const QString appName = QStringLiteral("QuickMemoApplet");
    if (enabled) {
        const QString executable = QDir::toNativeSeparators(QCoreApplication::applicationFilePath());
        runKey.setValue(appName, QStringLiteral("\"%1\" --autostart").arg(executable));
    } else {
        runKey.remove(appName);
    }
    return runKey.status() == QSettings::NoError;
#else
    Q_UNUSED(enabled)
    return false;
#endif
}

MemoWindow *MainWindow::memoWindow(MemoType type) const
{
    return type == MemoType::Question ? questionWindow : todoWindow;
}

void MainWindow::quitApplication()
{
    store->setWindowState(MemoType::Question, questionWindow->currentState());
    store->setWindowState(MemoType::Todo, todoWindow->currentState());
    store->save();
    questionWindow->setCloseAllowed(true);
    todoWindow->setCloseAllowed(true);
    dashboardWindow->setCloseAllowed(true);
    qApp->quit();
}
