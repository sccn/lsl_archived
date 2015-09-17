#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QCloseEvent>
#include <QFileDialog>
#include <QMessageBox>
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>
#include <string>
#include <vector>

// LSL API
#include <lsl_cpp.h>

#define WIN32_LEAN_AND_MEAN
#include "windows.h"

// data formatting settings
const int in_chns_per_device = 12;	// number of input channels per device (raw sensors)
const int out_chns_per_device = 6;	// number of output channels per device (Fx,Fy,Fz,Mx,My,Mz)
const int max_devices = 2;			// maximum number of devices that can be multiplexed into a stream

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent, const std::string &config_file);
    ~MainWindow();
    
private slots:
    // config file dialog ops (from main menu)
    void load_config_dialog();
    void save_config_dialog();

    // start the AMTI connection
    void link_amti();

    // close event (potentially disabled)
    void closeEvent(QCloseEvent *ev);
private:
    // background data reader thread
	void read_thread(HANDLE hPort, int comPort, int samplingRate, int numDevices);

    // raw config file IO
    void load_config(const std::string &filename);
    void save_config(const std::string &filename);
	
	bool stop_;															// whether the reader thread is supposed to stop
	std::string calibstr_[2];											// the original strings that describes the calibration matrices
	float calib_[max_devices][in_chns_per_device][out_chns_per_device];	// device calibration matrices (one per device, 6x6)
    boost::shared_ptr<boost::thread> reader_thread_;					// our reader thread

    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
