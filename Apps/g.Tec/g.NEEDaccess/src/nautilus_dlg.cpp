#include "nautilus_dlg.h"
#include "ui_nautilus_dlg.h"
#include "GDSClientAPI.h"
#include "GDSClientAPI_gNautilus.h"

NautilusDlg::NautilusDlg(QWidget *parent)
	: QDialog(parent),
	  ui(new Ui::NautilusDlg)
{
	ui->setupUi(this);
}


void NautilusDlg::set_config(void* config)
{
	m_device_config = config;
	update_ui();
}


void NautilusDlg::update_ui()
{
	GDS_GNAUTILUS_CONFIGURATION* cfg = (GDS_GNAUTILUS_CONFIGURATION*)m_device_config;
	ui->CARCheckBox->setChecked(cfg->CAR);
	ui->noiseCheckBox->setChecked(cfg->NoiseReduction);
	ui->accelCheckBox->setChecked(cfg->AccelerationData);
	ui->counterCheckBox->setChecked(cfg->Counter);
	ui->linkCheckBox->setChecked(cfg->LinkQualityInformation);
	ui->batteryCheckBox->setChecked(cfg->BatteryLevel);
	ui->digitalCheckBox->setChecked(cfg->DigitalIOs);
	ui->validationCheckBox->setChecked(cfg->ValidationIndicator);
}

void NautilusDlg::accept()
{
	GDS_GNAUTILUS_CONFIGURATION* cfg = (GDS_GNAUTILUS_CONFIGURATION*)m_device_config;
	cfg->CAR = ui->CARCheckBox->isChecked();
	cfg->NoiseReduction = ui->noiseCheckBox->isChecked();
	cfg->AccelerationData = ui->accelCheckBox->isChecked();
	cfg->Counter = ui->counterCheckBox->isChecked();
	cfg->LinkQualityInformation = ui->linkCheckBox->isChecked();
	cfg->BatteryLevel = ui->batteryCheckBox->isChecked();
	cfg->DigitalIOs = ui->digitalCheckBox->isChecked();
	cfg->ValidationIndicator = ui->validationCheckBox->isChecked();
	QDialog::accept();
}

/*
if (elname == "noise-reduction")
ui->noiseCheckBox->setChecked(xmlReader.readElementText().compare("true", Qt::CaseInsensitive) == 0);
else if (elname == "car")
ui->CARCheckBox->setChecked(xmlReader->readElementText().compare("true", Qt::CaseInsensitive) == 0);
else if (elname == "acceleration-data")
ui->accelCheckBox->setChecked(xmlReader->readElementText().compare("true", Qt::CaseInsensitive) == 0);
else if (elname == "counter")
ui->counterCheckBox->setChecked(xmlReader->readElementText().compare("true", Qt::CaseInsensitive) == 0);
else if (elname == "link-quality")
ui->linkCheckBox->setChecked(xmlReader->readElementText().compare("true", Qt::CaseInsensitive) == 0);
else if (elname == "battery-level")
ui->batteryCheckBox->setChecked(xmlReader->readElementText().compare("true", Qt::CaseInsensitive) == 0);
else if (elname == "digital-inputs")
ui->digitalCheckBox->setChecked(xmlReader->readElementText().compare("true", Qt::CaseInsensitive) == 0);
else if (elname == "validation-indicator")
ui->validationCheckBox->setChecked(xmlReader->readElementText().compare("true", Qt::CaseInsensitive) == 0);
*/