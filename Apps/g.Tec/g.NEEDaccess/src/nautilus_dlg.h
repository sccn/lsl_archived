#pragma once
#include <QDialog>


namespace Ui {
	class NautilusDlg;
}


class NautilusDlg : public QDialog
{
	Q_OBJECT
public:
	explicit NautilusDlg(QWidget *parent = 0);

	void set_config(void* config);

	Ui::NautilusDlg *ui;
	void* m_device_config;

private slots:
	void on_loadCfgButton_clicked();
	void on_saveCfgButton_clicked();
private:
	void update_ui();
	void accept() override;
	void save_config(QString filename);
	void load_config(const QString filename);

	const QString default_config_fname = "gNautilus_default.cfg";
};