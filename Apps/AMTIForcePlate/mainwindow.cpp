#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/foreach.hpp>
#include <boost/asio.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>


// command types to send to the device
const char cmd_start = 0x51;		// start streaming
const char cmd_stop = 0x52;			// stop streaming
const char cmd_autozero = 0x53;		// calibrate to zero at current load
const char cmd_50Hz = 0x72;			// set to 50-Hz sampling
const char cmd_100Hz = 0x66;		// set to 100-Hz sampling
const char cmd_200Hz = 0x76;		// set to 200-Hz sampling

// acknowledgements that can come back from the device
const char ack_start_single = 0x55;		// transmission starting, one device connected
const char ack_start_dual = 0x56;		// transmission starting, two devices connected
const char ack_autozero_single = 0x54;	// calibration completed, one device connected
const char ack_autozero_dual = 0x64;	// calibration completed, two devices connected

// transmission settings
const int buffer_size = 32768;		// for the serial port
const int samples_per_chunk = 5;	// for the lab streaming layer


MainWindow::MainWindow(QWidget *parent, const std::string &config_file) :
QMainWindow(parent),
ui(new Ui::MainWindow)
{
	ui->setupUi(this);

	// parse startup config file
	load_config(config_file);

	// make GUI connections
	QObject::connect(ui->actionQuit, SIGNAL(triggered()), this, SLOT(close()));
	QObject::connect(ui->linkButton, SIGNAL(clicked()), this, SLOT(link_amti()));
	QObject::connect(ui->actionLoad_Configuration, SIGNAL(triggered()), this, SLOT(load_config_dialog()));
	QObject::connect(ui->actionSave_Configuration, SIGNAL(triggered()), this, SLOT(save_config_dialog()));
}


void MainWindow::load_config_dialog() {
	QString sel = QFileDialog::getOpenFileName(this,"Load Configuration File","","Configuration Files (*.cfg)");
	if (!sel.isEmpty())
		load_config(sel.toStdString());
}

void MainWindow::save_config_dialog() {
	QString sel = QFileDialog::getSaveFileName(this,"Save Configuration File","","Configuration Files (*.cfg)");
	if (!sel.isEmpty())
		save_config(sel.toStdString());
}

void MainWindow::closeEvent(QCloseEvent *ev) {
	if (reader_thread_)
		ev->ignore();
}

void MainWindow::load_config(const std::string &filename) {
	using boost::property_tree::ptree;
	ptree pt;
	unsigned k=0;
	// parse file
	try {
		read_xml(filename, pt);
	} catch(std::exception &e) {
		QMessageBox::information(this,"Error",(std::string("Cannot read config file: ")+= e.what()).c_str(),QMessageBox::Ok);
		return;
	}

	// get config values
	try {
		ui->comPort->setValue(pt.get<int>("settings.comport",1));
		ui->samplingRate->setCurrentIndex(pt.get<int>("settings.samplingrate",0));
		// there can be 2 calibration matrices since two devices can be connected in a multi-plexed manner; the one below is calibration data for our force plate -- the numbers are device-specific and vendor-provided
        calibstr_[0] = pt.get<std::string>("settings.calibration_matrix1","[0.0088010,-0.0003439,-0.0002374,-0.0091382, 0.0002286,-0.0011453, 0.0087575,-0.0000330, 0.0001908,-0.0085257,-0.0003653,-0.0004479, \
																	       -0.0000572, 0.0089623, 0.0005131,-0.0003076, 0.0086073,-0.0000008,-0.0002277,-0.0084864,-0.0010120, 0.0004282,-0.0087719,-0.0007067, \
                                                                           -0.0016028,-0.0009292,-0.0885598, 0.0000639, 0.0000603,-0.0867306,-0.0003682, 0.0002382,-0.0868983,-0.0004845, 0.0002808,-0.0880958, \
                                                                            0.0000000, 0.0000000,-0.6305114, 0.0000000, 0.0000000,-0.6221169, 0.0000000, 0.0000000, 0.6244931, 0.0000000, 0.0000000, 0.6209047, \
                                                                            0.0000000, 0.0000000, 0.6524594, 0.0000000, 0.0000000,-0.6378316, 0.0000000, 0.0000000, 0.6425910, 0.0000000, 0.0000000,-0.6428833, \
                                                                           -0.0931016, 0.0958635, 0.0012947, 0.0966692,-0.0920663, 0.0012679, 0.0926413,-0.0907731, 0.0012704,-0.0901891, 0.0938268, 0.0012879]");
        calibstr_[1] = pt.get<std::string>("settings.calibration_matrix2","[]");
		// for each device...
		for (unsigned d=0;d<max_devices;d++) {
			if (calibstr_[d].size()>3) {
				// split matrix into number-strings
				std::string tmpstr = boost::algorithm::trim_copy(calibstr_[d]);
				std::vector<std::string> tmp; boost::algorithm::split(tmp,tmpstr.substr(1,tmpstr.size()-2),boost::algorithm::is_any_of(", \t"),boost::algorithm::token_compress_on);
				// transfer into float matrix...				
				for (unsigned y=0;y<out_chns_per_device;y++)
					for (unsigned x=0;x<in_chns_per_device;x++) {
						while(boost::algorithm::trim_copy(tmp[k]).empty())
							k++;
						calib_[d][x][y] = boost::lexical_cast<float>(tmp[k++]);
					}
			} else {
				// otherwise use the identity matrix
				for (unsigned y=0;y<out_chns_per_device;y++)
					for (unsigned x=0;x<in_chns_per_device;x++)
						calib_[d][x][y] = (x==y);
			}
		}
	} catch(std::exception &e) {
		QMessageBox::information(this,"Error in Config File",(std::string("Could not prepare settings for saving: ")+=e.what()).c_str(),QMessageBox::Ok);
		return;
	}
}

