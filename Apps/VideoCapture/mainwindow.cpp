#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <Objbase.h>



MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
	ui->setupUi(this);
	theWindow = this;

	has_camera_ = false;
	
	reader.open(0);
	cv::Mat foo_frame; // get first frame to find size and type
	reader>>foo_frame;
	if(reader.isOpened()){
			
		video_size_ = cv::Size((int) reader.get(CV_CAP_PROP_FRAME_WIDTH),
			                   (int) reader.get(CV_CAP_PROP_FRAME_HEIGHT));
		fps_        = reader.get(CV_CAP_PROP_FPS);
		if(fps_ == 0)fps_=30; // don't know why this is 0 here...
		has_camera_ = true;

	}else{return;} // needs more informative error checking

	r_stop_ = false;
	r_thread_.reset(new boost::thread(&MainWindow::read_thread,this));

	// for some reason on Windows writers need to be originated in the main thread? 
	// http://answers.opencv.org/question/76077/videowriter-fails-to-open-inside-thread/
	// but this also works if the reader and writer are in the same thread (not main thread)
	// all along
	writer.open("foo.avi", CV_FOURCC('M', 'J', 'P', 'G'), fps_, video_size_);
	w_stop_ = false;
	w_thread_.reset(new boost::thread(&MainWindow::write_thread, this));
	
	
}

// testbed for video capture app
void MainWindow::read_thread(void) {
	
	//cv::VideoCapture reader(0);
	cv::Mat foo_frame; // get first frame to find size and type
	reader>>foo_frame;
	if(reader.isOpened()){
			
		video_size_ = cv::Size((int) reader.get(CV_CAP_PROP_FRAME_WIDTH),
			                   (int) reader.get(CV_CAP_PROP_FRAME_HEIGHT));
		fps_        = reader.get(CV_CAP_PROP_FPS);
		if(fps_ == 0)fps_=30; // don't know why this is 0 here...
		has_camera_ = true;

	}else{return;} // needs more informative error checking

	// frame counter
	int cnt = 1;

	// lsl boilerplate
	lsl::stream_info   info("VideoCaptureRead", "Video", 1, fps_, lsl::cf_int32, boost::asio::ip::host_name());
	lsl::stream_outlet outlet(info);


	try{

		while(!r_stop_){
			
			double time_now = lsl::local_clock();
			
			cv::Mat frame;
			cv::Mat frameWrite;
			t_frame_data frame_data;

			// impose a hard lock
			//boost::unique_lock<boost::timed_mutex> lock(mutex_);
			reader>>frame;
			frame_data.frame     = frame;
			frame_data.timestamp = time_now;
			frame_data.count     = cnt;
			mutex_.lock();
			frame_buf_.push_back(frame_data);
			mutex_.unlock();
			//if(cnt%5==0){
			//	while(!frame_buf_.empty()){

			//		frameWrite = frame_buf_[0];
			//		writer.write(frameWrite);
			//		frame_buf_.erase(frame_buf_.begin());
			//	}
			//}
			//boost::timed_mutex *m = lock.release();
			//m->unlock();

			// push lsl counter
			outlet.push_sample(&cnt,time_now);
			cnt++;

			cv::imshow("VideoCapture", frame);

			

			// display the frame?
			if(cv::waitKey((int)(1000/fps_)-3) >= 0){// for this duration?
				r_stop_ = true;
				w_stop_ = true;
			}
		}
	}catch(std::exception &e) {
		QMessageBox::critical(this,"Error",(std::string("Error reading from camera 0: ")+=e.what()).c_str(),QMessageBox::Ok);
		return;
	}
	
}

void MainWindow::write_thread(void) {

		
		try{
			lsl::stream_info   info("VideoCaptureWrite", "Video", 1, fps_, lsl::cf_int32, boost::asio::ip::host_name());
			lsl::stream_outlet outlet(info);
			//cv::VideoWriter writer;
			//writer.open("foo.avi", -1, fps_, video_size_);
			//writer.open("foo.avi", CV_FOURCC('M', 'J', 'P', 'G'), fps_, video_size_);
			if(!writer.isOpened()){
				QMessageBox::critical(this, "Error", "failed to open write file", QMessageBox::Ok);
				//return;
			}
			while(!w_stop_){

				// make sure the camera is on and reading
				if(has_camera_){

					// sleep for a moment to rest the CPU
					//boost::this_thread::sleep(boost::posix_time::millisec(5)); // make this a variable?
					

					// try to empty the frame buffer
					while(!frame_buf_.empty()){
						//cv::Mat frame;

						// try to lock the mutex
						//boost::unique_lock<boost::timed_mutex> lock(mutex_, boost::try_to_lock);
						// if the reader isn't busy, write some data
						//if(lock.owns_lock()){
							
							// push the write data (counter and timestamp)
							outlet.push_sample(&frame_buf_[0].count,frame_buf_[0].timestamp);
							// write the frame
							writer.write(frame_buf_[0].frame);
							// pop the top frame
							mutex_.lock();
							frame_buf_.erase(frame_buf_.begin());
							mutex_.unlock();
							
						//}
						// otherwise, try again
					}
					
					
				}
			}


		}catch(std::exception &e) {
		QMessageBox::critical(this,"Error",(std::string("Error writing frames: ")+=e.what()).c_str(),QMessageBox::Ok);
		return;
	}



}

void MainWindow::closeEvent(QCloseEvent *ev) {

	//if (outletButtons_ || outletPosition_)
	//	ev->ignore();
}
void MainWindow::stop_read(void){

	if(r_thread_){
		try {
			r_stop_ = true;
			r_thread_->interrupt();
			r_thread_->join();
			r_thread_.reset();
			reader.release();
		} catch(std::exception &e) {
				QMessageBox::critical(this,"Error",(std::string("Could not stop camera read: ")+=e.what()).c_str(),QMessageBox::Ok);
				return;
		}
	}
}

void MainWindow::stop_write(void){
	if(w_thread_){
		try {
			w_stop_ = true;
			w_thread_->interrupt();
			w_thread_->join();
			w_thread_.reset();
			writer.release();
		} catch(std::exception &e) {
				QMessageBox::critical(this,"Error",(std::string("Could not stop camera write: ")+=e.what()).c_str(),QMessageBox::Ok);
				return;
		}
	}
}


MainWindow::~MainWindow() {
	stop_read();
	stop_write();
	delete ui;
}
