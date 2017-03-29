#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/foreach.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

const double sampling_rate = 5000.0;
const char *error_messages[] = {"No error.","Loss lock.","Low power.","Can't establish communication at start.","Synchronisation error"};

MainWindow::MainWindow(QWidget *parent, const std::string &config_file): QMainWindow(parent),ui(new Ui::MainWindow),hDevice(NULL)
{
	ui->setupUi(this);

	// parse startup config file
	load_config(config_file);

	// make GUI connections
	QObject::connect(ui->actionQuit, SIGNAL(triggered()), this, SLOT(close()));
	QObject::connect(ui->linkButton, SIGNAL(clicked()), this, SLOT(link()));
	QObject::connect(ui->actionLoad_Configuration, SIGNAL(triggered()), this, SLOT(load_config_dialog()));
	QObject::connect(ui->actionSave_Configuration, SIGNAL(triggered()), this, SLOT(save_config_dialog()));

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
		QMessageBox::information(this,"Error",(std::string("Cannot read config file: ")+= e.what()).c_str(),QMessageBox::Ok);
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
		BOOST_FOREACH(ptree::value_type &v, pt.get_child("channels.labels"))
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
		boost::algorithm::split(channelLabels,ui->channelLabels->toPlainText().toStdString(),boost::algorithm::is_any_of("\n"));
		BOOST_FOREACH(std::string &v, channelLabels)
			pt.add("channels.labels.label", v);
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


// start/stop the BrainAmpSeries connection
void MainWindow::link() {
	DWORD bytes_returned;
	if (reader_thread_) {
		// === perform unlink action ===
		try {
			stop_ = true;
			reader_thread_->interrupt();
			reader_thread_->join();
			reader_thread_.reset();
			if (hDevice>0) {
				DeviceIoControl(hDevice, IOCTL_BA_STOP, NULL, 0, NULL, 0, &bytes_returned, NULL);
				CloseHandle(hDevice);
				hDevice = NULL;
			}
		} catch(std::exception &e) {
			QMessageBox::critical(this,"Error",(std::string("Could not stop the background processing: ")+=e.what()).c_str(),QMessageBox::Ok);
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

			std::vector<std::string> channelLabels;
			boost::algorithm::split(channelLabels,ui->channelLabels->toPlainText().toStdString(),boost::algorithm::is_any_of("\n"));
			if (channelLabels.size() != channelCount)
				throw std::runtime_error("The number of channels labels does not match the channel count device setting.");

			// try to open the device
			std::string deviceName = "\\\\.\\BrainAmpUSB" + boost::lexical_cast<std::string>(deviceNumber);
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
			if(!DeviceIoControl(hDevice, IOCTL_BA_SETUP, &setup, sizeof(setup), NULL, 0, &bytes_returned, NULL))
				throw std::runtime_error("Could not apply device setup parameters.");

			// start recording
			long acquire_eeg = 1;
			if (!DeviceIoControl(hDevice, IOCTL_BA_START, &acquire_eeg, sizeof(acquire_eeg), NULL, 0, &bytes_returned, NULL))
				throw std::runtime_error("Could not start recording.");


			// start reader thread
			stop_ = false;
			reader_thread_.reset(new boost::thread(&MainWindow::read_thread,this,deviceNumber,serialNumber,impedanceMode,resolution,dcCoupling,chunkSize,channelCount, channelLabels));
		}

		catch(std::exception &e) {
			// try to decode the error message
			std::string msg = "Could not open USB device.";
			if (hDevice>0) {
				long error_code = 0;
				if (DeviceIoControl(hDevice, IOCTL_BA_ERROR_STATE, NULL, 0, &error_code, sizeof(error_code), &bytes_returned, NULL) && bytes_returned)
					msg = ((error_code&0xFFFF)>=0 && (error_code&0xFFFF)<=4) ? error_messages[error_code&0xFFFF] : "Unknown error (your driver version might not yet be supported).";
				else
					msg = "Could not retrieve error message because the device is closed";
				CloseHandle(hDevice);
				hDevice = NULL;
			}
			QMessageBox::critical(this,"Error",("Could not initialize the BrainAmpSeries interface: "+(e.what()+(" (driver message: "+msg+")"))).c_str(),QMessageBox::Ok);
			return;
		}

		// done, all successful
		ui->linkButton->setText("Unlink");
	}
}

// background data reader thread
void MainWindow::read_thread(int deviceNumber, ULONG serialNumber, int impedanceMode, int resolution, int dcCoupling, int chunkSize, int channelCount, std::vector<std::string> channelLabels) {
	
	const float unit_scales[] = {0.1,0.5,10,152.6};
	const char *unit_strings[] = {"100 nV","500 nV","10 µV","152.6 µV"};
	
	// reserve buffers to receive and send data
	int chunk_words = chunkSize*(channelCount+1);
	boost::int16_t *recv_buffer = new boost::int16_t[chunk_words];
	std::vector<std::vector<float> > send_buffer(chunkSize,std::vector<float>(channelCount));

	std::vector<unsigned> trigger_buffer(chunkSize);
	std::vector<std::vector<std::string>> marker_buffer(chunkSize, std::vector<std::string>(1));
	std::vector<std::string> s_mrkr;
	
	try {
		// create data streaminfo and append some meta-data
		lsl::stream_info data_info("BrainAmpSeries-" + boost::lexical_cast<std::string>(deviceNumber),"EEG",channelCount + (g_sampledMarkersEEG ? 1 : 0),sampling_rate,lsl::cf_float32,"BrainAmpSeries_" + boost::lexical_cast<std::string>(deviceNumber) + "_" + boost::lexical_cast<std::string>(serialNumber));
		lsl::xml_element channels = data_info.desc().append_child("channels");
		for (int k=0;k<channelLabels.size();k++)
			channels.append_child("channel")
				.append_child_value("label",channelLabels[k].c_str())
				.append_child_value("type","EEG")
				.append_child_value("unit","microvolts");
				// todo: label aux channels?
		if(g_sampledMarkersEEG) {
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
			.append_child_value("serial_number",boost::lexical_cast<std::string>(serialNumber).c_str());
		// make a data outlet
		lsl::stream_outlet data_outlet(data_info);

		// create unsampled marker streaminfo and outlet
		lsl::stream_outlet *marker_outlet;
		if(g_unsampledMarkers) {
			lsl::stream_info marker_info("BrainAmpSeries-" + boost::lexical_cast<std::string>(deviceNumber) + "-Markers","Markers",1,0,lsl::cf_string,"BrainAmpSeries_" + boost::lexical_cast<std::string>(deviceNumber) + "_" + boost::lexical_cast<std::string>(serialNumber) + "_markers");
			marker_outlet = new lsl::stream_outlet(marker_info);
		}
		
		// create sampled marker streaminfo and outlet
		lsl::stream_outlet *s_marker_outlet;
		if(g_sampledMarkers) {
			lsl::stream_info marker_info("BrainAmpSeries-" + boost::lexical_cast<std::string>(deviceNumber) + "-Sampled-Markers","Markers",1,sampling_rate,lsl::cf_string,"BrainAmpSeries_" + boost::lexical_cast<std::string>(deviceNumber) + "_" + boost::lexical_cast<std::string>(serialNumber) + "_sampled_markers");
			s_marker_outlet = new lsl::stream_outlet(marker_info);
		}


		// enter transmission loop		
		DWORD bytes_read;
		int last_mrk = 0;
		int prev_markerSampled = 0;
		int prev_markerEEG = 0;
		float f_mrkr;

		float scale = unit_scales[resolution];
		while (!stop_) {
			// read chunk into recv_buffer
			if(!ReadFile(hDevice,recv_buffer,2*chunk_words,&bytes_read, NULL))
				throw std::runtime_error(("Could not read data, error code " + boost::lexical_cast<std::string>(GetLastError())).c_str());
			if (bytes_read == 2*chunk_words) {

				double now = lsl::local_clock();

				// reformat into send_buffer
				for (int s=0;s<chunkSize;s++){
					// buffer for handling triggers
					trigger_buffer[s] = recv_buffer[channelCount + s*(channelCount+1)];

					for (int c=0;c<channelCount;c++) 
						send_buffer[s][c] = scale*recv_buffer[c + s*(channelCount+1)];

					if (g_sampledMarkersEEG) {
						f_mrkr = (trigger_buffer[s] == prev_markerEEG ? 0.0 : boost::lexical_cast<float>(trigger_buffer[s]));
						prev_markerEEG = trigger_buffer[s];
						send_buffer[channelCount] = f_mrkr;
					}
			
					if (g_sampledMarkers) {
						s_mrkr.clear();
						s_mrkr.push_back(trigger_buffer[s] == prev_markerSampled ? "" : boost::lexical_cast<std::string>(trigger_buffer[s]));
						marker_buffer.at(s) = s_mrkr;
						//std::cout << "s: " << s << std::endl;
						prev_markerSampled = trigger_buffer[s];
					}
					
					
				}

				// push data chunk into the outlet
				data_outlet.push_chunk(send_buffer,now);

				if(g_sampledMarkers)
					s_marker_outlet->push_chunk(marker_buffer,now);

				// push markers into outlet
				if(g_unsampledMarkers) {
				// push markers into outlet
					for (int s=0;s<chunkSize;s++)
						if (int mrk=trigger_buffer[s]){
							if (mrk != last_mrk) {
								std::string mrk_string = boost::lexical_cast<std::string>(mrk);
								marker_outlet->push_sample(&mrk_string,now + (s + 1 - chunkSize)/sampling_rate);
								last_mrk = mrk;
							}
						}
				}

			} else {
				// check for errors
				long error_code=0;
				if (DeviceIoControl(hDevice, IOCTL_BA_ERROR_STATE, NULL, 0, &error_code, sizeof(error_code), &bytes_read, NULL) && error_code)
					throw std::runtime_error(((error_code&0xFFFF)>=0 && (error_code&0xFFFF)<=4) ? error_messages[error_code&0xFFFF] : "Unknown error (your driver version might not yet be supported).");
				boost::thread::yield();
			}
		}
		// need to explicitly delete these outlets
		if(g_unsampledMarkers)delete(marker_outlet);
		if(g_sampledMarkers)delete(s_marker_outlet);
	}
	catch(boost::thread_interrupted &) {
		// thread was interrupted: no error
	}
	catch(std::exception &e) {
		// any other error
		QMessageBox::critical(this,"Error",(std::string("Error during processing: ")+=e.what()).c_str(),QMessageBox::Ok);
	}
	delete recv_buffer;
}

MainWindow::~MainWindow() {
	delete ui;
}
