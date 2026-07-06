#include "mainwindow.h"

#include "apptheme.h"
#include "dashboardwindow.h"
#include "hotkeymanager.h"
#include "inputwindow.h"
#include "memowindow.h"
#include "traycontroller.h"

#include <QAbstractAnimation>
#include <QApplication>
#include <QCoreApplication>
#include <QDir>
#include <QEasingCurve>
#include <QKeySequence>
#include <QPropertyAnimation>
#include <QSettings>
#include <QTimer>
#include <QWidget>

namespace {
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
    , appliedTheme(ThemeMode::Light)
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
    applyTheme(store->themeMode(), false);

    restoreWindows();
    setupConnections();
    registerStoredHotkey();
    applyAutostart(store->autostartEnabled());

    const bool launchedFromAutostart = QCoreApplication::arguments().contains(QStringLiteral("--autostart"));
    if (!launchedFromAutostart) {
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
        questionWindow->setRecords(store->records(MemoType::Question));
        todoWindow->setRecords(store->records(MemoType::Todo));
    });

    connect(store, &MemoStore::settingsChanged, this, [this]() {
        const ThemeMode mode = store->themeMode();
        const bool themeChanged = mode != appliedTheme;
        inputWindow->setCurrentType(store->currentType());
        inputWindow->setHideAfterSave(store->hideInputAfterSave());
        if (themeChanged) {
            applyTheme(mode, true);
        }
    });

    connect(questionWindow, &MemoWindow::memoClicked, store, &MemoStore::deleteMemo);
    connect(todoWindow, &MemoWindow::memoClicked, store, &MemoStore::deleteMemo);

    connect(questionWindow, &MemoWindow::stateChanged, store, &MemoStore::setWindowState);
    connect(todoWindow, &MemoWindow::stateChanged, store, &MemoStore::setWindowState);

    connect(inputWindow, &InputWindow::memoSubmitted, store, &MemoStore::addMemo);
    connect(inputWindow, &InputWindow::currentTypeChanged, store, &MemoStore::setCurrentType);

    connect(hotkeyManager, &HotkeyManager::activated, this, [this]() {
        if (inputWindow->isOpenForCapture()) {
            inputWindow->toggleCurrentType();
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

    connect(dashboardWindow, &DashboardWindow::recordDeleteRequested, this, [this](const QString &id, MemoType type) {
        Q_UNUSED(type)
        store->deleteMemo(id);
    }, Qt::QueuedConnection);

    connect(dashboardWindow, &DashboardWindow::alwaysOnTopChanged, this, [this](MemoType type, bool enabled) {
        memoWindow(type)->setAlwaysOnTop(enabled);
    });

    connect(dashboardWindow, &DashboardWindow::hotkeyChangeRequested, this, [this](const QKeySequence &sequence) {
        const QString oldHotkey = store->hotkey();
        QString error;
        if (!hotkeyManager->registerHotkey(sequence, &error)) {
            hotkeyManager->registerHotkey(QKeySequence(oldHotkey));
            dashboardWindow->setStatusMessage(error);
            return;
        }

        store->setHotkey(sequence.toString(QKeySequence::PortableText));
        dashboardWindow->setStatusMessage(QStringLiteral("快捷键已更新：%1")
                                              .arg(sequence.toString(QKeySequence::NativeText)));
    });

    connect(dashboardWindow, &DashboardWindow::themeChangeRequested, this, [this](ThemeMode mode) {
        store->setThemeMode(mode);
        dashboardWindow->setStatusMessage(QStringLiteral("主题已切换为：%1")
                                              .arg(MemoStore::themeDisplayName(mode)));
    });

    connect(dashboardWindow, &DashboardWindow::inputAutoHideChanged, this, [this](bool enabled) {
        store->setHideInputAfterSave(enabled);
        dashboardWindow->setStatusMessage(enabled ? QStringLiteral("保存后将自动收起输入框。")
                                                  : QStringLiteral("保存后将继续输入。"));
    });

    connect(dashboardWindow, &DashboardWindow::autostartChanged, this, [this](bool enabled) {
        if (!applyAutostart(enabled)) {
            dashboardWindow->setStatusMessage(QStringLiteral("开机自启设置失败。"));
            dashboardWindow->refresh();
            return;
        }

        store->setAutostartEnabled(enabled);
        dashboardWindow->setStatusMessage(enabled ? QStringLiteral("已开启开机自启。")
                                                  : QStringLiteral("已关闭开机自启。"));
    });

    connect(dashboardWindow, &DashboardWindow::exitRequested, this, &MainWindow::quitApplication);
}

void MainWindow::restoreWindows()
{
    questionWindow->restoreState(store->windowState(MemoType::Question));
    todoWindow->restoreState(store->windowState(MemoType::Todo));
}

void MainWindow::registerStoredHotkey()
{
    QString error;
    if (!hotkeyManager->registerHotkey(QKeySequence(store->hotkey()), &error)) {
        dashboardWindow->setStatusMessage(error);
    }
}

void MainWindow::applyTheme(ThemeMode mode, bool animate)
{
    qApp->setStyleSheet(AppTheme::applicationStyleSheet(mode));
    inputWindow->applyTheme(mode);
    questionWindow->applyTheme(mode);
    todoWindow->applyTheme(mode);
    dashboardWindow->applyTheme(mode);
    appliedTheme = mode;

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
    qApp->quit();
}
