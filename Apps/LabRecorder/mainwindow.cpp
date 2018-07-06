#include "ui_mainwindow.h"
#include "mainwindow.h"

#include <string>
#include <vector>
#include <QSettings>
#include <QDateTime>
#include <QFileDialog>
#include <QMessageBox>
#include <QDebug>
#include <fstream>

// recording class
#include "recording.h"

MainWindow::MainWindow(QWidget *parent, const std::string &config_file) :
QMainWindow(parent),
ui(new Ui::MainWindow) {

	ui->setupUi(this);
	connect(ui->actionQuit, &QAction::triggered, this, &QMainWindow::close);
	connect(ui->actionLoadConfig, &QAction::triggered, [this](){
		this->load_config(QFileDialog::getOpenFileName(this,"Load Configuration File","","Configuration Files (*.cfg)"));
	});
	connect(ui->actionSaveConfig, &QAction::triggered, [this](){
		this->save_config(QFileDialog::getSaveFileName(this, "Save Configuration File", "", "Configuration Files (*.cfg)"));
	});
	connect(ui->browseButton, &QPushButton::clicked, [this](){
			this->ui->locationEdit->setText(QFileDialog::getSaveFileName(this,"Save recordings as...", "untitled.xdf", "XDF recordings (*.xdf)"));
	});
	connect(ui->blockList, static_cast<void(QComboBox::*)(const QString &)>(&QComboBox::activated), this, &MainWindow::blockSelected);
	connect(ui->refreshButton, &QPushButton::clicked, this, &MainWindow::refreshStreams);
	connect(ui->startButton, &QPushButton::clicked, this, &MainWindow::startRecording);
	connect(ui->stopButton, &QPushButton::clicked, this, &MainWindow::stopRecording);
	connect(ui->actionAbout, &QAction::triggered, [this](){
		QString infostr = QStringLiteral("LSL library version: ") + QString::number(lsl::library_version()) +
		        "\nLSL library info:" + lsl::lsl_library_info();
		QMessageBox::about(this, "About LabRecorder", infostr);
	});

	load_config(config_file.c_str());

	timer.reset(new QTimer(this));
	connect(&*timer, &QTimer::timeout, this, &MainWindow::statusUpdate);
	timer->start(1000);
	//startTime = (int)lsl::local_clock();

}

void MainWindow::statusUpdate() const {
	if(currentRecording) {
		auto elapsed = static_cast<unsigned int>(lsl::local_clock() - startTime);
		auto fileinfo = QFileInfo(recFilename);
		fileinfo.refresh();
		auto size = fileinfo.size();
		QString timeString = QStringLiteral("Recording to %1 (%2); %3kb)").arg(
			fileinfo.fileName(),
			QDateTime::fromTime_t(elapsed).toUTC().toString("hh:mm:ss"),
			QString::number(size / 1000));
		statusBar()->showMessage(timeString);
	}
}

void MainWindow::closeEvent(QCloseEvent *ev) {
	if (currentRecording)
		ev->ignore();
}

void MainWindow::blockSelected(const QString& block) {
	if(currentRecording)
		QMessageBox::information(this, "Still recording", "Please stop recording before switching blocks.", QMessageBox::Ok);
	else {
		currentBlock = block;
		// scripted action code here...
	}
	//std::cout << item->text().toStdString() <<std::endl;
}

