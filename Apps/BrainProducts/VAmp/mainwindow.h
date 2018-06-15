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
#define LSL_DEBUG_BINDINGS
#include "../../../LSL/liblsl/include/lsl_cpp.h"

// BrainAmp API
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <WinIoCtl.h>


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
	void update_channel_labels(int n);
	void update_channels_for_sr(int n);

    // start the ActiChamp connection
    void link();

    // close event (potentially disabled)
    void closeEvent(QCloseEvent *ev);
private:

	void update_channel_counter();

    // background data reader thread
	void read_thread(int deviceNumber, int channelCount, int samplingRate, bool useAUX, bool unsampledMarkers, bool sampledMarkers, bool sampledMarkersEEG, std::vector<std::string> channelLabels);

    // raw config file IO
    void load_config(const std::string &filename);
    void save_config(const std::string &filename);
	
	bool stop_;											// whether the reader thread is supposed to stop
    boost::shared_ptr<boost::thread> reader_thread_;	// our reader thread

    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
