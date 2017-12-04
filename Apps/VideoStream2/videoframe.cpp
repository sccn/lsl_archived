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

	// if windows
	// get the names of all available cameras
	std::vector<std::string> cameraNames;
	GetDeviceInfo_win gdInfo;
	gdInfo.getCameraNames(cameraNames);

	// put the names into the ui
	for (std::vector<std::string>::iterator it = cameraNames.begin();
		it != cameraNames.end();
		++it) {
		ui->cbCameras->addItem(QString("%1").arg(it->c_str()));
	}
	
	std::vector<wmfCameraInfo> cameraInfo;
	gdInfo.getCameraInfo(ui->cbCameras->currentText().toStdString(), cameraInfo, m_cameraSettings);

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
