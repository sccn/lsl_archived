
#include "ui_mainwindow.h"
#include "mainwindow.h"
#ifdef __WIN32
#include <Objbase.h>
#endif
#include <iostream>
#include <string>
#include <vector>
#include <boost/lexical_cast.hpp>
#include <boost/asio.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <boost/filesystem.hpp>
// #include <boost/algorithm/string.hpp>
#include <algorithm>

// recording class
#include "recording.h"

// global -- can't be in header file
recording *currentRecording;


MainWindow::MainWindow(QWidget *parent, const std::string &config_file) :
QMainWindow(parent),
ui(new Ui::MainWindow) {

	ui->setupUi(this);
	QObject::connect(ui->actionQuit, SIGNAL(triggered()), this, SLOT(close()));	
	QObject::connect(ui->actionLoadConfig, SIGNAL(triggered()), this, SLOT(browseConfigDialog()));			
	QObject::connect(ui->browseButton, SIGNAL(clicked()), this, SLOT(browseLocationDialog()));	
	QObject::connect(ui->blockList, SIGNAL(itemClicked(QListWidgetItem *)), this, SLOT(blockSelected(QListWidgetItem *)));
	QObject::connect(ui->refreshButton, SIGNAL(clicked()), this, SLOT(refreshStreams()));
	QObject::connect(ui->startButton, SIGNAL(clicked()), this, SLOT(startRecording()));
	QObject::connect(ui->stopButton, SIGNAL(clicked()), this, SLOT(stopRecording()));

	currentlyRecording = false;
	load_config(config_file);

	timer = new QTimer(this);
	connect(timer, SIGNAL(timeout()), this, SLOT(statusUpdate()));
	timer->start(1000);
	//startTime = (int)lsl::local_clock();

}

void MainWindow::statusUpdate(void) {

	int elapsed;
	std::div_t div_strct;
	int hours;
	int remainder;
	int minutes;
	int seconds;
	std::stringstream timeString;
	if(currentlyRecording ==true) {
		elapsed = ((int)lsl::local_clock() - startTime);
		div_strct = std::div(elapsed, 3600);
		hours = div_strct.quot;
		remainder = div_strct.rem;
		div_strct = std::div(remainder, 60);
		minutes = div_strct.quot;
		seconds = div_strct.rem;

		// there's probably a better way to do this...
		timeString << "Recording ("; 
		if(hours<10)timeString <<"0";
		timeString<<hours<<":";
		if(minutes<10)timeString<<"0";
		timeString<<minutes<<":";
		if(seconds<10)timeString<<"0";
		timeString<<seconds<<")";

		statusBar()->showMessage(QString::fromStdString(timeString.str()));

	}

}

void MainWindow::closeEvent(QCloseEvent *ev) {
	if (currentlyRecording)
		ev->ignore();
}

void MainWindow::browseConfigDialog() {
	QString sel = QFileDialog::getOpenFileName(this,"Load Configuration File","","Configuration Files (*.cfg)");
	if (!sel.isEmpty())
		load_config(sel.toStdString());
}

void MainWindow::browseLocationDialog() {
	QString sel = QFileDialog::getSaveFileName(this,"Save recordings as...", "untitled.xdf", "XDF recordings (*.xdf);;XDF compressed recordings (*.xdfz)");
	if (!sel.isEmpty())	
		ui->locationEdit->setText(sel);
}

void MainWindow::blockSelected(QListWidgetItem *item) {
	
	if(currentlyRecording==true)
		QMessageBox::information(this, "Still recording", "Please stop recording before switching blocks.", QMessageBox::Ok);
	else {
		currentBlock = item->text().toStdString();
		
		// scripted action code here...

	}
	//std::cout << item->text().toStdString() <<std::endl;
}

