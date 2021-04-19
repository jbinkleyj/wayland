#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "/usr/include/wayland/wayland-client.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    


private slots:
    void slot_pushButtonConnectToDisplay();
//    void slot_platformNativeInterface();


};

#endif // MAINWINDOW_H
