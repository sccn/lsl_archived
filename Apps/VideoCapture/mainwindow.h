#ifndef MAINWINDOW_H
#define MAINWINDOW_H

// boost
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>
#include <boost/asio.hpp>

// Qt
#include <QMainWindow>
#include <QCloseEvent>
#include <QMessageBox>
#include <QHostInfo>

// opencv -- for webcams and such
#include "opencv2/opencv.hpp"

// pointgrey -- for interfacing with pointgrey cameras

// windows
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "mf_enumerate_cameras.h"
#endif

// LSL
#include "../../LSL/liblsl/include/lsl_cpp.h"

// STL
#include <string>
#include <vector>

// custom container for frame data, frame counter, and lsl timestamp
typedef struct _frame_data{
	cv::Mat         frame;
	int             count;
	double          timestamp;
}t_frame_data;

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
    void closeEvent(QCloseEvent *ev);


private:

    Ui::MainWindow *ui;										// window pointer
	//std::vector<const cv::Mat> frame_buf_;                  // container for video frames
	std::vector<t_frame_data> frame_buf_;                   // new and improved frame buffer
	boost::shared_ptr<boost::thread> r_thread_;             // thread for reading frames
	boost::shared_ptr<boost::thread> w_thread_;             // thread for writing frames
	//boost::timed_mutex mutex_;                              // conditional mutex for r/w threads
	boost::mutex mutex_;
	bool r_stop_;
	bool w_stop_;

	void stop_read(void);
	void stop_write(void);

	void read_thread(void);
	void write_thread(void);

	cv::VideoCapture reader;
	cv::VideoWriter writer;

	cv::Size video_size_;
	bool has_camera_;
	int fps_;

};

extern MainWindow *theWindow;

#endif // MAINWINDOW_H
