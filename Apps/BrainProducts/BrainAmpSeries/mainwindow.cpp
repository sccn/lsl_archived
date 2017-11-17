#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <chrono>
#include <iostream>
#include <lsl_cpp.h>

#ifdef WIN32
#include <winioctl.h>
#else
// dummy declarations to test compilation / static analysis on Linux/OS X
HANDLE INVALID_HANDLE_VALUE = nullptr;
enum Dummy {
	FILE_DEVICE_UNKNOWN,
	NORMAL_PRIORITY_CLASS,
	METHOD_BUFFERED,
	FILE_WRITE_DATA,
	FILE_READ_DATA,
	GENERIC_READ,
	GENERIC_WRITE,
	FILE_ATTRIBUTE_NORMAL,
	FILE_FLAG_WRITE_THROUGH,
	OPEN_EXISTING,
	HIGH_PRIORITY_CLASS
};
inline int GetCurrentProcess() { return 0; }
inline int SetPriorityClass(int, int) { return 0; }
inline int CTL_CODE(int, int, int, int) { return 0; }
inline void CloseHandle(HANDLE) {}
inline bool DeviceIoControl(HANDLE, int, void*, int, void*, int, void*, void*) { return true; }
inline HANDLE CreateFileA(const char*, int, int, void*, int, int, void*) { return static_cast<void*>(&INVALID_HANDLE_VALUE); }
inline int32_t GetLastError() { return 0; }
inline bool ReadFile(HANDLE, int16_t*, int, ulong*, void*) { return false; }
using DWORD = unsigned long;
using USHORT = uint16_t;
using ULONG = unsigned long;
using CHAR = signed char;
using UCHAR = unsigned char;
#endif

#include "BrainAmpIoCtl.h"

const double sampling_rate = 5000.0;
const char *error_messages[] = {"No error.","Loss lock.","Low power.","Can't establish communication at start.","Synchronisation error"};

MainWindow::MainWindow(QWidget *parent, const std::string &config_file): QMainWindow(parent),ui(new Ui::MainWindow),hDevice(NULL)
{
	ui->setupUi(this);

	// parse startup config file
	load_config(config_file);

	// make GUI connections
	connect(ui->actionQuit, &QAction::triggered, this, &MainWindow::close);
	connect(ui->linkButton, &QPushButton::clicked, this, &MainWindow::link);
	connect(ui->actionLoad_Configuration, &QAction::triggered, this, &MainWindow::load_config_dialog);
	connect(ui->actionSave_Configuration, &QAction::triggered, this, &MainWindow::save_config_dialog);

	g_unsampledMarkers  = false;
	g_sampledMarkers    = true;
	g_sampledMarkersEEG = false;
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

	// parse file
	try {
		read_xml(filename, pt);
	} catch(std::exception &e) {
		QMessageBox::information(this, "Error", QString("Cannot read config file: ") + e.what(), QMessageBox::Ok);
		return;
	}

	// get config values
	try {
		ui->deviceNumber->setValue(pt.get<int>("settings.devicenumber",1));
		ui->channelCount->setValue(pt.get<int>("settings.channelcount",32));
		ui->impedanceMode->setCurrentIndex(pt.get<int>("settings.impedancemode",0));
		ui->resolution->setCurrentIndex(pt.get<int>("settings.resolution",0));
		ui->dcCoupling->setCurrentIndex(pt.get<int>("settings.dccoupling",0));
		ui->chunkSize->setValue(pt.get<int>("settings.chunksize",32));
		ui->usePolyBox->setCheckState(pt.get<bool>("settings.usepolybox",false) ? Qt::Checked : Qt::Unchecked);
		ui->unsampledMarkers->setCheckState(pt.get<bool>("settings.unsampledmarkers",false) ? Qt::Checked : Qt::Unchecked);	
		ui->sampledMarkers->setCheckState(pt.get<bool>("settings.sampledmarkers",true) ? Qt::Checked : Qt::Unchecked);	
		ui->sampledMarkersEEG->setCheckState(pt.get<bool>("settings.sampledmarkersEEG",false) ? Qt::Checked : Qt::Unchecked);
		ui->channelLabels->clear();
		for (ptree::value_type &v: pt.get_child("channels.labels"))
			ui->channelLabels->appendPlainText(v.second.data().c_str());
	} catch(std::exception &) {
		QMessageBox::information(this,"Error in Config File","Could not read out config parameters.",QMessageBox::Ok);
		return;
	}
}

