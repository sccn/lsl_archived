#ifndef MAINWINDOW_H
#define MAINWINDOW_H


// custom video panel class
#include "videopanel.h"


// boost
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/asio.hpp>

// Qt
#include <QMainWindow>
#include <QMessageBox>
#include <QHostInfo>
#include <QCloseEvent>
#include <QCameraInfo>
#include <QComboBox>


#define WIN32_LEAN_AND_MEAN
#include <windows.h>




// LSL
//#include "../../LSL/liblsl/include/lsl_cpp.h"


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:    

    void closeEvent(QCloseEvent *ev);
	void newVideo(void);

private:
	Ui::MainWindow *ui;	// window pointer
	QList<QCameraInfo> cameraInfos;
	std::vector<boost::shared_ptr<VideoPanel>> videoPanels; //
	
};

//extern MainWindow *theWindow;

#endif // MAINWINDOW_H
