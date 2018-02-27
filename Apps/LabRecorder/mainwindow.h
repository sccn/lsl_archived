#ifndef MAINWINDOW_H
#define MAINWINDOW_H

// Qt
#include "ui_mainwindow.h"

#include <QMainWindow>
#include <QFileDialog>
#include <QCloseEvent>
#include <QListWidget>
#include <QTimer>

#ifdef __WIN32

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

// LSL
#include <lsl_cpp.h>

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
	void statusUpdate(void) const;
	void closeEvent(QCloseEvent *ev) override;
	void blockSelected(QListWidgetItem *item);
	void refreshStreams(void);
	void startRecording(void);
	void stopRecording(void);



private:

	bool currentlyRecording;
	recording *currentRecording;
	
	int startTime;
	std::unique_ptr<QTimer> timer;
 
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
    
	std::unique_ptr<Ui::MainWindow> ui;	// window pointer
};



#endif // MAINWINDOW_H
