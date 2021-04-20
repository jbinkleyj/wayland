#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "wayland/wayland-client.h"

#include <QDebug>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
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
    qDebug() << "is_Wayland_Display_Available:" << is_Wayland_Display_Available();
}


bool MainWindow::is_Wayland_Display_Available()
{
    wl_display *display = wl_display_connect( Q_NULLPTR ); // Want test of other display then "wayland-0" "wayland-1" ...
    if ( display == Q_NULLPTR )
    {
        fprintf(stderr, "Can't connect to display\n");
        return false;
    }
    
    printf( "connect to display\n" );

    wl_display_disconnect( display );
    printf( "disconnected from display\n" );
    
    return true;
}
