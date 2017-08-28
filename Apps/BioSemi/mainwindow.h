#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QMainWindow>
#include <QFileDialog>
#include <QMessageBox>
#include <lsl_cpp.h>
#include "biosemi_io.h"
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>
#include <boost/asio.hpp>
#include <boost/algorithm/string.hpp>
#include <fstream>


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

    // chanlocs browse action
    void load_chanlocs_dialog();

    // start the amplifier connection
    void connect_amp();

	// close event
	void closeEvent(QCloseEvent *ev);

private:
    // background data reader thread
    void read_thread(std::map<std::string,std::vector<std::string> > locmap, std::string capdesign, std::string caplocation, std::string capcircumference, std::string capreference, bool resample);

    // raw config file IO
    void load_config(const std::string &filename);
    void save_config(const std::string &filename);

	bool stop_;

    // the biosemi backend
    boost::shared_ptr<biosemi_io> biosemi_;
    boost::shared_ptr<boost::thread> reader_thread_;

    // misc meta-data from the config
    std::string location_system_;
    std::string location_manufacturer_;
	std::vector<std::string> channels_;
	std::vector<std::string> types_;
	std::vector<int> index_set_;

    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
