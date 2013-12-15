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


using namespace boost::asio;
using boost::asio::ip::udp;
typedef boost::shared_ptr<udp::socket> udp_socket_p;


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

    // close event (potentially disabled)
    void closeEvent(QCloseEvent *ev);
private:
    // background data reader thread
	void read_thread(udp_socket_p socket, std::string serverAddress);

    // raw config file IO
    void load_config(const std::string &filename);
    void save_config(const std::string &filename);

	boost::asio::io_service io_service_;
    boost::shared_ptr<boost::thread> reader_thread_;	// our reader thread
	bool stop_;											// whether our reader thread is supposed to stop

    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
