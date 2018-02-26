#pragma once
#include <QDialog>
#include "GDSClientAPI.h"


namespace Ui {
	class GHIampDlg;
}


class GHIampDlg : public QDialog
{
	Q_OBJECT
public:
	explicit GHIampDlg(QWidget *parent = 0);

	void set_config(GDS_HANDLE*, GDS_CONFIGURATION_BASE, std::vector<std::string>*, std::vector<double>*);

	Ui::GHIampDlg *ui;
	void* m_device_config;

private slots:
	void on_loadCfgButton_clicked();
	void on_saveCfgButton_clicked();
	void on_samplerate_comboBox_currentIndexChanged(int index);
	void on_bandpass_pushButton_clicked();
	void on_notch_pushButton_clicked();
	void on_bipolar_pushButton_clicked();
	void on_impedance_pushButton_clicked();
private:
	void create_widgets();
	void update_ui();
	void update_filters();
	void update_impedance_table();
	void accept() override;
	void load_config(const QString filename);
	void save_config(QString filename);
	void apply_value_to_enabled_chans(int value, int widget_ix);

	GDS_CONFIGURATION_BASE m_config;
	GDS_HANDLE* m_pHandle;
	std::vector<std::string>* m_pChannel_labels;
	std::vector<double>* m_pChannel_impedances;  // TOOD: Use GDS_GUSBAMP_GetImpedance to fill this.
	const std::vector<uint32_t> gHIamp_sample_rates = { 32, 64, 128, 256, 512, 600, 1200, 2400, 4800, 9600, 19200, 38400 };

	const QString default_config_fname = "gHIamp_default.cfg";
};