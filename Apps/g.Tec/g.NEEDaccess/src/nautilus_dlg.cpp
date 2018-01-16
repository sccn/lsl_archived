#include "nautilus_dlg.h"
#include "ui_nautilus_dlg.h"

NautilusDlg::NautilusDlg(QWidget *parent)
	: QDialog(parent),
	  ui(new Ui::NautilusDlg)
{
	ui->setupUi(this);
}