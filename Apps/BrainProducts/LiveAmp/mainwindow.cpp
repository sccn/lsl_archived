
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/foreach.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#define VERSIONSTREAM(version) version.Major << "." << version.Minor << "." << version.Build << "." << version.Revision

HANDLE mDevice;
const double sampling_rates[] = {250,500,1000};
int mSamplingRate;
int mChannelCount = 0;
std::vector<int> mChannelIndexes;
std::vector<int> eegIndexes;
std::vector<int> auxIndexes;
std::vector<int> accIndexes;
std::vector<int> mTrigInIndexes;


int LiveAmp_SampleSize(HANDLE hDevice, int *typeArray, int* usedChannelsCnt);

MainWindow::MainWindow(QWidget *parent, const std::string &config_file): QMainWindow(parent),ui(new Ui::MainWindow)
{
	ui->setupUi(this);

	// parse startup config file
	load_config(config_file);

	// make GUI connections
	QObject::connect(ui->actionQuit, SIGNAL(triggered()), this, SLOT(close()));
	QObject::connect(ui->linkButton, SIGNAL(clicked()), this, SLOT(link()));
	QObject::connect(ui->actionLoad_Configuration, SIGNAL(triggered()), this, SLOT(load_config_dialog()));
	QObject::connect(ui->actionSave_Configuration, SIGNAL(triggered()), this, SLOT(save_config_dialog()));

	mDevice = NULL;
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
		std::string serialN = pt.get<std::string>("settings.deviceSerialNumber", "x-0077");
		ui->deviceSerialNumber->setText(serialN.c_str()); 
		ui->channelCount->setValue(pt.get<int>("settings.channelcount",32));
		ui->chunkSize->setValue(pt.get<int>("settings.chunksize",10));
		ui->samplingRate->setCurrentIndex(pt.get<int>("settings.samplingrate",2));
		ui->useBipolar->setCheckState(pt.get<bool>("settings.useBipolar",false) ? Qt::Checked : Qt::Unchecked);
		ui->useAUX->setCheckState(pt.get<bool>("settings.useAux",false) ? Qt::Checked : Qt::Unchecked);
		ui->useACC->setCheckState(pt.get<bool>("settings.useACC",true) ? Qt::Checked : Qt::Unchecked);
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
		pt.put("settings.deviceSerialNumber",ui->deviceSerialNumber->text().toStdString());
		pt.put("settings.channelcount",ui->channelCount->value());
		pt.put("settings.chunksize",ui->chunkSize->value());
		pt.put("settings.samplingrate",ui->samplingRate->currentIndex());
		pt.put("settings.useBipolar",ui->useBipolar->checkState()==Qt::Checked);
		pt.put("settings.useAux",ui->useAUX->checkState()==Qt::Checked);
		pt.put("settings.activeshield",ui->useACC->checkState()==Qt::Checked);
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


// start/stop the LiveAmp connection
void MainWindow::link() {
	if (reader_thread_) {
		// === perform unlink action ===
		try {
			stop_ = true;
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

		try {
			// get the UI parameters...
			std::string serialN = ui->deviceSerialNumber->text().toStdString();
			mChannelCount = ui->channelCount->value();
			int chunkSize = ui->chunkSize->value();
			mSamplingRate = sampling_rates[ui->samplingRate->currentIndex()];
			
			bool useAUX = ui->useAUX->checkState()==Qt::Checked;
			bool useACC = ui->useACC->checkState()==Qt::Checked;

			std::vector<std::string> channelLabels;
			boost::algorithm::split(channelLabels,ui->channelLabels->toPlainText().toStdString(),boost::algorithm::is_any_of("\n"));
			if (channelLabels.size() != mChannelCount)
				throw std::runtime_error("The number of channels labels does not match the channel count device setting.");

			// try to verify that the requested device is available
			t_VersionNumber version;
			GetLibraryVersion(&version);
			std::cout << "Library Version " << VERSIONSTREAM(version) << std::endl;

			if(!InitializeLiveAmp())
				return;

			// start reader thread
			stop_ = false;
			int deviceNumber=0;
			reader_thread_.reset(new boost::thread(&MainWindow::read_thread, this, chunkSize, mSamplingRate, useAUX, useACC,channelLabels));
		}

		catch(std::exception &e) {
			// generate error message
			std::string msg = "Could not retrieve driver error message";
			int errorcode=0; 

			if (mDevice) {
				ampCloseDevice(mDevice);
				mDevice = NULL;
			}

			QMessageBox::critical(this,"Error",("Could not initialize the LiveAmp interface: "+(e.what()+(" (driver message: "+msg+")"))).c_str(),QMessageBox::Ok);
			return;
		}

		// done, all successful
		ui->linkButton->setText("Unlink");
	}
}

bool _resample = false;
int numbresSamples = 0;
// background data reader thread
void MainWindow::read_thread(int chunkSize, int samplingRate, bool useAUX, bool useACC, std::vector<std::string> channelLabels) {
	
	int res = 0;
	bool started = false;	
	BYTE *buffer = NULL;
	std::string serialNumber = ui->deviceSerialNumber->text().toStdString();

	
	try {		
	
		if(!ConfigureLiveAmp())  // configure LiveAMp parameters
			return;
		
		if(!EnableLiveAmpChannels(useAUX, useACC))
			return;

		if(!GenerateUsedPhysicalChannelIndexes())
			return;

		
		int dataTypeArray[80];
		int usedChannelsCounter=0;
		int sampleSize = LiveAmp_SampleSize(mDevice, dataTypeArray, &usedChannelsCounter);

		res = ampStartAcquisition(mDevice);
		if(res != AMP_OK)					
			throw std::runtime_error(("Error starting acquisition, error code:  " + boost::lexical_cast<std::string>(res)).c_str());

		started = true;

		std::vector<std::vector<double>> liveamp_buffer(chunkSize,std::vector<double>(usedChannelsCounter));
		std::vector<std::vector<double>> lslsend_buffer(chunkSize,std::vector<double>(usedChannelsCounter));
		std::vector<uint16_t> trigger_buffer(1);

		std::vector<std::string> adjustedChannelLabels;
		AdjustChannelLabels(channelLabels, adjustedChannelLabels);

		// create data streaminfo and append some meta-data
		lsl::stream_info data_info("LiveAmpSN- " + serialNumber,"EEG", adjustedChannelLabels.size(), samplingRate,lsl::cf_float32,"LiveAmpSN-" + serialNumber);
		lsl::xml_element channels = data_info.desc().append_child("channels");
		for (std::size_t k=0;k<adjustedChannelLabels.size();k++)
			channels.append_child("channel")
				.append_child_value("label",adjustedChannelLabels[k].c_str())
				.append_child_value("type","EEG")
				.append_child_value("unit","microvolts");
		data_info.desc().append_child("acquisition")
			.append_child_value("manufacturer","Brain Products");
		// make a data outlet
		lsl::stream_outlet data_outlet(data_info);

		// create marker streaminfo and outlet
		lsl::stream_info marker_info("LiveAmpSN- " + serialNumber + "-Markers","Markers",1,0,lsl::cf_string,"LiveAmpSN-" + serialNumber + "_markers");
		lsl::stream_outlet marker_outlet(marker_info);
			
		// enter transmission loop		
		int last_mrk = 0;
		int bytes_read, samples_read;

		// read data stream from amplifier
		int32_t BufferSize = (chunkSize + 10) * sampleSize;
		buffer = new BYTE[BufferSize];

		trigger_buffer.clear();
		lslsend_buffer.clear();
		//QMessageBox::information(this,"Information", std::string("start thread size()").c_str(), QMessageBox::Ok);

		while (!stop_) {
			
			samples_read = ampGetData(mDevice, buffer, BufferSize, 0);
			if (samples_read <= 0)
				continue;

			//QMessageBox::information(this,"Information", std::string("samples_read = " + boost::lexical_cast<std::string>(samples_read)).c_str(), QMessageBox::Ok);

			ExtractLiveAmpData(buffer, samples_read, sampleSize, dataTypeArray, usedChannelsCounter, liveamp_buffer);
			PrepareDataToSendOverLSL(liveamp_buffer, lslsend_buffer, trigger_buffer);		
			
			//QMessageBox::information(this,"Information", std::string("liveamp_buffer size()" + boost::lexical_cast<std::string>(lslsend_buffer.size())).c_str(), QMessageBox::Ok);
			
			if(lslsend_buffer.size() >= chunkSize)
			{
				//QMessageBox::information(this,"Information", std::string("lslsend_buffer size()" + boost::lexical_cast<std::string>(lslsend_buffer.size())).c_str(), QMessageBox::Ok);
				double now = lsl::local_clock();
				int sendChannels = lslsend_buffer[0].size();
				std::vector<std::vector<double>> send_buffer;//(chunkSize, std::vector<double>(sendChannels));
				std::vector<double> send_trig(chunkSize);

				send_buffer.resize(chunkSize);
				//QMessageBox::information(this,"Information", std::string("sendChannels " + boost::lexical_cast<std::string>(sendChannels)).c_str(), QMessageBox::Ok);
				for (int s = 0; s < chunkSize; s++)
				{
					send_buffer[s].resize(sendChannels);

					for (int ch = 0; ch < sendChannels; ch++ )
						send_buffer[s][ch] = lslsend_buffer[s][ch];

					send_trig [s] = trigger_buffer[s];
					//QMessageBox::information(this,"Information", std::string("send_trig size()" + boost::lexical_cast<std::string>(send_trig.size())).c_str(), QMessageBox::Ok);				
				}

				//QMessageBox::information(this,"Information", std::string("send_buffer size()" + boost::lexical_cast<std::string>(send_buffer.size())).c_str(), QMessageBox::Ok);

				data_outlet.push_chunk(send_buffer, now);

				// push markers into outlet
				for (int s=0; s<samples_read; s++) {
					if (int mrk=send_trig[s]) {
						if (mrk != last_mrk) {
							std::string mrk_string = boost::lexical_cast<std::string>(mrk);
							marker_outlet.push_sample(&mrk_string,now + (s + 1 - samples_read)/samplingRate);
							last_mrk = mrk;
						}
					}
				}


				int difSamp = lslsend_buffer.size() - chunkSize;
				send_buffer.clear();
				send_buffer.resize(difSamp);
				send_trig.clear();
				send_trig.resize(difSamp);

				for (int s = 0; s < difSamp; s++)
				{
					send_buffer[s].resize(sendChannels);
					for (int ch = 0; ch < sendChannels; ch++ )
						send_buffer[s][ch] = lslsend_buffer[difSamp + s][ch];

					send_trig [s] = trigger_buffer[difSamp + s];
				}

				lslsend_buffer.clear();
				lslsend_buffer.resize(difSamp);
				trigger_buffer.clear();
				trigger_buffer.resize(difSamp);

				for (int s = 0; s < difSamp; s++)
				{
					lslsend_buffer[s].resize(sendChannels);
					for (int ch = 0; ch < sendChannels; ch++ )
						lslsend_buffer[s][ch] = send_buffer[s][ch];

					trigger_buffer [s] = send_trig[s];
				}
			}
		}		
	}
	catch(boost::thread_interrupted &) {
		// thread was interrupted: no error
	}
	catch(std::exception &e) {
		// any other error
		std::cerr << e.what() << std::endl;
	}

	if(buffer != NULL)
		delete[] buffer;
	buffer = NULL;

	if (mDevice) {
		if (started)
			ampStopAcquisition(mDevice);	
		ampCloseDevice(mDevice);
		mDevice = NULL;
	}
}



MainWindow::~MainWindow() {
	delete ui;
}


bool MainWindow::InitializeLiveAmp()
{	
	HANDLE hlocDevice = NULL;
	int res;
	char HWI[20];
	strcpy_s(HWI, "ANY");  // use word SIM to simulate the LiveAmp

	mDevice = NULL;
	std::string serialN = ui->deviceSerialNumber->text().toStdString();

	QMessageBox *ms = new QMessageBox(this);
	ms->setWindowTitle("connecting to LiveAmp amplifier...it takes some seconds...");
	ms->setText("Connecting to LiveAmp amplifier(s)...");
	ms->show();

	res = ampEnumerateDevices(HWI, sizeof(HWI), "LiveAmp", 0);
			
	ms->close();
	delete ms;
			
	if (res <= 0)			
		throw std::runtime_error("No LiveAmp connected");
	else
	{				
		int numDevices = res;
		for (int i = 0; i < numDevices; i++)
		{
			hlocDevice = NULL;
			res = ampOpenDevice(i, &hlocDevice);
			if(res != AMP_OK)
			{
				std::string msg = "Cannot open device: ";
				msg.append (boost::lexical_cast<std::string>(i).c_str());
				msg.append("  error= " );
				msg.append(boost::lexical_cast<std::string>(res).c_str());
				throw std::runtime_error(msg);
			}
				
			char sVar[20]; 
			res = ampGetProperty(hlocDevice, PG_DEVICE, i, DPROP_CHR_SerialNumber, sVar, sizeof(sVar));
			if(res != AMP_OK)		
			{
				std::string msg = "Cannot read Serial number, ";
				msg.append (boost::lexical_cast<std::string>(i).c_str());
				msg.append("  error= " );
				msg.append(boost::lexical_cast<std::string>(res).c_str());
				throw std::runtime_error(msg);
			}

			//QMessageBox::information(this,"Information", sVar, QMessageBox::Ok);

			int check = strcmp(sVar, serialN.c_str());
			if(check == 0)
			{
				QMessageBox::information(this,"Information", std::string("LiveAmp SN: " + serialN + " successfully connected!").c_str(), QMessageBox::Ok);

				mDevice = hlocDevice; // save device handler
				return true;
			}
			else
			{		
				res = ampCloseDevice(hlocDevice);
				if(res != AMP_OK)
				{						
					std::string msg = "Cannot close device: ";
					msg.append (boost::lexical_cast<std::string>(i).c_str());
					msg.append("  error= " );
					msg.append(boost::lexical_cast<std::string>(res).c_str());
					throw std::runtime_error(msg);
				}
			}
		}
	}

	if(mDevice == NULL)
	{
		std::string msg = "There is no LiveAmp with serial number: " + serialN + " detected!";		
		throw std::runtime_error(msg);
	}

	return false;
}

// configure LiveAmp amplifier, sampling rate, mode, channels etc...
bool MainWindow::ConfigureLiveAmp(void)
{	
	try
	{
		// amplifier configuration
		float fVar = (mSamplingRate);
		int res = ampSetProperty(mDevice, PG_DEVICE, 0, DPROP_F32_BaseSampleRate, &fVar, sizeof(fVar));
		if(res != AMP_OK)					
			throw std::runtime_error(("Error setting sampling rate, error code:  " + boost::lexical_cast<std::string>(res)).c_str());

		int mode = RM_NORMAL;
		res = ampSetProperty(mDevice, PG_DEVICE, 0, DPROP_I32_RecordingMode, &mode, sizeof(mode));
		if(res != AMP_OK)					
			throw std::runtime_error(("Error setting acquisition mode, error code:  " + boost::lexical_cast<std::string>(res)).c_str());	

	}
	catch(std::exception &e) {
		// generate error message
		std::string msg = "ConfigureLiveAmp  error. ";

		QMessageBox::critical(this,"Error", (std::string("ConfigureLiveAmp error:  ") + e.what()).c_str() ,QMessageBox::Ok);
		return false;
	}

	return true;
}

// enables physical channels on LiveAmp 
bool MainWindow::EnableLiveAmpChannels(bool enableAUX, bool enableACC)
{
	int avlbchannels;	
	bool useBipolar = ui->useBipolar->checkState()==Qt::Checked;

	try
	{
		int res = ampGetProperty(mDevice, PG_DEVICE, 0, DPROP_I32_AvailableChannels, &avlbchannels, sizeof(avlbchannels));
		//QMessageBox::information(this, "Information", std::string("avlbchannels= " +   boost::lexical_cast<std::string>(avlbchannels)).c_str(), QMessageBox::Ok);

		for (int c = 0; c < avlbchannels; c++)
		{
			int type;

			res = ampGetProperty(mDevice, PG_CHANNEL, c, CPROP_I32_Type, &type, sizeof(type));
			if(res != AMP_OK)					
				throw std::runtime_error(("Error getting property for channel type: error code:  " + boost::lexical_cast<std::string>(res)).c_str());
		
			//QMessageBox::information(this, "Information", std::string("channel #' " +   boost::lexical_cast<std::string>(c)).c_str(), QMessageBox::Ok);
			if (type == CT_AUX)
			{		
				char cValue[20];
				int enabled;

				res = ampGetProperty(mDevice, PG_CHANNEL, c, CPROP_CHR_Function, &cValue, sizeof(cValue));
				if (res != AMP_OK)			
					throw std::runtime_error(("Error GetProperty CPROP_CHR_Function error: "  + boost::lexical_cast<std::string>(res)).c_str());
			
				res = ampGetProperty(mDevice, PG_CHANNEL, c, CPROP_B32_RecordingEnabled, &enabled, sizeof(enabled));
				if (res != AMP_OK)	
				{
					//QMessageBox::information(this, "Information",   ("Error GetProperty enable for ACC channels, error: " + boost::lexical_cast<std::string>(res)).c_str()  , QMessageBox::Ok);
					throw std::runtime_error(("Error SetProperty enable for ACC channels, error: "  + boost::lexical_cast<std::string>(res)).c_str());
				}
			
				if(cValue == NULL || cValue[0] == NULL || !(cValue[0] == 'X' || cValue[0] == 'Y' ||cValue[0] == 'Z' || cValue[0] == 'x' ||cValue[0] == 'y' ||cValue[0] == 'z'))
				{
					if(enabled != enableAUX)
					{
						enabled = enableAUX;

						// Enables/disables AUX channels
						res = ampSetProperty(mDevice, PG_CHANNEL, c, CPROP_B32_RecordingEnabled, &enabled, sizeof(enabled));
						if (res != AMP_OK)	
							throw std::runtime_error(("Error SetProperty enable for ACC channels, error: "  + boost::lexical_cast<std::string>(res)).c_str());
					}
				}
				else	
				{				
					if(enabled != enableACC)
					{
						enabled = enableACC;

						// Enables/disables ACC channels
						res = ampSetProperty(mDevice, PG_CHANNEL, c, CPROP_B32_RecordingEnabled, &enabled, sizeof(enabled));
						if (res != AMP_OK)			
							throw std::runtime_error(("Error SetProperty enable for ACC channels, error: "  + boost::lexical_cast<std::string>(res)).c_str());
					}
				}
			
			}


			else if (type == CT_EEG || type == CT_BIP)
			{
				int enableEEG = true;
				// match indexes of input channel list (from UI List control)
				if(c >= mChannelCount)
				{
					enableEEG = false;
					res = ampSetProperty(mDevice, PG_CHANNEL, c, CPROP_B32_RecordingEnabled, &enableEEG, sizeof(enableEEG));
					if (res != AMP_OK)								
						throw std::runtime_error(("Error SetProperty disable for EEG channels, error: "  + boost::lexical_cast<std::string>(res)).c_str());
				}
				else
				{
					res = ampSetProperty(mDevice, PG_CHANNEL, c, CPROP_B32_RecordingEnabled, &enableEEG, sizeof(enableEEG));
					if (res != AMP_OK)								
						throw std::runtime_error(("Error SetProperty enable for EEG channels, error: "  + boost::lexical_cast<std::string>(res)).c_str());	


					if(useBipolar &&  c > 23 && c < 32 ) // set last 8 channels to bipolar
					{
						int bipType = CT_BIP;
						res = ampSetProperty(mDevice, PG_CHANNEL, c, CPROP_I32_Type, &bipType, sizeof(bipType));
						if (res != AMP_OK)								
							throw std::runtime_error(("Error SetProperty enable BIPOLAR EEG channels, error: "  + boost::lexical_cast<std::string>(res)).c_str());	

					}
					
				}
			}

			// Trigger channel cannot be enabled disabled
			else if (type == CT_TRG || type == CT_DIG)		
				continue;					
		}
	}
	catch(std::exception &e) {
		QMessageBox::critical(this,"Error", (std::string("ConfigureLiveAmp error. ") + e.what()).c_str() ,QMessageBox::Ok);
		return false;
	}

	return true;
}

bool MainWindow::GenerateUsedPhysicalChannelIndexes(void)
{
	int enableCh;
	int type;
	int allEnabledChannel;  // only used==enabled channel

	eegIndexes.clear();
	auxIndexes.clear();
	accIndexes.clear();
	mTrigInIndexes.clear();
	mChannelIndexes.clear();

	// after enabling and disabling physical channels by LibveAmp, the order (indexing) changes... it must be checked again the channel types and indexing.
	int avlbchannels;

	try
	{
		int res = ampGetProperty(mDevice, PG_DEVICE, 0, DPROP_I32_AvailableChannels, &avlbchannels, sizeof(avlbchannels));
		for (int c = 0; c < avlbchannels; c++)
		{		
			res = ampGetProperty(mDevice, PG_CHANNEL, c, CPROP_B32_RecordingEnabled, &enableCh, sizeof(enableCh));
			if(res != AMP_OK)					
				throw std::runtime_error(("Error getting property for channel CPROP_B32_RecordingEnabled: error code:  " + boost::lexical_cast<std::string>(res)).c_str());

			if(!enableCh)
				continue;

			res = ampGetProperty(mDevice, PG_CHANNEL, c, CPROP_I32_Type, &type, sizeof(type));
			if(res != AMP_OK)					
				throw std::runtime_error(("Error getting property for channel type: error code:  " + boost::lexical_cast<std::string>(res)).c_str());
		
			if (type == CT_AUX)
			{		
				char cValue[20];	
				res = ampGetProperty(mDevice, PG_CHANNEL, c, CPROP_CHR_Function, &cValue, sizeof(cValue));
				if (res != AMP_OK)			
					throw std::runtime_error(("Error GetProperty CPROP_CHR_Function error: "  + boost::lexical_cast<std::string>(res)).c_str());
				
				if(cValue[0] == 'X' || cValue[0] == 'Y' ||cValue[0] == 'Z' || cValue[0] == 'x' ||cValue[0] == 'y' ||cValue[0] == 'z')	
					accIndexes.push_back(c);				
			
				else
					auxIndexes.push_back(c);
			}
		
			else if (type == CT_EEG || type == CT_BIP)
				eegIndexes.push_back(c);
		
				
			else if (type == CT_TRG || type == CT_DIG)
			{
				char cValue[20];	
				res = ampGetProperty(mDevice, PG_CHANNEL, c, CPROP_CHR_Function, &cValue, sizeof(cValue));
				if (res != AMP_OK)								
						throw std::runtime_error(("Error GetProperty CPROP_CHR_Function for CT_TRG, error: "  + boost::lexical_cast<std::string>(res)).c_str());		
								
				// we are interested only on Trigger Inputs... 
				if(strcmp("Trigger Input", cValue) == 0 )
					mTrigInIndexes.push_back(c); 	
			}

			allEnabledChannel++;

			if (type == CT_EEG || type == CT_BIP || type == CT_AUX)
			{
				float gain = 2;
				res = ampSetProperty(mDevice, PG_CHANNEL, c, CPROP_F32_Gain, &gain, sizeof(gain));
				if (res != AMP_OK)
					throw std::runtime_error(("Error SetProperty CPROP_F32_Gain, error: "  + boost::lexical_cast<std::string>(res)).c_str());		
			}
		}		
	}		

	catch(std::exception &e) {
		QMessageBox::critical(this,"Error", (std::string("GenerateUsedPhysicalChannelIndexes error.  ") + e.what()).c_str() ,QMessageBox::Ok);
		return false;
	}

	//QMessageBox::information(this, "Information", std::string("eegIndexes # " +   boost::lexical_cast<std::string>(eegIndexes.size())).c_str(), QMessageBox::Ok);

	// copy indexes of eeg, aux and acc physical channels 
	for (int i=0; i < eegIndexes.size(); i++)
		mChannelIndexes.push_back(eegIndexes[i]);

	//QMessageBox::information(this, "Information", std::string("auxIndexes # " +   boost::lexical_cast<std::string>(auxIndexes.size())).c_str(), QMessageBox::Ok);
	for (int i=0; i < auxIndexes.size(); i++)
		mChannelIndexes.push_back(auxIndexes[i]);

	//QMessageBox::information(this, "Information", std::string("accIndexes # " +   boost::lexical_cast<std::string>(accIndexes.size())).c_str(), QMessageBox::Ok);
	for (int i=0; i < accIndexes.size(); i++)
		mChannelIndexes.push_back(accIndexes[i]);

	return true;

}


// adjust channel labels, in case that there are more channels send to LSL (like aux or acc)
void MainWindow::AdjustChannelLabels(std::vector<std::string>& inpuChannelLabels, std::vector<std::string> &adjustedChannelLabels)
{
	// there is order of channels by LiveAmp : EEG + AUX + ACC
	if(inpuChannelLabels.size() != eegIndexes.size())
		QMessageBox::critical(this,"Error", "AdjustChannelLabels error, number of elements in input channel labels doesn't match!" ,QMessageBox::Ok);

	adjustedChannelLabels.clear();
	adjustedChannelLabels.resize(inpuChannelLabels.size());
	
	for (int i = 0; i < inpuChannelLabels.size(); i++)
		adjustedChannelLabels[i] = inpuChannelLabels[i];


	for (int i = 0; i < auxIndexes.size(); i++)
	{
		std::string aux = "AUX";
		aux += boost::lexical_cast<std::string>(i);
		adjustedChannelLabels.push_back (aux);
	}

	for (int i = 0; i < accIndexes.size(); i++)
	{
		std::string acc;
		std::string ix = boost::lexical_cast<std::string>(i/3 + 1);
		
		if (i%3 == 0)
			acc = "X" + ix;
		else if (i%3 == 1)
			acc = "Y" + ix;
		else if (i%3 == 2)
			acc = "Z" + ix;

		//QMessageBox::information(this, "Information", (std::string("ACC= ") +   acc).c_str(), QMessageBox::Ok);

		adjustedChannelLabels.push_back (acc);
	}

}



// extracts recorded samples form LiveAMp structure and copies to LSL buffer structure, ready to send over LSL
void MainWindow::PrepareDataToSendOverLSL(std::vector<std::vector<double>> &LiveAmptData, std::vector<std::vector<double>> &LSLData, std::vector<uint16_t> &trigger_buffer)
{
	int numSamples = LiveAmptData.size();

	if(numSamples <= 0)
		return;
	
	int lslChannleCount = mChannelIndexes.size();
	

	for (int s = 0; s < numSamples; s++)
	{
		std::vector<double> locBuffer (lslChannleCount); 
		for (int i=0; i < lslChannleCount; i++)	
			locBuffer[i] = LiveAmptData[s][mChannelIndexes[i]];  
				
		LSLData.push_back(locBuffer);
		trigger_buffer.push_back(LiveAmptData[s][mTrigInIndexes[0]]);  // for now we use only the first Trigger In channel!
	}
}

void MainWindow::ExtractLiveAmpData(BYTE* buffer, int seamples_read, int samplesize, int *dataTypes, int usedChannelsCnt, std::vector<std::vector<double>> &extractData)
{
	uint64_t sc;
	int numSamples = seamples_read / samplesize;
	int offset = 0;
	double sample = 0;
		
	extractData.clear();
	extractData.resize(numSamples);

	for (int s = 0; s < numSamples; s++)
	{
		offset = 0;
		sc = *(uint64_t*)&buffer[s*samplesize + offset];				
		offset += 8; // sample counter offset 

		extractData[s].resize(usedChannelsCnt);

		for (int i=0; i < usedChannelsCnt; i++)
		{
			switch (dataTypes[i])
			{
				case DT_INT16:
					{
						int16_t tmp = *(int16_t*)&buffer[s*samplesize + offset];
						sample = (double) tmp;
						offset += 2;
						break;
					}
				case DT_UINT16:
					{
						uint16_t tmp = *(uint16_t*)&buffer[s*samplesize + offset];
						sample = (double) tmp;
						offset += 2;
						break;
					}					
				case DT_INT32:
					{
						int32_t tmp = *(int32_t*)&buffer[s*samplesize + offset];
						sample = (double) tmp;
						offset += 4;
						break;
					}
				case DT_UINT32:
					{
						uint32_t tmp = *(uint32_t*)&buffer[s*samplesize + offset];
						sample = (double) tmp;
						offset += 4;
						break;
					}
				case DT_FLOAT32:
					{
						float tmp = *(float*)&buffer[s*samplesize + offset];
						sample = (double) tmp;
						offset += 4;
						break;
					}
				case DT_INT64:
					{
						int64_t tmp = *(int64_t*)&buffer[s*samplesize + offset];
						sample = (double) tmp;
						offset += 8;
						break;
					}
				case DT_UINT64:
					{
						uint64_t tmp = *(uint64_t*)&buffer[s*samplesize + offset];
						sample = (double) tmp;
						offset += 8;
						break;
					}
				case DT_FLOAT64:
					{
						
						double tmp = *(double*)&buffer[s*samplesize + offset];
						sample = (double) tmp;
						offset += 8;
						break;					
					}					
				default:
					break;
			}

			extractData[s][i] = sample;
		}
	}
}

// Get the current sample size in byte
int LiveAmp_SampleSize(HANDLE hDevice, int *typeArray, int* usedChannelsCnt)
{
	int res;
	int channels, datatype;
	BOOL enabled;
	int bytesize = 0;
	int cntUsedCh = 0;

	// iterate through all enabled channels
	res = ampGetProperty(hDevice, PG_DEVICE, 0, DPROP_I32_AvailableChannels, &channels, sizeof(channels));
	for (int c = 0; c < channels; c++)
	{
		res = ampGetProperty(hDevice, PG_CHANNEL, c, CPROP_B32_RecordingEnabled, &enabled, sizeof(enabled));
		if (enabled)
		{
			res = ampGetProperty(hDevice, PG_CHANNEL, c, CPROP_I32_DataType, &datatype, sizeof(datatype));
			
			typeArray[cntUsedCh] = datatype;
			cntUsedCh++;

			switch (datatype)
			{
			case DT_INT16:
			case DT_UINT16:
				{
					bytesize += 2;
				}
				break;
			case DT_INT32:
			case DT_UINT32:
			case DT_FLOAT32:
				{
					bytesize += 4;				
					
				}
				break;
			case DT_INT64:
			case DT_UINT64:
			case DT_FLOAT64:
				{
					bytesize += 8;
				}
				break;
			default:
				break;
			}
		}
	}
	// add the sample counter size
	bytesize += 8;
	*usedChannelsCnt = cntUsedCh;
	return bytesize;
}
