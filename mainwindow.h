#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "memostore.h"

#include <QObject>
#include <QKeySequence>

class DashboardWindow;
class HotkeyManager;
class InputWindow;
class MemoWindow;
class TrayController;

class MainWindow : public QObject
{
    Q_OBJECT

public:
    explicit MainWindow(QObject *parent = nullptr);
    ~MainWindow() override;

private:
    void setupConnections();
    void restoreWindows();
    void registerStoredHotkey();
    void applyTheme(ThemeMode mode);
    bool applyAutostart(bool enabled);
    MemoWindow *memoWindow(MemoType type) const;

    MemoStore *store;
    InputWindow *inputWindow;
    MemoWindow *questionWindow;
    MemoWindow *todoWindow;
    DashboardWindow *dashboardWindow;
    HotkeyManager *hotkeyManager;
    TrayController *trayController;
};

#endif // MAINWINDOW_H
