#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "ui_formMainWindow_wl.h"
#include "portal_wl.h"

#include <QWindow>
#include <gst/gst.h>

namespace Ui
{
class QvkMainWindow_wl;
}


class QvkMainWindow_wl : public QMainWindow
{
    Q_OBJECT
public:
    QvkMainWindow_wl( QWidget *parent = Q_NULLPTR, Qt::WindowFlags f = Qt::WindowFlags() );
    ~QvkMainWindow_wl();
    Ui::formMainWindow_wl *ui;
    Portal_wl *portal_wl = new Portal_wl();

public Q_SLOTS:
    void slot_start();
    void slot_stop();
    void slot_start_gst( QString vk_fd, QString vk_path );


private:
    GstElement *vk_gstElement = nullptr;
    QString Vk_get_Videocodec_Encoder();


};

#endif 