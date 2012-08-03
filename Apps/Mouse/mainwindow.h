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

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

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
    void link_mouse();
private:
	static LRESULT CALLBACK mouse_callback(int inCode, WPARAM inWParam, LPARAM inLParam);
	LRESULT CALLBACK on_event(int inCode, WPARAM inWParam, LPARAM inLParam);

	HHOOK mouseHook_;										// handle to our mouse hook
	boost::shared_ptr<lsl::stream_outlet> outletButtons_;	// stream outlet for mouse button presses
	boost::shared_ptr<lsl::stream_outlet> outletPosition_;	// stream outlet for mouse movements
    Ui::MainWindow *ui;										// window pointer
};

extern MainWindow *theWindow;

#endif // MAINWINDOW_H
