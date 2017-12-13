#include <QMediaMetaData>
#include <QPalette>

#include "videoframe.h"
Q_DECLARE_METATYPE(QCameraInfo)

VideoFrame::VideoFrame(QWidget *parent) : QMainWindow(parent), ui(new Ui::VideoFrame) {
	ui->setupUi(this);

	// paint the background of the cameraviewfinder black
	QPalette pal = palette();
	pal.setColor(QPalette::Background, Qt::black);
	ui->wBackground->setAutoFillBackground(true);
	ui->wBackground->setPalette(pal);

	QObject::connect(ui->cbFrameRate, SIGNAL(currentIndexChanged(int)), this, SLOT(updateResolutions(int)));

	updateCameras();
	
	////audio codecs
	//ui->cbAudioCodec->addItem(tr("Default audio codec"), QVariant(QString()));
	//const QStringList supportedAudioCodecs = mediaRecorder->supportedAudioCodecs();
	//for (const QString &codecName : supportedAudioCodecs) {
	//	QString description = mediaRecorder->audioCodecDescription(codecName);
	//	ui->cbAudioCodec->addItem(codecName + ": " + description, QVariant(codecName));
	//}

	////sample rate:
	//const QList<int>supportedAudioSampleRates = mediaRecorder->supportedAudioSampleRates();
	//for (int sampleRate : supportedAudioSampleRates)
	//	ui->cbAudioSR->addItem(QString::number(sampleRate), QVariant(sampleRate));

	//ui->audioQualitySlider->setRange(0, int(QMultimedia::VeryHighQuality));

	//// cameras
	//m_availableCameras.clear();
	//m_availableCameras = QCameraInfo::availableCameras();
	//for (const QCameraInfo &cameraInfo : m_availableCameras) {
	//	QString cameraName = cameraInfo.deviceName();
	//	if (cameraInfo == QCameraInfo::defaultCamera())
	//		cameraName += " (default)";
	//	ui->cbCameras->addItem(cameraName);

	//}

	//// video settings
	//ui->cbVideoCodec->addItem(tr("Default video codec"), QVariant(QString()));
	//const QStringList supportedVideoCodecs = mediaRecorder->supportedVideoCodecs();
	//for (const QString &codecName : supportedVideoCodecs) {
	//	QString description = mediaRecorder->videoCodecDescription(codecName);
	//	ui->cbVideoCodec->addItem(codecName + ": " + description, QVariant(codecName));
	//}

	////ui->videoQualitySlider->setRange(0, int(QMultimedia::VeryHighQuality));


	//ui->cbResolution->addItem(tr("Default"));
	//const QList<QSize> supportedResolutions = mediaRecorder->supportedResolutions();
	//for (const QSize &resolution : supportedResolutions) {
	//	ui->cbResolution->addItem(QString("%1x%2").arg(resolution.width()).arg(resolution.height()),
	//		QVariant(resolution));
	//}

	//ui->cbFrameRate->addItem(tr("Default"));
	//const QList<qreal> supportedFrameRates = mediaRecorder->supportedFrameRates();
	//for (qreal rate : supportedFrameRates) {
	//	QString rateString = QString("%1").arg(rate, 0, 'f', 2);
	//	ui->cbFrameRate->addItem(rateString, QVariant(rate));
	//}

	//ui->cbContainer->addItem(tr("Default container"), QVariant(QString()));
	//const QStringList formats = mediaRecorder->supportedContainers();
	//for (const QString &format : formats) {
	//	ui->cbContainer->addItem(format + ": " + mediaRecorder->containerDescription(format),
	//		QVariant(format));
	//}
}

VideoFrame::~VideoFrame() {
	delete ui;
}

void VideoFrame::updateCameras(void) {
	// if windows
	// get the names of all available cameras
	std::vector<std::string> cameraNames;
	m_gdInfo.getCameraNames(cameraNames);

	// put the names into the ui
	for (std::vector<std::string>::iterator it = cameraNames.begin();
		it != cameraNames.end();
		++it) {
		ui->cbCameras->addItem(QString("%1").arg(it->c_str()));
	}

	updateFrameRate(1);
	
}

void VideoFrame::updateFrameRate(int idx) {

	std::vector<wmfCameraInfo> cameraInfo;
	m_gdInfo.getCameraInfo(ui->cbCameras->currentText().toStdString(), cameraInfo);

	for (GetDeviceInfo_win::t_cameraSettings::iterator it = m_gdInfo.cameraSettings.begin();
		it != m_gdInfo.cameraSettings.end();
		++it) {
		ui->cbFrameRate->addItem(QString("%1").arg(it->first));
	}

	updateResolutions(ui->cbFrameRate->currentIndex());
}

void VideoFrame::updateResolutions(int idx) {

	int which = std::stoi(ui->cbFrameRate->currentText().toStdString());
	ui->cbResolution->clear();
	for (GetDeviceInfo_win::t_cameraSettings::iterator it1 = m_gdInfo.cameraSettings.begin();
		it1 != m_gdInfo.cameraSettings.end();
		++it1) {
		if (it1->first == which) {
			for (std::vector<resolution>::iterator it2 = it1->second.begin();
				it2 != it1->second.end();
				++it2) {
				ui->cbResolution->addItem(QString("%1 x %2").arg(it2->width).arg(it2->height));
			}
		}
	}
}