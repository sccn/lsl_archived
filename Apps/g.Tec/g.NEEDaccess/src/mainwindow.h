#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMutex>
#include "lsl_cpp.h"
#include "GDSClientAPI.h"

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

signals:
	void dataReady(void);

private slots:
	// GUI
    void on_scanPushButton_clicked();
	void on_connectPushButton_clicked();
    void on_goPushButton_clicked();
	void on_availableListWidget_itemSelectionChanged();
	void on_loadConfigPushButton_clicked();
	void notify_samples_pushed();

private:
	struct chan_info_type {
		bool enabled;
		std::string label;
		std::string type;
		std::string unit;
	};
	struct dev_info_type {
		std::string name;
		bool is_creator = false;
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
	void enable_config_elements(bool enabled);

    Ui::MainWindow *ui;
	QMutex mutex;
	QTimer* m_pTimer;
	bool m_bConnected = false;
	bool m_bStreaming = false;
	size_t m_samplesPushed = 0;

	// GDS communication
	GDS_HANDLE m_connectionHandle = 0;
	GDS_ENDPOINT m_hostEndpoint = { "127.0.0.1", 50223 };
	GDS_ENDPOINT m_localEndpoint = { "127.0.0.1", 50224 };
	dev_info_type m_devInfo;

	// LSL variables
	float* m_dataBuffer = NULL;
	lsl::stream_outlet* m_eegOutlet;
};

#endif // MAINWINDOW_H