void MainWindow::save_config(const std::string &filename) {
	using boost::property_tree::ptree;
	ptree pt;

	// transfer UI content into property tree
	try {
		pt.put("settings.comport",ui->comPort->value());
		pt.put("settings.samplingrate",ui->samplingRate->currentIndex());
		pt.put("settings.calibration_matrix1",calibstr_[0]);
		pt.put("settings.calibration_matrix2",calibstr_[1]);
		// note: calibration matrix is not GUI-editable, no need to save
	} catch(std::exception &e) {
		QMessageBox::critical(this,"Error",(std::string("Could not prepare settings for saving: ")+=e.what()).c_str(),QMessageBox::Ok);
	}

	// write to disk
	try {
		write_xml(filename, pt);
	} catch(std::exception &e) {
		QMessageBox::critical(this,"Error",(std::string("Could not write to config file: ")+=e.what()).c_str(),QMessageBox::Ok);
	}
}


// start/stop the AMTI connection
void MainWindow::link_amti() {
	if (reader_thread_) {
		// === perform unlink action ===
		try {
			stop_ = true;
			reader_thread_->interrupt();
			reader_thread_->join();
			reader_thread_.reset();
		} catch(std::exception &e) {
			QMessageBox::critical(this,"Error",(std::string("Could not stop the background processing: ")+=e.what()).c_str(),QMessageBox::Ok);
			return;
		}

		// indicate that we are now successfully unlinked
		ui->linkButton->setText("Link");
	} else {
		// === perform link action ===
		HANDLE hPort = NULL;
		DWORD dwBytesWritten, dwBytesRead;
		unsigned char cmd=0, resp=0;
		int bytes_skipped=0;
		try {
			// get the UI parameters...
			int comPort = ui->comPort->value();
			int samplingRate = boost::lexical_cast<int>(ui->samplingRate->itemText(ui->samplingRate->currentIndex()).toStdString());
			std::cout<<samplingRate<<std::endl;
			int numDevices = 0;

			// try to open the serial port
			std::string fname = "\\\\.\\COM" + boost::lexical_cast<std::string>(comPort);
			hPort = CreateFileA(fname.c_str(),GENERIC_READ|GENERIC_WRITE,0,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0);
			if (hPort == INVALID_HANDLE_VALUE)
				throw std::runtime_error("Could not open serial port. Please make sure that the device is plugged in, turned on, and that you have the correct port number.");

			// set some timeouts
			COMMTIMEOUTS timeouts = {0};
			timeouts.ReadIntervalTimeout = 10000;
			timeouts.ReadTotalTimeoutConstant = 5000;
			timeouts.ReadTotalTimeoutMultiplier = 50;
			timeouts.WriteTotalTimeoutConstant = 500;
			timeouts.WriteTotalTimeoutMultiplier = 50;
			SetCommTimeouts(hPort,&timeouts);

			SetupComm(hPort,buffer_size,buffer_size);

			// send a stop command and give it some time to settle
			cmd = cmd_stop;
			if (!WriteFile(hPort,(LPSTR)&cmd,1,&dwBytesWritten,NULL))
				throw std::runtime_error("Cannot send commands to the device.");
			boost::this_thread::sleep(boost::posix_time::millisec(100));

			// set the desired baud rate on the windows serial port
			DCB dcb = {0};
			dcb.DCBlength = sizeof(DCB);
			GetCommState(hPort,&dcb);
			dcb.ByteSize = 8;
			// this has to be 57600 to set the baud rate on the force plate
			dcb.BaudRate = 57600; 
			if (!SetCommState(hPort,&dcb))
				QMessageBox::information(this,"Note","Could not set baud rate of serial connection.",QMessageBox::Ok);
			
			resp = 0;
			bytes_skipped=0;

			// set the baud rate on the device
			cmd = (samplingRate==50) ? cmd_50Hz : ((samplingRate==100) ? cmd_100Hz : cmd_200Hz);
			if (!WriteFile(hPort,(LPSTR)&cmd,1,&dwBytesWritten,NULL))
				throw std::runtime_error("Cannot send commands to the device.");
			
			while (resp != cmd) {
				// communicate baud rate
				if ((!ReadFile(hPort,&resp,1,&dwBytesRead,NULL) || !dwBytesRead)
					throw std::runtime_error("Did not receive a response from the device (after baud rate choice). Please make sure that your device is set up properly and that your COM port supports a baud rate of " + boost::lexical_cast<std::string>(dcb.BaudRate) + ".");
		
				if (++bytes_skipped > 2*buffer_size)
					throw std::runtime_error("Device does not response in an expected way. Please make sure that this software is compatible with your type of force plate.");

			}

			// now set the baud rate on the port to the desired rate
			dcb.BaudRate = samplingRate == 50 ? 57600 : 115200;
			if (!SetCommState(hPort,&dcb))
				QMessageBox::information(this,"Note","Could not set baud rate of serial connection.",QMessageBox::Ok);
			
			
			// perform the auto-zero calibration and check the ack (may take up to 10 seconds)
			cmd = cmd_autozero;
			if (!WriteFile(hPort,(LPSTR)&cmd,1,&dwBytesWritten,NULL))
				throw std::runtime_error("Device stopped responding after baud-rate reset.");
			if (!ReadFile(hPort,&resp,1,&dwBytesRead,NULL) || !dwBytesRead)
				throw std::runtime_error("Device stopped responding during auto-zero calibration.");
			if (resp != ack_autozero_single && resp != ack_autozero_dual)
				throw std::runtime_error("Received an unexpected response from the device. Please make sure that this software is compatible with your type of force plate.");
			numDevices = (resp == ack_autozero_single) ? 1 : 2;

			// start transmission
			cmd = cmd_start;
			if (!WriteFile(hPort,(LPSTR)&cmd,1,&dwBytesWritten,NULL))
				throw std::runtime_error("Device stopped responding while sending the start command.");
			
			// and check the ack (discard any reponse that came after the auto-zero calibration)
			resp = 0;
			bytes_skipped=0;
			while ((resp != ack_start_single) && (resp != ack_start_dual)) {
				if (!ReadFile(hPort,&resp,1,&dwBytesRead,NULL) || !dwBytesRead)
					throw std::runtime_error("Device stopped responding while waiting for the start ack.");
				if (++bytes_skipped > 256)
					throw std::runtime_error("Device stopped responding while waiting for the start ack or gave unexpected outputs.");
			}
			if ((resp != ack_start_single) && (resp != ack_start_dual))
				throw std::runtime_error("Device produces unknown outputs in response to the start command.");
			if (resp != ((numDevices == 1) ? ack_start_single : ack_start_dual))
				throw std::runtime_error("Device is reporting a change in the number of connected force plates. Please re-try.");
			
			// start reading
			stop_ = false;
			reader_thread_.reset(new boost::thread(&MainWindow::read_thread,this,hPort,comPort,samplingRate,numDevices));
		}
		catch(std::exception &e) {
			if (hPort != INVALID_HANDLE_VALUE)g
				CloseHandle(hPort);
			QMessageBox::critical(this,"Error",(std::string("Could not initialize the AMTI interface: ")+=e.what()).c_str(),QMessageBox::Ok);
			// sometimes it fails, (randomly from what I've seen) so let the user know to try again, and to powercycle if no joy comes 
			QMessageBox::warning(this,"Retry","Please try to link again. If error persists, power cycle the force plate(s) and restart the app.",QMessageBox::Ok);
			return;
		}

		// done, all successful
		ui->linkButton->setText("Unlink");
	}
}


// background data reader thread
void MainWindow::read_thread(HANDLE hPort, int comPort, int samplingRate, int numDevices) {
	std::string deviceTag[] = {"1","2","3","4"};
	int channelCount = numDevices*out_chns_per_device+1; // plus 1 trigger channel

	// create streaminfo
	lsl::stream_info info("AMTI Force Plate","Force",channelCount,samplingRate,lsl::cf_float32,"AMTI_Plate" + boost::lexical_cast<std::string>(channelCount) + "_" + boost::asio::ip::host_name());
	// append some meta-data
	lsl::xml_element channels = info.desc().append_child("channels");
	for (int k=0;k<numDevices;k++) {
		channels.append_child("channel")
			.append_child_value("label",("Fx" + deviceTag[k]).c_str())
			.append_child_value("type","ForceX");
		channels.append_child("channel")
			.append_child_value("label",("Fy" + deviceTag[k]).c_str())
			.append_child_value("type","ForceY");
		channels.append_child("channel")
			.append_child_value("label",("Fz" + deviceTag[k]).c_str())
			.append_child_value("type","ForceZ");
		channels.append_child("channel")
			.append_child_value("label",("Mx" + deviceTag[k]).c_str())
			.append_child_value("type","MomentumX");
		channels.append_child("channel")
			.append_child_value("label",("My" + deviceTag[k]).c_str())
			.append_child_value("type","MomentumY");
		channels.append_child("channel")
			.append_child_value("label",("Mz" + deviceTag[k]).c_str())
			.append_child_value("type","MomentumZ");
	}
	channels.append_child("channel")
		.append_child_value("label","Trig")
		.append_child_value("type","Trigger");
	info.desc().append_child("acquisition")
		.append_child_value("manufacturer","AMTI");

	// make a new outlet
	lsl::stream_outlet outlet(info,samples_per_chunk);
	
	std::vector<float> sample(channelCount);									// the final sample
	std::vector<unsigned char> shift_buffer(numDevices*in_chns_per_device*2);	// this is a circular buffer that holds the last data words that make up a measurement
	DWORD bytes_read;

	// enter transmission loop
	while (!stop_) {
		// shift in bytes at the end until we get a complete measurement
		while (!stop_) {
			for (unsigned k=0;k<shift_buffer.size()-1;k++)
				shift_buffer[k] = shift_buffer[k+1];
			if (!ReadFile(hPort,&shift_buffer.back(),1,&bytes_read,NULL)) {
				QMessageBox::critical(this,"Error","Connection broke off.",QMessageBox::Ok);
				return;
			}
			// check termination condition
			if ((shift_buffer[1]>>4) != 0 && (shift_buffer[1]>>4) != 0xF)
				continue;
			bool match = true;
			for (unsigned k=1;k<shift_buffer.size()/2;k++)
				if ((shift_buffer[1+k*2]>>4) != k) {
					match = false;
					break;
				}
			if (match)
				break;
		}
		// assign the trigger channel
		sample.back() = shift_buffer[1]>>4 == 0xF;
		// calculate the remaining values
		short *values = (short*)(&shift_buffer[0]);
		for (unsigned k=0;k<shift_buffer.size()/2;k++)
			values[k] = (values[k] & 0x0FFF) - 2048;
		// for each device...
		for (unsigned d=0;d<numDevices;d++) {
			// for each output channel...
			for (unsigned oc=0; oc<out_chns_per_device; oc++) {
				double acc = 0;
				// perform an inner product across all input channels with the calibration mapping for this device & output channel
				for (unsigned ic=0; ic<in_chns_per_device; ic++)
					acc += values[d*in_chns_per_device + ic] * calib_[d][ic][oc];
				sample[oc] = acc;
			}
		}		
		// push into the outlet
		outlet.push_sample(sample);
	}
	CloseHandle(hPort);
}

MainWindow::~MainWindow() {
	delete ui;
}
