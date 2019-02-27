#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "/usr/include/wayland/wayland-client.h"

#include <QDebug>
#include <qpa/qplatformnativeinterface.h>

#include <stdio.h>
#include <stdlib.h>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent),
                                          ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect( ui->pushButtonConnectToDisplay, SIGNAL( clicked( bool ) ), this, SLOT( slot_pushButtonConnectToDisplay() ) );
    connect( ui->pushButtonplatformNativeInterface, SIGNAL( clicked( bool ) ), this, SLOT( slot_platformNativeInterface() ) );
}


MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::slot_platformNativeInterface()
{
    // https://jan.newmarch.name/Wayland/ProgrammingClient/
    QPlatformNativeInterface *native = QGuiApplication::platformNativeInterface();
    struct wl_display *wl_dpy = (struct wl_display *)native->nativeResourceForWindow("display", NULL);

    //If you need to get the Wayland surface, this is done by
    struct wl_surface *surface = static_cast<struct wl_surface *>(native->nativeResourceForWindow("surface", this->windowHandle() ));
}


void MainWindow::slot_pushButtonConnectToDisplay()
{
    vk_display = wl_display_connect( Q_NULLPTR );
    if ( vk_display == Q_NULLPTR )
    {
        qDebug() << "Can't connect to display\n";
        return;
    }
    qDebug() << "connect to display";

    wl_display_disconnect( vk_display );
    qDebug() << "disconnected from display";
}
