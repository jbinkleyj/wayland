#include "mainWindow.h"
#include "global.h"

#include <QStringList>
#include <QStandardPaths>
#include <QDebug>
#include <QDateTime>
#include <QThread>

MainWindow::MainWindow( QWidget *parent, Qt::WindowFlags f )
    : QMainWindow(parent, f)
    , ui(new Ui::PortalTest)
{
    ui->setupUi( this );

    connect( ui->pushButtonStart, SIGNAL( clicked() ), this, SLOT( slot_start() ) );
    connect( ui->pushButtonStop,  SIGNAL( clicked() ), this, SLOT( slot_stop() ) );
    connect( portalTest, SIGNAL( signal_fd_path( QString, QString ) ), this, SLOT( slot_start_gst( QString, QString ) ) );

    gst_init( nullptr, nullptr );
}

MainWindow::~MainWindow()
{
}

void MainWindow::slot_start()
{
    qDebug().noquote() << "start";
    portalTest->requestScreenSharing();
}


QString MainWindow::Vk_get_Videocodec_Encoder()
{
    QString value;
    QString encoder = "openh264enc"; //ui->comboBoxVideoCodec->currentData().toString();

    if ( encoder == "openh264enc" )
    {
        QStringList list;
        list << encoder;
        list << "qp-min=23"; // + QString::number( sliderOpenh264->value() );
        list << "qp-max=23"; // + QString::number( sliderOpenh264->value() );
        list << "usage-type=camera"; // We need camera not screen. With screen and a fast sequence of images the video jerks.
        list << "complexity=low";
        list << "multi-thread=" + QString::number( QThread::idealThreadCount() );
        list << "slice-mode=auto"; // Number of slices equal to number of threads
        value = list.join( " " );
        value.append( " ! h264parse" );
    }

    return value;
}


void MainWindow::slot_start_gst( QString vk_fd, QString vk_path )
{
    QStringList pipeline;
    pipeline << QString( "pipewiresrc fd=" ).append( vk_fd ).append( " path=" ).append( vk_path ).append( " do-timestamp=true" );
    pipeline << "videoconvert";
    pipeline << "videorate";
    pipeline << "video/x-raw, framerate=30/1";
    pipeline << Vk_get_Videocodec_Encoder();
    pipeline << "matroskamux name=mux";

    QString newVideoFilename = global::name + "-" + QDateTime::currentDateTime().toString( "yyyy-MM-dd_hh-mm-ss" ) + ".mkv";// + ui->comboBoxFormat->currentText();
    pipeline << "filesink location=\"" + QStandardPaths::writableLocation( QStandardPaths::MoviesLocation ) + "/" + newVideoFilename + "\"";

    QString launch = pipeline.join( " ! " );

    qDebug().noquote();
    qDebug().noquote() << global::nameOutput << launch;
    qDebug().noquote();

    vk_gstElement = gst_parse_launch( launch.toUtf8(), nullptr );
    gst_element_set_state( vk_gstElement, GST_STATE_PLAYING );
}

void MainWindow::slot_stop()
{
    // send EOS to pipeline
    gst_element_send_event( vk_gstElement, gst_event_new_eos() );

    // wait for the EOS to traverse the pipeline and is reported to the bus
    GstBus *bus = gst_element_get_bus( vk_gstElement );
    gst_bus_timed_pop_filtered( bus, GST_CLOCK_TIME_NONE, GST_MESSAGE_EOS );

    gst_element_set_state( vk_gstElement, GST_STATE_NULL );

    qDebug().noquote() << global::nameOutput << "Stop record";
}
