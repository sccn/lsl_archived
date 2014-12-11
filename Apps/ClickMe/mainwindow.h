#ifndef MAINWINDOW_H
#define MAINWINDOW_H

// boost
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>
// Qt
#include <QtGui/QMainWindow>
#include <QtGui/QMessageBox>
#include <QtGui/QComboBox>
#include <QtNetwork/QHostInfo>
#include <QtGui/QFileDialog>
#include <QtCore/QFile>

// LSL
#include "../../LSL/liblsl/include/lsl_cpp.h"
#include "../../LSL/liblsl/include/lsl_c.h"
//for writing to file
#include <stdio.h>

#define T_R 0
#define T_FB 1

namespace Ui {

class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
	
    ~MainWindow();

private slots:    
	// close event: used to disable closing while linked
    //void closeEvent(QCloseEvent *ev);

	void got_clicked(void);
	void got_started(void);
	void got_stopped(void);

private:

	void listen(void);

	boost::shared_ptr<boost::thread> listen_thread_;
	Ui::MainWindow *ui;									// window pointer
};

#endif // MAINWINDOW_H
