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

// Tobii API
#include "tet.h"
#include "ttime.h"
#pragma comment (lib,"tet.lib")
#pragma comment (lib,"ttime.lib")

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

    // start the Tobii connection
    void link_tobii();

    // close event (potentially disabled)
    void closeEvent(QCloseEvent *ev);
private:
	// packet handling callback
	static void __stdcall callback(ETet_CallbackReason reason, void *pData, void *pApplicationData);
	void handle_packet(ETet_CallbackReason reason, void *pData);

    // raw config file IO
    void load_config(const std::string &filename);
    void save_config(const std::string &filename);

	boost::shared_ptr<lsl::stream_outlet> outlet;	// our stream outlet

    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
