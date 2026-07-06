#include "traycontroller.h"

#include <QAction>
#include <QApplication>
#include <QMenu>
#include <QString>
#include <QStyle>
#include <QSystemTrayIcon>

TrayController::TrayController(QObject *parent)
    : QObject(parent)
    , trayIcon(new QSystemTrayIcon(this))
    , trayMenu(new QMenu())
    , openDashboardAction(new QAction(QStringLiteral("打开后台"), this))
    , exitAction(new QAction(QStringLiteral("退出程序"), this))
{
    trayIcon->setIcon(qApp->style()->standardIcon(QStyle::SP_FileDialogDetailedView));
    trayIcon->setToolTip(QStringLiteral("Quick Memo Applet"));

    trayMenu->addAction(openDashboardAction);
    trayMenu->addSeparator();
    trayMenu->addAction(exitAction);
    trayIcon->setContextMenu(trayMenu);

    connect(openDashboardAction, &QAction::triggered, this, &TrayController::openDashboardRequested);
    connect(exitAction, &QAction::triggered, this, &TrayController::exitRequested);
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

void TrayController::showStartupMessage(const QString &hotkeyText)
{
    if (!QSystemTrayIcon::supportsMessages()) {
        return;
    }

    trayIcon->showMessage(QStringLiteral("Quick Memo 已在后台运行"),
                          QStringLiteral("按 %1 呼出输入框，点击托盘图标可打开后台。").arg(hotkeyText),
                          QSystemTrayIcon::Information,
                          4200);
}
