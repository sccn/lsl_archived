#ifndef VIDEOPANEL_H
#define VIDEOPANEL_H

#define WIN32_LEAN_AND_MEAN

#ifdef _WIN32
#include "wincapture.h"
#endif

#include <QCamera>
#include <QtWidgets>
#include <QMediaRecorder>
#include <QCameraImageCapture>
#include <QCameraViewFinder>
#include <QCameraInfo>
#include <QVideoWidget>
#include <QApplication>
#include <QMediaService>
#include <QMediaRecorder>
#include <QMediaMetaData>
#include <QMainWindow>


#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>

namespace Ui {
class videoPanel;
}

class VideoPanel : public QWidget
{
    Q_OBJECT

public:
    VideoPanel(QWidget *parent = 0);
    ~VideoPanel();

private slots:
	void monitorToggle(int state);
	void configureVideoSettings(void);

	void record(void);

private:

    Ui::videoPanel *ui;

	QCamera *camera;
	QList<QCameraInfo> cameraInfos;
	QCameraImageCapture *imageCapture;
    QMediaRecorder* mediaRecorder;

    QImageEncoderSettings imageSettings;
    QAudioEncoderSettings audioSettings;
    QVideoEncoderSettings videoSettings;

#ifdef _WIN32
	//// for windows
	WinCapture *winCapture;
	int winFormatIdx;
	void winCapture_thread(void); // windows only
#endif
    
	QString videoContainerFormat;
    bool isCapturingVideo;

	void refreshCameras(void);
	void setCamera(void);




	void capture_thread();

	boost::shared_ptr<boost::thread> capture_thread_;
	bool stop_;

};

#endif // VIDEOPANEL_H
