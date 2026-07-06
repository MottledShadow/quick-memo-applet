#include "mainwindow.h"

#include <QApplication>
#include <QCoreApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QCoreApplication::setOrganizationName("MOS");
    QCoreApplication::setApplicationName("QuickMemoApplet");
    QApplication::setQuitOnLastWindowClosed(false);

    MainWindow w;
    return QApplication::exec();
}
