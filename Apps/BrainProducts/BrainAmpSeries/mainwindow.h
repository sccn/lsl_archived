#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QCloseEvent>
#include <QFileDialog>
#include <QMessageBox>
#include <atomic>
#include <thread>
#include <vector>

#ifdef WIN32
    #define WIN32_LEAN_AND_MEAN
    #include <windows.h>
#else
	using HANDLE = void*;
	using ULONG = unsigned long;
#endif


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
    void closeEvent(QCloseEvent *ev) override;
private:
    // background data reader thread
	template<typename T>
	void read_thread(int deviceNumber, ULONG serialNumber, int impedanceMode,
					 int resolution, int dcCoupling, int chunkSize, int channelCount,
					 std::vector<std::string> channelLabels);

    // raw config file IO
    void load_config(const std::string &filename);
    void save_config(const std::string &filename);
	
	HANDLE hDevice;
	std::atomic<bool> stop_;						// whether the reader thread is supposed to stop
    std::unique_ptr<std::thread> reader_thread_;	// our reader thread

	bool g_unsampledMarkers;
	bool g_sampledMarkers;
	bool g_sampledMarkersEEG;

	bool pullUpHiBits;
	bool pullUpLowBits;
	uint16_t g_pull_dir;

    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
