#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/foreach.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/asio.hpp>
#include <boost/chrono.hpp>

#include <QCloseEvent>

#include "mainwindow.h"
#include "ui_mainwindow.h"

#ifdef _WIN32
	#include <windows.h>
	#include <conio.h>
#endif


void MainWindow::listen(int sps){

    //std::cout<<"Welcome to the listen thread...\n";
	QString status; 
	float per = 1.0/(float)sps;
	per = 1.0;
	IEE_DataChannel_t epochChannelList[] = {

		IED_COUNTER, IED_INTERPOLATED,
		IED_AF3, IED_F7, IED_F3, IED_FC5, IED_T7,
		IED_P7, IED_O1, IED_O2, IED_P8, IED_T8,
		IED_FC6, IED_F4, IED_F8, IED_AF4, IED_RAW_CQ,
		IED_GYROX, IED_GYROY, IED_MARKER, IED_TIMESTAMP
	};


	const char *channels[] = {
		"COUNTER",
		"AF3", "F7", "F3", "FC5", "T7",
		"P7", "O1", "O2", "P8", "T8",
		"FC6", "F4", "F8", "AF4",
		"GYROX", "GYROY", "TIMESTAMP",
		"FUNC_ID", "FUNC_VALUE", "MARKER", "SYNC_SIGNAL"
	};

    EmoEngineEventHandle eEvent         = IEE_EmoEngineEventCreate();
    EmoStateHandle eState               = IEE_EmoStateCreate();
    unsigned int datarate               = 0;
    unsigned int userId                 = 0;
    bool readytocollect                 = false;
    int state                           = 0;	
    unsigned int nSamplesTaken          = 0;
    unsigned int channelCount           = sizeof(epochChannelList)/
                                          sizeof(IEE_DataChannel_t);
	IEE_Event_t eventType;
	//double lsl_buffer[24];
 
	std::vector< std::vector<double> > lsl_buffer;

	// connect to the headset

	//create data streaminfo
	lsl::stream_info info("Emotiv", "EEG",
		channelCount, sps, lsl::cf_double64, "fooid");
	// append some meta-data
	info.desc().append_child_value("manufacturer", "Emotiv");
	lsl::xml_element channels_xml = info.desc().append_child("channels");
    for (int i=0; i<channelCount; i++)
        {
            channels_xml.append_child("channel")
                .append_child_value("label", channels[i])
                .append_child_value("unit", "uV")
                .append_child_value("type", "EEG");  // TODO: Check units
        }

	// create lsl outlet
	lsl::stream_outlet outlet(info);

	if (IEE_EngineConnect() != EDK_OK)
        {
            throw std::runtime_error("Emotiv Engine start up failed.");
        }

    DataHandle hData = IEE_DataCreate();
    IEE_DataSetBufferSizeInSec(per);

	double last_time_lsl = lsl::local_clock();
	double last_time_emo = 0;
	double dur_lsl;
	double dur_emo;
	double now;
	double offset;
	double offset_timestamp;
	bool is_first_time = true;
	try {
		
		while(!stop_) {
			state=IEE_EngineGetNextEvent(eEvent);
			std::cout << "State: " << state << std::endl;
			if(state == EDK_OK) {

				eventType = IEE_EmoEngineEventGetType(eEvent);
				if(eventType==IEE_UserAdded) {
					IEE_EmoEngineEventGetUserId(eEvent, &userId);
					IEE_EmoEngineEventGetEmoState(eEvent, eState);
                    IEE_DataAcquisitionEnable(userId, true);
					readytocollect = true;
					status = QString("connected to : %1").arg(userId);
					emit sendMessage(status);
				}
				if (readytocollect && (eventType == IEE_EmoStateUpdated)) {
					status = QString("receiving from : %1").arg(userId);
					emit sendMessage(status);

					IEE_DataUpdateHandle(0, hData);
					IEE_DataGetNumberOfSample(hData, &nSamplesTaken);
					std::cout << "Updated " << nSamplesTaken << std::endl;

					// Create the emotiv data buffer
					double ** emo_buffer = new double*[channelCount];
					for (int i=0; i<channelCount; i++)
					{
						emo_buffer[i] = new double[nSamplesTaken];
					}

					// Pull data from Emotiv into emo_buffer
					IEE_DataGetMultiChannels(hData, epochChannelList,
						channelCount, emo_buffer, nSamplesTaken);
					now = lsl::local_clock();

					// shove it into lsl buff with correct? timestamps
					if (is_first_time) {// first get the initial record time offset
						offset = lsl::local_clock();
						is_first_time = false;
					}

					lsl_buffer.resize(nSamplesTaken);
					for (int i=0; i<nSamplesTaken; i++){
						// fill the lsl buffer for each sample
						for (int j=0; j<channelCount; j++) 
							lsl_buffer[i].push_back(emo_buffer[j][i]);			

					}
				
					outlet.push_chunk(lsl_buffer, now, true);
					for (std::vector<std::vector<double>>::iterator it=lsl_buffer.begin(); it!=lsl_buffer.end(); ++it) 
						it->clear();
					lsl_buffer.clear();

					dur_lsl       = lsl::local_clock()-last_time_lsl;
					last_time_lsl = dur_lsl+last_time_lsl;
					dur_emo       = emo_buffer[20][0]-last_time_emo;
					last_time_emo = dur_emo+last_time_emo;
					std::cout << "dur_lsl : " << dur_lsl << std::endl;
					std::cout << "dur_emo : " << dur_emo << std::endl;


					for (int i=0; i<channelCount; i++)
						delete emo_buffer[i];
					delete emo_buffer;
				}
			}
				
			else {
				status = QString("connection to : %1 broken").arg(userId);
				emit sendMessage(status);
			}
			Sleep(1000);
				
			
		}
			
		
		
    }
	catch(boost::thread_interrupted &) {
		// thread was interrupted: no error
	}
	catch(std::exception &e) {
		// any other error
		//QMessageBox::critical(this,"Error",(std::string("Error during processing: ")+=e.what()).c_str(),QMessageBox::Ok);
		status = QString("error during processing: %1").arg(e.what());
		emit sendMessage(status);
	}

end:
	ui->linkButton->setText("Link");
	return;
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

void MainWindow::statusMessage(QString msg) {
	ui->connectionStatus->setText(msg);
}

void MainWindow::closeEvent(QCloseEvent *ev) {
	if (listen_thread_)
	  ev->setAccepted(false);//ignore();
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

		//// initialize the list of cap designs
		//std::string samplingrates = pt.get<std::string>("samplingrates","[128, 256]");
		//std::vector<std::string> rates;
		//boost::algorithm::split( rates, samplingrates.substr(1,knownschemes.size()-2),boost::algorithm::is_any_of(","),boost::algorithm::token_compress_on);
		//ui->cb_sps->clear();
		//for (int k=0;k<schemes.size();k++) {
		//	boost::algorithm::trim_if(rates[k],boost::algorithm::is_any_of(" '\""));
		//	ui->cb_sps->addItem(rates[k].c_str());
			
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
		//pt.put("settings.serverip",ui->serverIP->text().toStdString());
		//pt.put("settings.port",ui->port->text().toStdString());
		//pt.put("settings.fps",ui->fps->text().toStdString());
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


// start/stop the emotiv connection
void MainWindow::link() {

	int sps;
	if (listen_thread_) {
		// === perform unlink action ===
		try {
			setCursor(Qt::WaitCursor);
			stop_ = true;
			listen_thread_->join();
			listen_thread_->interrupt();
			listen_thread_.reset();
			setCursor(Qt::ArrowCursor);
		} catch(std::exception &e) {
			QMessageBox::critical(this,"Error",(std::string("Could not stop the background processing: ")+=e.what()).c_str(),QMessageBox::Ok);
			return;
		}
		// indicate that we are now successfully unlinked
		emit sendMessage("not connected");
		("not connected");
		ui->linkButton->setText("Link");
	} else {
		// === perform link action ===
		try {
			// get the UI parameters...
			sps = ui->cb_sps->currentText().toInt();	
			// start reading
			stop_ = false;
			listen_thread_.reset(new boost::thread(&MainWindow::listen,this, sps));
			// done, all successful
			ui->connectionStatus->setText(QString("Connecting..."));
			ui->linkButton->setText("Unlink");
		
		}
		catch(std::exception &e) {
			QMessageBox::critical(this,"Error",(std::string("Could not interface with Pupil Pro ")+=e.what()).c_str(),QMessageBox::Ok);
			return;
		}


	}
}

MainWindow::MainWindow(QWidget *parent, const std::string &config_file) : QMainWindow(parent), ui(new Ui::MainWindow) {

#if defined(Q_OS_MAC)
        // Doesn't default to its app directory.  Goes to root for some reason with Mavericks
        QDir::setCurrent(qApp->applicationDirPath());
        qDebug() << "Working Directory:" << QDir::currentPath();
#endif


	ui->setupUi(this);

	// parse startup config file
	load_config(config_file);
	ui->connectionStatus->setText("not connected");

	// make GUI connections
	QObject::connect(ui->actionQuit, SIGNAL(triggered()), this, SLOT(close()));
	QObject::connect(ui->linkButton, SIGNAL(clicked()), this, SLOT(link()));
	QObject::connect(this, SIGNAL(sendMessage(QString)), this, SLOT(statusMessage(QString)));
	QObject::connect(ui->actionLoad_Configuration, SIGNAL(triggered()), this, SLOT(load_config_dialog()));
	QObject::connect(ui->actionSave_Configuration, SIGNAL(triggered()), this, SLOT(save_config_dialog()));
	

}
MainWindow::~MainWindow() {
	//listen_thread_->interrupt();
	if(listen_thread_)
	{
				
		listen_thread_->interrupt();
		listen_thread_->join();
	}

	delete ui;
}
