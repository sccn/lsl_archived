#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "ui_mainwindow.h"
#include "videoframe.h"

// Qt
#include <QMainWindow>

#ifdef __WIN32

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

// LSL
#include <lsl_cpp.h>


namespace Ui{
class MainWidnow;
}

class MainWindow : public QMainWindow{
	Q_OBJECT

public:
	explicit MainWindow(QWidget *parent, const std::string &config_file);
	~MainWindow();

private slots:

	void LaunchNewVideoFrame();

private:
	Ui::MainWindow *ui;										// window pointer


};

#endif // MAINWINDOW_H