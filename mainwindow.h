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
class QWidget;

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
    void syncLanguage();
    void applyAppearance(bool animate);
    void playThemeTransition(QWidget *widget);
    bool applyAutostart(bool enabled);
    MemoWindow *memoWindow(MemoType type) const;
    void quitApplication();

    MemoStore *store;
    InputWindow *inputWindow;
    MemoWindow *questionWindow;
    MemoWindow *todoWindow;
    DashboardWindow *dashboardWindow;
    HotkeyManager *hotkeyManager;
    TrayController *trayController;
    ThemeMode appliedTheme;
    FontSizeMode appliedFontSize;
    DensityMode appliedDensity;
};

#endif // MAINWINDOW_H
