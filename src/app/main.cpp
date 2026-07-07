#include "mainwindow.h"

#include <QApplication>
#include <QCoreApplication>
#include <QIcon>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QCoreApplication::setOrganizationName("MOS");
    QCoreApplication::setApplicationName("QuickMemoApplet");
    QApplication::setWindowIcon(QIcon(QStringLiteral(":/icons/app.png")));
    QApplication::setQuitOnLastWindowClosed(false);

    MainWindow w;
    return QApplication::exec();
}
