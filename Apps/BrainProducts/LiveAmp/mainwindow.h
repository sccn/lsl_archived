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
#include <lsl_cpp.h>

// BrainAmp API
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <WinIoCtl.h>
#include "Amplifier_LIB.h"



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
private:
    // background data reader thread
	void read_thread(int chunkSize, int samplingRate, bool useAUX, bool activeShield, std::vector<std::string> channelLabels);

    // raw config file IO
    void load_config(const std::string &filename);
    void save_config(const std::string &filename);

	bool InitializeLiveAmp(void);
	bool ConfigureLiveAmp(void);

	void ExtractLiveAmpData(BYTE* buffer, int size, int samplesize, int *dataTypes, int usedChannelsCnt, std::vector<std::vector<double>> &extractData);

	bool EnableLiveAmpChannels(bool enableAUX, bool enableACC);
	bool GenerateUsedPhysicalChannelIndexes(void);
	void PrepareDataToSendOverLSL(std::vector<std::vector<double>> &LiveAmptData, std::vector<std::vector<double>> &LSLData, std::vector<uint16_t> &trigger_buffer);
	void AdjustChannelLabels(std::vector<std::string>& inpuChannelLabels, std::vector<std::string> &adjustedChannelLabels);

	boost::shared_ptr<boost::thread> reader_thread_;	// our reader thread

    Ui::MainWindow *ui;

	bool stop_;											// whether the reader thread is supposed to stop

};

#endif // MAINWINDOW_H
