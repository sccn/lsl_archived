// reset camera on combo box state change



#include "videopanel.h"
#include "ui_videopanel.h"
#include "videosettings.h"

#include <iostream>



VideoPanel::VideoPanel(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::videoPanel),
	camera(0),
	imageCapture(0),
	mediaRecorder(0)
{
    ui->setupUi(this); 
	
	QObject::connect(ui->monitorCheck, SIGNAL(stateChanged(int)), this, SLOT(monitorToggle(int)) );
	QObject::connect(ui->recordingSettingsButton, SIGNAL(clicked()), this, SLOT(configureVideoSettings()));
	QObject::connect(ui->recordButton, SIGNAL(clicked()), this, SLOT(record()));
	
	//WinCapture::CreateInstance(&winCapture);
#ifdef _WIN32
	winCapture = new WinCapture();
	winFormatIdx = 0;
#endif
	refreshCameras();




	// recording flag
	stop_ = true;
}

VideoPanel::~VideoPanel()
{

	delete mediaRecorder;
    delete imageCapture;
    delete camera;
#ifdef _WIN32
	delete winCapture;
#endif

}

void VideoPanel::refreshCameras(void) {

#ifdef _WIN32

	std::vector<std::string> cameranames; 
	winCapture->EnumerateCameras(cameranames);

	for (std::vector<std::string>::iterator it = cameranames.begin();
		it != cameranames.end();
		++it){
		std::cout << *it << std::endl;
		ui->cameraBox->addItem(QString("%1").arg(it->c_str()));
	}
	ui->recordingSettingsButton->setEnabled(true);

	std::cout << ui->cameraBox->currentText().toStdString() << std::endl;
#else

	// Camera devices:
	// display device descriptions in combo box
	cameraInfos = QCameraInfo::availableCameras();
	for (QList<QCameraInfo>::Iterator it = cameraInfos.begin();
		it != cameraInfos.end(); ++it)
		ui->cameraBox->addItem(it->description());

	ui->recordingSettingsButton->setEnabled(TRUE);
	
	std::cout << ui->cameraBox->currentText().toStdString() << std::endl;

	if (cameraInfos.count() > 0);
		// set the default camera
		//setCamera();
#endif

}

void VideoPanel::setCamera(void) {

	delete imageCapture;
	delete mediaRecorder;
	delete camera;

	for (QList<QCameraInfo>::Iterator it = cameraInfos.begin();
		it != cameraInfos.end(); ++it)
		if (it->description()==ui->cameraBox->currentText()) 
			camera = new QCamera(*it);
	camera->setViewfinder(ui->viewFinder);

	imageCapture = new QCameraImageCapture(camera);

	mediaRecorder = new QMediaRecorder(camera);


	ui->recordingSettingsButton->setEnabled(TRUE);

}


void VideoPanel::monitorToggle(int state) {

#ifdef _WIN32
#else
	if (camera&&state == Qt::CheckState::Checked){
		camera->start();
	}
	if (camera&&state==Qt::CheckState::Unchecked)camera->stop();
#endif
}


void VideoPanel::configureVideoSettings(void) {
//
#ifdef _WIN32
	std::cout << ui->cameraBox->currentText().toStdString() << std::endl;

	winCapture->SelectDevice(ui->cameraBox->currentText().toStdString());
	VideoSettings settingsDialog(winCapture);


	settingsDialog.setWinFormat(winFormatIdx);
	if (settingsDialog.exec()) {
				winFormatIdx = settingsDialog.getWinFormatIdx();
		ui->requestedFR->setText(QString("%1").arg(winCapture->getInfoVec()[winFormatIdx].requestedFR));
		ui->resolutionEdit->setText(QString("%1x%2").arg(winCapture->getInfoVec()[winFormatIdx].width).arg(winCapture->getInfoVec()[winFormatIdx].height));
		std::cout << "winFormatIdx " << winFormatIdx << std::endl;
		winCapture->setFormatIdx(winFormatIdx);
	}

//
#else
	
	// copied from QT5 'Camera Example'

	//VideoSettings settingsDialog(mediaRecorder);
	// // if it is already there, then keep the current settings
	//settingsDialog.setAudioSettings(audioSettings);
	//settingsDialog.setVideoSettings(videoSettings);
	//settingsDialog.setFormat(videoContainerFormat);

	//if (settingsDialog.exec()) {
	//	audioSettings = settingsDialog.audioSettings();
	//	videoSettings = settingsDialog.videoSettings();
	//	videoContainerFormat = settingsDialog.format();

	//	mediaRecorder->setEncodingSettings(
	//		audioSettings,
	//		videoSettings,
	//		videoContainerFormat);
	//}
#endif
	ui->recordButton->setEnabled(true);
	ui->recordButton->setText("Record");
}

void VideoPanel::record(){

#ifdef _WIN32
	if (!winCapture->IsCapturing()) {
		try {


			//cameraInfos.clear();
			IMFActivate *pActivate = NULL;
			winCapture->initCapture(L"C:\\Users\\dmedine\\Desktop\\test_capture.mp4", pActivate);
			//winCapture_thread();

			//capture_thread_.reset(new boost::thread(&VideoPanel::winCapture_thread, this));
			ui->recordButton->setText("Stop");

		}
		catch (std::exception &e) {
			QMessageBox::critical(this, "Error", (std::string("Could not start recording: ") += e.what()).c_str(), QMessageBox::Ok);
			return;
		}
	}
	else {
		try{
			winCapture->finishCapture();
			ui->recordButton->setText("Record");
		}
		catch (std::exception &e) {
			QMessageBox::critical(this, "Error", (std::string("Could not stop recording: ") += e.what()).c_str(), QMessageBox::Ok);
			return;
		}


	}

#else
	if (capture_thread_) {

		// we are recording: shut down the recording thread
		try {
			setCursor(Qt::WaitCursor);
			stop_ = true;
			capture_thread_->interrupt();
			capture_thread_->join();
			capture_thread_.reset();
			setCursor(Qt::ArrowCursor);
		}
		catch (std::exception &e) {
			QMessageBox::critical(this, "Error", (std::string("Could not stop the background processing: ") += e.what()).c_str(), QMessageBox::Ok);
			return;
		}

		// indicate that we have stopped recording
		ui->recordButton->setText("Record");

	}

	else {
		stop_ = false;
	}

#endif


}

//void VideoPanel::winCapture_thread(void) {
//
//	HRESULT hr;
//
//	IMFMediaSource *pSource = NULL;		
//	//IMFSourceReader *pReader = winCapture->getSourceReader();
//
//	//pReader->AddRef();
//	hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
//	// convert qstring filename to WCHAR and send it through
//	winCapture->initCapture(L"C:\\Users\\dmedine\\Desktop\\capture_foo.mp4", pSource);
//
//	while (!stop_) {
//		hr=winCapture->rwFunction();
//	}
//	winCapture->finishCapture();
//	CoUninitialize();
//}