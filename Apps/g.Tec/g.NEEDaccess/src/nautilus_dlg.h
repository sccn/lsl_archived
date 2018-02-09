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

private:
	void update_ui();
	void accept() override;
};