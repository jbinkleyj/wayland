#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDBusConnection>
#include <QDBusConnectionInterface>
#include <QDBusInterface>

#include "/usr/include/wayland/wayland-client.h"

namespace Ui {
class MainWindow;
}

//struct wl_display *vk_display = NULL;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    wl_display *vk_display = NULL;


private slots:
    void slot_pushButtonConnectToDisplay();


};

#endif // MAINWINDOW_H
