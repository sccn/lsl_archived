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

	Ui::NautilusDlg *ui;
};