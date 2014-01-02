#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/foreach.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include "eyecursor.h"
const int num_channels = 13;					// hard-coded # of channels
double x_pos = 0.0;
double y_pos = 0.0;
boost::shared_ptr<lsl::stream_outlet> outlet_;  // current stream outlet if linked (should be a member of MainWindow, but isn't due to the CBF)
boost::mutex mtxx_;

MainWindow::MainWindow(QWidget *parent, const std::string &config_file) :
QMainWindow(parent), ui(new Ui::MainWindow) {
	display_thread_ = NULL;
	if(!al_init()) {
      fprintf(stderr, "failed to initialize allegro!\n");
   }

   if(!al_init_primitives_addon()) {
      fprintf(stderr, "failed to initialize allegro primitives addon!\n");
   }
	// create the console
/*	if(AllocConsole()) {
		freopen("CONOUT$", "w", stdout);
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_RED);
	}
	std::ios::sync_with_stdio();
	*/
	ui->setupUi(this);

	// set logger 
	iV_SetLogger(1, "iViewX-logfile.txt");

	// parse startup config file
	load_config(config_file);

	// make GUI connections
	QObject::connect(ui->actionQuit, SIGNAL(triggered()), this, SLOT(close()));
	QObject::connect(ui->linkButton, SIGNAL(clicked()), this, SLOT(link_iviewx()));
	QObject::connect(ui->openDisplayButton, SIGNAL(clicked()), this, SLOT(open_display_action()));
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
	if(display_thread_) open_display_action();
	if(outlet_)	link_iviewx();
	al_shutdown_primitives_addon();
	al_uninstall_system();
	

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
		ui->serverAddress->setText(pt.get<std::string>("server.address","127.0.0.1").c_str());
		ui->sendPort->setValue(pt.get<int>("local.sendport",4444));
		ui->listenAddress->setText(pt.get<std::string>("local.address","127.0.0.1").c_str());
		ui->receivePort->setValue(pt.get<int>("local.receiveport",5555));
		ui->displayLeftEdit->setValue(pt.get<int>("display.left",0));
		ui->displayTopEdit->setValue(pt.get<int>("display.top",0));
		ui->displayWidthEdit->setValue(pt.get<int>("display.width",1600));
		ui->displayHeightEdit->setValue(pt.get<int>("display.height",900));
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
		pt.put("server.address",ui->serverAddress->text().toStdString());
		pt.put("local.sendport",ui->sendPort->value());
		pt.put("local.address",ui->listenAddress->text().toStdString());
		pt.put("local.receiveport",ui->receivePort->value());
		pt.put("display.left",ui->displayLeftEdit->value());
		pt.put("display.top",ui->displayTopEdit->value());
		pt.put("display.width",ui->displayWidthEdit->value());
		pt.put("display.height",ui->displayHeightEdit->value());
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


void MainWindow::open_display_action() {

	if(!display_thread_) {
		int displayLeft = ui->displayLeftEdit->value();
		int displayTop = ui->displayTopEdit->value();
		int displayWidth = ui->displayWidthEdit->value();
		int displayHeight = ui->displayHeightEdit->value();
	
		open_display(displayLeft, displayTop, displayWidth, displayHeight);
		display_thread_ = new boost::thread(&MainWindow::display_thread,this);
		ui->openDisplayButton->setText("Close Display");
	} else {
		display_thread_->interrupt();
		display_thread_->join();
		display_thread_ = NULL;
		destroy_display();
		ui->openDisplayButton->setText("Open Display");
	}
}


// start/stop the iViewX connection
void MainWindow::link_iviewx() {
	if (outlet_) {
		// === perform unlink action ===
		try {
			iV_SetSampleCallback(NULL);
			iV_SetTrackingMonitorCallback(NULL);
			iV_Disconnect();
			outlet_.reset();
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
			std::string serverAddress = ui->serverAddress->text().toStdString();
			int sendPort = ui->sendPort->value();
			std::string listenAddress = ui->listenAddress->text().toStdString();
			int receivePort = ui->receivePort->value();


			// connect to iViewX 
			int ret_connect = iV_Connect((char*)serverAddress.c_str(), sendPort, (char*)listenAddress.c_str(), receivePort);

			switch(ret_connect) {
			case RET_SUCCESS:
				break; // all fine
			case ERR_COULD_NOT_CONNECT:
				throw std::runtime_error("Connection could not be established");
			case ERR_WRONG_PARAMETER:
				throw std::runtime_error("Wrong Parameter used");
			default:
				throw std::runtime_error("Some unknown error occurred");
			}

			// read out tracker meta-data
			SystemInfoStruct systemInfoData; 
			iV_GetSystemInfo(&systemInfoData);
			char *ETDeviceStrings[] = {"NONE", "RED", "REDm", "HiSpeed", "MRI", "HED", "ETG", "Custom", "unknown", "unknown", "unknown", "unknown", "unknown", "unknown", "unknown", "unknown"};
			bool is_RED = std::string(ETDeviceStrings[systemInfoData.iV_ETDevice]).find("RED") != std::string::npos;

			// create streaminfo
			lsl::stream_info info("iViewX","Gaze",num_channels,systemInfoData.samplerate,lsl::cf_float32,"iViewX_" + serverAddress + boost::lexical_cast<std::string>(sendPort) + "_s6786sdf45");

			// append some (optional) meta-data
			lsl::xml_element channels = info.desc().append_child("channels");
			channels.append_child("channel")
				.append_child_value("label","Screen_X_left")
				.append_child_value("eye","left")
				.append_child_value("type","ScreenX")
				.append_child_value("unit","pixels");
			channels.append_child("channel")
				.append_child_value("label","Screen_Y_left")
				.append_child_value("eye","left")
				.append_child_value("type","ScreenY")
				.append_child_value("unit","pixels");
			channels.append_child("channel")
				.append_child_value("label","Screen_X_right")
				.append_child_value("eye","right")
				.append_child_value("type","ScreenX")
				.append_child_value("unit","pixels");
			channels.append_child("channel")
				.append_child_value("label","Screen_Y_right")
				.append_child_value("eye","right")
				.append_child_value("type","ScreenY")
				.append_child_value("unit","pixels");

			channels.append_child("channel")
				.append_child_value("label","Diameter_left")
				.append_child_value("eye","left")
				.append_child_value("type","Diameter")
				.append_child_value("unit",is_RED?"millimeters":"pixels");
			channels.append_child("channel")
				.append_child_value("label","Diameter_right")
				.append_child_value("eye","right")
				.append_child_value("type","Diameter")
				.append_child_value("unit",is_RED?"millimeters":"pixels");

			channels.append_child("channel")
				.append_child_value("label","EyePosition_X_left")
				.append_child_value("eye","left")
				.append_child_value("type","PositionX")
				.append_child_value("unit","millimeters");
			channels.append_child("channel")
				.append_child_value("label","EyePosition_Y_left")
				.append_child_value("eye","left")
				.append_child_value("type","PositionY")
				.append_child_value("unit","millimeters");
			channels.append_child("channel")
				.append_child_value("label","EyePosition_Z_left")
				.append_child_value("eye","left")
				.append_child_value("type","PositionZ")
				.append_child_value("unit","millimeters");
			channels.append_child("channel")
				.append_child_value("label","EyePosition_X_right")
				.append_child_value("eye","right")
				.append_child_value("type","PositionX")
				.append_child_value("unit","millimeters");
			channels.append_child("channel")
				.append_child_value("label","EyePosition_Y_right")
				.append_child_value("eye","right")
				.append_child_value("type","PositionY")
				.append_child_value("unit","millimeters");
			channels.append_child("channel")
				.append_child_value("label","EyePosition_Z_right")
				.append_child_value("eye","right")
				.append_child_value("type","PositionZ")
				.append_child_value("unit","millimeters");
			channels.append_child("channel")
				.append_child_value("label","PlaneNumber")
				.append_child_value("type","Index");

			info.desc().append_child("acquisition")
				.append_child_value("manufacturer","SMI")
				.append_child_value("model",ETDeviceStrings[systemInfoData.iV_ETDevice])
				.append_child_value("tracker_version",(boost::lexical_cast<std::string>(systemInfoData.iV_MajorVersion) + "." + boost::lexical_cast<std::string>(systemInfoData.iV_MinorVersion) + "." + boost::lexical_cast<std::string>(systemInfoData.iV_Buildnumber)).c_str())
				.append_child_value("API_version",(boost::lexical_cast<std::string>(systemInfoData.API_MajorVersion) + "." + boost::lexical_cast<std::string>(systemInfoData.API_MinorVersion) + "." + boost::lexical_cast<std::string>(systemInfoData.API_Buildnumber)).c_str());

			// instantiate new outlet
			outlet_.reset(new lsl::stream_outlet(info));

			// setup callbacks
			iV_SetSampleCallback(on_sample);

		}
		catch(std::exception &e) {
			QMessageBox::critical(this,"Error",(std::string("Could not initialize the iViewX interface: ")+=e.what()).c_str(),QMessageBox::Ok);
			return;
		}

		// done, all successful
		ui->linkButton->setText("Unlink");
	}
}

int __stdcall on_sample(SampleStruct s) {
	if (!outlet_)
		return 0;

	float sample[num_channels] = {
		s.leftEye.gazeX,s.leftEye.gazeY,
		s.rightEye.gazeX,s.rightEye.gazeY,
		s.leftEye.diam,s.rightEye.diam,
		s.leftEye.eyePositionX,s.leftEye.eyePositionY,s.leftEye.eyePositionZ,
		s.rightEye.eyePositionX,s.rightEye.eyePositionY,s.rightEye.eyePositionZ,
		s.planeNumber};

	// calc sample age
	long long tracker_time = s.timestamp;
	iV_GetCurrentTimestamp(&tracker_time);
	double age = (tracker_time - s.timestamp)/1000000.0;
	
	// push into LSL
	outlet_->push_sample(sample,lsl::local_clock()-age);
	mtxx_.lock();
	x_pos = (s.leftEye.gazeX+s.rightEye.gazeX)/2.0;
	y_pos = (s.leftEye.gazeY+s.rightEye.gazeY)/2.0;
	mtxx_.unlock();
	return 1;
}

void MainWindow::display_thread() {
	while(true) {
		mtxx_.lock();
		double xx = x_pos;
		double yy = y_pos;
		mtxx_.unlock();
		try{
			update_display(xx, yy);
			boost::this_thread::sleep(boost::posix_time::milliseconds(5));
		} catch(boost::thread_interrupted&) {
			return;
		}
		
	}
	//destroy_display();
}

MainWindow::~MainWindow() {
	delete ui;
}
