#ifndef TRAYCONTROLLER_H
#define TRAYCONTROLLER_H

#include <QObject>

class QAction;
class QMenu;
class QString;
class QSystemTrayIcon;
class QWidget;

class TrayController : public QObject
{
    Q_OBJECT

public:
    explicit TrayController(QObject *parent = nullptr);
    ~TrayController() override;
    void showStartupMessage(const QString &hotkeyText);

signals:
    void openDashboardRequested();
    void exitRequested();

private:
    QSystemTrayIcon *trayIcon;
    QMenu *trayMenu;
    QAction *openDashboardAction;
    QAction *exitAction;
    QWidget *startupToast;
};

#endif // TRAYCONTROLLER_H
