#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QCloseEvent>
#include <QFileDialog>
#include <QMessageBox>
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>
#include <string>
#include <vector>

// Enobio API
#include "CEnobioCtrl.h"
#pragma comment (lib,"EnobioDLL.lib")
// LSL API
#include <lsl_cpp.h>


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent, const std::string &config_file);
    ~MainWindow();
    
private slots:
    // config file dialog ops (from main menu)
    void load_config_dialog();
    void save_config_dialog();

    // start the Enobio connection
    void link_enobio();

    // close event (potentially disabled)
    void closeEvent(QCloseEvent *ev);
private:
    // background data reader thread
	void read_thread(int nChannels, int nRate);

    // raw config file IO
    void load_config(const std::string &filename);
    void save_config(const std::string &filename);
	
	Enobio *amp;										// pointer to the amp class
	bool stop_;											// whether we're trying to stop the reader thread
    boost::shared_ptr<boost::thread> reader_thread_;	// our reader thread

    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
