#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/foreach.hpp>
#include <boost/functional/hash.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <sstream>
#include <pugixml/pugixml.hpp>

MainWindow::MainWindow(QWidget *parent, const std::string &config_file) :
QMainWindow(parent),
ui(new Ui::MainWindow)
{
	ui->setupUi(this);

	// parse startup config file
	load_config(config_file);

	// make GUI connections
	QObject::connect(ui->actionQuit, SIGNAL(triggered()), this, SLOT(close()));
	QObject::connect(ui->linkButton, SIGNAL(clicked()), this, SLOT(on_link()));
	QObject::connect(ui->browseButton, SIGNAL(clicked()), this, SLOT(pick_file_dialog()));
	QObject::connect(ui->actionLoad_Configuration, SIGNAL(triggered()), this, SLOT(load_config_dialog()));
	QObject::connect(ui->actionSave_Configuration, SIGNAL(triggered()), this, SLOT(save_config_dialog()));
}

// file browse action
void MainWindow::pick_file_dialog() {
	QString sel = QFileDialog::getOpenFileName(this,"Open Source File","","Any File (*.*)");
	if (!sel.isEmpty())
		ui->fileName->setText(sel);
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
	if (outlet)
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
		ui->fileName->setText(pt.get<std::string>("source.filename","").c_str());
		ui->isXML->setChecked(pt.get<bool>("source.isxml",false));
		ui->streamName->setText(pt.get<std::string>("stream.name","Static Info").c_str());
		ui->streamType->setText(pt.get<std::string>("stream.type","Metadata").c_str());
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
		pt.put("source.filename",ui->fileName->text().toStdString());
		pt.put("source.isxml",ui->isXML->checkState() == Qt::Checked);
		pt.put("stream.name",ui->streamName->text().toStdString());
		pt.put("stream.type",ui->streamType->text().toStdString());
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

// start/stop the Tobii connection
void MainWindow::on_link() {
	if (outlet) {
		// === perform unlink action ===
		try {
			outlet.reset();
		} catch(std::exception &e) {
			QMessageBox::critical(this,"Error",(std::string("Could not unlink the outlet: ")+=e.what()).c_str(),QMessageBox::Ok);
			return;
		}

		// indicate that we are now successfully unlinked
		ui->linkButton->setText("Link");
	} else {
		// === perform link action ===
		try {
			// get the UI parameters...
			std::string fileName = ui->fileName->text().toStdString();
			bool isXML = ui->isXML->checkState()==Qt::Checked;
			std::string streamName = ui->streamName->text().toStdString();
			std::string streamType = ui->streamType->text().toStdString();

			// try to open the file
			std::ifstream f(fileName.c_str());
			if (f.fail())
				throw std::exception((std::string("Could not open file: ") + fileName).c_str());
			std::stringstream content; content << f.rdbuf();
			std::string contentStr(content.str());

			// create the streaminfo
			boost::hash<std::string> stringHash;
			std::size_t h = stringHash(contentStr) + stringHash(streamName) + stringHash(streamType);
			lsl::stream_info info(streamName,streamType,0,0,lsl::cf_float32,"StaticInfo_" + boost::lexical_cast<std::string>(h + isXML));

			if (isXML) {
				// append file content to the desc field
				pugi::xml_document doc;
				pugi::xml_parse_result res = doc.load_file(fileName.c_str());
				if (!res)
					throw std::exception((std::string("could not parse XML document: ") + res.description()).c_str());
				info.desc().append_copy(doc.document_element());
			} else {
				// set content as plain text
				info.desc().append_child_value("content",contentStr.c_str());
			}

			// make a new outlet
			outlet.reset(new lsl::stream_outlet(info));
		}
		catch(std::exception &e) {
			QMessageBox::critical(this,"Error",(std::string("Could not initialize the outlet: ")+=e.what()).c_str(),QMessageBox::Ok);
			return;
		}

		// done, all successful
		ui->linkButton->setText("Unlink");
	}
}

MainWindow::~MainWindow() {
	delete ui;
}

