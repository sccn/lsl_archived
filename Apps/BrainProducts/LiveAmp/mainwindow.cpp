
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
	QObject::connect(ui->useACC, SIGNAL(clicked(bool)), this, SLOT(update_channel_labels_acc(bool)));
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
	// TODO: parameterize this to follow behavior according to 
	// available channels
	if (eeg > 24 && bip != 0) {
		ui->eegChannelCount->setMaximum(24);
		eeg = 24;
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

void MainWindow::update_channel_labels_aux(int n) {
	update_channel_labels();
}

void MainWindow::update_channel_labels_acc(bool b) {
	update_channel_labels();
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
	std::vector<std::pair<std::string, int>> ampData;
	this->setCursor(Qt::WaitCursor);
	this->setWindowTitle("Searching for Devices...");
	//wait_message();
	//ampData.clear();
	try{
		LiveAmp::enumerate(ampData, ui->useSim->checkState());
	}catch(std::exception &e) {
		QMessageBox::critical(this,"Error",(std::string("Could not locate LiveAmp device(s): ")+=e.what()).c_str(),QMessageBox::Ok);
	}

	this->setCursor(Qt::ArrowCursor);

	if(!live_amp_sns.empty())live_amp_sns.clear();
	if(!usableChannelsByDevice.empty())usableChannelsByDevice.clear();

	// if we have liveamps, enumerate them in the gui:
	int foo = ui->deviceCb->count();
	if(!ampData.empty()) {
		ui->deviceCb->clear();
		//for(int k=0;k<foo;k++)
		//	ui->deviceCb->removeItem(k);
		std::stringstream ss;
		int i=0;
		
		for(std::vector<std::pair<std::string, int>>::iterator it=ampData.begin(); it!=ampData.end();++it){
			ss.clear();
			ss << it->first << " (" << it->second << ")";
			std::cout<<it->first<<std::endl;
			auto x = ss.str(); // oh, c++...
			qsl << QString(ss.str().c_str()); // oh, Qt...
			live_amp_sns.push_back(it->first);
			usableChannelsByDevice.push_back(it->second);
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

			//if (auxChannelLabels.size() != ui->auxChannelCount->value()){
			//	QMessageBox::critical(this,"Error","The number of aux channels labels does not match the aux channel count device setting.",QMessageBox::Ok);
			//	return;
			//}

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
			//if (liveAmp->hasSTE())         // if there is a STE, set the output mode, sync values are hardcoded except for freq
			liveAmp->setOutTriggerMode(triggerOutputMode, 8, ui->sbSyncFreq->value(), 5);

			if(!check_configuration())
			{
				QMessageBox::critical(this,"Error","Requested configuration settings do not match the hardware settings on your device. Please check your module's channel settings." ,QMessageBox::Ok);
				return;
			}

			// change GUI
			this->setCursor(Qt::ArrowCursor);

			// report
			if(liveAmp!=NULL);
				//QMessageBox::information( this,"Connected!", "Click OK to proceed",QMessageBox::Button::Ok);

			else {
				QMessageBox::critical( this,"Error", "Could not connect to LiveAmp. Please restart the device and check connections.",QMessageBox::Button::Ok);
				ui->linkButton->setText("Link");
				return;	
			}
			
			
			// for now this is hard coded
			// we simply enumerate eegchannels via labels
			// this should be flexible and settable via the GUI
			// i.e. the user can map channel labels to eegIndeces
			//boost::shared_ptr<std::vector<int>> eegIndices(new std::vector<int>);
			////eegIndices->clear();
			//for(int i=0;i<eegChannelLabels.size();i++)
			//	eegIndices->push_back(i);

			//boost::shared_ptr<std::vector<int>> bipolarIndices(new std::vector<int>);
			////bipolarIndices->clear();
			//for(int i=0;i<bipolarChannelLabels.size();i++)
			//	bipolarIndices->push_back(i+24);

			std::vector<int> eegIndices;
			//eegIndices->clear();
			for (int i = 0; i<eegChannelLabels.size(); i++)
				eegIndices.push_back(i);

			std::vector<int> bipolarIndices;
			//bipolarIndices->clear();
			for (int i = 0; i<bipolarChannelLabels.size(); i++)
				bipolarIndices.push_back(i + 24);

			std::vector<int> auxIndices;
			//auxIndices->clear();
			for (int i = 0; i<ui->auxChannelCount->value(); i++)
				auxIndices.push_back(i + 32);
			// enable the channels on the device
			// this will check for availability and will map everything appropriately
			//liveAmp->enableChannels(*eegIndices, *bipolarIndices, *auxIndices, useACC);
			liveAmp->enableChannels(eegIndices, bipolarIndices, auxIndices, useACC);

			// start reader thread
			stop_ = false;
			reader_thread_.reset(new boost::thread(&MainWindow::read_thread, this, chunkSize, samplingRate, channelLabels));

			this->setWindowTitle("LiveAmp Connector");
		} catch(std::exception &e) {
	
		
			int errorcode=0; 
			if(liveAmp)if(liveAmp->getHandle()!=NULL)liveAmp->close();
			delete liveAmp;
			liveAmp = NULL;
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

	lsl::stream_outlet *marker_outlet = NULL;
	lsl::stream_outlet *marker_outlet_in = NULL;
	lsl::stream_outlet *marker_outlet_out = NULL;
	lsl::stream_outlet *s_marker_outlet = NULL;
	lsl::stream_outlet *s_marker_outlet_in = NULL;
	lsl::stream_outlet *s_marker_outlet_out = NULL;

	int res = SetPriorityClass(GetCurrentProcess(), HIGH_PRIORITY_CLASS);

	bool started = false;	
	BYTE *buffer = NULL;
	// for chunk storage
	size_t sampleCount;
	int totalChannelCount = ui->eegChannelCount->value() +
		ui->bipolarChannelCount->value() +
		ui->auxChannelCount->value() +
		((ui->useACC->isChecked()) ? 3 : 0);


	// extra EEG channel counter
	int extraEEGMarkerChannelCnt = 0;
	if (sampledMarkersEEG)
	{
		if (liveAmp->hasSTE())
		{
			extraEEGMarkerChannelCnt = 2;
			if (!ui->rbDefault->isChecked())
				extraEEGMarkerChannelCnt = 3;
		}
	}
	
	// these are just used for local loop storage
	std::vector<float> sample_buffer(totalChannelCount);

	// this one has the enabled channels for incoming data
	std::vector<std::vector<float>> liveamp_buffer(chunkSize,std::vector<float>(liveAmp->getEnabledChannelCnt()));
		
	// this one has the full signal for lsl push
	std::vector<std::vector<float>> chunk_buffer(chunkSize,std::vector<float>(totalChannelCount + extraEEGMarkerChannelCnt));

	// declarations for sampled/unsampled trigger marker record
	// declare this anyway, tho not always used
	std::vector<std::vector<std::string>> sampled_marker_buffer(chunkSize, std::vector<std::string>(1));
	std::vector<std::string> s_mrkr;
	std::vector<std::vector<std::string>> sampled_marker_buffer_in(chunkSize, std::vector<std::string>(1));
	std::vector<std::string> s_mrkr_in;
	std::vector<std::vector<std::string>> sampled_marker_buffer_out(chunkSize, std::vector<std::string>(1));
	std::vector<std::string> s_mrkr_out;

	std::vector<int16_t> unsampled_marker_buffer(chunkSize);
	std::vector<int16_t> unsampled_marker_buffer_in(chunkSize);
	std::vector<int16_t> unsampled_marker_buffer_out(chunkSize);

	// for keeping track of trigger signal changes, which is all we are interested in
	float f_mrkr;
	float prev_marker_float=0.0;
	float f_mrkr_in;
	float prev_marker_float_in = 0.0;
	float f_mrkr_out;
	float prev_marker_float_out = 0.0;
	float f_u_mrkr;
	float prev_u_marker_float = 0.0;
	float f_u_mrkr_in;
	float prev_u_marker_float_in = 0.0;
	float f_u_mrkr_out;
	float prev_u_marker_float_out = 0.0;



	try {

		// start data streaming
		liveAmp->startAcquisition();

		// get a local copy of this so we don't have to copy on each loop iteration
		std::vector<int>triggerIndeces = liveAmp->getTrigIndices();

		// create data streaminfo and append some meta-data
		lsl::stream_info data_info("LiveAmpSN-" + liveAmp->getSerialNumber(),"EEG", totalChannelCount + extraEEGMarkerChannelCnt, samplingRate,lsl::cf_float32,"LiveAmpSN-" + liveAmp->getSerialNumber());
		lsl::xml_element channels = data_info.desc().append_child("channels");
		
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
			else {
				channels.append_child("channel")
					.append_child_value("label", channelLabels[k].c_str())
					.append_child_value("type", "ACC")
					.append_child_value("unit", "milligrams");
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

		data_info.desc().append_child("acquisition")
			.append_child_value("manufacturer","Brain Products");

		// make a data outlet
		lsl::stream_outlet data_outlet(data_info);



		// create marker streaminfo and outlet
		if(unsampledMarkers) {
			lsl::stream_info marker_info("LiveAmpSN-" + liveAmp->getSerialNumber() + "-DeviceTrigger","Markers", 1, 0, lsl::cf_string,"LiveAmpSN-" + liveAmp->getSerialNumber() + "_DeviceTrigger");
			marker_outlet = new lsl::stream_outlet(marker_info);

			if (liveAmp->hasSTE())
			{
				lsl::stream_info marker_info_in("LiveAmpSN-" + liveAmp->getSerialNumber() + "-STETriggerIn", "Markers", 1, 0, lsl::cf_string, "LiveAmpSN-" + liveAmp->getSerialNumber() + "_STETriggerIn");
				marker_outlet_in = new lsl::stream_outlet(marker_info_in);
				if (!ui->rbDefault->isChecked())
				{
					lsl::stream_info marker_info_out("LiveAmpSN-" + liveAmp->getSerialNumber() + "-STETriggerOut", "Markers", 1, 0, lsl::cf_string, "LiveAmpSN-" + liveAmp->getSerialNumber() + "_STETriggerOut");
					marker_outlet_out = new lsl::stream_outlet(marker_info_out);
				}
			}
		}	

		// sampled trigger stream as string
		if(sampledMarkers) {
			lsl::stream_info s_marker_info("LiveAmpSN-" + liveAmp->getSerialNumber() + "-SampledDeviceTrigger","sampledMarkers", 1, samplingRate, lsl::cf_string,"LiveAmpSN-" + liveAmp->getSerialNumber() + "_sampled_DeviceTrigger");
			s_marker_outlet = new lsl::stream_outlet(s_marker_info);
			if (liveAmp->hasSTE())
			{
				lsl::stream_info s_marker_info_in("LiveAmpSN-" + liveAmp->getSerialNumber() + "-SampledSTETriggerIn", "sampledMarkers", 1, samplingRate, lsl::cf_string, "LiveAmpSN-" + liveAmp->getSerialNumber() + "_sampled_STETriggerIn");
				s_marker_outlet_in = new lsl::stream_outlet(s_marker_info_in);
				if (!ui->rbDefault->isChecked())
				{
					lsl::stream_info s_marker_info_out("LiveAmpSN-" + liveAmp->getSerialNumber() + "-SampledSTETriggerOut", "sampledMarkers", 1, samplingRate, lsl::cf_string, "LiveAmpSN-" + liveAmp->getSerialNumber() + "_sampled_STETriggerOut");
					s_marker_outlet_out = new lsl::stream_outlet(s_marker_info_out);
				}
			}
		}			
		int last_mrk = 0;

		// read data stream from amplifier
		int32_t bufferSize = (chunkSize + 10) * liveAmp->getSampleSize();
		buffer = new BYTE[bufferSize];

		chunk_buffer.clear();

		int64_t samples_read;

		// enter transmission loop
		while (!stop_) {
			

			// pull the data from the amplifier 
			samples_read = liveAmp->pullAmpData(buffer, bufferSize);


			if (samples_read <= 0){
				// CPU saver, this is ok even at higher sampling rates
				boost::this_thread::sleep(boost::posix_time::milliseconds(1));
				continue;
			}

			// push the data into a vector of vector of doubles (note: TODO: template-ify this so that it takes any vector type)
			liveAmp->pushAmpData(buffer, bufferSize, samples_read, liveamp_buffer);
	
			// check to see that we got any samples
			sampleCount = liveamp_buffer.size();
			
			if(sampleCount >= chunkSize){
			
				int k;
				int i;

				// transpose the data into lsl buffers
				for (i=0;i<sampleCount;i++) {

					// clear the local buffers
					sample_buffer.clear();
											
					// first the EEG + Aux + ACC (if any)
					for (k=0; k<totalChannelCount; k++)	
						sample_buffer.push_back(liveamp_buffer[i][k]); 

					// next, shove in the trigger markers if necessary

					// if the trigger is a new value, record it, else it is 0.0
					// totalChannelCount is always equivalent to the last channel in the liveamp_buffer
					// which corresponds to the output trigger, the one before it is the input trigger
					float mrkr_tmp = (float)(1-((int)liveamp_buffer[i][totalChannelCount] % 2)); // only 1 bit
					f_mrkr = (mrkr_tmp == prev_marker_float ? -1.0 : (float)((int)liveamp_buffer[i][totalChannelCount] % 2));
					prev_marker_float = mrkr_tmp;

					// if we want the trigger in the EEG signal:
					if(sampledMarkersEEG)
						sample_buffer.push_back(f_mrkr);

					// if we want either type of string markers, record it as well
					if(sampledMarkers){
						s_mrkr.clear();
						s_mrkr.push_back(f_mrkr == -1.0 ? "" : boost::lexical_cast<std::string>((int)f_mrkr));
						sampled_marker_buffer.push_back(s_mrkr);
						
					}

					// unfortunately, we have up to 3 trigger channels to check
					if (liveAmp->hasSTE()&&(sampledMarkers||sampledMarkersEEG))
					{
						float mrkr_tmp_in = (float)(255-((int)liveamp_buffer[i][totalChannelCount] >> 8));
						f_mrkr_in = (mrkr_tmp_in == prev_marker_float_in ? -1.0 : mrkr_tmp_in);
						if (mrkr_tmp_in != prev_marker_float_in)
							prev_marker_float_in = prev_marker_float_in;
						prev_marker_float_in = mrkr_tmp_in;

						// if we want the trigger in the EEG signal:
						if (sampledMarkersEEG)
							sample_buffer.push_back(f_mrkr_in);

						// if we want either type of string markers, record it as well
						// this is not optimized because later we have to cast a string as an int
						if (sampledMarkers) {
							s_mrkr_in.clear();
							s_mrkr_in.push_back(f_mrkr_in == -1.0 ? "" : boost::lexical_cast<std::string>((int)f_mrkr_in));
							sampled_marker_buffer_in.push_back(s_mrkr_in);

						}
						if (!ui->rbDefault->isChecked())
						{
							float mrkr_tmp_out = (float)(255-((int)liveamp_buffer[i][totalChannelCount+1] >> 8));
							f_mrkr_out = (mrkr_tmp_out == prev_marker_float_out ? -1.0 : mrkr_tmp_out);
							prev_marker_float_out = mrkr_tmp_out;

							// if we want the trigger in the EEG signal:
							if (sampledMarkersEEG)
								sample_buffer.push_back(f_mrkr_out);

							// if we want either type of string markers, record it as well
							// this is not optimized because later we have to cast a string as an int
							if (sampledMarkers) {
								s_mrkr_out.clear();
								s_mrkr_out.push_back(f_mrkr_out == -1.0 ? "" : boost::lexical_cast<std::string>((int)f_mrkr_out));
								sampled_marker_buffer_out.push_back(s_mrkr_out);

							}
						}
					}

					// now complete the signal buffer for lsl push
					chunk_buffer.push_back(sample_buffer);


				}

				double now = lsl::local_clock();
			
				// push the eeg chunk
				data_outlet.push_chunk(chunk_buffer, now);
				// clear our data buffers
				chunk_buffer.clear();


				// push the unsampled markers one at a time
				if(unsampledMarkers) {
					//std::stringstream ss;
					for(int s=0;s<sampleCount;s++){
						// shift left to 0 out the top 8 bits, then shift right to return and keep the lower 8
						// subtract from 1 because the bit order goes from right to left
						f_u_mrkr = (float)(1-(int)liveamp_buffer[s][totalChannelCount]%2);
						if(f_u_mrkr!=prev_u_marker_float){
							std::string mrk_string = boost::lexical_cast<std::string>(f_u_mrkr);	
							marker_outlet->push_sample(&mrk_string,now + (s + 1 - sampleCount)/samplingRate);
						}
						prev_u_marker_float = f_u_mrkr;
						if (liveAmp->hasSTE())
						{
							f_u_mrkr_in = (float)(255-((int)liveamp_buffer[s][totalChannelCount] >> 8));
							if (f_u_mrkr_in != prev_u_marker_float_in) {
								std::string mrk_string_in = boost::lexical_cast<std::string>(f_u_mrkr_in);
								marker_outlet_in->push_sample(&mrk_string_in, now + (s + 1 - sampleCount) / samplingRate);
							}
							prev_u_marker_float_in = f_u_mrkr_in;
							if (!ui->rbDefault->isChecked())
							{
								f_u_mrkr_out = (float)(255-((int)liveamp_buffer[s][totalChannelCount+1] >> 8));
								if (f_u_mrkr_out != prev_u_marker_float_out) {
									std::string mrk_string_out = boost::lexical_cast<std::string>(f_u_mrkr_out);
									marker_outlet_out->push_sample(&mrk_string_out, now + (s + 1 - sampleCount) / samplingRate);
								}
								prev_u_marker_float_out = f_u_mrkr_out;
							}
						}
					}
				}

								
				liveamp_buffer.clear();

				// push the sampled markers
				if(sampledMarkers) {
					s_marker_outlet->push_chunk(sampled_marker_buffer, now);
					if (liveAmp->hasSTE())
					{
						s_marker_outlet_in->push_chunk(sampled_marker_buffer_in, now);
						if (!ui->rbDefault->isChecked())
						{
							s_marker_outlet_out->push_chunk(sampled_marker_buffer_out, now);
							sampled_marker_buffer_out.clear();
						}
						sampled_marker_buffer_in.clear();
					}
					sampled_marker_buffer.clear();	

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
		delete(marker_outlet);
		if (liveAmp->hasSTE())
		{
			delete(marker_outlet_in);
			if (!ui->rbDefault->isChecked())
				delete(marker_outlet_out);
		}

	}
	if (sampledMarkers)
	{
		delete(s_marker_outlet);
		if (liveAmp->hasSTE())
		{
			delete(s_marker_outlet_in);
			if (!ui->rbDefault->isChecked())
				delete(s_marker_outlet_out);
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

