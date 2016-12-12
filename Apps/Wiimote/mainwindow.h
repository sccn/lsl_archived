#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QCloseEvent>
#include <QFileDialog>
#include <QMessageBox>
#include <thread>
#include <string>
#include <vector>

// LSL API
#include <lsl_cpp.h>

// wiiuse API
#include "wiiuse.h"

//#if defined(_WIN32)
//#define WIN32_LEAN_AND_MEAN
//#include <windows.h>
//#endif


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent);
    ~MainWindow();
    
private slots:
    // scan for Wii devices and start a thread.
    void link();

    // close event (potentially disabled)
    void closeEvent(QCloseEvent *ev);
    
    // function running in background thread
    void stream_func();
private:

    void handle_event(int wm_ix, double timestamp);
        
    // Convenience functions.
    int get_channel_count(int wm_ix);
    void append_setup_info(int wm_ix, lsl::stream_info& out_info);
    void append_channel_info(int wm_ix, lsl::stream_info& out_info);
    
    Ui::MainWindow  *ui;
    wiimote**       mDevices;   //
    int             nDevices;
    QTimer*         m_pTimer;
    std::vector<lsl::stream_outlet*> m_marker_outlets;
    std::vector<lsl::stream_outlet*> m_data_outlets;
    std::vector<std::vector<float>> m_data_buffers;
};

#endif // MAINWINDOW_H
