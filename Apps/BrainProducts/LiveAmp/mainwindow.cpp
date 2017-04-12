
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
	QObject::connect(ui->deviceCb,SIGNAL(currentIndexChanged(int)),this,SLOT(choose_device(int)));

	unsampledMarkers = false;
	sampledMarkers = true;
	sampledMarkersEEG = false;

		
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
		pt.put("settings.deviceSerialNumber",ui->deviceSerialNumber->text().toStdString());
		pt.put("settings.channelcount",ui->channelCount->value());
		pt.put("settings.chunksize",ui->chunkSize->value());
		pt.put("settings.samplingrate",ui->samplingRate->currentIndex());
		pt.put("settings.useBipolar",ui->useBipolar->checkState()==Qt::Checked);
		pt.put("settings.useAux",ui->useAUX->checkState()==Qt::Checked);
		pt.put("settings.activeshield",ui->useACC->checkState()==Qt::Checked);
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


void MainWindow::wait_message(){

	QMessageBox msgBox;
	msgBox.setWindowTitle("Please Wait");
	msgBox.setIcon(QMessageBox::Information);
	msgBox.setText("connecting to devices");
	msgBox.addButton(QMessageBox::Ok);
	msgBox.exec();
}


void MainWindow::refresh_devices(){
	

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

	if(!live_amp_sns.empty())
		live_amp_sns.clear();
	// if we have liveamps, enumerate them in the gui:
	int foo = ui->deviceCb->count();
	if(!ampData.empty()) {
		for(int k=0;k<foo;k++)
			ui->deviceCb->removeItem(k);
		std::stringstream ss;
		int i=0;
		QStringList qsl;
		for(std::vector<std::pair<std::string, int>>::iterator it=ampData.begin(); it!=ampData.end();++it){
			ss.clear();
			ss << it->first << " (" << it->second << ")";
			auto x = ss.str(); // oh, c++...
			qsl << QString(x.c_str()); // oh, Qt...
			live_amp_sns.push_back(it->first);
		}
		ui->deviceCb->addItems(qsl);
	}
	
	this->setWindowTitle("LiveAmp Connector");
	
}


