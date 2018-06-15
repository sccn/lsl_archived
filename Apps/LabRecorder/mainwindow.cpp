#include "ui_mainwindow.h"
#include "mainwindow.h"

#include <cerrno>

#include <iostream>
#include <string>
#include <vector>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <boost/filesystem.hpp>
#include <QDateTime>
#include <QFileDialog>
#include <QMessageBox>
#include <fstream>

// recording class
#include "recording.h"

MainWindow::MainWindow(QWidget *parent, const std::string &config_file) :
QMainWindow(parent),
ui(new Ui::MainWindow) {

	ui->setupUi(this);
	connect(ui->actionQuit, &QAction::triggered, this, &QMainWindow::close);
	connect(ui->actionLoadConfig, &QAction::triggered, [&](){
		QString sel = QFileDialog::getOpenFileName(this,"Load Configuration File","","Configuration Files (*.cfg)");
		if (!sel.isEmpty()) load_config(sel.toStdString());
	});
	connect(ui->browseButton, &QPushButton::clicked, [&](){
		QString sel = QFileDialog::getSaveFileName(this,"Save recordings as...", "untitled.xdf", "XDF recordings (*.xdf);;XDF compressed recordings (*.xdfz)");
		if (!sel.isEmpty())
			ui->locationEdit->setText(sel);
	});
	//QObject::connect(ui->blockList, &QListWidget::itemClicked, QMainWindow::blockSelected);
	connect(ui->refreshButton, &QPushButton::clicked, this, &MainWindow::refreshStreams);
	connect(ui->startButton, &QPushButton::clicked, this, &MainWindow::startRecording);
	connect(ui->stopButton, &QPushButton::clicked, this, &MainWindow::stopRecording);

	currentlyRecording = false;
	load_config(config_file);

	timer.reset(new QTimer(this));
	connect(&*timer, &QTimer::timeout, this, &MainWindow::statusUpdate);
	timer->start(1000);
	//startTime = (int)lsl::local_clock();

}

void MainWindow::statusUpdate() const {
	if(currentlyRecording) {
		auto elapsed = static_cast<unsigned int>(lsl::local_clock() - startTime);

		std::ifstream in(recFilename, std::ifstream::ate | std::ifstream::binary);
		auto size = in.tellg();
		QString timeString = QStringLiteral("Recording (%1); %2kb)").arg(
			QDateTime::fromTime_t(elapsed).toUTC().toString("hh:mm:ss"),
			QString::number(size / 1000));
		
		statusBar()->showMessage(timeString);
	}
}

void MainWindow::closeEvent(QCloseEvent *ev) {
	if (currentlyRecording)
		ev->ignore();
}

void MainWindow::blockSelected(QListWidgetItem *item) {
	if(currentlyRecording)
		QMessageBox::information(this, "Still recording", "Please stop recording before switching blocks.", QMessageBox::Ok);
	else {
		currentBlock = item->text().toStdString();
		
		// scripted action code here...

	}
	//std::cout << item->text().toStdString() <<std::endl;
}

