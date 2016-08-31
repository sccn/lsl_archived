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

namespace ip = boost::asio::ip;
typedef ip::tcp::iostream socket_stream;
typedef boost::shared_ptr<socket_stream> socket_stream_p;

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

    // start the ampserver connection
    void link_ampserver();

    // close event (potentially disabled)
    void closeEvent(QCloseEvent *ev);
private:
	void halt_ampserver(int id);

    // background data reader thread
	void read_thread(const std::string &address, int amplifierId, int samplingRate, int nChannels);

    // raw config file IO
    void load_config(const std::string &filename);
    void save_config(const std::string &filename);
	
    boost::shared_ptr<boost::thread> reader_thread_;	// our reader thread
	// streams talking to the AmpServer
	socket_stream_p commandStream_;
	socket_stream_p notificationStream_;
	socket_stream_p dataStream_;

    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
