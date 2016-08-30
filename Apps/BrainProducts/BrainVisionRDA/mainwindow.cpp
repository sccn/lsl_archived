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
#include <boost/asio.hpp>
#include "rda_client.h"

using boost::asio::ip::tcp;

const int RDA_Port = 51244;				// TCP port number of the RDA server 
const double sample_age = 0.02;			// assumed buffer lag
const int skip_blocks_after_reset = 15;	// number of data blocks to skip after an amplifier reset

MainWindow::MainWindow(QWidget *parent, const std::string &config_file) :
QMainWindow(parent),
ui(new Ui::MainWindow)
{
	ui->setupUi(this);

	// parse startup config file
	load_config(config_file);
	ui->connectionStatus->setText("not connected");

	// make GUI connections
	QObject::connect(ui->actionQuit, SIGNAL(triggered()), this, SLOT(close()));
	QObject::connect(ui->linkButton, SIGNAL(clicked()), this, SLOT(link_rda()));
	QObject::connect(this, SIGNAL(sendMessage(QString)), this, SLOT(statusMessage(QString)));
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

void MainWindow::statusMessage(QString msg) {
	ui->connectionStatus->setText(msg);
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
		ui->serverIP->setText(pt.get<std::string>("settings.serverip","127.0.0.1").c_str());
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
		pt.put("settings.serverip",ui->serverIP->text().toStdString());
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

// start/stop the RDA connection
void MainWindow::link_rda() {
	if (reader_thread_) {
		// === perform unlink action ===
		try {
			setCursor(Qt::WaitCursor);
			stop_ = true;
			reader_thread_->join();
			reader_thread_->interrupt();
			reader_thread_.reset();
			setCursor(Qt::ArrowCursor);
		} catch(std::exception &e) {
			QMessageBox::critical(this,"Error",(std::string("Could not stop the background processing: ")+=e.what()).c_str(),QMessageBox::Ok);
			return;
		}
		// indicate that we are now successfully unlinked
		emit sendMessage("not connected");
		ui->linkButton->setText("Link");
	} else {
		// === perform link action ===
		try {
			// get the UI parameters...
			QString serverIP = ui->serverIP->text();
			// start reading
			stop_ = false;
			reader_thread_.reset(new boost::thread(&MainWindow::read_thread,this,serverIP));
		}
		catch(std::exception &e) {
			QMessageBox::critical(this,"Error",(std::string("Could not initialize the BrainVision RDA interface: ")+=e.what()).c_str(),QMessageBox::Ok);
			return;
		}

		// done, all successful
		ui->linkButton->setText("Unlink");
	}
}


// background data reader thread
void MainWindow::read_thread(QString serverIP) {
	QString status; 
	int connectionPhase = 0;
	long blockCounter;
	RDA_MessageStart rdaInfo;
	try {
		boost::scoped_ptr<lsl::stream_outlet> lsl_outlet;
		boost::scoped_ptr<lsl::stream_info> lsl_info;
		lsl::xml_element channels;
		boost::scoped_ptr<lsl::stream_outlet> lsl_event_outlet;
		boost::scoped_ptr<lsl::stream_info> lsl_event_info;
		boost::shared_ptr<RDA_client> rda_client;
		boost::system::error_code ec;
		boost::asio::io_service io_service;
		tcp::socket socket_(io_service);
		tcp::resolver resolver(io_service);
		tcp::resolver::query query(serverIP.toStdString(),  boost::lexical_cast<std::string>(RDA_Port));
		tcp::endpoint ep;
		tcp::resolver::iterator it_resolver;
		status = QString("waiting for server at %1:%2 ...").arg(serverIP).arg(RDA_Port);
		emit sendMessage(status);
		std::string lsl_id = QString("RDA %1:%2").arg(serverIP).arg(RDA_Port).toStdString();
		while(!stop_) {
			switch(connectionPhase) {
				case 0:
					// prepare a new connection
					if(socket_.is_open())
						socket_.close();
					io_service.reset();
					connectionPhase = 1;
				case 1: 
					// wait for a connection to the RDA server
					it_resolver = boost::asio::connect(socket_, resolver.resolve(query), ec);
					if(!ec){
						ep = *it_resolver;
						std::string addr = ep.address().to_v4().to_string();
						status = QString("connected to %1:%2").arg(addr.c_str()).arg(RDA_Port);
						emit sendMessage(status);
						lsl_id = QString("RDA %1:%2").arg(addr.c_str()).arg(RDA_Port).toStdString();
						rda_client.reset(new RDA_client(io_service, socket_));
						connectionPhase = 2;
					}
					else
						boost::this_thread::sleep(boost::posix_time::milliseconds(20));
					break;
				case 2:
					// we are connected, or?
					if(!rda_client->is_connected())
					{
						status = QString("waiting for server at %1:%2 ...").arg(serverIP).arg(RDA_Port);
						emit sendMessage(status);
						rda_client.reset();
						connectionPhase = 0;
					}
					else
					{
						// get the recorder messages
						while(!stop_ && !rda_client->messages().empty())
						{
							RDA_message& msg = rda_client->messages().front();
							switch(msg.type())
							{
							case RDA_message::START:
								rdaInfo = msg.decode_startmessage();

								// create data streaminfo
								lsl_info.reset( new lsl::stream_info("BrainVision RDA", "EEG",
									(int)rdaInfo.numChannels, rdaInfo.sampleRate, lsl::cf_float32, lsl_id));
								// append some meta-data
								channels = lsl_info->desc().append_child("channels");
								for (unsigned int k = 0; k < rdaInfo.channelLabels.size(); k++) 
								{
									channels.append_child("channel")
										.append_child_value("label", rdaInfo.channelLabels[k].c_str())
										.append_child_value("type", "EEG")
										.append_child_value("unit", "microvolts");
								}
								lsl_info->desc().append_child("acquisition")
									.append_child_value("manufacturer","Brain Products");
								// make a new data outlet
								lsl_outlet.reset(new lsl::stream_outlet(*lsl_info.get()));

								// create event stream info
								lsl_event_info.reset( new lsl::stream_info("BrainVision RDA Markers", "Markers",
									1, lsl::IRREGULAR_RATE, lsl::cf_string, lsl_id + " Marker"));
								// make a new event outlet
								lsl_event_outlet.reset(new lsl::stream_outlet(*lsl_event_info.get(),1));

								blockCounter = -15;
								break;

							case RDA_message::DATA32:
								if(lsl_outlet)
								{
									std::vector<std::vector<float>> samples;
									std::vector<RDA_Marker> markers;
									msg.decode_data32(rdaInfo.channelResolutions, samples, markers);
									if(blockCounter < -1)
										// skip the first blocks after a NEWSTATE message
										blockCounter++;
									else 
									{
										double now = lsl::local_clock();
										// send samples
										lsl_outlet->push_chunk(samples,now);
										// send markers
										for(size_t mkr = 0; mkr < markers.size(); mkr++) {
											double tm = now + ((double)markers[mkr].samplePosition + 1 - samples.size()) / rdaInfo.sampleRate;
											lsl_event_outlet->push_sample(markers[mkr].description.empty() ? &markers[mkr].type : &markers[mkr].description, tm);
										}
									}
								}
								break;

							case RDA_message::STOP:
								// close stream outlets
								lsl_outlet.reset();
								lsl_event_outlet.reset();
								emit sendMessage(QString("acquisition stopped."));
								break;

							case RDA_message::NEWSTATE:
								// recorder state changed
								blockCounter = -skip_blocks_after_reset;
								break;
							}
							rda_client->messages().pop_front();
						}
						if(rda_client->messages().empty())
							boost::this_thread::sleep(boost::posix_time::milliseconds(1));
					}
					break;
			}
		} // leave connection / transmission loop
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
}

MainWindow::~MainWindow() {
	delete ui;
}