void MainWindow::load_config(const std::string &filename) {
	std::cout << "loading config file " << filename << std::endl;
	try {
		using boost::property_tree::ptree;
		ptree pt;
		read_ini(filename, pt);


		// ----------------------------
		// required streams
		// ----------------------------	
		std::string str_requiredStreams = pt.get<std::string>("RequiredStreams","");
		std::string rs_substr = str_requiredStreams.substr(1, str_requiredStreams.size()-2);
		boost::algorithm::split(requiredStreams,rs_substr,boost::algorithm::is_any_of(","),boost::algorithm::token_compress_on);
		for (int k=0;k<requiredStreams.size();k++)
			boost::algorithm::trim_if(requiredStreams[k],boost::algorithm::is_any_of(" '\""));
		

		// ----------------------------
		// recording location
		// ----------------------------
		std::vector<std::string>sessionBlocks;
		QListWidgetItem *item;
		std::string str_sessionBlocks = pt.get<std::string>("SessionBlocks","");
		std::string sb_substr = str_sessionBlocks.substr(1, str_sessionBlocks.size()-2);
		boost::algorithm::split(sessionBlocks,sb_substr,boost::algorithm::is_any_of(","),boost::algorithm::token_compress_on);
		
		for (int k=0;k<sessionBlocks.size();k++) {
			boost::algorithm::trim_if(sessionBlocks[k],boost::algorithm::is_any_of(" '\""));
			item=new QListWidgetItem(QString::fromStdString(sessionBlocks[k]), ui->blockList);
			ui->blockList->addItem(item);
			if(k==0) {
				item->setSelected(true);
				blockSelected(item);
			}
		}
		// get the path as a string
		std::string str_path = pt.get<std::string>("StorageLocation", "C:\\Recordings\\CurrentStudy\\exp%n\\untitled.xdf");
		ui->locationEdit->setText(str_path.c_str());


		// scan the path for %n and %b
		std::string str_n("\%n");
		std::string str_b("\%b");
		std::size_t pos_n;
		std::size_t pos_b;
		pos_n = str_path.find(str_n);
		pos_b = str_path.find(str_b);	
		
		// variables for string/path parsing
		std::string abs_path;	
		std::stringstream ss;
		boost::filesystem::path p; 

		int i=1;
		
		if(pos_n<str_path.size()) {
			abs_path.append(str_path.begin(),str_path.begin()+pos_n);
		
			// find the last value fo %n in the directories			
			for(i=1;i<10000;i++) {
				ss << i; // convert int to string stream
				p=(abs_path+ss.str()).c_str(); // build the path name
				if(!boost::filesystem::exists(p)) break; // check for it
				ss.str(std::string()); // flush the string stream				
			}
		
			
		}

		// update gui 
		ui->experimentNumberSpin->setValue(i);

	} catch(std::exception &e) {
		std::cout << "Problem parsing config file: " << e.what() << std::endl;
	}

	refreshStreams();
}


void MainWindow::refreshStreams(void) {

	std::cout << "refreshing streams ..." <<std::endl;
	resolvedStreams.clear();
	resolvedStreams = lsl::resolve_streams(1.0);
	std::vector<std::string> streamNames;


	for(std::vector<lsl::stream_info>::iterator it=resolvedStreams.begin(); it!=resolvedStreams.end(); ++it)
		streamNames.push_back(it->name()+ " (" + it->hostname()+")");	
	
	// add code to sort the streams here?
	std::sort(streamNames.begin(), streamNames.end());
		


	// for some reason std::find returns a bad iterator on negative results
	// so I do it the ludite way
	
	missingStreams.clear();
	bool got_one;
	std::vector<std::string>::iterator it1;
	std::vector<std::string>::iterator it2; 
	for(it1 = requiredStreams.begin(); it1!=requiredStreams.end(); ++it1) {
		got_one = false;
		for(it2 = streamNames.begin(); it2!=streamNames.end(); ++it2) {
			if(*it1 == *it2) // we have a match  
				got_one = true;
				break;
		}
		if(got_one==false) // TODO, check to make sure this isn't an emtpy string!
			if(*it1!="")
				missingStreams.push_back(*it1); // push this string onto the missing vector
	}

	std::sort(missingStreams.begin(), missingStreams.end());


	QBrush good_brush, bad_brush; 
	good_brush.setColor(QColor(0,128,0));
	bad_brush.setColor(QColor(255,0,0));

	std::vector<std::string> previouslyChecked;
	QListWidgetItem *item;
	for(unsigned i=0;i<ui->streamList->count();i++) {
		item=ui->streamList->item(i);
		if(std::find(streamNames.begin(), streamNames.end(), item->text().toStdString())!=streamNames.end()) { 
			if(item->checkState() == Qt::Checked)	
				previouslyChecked.push_back(item->text().toStdString());
		}
	}



	
	ui->streamList->clear();
	
	for(unsigned i=0; i<streamNames.size(); i++) {	
		
		item=new QListWidgetItem(QString::fromStdString(streamNames[i]), ui->streamList);
		item->setForeground(good_brush);
		item->setCheckState(Qt::Unchecked);

		if(std::find(previouslyChecked.begin(), previouslyChecked.end(), item->text().toStdString())!=previouslyChecked.end())
			item->setCheckState(Qt::Checked);
		
		if(std::find(requiredStreams.begin(), requiredStreams.end(), item->text().toStdString())!=requiredStreams.end())
			item->setCheckState(Qt::Checked);
		
		ui->streamList->addItem(item);	
	}

	for(unsigned i=0; i<missingStreams.size(); i++) {	
		item=new QListWidgetItem(QString::fromStdString(missingStreams[i]), ui->streamList);
		item->setForeground(bad_brush);
		item->setCheckState(Qt::Checked);
		ui->streamList->addItem(item);	
	}


}

