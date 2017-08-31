#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "gdsthread.h"

const QString default_config_fname = "gneedaccess_config.cfg";

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

signals:
    void devicesAdded(QStringList); // Emitted after a device is added to streamingListWidget.
    void startStreams();

private slots:
    void update_device_lists(const std::vector<GDSThread::GDS_DeviceInfo>);
    void devices_connected_result(bool);

    void on_scanPushButton_clicked();
    void on_toStreamPushButton_clicked();
    void on_goPushButton_clicked();

private:
    void load_config(const QString filename);
    // void save_config(const QString filename);  // TODO: Actually save the config to a file.

    Ui::MainWindow *ui;
    GDSThread m_thread;
};

#endif // MAINWINDOW_H
