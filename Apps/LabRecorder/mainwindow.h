#ifndef MAINWINDOW_H
#define MAINWINDOW_H

// boost
/* #include <boost/shared_ptr.hpp> */
/* #include <boost/thread.hpp> */
// Qt
#include <QMainWindow>
#include <QFileDialog>
#include <QCloseEvent>
#include <QMessageBox>
#include <QListWidget>
#include <QListWidgetItem>
#include <QHostInfo>
#include <QBrush>
#include <QTimer>
#include <QStatusBar>
#include <fstream>

#ifdef __WIN32

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "windows.h"
#endif

// LSL
#include "../../LSL/liblsl/include/lsl_cpp.h"

// recording class
#include "recording.h"

class RecorderItem {
	
public: 
	QListWidgetItem listItem;
	std::string uid;
};

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent,const std::string &config_file);
    ~MainWindow();

private slots:    
	
	void statusUpdate(void);	
    void closeEvent(QCloseEvent *ev);
	void browseConfigDialog(void);
	void browseLocationDialog(void);
	void blockSelected(QListWidgetItem *item);
	void refreshStreams(void);
	void startRecording(void);
	void stopRecording(void);


	// I think this is unnecessary
	//void streamClicked(void);



private:

	
	bool currentlyRecording;
	recording *currentRecording;
	
	int startTime;
	QTimer *timer;
 
	int currentTrial;
	std::string currentBlock;

	std::vector<lsl::stream_info> resolvedStreams;
	std::vector<lsl::stream_info> checkedStreams;
	std::vector<std::string> requiredStreams;
	std::vector<std::string> onlineSyncStreams;
	std::map<std::string, int> syncOptionsByStreamName;
	std::vector<std::string> missingStreams;

	std::string recFilename;
	FILE *p_recFile;



	// function for loading config file
	void load_config(const std::string &filename);
    
	Ui::MainWindow *ui;										// window pointer
};



#endif // MAINWINDOW_H
