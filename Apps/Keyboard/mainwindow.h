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
    void link_keyboard();
private:
	static LRESULT CALLBACK keyboard_callback(int inCode, WPARAM inWParam, LPARAM inLParam);
	LRESULT CALLBACK on_key(int inCode, WPARAM inWParam, LPARAM inLParam);

	HHOOK kbdHook_;									// handle to our keyboard hook
	bool isPressed_[256];							// whether a key is already pressed
	boost::shared_ptr<lsl::stream_outlet> outlet_;	// our stream outlet
    Ui::MainWindow *ui;								// window pointer
};

extern MainWindow *theWindow;

#endif // MAINWINDOW_H