// handle changes in chosen device
void MainWindow::choose_device(int which){
	if(!live_amp_sns.empty())
		ui->deviceSerialNumber->setText(QString(live_amp_sns[which].c_str()));

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
			
			bool useAUX = ui->useAUX->checkState()==Qt::Checked;
			bool useACC = ui->useACC->checkState()==Qt::Checked;
			bool useBipolar = ui->useBipolar->checkState()==Qt::Checked;
			bool useSim = ui->useSim->checkState()==Qt::Checked;

			unsampledMarkers  = ui->unsampledMarkers->checkState()==Qt::Checked;
			sampledMarkers    = ui->sampledMarkers->checkState()==Qt::Checked;
			sampledMarkersEEG = ui->sampledMarkersEEG->checkState()==Qt::Checked;

			std::vector<std::string> eegChannelLabels;
			boost::algorithm::split(eegChannelLabels,ui->channelLabels->toPlainText().toStdString(),boost::algorithm::is_any_of("\n"));
			if (eegChannelLabels.size() != ui->channelCount->value()){
				QMessageBox::critical(this,"Error","The number of eeg channels labels does not match the channel count device setting.",QMessageBox::Ok);
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
			std::vector<int>eegIndeces;
			eegIndeces.clear();
			for(int i=0;i<eegChannelLabels.size();i++)
				eegIndeces.push_back(i);

			// enable the channels on the device
			// this will check for availability and will map everything appropriately
			liveAmp->enableChannels(eegIndeces, useAUX, useACC, useBipolar);

			// start reader thread
			stop_ = false;
			reader_thread_.reset(new boost::thread(&MainWindow::read_thread, this, chunkSize, samplingRate, useAUX, useACC, useBipolar, eegChannelLabels));
			
			this->setWindowTitle("LiveAmp Connector");
		} catch(std::exception &e) {
	
		
			int errorcode=0; 

			delete liveAmp;

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
void MainWindow::read_thread(int chunkSize, int samplingRate, bool useAUX, bool useACC, bool useBipolar, std::vector<std::string> eegChannelLabels){

	int res = SetPriorityClass(GetCurrentProcess(), HIGH_PRIORITY_CLASS);

	bool started = false;	
	BYTE *buffer = NULL;
	// for chunk storage
	int sampleCount;

	// number of eeg+aux+acc channels (nothing special to do here, just shove it into a vector of vectors for lsl push)
	int eegAuxAccChannelCount = eegChannelLabels.size();
	if(useAUX) eegAuxAccChannelCount += liveAmp->getAuxIndeces().size();
	if(useACC) eegAuxAccChannelCount += liveAmp->getAccIndeces().size();

	// these are just used for local loop storage
	std::vector<float> eeg_buffer(eegAuxAccChannelCount);
	int16_t int_marker;

	// this one has the enabled channels for incoming data
	std::vector<std::vector<float>> liveamp_buffer(chunkSize,std::vector<float>(liveAmp->getEnabledChannelCnt()));
		
	// this one has the full signal for lsl push
	std::vector<std::vector<float>> eeg_aux_acc_buffer(chunkSize,std::vector<float>(eegAuxAccChannelCount + (sampledMarkersEEG ? 1 : 0)));

	// declarations for sampled/unsampled trigger marker record
	// declare this anyway, tho not always used
	std::vector<std::vector<std::string>> sampled_marker_buffer(chunkSize, std::vector<std::string>(1));
	std::vector<std::string> s_mrkr;

	std::vector<int16_t> unsampled_marker_buffer(chunkSize);

	// for keeping track of trigger signal changes, which is all we are interested in
	float f_mrkr;
	float prev_marker_float;
	float f_u_mrkr;
	float prev_u_marker_float;



	try {

		// start data streaming
		liveAmp->startAcquisition();

		// get a local copy of this so we don't have to copy on each loop iteration
		std::vector<int>triggerIndeces = liveAmp->getTrigIndeces();

		// create data streaminfo and append some meta-data
		lsl::stream_info data_info("LiveAmpSN-" + liveAmp->getSerialNumber(),"EEG", eegAuxAccChannelCount + (sampledMarkersEEG ? 1 : 0), samplingRate,lsl::cf_float32,"LiveAmpSN-" + liveAmp->getSerialNumber());
		lsl::xml_element channels = data_info.desc().append_child("channels");
		
		// append the eeg channel labels -- again, this is currently hard coded 
		// TODO: change this to user controlled via GUI
		for (std::size_t k=0;k<eegChannelLabels.size();k++)
			channels.append_child("channel")
				.append_child_value("label",eegChannelLabels[k].c_str())
				.append_child_value("type","EEG")
				.append_child_value("unit","microvolts");

		// append the aux channel metadata
		for (std::size_t k=0;k<liveAmp->getAuxIndeces().size();k++)
			channels.append_child("channel")
				.append_child_value("type","AUX")
				.append_child_value("unit","microvolts");

		// append the accelerometer channel metadata
		for (std::size_t k=0;k<liveAmp->getAccIndeces().size();k++)
			channels.append_child("channel")
				.append_child_value("type","ACC")
				.append_child_value("unit","microvolts")
				.append_child_value("direction",(k%3==0 ? "X":(k%3 ? "Y":"Z")) );

		if(sampledMarkersEEG){
			// append the trigger channel metadata
			for (std::size_t k=0;k<liveAmp->getTrigIndeces().size();k++)
				channels.append_child("channel")
					.append_child_value("type","Trigger")
					.append_child_value("unit","microvolts");
		}

		data_info.desc().append_child("acquisition")
			.append_child_value("manufacturer","Brain Products");

		// make a data outlet
		lsl::stream_outlet data_outlet(data_info);

		lsl::stream_outlet *marker_outlet;
		lsl::stream_outlet *s_marker_outlet;

		// create marker streaminfo and outlet
		if(unsampledMarkers) {
			lsl::stream_info marker_info("LiveAmpSN-" + liveAmp->getSerialNumber() + "-Markers","Markers", 1, 0, lsl::cf_string,"LiveAmpSN-" + liveAmp->getSerialNumber() + "_markers");
			marker_outlet = new lsl::stream_outlet(marker_info);
		}	

		// sampled trigger stream as string
		if(sampledMarkers) {
			lsl::stream_info s_marker_info("LiveAmpSN-" + liveAmp->getSerialNumber() + "-Sampled-Markers","sampledMarkers", 1, samplingRate, lsl::cf_string,"LiveAmpSN-" + liveAmp->getSerialNumber() + "_sampled_markers");
			s_marker_outlet = new lsl::stream_outlet(s_marker_info);
			// ditch the outlet if we don't need it (need to do it this way in order to trick C++ compiler into using this object conditionally)
		}
			
			
		int last_mrk = 0;
		int bytes_read;

		// read data stream from amplifier
		int32_t bufferSize = (chunkSize + 10) * liveAmp->getSampleSize();
		buffer = new BYTE[bufferSize];

		eeg_aux_acc_buffer.clear();

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
					eeg_buffer.clear();
											
					// first the EEG + Aux + ACC (if any)
					for (k=0; k<eegAuxAccChannelCount; k++)	
						eeg_buffer.push_back(liveamp_buffer[i][k]); 

					// next, shove in the trigger markers if necessary

					// if the trigger is a new value, record it, else it is 0.0
					// eegAuxAccChannelCount is always equivalent to the last channel in the liveamp_buffer
					float mrkr_tmp = (liveamp_buffer[i][eegAuxAccChannelCount]);
					f_mrkr = (mrkr_tmp == prev_marker_float ? -1.0 : liveamp_buffer[i][eegAuxAccChannelCount]);
					prev_marker_float = mrkr_tmp;

					// if we want the trigger in the EEG signal:
					if(sampledMarkersEEG)
						eeg_buffer.push_back(f_mrkr);

					// if we want either type of string markers, record it as well
					// this is not optimized because later we have to cast a string as an int
					if(sampledMarkers || unsampledMarkers){
						s_mrkr.clear();
						s_mrkr.push_back(f_mrkr == -1.0 ? "" : boost::lexical_cast<std::string>(liveamp_buffer[i][eegAuxAccChannelCount]));
						sampled_marker_buffer.push_back(s_mrkr);
						
					}

					// now complete the signal buffer for lsl push
					eeg_aux_acc_buffer.push_back(eeg_buffer);


				}

				double now = lsl::local_clock();
			
				// push the eeg chunk
				data_outlet.push_chunk(eeg_aux_acc_buffer, now);
				// clear our data buffers
				eeg_aux_acc_buffer.clear();


				// push the unsampled markers one at a time
				if(unsampledMarkers) {
					//std::stringstream ss;
					for(int s=0;s<sampleCount;s++){
						f_u_mrkr = liveamp_buffer[s][eegAuxAccChannelCount];
						//if(strcmp(sampled_marker_buffer[i][0].c_str(), "")){
						//	ss.clear();
						//	ss << sampled_marker_buffer[i][0];
						//	marker_outlet->push_sample(&(ss.str()),now + (i + 1 - sampleCount)/samplingRate);
						//	
						//}
						if(f_u_mrkr!=prev_u_marker_float){
							std::string mrk_string = boost::lexical_cast<std::string>(f_u_mrkr);	
							marker_outlet->push_sample(&mrk_string,now + (s + 1 - sampleCount)/samplingRate);
						}
						prev_u_marker_float = f_u_mrkr;
					}
				}

								
				liveamp_buffer.clear();

				// push the sampled markers
				if(sampledMarkers) {
					s_marker_outlet->push_chunk(sampled_marker_buffer, now);
					sampled_marker_buffer.clear();	
				}
			}
		}
		// cleanup (if necessary)
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

	if(buffer != NULL)
		delete[] buffer;
	buffer = NULL;
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

