/*********************************************************

C++ source code controlling the GUI and behavior of
the LSL LiveAmp Connector. Definition of the class declared
in mainwindow.h.

Copyright (c) 2018 Brain Products

Released under the MIT license (see LICENSE.txt)

**********************************************************/


#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/foreach.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <QFutureWatcher>
#include <QtCore>



#include "LiveAmp.h"

#define VERSIONSTREAM(version) version.Major << "." << version.Minor << "." << version.Build << "." << version.Revision

const int sampling_rates[] = {250,500,1000};

int LiveAmp_SampleSize(HANDLE hDevice, int *typeArray, int* usedChannelsCnt);

MainWindow::MainWindow(QWidget *parent, const std::string &config_file): QMainWindow(parent),ui(new Ui::MainWindow) {
	ui->setupUi(this);
	// parse startup config file
	load_config(config_file);

	// make GUI connections
	QObject::connect(ui->actionQuit, SIGNAL(triggered()), this, SLOT(close()));
	QObject::connect(ui->linkButton, SIGNAL(clicked()), this, SLOT(link()));
	QObject::connect(ui->actionLoad_Configuration, SIGNAL(triggered()), this, SLOT(load_config_dialog()));
	QObject::connect(ui->actionSave_Configuration, SIGNAL(triggered()), this, SLOT(save_config_dialog()));
	QObject::connect(ui->refreshDevices,SIGNAL(clicked()),this,SLOT(refresh_devices()));
	QObject::connect(ui->eegChannelCount, SIGNAL(valueChanged(int)),this, SLOT(update_channel_labels_with_eeg(int)));
	QObject::connect(ui->bipolarChannelCount, SIGNAL(valueChanged(int)),this, SLOT(update_channel_labels_with_bipolar(int)));
	QObject::connect(ui->deviceCb,SIGNAL(currentIndexChanged(int)),this,SLOT(choose_device(int)));
	QObject::connect(ui->auxChannelCount, SIGNAL(valueChanged(int)), this, SLOT(update_channel_labels_aux(int)));
	QObject::connect(ui->useACC, SIGNAL(clicked(bool)), this, SLOT(update_channel_labels_bools(bool)));
	QObject::connect(ui->rbSync, SIGNAL(clicked(bool)), this, SLOT(radio_button_behavior(bool)));
	QObject::connect(ui->rbMirror, SIGNAL(clicked(bool)), this, SLOT(radio_button_behavior(bool)));

	unsampledMarkers = false;
	sampledMarkers = true;
	sampledMarkersEEG = false;

		
}

void MainWindow::update_channel_counters(int n){

	ui->eegChannelCount->setMaximum(n);
	ui->eegChannelCount->setValue(n-ui->bipolarChannelCount->value());
	update_channel_labels_with_bipolar(ui->bipolarChannelCount->value());

}

void MainWindow::update_channel_labels(void) {

	if (!ui->overwriteChannelLabels->isChecked())return;

	int eeg = ui->eegChannelCount->value();
	int bip = ui->bipolarChannelCount->value();
	int aux = ui->auxChannelCount->value();
	int acc = (ui->useACC->isChecked()) ? 3 : 0;

	overrideAutoUpdate = true;
	int maxEEG = (eeg>32)?56:24;
	if (eeg > maxEEG && bip != 0) {
		ui->eegChannelCount->setMaximum(maxEEG);
		eeg = maxEEG;
	}
	overrideAutoUpdate = false;

	int total_electrodes = eeg + bip;

	std::string str;
	ui->channelLabels->clear();
	long long i;
	for (i = 1; i <= total_electrodes; i++) {
		str = std::to_string(i);
		if (i>ui->eegChannelCount->value())str += "*";
		ui->channelLabels->appendPlainText(str.c_str());
		//ui->channelLabels->appendPlainText("\n");
	}
	for (i = 1; i <= aux; i++) {
		str = "AUX_" + std::to_string(i);
		ui->channelLabels->appendPlainText(str.c_str());
	}

	if (acc == 3) {
		ui->channelLabels->appendPlainText("ACC_X\nACC_Y\nACC_Z");
	}

}

void MainWindow::update_channel_labels_with_eeg(int n){
	
	if (overrideAutoUpdate)return;
	update_channel_labels();
}

void MainWindow::update_channel_labels_with_bipolar(int n){
	
	if(!ui->overwriteChannelLabels->isChecked())return;
	update_channel_labels();
	
}

void MainWindow::update_channel_labels_aux(int n) {
	update_channel_labels();
}

