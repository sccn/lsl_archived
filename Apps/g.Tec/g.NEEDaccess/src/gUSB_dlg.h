#pragma once
#include <QDialog>
#include "GDSClientAPI.h"


namespace Ui {
	class GUSBDlg;
}


class GUSBDlg : public QDialog
{
	Q_OBJECT
public:
	explicit GUSBDlg(QWidget *parent = 0);

	void set_configs(GDS_HANDLE*, std::vector<GDS_CONFIGURATION_BASE>);

	Ui::GUSBDlg *ui;
	void* m_device_config;

private slots:
	void on_loadCfgButton_clicked();
	void on_saveCfgButton_clicked();
	void handle_sampleRateChange(int index);
	void on_bandpass_pushButton_clicked();
	void on_notch_pushButton_clicked();
private:
	void create_widgets();
	void update_ui();
	void update_filters();
	void accept() override;
	void load_config(const QString filename);
	void save_config(const QString filename);
	void apply_filter_to_enabled_chans(int widget_ix, int value);

	std::vector<GDS_CONFIGURATION_BASE> m_configs;
	GDS_HANDLE* m_pHandle;
	const std::vector<uint32_t> gUSBamp_sample_rates = { 32, 64, 128, 256, 512, 600, 1200, 2400, 4800, 9600, 19200, 38400 };

	const QString default_config_fname = "gUSB_config.cfg";
};