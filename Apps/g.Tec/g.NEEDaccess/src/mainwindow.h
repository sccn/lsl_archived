#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
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

private slots:
    // From GUI
    void on_scanPushButton_clicked();
	void on_connectPushButton_clicked();
    void on_goPushButton_clicked();
	void on_availableListWidget_itemSelectionChanged();

private:
	static bool handleResult(QString calling_func, GDS_RESULT ret);
	static void dataReadyCallback(GDS_HANDLE connectionHandle, void* usrData);

    void load_config(const QString filename);
    // void save_config(const QString filename);  // TODO: Actually save the config to a file.

	struct GDS_DeviceInfo
	{
		uint32_t index;
		bool inUse;
		QString name;
		GDS_DEVICE_TYPE type;
	};

    Ui::MainWindow *ui;

	// GDS communication
	GDS_HANDLE m_connectionHandle = 0;
	GDS_ENDPOINT m_hostEndpoint = { "127.0.0.1", 50223 };
	GDS_ENDPOINT m_localEndpoint = { "127.0.0.1", 50224 };
	int16_t m_updateRate;
	// LSL variables
	std::vector<std::vector<float>> m_dataBuffer;
	lsl::stream_outlet* m_eegOutlet;
	// lsl::stream_outlet* m_eventOutlet;
};

#endif // MAINWINDOW_H