void MainWindow::update_channel_labels_bools(bool b) {
	update_channel_labels();
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
		ui->eegChannelCount->setValue(pt.get<int>("settings.channelcount",32));
		eegChannelCount = ui->eegChannelCount->value();
		ui->bipolarChannelCount->setValue(pt.get<int>("settings.bipolarcount",32));		
		ui->chunkSize->setValue(pt.get<int>("settings.chunksize",10));
		ui->samplingRate->setCurrentIndex(pt.get<int>("settings.samplingrate",2));
		ui->auxChannelCount->setValue(pt.get<int>("settings.auxChannelCount", 0));
		ui->useACC->setCheckState(pt.get<bool>("settings.useACC",true) ? Qt::Checked : Qt::Unchecked);
		ui->unsampledMarkers->setCheckState(pt.get<bool>("settings.unsampledmarkers",false) ? Qt::Checked : Qt::Unchecked);	
		ui->sampledMarkers->setCheckState(pt.get<bool>("settings.sampledmarkers",true) ? Qt::Checked : Qt::Unchecked);	
		ui->sampledMarkersEEG->setCheckState(pt.get<bool>("settings.sampledmarkersEEG",false) ? Qt::Checked : Qt::Unchecked);	
		ui->overwriteChannelLabels->setCheckState(pt.get<bool>("settings.overwrite",true) ? Qt::Checked : Qt::Unchecked);	
		t_TriggerOutputMode tom = (t_TriggerOutputMode)pt.get<int>("settings.triggerOutputMode", 0);
		if (tom == TM_SYNC)ui->rbSync->setChecked(true);
		else if (tom == TM_MIRROR)ui->rbMirror->setChecked(true);
		else ui->rbDefault->setChecked(true);
		radio_button_behavior(true);
		int syncFreq = (pt.get<int>("settings.syncFrequency", 1));
		ui->sbSyncFreq->setValue((syncFreq < 1) ? 1 : syncFreq);
		ui->channelLabels->clear();
		BOOST_FOREACH(ptree::value_type &v, pt.get_child("channels.labels"))
			ui->channelLabels->appendPlainText(v.second.data().c_str());
		update_channel_labels();
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
		pt.put("settings.channelcount",ui->eegChannelCount->value());
		pt.put("settings.bipolarcount",ui->bipolarChannelCount->value());
		pt.put("settings.chunksize",ui->chunkSize->value());
		pt.put("settings.samplingrate",ui->samplingRate->currentIndex());
		pt.put("settings.auxChannelCount", ui->auxChannelCount->value());
		pt.put("settings.useACC", ui->useACC->checkState() == Qt::Checked);
		pt.put("settings.unsampledmarkers",ui->unsampledMarkers->checkState()==Qt::Checked);
		pt.put("settings.sampledmarkers",ui->sampledMarkers->checkState()==Qt::Checked);
		pt.put("settings.sampledmarkersEEG",ui->sampledMarkersEEG->checkState()==Qt::Checked);
		pt.put("settings.overwrite",ui->overwriteChannelLabels->checkState()==Qt::Checked);
		pt.put("settings.triggerOutputMode", triggerOutputMode);
		pt.put("settings.syncFrequency", ui->sbSyncFreq->value());

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

void MainWindow::radio_button_behavior(bool b) {
	if (ui->rbSync->isChecked())
	{
		ui->sbSyncFreq->setEnabled(true);
		triggerOutputMode = TM_SYNC;
	}
	if (ui->rbMirror->isChecked())
	{
		ui->sbSyncFreq->setEnabled(false);
		triggerOutputMode = TM_MIRROR;
	}
	if (ui->rbDefault->isChecked()) 
	{
		ui->sbSyncFreq->setEnabled(false);
		triggerOutputMode = TM_DEFAULT;
	}
}

void MainWindow::wait_message(){

	QMessageBox msgBox;
	msgBox.setWindowTitle("Please Wait");
	msgBox.setIcon(QMessageBox::Information);
	msgBox.setText("connecting to devices");
	msgBox.addButton(QMessageBox::Ok);
	msgBox.exec();
}


void MainWindow::refresh_devices(){
	
	ui->deviceCb->blockSignals(true);
	QStringList qsl;
	//std::vector<std::pair<std::string, int>> ampData;
	LIVEAMP_DEVICEENUMINFO deviceInfo;
	this->setCursor(Qt::WaitCursor);
	this->setWindowTitle("Searching for Devices...");
	//wait_message();
	//ampData.clear();
	try{
		LiveAmp::enumerate(deviceInfo, ui->useSim->checkState());
		//LiveAmp::enumerate(ampData, ui->useSim->checkState());
	}catch(std::exception &e) {
		QMessageBox::critical(this,"Error",(std::string("Could not locate LiveAmp device(s): ")+=e.what()).c_str(),QMessageBox::Ok);
	}

	this->setCursor(Qt::ArrowCursor);

	if(!live_amp_sns.empty())live_amp_sns.clear();
	if(!usableChannelsByDevice.empty())usableChannelsByDevice.clear();

	// if we have liveamps, enumerate them in the gui:
	//if(!ampData.empty()) {
	if (!deviceInfo.empty()) {
		ui->deviceCb->clear();
		//for(int k=0;k<foo;k++)
		//	ui->deviceCb->removeItem(k);
		std::stringstream ss;
		int i = 0;

		//for(std::vector<std::pair<std::string, int>>::iterator it=ampData.begin(); it!=ampData.end();++it){
		for (LIVEAMP_DEVICEENUMINFO::iterator it = deviceInfo.begin(); it != deviceInfo.end(); ++it) {
			ss.clear();
			ss << it->sSerialNumber << " (" << it->nChannelCount << ")";
			std::cout<<it->sSerialNumber<<std::endl;
			auto x = ss.str(); // oh, c++...
			qsl << QString(ss.str().c_str()); // oh, Qt...
			live_amp_sns.push_back(it->sSerialNumber);
			usableChannelsByDevice.push_back(it->nChannelCount);
		}
		ui->deviceCb->addItems(qsl);
		choose_device(0);
	}
	
	this->setWindowTitle("LiveAmp Connector");
	ui->deviceCb->blockSignals(true);
	
}


// handle changes in chosen device
void MainWindow::choose_device(int which){
	
	if(!live_amp_sns.empty())
		ui->deviceSerialNumber->setText(QString(live_amp_sns[which].c_str()));
	eegChannelCount = usableChannelsByDevice[ui->deviceCb->currentIndex()];

	update_channel_counters(eegChannelCount);

}

bool MainWindow::check_configuration(){
	bool res = true;

	return res;
}

// start/stop the LiveAmp connection
void MainWindow::link() {

	if (reader_thread_) {
		// === perform unlink action ===
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

		// === perform link action ===

		try {
			// get the UI parameters...
			std::string serialN = ui->deviceSerialNumber->text().toStdString();
			
			
			int chunkSize = ui->chunkSize->value();
			int samplingRate = sampling_rates[ui->samplingRate->currentIndex()];
			
			bool useACC = ui->useACC->checkState()==Qt::Checked;
			bool useSim = ui->useSim->checkState()==Qt::Checked;
			
			unsampledMarkers  = ui->unsampledMarkers->checkState()==Qt::Checked;
			sampledMarkers    = ui->sampledMarkers->checkState()==Qt::Checked;
			sampledMarkersEEG = ui->sampledMarkersEEG->checkState()==Qt::Checked;

			std::vector<std::string> channelLabels;
			boost::algorithm::split(channelLabels,ui->channelLabels->toPlainText().toStdString(),boost::algorithm::is_any_of("\n"));
			
			std::vector<std::string> eegChannelLabels;
			std::vector<std::string> bipolarChannelLabels;
			std::vector<std::string> auxChannelLabels;
			int i=0;
			for(std::vector<std::string>::iterator it=channelLabels.begin();it<channelLabels.end();++it)
			{
				if(i<ui->eegChannelCount->value())
					eegChannelLabels.push_back(*it);
				else if (i-ui->eegChannelCount->value()<ui->bipolarChannelCount->value())
					bipolarChannelLabels.push_back(*it);
				else
					auxChannelLabels.push_back(*it);
				i++;
			}

			if (eegChannelLabels.size() != ui->eegChannelCount->value()) {
				QMessageBox::critical(this, "Error", "The number of eeg channels labels does not match the eeg channel count device setting.", QMessageBox::Ok);
				return;
			}

			if (bipolarChannelLabels.size() != ui->bipolarChannelCount->value()) {
				QMessageBox::critical(this, "Error", "The number of bipolar channels labels does not match the bipolar channel count device setting.", QMessageBox::Ok);
				return;
			}

			// print library version
			t_VersionNumber version;
			GetLibraryVersion(&version);
			std::cout << "Library Version " << VERSIONSTREAM(version) << std::endl;

			// set the sampling rate and serial number
			float fSamplingRate = (float) samplingRate;
			std::string strSerialNumber = ui->deviceSerialNumber->text().toStdString();
			
			//wait_message();
			this->setWindowTitle(QString(std::string("Connecting to "+strSerialNumber).c_str()));
			// prepare the liveamp object
			liveAmp = NULL;

			// change GUI
			this->setCursor(Qt::WaitCursor);
			
			// construct
			std::string error;
			liveAmp = new LiveAmp(strSerialNumber, fSamplingRate, useSim, RM_NORMAL);
			liveAmp->setOutTriggerMode(triggerOutputMode, 8, ui->sbSyncFreq->value(), 5);
			is64 = liveAmp->getIs64();

			// change GUI
			this->setCursor(Qt::ArrowCursor);

			// report
			if(liveAmp==NULL) {
				QMessageBox::critical( this,"Error", "Could not connect to LiveAmp. Please restart the device and check connections.",QMessageBox::Button::Ok);
				ui->linkButton->setText("Link");
				return;	
			}
			
			// this accounts for 64 vs <=32 
			int maxEEG = is64 ? 56 : 24;
			std::vector<int> eegIndices;
			for (int i = 0; i<eegChannelLabels.size(); i++)
				eegIndices.push_back(i);

			std::vector<int> bipolarIndices;
			for (int i = 0; i<bipolarChannelLabels.size(); i++)
				bipolarIndices.push_back(i + maxEEG);

			std::vector<int> auxIndices;
			for (int i = 0; i<ui->auxChannelCount->value(); i++)
				auxIndices.push_back(i + maxEEG+8);
			// enable the channels on the device
			// this will check for availability and will map everything appropriately
			liveAmp->enableChannels(eegIndices, bipolarIndices, auxIndices, useACC);

			// start reader thread
			stop_ = false;
			reader_thread_.reset(new boost::thread(&MainWindow::read_thread, this, chunkSize, samplingRate, channelLabels));

			this->setWindowTitle("LiveAmp Connector");
		}
		catch (std::exception &e) {

			int errorcode = 0;
			try {
				if (liveAmp)if (liveAmp->getHandle() != NULL)liveAmp->close();
				delete liveAmp;
				liveAmp = NULL;
			}
			catch (std::exception &e) {
				;
			}
			QMessageBox::critical(this,"Error",(std::string("Could not initialize the LiveAmp interface: ")+=e.what()).c_str(),QMessageBox::Ok);
			ui->linkButton->setEnabled(true);
			ui->linkButton->setText("Link");
			this->setCursor(Qt::ArrowCursor);
			return;
		}

		// done, all successful
		ui->linkButton->setEnabled(true);
		ui->linkButton->setText("Unlink");
	}

	
}


// background data reader thread
void MainWindow::read_thread(int chunkSize, int samplingRate, std::vector<std::string> channelLabels) {

	lsl::stream_outlet *sampledMarkerOutlet = NULL;
	lsl::stream_outlet *sampledMarkerOutletIn = NULL;
	lsl::stream_outlet *sampledMarkerOutletOut = NULL;
	lsl::stream_outlet *unsampledMarkerOutlet = NULL;
	lsl::stream_outlet *unsampledMarkerOutletIn = NULL;
	lsl::stream_outlet *unsampledMarkerOutletOut = NULL;

	int res = SetPriorityClass(GetCurrentProcess(), HIGH_PRIORITY_CLASS);
	
	BYTE *buffer = NULL;
	size_t sampleCount;
	double now;

	// get local copies of the channel indeces
	std::vector<int> eegIndeces = liveAmp->getEEGIndices();
	std::vector<int> bipolarIndeces = liveAmp->getBipolarIndices();
	std::vector<int> auxIndeces = liveAmp->getAuxIndices();
	std::vector<int> accIndeces = liveAmp->getAccIndices();
	std::vector<int> triggerIndeces = liveAmp->getTrigIndices();
	
	int totalChannelCount = eegIndeces.size() +
		bipolarIndeces.size() + 
		auxIndeces.size() + 
		accIndeces.size();

	int triggerStartIndx = liveAmp->getTrigIndices()[0];
	int triggerIndxOffset = is64 ? 2 : 1;

	// extra EEG channel counter
	int extraEEGMarkerChannelCnt = 0;
	if (sampledMarkersEEG)
	{
		extraEEGMarkerChannelCnt++;
		if (liveAmp->hasSTE())
		{
			extraEEGMarkerChannelCnt++;
			if (!ui->rbDefault->isChecked())
				extraEEGMarkerChannelCnt++;
		}
	}
	
	// these are just used for local loop storage
	std::vector<float> sampleBuffer(totalChannelCount + 
		extraEEGMarkerChannelCnt);

	// this one has the enabled channels for incoming data
	liveAmp->setupLiveAmpBuffer(chunkSize);
	// this one has the full signal for lsl push
	std::vector<std::vector<float>> chunkBuffer(chunkSize,
		std::vector<float>(totalChannelCount + 
		extraEEGMarkerChannelCnt));

	// declarations for sampled/unsampled trigger marker record
	// declare this anyway, tho not always used
	std::vector<std::vector<std::string>> vSampledMarkerBuffer(chunkSize, std::vector<std::string>(1));
	std::vector<std::string> sMarker;
	std::vector<std::vector<std::string>> vSampledMarkerBufferIn(chunkSize, std::vector<std::string>(1));
	std::vector<std::string> sMarkerIn;
	std::vector<std::vector<std::string>> vSampledMarkerBufferOut(chunkSize, std::vector<std::string>(1));
	std::vector<std::string> sMarkerOut;

	// for keeping track of trigger signal changes, which is all we are interested in
	int nMarker = 0;
	int nTmpMarker = 0;
	int nPrevMarker=0;
	int nMarkerSlave = 0;
	int nTmpMarkerSlave = 0;
	int nPrevMarkerSlave = 0;
	int nUnsampledMarker = 0;
	int nUnsampledTmpMarker = 0;
	int nPrevUnsampledMarker = 0;
	int nUnsampledMarkerSlave = 0;
	int nUnsampledTmpMarkerSlave = 0;
	int nPrevUnsampledMarkerSlave = 0;
	int nPrevUnsampledMarkerTest = 0;
	float fMarkerIn;
	float fPrevMarkerIn = 0.0;
	float fMarkerSlave;
	float fPrevMarkerSlave = 0.0;
	float fMarkerOut;
	float fPrevMarkerOut = 0.0;
	float fUnsampledMarkerIn;
	float fPrevUnsampledMarkerIn = 0.0;
	float fUnsampledMarkerOut;
	float fPrevUnsampledMarkerOut = 0.0;

	try {

		// start data streaming
		liveAmp->startAcquisition();

		// create data streaminfo and append some meta-data
		lsl::stream_info dataInfo("LiveAmpSN-" + liveAmp->getSerialNumber(),"EEG", totalChannelCount + extraEEGMarkerChannelCnt, samplingRate,lsl::cf_float32,"LiveAmpSN-" + liveAmp->getSerialNumber());
		lsl::xml_element channels = dataInfo.desc().append_child("channels");
		
		// append the eeg channel labels
		for (std::size_t k = 0; k < channelLabels.size(); k++) {
			if (k < ui->eegChannelCount->value()) {
				channels.append_child("channel")
					.append_child_value("label", channelLabels[k].c_str())
					.append_child_value("type", "EEG")
					.append_child_value("unit", "microvolts");
			}
			else if (k < ui->eegChannelCount->value() + ui->bipolarChannelCount->value()) {
				channels.append_child("channel")
					.append_child_value("label", channelLabels[k].c_str())
					.append_child_value("type", "bipolar")
					.append_child_value("unit", "microvolts");
			}
			else if (k < ui->eegChannelCount->value() + ui->bipolarChannelCount->value() + ui->auxChannelCount->value()) {
				channels.append_child("channel")
					.append_child_value("label", channelLabels[k].c_str())
					.append_child_value("type", "AUX")
					.append_child_value("unit", "microvolts");
			}
			else if(ui->useACC->isChecked()){
				channels.append_child("channel")
					.append_child_value("label", channelLabels[k].c_str())
					.append_child_value("type", "ACC")
					.append_child_value("unit", "milliGs");
			}
			else {
				channels.append_child("channel")
					.append_child_value("label", channelLabels[k].c_str())
					.append_child_value("type", "DriverTimestamps")
					.append_child_value("unit", "seconds");
			}
		}

		// always use the built in trigger
		if(sampledMarkersEEG){
				// append the trigger channel metadata
				channels.append_child("channel")
					.append_child_value("type","DeviceTrigger")
					.append_child_value("unit","integer");
		}

		// only create this channel if the STE is connected
		if (sampledMarkersEEG && liveAmp->hasSTE()) {
			// append the trigger channel metadata
				channels.append_child("channel")
				.append_child_value("type", "STETriggerIn")
				.append_child_value("unit", "integer");
		}

		// only create this channel if the STE is connected and in mirror or sync mode
		if (sampledMarkersEEG && liveAmp->hasSTE()&&!ui->rbDefault->isChecked()) {
			// append the trigger channel metadata
				channels.append_child("channel")
				.append_child_value("type", "STETriggerOut")
				.append_child_value("unit", "integer");
		}

		dataInfo.desc().append_child("acquisition")
			.append_child_value("manufacturer","Brain Products");

		// make a data outlet
		lsl::stream_outlet dataOutlet(dataInfo);

		// create marker streaminfo and outlet
		if(unsampledMarkers) {
			lsl::stream_info markerInfo("LiveAmpSN-" + liveAmp->getSerialNumber() + "-DeviceTrigger","Markers", 1, 0, lsl::cf_string,"LiveAmpSN-" + liveAmp->getSerialNumber() + "_DeviceTrigger");
			unsampledMarkerOutlet = new lsl::stream_outlet(markerInfo);

			if (liveAmp->hasSTE())
			{
				lsl::stream_info markerInfoIn("LiveAmpSN-" + liveAmp->getSerialNumber() + "-STETriggerIn", "Markers", 1, 0, lsl::cf_string, "LiveAmpSN-" + liveAmp->getSerialNumber() + "_STETriggerIn");
				unsampledMarkerOutletIn = new lsl::stream_outlet(markerInfoIn);
				if (!ui->rbDefault->isChecked())
				{
					lsl::stream_info markerInfoOut("LiveAmpSN-" + liveAmp->getSerialNumber() + "-STETriggerOut", "Markers", 1, 0, lsl::cf_string, "LiveAmpSN-" + liveAmp->getSerialNumber() + "_STETriggerOut");
					unsampledMarkerOutletOut = new lsl::stream_outlet(markerInfoOut);
				}
			}
		}	

		// sampled trigger stream as string
		if(sampledMarkers) {
			lsl::stream_info sampledMarkerInfo("LiveAmpSN-" + liveAmp->getSerialNumber() + "-SampledDeviceTrigger","sampledMarkers", 1, samplingRate, lsl::cf_string,"LiveAmpSN-" + liveAmp->getSerialNumber() + "_sampled_DeviceTrigger");
			sampledMarkerOutlet = new lsl::stream_outlet(sampledMarkerInfo);
			if (liveAmp->hasSTE())
			{
				lsl::stream_info sampledMarkerInfoIn("LiveAmpSN-" + liveAmp->getSerialNumber() + "-SampledSTETriggerIn", "sampledMarkers", 1, samplingRate, lsl::cf_string, "LiveAmpSN-" + liveAmp->getSerialNumber() + "_sampled_STETriggerIn");
				sampledMarkerOutletIn = new lsl::stream_outlet(sampledMarkerInfoIn);
				if (!ui->rbDefault->isChecked())
				{
					lsl::stream_info sampledMarkerInfoOut("LiveAmpSN-" + liveAmp->getSerialNumber() + "-SampledSTETriggerOut", "sampledMarkers", 1, samplingRate, lsl::cf_string, "LiveAmpSN-" + liveAmp->getSerialNumber() + "_sampled_STETriggerOut");
					sampledMarkerOutletOut = new lsl::stream_outlet(sampledMarkerInfoOut);
				}
			}
		}			
		
		// read data stream from amplifier
		int32_t bufferSize = (chunkSize + 10) * liveAmp->getSampleSize();
		buffer = new BYTE[bufferSize];

		chunkBuffer.clear();

		int64_t samples_read;
		liveAmp->clearLiveAmpBuffer();
		// enter transmission loop
		while (!stop_) {
			
			// pull the data from the amplifier 
			samples_read = liveAmp->getAmpRawData(buffer, bufferSize);

			if (samples_read <= 0){
				// CPU saver, this is ok even at higher sampling rates
				boost::this_thread::sleep(boost::posix_time::milliseconds(1));
				continue;
			}

			// push the data into a vector of vector of doubles (note: TODO: template-ify this so that it takes any vector type)
			//liveAmp->pushAmpData(buffer, bufferSize, samples_read, liveamp_buffer);
			now = liveAmp->repackageAmpRawData(buffer, bufferSize, samples_read);
			// check to see that we got any samples
			//sampleCount = liveamp_buffer.size();
			sampleCount = liveAmp->getSampleCnt();
			if(sampleCount >= chunkSize){
			
				int k;
				int i;

				// move the data into lsl buffers
				for (i = 0; i < sampleCount; i++) {

					// clear the local buffers
					sampleBuffer.clear();

					// first the EEG + Aux + ACC (if any)
					for (std::vector<int>::iterator it = eegIndeces.begin();
						it != eegIndeces.end();
						++it)
						sampleBuffer.push_back(liveAmp->m_labDataContainer[i][*it]);

					for (std::vector<int>::iterator it = auxIndeces.begin();
						it != auxIndeces.end();
						++it)
						sampleBuffer.push_back(liveAmp->m_labDataContainer[i][*it]);

					for (std::vector<int>::iterator it = accIndeces.begin();
						it != accIndeces.end();
						++it)
						sampleBuffer.push_back(liveAmp->m_labDataContainer[i][*it]);
										
					// next, shove in the trigger markers if necessary
					// if the trigger is a new value, record it, else it is -1		
					nTmpMarker = (1-((int)liveAmp->m_labDataContainer[i][triggerStartIndx] % 2)); 
					if (is64) // check for slave markers
					{
						// we do this bitwise on the off chance that both triggers are pressed/released simultaneously
						nTmpMarkerSlave = (1 - ((int)liveAmp->m_labDataContainer[i][triggerStartIndx + 2] % 2));
						nMarkerSlave = (nTmpMarkerSlave == nPrevMarkerSlave ? 0 : (nTmpMarkerSlave == 1 ? 4 : 2));
						nTmpMarker = nTmpMarker | nMarkerSlave;
						nPrevMarkerSlave = nTmpMarkerSlave;
					}
					
					nMarker = (nTmpMarker == nPrevMarker ? -1 : nTmpMarker);
					nPrevMarker = nTmpMarker;

					// if we want the trigger in the EEG signal:
					if(sampledMarkersEEG)
						sampleBuffer.push_back((float)nMarker);

					// if we want either type of string markers, record it as well
					if(sampledMarkers){
						sMarker.clear();
						sMarker.push_back(nMarker == -1 ? "" : boost::lexical_cast<std::string>(nMarker));
						vSampledMarkerBuffer.push_back(sMarker);		
					}

					// unfortunately, we have up to 3 trigger channels to check
					if (liveAmp->hasSTE()&&(sampledMarkers||sampledMarkersEEG))
					{
						float fTmpMarkerIn = (float)(((int)liveAmp->m_labDataContainer[i][triggerStartIndx] >> 8));
						fMarkerIn = (fTmpMarkerIn == fPrevMarkerIn ? -1.0 : fTmpMarkerIn);
						if (fTmpMarkerIn != fPrevMarkerIn)
							fPrevMarkerIn = fPrevMarkerIn;
						fPrevMarkerIn = fTmpMarkerIn;

						// if we want the trigger in the EEG signal:
						if (sampledMarkersEEG)
							sampleBuffer.push_back(fMarkerIn);

						// if we want either type of string markers, record it as well
						// this is not optimized because later we have to cast a string as an int
						if (sampledMarkers) {
							sMarkerIn.clear();
							sMarkerIn.push_back(fMarkerIn == -1.0 ? "" : boost::lexical_cast<std::string>((int)fMarkerIn));
							vSampledMarkerBufferIn.push_back(sMarkerIn);

						}
						if (!ui->rbDefault->isChecked())
						{
							float fTmpMarkerOut = (float)(((int)liveAmp->m_labDataContainer[i][triggerStartIndx + 1] >> 8));//liveamp_buffer[i][totalChannelCount+1] >> 8));
							fMarkerOut = (fTmpMarkerOut == fPrevMarkerOut ? -1.0 : fTmpMarkerOut);
							fPrevMarkerOut = fTmpMarkerOut;

							// if we want the trigger in the EEG signal:
							if (sampledMarkersEEG)
								sampleBuffer.push_back(fMarkerOut);

							// if we want either type of string markers, record it as well
							// this is not optimized because later we have to cast a string as an int
							if (sampledMarkers) {
								sMarkerOut.clear();
								sMarkerOut.push_back(fMarkerOut == -1.0 ? "" : boost::lexical_cast<std::string>((int)fMarkerOut));
								vSampledMarkerBufferOut.push_back(sMarkerOut);

							}
						}
					}

					// now complete the signal buffer for lsl push
					chunkBuffer.push_back(sampleBuffer);


				}

				if(!ui->enableDriverTimes->isChecked())now = lsl::local_clock();
			
				// push the eeg chunk
				dataOutlet.push_chunk(chunkBuffer, now);
				// clear our data buffers
				chunkBuffer.clear();
				
				// push the unsampled markers one at a time
				if(unsampledMarkers) {
					//std::stringstream ss;
					for(int s=0;s<sampleCount;s++){
						// shift left to 0 out the top 8 bits, then shift right to return and keep the lower 8
						// subtract from 1 because the bit order goes from right to left
						nUnsampledTmpMarker = (1 - ((int)liveAmp->m_labDataContainer[s][triggerStartIndx] % 2));
						// TODO: fix this logic so that both triggers work properly
						//if (is64) // check for slave markers
						//{
						//	// we do this bitwise on the off chance that both triggers are pressed/released simultaneously
						//	nUnsampledTmpMarkerSlave = (1 - ((int)liveAmp->m_labDataContainer[s][triggerStartIndx + 2] % 2));
						//	if (nUnsampledTmpMarkerSlave == nPrevUnsampledMarkerSlave)
						//		nUnsampledMarkerSlave = 0;
						//	else {
						//		nUnsampledMarkerSlave = (nUnsampledTmpMarkerSlave == 1) ? 4 : 2;
						//	}
						//
						//	nUnsampledMarker = nUnsampledTmpMarker | nUnsampledMarkerSlave;
						//								
						//	if (nUnsampledTmpMarker != nPrevUnsampledMarker || nUnsampledTmpMarker != nPrevUnsampledMarkerSlave) {
						//		std::string sMarker = boost::lexical_cast<std::string>(nUnsampledMarker);
						//		unsampledMarkerOutlet->push_sample(&sMarker, now + (s + 1 - sampleCount) / samplingRate);
						//		
						//	}
						//	nPrevUnsampledMarkerSlave = nUnsampledTmpMarkerSlave;
						//	nPrevUnsampledMarker = nUnsampledTmpMarker;
						//}

						//else
						//{
							if (nUnsampledTmpMarker != nPrevUnsampledMarker) {
								std::string sMarker = boost::lexical_cast<std::string>(nUnsampledTmpMarker);
								unsampledMarkerOutlet->push_sample(&sMarker, now + (s + 1 - sampleCount) / samplingRate);
							}
							nPrevUnsampledMarker = nUnsampledTmpMarker;
						//}

						if (liveAmp->hasSTE())
						{
							fUnsampledMarkerIn = (float)(((int)liveAmp->m_labDataContainer[s][triggerStartIndx] >> 8));
							if (fUnsampledMarkerIn != fPrevUnsampledMarkerIn) {
								std::string sMarkerIn = boost::lexical_cast<std::string>(fUnsampledMarkerIn);
								unsampledMarkerOutletIn->push_sample(&sMarkerIn, now + (s + 1 - sampleCount) / samplingRate);
							}
							fPrevUnsampledMarkerIn = fUnsampledMarkerIn;
							if (!ui->rbDefault->isChecked())
							{
								fUnsampledMarkerOut = (float)(((int)liveAmp->m_labDataContainer[s][triggerStartIndx + 1] >> 8));
								if (fUnsampledMarkerOut != fPrevUnsampledMarkerOut) {
									std::string sMarkerOut = boost::lexical_cast<std::string>(fUnsampledMarkerOut);
									unsampledMarkerOutletOut->push_sample(&sMarkerOut, now + (s + 1 - sampleCount) / samplingRate);
								}
								fPrevUnsampledMarkerOut = fUnsampledMarkerOut;
							}
						}
					}
				}

				liveAmp->clearLiveAmpBuffer();
				// push the sampled markers
				if(sampledMarkers) {
					sampledMarkerOutlet->push_chunk(vSampledMarkerBuffer, now);
					if (liveAmp->hasSTE())
					{
						sampledMarkerOutletIn->push_chunk(vSampledMarkerBufferIn, now);
						if (!ui->rbDefault->isChecked())
						{
							sampledMarkerOutletOut->push_chunk(vSampledMarkerBufferOut, now);
							vSampledMarkerBufferOut.clear();
						}
						vSampledMarkerBufferIn.clear();
					}
					vSampledMarkerBuffer.clear();	

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
	// cleanup (if necessary)
	if (unsampledMarkers)
	{
		delete(unsampledMarkerOutlet);
		if (liveAmp->hasSTE())
		{
			delete(unsampledMarkerOutletIn);
			if (!ui->rbDefault->isChecked())
				delete(unsampledMarkerOutletOut);
		}

	}
	if (sampledMarkers)
	{
		delete(sampledMarkerOutlet);
		if (liveAmp->hasSTE())
		{
			delete(sampledMarkerOutletIn);
			if (!ui->rbDefault->isChecked())
				delete(sampledMarkerOutletOut);
		}
	}
	try{
		liveAmp->stopAcquisition(); 
		liveAmp->close();
		delete liveAmp;
	}catch(std::exception &e) {
		// any problem closing liveamp
		std::cerr << e.what() << std::endl;
	}


}



MainWindow::~MainWindow() {

	delete ui;
}

