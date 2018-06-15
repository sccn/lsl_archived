#pragma once
#include <QDialog>
#include "GDSClientAPI.h"


namespace Ui {
	class NautilusDlg;
}


class NautilusDlg : public QDialog
{
	Q_OBJECT
public:
	explicit NautilusDlg(QWidget *parent = 0);

	void set_config(GDS_HANDLE* connection_handle, GDS_CONFIGURATION_BASE config, std::vector<std::string>* chan_labels, std::vector<double>* impedances);

	Ui::NautilusDlg *ui;

private slots:
	void on_loadCfgButton_clicked();
	void on_saveCfgButton_clicked();
	void on_impedance_pushButton_clicked();
	void on_samplerate_comboBox_currentIndexChanged(int index);
	void on_networkchan_comboBox_currentIndexChanged(int index);
	void on_bipolar_pushButton_clicked();
	void on_bandpass_pushButton_clicked();
	void on_inputrange_pushButton_clicked();
	void on_notch_pushButton_clicked();
	
private:
	void create_widgets();
	void update_ui();
	void update_filters();
	void accept() override;
	void save_config(QString filename);
	void load_config(const QString filename);
	void apply_value_to_enabled_chans(int value, int widget_ix);
	void update_impedance_table();

	GDS_CONFIGURATION_BASE m_config;
	GDS_HANDLE* m_pHandle;
	std::vector<std::string>* m_pChannel_labels;
	std::vector<double>* m_pChannel_impedances;
	std::vector<uint32_t> gnautilus_sample_rates = { 0 };
	std::vector<uint32_t> gnautilus_network_channels = { 0 };
	std::vector<double> gnautilus_sensitivities = { 0 };

	const QString default_config_fname = "gNautilus_default.cfg";
};