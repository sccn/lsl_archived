#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "openvrthread.h"

const QString default_config_fname = "openvr_config.cfg";

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0,
                        const QString config_file = default_config_fname);
    ~MainWindow();
    
private slots:

    void on_actionLoad_Configuration_triggered();
    void on_actionSave_Configuration_triggered();
    void update_connect_label(bool status);
    void update_list_devices(QStringList deviceList);
	void update_stream_button(bool status);

    void on_pushButton_scan_clicked();

    void on_pushButton_stream_clicked();

private:
    void load_config(const QString filename);
    void save_config(const QString filename);

    Ui::MainWindow *ui;
    OpenVRThread m_thread;
};

#endif // MAINWINDOW_H
