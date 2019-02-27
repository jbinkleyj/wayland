#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>
#include <QDesktopWidget>
#include <QGuiApplication>
#include <QFile>

#include <stdio.h>
#include <stdlib.h>

// cc -o connect connect.c -lwayland-client

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent),
                                          ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect( ui->pushButtonConnectToDisplay, SIGNAL( clicked( bool ) ), this, SLOT( slot_pushButtonConnectToDisplay() ) );
}


MainWindow::~MainWindow()
{
    delete ui;
}


/* https://www.jan.newmarch.name/Wayland/Qt/#aftertoc

    QPlatformNativeInterface *native =
        QGuiApplication::platformNativeInterface();
    struct wl_display *wl_dpy = (struct wl_display *)
        native->nativeResourceForWindow("display", NULL);
	
      

If you need to get the Wayland surface, this is done by

	
    QPlatformNativeInterface *native =
        QGuiApplication::platformNativeInterface();
    struct wl_surface *surface = static_cast<struct wl_surface *>(
       native->nativeResourceForWindow("surface", this->windowHandle()
*/       


void MainWindow::slot_pushButtonConnectToDisplay()
{
    
    vk_display = wl_display_connect(NULL);
    if (vk_display == NULL)
    {
        fprintf(stderr, "Can't connect to display\n");
        return;
    }
    printf("connected to display\n");

    wl_display_disconnect(vk_display);
    printf("disconnected from display\n");

}
