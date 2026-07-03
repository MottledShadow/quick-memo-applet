#include "mainwindow.h"

#include "dashboardwindow.h"
#include "hotkeymanager.h"
#include "inputwindow.h"
#include "memowindow.h"
#include "traycontroller.h"

#include <QApplication>
#include <QCoreApplication>
#include <QDir>
#include <QKeySequence>
#include <QSettings>

MainWindow::MainWindow(QObject *parent)
    : QObject(parent)
    , store(new MemoStore(this))
    , inputWindow(nullptr)
    , questionWindow(nullptr)
    , todoWindow(nullptr)
    , dashboardWindow(nullptr)
    , hotkeyManager(new HotkeyManager(this))
    , trayController(new TrayController(this))
{
    store->load();

    inputWindow = new InputWindow();
    questionWindow = new MemoWindow(MemoType::Question);
    todoWindow = new MemoWindow(MemoType::Todo);
    dashboardWindow = new DashboardWindow(store);

    questionWindow->setRecords(store->records(MemoType::Question));
    todoWindow->setRecords(store->records(MemoType::Todo));
    inputWindow->setCurrentType(store->currentType());

    restoreWindows();
    setupConnections();
    registerStoredHotkey();
    applyAutostart(store->autostartEnabled());
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
        inputWindow->setCurrentType(store->currentType());
    });

    connect(questionWindow, &MemoWindow::memoClicked, store, &MemoStore::deleteMemo);
    connect(todoWindow, &MemoWindow::memoClicked, store, &MemoStore::deleteMemo);

    connect(questionWindow, &MemoWindow::stateChanged, store, &MemoStore::setWindowState);
    connect(todoWindow, &MemoWindow::stateChanged, store, &MemoStore::setWindowState);

    connect(inputWindow, &InputWindow::memoSubmitted, store, &MemoStore::addMemo);
    connect(inputWindow, &InputWindow::currentTypeChanged, store, &MemoStore::setCurrentType);

    connect(hotkeyManager, &HotkeyManager::activated, this, [this]() {
        if (inputWindow->isVisible()) {
            inputWindow->toggleCurrentType();
        }
        inputWindow->showAndFocus();
    });
    connect(trayController, &TrayController::openDashboardRequested, this, [this]() {
        dashboardWindow->show();
        dashboardWindow->raise();
        dashboardWindow->activateWindow();
    });

    connect(dashboardWindow, &DashboardWindow::showMemoRequested, this, [this](MemoType type) {
        MemoWindow *window = memoWindow(type);
        window->show();
        window->raise();
        window->activateWindow();
    });

    connect(dashboardWindow, &DashboardWindow::hideMemoRequested, this, [this](MemoType type) {
        memoWindow(type)->hide();
    });

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

    connect(dashboardWindow, &DashboardWindow::exitRequested, this, [this]() {
        store->setWindowState(MemoType::Question, questionWindow->currentState());
        store->setWindowState(MemoType::Todo, todoWindow->currentState());
        store->save();
        qApp->quit();
    });
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

bool MainWindow::applyAutostart(bool enabled)
{
#ifdef Q_OS_WIN
    QSettings runKey(QStringLiteral("HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Run"),
                     QSettings::NativeFormat);
    const QString appName = QStringLiteral("QuickMemoApplet");
    if (enabled) {
        const QString executable = QDir::toNativeSeparators(QCoreApplication::applicationFilePath());
        runKey.setValue(appName, QStringLiteral("\"%1\"").arg(executable));
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
