#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/foreach.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>


const int impedance_cycle_length = 4;				// cycle length for the impedance signal
const int samples_per_chunk = 32;					// the chunk granularity at which we transmit data into LSL
const double value_scale = 1000000.0/4294967296.0;	// rescaling from 32-bit integers to microvolts

MainWindow::MainWindow(QWidget *parent, const std::string &config_file) :
QMainWindow(parent),
ui(new Ui::MainWindow)
{
	ui->setupUi(this);

	// parse startup config file
	load_config(config_file);

	// make GUI connections
	QObject::connect(ui->actionQuit, SIGNAL(triggered()), this, SLOT(close()));
	QObject::connect(ui->linkButton, SIGNAL(clicked()), this, SLOT(link_cognionics()));
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
		ui->comPort->setValue(pt.get<int>("settings.comport",1));
		ui->samplingRate->setValue(pt.get<int>("settings.samplingrate",250));
		ui->channelCount->setValue(pt.get<int>("settings.channelcount",24));
		ui->stripImpedance->setChecked(pt.get<bool>("settings.stripimpedance",false));
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
		pt.put("settings.comport",ui->comPort->value());
		pt.put("settings.samplingrate",ui->samplingRate->value());
		pt.put("settings.channelcount",ui->channelCount->value());
		pt.put("settings.stripimpedance",ui->stripImpedance->checkState() == Qt::Checked);
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


// start/stop the cognionics connection
void MainWindow::link_cognionics() {
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
		HANDLE hPort = NULL;

		try {
			// get the UI parameters...
			int comPort = ui->comPort->value();
			int samplingRate = ui->samplingRate->value();
			int channelCount = ui->channelCount->value();
			bool stripImpedance = ui->stripImpedance->checkState() == Qt::Checked;
			std::vector<std::string> channelLabels;
			boost::algorithm::split(channelLabels,ui->channelLabels->toPlainText().toStdString(),boost::algorithm::is_any_of("\n"));
			if (channelLabels.size() != channelCount)
				throw std::runtime_error("The number of channels labels does not match the channel count device setting.");

			// try to open the serial port
			std::string fname = "\\\\.\\COM" + boost::lexical_cast<std::string>(comPort);
			hPort = CreateFileA(fname.c_str(),GENERIC_READ|GENERIC_WRITE,0,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0);
			if (hPort == INVALID_HANDLE_VALUE)
				throw std::runtime_error("Could not open serial port. Please make sure that the device is paired (pairing code 0000) and that you have the right serial port number.");

			//setup serial port parameters
			DCB dcbSerialParams = {0};
			if (!GetCommState(hPort, &dcbSerialParams))
				QMessageBox::critical(this,"Error","Could not get COM port state.",QMessageBox::Ok);
			dcbSerialParams.BaudRate=1500000;
			dcbSerialParams.ByteSize=8;
			dcbSerialParams.StopBits=ONESTOPBIT;
			dcbSerialParams.Parity=NOPARITY;

			if(!SetCommState(hPort, &dcbSerialParams))
				QMessageBox::critical(this,"Error","Could not set baud rate.",QMessageBox::Ok);

			// try to set timeouts
			COMMTIMEOUTS timeouts = {0};
			timeouts.ReadIntervalTimeout = 500;
			timeouts.ReadTotalTimeoutConstant = 50;
			timeouts.ReadTotalTimeoutMultiplier = 10;
			timeouts.WriteTotalTimeoutConstant = 50;
			timeouts.WriteTotalTimeoutMultiplier = 10;

			if (!SetCommTimeouts(hPort,&timeouts))
				QMessageBox::critical(this,"Error","Could not set COM port timeouts.",QMessageBox::Ok);

			// start reading
			stop_ = false;
			reader_thread_.reset(new boost::thread(&MainWindow::read_thread,this,hPort,comPort,samplingRate,channelCount,channelLabels,stripImpedance));
		}
		catch(std::exception &e) {
			if (hPort != INVALID_HANDLE_VALUE)
				CloseHandle(hPort);
			QMessageBox::critical(this,"Error",(std::string("Could not initialize the Cognionics interface: ")+=e.what()).c_str(),QMessageBox::Ok);
			return;
		}

		// done, all successful
		ui->linkButton->setText("Unlink");
	}
}


