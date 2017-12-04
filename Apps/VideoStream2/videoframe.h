#ifndef VIDEOFRAME_H
#define VIDEOFRAME_H

// Qt
#include "ui_videoframe.h"
#include <QMainWindow>
#include <QCamera>
#include <QCameraInfo>
#include <QCameraImageCapture>
#include <QMediaRecorder>
#include <QCameraViewfinder>

// if windows
#include "GetDeviceInfo_win.h"


#include <lsl_cpp.h>
namespace Ui {
class VideoFrame;
}

class VideoFrame : public QMainWindow {
	Q_OBJECT

public:
	explicit VideoFrame(QWidget *parent);
	~VideoFrame();

private:
	Ui::VideoFrame *ui;

	QCamera*camera = nullptr;
	QMediaRecorder *mediaRecorder = nullptr;

	QImageEncoderSettings m_imageSettings;
	QAudioEncoderSettings m_audioSettings;
	QVideoEncoderSettings m_videoSettings;
	QString m_qsVideoContainerFormat;
	bool m_bIsCapturingImage = false;
	bool m_bApplicationExiting = false;

	QList<QCameraInfo> m_availableCameras;
	// windows only
	GetDeviceInfo_win::t_cameraSettings m_cameraSettings;

};

#endif // VIDEOFRAME_H