void MainWindow::save_config(const std::string &filename) {
	using boost::property_tree::ptree;
	ptree pt;

	// transfer UI content into property tree
	try {
		pt.put("settings.devicenumber",ui->deviceNumber->value());
		pt.put("settings.channelcount",ui->channelCount->value());
		pt.put("settings.impedancemode",ui->impedanceMode->currentIndex());
		pt.put("settings.resolution",ui->resolution->currentIndex());
		pt.put("settings.dccoupling",ui->dcCoupling->currentIndex());
		pt.put("settings.chunksize",ui->chunkSize->value());
		pt.put("settings.usepolybox",ui->usePolyBox->checkState()==Qt::Checked);
		pt.put("settings.unsampledmarkers",ui->unsampledMarkers->checkState()==Qt::Checked);
		pt.put("settings.sampledmarkers",ui->sampledMarkers->checkState()==Qt::Checked);
		pt.put("settings.sampledmarkersEEG",ui->sampledMarkersEEG->checkState()==Qt::Checked);
		std::vector<std::string> channelLabels;
		for (auto& label: ui->channelLabels->toPlainText().split('\n')) pt.add("channels.labels.label", label.toStdString());
	} catch(std::exception &e) {
		QMessageBox::critical(this, "Error", QString("Could not prepare settings for saving: ") + e.what(), QMessageBox::Ok);
	}

	// write to disk
	try {
		write_xml(filename, pt);
	} catch(std::exception &e) {
		QMessageBox::critical(this, "Error", QString("Could not write to config file: ") + e.what(), QMessageBox::Ok);
	}
}


// start/stop the BrainAmpSeries connection
void MainWindow::link() {
	DWORD bytes_returned;
	if (reader_thread_) {
		// === perform unlink action ===
		try {
			stop_ = true;
			reader_thread_->join();
			reader_thread_.reset();
			int res = SetPriorityClass(GetCurrentProcess(), NORMAL_PRIORITY_CLASS);
			if (hDevice!=nullptr) {
				DeviceIoControl(hDevice, IOCTL_BA_STOP, NULL, 0, NULL, 0, &bytes_returned, NULL);
				CloseHandle(hDevice);
				hDevice = NULL;
				
			}
		} catch(std::exception &e) {
			QMessageBox::critical(this,"Error",QString("Could not stop the background processing: ") + e.what(),QMessageBox::Ok);
			return;
		}

		// indicate that we are now successfully unlinked
		ui->linkButton->setText("Link");
	} else {
		// === perform link action ===

		try {
			// get the UI parameters...
			int deviceNumber = ui->deviceNumber->value();
			int channelCount = ui->channelCount->value();
			int impedanceMode = ui->impedanceMode->currentIndex();
			int resolution = ui->resolution->currentIndex();
			int dcCoupling = ui->dcCoupling->currentIndex();
			int chunkSize = ui->chunkSize->value();
			bool usePolyBox = ui->usePolyBox->checkState()==Qt::Checked;

			g_unsampledMarkers  = ui->unsampledMarkers->checkState()==Qt::Checked;
			g_sampledMarkers    = ui->sampledMarkers->checkState()==Qt::Checked;
			g_sampledMarkersEEG = ui->sampledMarkersEEG->checkState()==Qt::Checked;

			std::vector<std::string> channelLabels(channelCount);
			for(auto& label: ui->channelLabels->toPlainText().split('\n'))
				channelLabels.push_back(label.toStdString());
			if (channelLabels.size() != channelCount)
				throw std::runtime_error("The number of channels labels does not match the channel count device setting.");

			// try to open the device
			std::string deviceName = R"(\\.\BrainAmpUSB)" + std::to_string(deviceNumber);
			hDevice = CreateFileA(deviceName.c_str(), GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_WRITE_THROUGH, NULL);
			if (hDevice == INVALID_HANDLE_VALUE)
				throw std::runtime_error("Could not open USB device. Please make sure that the device is plugged in, turned on, and that the driver is installed correctly.");

			// get serial number
			ULONG serialNumber=0;
			if(!DeviceIoControl(hDevice, IOCTL_BA_GET_SERIALNUMBER, NULL, 0, &serialNumber, sizeof(serialNumber), &bytes_returned, NULL))
				std::cerr << "Could not get device serial number." << std::endl;

			// set up device parameters
			BA_SETUP setup = {0};
			setup.nChannels = channelCount;
			for (int c=0;c<channelCount;c++)
				setup.nChannelList[c] = c + (usePolyBox?-8:0);
			setup.nPoints = chunkSize;
			setup.nHoldValue = 0;
			for (int c=0;c<channelCount;c++)
				setup.nResolution[c] = resolution;
			for (int c=0;c<channelCount;c++)
				setup.nDCCoupling[c] = dcCoupling;
			setup.nLowImpedance = impedanceMode;

			pullUpHiBits = true;
			pullUpLowBits = true;
			g_pull_dir = (pullUpLowBits ? 0xff : 0) | (pullUpHiBits ? 0xff00 : 0);
			if(!DeviceIoControl(hDevice, IOCTL_BA_DIGITALINPUT_PULL_UP, &g_pull_dir, sizeof(g_pull_dir), NULL, 0, &bytes_returned, NULL))
				throw std::runtime_error("Could not apply pull up/down parameter.");

			if(!DeviceIoControl(hDevice, IOCTL_BA_SETUP, &setup, sizeof(setup), NULL, 0, &bytes_returned, NULL))
				throw std::runtime_error("Could not apply device setup parameters.");

			// start recording
			long acquire_eeg = 1;
			if (!DeviceIoControl(hDevice, IOCTL_BA_START, &acquire_eeg, sizeof(acquire_eeg), NULL, 0, &bytes_returned, NULL))
				throw std::runtime_error("Could not start recording.");

			// start reader thread
			stop_ = false;
			reader_thread_.reset(new std::thread(&MainWindow::read_thread,this,deviceNumber,serialNumber,impedanceMode,resolution,dcCoupling,chunkSize,channelCount,channelLabels));
		}

		catch(std::exception &e) {
			// try to decode the error message
			const char* msg = "Could not open USB device.";
			if (hDevice!=nullptr) {
				long error_code = 0;
				if (DeviceIoControl(hDevice, IOCTL_BA_ERROR_STATE, NULL, 0, &error_code, sizeof(error_code), &bytes_returned, NULL) && bytes_returned)
					msg = ((error_code&0xFFFF)>=0 && (error_code&0xFFFF)<=4) ? error_messages[error_code&0xFFFF] : "Unknown error (your driver version might not yet be supported).";
				else
					msg = "Could not retrieve error message because the device is closed";
				CloseHandle(hDevice);
				hDevice = nullptr;
			}
			QMessageBox::critical(this,"Error", QString("Could not initialize the BrainAmpSeries interface: ")+e.what()+" (driver message: "+msg+")",QMessageBox::Ok);
			return;
		}

		// done, all successful
		ui->linkButton->setText("Unlink");
	}
}

