#ifndef EGIAmpWindow_H
#define EGIAmpWindow_H

#include <QCloseEvent>
#include <QFileDialog>
#include <QMainWindow>
#include <QMessageBox>
#include <atomic>
#include <boost/asio.hpp>
#include <string>
#include <thread>
#include <vector>

namespace ip = boost::asio::ip;
typedef ip::tcp::iostream socket_stream;

namespace Ui {
class EGIAmpWindow;
}

class EGIAmpWindow : public QMainWindow {
	Q_OBJECT

public:
	explicit EGIAmpWindow(QWidget* parent, const std::string& config_file);
	EGIAmpWindow(const EGIAmpWindow&) = delete;
	~EGIAmpWindow();

private slots:
	// config file dialog ops (from main menu)
	void load_config_dialog();
	void save_config_dialog();

	// start the ampserver connection
	void link_ampserver();

	// close event (potentially disabled)
	void closeEvent(QCloseEvent* ev);
	void displayError(QString description);

	void unlockUI();
	void lockUI();

signals:
	void appendStatusMessage(QString message);
	void enableSampleRateSelection(bool enable);
	void channelCountUpdated(int channelCount);
	void channelCountEnabled(bool enabled);
	void fieldsEnabled(bool enabled);
	void setLinkButtonText(QString text);
	void error(QString description);
	void enableUI();
	void disableUI();

private:
	typedef enum { packetType1 = 1, packetType2 = 2 } PacketType;

	typedef enum { NA300 = 0, NA400 = 1, NA410 = 2, NAunknown = 3 } AmplifierType;

	void halt_ampserver(int id);
	std::atomic<bool> stop_flag;

	// background data reader thread
	void read_packet_format_2(int nChannels, int* packetsReceived);
	void read_packet_format_1(int nChannels, int* packetsReceived);
	void getNotifications();
	bool getAmplifierDetails(int amplifierId);

	bool initAmplifier(bool verbose = false);

	std::string sendCommand(std::string command, std::string ampId, std::string channel,
	                        std::string value);
	void sendDatastreamCommand(std::string command, std::string ampId, std::string channel,
	                           std::string value);

	// raw config file IO
	void load_config(const std::string& filename);
	void save_config(const std::string& filename);

	std::unique_ptr<std::thread> reader_thread_;       // our reader thread
	std::unique_ptr<std::thread> notification_thread_; // our reader thread
	// streams talking to the AmpServer
	socket_stream commandStream_;
	socket_stream notificationStream_;
	socket_stream dataStream_;

	PacketType packetType_ = packetType1;
	AmplifierType amplifierType_ = NAunknown;
	uint16_t channelCount_ = 0;
	float scalingFactor_ = 0.0;
	int lastPacketCounter_ = 0;
	int lastTimeStamp_ = 0;
	int lastPacketCounterWithTimeStamp_ = 0;
	int packetsReceived_ = 0;

	Ui::EGIAmpWindow* ui;
};

#endif // EGIAmpWindow_H
