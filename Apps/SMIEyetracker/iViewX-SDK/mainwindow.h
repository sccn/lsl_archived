#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QCloseEvent>
#include <QFileDialog>
#include <QMessageBox>
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>
#include <boost/asio.hpp>
#include <string>
#include <vector>

// LSL API
#include <lsl_cpp.h>

// iViewX API
#include "iViewXAPI.h"
int __stdcall on_sample(SampleStruct s);

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

    // start the iViewX connection
    void link_iviewx();
	void open_display_action();
    // close event (potentially disabled)
    void closeEvent(QCloseEvent *ev);

private:
    // raw config file IO
    void load_config(const std::string &filename);
    void save_config(const std::string &filename);
	void display_thread();
	//boost::shared_ptr<boost::thread> display_thread_;
	boost::thread *display_thread_;



    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
