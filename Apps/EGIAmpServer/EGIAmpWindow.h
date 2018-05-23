#ifndef EGIAmpWindow_H
#define EGIAmpWindow_H

#include <QMainWindow>
#include <QCloseEvent>
#include <QFileDialog>
#include <QMessageBox>
#include <boost/asio.hpp>
#include <string>
#include <vector>
#include <thread>
#include <atomic>

// LSL API
#include <lsl_cpp.h>

namespace ip = boost::asio::ip;
typedef ip::tcp::iostream socket_stream;

namespace Ui {
class EGIAmpWindow;
}

class EGIAmpWindow : public QMainWindow
{
    Q_OBJECT
    
public:
	explicit EGIAmpWindow(QWidget *parent, const std::string &config_file);
	EGIAmpWindow(const EGIAmpWindow&) = delete;
	~EGIAmpWindow();
    
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
	std::atomic<bool> stop_flag;

    // background data reader thread
	void read_thread(const std::string &address, int amplifierId, int samplingRate, int nChannels);

    // raw config file IO
    void load_config(const std::string &filename);
    void save_config(const std::string &filename);
	
	std::unique_ptr<std::thread> reader_thread_;	// our reader thread
	// streams talking to the AmpServer
	socket_stream commandStream_;
	socket_stream notificationStream_;
	socket_stream dataStream_;

	Ui::EGIAmpWindow *ui;
};

#endif // EGIAmpWindow_H
