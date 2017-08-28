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
//#define LSL_DEBUG_BINDINGS
#include "../../../LSL/liblsl/include/lsl_cpp.h"

// BrainAmp API
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <WinIoCtl.h>
#include "ActiChamp.h"


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

    // start the ActiChamp connection
    void link();

    // close event (potentially disabled)
    void closeEvent(QCloseEvent *ev);

	// set the min chunk size according sampling rate
	void setMinChunk();

private:

    // background data reader thread
	void read_thread(int deviceNumber, int channelCount, int chunkSize, int samplingRate, bool useAUX, bool activeShield, std::vector<std::string> channelLabels);

    // raw config file IO
    void load_config(const std::string &filename);
    void save_config(const std::string &filename);

	bool g_unsampledMarkers;
	bool g_sampledMarkers;
	bool g_sampledMarkersEEG;
	
	bool stop_;											// whether the reader thread is supposed to stop
    boost::shared_ptr<boost::thread> reader_thread_;	// our reader thread

    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
