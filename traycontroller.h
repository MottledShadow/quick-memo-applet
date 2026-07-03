#ifndef TRAYCONTROLLER_H
#define TRAYCONTROLLER_H

#include <QObject>

class QAction;
class QMenu;
class QSystemTrayIcon;

class TrayController : public QObject
{
    Q_OBJECT

public:
    explicit TrayController(QObject *parent = nullptr);
    ~TrayController() override;

signals:
    void openDashboardRequested();

private:
    QSystemTrayIcon *trayIcon;
    QMenu *trayMenu;
    QAction *openDashboardAction;
};

#endif // TRAYCONTROLLER_H
