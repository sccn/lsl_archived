#ifndef MAINWINDOW_H
#define MAINWINDOW_H

// Qt
#include "ui_mainwindow.h"

#include <QMainWindow>
#include <QCloseEvent>
#include <QListWidget>
#include <QTimer>
#include <QStringList>

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

private slots:
	void statusUpdate(void) const;
	void closeEvent(QCloseEvent *ev) override;
	void blockSelected(const QString& block);
	std::vector<lsl::stream_info> refreshStreams(void);
	void startRecording(void);
	void stopRecording(void);

private:
	QSet<QString> getCheckedStreams() const;

	std::unique_ptr<recording> currentRecording;
	
	int startTime;
	std::unique_ptr<QTimer> timer;
 
	int currentTrial;
	QString currentBlock;

	QStringList requiredStreams;
	std::map<std::string, int> syncOptionsByStreamName;
	QSet<QString> missingStreams;

	QString recFilename;

	// function for loading config file
	void load_config(QString filename);
	void save_config(QString filename);
    
	std::unique_ptr<Ui::MainWindow> ui;	// window pointer
};



#endif // MAINWINDOW_H
