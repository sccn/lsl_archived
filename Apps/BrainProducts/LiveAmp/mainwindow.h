#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QCloseEvent>
#include <QFileDialog>
#include <QMessageBox>
#include <QThread>
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>
#include <string>
#include <vector>

// LSL API
#define LSL_DEBUG_BINDINGS
#include "../../../LSL/liblsl/include/lsl_cpp.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <WinIoCtl.h>

#include "LiveAmp.h"



namespace Ui {
class MainWindow;
}

class WaitThread : public QThread
{
	Q_OBJECT

protected:
	void run();
};



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
   
	// get list of available devices
	void refresh_devices();

	// link to selected device
	void link();

    // close event (potentially disabled)
    void closeEvent(QCloseEvent *ev);

	
	// if the device combo box item changes
	void choose_device(int which);


private:

	void wait_message();


	// background data reader thread
	void read_thread(int chunkSize, int samplingRate, bool useAUX, bool useACC, bool useBipolar, std::vector<std::string> eegChannelLabels);
	
	
	// container for amplifier enumeration
	//std::vector<std::pair<std::string, int>> ampData;

    // raw config file IO
    void load_config(const std::string &filename);
    void save_config(const std::string &filename);

	bool ConfigureLiveAmp(void);
	bool InitializeLiveAmp(void);
	
	void ExtractLiveAmpData(BYTE* buffer, int size, int samplesize, int *dataTypes, int usedChannelsCnt, std::vector<std::vector<double>> &extractData);
	bool EnableLiveAmpChannels(bool enableAUX, bool enableACC);
	bool GenerateUsedPhysicalChannelIndexes(void);
	void PrepareDataToSendOverLSL(std::vector<std::vector<double>> &LiveAmptData, std::vector<std::vector<double>> &LSLData, std::vector<uint16_t> &trigger_buffer);
	void AdjustChannelLabels(std::vector<std::string>& inpuChannelLabels, std::vector<std::string> &adjustedChannelLabels);
	
	bool unsampledMarkers;  
	bool sampledMarkers;   
	bool sampledMarkersEEG; 
	bool useSim;

	LiveAmp *liveAmp;
	std::vector<std::string> live_amp_sns;				// live amp serial number container

	boost::shared_ptr<boost::thread> reader_thread_;	// our reader thread
	
    Ui::MainWindow *ui;

	bool use_simulators;
	bool stop_;											// whether the reader thread is supposed to stop


};

#endif // MAINWINDOW_H
