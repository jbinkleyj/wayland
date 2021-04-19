#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "/usr/include/wayland/wayland-client.h"

#include <QDebug>

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


void MainWindow::slot_pushButtonConnectToDisplay()
{
    wl_display *display = wl_display_connect( Q_NULLPTR );
    if ( display == Q_NULLPTR )
    {
        fprintf(stderr, "Can't connect to display\n");
        return;
    }
    printf( "connect to display\n" );

    wl_display_disconnect( display );
    printf( "disconnected from display\n" );
}
