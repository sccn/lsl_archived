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

// BrainAmp API
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <WinIoCtl.h>
#include "BrainAmpIoCtl.h"


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

    // start the BrainAmpSeries connection
    void link();

    // close event (potentially disabled)
    void closeEvent(QCloseEvent *ev);
private:
    // background data reader thread
	void read_thread(int deviceNumber, ULONG serialNumber, int impedanceMode, int resolution, int dcCoupling, int chunkSize, int channelCount, std::vector<std::string> channelLabels);

    // raw config file IO
    void load_config(const std::string &filename);
    void save_config(const std::string &filename);
	
	HANDLE hDevice;
	bool stop_;											// whether the reader thread is supposed to stop
    boost::shared_ptr<boost::thread> reader_thread_;	// our reader thread

	bool g_unsampledMarkers;
	bool g_sampledMarkers;
	bool g_sampledMarkersEEG;

	bool pullUpHiBits;
	bool pullUpLowBits;
	USHORT g_pull_dir;

    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
