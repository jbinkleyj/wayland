#include "mainWindow.h"

#include <QStringList>
#include <QStandardPaths>
#include <QDebug>

MainWindow::MainWindow( QWidget *parent, Qt::WindowFlags f )
    : QMainWindow(parent, f)
    , ui(new Ui::PortalTest)
{
    ui->setupUi( this );

    connect( ui->screenShareButton, SIGNAL( clicked() ), this, SLOT( slot_start() ) );
    connect( ui->pushButtonStop,    SIGNAL( clicked() ), this, SLOT( slot_stop() ) );
    connect( portalTest, SIGNAL( signal_fd_path( QString, QString ) ), this, SLOT(slot_start_gst( QString, QString ) ) );

    gst_init( nullptr, nullptr );
}

MainWindow::~MainWindow()
{
}

void MainWindow::slot_start()
{
    qDebug() << "start";
    portalTest->requestScreenSharing();
}


void MainWindow::slot_start_gst( QString vk_fd, QString vk_path )
{
    QStringList gstLaunch;
    gstLaunch << QString( "pipewiresrc fd=" ).append( vk_fd ).append( " path=" ).append( vk_path ).append( " do-timestamp=true" );
    gstLaunch << "videoconvert";
    gstLaunch << "videorate";
    gstLaunch << "video/x-raw, framerate=30/1";
    gstLaunch << "queue max-size-bytes=1073741824 max-size-time=10000000000 max-size-buffers=1000";
    gstLaunch << "x264enc qp-min=17 qp-max=17 speed-preset=superfast threads=4";
    gstLaunch << "video/x-h264, profile=baseline";
    gstLaunch << "matroskamux name=mux";
    gstLaunch << "filesink location="  + QStandardPaths::writableLocation( QStandardPaths::MoviesLocation ) + "/" + "vokoscreenNG-bad.mkv";
    QString launch = gstLaunch.join( " ! " );

    qDebug();
    qDebug() << launch;
    qDebug();

    element = gst_parse_launch( launch.toUtf8(), nullptr );
    gst_element_set_state( element, GST_STATE_PLAYING );
}

void MainWindow::slot_stop()
{
    GstStateChangeReturn ret ;
    ret = gst_element_set_state( element, GST_STATE_PAUSED );
    ret = gst_element_set_state( element, GST_STATE_READY );
    ret = gst_element_set_state( element, GST_STATE_NULL );
    gst_object_unref( element );
qDebug().noquote() << "Stop record";
    make_time_true();
}

void MainWindow::make_time_true()
{
    QStringList gstLaunch;
    gstLaunch << "filesrc location="  + QStandardPaths::writableLocation( QStandardPaths::MoviesLocation ) + "/" + "vokoscreenNG-bad.mkv";
    gstLaunch << "matroskademux name=d d.video_0";
    gstLaunch << "matroskamux";
    gstLaunch << "filesink location=" + QStandardPaths::writableLocation( QStandardPaths::MoviesLocation ) + "/" + "vokoscreenNG-good.mkv";
    QString launch = gstLaunch.join( " ! " );
qDebug() << launch;
    element = gst_parse_launch(launch.toUtf8(), nullptr);
    gst_element_set_state( element, GST_STATE_PLAYING);
}