// background data reader thread
void MainWindow::read_thread(int deviceNumber, ULONG serialNumber, int impedanceMode, int resolution, int dcCoupling, int chunkSize, int channelCount, std::vector<std::string> channelLabels) {
	const float unit_scales[] = {0.1f, 0.5f, 10.f, 152.6f};
	const char *unit_strings[] = {"100 nV","500 nV","10 muV","152.6 muV"};
	// reserve buffers to receive and send data
	int chunk_words = chunkSize*(channelCount+1);
	std::vector<int16_t> recv_buffer(chunk_words, 0);
	std::size_t outbufferChannelCount = channelCount + (g_sampledMarkersEEG ? 1 : 0);
	std::vector<std::vector<float>> send_buffer(chunkSize, std::vector<float>(outbufferChannelCount));

	std::vector<std::vector<std::string>> marker_buffer(chunkSize, std::vector<std::string>(1));
	std::vector<std::string> s_mrkr;
	std::vector<uint16_t> trigger_buffer(chunkSize);

	int res = SetPriorityClass(GetCurrentProcess(), HIGH_PRIORITY_CLASS);

	// for keeping track of sampled marker stream data
	uint16_t mrkr = 0;
	uint16_t prev_mrkr = 0;

	// for keeping track of unsampled markers
	uint16_t us_prev_mrkr = 0;

	std::unique_ptr<lsl::stream_outlet> marker_outlet, s_marker_outlet;
	try {
		// create data streaminfo and append some meta-data
		auto stream_format = lsl::cf_float32;
		lsl::stream_info data_info("BrainAmpSeries-" + std::to_string(deviceNumber), "EEG",
		                           outbufferChannelCount, sampling_rate, stream_format,
		                           "BrainAmpSeries_" + std::to_string(deviceNumber) + "_" +std::to_string(serialNumber));
		lsl::xml_element channels = data_info.desc().append_child("channels");
		for (const auto& channelLabel : channelLabels)
			channels.append_child("channel")
				.append_child_value("label",channelLabel)
				.append_child_value("type","EEG")
				.append_child_value("unit","microvolts");
		if (g_sampledMarkersEEG){
			channels.append_child("channel")
				.append_child_value("label", "triggerStream")	
				.append_child_value("type","EEG")
				.append_child_value("unit","code");
		}

		data_info.desc().append_child("amplifier").append_child("settings")
			.append_child_value("low_impedance_mode",impedanceMode ? "true" : "false")
			.append_child_value("resolution",unit_strings[resolution])
			.append_child_value("dc_coupling",dcCoupling?"DC":"AC");
		data_info.desc().append_child("acquisition")
			.append_child_value("manufacturer","Brain Products")
			.append_child_value("serial_number", std::to_string(serialNumber));
		// make a data outlet
		lsl::stream_outlet data_outlet(data_info);

		//// create marker streaminfo and outlet
		//lsl::stream_info marker_info("BrainAmpSeries-" +std::to_string(deviceNumber) + "-Markers","Markers",1,0,lsl::cf_string,"BrainAmpSeries_" +std::to_string(deviceNumber) + "_" + std::to_string(serialNumber) + "_markers");
		//lsl::stream_outlet marker_outlet(marker_info);

		// create unsampled marker streaminfo and outlet
		
		if(g_unsampledMarkers) {
			lsl::stream_info marker_info("BrainAmpSeries-" + std::to_string(deviceNumber) + "-Markers","Markers",1,0,lsl::cf_string,"BrainAmpSeries_" + std::to_string(deviceNumber) + "_" + std::to_string(serialNumber) + "_markers");
			marker_outlet.reset(new lsl::stream_outlet(marker_info));
		}
		
		// create sampled marker streaminfo and outlet
		if(g_sampledMarkers) {
			lsl::stream_info marker_info("BrainAmpSeries-" + std::to_string(deviceNumber) + "-Sampled-Markers","sampledMarkers", 1,sampling_rate,lsl::cf_string,"BrainAmpSeries_" + std::to_string(deviceNumber) + "_" +std::to_string(serialNumber) + "_sampled_markers");
			s_marker_outlet.reset(new lsl::stream_outlet(marker_info));
		}
			
		// enter transmission loop		
		DWORD bytes_read;


		float scale = unit_scales[resolution];
		while (!stop_) {
			// read chunk into recv_buffer
			if(!ReadFile(hDevice, recv_buffer.data(), 2 * chunk_words, &bytes_read, NULL))
				throw std::runtime_error("Could not read data, error code " + std::to_string(GetLastError()));

			if (bytes_read <= 0){
				// CPU saver, this is ok even at higher sampling rates
				std::this_thread::sleep_for(std::chrono::milliseconds(1));
				continue;
			}
			
			if (bytes_read == 2*chunk_words) {
				double now = lsl::local_clock();
			
				

				// reformat into send_buffer
				for (int s=0;s<chunkSize;s++) {
				
					for (int c=0;c<channelCount;c++)
						send_buffer[s][c] = scale*recv_buffer[c + s*(channelCount+1)];
				
					// buffer for handling triggers
					//trigger_buffer[s] = recv_buffer[channelCount + s*(channelCount+1)];//???
					mrkr = recv_buffer[channelCount + s*(channelCount+1)];
					mrkr ^= g_pull_dir;
					trigger_buffer[s] = mrkr;

					if (g_sampledMarkersEEG)
						send_buffer[s][channelCount] = (mrkr==prev_mrkr ? 0.0 : static_cast<float>(mrkr));
					
					if (g_sampledMarkers || g_unsampledMarkers) {
						s_mrkr.clear();
						s_mrkr.push_back(mrkr==prev_mrkr ? "" : std::to_string(mrkr));
						if(mrkr!=prev_mrkr){std::cout << "s: " << s << " mrkr: " << s_mrkr[0] << std::endl;
							if(g_unsampledMarkers)marker_outlet->push_sample(&s_mrkr[0],now + (s + 1 - chunkSize)/sampling_rate);
						}
						marker_buffer.at(s) = s_mrkr;
					}
					prev_mrkr = mrkr;

				}

				// push data chunk into the outlet
				data_outlet.push_chunk(send_buffer,now);
			
				if(g_sampledMarkers)
					s_marker_outlet->push_chunk(marker_buffer,now);
			
				// push markers into outlet
				//if(g_unsampledMarkers) {
				//// push markers into outlet
				//	for (int s=0;s<chunkSize;s++){
				//		if (USHORT us_mrkr=trigger_buffer[s]){
				//			if (us_mrkr != us_prev_mrkr) {
				//				std::string str= std::to_string(us_mrkr);
				//				marker_outlet->push_sample(&str,now + (s + 1 - chunkSize)/sampling_rate);	
				//				us_prev_mrkr = us_mrkr;
				//			}
				//		}
				//	}
				//}
				
			} else {
				// check for errors
				long error_code=0;
				if (DeviceIoControl(hDevice, IOCTL_BA_ERROR_STATE, NULL, 0, &error_code, sizeof(error_code), &bytes_read, NULL) && error_code)
					throw std::runtime_error(((error_code&0xFFFF)>=0 && (error_code&0xFFFF)<=4) ? error_messages[error_code&0xFFFF] : "Unknown error (your driver version might not yet be supported).");
				std::this_thread::yield();
			}
		}

	}
	catch(std::exception &e) {
		// any other error
		QMessageBox::critical(nullptr, "Error", QString("Error during processing: ") + e.what(),QMessageBox::Ok);
	}
}

MainWindow::~MainWindow() {
	delete ui;
}
