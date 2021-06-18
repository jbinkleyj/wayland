
#include "mainWindow_wl.h"

#include <QApplication>


int main(int argc, char *argv[])
{
    QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QGuiApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);

    QApplication a(argc, argv);

    QvkMainWindow_wl *mainWindow = new QvkMainWindow_wl( 0 );
    mainWindow->show();

    return a.exec();
}