void MainWindow::load_config(QString filename) {
	qInfo() << "loading config file " << filename;
	try
    {
		if(!QFileInfo(filename).exists()) throw std::runtime_error("Settings file doesn't exist.");
		QSettings pt(filename, QSettings::Format::IniFormat);

		// ----------------------------
		// required streams
		// ----------------------------
		requiredStreams = pt.value("RequiredStreams").toStringList();

		// ----------------------------
		// online sync streams
		// ----------------------------
		QStringList onlineSyncStreams = pt.value("OnlineSync", "").toStringList();
		for(QString& oss: onlineSyncStreams) {
			QStringList words = oss.split(' ', QString::SkipEmptyParts);
			// The first two words ("StreamName (PC)") are the stream identifier
			if(words.length() < 2) {
				qInfo() << "Invalid sync stream config: " << oss;
				continue;
			}
			QString key = words.takeFirst() + ' ' + words.takeFirst();

			int val = 0;
			for (auto word: words) {
				if (word == "post_clocksync") { val |= lsl::post_clocksync; }
				if (word == "post_dejitter") { val |= lsl::post_dejitter; }
				if (word == "post_monotonize") { val |= lsl::post_monotonize; }
				if (word == "post_threadsafe") { val |= lsl::post_threadsafe; }
				if (word == "post_ALL") { val = lsl::post_ALL; }
			}
			syncOptionsByStreamName[key.toStdString()] = val;
			qInfo() << "stream sync options: " << key << ": " << val;
		}

		// ----------------------------
		// recording location
		// ----------------------------
		ui->blockList->addItems(pt.value("SessionBlocks").toStringList());
		if(ui->blockList->count())
			currentBlock = ui->blockList->itemText(0);

		// get the path as a string
		QString str_path = pt.value("StorageLocation", "C:/Recordings/CurrentStudy/exp%n/untitled.xdf").toString();
		ui->locationEdit->setText(str_path);

		// replace %n as experiment number placeholder
		int pos_n = str_path.indexOf(QStringLiteral("%n"));
		if (pos_n != -1) {
			str_path[pos_n + 1] = '1';
			for (int i = 1; i < 10000; i++) {
				if (!QDir(str_path.arg(i)).exists()) {
					// update gui
					ui->experimentNumberSpin->setValue(i);
					break;
				}
			}
		}
	} catch(std::exception &e) {
		qWarning() << "Problem parsing config file: " << e.what();
	}
	//std::cout << "refreshing streams ..." <<std::endl;
	refreshStreams();
}

void MainWindow::save_config(QString filename)
{
	QSettings settings(filename, QSettings::Format::IniFormat);
	settings.setValue("StorageLocation", ui->locationEdit->text());
	qInfo() << requiredStreams;
	settings.setValue("RequiredStreams", requiredStreams);
	// Stub.
}

QSet<QString> MainWindow::getCheckedStreams() const
{
	QSet<QString> checked;
	for(int i=0;i<ui->streamList->count();i++) {
		QListWidgetItem* item = ui->streamList->item(i);
		if(item->checkState() == Qt::Checked)
			checked.insert(item->text());
	}
	return checked;
}

/**
 * @brief MainWindow::refreshStreams Find streams, generate a list of missing streams
 * and fill the UI streamlist.
 * @return A vector of found stream_infos
 */
std::vector<lsl::stream_info> MainWindow::refreshStreams() {
	std::vector<lsl::stream_info> resolvedStreams = lsl::resolve_streams(1.0);

	QSet<QString> foundStreamNames;
	for(auto& s: resolvedStreams)
		foundStreamNames.insert(QString::fromStdString(s.name()+ " (" + s.hostname()+")"));

	const QSet<QString> previouslyChecked = getCheckedStreams();
	// Missing streams: all checked or required streams that weren't found
	missingStreams = (previouslyChecked + requiredStreams.toSet()) - foundStreamNames;

	// (Re-)Populate the UI list
	const QBrush good_brush(QColor(0,128,0)), bad_brush(QColor(255,0,0));
	ui->streamList->clear();
	for(auto&& streamName: foundStreamNames + missingStreams) {
		QListWidgetItem* item = new QListWidgetItem(streamName, ui->streamList);

		item->setCheckState(previouslyChecked.contains(streamName) ? Qt::Checked : Qt::Unchecked);
		item->setForeground(missingStreams.contains(streamName) ? bad_brush : good_brush);

		ui->streamList->addItem(item);
	}

	return resolvedStreams;
}

