#ifndef MAINWINDOW_H
#define MAINWINDOW_H

// boost
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>
// Qt
#include <QMainWindow>
#include <QCloseEvent>
#include <QMessageBox>
#include <QHostInfo>
// WASAPI
#include <Audioclient.h>
#include <Mmdeviceapi.h>
// LSL
#include "../../LSL/liblsl/include/lsl_cpp.h"


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:    
	// close event: used to disable closing while linked
    void closeEvent(QCloseEvent *ev);

    // start/stop the linkage
    void link_audio();

private:
	// the actual processing thread 
	void process_samples(IMMDeviceEnumerator *pEnumerator,IMMDevice *pDevice,IAudioClient *pAudioClient,IAudioCaptureClient *pCaptureClient,WAVEFORMATEX *pwfx,LPWSTR devID);

	bool shutdown_;										// tells the thread that it should shut itself down
	boost::shared_ptr<boost::thread> reader_thread_;	// the background thread
    Ui::MainWindow *ui;									// window pointer
	
	std::string source_id;

};

#endif // MAINWINDOW_H
