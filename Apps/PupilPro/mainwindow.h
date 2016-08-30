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
//#include "lsl_cpp.h"
#include "../../LSL/liblsl/include/lsl_cpp.h"
//#include "lsl_c.h"
#include "../../LSL/liblsl/include/lsl_c.h"
//for writing to file
#include <stdio.h>
#include "zmq_client.h"

#define T_R 0
#define T_FB 1

namespace Ui {

class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent, const std::string &config_file);
	
    ~MainWindow();

private slots:    
    // config file dialog ops (from main menu)
    void load_config_dialog();
    void save_config_dialog();

	// start the BrainVision RDA connection
    void link_pupilpro();

    // close event (potentially disabled)
    void closeEvent(QCloseEvent *ev);

	// status messages
	void statusMessage(QString msg);

signals:
	void sendMessage(QString msg);

private:

    // raw config file IO
    void load_config(const std::string &filename);
    void save_config(const std::string &filename);

	void listen(QString serverIP, QString port, int fps);
	boost::shared_ptr<boost::thread> listen_thread_;
	void connect(QString serverIP, QString port);
	boost::shared_ptr<boost::thread> connect_thread_;

	void parse(std::string str, std::vector<double>& sample, std::vector<std::string>& markers, int& marker_cnt);

	bool stop_;
	boost::mutex mtx_;
	ZMQ_Client zmq_client_;

	Ui::MainWindow *ui;									// window pointer
};

#endif // MAINWINDOW_H
