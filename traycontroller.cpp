#include "traycontroller.h"

#include <QAction>
#include <QApplication>
#include <QMenu>
#include <QStyle>
#include <QSystemTrayIcon>

TrayController::TrayController(QObject *parent)
    : QObject(parent)
    , trayIcon(new QSystemTrayIcon(this))
    , trayMenu(new QMenu())
    , openDashboardAction(new QAction(QStringLiteral("打开后台"), this))
{
    trayIcon->setIcon(qApp->style()->standardIcon(QStyle::SP_FileDialogDetailedView));
    trayIcon->setToolTip(QStringLiteral("Quick Memo Applet"));

    trayMenu->addAction(openDashboardAction);
    trayIcon->setContextMenu(trayMenu);

    connect(openDashboardAction, &QAction::triggered, this, &TrayController::openDashboardRequested);
    connect(trayIcon, &QSystemTrayIcon::activated, this, [this](QSystemTrayIcon::ActivationReason reason) {
        if (reason == QSystemTrayIcon::DoubleClick || reason == QSystemTrayIcon::Trigger) {
            emit openDashboardRequested();
        }
    });

    trayIcon->show();
}

TrayController::~TrayController()
{
    trayIcon->hide();
    delete trayMenu;
}