void MainWindow::startRecording(void) {
	
	if (currentlyRecording == false ) {

		// automatically refresh streams
		refreshStreams();

		// check that all checked streams exist
		QListWidgetItem *item;
		QMessageBox *msgBox;
		for(int i=0; i<ui->streamList->count();i++) {
			item = ui->streamList->item(i);
			// if a checked stream is now missing
			if(item->checkState() == Qt::Checked && // if checked
				(std::find(missingStreams.begin(),  // and missing
			    		   missingStreams.end(), 
						   item->text().toStdString())!=missingStreams.end())) {
				// are you sure?
				msgBox = new QMessageBox;
				msgBox->setText("At least one of the streams that you checked seems to be offline.");
				msgBox->setInformativeText("Do you want to start recording anyway?");
				msgBox->setIcon(QMessageBox::Warning);
				msgBox->setStandardButtons(QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
				msgBox->setDefaultButton(QMessageBox::No);
				if(msgBox->exec()!=QMessageBox::Yes)
					return;
				break;
			}
		}

		// determine the experiment number block
		// scan the path for %n and %b
		std::string filename = ui->locationEdit->text().toStdString();
		std::string str_n("\%n");
		std::size_t pos_n;
		pos_n = filename.find(str_n);
		std::stringstream ss;
		if(pos_n < filename.size()) { // check to make sure it is there
			ss << ui->experimentNumberSpin->value();
			filename.replace(pos_n,  2, ss.str());
		}

		std::string str_b("\%b");
		std::size_t pos_b;
		pos_b = filename.find(str_b);
		if(pos_b<filename.size()) // check to make sure it is there
			filename.replace(pos_b,  2, currentBlock);
 
		// rename existing file if necessary
		int lastdot;
		std::string rename_to;
		if(boost::filesystem::exists(filename.c_str())) {
			lastdot = filename.find_last_of(".");
			for(int i=1;i<=9999;i++) { // search for highest _oldN
				ss << i;
				rename_to = filename.substr(0, lastdot) +
					"_old"+ss.str()+filename.substr(lastdot,filename.size());
				ss.str(std::string()); // flush the string stream

				if(!boost::filesystem::exists(rename_to.c_str())) { // found it
					try {
						boost::filesystem::rename(boost::filesystem::path(filename.c_str()),
							                      boost::filesystem::path(rename_to.c_str()));
					} catch(std::exception &e) {
						QMessageBox::information(this,"Permissions issue", ("Can not rename the file " + filename + " to " + rename_to + ". Please check your permissions.").c_str(), QMessageBox::Ok);
						return;
					}
				break;
				}
			}
			//std::cout << filename << std::endl;
			//std::cout << rename_to << std::endl;
		}
		std::string targetdir;

		// regardless, we need to check for this one
		targetdir = filename.substr(0,filename.find_last_of("/\\"));
		try {
			if(!boost::filesystem::exists(targetdir.c_str()))
				boost::filesystem::create_directories(targetdir.c_str());
		} catch(std::exception &e) {
			std::cout << "with creating directory: " << e.what() << std::endl;
			QMessageBox::information(this,"Permissions issue", ("Can not create the directory " + targetdir + ". Please check your permissions.").c_str(), QMessageBox::Ok);
			return;
		}

		std::vector<std::string> watchfor;

		// go through all the listed streams
		checkedStreams.clear();
		for(int i=0;i<ui->streamList->count();i++)
			// check if it is checked
			if(ui->streamList->item(i)->checkState() == Qt::Checked){

				// if checked and not missing, add it to the checkedStreams vector
				for(std::vector<lsl::stream_info>::iterator it=resolvedStreams.begin(); it!=resolvedStreams.end(); ++it)
					if(ui->streamList->item(i)->text().toStdString().compare(it->name()+ " (" + it->hostname()+")")==0)
						checkedStreams.push_back(*it);

				// if it is checked and also missing, watch for it
				if(std::find(missingStreams.begin(), missingStreams.end(), ui->streamList->item(i)->text().toStdString())!=missingStreams.end())
					watchfor.push_back(ui->streamList->item(i)->text().toStdString());
			}

		for(std::vector<std::string>::iterator it=watchfor.begin(); it!=watchfor.end();++it)
			std::cout << *it << std::endl;

		currentRecording = new recording(filename, checkedStreams, watchfor, 1);
		currentlyRecording = true;
		ui->stopButton->setEnabled(true);
		ui->startButton->setEnabled(false);
		startTime = (int)lsl::local_clock();

	}
	else
		QMessageBox::information(this,"Already recording", "The recording is already running", QMessageBox::Ok);

}

void MainWindow::stopRecording(void) {

	if(currentlyRecording==false)
		QMessageBox::information(this, "Not recording", "There is not ongoing recording", QMessageBox::Ok);
	else {

		// scripted action code here

		try {
			delete currentRecording;
		}catch(std::exception &e){
			std::cout << "exception on stop: " << e.what() << std::endl;
		}
		
		currentlyRecording=false;
		ui->startButton->setEnabled(true);
		ui->stopButton->setEnabled(false);
		statusBar()->showMessage("Stopped");

	}
}


MainWindow::~MainWindow() {
	delete timer;
	delete ui;
}
