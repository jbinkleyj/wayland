#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "ui_portaltest.h"
#include "portaltest.h"

#include <QWindow>
#include <gst/gst.h>

namespace Ui
{
class MainWindow;
}


class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow( QWidget *parent = Q_NULLPTR, Qt::WindowFlags f = Qt::WindowFlags() );
    ~MainWindow();
    Ui::PortalTest *ui;
    PortalTest *portalTest = new PortalTest();

public Q_SLOTS:
    void slot_start();
    void slot_stop();
    void slot_start_gst( QString vk_fd, QString vk_path );


private:
    GstElement *vk_gstElement = nullptr;
    QString Vk_get_Videocodec_Encoder();


};

#endif 
