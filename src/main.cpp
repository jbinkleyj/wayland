
#include "mainWindow_wl.h"

#include <QApplication>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QvkMainWindow_wl *mainWindow = new QvkMainWindow_wl( 0 );
    mainWindow->show();

    return a.exec();
}