void MainWindow::startRecording() {
	
	if (!currentRecording ) {

		// automatically refresh streams
		const std::vector<lsl::stream_info> resolvedStreams = refreshStreams();
		const QSet<QString> checked = getCheckedStreams();

		// if a checked stream is now missing
		// change to "checked.intersects(missingStreams) as soon as Ubuntu 16.04/Qt 5.5 is EOL
		QSet<QString> missing = checked;
		if(!missing.intersect(missingStreams).isEmpty()) {
				// are you sure?
			    QMessageBox msgBox(QMessageBox::Warning,
				                   "Stream not found",
				                   "At least one of the streams that you checked seems to be offline",
				                   QMessageBox::Yes | QMessageBox::No,
				                   this);
				msgBox.setInformativeText("Do you want to start recording anyway?");
				msgBox.setDefaultButton(QMessageBox::No);
				if(msgBox.exec()!=QMessageBox::Yes)
					return;
		}

		if(checked.isEmpty()) {
			QMessageBox msgBox(QMessageBox::Warning, "No streams selected",
							   "You have selected no streams", QMessageBox::Yes | QMessageBox::No,
				                   this);
				msgBox.setInformativeText("Do you want to start recording anyway?");
				msgBox.setDefaultButton(QMessageBox::No);
				if(msgBox.exec()!=QMessageBox::Yes)
					return;
		}

		// determine the experiment number block
		// scan the path for %n/%1 and %b/%2
		recFilename = ui->locationEdit->text();
		int pos_n = recFilename.indexOf("%n");
		if(pos_n != -1)
			recFilename.replace(pos_n, 2, QString::number(ui->experimentNumberSpin->value()));

		int pos_b = recFilename.indexOf("%b");
		if(pos_b != -1) // check to make sure it is there
			recFilename.replace(pos_b,  2, currentBlock);

		QFileInfo recFileInfo(recFilename);
		if(recFileInfo.exists()) {
			if(recFileInfo.isDir()) {
				QMessageBox::warning(this, "Error", "Recording path already exists and is a directory");
				return;
			}
			QString rename_to = recFileInfo.baseName() + "_old%1." + recFileInfo.suffix();
			// search for highest _oldN
			int i = 1;
			while(QFileInfo(rename_to.arg(i)).exists()) i++;
			QString newname = rename_to.arg(i);
			if(!QFile::rename(recFileInfo.absoluteFilePath(), newname)) {
				QMessageBox::warning(this,"Permissions issue", "Can not rename the file " + recFilename + " to " + recFileInfo.path() + '/' + newname);
			    return;
			}
			qInfo() << "Moved existing file to " << newname;
			recFileInfo.refresh();
		}

		// regardless, we need to create the directory if it doesn't exist
		if(!recFileInfo.dir().mkpath(".")) {
			QMessageBox::warning(this,"Permissions issue", "Can not create the directory " + recFileInfo.dir().path() + ". Please check your permissions.");
			return;
		}

		// go through all the listed streams
		std::vector<lsl::stream_info> checkedStreams;

		for(const lsl::stream_info& stream: resolvedStreams)
			if(checked.contains(QString::fromStdString(stream.name() + " (" + stream.hostname()+')')))
				checkedStreams.push_back(stream);

		std::vector<std::string> watchfor;
		for(const QString& missing: missingStreams)
			watchfor.push_back(missing.toStdString());
		qInfo() << "Missing: " << missingStreams;

		currentRecording.reset(new recording(recFilename.toStdString(), checkedStreams, watchfor, syncOptionsByStreamName, 1));
		ui->stopButton->setEnabled(true);
		ui->startButton->setEnabled(false);
		startTime = (int)lsl::local_clock();

	}
	else
		QMessageBox::information(this,"Already recording", "The recording is already running", QMessageBox::Ok);

}

void MainWindow::stopRecording() {

	if(!currentRecording)
		QMessageBox::information(this, "Not recording", "There is not ongoing recording", QMessageBox::Ok);
	else {
		try {
			currentRecording = nullptr;
		}catch(std::exception &e){
			qWarning() << "exception on stop: " << e.what();
		}
		ui->startButton->setEnabled(true);
		ui->stopButton->setEnabled(false);
		statusBar()->showMessage("Stopped");

	}
}

