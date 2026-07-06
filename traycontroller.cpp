#include "traycontroller.h"

#include "apptext.h"

#include <QAction>
#include <QApplication>
#include <QFrame>
#include <QGuiApplication>
#include <QLabel>
#include <QMenu>
#include <QScreen>
#include <QString>
#include <QStyle>
#include <QSystemTrayIcon>
#include <QTimer>
#include <QVBoxLayout>

TrayController::TrayController(QObject *parent)
    : QObject(parent)
    , trayIcon(new QSystemTrayIcon(this))
    , trayMenu(new QMenu())
    , openDashboardAction(new QAction(this))
    , exitAction(new QAction(this))
    , startupToast(nullptr)
    , appLanguage(AppLanguage::ZhCn)
{
    trayIcon->setIcon(qApp->style()->standardIcon(QStyle::SP_FileDialogDetailedView));
    trayIcon->setToolTip(QStringLiteral("Quick Memo Applet"));

    trayMenu->addAction(openDashboardAction);
    trayMenu->addSeparator();
    trayMenu->addAction(exitAction);
    trayIcon->setContextMenu(trayMenu);
    setLanguage(appLanguage);

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
    if (startupToast != nullptr) {
        startupToast->close();
    }
    trayIcon->hide();
    delete trayMenu;
}

void TrayController::setLanguage(AppLanguage language)
{
    appLanguage = language;
    openDashboardAction->setText(AppText::openDashboard(appLanguage));
    exitAction->setText(AppText::exitApplication(appLanguage));
}

void TrayController::showStartupMessage(const QString &hotkeyText)
{
    if (startupToast != nullptr) {
        startupToast->close();
        startupToast = nullptr;
    }

    startupToast = new QFrame();
    startupToast->setObjectName("StartupToast");
    startupToast->setWindowFlags(Qt::Tool | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    startupToast->setAttribute(Qt::WA_DeleteOnClose, true);
    startupToast->setStyleSheet(QStringLiteral(R"(
QFrame#StartupToast {
    color: #111827;
    background: #FFFFFF;
    border: 1px solid #CBD5E1;
    border-radius: 8px;
}
QLabel#StartupToastTitle {
    color: #111827;
    font-family: "Segoe UI", "Microsoft YaHei UI", "Microsoft YaHei", sans-serif;
    font-size: 14px;
    font-weight: 600;
}
QLabel#StartupToastBody {
    color: #4B5563;
    font-family: "Segoe UI", "Microsoft YaHei UI", "Microsoft YaHei", sans-serif;
    font-size: 12px;
    line-height: 16px;
}
)"));

    auto *layout = new QVBoxLayout(startupToast);
    layout->setContentsMargins(16, 12, 16, 12);
    layout->setSpacing(6);

    auto *titleLabel = new QLabel(AppText::startupTitle(appLanguage), startupToast);
    titleLabel->setObjectName("StartupToastTitle");

    auto *bodyLabel = new QLabel(AppText::startupBody(hotkeyText, appLanguage), startupToast);
    bodyLabel->setObjectName("StartupToastBody");
    bodyLabel->setWordWrap(true);

    layout->addWidget(titleLabel);
    layout->addWidget(bodyLabel);

    startupToast->setFixedWidth(280);
    startupToast->adjustSize();

    const QScreen *screen = QGuiApplication::primaryScreen();
    if (screen != nullptr) {
        const QRect area = screen->availableGeometry();
        const QPoint position(area.right() - startupToast->width() - 24,
                              area.bottom() - startupToast->height() - 24);
        startupToast->move(position);
    }

    connect(startupToast, &QObject::destroyed, this, [this]() {
        startupToast = nullptr;
    });

    startupToast->show();
    QTimer::singleShot(5200, startupToast, &QWidget::close);
}
