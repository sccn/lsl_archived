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
	void set_configs(GDS_HANDLE*, std::vector<GDS_CONFIGURATION_BASE>, std::vector<std::string>*, std::vector<double>*);

	Ui::GUSBDlg *ui;

private slots:
	void on_loadCfgButton_clicked();
	void on_saveCfgButton_clicked();
	void handle_sampleRateChange(int index);
	void on_bandpass_pushButton_clicked();
	void on_notch_pushButton_clicked();
	void on_impedance_pushButton_clicked();
private:
	void create_widgets();
	void update_ui();
	void update_filters();
	void update_impedance_table();
	void accept() override;
	void load_config(const QString filename);
	void save_config(QString filename);
	void apply_filter_to_enabled_chans(int widget_ix, int value);

	GDS_CONFIGURATION_BASE m_config;
	std::vector<GDS_CONFIGURATION_BASE> m_configs;
	GDS_HANDLE* m_pHandle;
	std::vector<std::string>* m_pChannel_labels;
	std::vector<double>* m_pChannel_impedances;  // TOOD: Use GDS_GUSBAMP_GetImpedance to fill this.
	const std::vector<uint32_t> gUSBamp_sample_rates = { 32, 64, 128, 256, 512, 600, 1200, 2400, 4800, 9600, 19200, 38400 };

	const QString default_config_fname = "gUSB_default.cfg";
};