// background data reader thread
void MainWindow::read_thread(HANDLE hPort, int comPort, int samplingRate, int channelCount, std::vector<std::string> channelLabels, bool stripImpedance) {
	try {

		// create streaminfo
		lsl::stream_info info("Cognionics","EEG",channelCount,samplingRate,lsl::cf_float32,"Cognionics_C" + boost::lexical_cast<std::string>(comPort));
		// append some meta-data
		lsl::xml_element channels = info.desc().append_child("channels");
		for (int k=0;k<channelLabels.size();k++) {
			channels.append_child("channel")
				.append_child_value("label",channelLabels[k].c_str())
				.append_child_value("type","EEG")
				.append_child_value("unit","microvolts");
		}
		info.desc().append_child("acquisition")
			.append_child_value("manufacturer","Cognionics");

		// make a new outlet
		lsl::stream_outlet outlet(info,samples_per_chunk);

		// reserve memory
		std::vector<float> sample(channelCount);

		// send impedance command
		DWORD dwBytesWritten = 0;
		unsigned char cmd_stripimp = 0x12;
		unsigned char cmd_keepimp = 0x11;
		if (stripImpedance)
			WriteFile(hPort,(LPSTR)&cmd_stripimp,1,&dwBytesWritten,NULL);
		else
			WriteFile(hPort,(LPSTR)&cmd_keepimp,1,&dwBytesWritten,NULL);


		unsigned char temp, cur_counter;
		int msb, lsb2, lsb1;
		unsigned long bytes_read;
		int last_counter=0;
		unsigned long sample_index=0;
		int last_skip_index = 0;
		int skipped_total=0;
		std::vector<std::vector<float> > impedance_buffer(impedance_cycle_length,std::vector<float>(channelCount));

		// scan for the sync byte to find the beginning of the next sample
		temp = 0;
		while (temp != 0xFF)
			ReadFile(hPort,&temp,1,&bytes_read,NULL);

		// enter transmission loop
		while (!stop_) {

			// receive sample counter value
			ReadFile(hPort,&cur_counter,1,&bytes_read,NULL);
			
			// determine how many samples were skipped (missed due to packet loss)
			int samples_skipped = cur_counter-last_counter-1;
			if (samples_skipped < 0)
				samples_skipped += 128;
			if (samples_skipped > 40 && sample_index>1500+last_skip_index)
				last_skip_index = sample_index; // debug breakpoint 
			skipped_total += samples_skipped;
			last_counter = cur_counter;

			// insert dummy samples for every skipped sample
			for (int k=0;k<samples_skipped;k++)
				outlet.push_sample(impedance_buffer[sample_index++ % impedance_cycle_length]);

			// receive next sample
			for(int c=0; c < channelCount; c++) {
				ReadFile(hPort,&temp,1,&bytes_read,NULL);
				msb = temp;
				ReadFile(hPort,&temp,1,&bytes_read,NULL);
				lsb2 = temp;
				ReadFile(hPort,&temp,1,&bytes_read,NULL);
				lsb1 = temp;
				sample[c] = (double)((msb<<24) | (lsb2<<17) | (lsb1<<10)) * value_scale;
			}

			// confirm that the sample is complete (i.e., sync byte is next)
			ReadFile(hPort,&temp,1,&bytes_read,NULL);
			if (temp == stripImpedance?0x11:0x10) {
				// sample insert into impedance buffer and push into the outlet
				impedance_buffer[sample_index++ % impedance_cycle_length] = sample;
				outlet.push_sample(sample);
			} else {
				// the sample was lost, replace it by the contents of the impedance buffer
				outlet.push_sample(impedance_buffer[sample_index++ % impedance_cycle_length]);
			}
			// ... and scan forward until we got the next sync byte
			temp = 0;
			while (temp != 0xFF)
				ReadFile(hPort,&temp,1,&bytes_read,NULL);
		}
	}
	catch(boost::thread_interrupted &e) {
		// thread was interrupted: no error
	}
	catch(std::exception &e) {
		// any other error
		QMessageBox::critical(this,"Error",(std::string("Error during processing: ")+=e.what()).c_str(),QMessageBox::Ok);
	}
	CloseHandle(hPort);
}

MainWindow::~MainWindow() {
	delete ui;
}
