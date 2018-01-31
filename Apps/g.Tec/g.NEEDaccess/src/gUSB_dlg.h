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

	void set_configs(std::vector<GDS_CONFIGURATION_BASE>);

	Ui::GUSBDlg *ui;
	void* m_device_config;

private:
	void create_widgets();
	void update_ui();
	void accept() override;

	std::vector<GDS_CONFIGURATION_BASE> m_configs;
	const std::vector<uint32_t> gUSBamp_sample_rates = { 32, 64, 128, 256, 512, 600, 1200, 2400, 4800, 9600, 19200, 38400 };
};