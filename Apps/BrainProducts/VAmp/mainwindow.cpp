/* 
/  Thanks to Ratko Petrovich at Brain Products for /
/  discovering and correcting the channel swapping bug /
/  relating to the resampling routine. /
*/


#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/foreach.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <iostream>

#include "FirstAmp.h"
#include "Device.h"
#include "GenericData.h"

const double sampling_rates[] = {100, 250, 500, 1000, 2000, 5000, 10000, 20000};


void Transpose(const vector<vector<double> > &in, vector<vector<double> > &out);

MainWindow::MainWindow(QWidget *parent, const std::string &config_file): QMainWindow(parent),ui(new Ui::MainWindow)
{
	ui->setupUi(this);

	// parse startup config file
	load_config(config_file);

	// make GUI connections
	QObject::connect(ui->actionQuit, SIGNAL(triggered()), this, SLOT(close()));
	QObject::connect(ui->linkButton, SIGNAL(clicked()), this, SLOT(link()));
	QObject::connect(ui->channelCount, SIGNAL(valueChanged(int)),this, SLOT(update_channel_labels(int)));
	QObject::connect(ui->samplingRate, SIGNAL(currentIndexChanged(int)),this, SLOT(update_channels_for_sr(int)));
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

void MainWindow::update_channel_counter() {
	int i=0;
	std::vector<std::string> channelLabels;
	boost::algorithm::split(channelLabels,ui->channelLabels->toPlainText().toStdString(),boost::algorithm::is_any_of("\n"));
	BOOST_FOREACH(std::string &v, channelLabels)++i;
	//std::cout << i << std::endl;
	ui->channelCount->setValue(i);
	//ui->channelCount->setMaximum(max_channels);
	int channelCount = ui->channelCount->value();	
}



void MainWindow::update_channel_labels(int n) {
	
	std::string str;	
	ui->channelLabels->clear();	
	long long i;
	for(i=1;i<=n;i++) {
		str = std::to_string(i);
		ui->channelLabels->appendPlainText(str.c_str());
		//ui->channelLabels->appendPlainText("\n");
	}
}

void MainWindow::update_channels_for_sr(int n) {
	
	const double sr = sampling_rates[n];
	if(sr>2000 && ui->channelCount->value()>4){
		ui->channelCount->setValue(4);
		update_channel_labels(4);
		ui->useAUX->setEnabled(false);
		ui->useAUX->setChecked(false);
	}
	if(sr<=2000)ui->useAUX->setEnabled(true);
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
		ui->deviceNumber->setValue(pt.get<int>("settings.devicenumber",0));
		ui->channelCount->setValue(pt.get<int>("settings.channelcount",16));
		ui->samplingRate->setCurrentIndex(pt.get<int>("settings.samplingrate",2));
		ui->useAUX->setCheckState(pt.get<bool>("settings.useaux",false) ? Qt::Checked : Qt::Unchecked);
		ui->unsampledMarkers->setCheckState(pt.get<bool>("settings.unsampledmarkers",false) ? Qt::Checked : Qt::Unchecked);	
		ui->sampledMarkers->setCheckState(pt.get<bool>("settings.sampledmarkers",true) ? Qt::Checked : Qt::Unchecked);	
		ui->sampledMarkersEEG->setCheckState(pt.get<bool>("settings.sampledmarkersEEG",false) ? Qt::Checked : Qt::Unchecked);

		//std::cout<<ui->channelLabels->toPlainText().toStdString()<<std::endl;
		ui->channelLabels->clear();
		int i=0;
		BOOST_FOREACH(ptree::value_type &v, pt.get_child("channels.labels")) {
			++i;
			ui->channelLabels->appendPlainText(v.second.data().c_str());
		}
		
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
		pt.put("settings.samplingrate",ui->samplingRate->currentIndex());
		pt.put("settings.useaux",ui->useAUX->checkState()==Qt::Checked);
		pt.put("settings.unsampledmarkers",ui->unsampledMarkers->checkState()==Qt::Checked);
		pt.put("settings.sampledmarkers",ui->sampledMarkers->checkState()==Qt::Checked);
		pt.put("settings.sampledmarkersEEG",ui->sampledMarkersEEG->checkState()==Qt::Checked);


		std::vector<std::string> channelLabels;
		boost::algorithm::split(channelLabels,ui->channelLabels->toPlainText().toStdString(),boost::algorithm::is_any_of("\n"));
		BOOST_FOREACH(std::string &v, channelLabels)
			pt.add("channels.labels.label", v);
		update_channel_counter();
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


// start/stop the ActiChamp connection
void MainWindow::link() {

	CDevice *l_pDevice = NULL;		// Instance of device

	if (reader_thread_) {
		//// === perform unlink action ===
		try {
			stop_ = true;
			reader_thread_->join();
			reader_thread_.reset();
			int res = SetPriorityClass(GetCurrentProcess(), NORMAL_PRIORITY_CLASS);
		} catch(std::exception &e) {
			QMessageBox::critical(this,"Error",(std::string("Could not stop the background processing: ")+=e.what()).c_str(),QMessageBox::Ok);
			return;
		}

		// indicate that we are now successfully unlinked
		ui->linkButton->setText("Link");
	} else {
	HANDLE hDevice = NULL;
		try {
			// get the UI parameters...
			int deviceNumber = ui->deviceNumber->value();
			int samplingRate = sampling_rates[ui->samplingRate->currentIndex()];
			//std::cout << samplingRate << std::endl;
			bool useAUX = ui->useAUX->checkState()==Qt::Checked;			
			
			bool unsampledMarkers  = ui->unsampledMarkers->checkState()==Qt::Checked;
			bool sampledMarkers    = ui->sampledMarkers->checkState()==Qt::Checked;
			bool sampledMarkersEEG = ui->sampledMarkersEEG->checkState()==Qt::Checked;
			
			update_channel_counter();
			
			int i=0;
			std::vector<std::string> channelLabels;
			boost::algorithm::split(channelLabels,ui->channelLabels->toPlainText().toStdString(),boost::algorithm::is_any_of("\n"));
			BOOST_FOREACH(std::string &v, channelLabels)++i;
			//std::cout << i << std::endl;
			int channelCount = ui->channelCount->value();	

			// try to connect with VAmp device
			l_pDevice = CDevice::GetInstance();
			if (l_pDevice == NULL) 
				throw std::runtime_error("Could not get instance of VAmp");
			
			// (1) Open VAmp device
			if (S_OK != l_pDevice->Open()) 
				throw std::runtime_error("Cannot open VAmp !!!");

			// (2) Setup configuration
			int nMaxChannels = l_pDevice->GetChannels();
			int nAnalogChannels = l_pDevice->GetAnalogChannels();

			int nDataMode = dmNormal; /* normal - all channels at 2 kHz */
			if(samplingRate > 2000)
			{
				nDataMode = dm20kHz4Channels; /* fast - 20 kHz, 4 Channels, no auxiliary */
			}

			// check settings:
			if(nDataMode == dmNormal)
			{
				if (channelCount > 16)
					throw std::runtime_error("Channel count bigger than 16");
			}
			else
			{
				// by 20KHz there are only 4 channels
				if (channelCount > 4)
					throw std::runtime_error("Channel count bigger than 4");
 
				if(useAUX)
					throw std::runtime_error("Cannot use AUX channels in high-speed mode");
			}

			// check if there are no channel labels, (since stringLabels shows one element, although the string label list is empty)
			if (channelLabels.size() != channelCount)
			{
				if(channelLabels.size() == 1 && (channelLabels[0]).empty())
				{
					
				}
				else
					throw std::runtime_error("The number of channels labels does not match the channel count device setting.");
			}
		
			//int channelsToSendLSL = channelCount;
			if(useAUX && nDataMode == dmNormal)
				channelCount += 2; // add two AUX channels

			if(sampledMarkersEEG)
				channelCount += 1;	

			if(channelCount == 0)
				QMessageBox::information(this,"Information", std::string("There is no analog channel selected ! \n Only triggers will be recorded. ").c_str(), QMessageBox::Ok);

			/*
			// check that the needed modules are present
			if (useAUX && !modules.Present&(1<<0))
				throw std::runtime_error("AUX channels were requested but the device reports that no AUX module is present.");
			int neededEEGchannels = channelCount - (useAUX?properties.CountAux:0);
			*/


			// (5) Close device 
			l_pDevice->Stop();
			l_pDevice->Close();

			// start reader thread
			stop_ = false;
			reader_thread_.reset(new boost::thread(&MainWindow::read_thread,this,deviceNumber,channelCount,samplingRate,useAUX,unsampledMarkers,sampledMarkers,sampledMarkersEEG,channelLabels));
		}

		catch(std::exception &e) {
			// generate error message
			std::string msg = "Could not retrieve driver error message";
			int errorcode=0; char buffer[16384];
			
			if (hDevice) {
				//champClose(hDevice);
				hDevice = NULL;
			}
			QMessageBox::critical(this,"Error",("Could not initialize the VAmp interface: "+(e.what()+(" (driver message: "+msg+")"))).c_str(),QMessageBox::Ok);
			return;
		}

		// done, all successful
		ui->linkButton->setText("Unlink");
	}
}

bool _resample = false;

// background data reader thread
void MainWindow::read_thread(int deviceNumber, int channelCount, int samplingRate, bool useAUX, bool unsampledMarkers, bool sampledMarkers, bool sampledMarkersEEG, std::vector<std::string> channelLabels) {
	HANDLE hDevice = NULL;
	CDevice *l_pDevice = NULL;		// Instance of device
	const int chunkSize = 20;       // Only available chunck size for VAmp
	
	
	int res = SetPriorityClass(GetCurrentProcess(), HIGH_PRIORITY_CLASS);
	std::string threadId = boost::lexical_cast<std::string>(boost::this_thread::get_id());
    unsigned long threadNumber = 0;
    sscanf(threadId.c_str(), "%lx", &threadNumber);
	std::cout << res << ":" <<  threadNumber << std::endl;


	bool started = false;
	try {
		// try to open the device again (we're doing everything in the same thread to not confuse the driver)
		// try to open VAmp device
		l_pDevice = CDevice::GetInstance();
		if (l_pDevice == NULL) 
			throw std::runtime_error("Could not get instance of VAmp");
	
		// (1) Open device
		if (S_OK != l_pDevice->Open()) 
			throw std::runtime_error("Cannot open VAmp !!!");

		// (2) Setup configuration
		int nMaxChannels = l_pDevice->GetChannels();  // eeg + aux + digital port channels
		int nAnalogChannels = l_pDevice->GetAnalogChannels(); // eeg + aux channels
	
		int nDataMode = dmNormal; /* normal - all channels at 2 kHz */
		if(samplingRate > 2000)
			nDataMode = dm20kHz4Channels; /* fast - 20 kHz, 4 Channels, no auxiliary */
		l_pDevice->SetDataMode(nDataMode);

		l_pDevice->SetSamplingRate(samplingRate);		

		LPSAFEARRAY pData;
		CComSafeArray<double, VT_R8> pResolutions;	// table of channel resolution.
		CComSafeArray<char, VT_I1> pChTypes;		// table of channel type.

		l_pDevice->GetChannelTypes(&pData);  	
		pChTypes.Attach(pData);
		l_pDevice->GetChannelResolutions(&pData); 	
		pResolutions.Attach(pData);

		//int nAnalogChannels = 0
		int nRefChanIdx = 0;
		l_pDevice->GetRefChannel(&nRefChanIdx);
		
		int tblChannelsPos[4], tblChannelsNeg[4];
		for (UINT n = 0; n < 4; n++)
		{
			// differential pair 2 (channel n - channel REF)
			tblChannelsPos[n] = n;
			tblChannelsNeg[n] = nRefChanIdx; // channel REF
		}

		// try to start acquisition
		// (3) start data acquisition
		if (S_OK != l_pDevice->Start(IAC_AT_DATA, nDataMode, tblChannelsPos, tblChannelsNeg))	
			throw std::runtime_error("Could not start the acquisition");
	
		started = true;
		
		std::vector<std::vector<double>> send_buffer(chunkSize,std::vector<double>(channelCount));

		std::vector<std::vector<std::string>> marker_buffer(chunkSize, std::vector<std::string>(1));
		std::vector<std::string> s_mrkr;
		std::vector<USHORT> trig_buffer(10);
		
		_resample = false;
		if(samplingRate != 2000 && samplingRate != 20000)
			_resample = true;

		//std::cout << "channelCount: " << channelCount << std::endl;

		// create data streaminfo and append some meta-data
		lsl::stream_info data_info("VAmp-" + boost::lexical_cast<std::string>(deviceNumber) + "-EEG","EEG",channelCount, samplingRate, lsl::cf_float32,"VAmp_" + boost::lexical_cast<std::string>(deviceNumber) + "_EEG");
		lsl::xml_element channels = data_info.desc().append_child("channels");
		int count = channelCount - (sampledMarkersEEG ? 0 : 1);
		//if(!regularizeMarkers)count--;
		for (std::size_t k=0; k < count;k++)
		{
			if(k < channelLabels.size())
			{
				channels.append_child("channel")
					.append_child_value("label",channelLabels[k].c_str())
					.append_child_value("type","EEG")
					.append_child_value("unit","microvolts");
			}
			else
			{	
				channels.append_child("channel")
					.append_child_value("label", "aux_" + boost::lexical_cast<std::string>(k - channelLabels.size() + 1))	
					.append_child_value("type","EEG")
					.append_child_value("unit","microvolts");
			}
		}
		if(sampledMarkersEEG) {
				channels.append_child("channel")
					.append_child_value("label", "triggerStream")	
					.append_child_value("type","EEG")
					.append_child_value("unit","code");
		}

		data_info.desc().append_child("acquisition").append_child_value("manufacturer","Brain Products");
		// make a data outlet
		lsl::stream_outlet data_outlet(data_info);
		
		// trick C++ compiler into having a conditional object
		lsl::stream_outlet *marker_outlet;
		// create marker streaminfo and outlet
		if(unsampledMarkers) {
			lsl::stream_info marker_info("VAmp-" + boost::lexical_cast<std::string>(deviceNumber) + "-Markers","Markers", 1, 0, lsl::cf_string,"VAmp_" + boost::lexical_cast<std::string>(deviceNumber) + "_markers");
			marker_outlet = new lsl::stream_outlet(marker_info);
		}	

		lsl::stream_outlet *s_marker_outlet;
		if(sampledMarkers) {
			lsl::stream_info s_marker_info("VAmp-" + boost::lexical_cast<std::string>(deviceNumber) + "-Sampled-Markers","sampledMarkers", 1, samplingRate, lsl::cf_string,"VAmp_" + boost::lexical_cast<std::string>(deviceNumber) + "_sampled_markers");
			s_marker_outlet = new lsl::stream_outlet(s_marker_info);
				// ditch the outlet if we don't need it (need to do it this way in order to trick C++ compiler into using this object conditionally)
		}

		int samples_read = 0;
		USHORT prev_marker = 0;
		USHORT prev_markerSampled = 0;
		USHORT prev_markerEEG = 0;
		float f_mrkr;
		
		
		UINT nBlockCnt = 0;
		UINT nBlockSize = 19; // 16 eeg + 2 aux + 1 digital port
		if(nDataMode != dmNormal){
			if(useAUX) {
				throw std::runtime_error("Cannot use aux channels in high speed mode");
				return;
			}
			nBlockSize = 5;  // 4 eeg + 1 1 digital port	
		}
		
		int eegChannelToSend = channelCount;
		if(useAUX)
			eegChannelToSend -= 2;

		// enter transmission loop		
		while (!stop_) {
			// (4) Serve data
			CComSafeArray<float, VT_R4> saData;		// SafeArray for Monitoring, TestSignal.
			DATE date = 0;
			LPSAFEARRAY sa;
			int nErrorSeverity = 0;
			HRESULT hResult = S_FALSE;
			hResult = l_pDevice->GetData(&sa, &nMaxChannels, &samples_read, &date, &nErrorSeverity);

			if(samples_read<=0 && samplingRate <= 2000){
				boost::this_thread::sleep(boost::posix_time::milliseconds(1));
				continue;
			}
			
			if (hResult == S_OK)
			{
				saData.Attach(sa);
				float *pBuffer = (float *)saData.m_psa->pvData;
			
				int auxCount=0;
				double now = lsl::local_clock();
				if (samples_read > 0) {
				
					int nBlock = 0;
					send_buffer.resize(samples_read);
					marker_buffer.resize(samples_read);
					trig_buffer.resize(samples_read);
				

					for (int s=0;s<samples_read;s++) {					
						send_buffer[s].resize(channelCount);
						// send first EEG channels
						for (int c=0; c<eegChannelToSend; c++)						
							 send_buffer[s][c] =  *(pBuffer + nBlock + c) * (float)pResolutions.GetAt(c);

						// send 2xAUX channels, if any
						if(useAUX)
						{
							auxCount = 2;
							for (int c=0; c < 2; c++)						
								send_buffer[s][eegChannelToSend +c] =  *(pBuffer + nBlock + (nAnalogChannels - 2) + c) * (float)pResolutions.GetAt(nAnalogChannels - 2 + c);
						}

						USHORT markers = *(pBuffer + nBlock + (nBlockSize - 1)); // get 16bit variable that contains 1 channel trig + 8 digital bits
						trig_buffer[s] = markers & 0xFFFF;
						
						if (sampledMarkersEEG) {
							f_mrkr = (trig_buffer[s] == prev_markerEEG ? 0.0 : boost::lexical_cast<float>(trig_buffer[s]));
							prev_markerEEG = trig_buffer[s];
							send_buffer[s][eegChannelToSend + auxCount - 1] = f_mrkr;
						}
						
						if (sampledMarkers) {
							s_mrkr.clear();
							s_mrkr.push_back(trig_buffer[s] == prev_markerSampled ? "" : boost::lexical_cast<std::string>(trig_buffer[s]));
							marker_buffer.at(s) = s_mrkr;
							//std::cout << "s: " << s << std::endl;
							prev_markerSampled = trig_buffer[s];
						}

						nBlock += nBlockSize;  	
					}
				
					// send EEG to LSL	
					data_outlet.push_chunk(send_buffer, now);
					
					// send the sample markers (if we want to)
					if(sampledMarkers)
						s_marker_outlet->push_chunk(marker_buffer, now);		
		
					// push markers into outlet
					if(unsampledMarkers) {
						for (int s=0;s<samples_read;s++) {
							if (USHORT mrk=trig_buffer[s]) {
								if (mrk != prev_marker) 
								{
									std::string mrk_string = boost::lexical_cast<std::string>(mrk);
									marker_outlet->push_sample(&mrk_string, now + (s + 1 - samples_read)/samplingRate);
									prev_marker = mrk;
								}	
							}
						}
					}
				}
			} // close if good sample condition
		} // close while loop
		// need to explicitly delete these objects
		if(unsampledMarkers)delete(marker_outlet);
		if(sampledMarkers)delete(s_marker_outlet);
	}
	catch(boost::thread_interrupted &) {
		// thread was interrupted: no error
	}
	catch(std::exception &e) {
		// any other error
		std::cerr << e.what() << std::endl;
	}

	if (l_pDevice) {
		if (started)
			l_pDevice->Stop();
		l_pDevice->Close();
		l_pDevice = NULL;
	}


}

MainWindow::~MainWindow() {
	delete ui;
}

