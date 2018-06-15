#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/foreach.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

const int num_channels = 18; // plus 2 confidence channels

MainWindow::MainWindow(QWidget *parent, const std::string &config_file) :
QMainWindow(parent),
ui(new Ui::MainWindow)
{
	ui->setupUi(this);

	// parse startup config file
	load_config(config_file);

	// make GUI connections
	QObject::connect(ui->actionQuit, SIGNAL(triggered()), this, SLOT(close()));
	QObject::connect(ui->linkButton, SIGNAL(clicked()), this, SLOT(link_iviewx()));
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

	// parse file
	try {
		read_xml(filename, pt);
	} catch(std::exception &e) {
		QMessageBox::information(this,"Error",(std::string("Cannot read config file: ")+= e.what()).c_str(),QMessageBox::Ok);
		return;
	}

	// get config values
	try {
		ui->serverAddress->setText(pt.get<std::string>("server.address","").c_str());
		ui->serverPort->setValue(pt.get<int>("server.port",3333));
		ui->listenAddress->setText(pt.get<std::string>("listen.address","").c_str());
		ui->listenPort->setValue(pt.get<int>("listen.port",2222));
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
		pt.put("server.port",ui->serverPort->value());
		pt.put("listen.address",ui->listenAddress->text().toStdString());
		pt.put("listen.port",ui->listenPort->value());
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

// start/stop the iViewX connection
void MainWindow::link_iviewx() {
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
		try {
			// get the UI parameters...
			int serverPort = ui->serverPort->value();
			std::string serverAddress = ui->serverAddress->text().toStdString();
			int listenPort = ui->listenPort->value();
			std::string listenAddress = ui->listenAddress->text().toStdString();

			// try to resolve the iViewX server
			udp::resolver resolver(io_service_);
			udp::resolver::query query(udp::v4(), serverAddress, boost::lexical_cast<std::string>(serverPort));
			udp::endpoint server_endpoint = *resolver.resolve(query);

			// open UDP socket
			udp_socket_p socket(new udp::socket(io_service_));
			socket->open(udp::v4());
			// bind it to a local port & interface
			if (listenAddress.empty() || listenAddress == "0.0.0.0")
				socket->bind(boost::asio::ip::udp::endpoint(boost::asio::ip::udp::v4(), listenPort));
			else
				socket->bind(boost::asio::ip::udp::endpoint(boost::asio::ip::address::from_string(listenAddress), listenPort));

			// negotiate format
			socket->send_to(boost::asio::buffer("ET_STR \n"), server_endpoint);
			socket->send_to(boost::asio::buffer("ET_FRM \"%ET %SX %SY %DX %DY %PX %PY %CX %CY\"\n"), server_endpoint);

			udp::endpoint remote_endpoint;
			boost::array<char,4096> recv_buf;
			boost::system::error_code ec;
			if (!socket->receive_from(boost::asio::buffer(recv_buf),remote_endpoint,0,ec))
				throw std::runtime_error("Did not get a response from the iViewX server.");
			socket->send_to(boost::asio::buffer("ET_REC \n"), server_endpoint);

			// start reading
			stop_ = false;
			reader_thread_.reset(new boost::thread(&MainWindow::read_thread,this,socket,serverAddress));
		}
		catch(std::exception &e) {
			QMessageBox::critical(this,"Error",(std::string("Could not initialize the iViewX interface: ")+=e.what()).c_str(),QMessageBox::Ok);
			return;
		}

		// done, all successful
		ui->linkButton->setText("Unlink");
	}
}


// background data reader thread
void MainWindow::read_thread(udp_socket_p socket, std::string serverAddress) {
	// create streaminfo
	lsl::stream_info info("iViewX","Gaze",num_channels,0,lsl::cf_float32,"iViewX_" + serverAddress + "_s678678ff");
	// append some meta-data
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
		.append_child_value("label","Diameter_X_left")
		.append_child_value("eye","left")
		.append_child_value("type","DiameterX")
		.append_child_value("unit","pixels");
	channels.append_child("channel")
		.append_child_value("label","Diameter_Y_left")
		.append_child_value("eye","left")
		.append_child_value("type","DiameterY")
		.append_child_value("unit","pixels");
	channels.append_child("channel")
		.append_child_value("label","Diameter_X_right")
		.append_child_value("eye","right")
		.append_child_value("type","DiameterX")
		.append_child_value("unit","pixels");
	channels.append_child("channel")
		.append_child_value("label","Diameter_Y_right")
		.append_child_value("eye","right")
		.append_child_value("type","DiameterY")
		.append_child_value("unit","pixels");

	channels.append_child("channel")
		.append_child_value("label","Pupil_X_left")
		.append_child_value("eye","left")
		.append_child_value("type","PupilX")
		.append_child_value("unit","pixels");
	channels.append_child("channel")
		.append_child_value("label","Pupil_Y_left")
		.append_child_value("eye","left")
		.append_child_value("type","PupilY")
		.append_child_value("unit","pixels");
	channels.append_child("channel")
		.append_child_value("label","Pupil_X_right")
		.append_child_value("eye","right")
		.append_child_value("type","PupilX")
		.append_child_value("unit","pixels");
	channels.append_child("channel")
		.append_child_value("label","Pupil_Y_right")
		.append_child_value("eye","right")
		.append_child_value("type","PupilY")
		.append_child_value("unit","pixels");

	channels.append_child("channel")
		.append_child_value("label","Reflex_X_left")
		.append_child_value("eye","left")
		.append_child_value("type","ReflexX")
		.append_child_value("unit","pixels");
	channels.append_child("channel")
		.append_child_value("label","Reflex_Y_left")
		.append_child_value("eye","left")
		.append_child_value("type","ReflexY")
		.append_child_value("unit","pixels");
	channels.append_child("channel")
		.append_child_value("label","Reflex_X_right")
		.append_child_value("eye","right")
		.append_child_value("type","ReflexX")
		.append_child_value("unit","pixels");
	channels.append_child("channel")
		.append_child_value("label","Reflex_Y_right")
		.append_child_value("eye","right")
		.append_child_value("type","ReflexY")
		.append_child_value("unit","pixels");
	channels.append_child("channel")
		.append_child_value("label","Confidence_left")
		.append_child_value("eye","left")
		.append_child_value("type","Confidence")
		.append_child_value("unit","normalized");
	channels.append_child("channel")
		.append_child_value("label","Confidence_right")
		.append_child_value("eye","right")
		.append_child_value("type","Confidence")
		.append_child_value("unit","normalized");

	info.desc().append_child("acquisition")
		.append_child_value("manufacturer","SMI")
		.append_child_value("model","iViewX");

	// make a new outlet
	lsl::stream_outlet outlet(info);

	// enter transmission loop
	udp::endpoint remote_endpoint;
	boost::array<char,4096> recv_buf;
	try {
		while(!stop_) {

			// wait for a new packet
			size_t len = socket->receive_from(boost::asio::buffer(recv_buf), remote_endpoint);
			std::istringstream response(std::string(&recv_buf[0],len));
			std::string tmp; response >> tmp;

			// was it a sample?
			if (tmp == "ET_SPL") {
				double screen_xl=0,screen_yl=0, screen_xr=0,screen_yr=0;
				double diameter_xl=0,diameter_yl=0, diameter_xr=0,diameter_yr=0;
				double pupil_xl=0,pupil_yl=0, pupil_xr=0,pupil_yr=0;
				double reflex_xl=0,reflex_yl=0, reflex_xr=0,reflex_yr=0;

				// determine laterality
				response >> tmp;
				bool have_left = (tmp == "b" || tmp == "l");
				bool have_right = (tmp == "b" || tmp == "r");

				if (have_left)
					response >> screen_xl;
				if (have_right)
					response >> screen_xr;
				if (have_left)
					response >> screen_yl;
				if (have_right)
					response >> screen_yr;

				if (have_left)
					response >> diameter_xl;
				if (have_right)
					response >> diameter_xr;
				if (have_left)
					response >> diameter_yl;
				if (have_right)
					response >> diameter_yr;

				if (have_left)
					response >> pupil_xl;
				if (have_right)
					response >> pupil_xr;
				if (have_left)
					response >> pupil_yl;
				if (have_right)
					response >> pupil_yr;

				if (have_left)
					response >> reflex_xl;
				if (have_right)
					response >> reflex_xr;
				if (have_left)
					response >> reflex_yl;
				if (have_right)
					response >> reflex_yr;

				// construct sample
				float sample[num_channels] = {
					screen_xl,screen_yl,screen_xr,screen_yr,
					diameter_xl,diameter_yl, diameter_xr,diameter_yr,
					pupil_xl,pupil_yl, pupil_xr,pupil_yr,
					reflex_xl,reflex_yl, reflex_xr,reflex_yr,
					have_left?1:0, have_right?1:0};
					// push into LSL
					outlet.push_sample(sample);
			}
		}
	} 
	catch(boost::thread_interrupted &) { }
	catch(std::exception &e) {
		QMessageBox::critical(this,"Error",(std::string("Transmission error: ")+=e.what()).c_str(),QMessageBox::Ok);
	}
}

MainWindow::~MainWindow() {
	delete ui;
}
