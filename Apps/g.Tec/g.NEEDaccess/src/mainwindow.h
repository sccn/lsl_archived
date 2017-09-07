#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "lsl_cpp.h"
#include "GDSClientAPI.h"
//#include "gdsthread.h"

const QString default_config_fname = "gneedaccess_config.cfg";

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0,
        const QString config_file = default_config_fname);
    ~MainWindow();

private slots:
	// GUI
    void on_scanPushButton_clicked();
	void on_connectPushButton_clicked();
    void on_goPushButton_clicked();
	void on_availableListWidget_itemSelectionChanged();
	// React to GDSThread
	void handleOutletsStarted(bool success);
	void handleDataSent(int nSamples);

private:
	struct chan_info_type {
		bool enabled;
		std::string label;
		std::string type;
		std::string unit;
	};
	struct dev_info_type {
		std::string name;
		int channel_count = 0;
		size_t nsamples_per_scan = 0;
		size_t scans_per_block = 0;
		double nominal_srate = lsl::IRREGULAR_RATE;
		lsl::channel_format_t channel_format = lsl::cf_float32;
		std::vector<chan_info_type> channel_infos;
	};

	// Static methods
	static bool handleResult(std::string calling_func, GDS_RESULT ret);
	static void dataReadyCallback(GDS_HANDLE connectionHandle, void* usrData);
	
	// Private methods
    void load_config(const QString filename);
    // void save_config(const QString filename);  // TODO: Actually save the config to a file.

    Ui::MainWindow *ui;

	// GDS communication
	GDS_HANDLE m_connectionHandle = 0;
	GDS_ENDPOINT m_hostEndpoint = { "127.0.0.1", 50223 };
	GDS_ENDPOINT m_localEndpoint = { "127.0.0.1", 50224 };
	int16_t m_updateRate;

	// LSL variables
	std::vector<std::vector<float>> m_dataBuffer;
	lsl::stream_outlet* m_eegOutlet;

	//GDSThread m_thread;
};

#endif // MAINWINDOW_H
