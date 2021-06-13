#include "mainWindow.h"

#include <QStringList>
#include <QStandardPaths>
#include <QDebug>

MainWindow::MainWindow( QWidget *parent, Qt::WindowFlags f )
    : QMainWindow(parent, f)
    , ui(new Ui::PortalTest)
{
    ui->setupUi( this );

    ui->screenShareCombobox->hide();

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
    gstLaunch << "filesink location="  + QStandardPaths::writableLocation( QStandardPaths::MoviesLocation ) + "/" + "vokoscreenNG.mkv";
    QString launch = gstLaunch.join( " ! " );

    qDebug();
    qDebug() << launch;
    qDebug();

    element = gst_parse_launch( launch.toUtf8(), nullptr );
    gst_element_set_state( element, GST_STATE_PLAYING );
}

void MainWindow::slot_stop()
{

    // send EOS to pipeline
    gst_element_send_event( element, gst_event_new_eos() );

    // wait for the EOS to traverse the pipeline and is reported to the bus
    GstBus *bus = gst_element_get_bus( element );
    gst_bus_timed_pop_filtered( bus, GST_CLOCK_TIME_NONE, GST_MESSAGE_EOS );

    gst_element_set_state( element, GST_STATE_NULL );

    qDebug().noquote() << "Stop record";
}
