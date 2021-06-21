
#include "mainWindow_wl.h"

#include <gst/gst.h>

#include <QApplication>


int main(int argc, char *argv[])
{
    QLoggingCategory::defaultCategory()->setEnabled( QtDebugMsg, true );

    QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QGuiApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);

    QApplication a(argc, argv);

    gst_init (&argc, &argv);

    QvkMainWindow_wl *mainWindow = new QvkMainWindow_wl( 0 );
    mainWindow->show();

    return a.exec();
}