void MainWindow::load_config(const std::string &filename) {
	std::cout << "loading config file " << filename << std::endl;
	try
    {
		using boost::property_tree::ptree;
		ptree pt;
		read_ini(filename, pt);


		// ----------------------------
		// required streams
		// ----------------------------	
		std::string str_requiredStreams = pt.get<std::string>("RequiredStreams","");
		if(str_requiredStreams != "[]") {
			std::string rs_substr = str_requiredStreams.substr(1, str_requiredStreams.size()-2);
			boost::algorithm::split(requiredStreams,rs_substr,boost::algorithm::is_any_of(","),boost::algorithm::token_compress_on);
			for (auto& requiredStream: requiredStreams){
				boost::algorithm::trim_if(requiredStream,boost::algorithm::is_any_of(" '\""));
				std::cout << requiredStream << std::endl;
			}
		}

		// ----------------------------
		// online sync streams
		// ----------------------------	
		std::string str_onlineSyncStreams = pt.get<std::string>("OnlineSync","");
		if(str_onlineSyncStreams != "[]") {
			std::string oss_substr = str_onlineSyncStreams.substr(1, str_onlineSyncStreams.size()-2);
			boost::algorithm::split(onlineSyncStreams,oss_substr,boost::algorithm::is_any_of(","),boost::algorithm::token_compress_on);
			for(std::string& oss: onlineSyncStreams) {
				boost::algorithm::trim_if(oss, boost::algorithm::is_any_of(" '\""));
				std::vector<std::string>words;
				boost::algorithm::split(words, oss, boost::algorithm::is_any_of(" "), boost::algorithm::token_compress_on);

				std::string key = std::string(words[0] + " " + words[1]);
				
				int val = 0;
				for (std::size_t l = 2; l < words.size(); l++) {
					if (words[l] == "post_clocksync") { val |= lsl::post_clocksync; }
					if (words[l] == "post_dejitter") { val |= lsl::post_dejitter; }
					if (words[l] == "post_monotonize") { val |= lsl::post_monotonize; }
					if (words[l] == "post_threadsafe") { val |= lsl::post_threadsafe; }
					if (words[l] == "post_ALL") { val = lsl::post_ALL; }
				}
				syncOptionsByStreamName.insert(std::make_pair(key, val));
				std::cout << "key = " << key << std::endl;
				std::cout << "val = " << val << std::endl;
			}

		}

		// ----------------------------
		// recording location
		// ----------------------------
		std::vector<std::string>sessionBlocks;
		std::string str_sessionBlocks = pt.get<std::string>("SessionBlocks","");
		if(str_sessionBlocks != "[]") {
			std::string sb_substr = str_sessionBlocks.substr(1, str_sessionBlocks.size()-2);
			boost::algorithm::split(sessionBlocks,sb_substr,boost::algorithm::is_any_of(","),boost::algorithm::token_compress_on);

			bool selected = false;
			for (auto& sessionBlock: sessionBlocks) {
				boost::algorithm::trim_if(sessionBlock, boost::algorithm::is_any_of(" '\""));
				QListWidgetItem* item = new QListWidgetItem(QString::fromStdString(sessionBlock), ui->blockList);
				ui->blockList->addItem(item);
				if(!selected) {
					item->setSelected(true);
					blockSelected(item);
					selected = true;
				}
			}
		}

		// get the path as a string
		#ifdef win32
		const char* defaultpath = "C:\\Recordings\\CurrentStudy\\exp%n\\untitled.xdf";
		#else //win32
		const char* defaultpath = "exp%n/untitled.xdf";
		#endif //win32
		std::string str_path = pt.get<std::string>("StorageLocation", defaultpath);
		ui->locationEdit->setText(str_path.c_str());

		// scan the path for %n
		std::size_t pos_n = str_path.find("%n");
		
		// variables for string/path parsing
		std::string abs_path;
		boost::filesystem::path p;

		if (pos_n != std::string::npos)
		{
			abs_path.append(str_path.begin(), str_path.begin() + pos_n);
			// find the last value fo %n in the directories
			for (int i = 1; i < 10000; i++)
			{
				p = abs_path + std::to_string(i); // build the path name
				if (!boost::filesystem::exists(p)) {
					// update gui
					ui->experimentNumberSpin->setValue(i);

					break; // check for it
				}
			}
		}

	} catch(std::exception &e) {
		std::cout << "Problem parsing config file: " << e.what() << std::endl;
	}
	//std::cout << "refreshing streams ..." <<std::endl;
	refreshStreams();
}


void MainWindow::refreshStreams() {

	//std::cout << "refreshing streams ..." <<std::endl;
	resolvedStreams.clear();
	resolvedStreams = lsl::resolve_streams(1.0);
	std::vector<std::string> streamNames;


	for(auto& s: resolvedStreams)
		streamNames.push_back(s.name()+ " (" + s.hostname()+")");	
	
	// add code to sort the streams here?
	std::sort(streamNames.begin(), streamNames.end());
		

	missingStreams.clear();
	for(const auto& requiredStream: requiredStreams) {
		if (requiredStream.empty()) continue;
		auto it = std::find(streamNames.cbegin(), streamNames.cend(), requiredStream);
		if (it == streamNames.cend())
			missingStreams.push_back(requiredStream); // push this string onto the missing vector
	}

	std::sort(missingStreams.begin(), missingStreams.end());


	QBrush good_brush, bad_brush; 
	good_brush.setColor(QColor(0,128,0));
	bad_brush.setColor(QColor(255,0,0));

	std::vector<std::string> previouslyChecked;
	QListWidgetItem *item;
	for(int i=0;i<ui->streamList->count();i++) {
		item=ui->streamList->item(i);
		if(std::find(streamNames.begin(), streamNames.end(), item->text().toStdString())!=streamNames.end()) { 
			if(item->checkState() == Qt::Checked)	
				previouslyChecked.push_back(item->text().toStdString());
		}
	}

	
	ui->streamList->clear();
	
	for(auto&& streamName: streamNames) {
		item=new QListWidgetItem(QString::fromStdString(streamName), ui->streamList);
		item->setForeground(good_brush);
		item->setCheckState(Qt::Unchecked);

		if(std::find(previouslyChecked.begin(), previouslyChecked.end(), item->text().toStdString())!=previouslyChecked.end())
			item->setCheckState(Qt::Checked);
		
		if(std::find(requiredStreams.begin(), requiredStreams.end(), item->text().toStdString())!=requiredStreams.end())
			item->setCheckState(Qt::Checked);
		
		ui->streamList->addItem(item);	
	}

	for(auto&& missingStream: missingStreams) {
		item=new QListWidgetItem(QString::fromStdString(missingStream), ui->streamList);
		item->setForeground(bad_brush);
		item->setCheckState(Qt::Checked);
		ui->streamList->addItem(item);	
	}


}

void MainWindow::startRecording() {
	
	if (!currentlyRecording ) {

		// automatically refresh streams
		refreshStreams();

		for(int i=0; i<ui->streamList->count();i++) {
			QListWidgetItem *item = ui->streamList->item(i);
			// if a checked stream is now missing
			if(item->checkState() == Qt::Checked && // if checked
				(std::find(missingStreams.begin(),  // and missing
			    		   missingStreams.end(), 
						   item->text().toStdString())!=missingStreams.end())) {
				// are you sure?
				QMessageBox msgBox;
				msgBox.setText("At least one of the streams that you checked seems to be offline.");
				msgBox.setInformativeText("Do you want to start recording anyway?");
				msgBox.setIcon(QMessageBox::Warning);
				msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
				msgBox.setDefaultButton(QMessageBox::No);
				if(msgBox.exec()!=QMessageBox::Yes)
					return;
				break;
			}
		}

		// determine the experiment number block
		// scan the path for %n and %b
		recFilename = ui->locationEdit->text().toStdString();
		std::size_t pos_n = recFilename.find("%n");

		if(pos_n < recFilename.size()) { // check to make sure it is there
			recFilename.replace(pos_n,  2, std::to_string(ui->experimentNumberSpin->value()));
		}

		std::size_t pos_b = recFilename.find("%b");
		if(pos_b<recFilename.size()) // check to make sure it is there
			recFilename.replace(pos_b,  2, currentBlock);
 
		if(boost::filesystem::exists(recFilename)) {
			size_t lastdot = recFilename.find_last_of('.');
			for(int i=1;i<=9999;i++) { // search for highest _oldN
				std::string rename_to = recFilename.substr(0, lastdot) +
					"_old" + std::to_string(i) +
					recFilename.substr(lastdot,recFilename.size());

				if(!boost::filesystem::exists(rename_to)) { // found it
					try {
						boost::filesystem::rename(boost::filesystem::path(recFilename),
							                      boost::filesystem::path(rename_to));
					} catch(std::exception &e) {
						QMessageBox::information(this,"Permissions issue", QString::fromStdString("Can not rename the file " + recFilename + " to " + rename_to + ": " + e.what()), QMessageBox::Ok);
						return;
					}
				break;
				}
			}
			//std::cout << recFilename << std::endl;
			//std::cout << rename_to << std::endl;
		}

		// regardless, we need to check for this one
		std::string targetdir = recFilename.substr(0,recFilename.find_last_of("/\\"));
		try {
			if(!boost::filesystem::exists(targetdir))
				boost::filesystem::create_directories(targetdir);
		} catch(std::exception &e) {
			std::cout << "with creating directory: " << e.what() << std::endl;
			QMessageBox::information(this,"Permissions issue", "Can not create the directory " + QString(targetdir.c_str()) + ". Please check your permissions.", QMessageBox::Ok);
			return;
		}

		std::vector<std::string> watchfor;

		// go through all the listed streams
		checkedStreams.clear();
		for(int i=0;i<ui->streamList->count();i++)
			// check if it is checked
			if(ui->streamList->item(i)->checkState() == Qt::Checked){

				// if checked and not missing, add it to the checkedStreams vector
				for(const lsl::stream_info& stream: resolvedStreams)
					if(ui->streamList->item(i)->text().toStdString() == stream.name() + " (" + stream.hostname()+")")
						checkedStreams.push_back(stream);

				// if it is checked and also missing, watch for it
				if(std::find(missingStreams.begin(), missingStreams.end(), ui->streamList->item(i)->text().toStdString())!=missingStreams.end())
					watchfor.push_back(ui->streamList->item(i)->text().toStdString());
			}

		for(const std::string& s: watchfor)
			std::cout << s << std::endl;

		currentRecording = new recording(recFilename, checkedStreams, watchfor, syncOptionsByStreamName, 1);
		currentlyRecording = true;
		ui->stopButton->setEnabled(true);
		ui->startButton->setEnabled(false);
		startTime = (int)lsl::local_clock();

	}
	else
		QMessageBox::information(this,"Already recording", "The recording is already running", QMessageBox::Ok);

}

void MainWindow::stopRecording() {

	if(!currentlyRecording)
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

