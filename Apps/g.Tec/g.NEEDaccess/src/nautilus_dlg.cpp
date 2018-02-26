#include "nautilus_dlg.h"
#include "ui_nautilus_dlg.h"
#include "GDSClientAPI.h"
#include "GDSClientAPI_gNautilus.h"
#include <QtWidgets>

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

	load_config(default_config_fname);
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


void NautilusDlg::on_loadCfgButton_clicked()
{
	QString sel = QFileDialog::getOpenFileName(this,
		"Load Configuration File",
		"",//QDir::currentPath()
		"Configuration Files (*.cfg)");
	if (!sel.isEmpty())
	{
		load_config(sel);
	}
}


void NautilusDlg::load_config(const QString filename)
{
	QFile xmlFile(filename);
	if (!xmlFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
		qDebug() << "Could not load XML from file " << filename;
		return;
	}
	QXmlStreamReader xmlReader(&xmlFile);

	while (!xmlReader.atEnd() && !xmlReader.hasError()) {
		xmlReader.readNext();
		if (xmlReader.isStartElement())
		{
			QStringRef elname = xmlReader.name();
			if (elname == "noise-reduction")
				ui->noiseCheckBox->setChecked(xmlReader.readElementText().compare("true", Qt::CaseInsensitive) == 0);
			else if (elname == "car")
				ui->CARCheckBox->setChecked(xmlReader.readElementText().compare("true", Qt::CaseInsensitive) == 0);
			else if (elname == "acceleration-data")
				ui->accelCheckBox->setChecked(xmlReader.readElementText().compare("true", Qt::CaseInsensitive) == 0);
			else if (elname == "counter")
				ui->counterCheckBox->setChecked(xmlReader.readElementText().compare("true", Qt::CaseInsensitive) == 0);
			else if (elname == "link-quality")
				ui->linkCheckBox->setChecked(xmlReader.readElementText().compare("true", Qt::CaseInsensitive) == 0);
			else if (elname == "battery-level")
				ui->batteryCheckBox->setChecked(xmlReader.readElementText().compare("true", Qt::CaseInsensitive) == 0);
			else if (elname == "digital-inputs")
				ui->digitalCheckBox->setChecked(xmlReader.readElementText().compare("true", Qt::CaseInsensitive) == 0);
			else if (elname == "validation-indicator")
				ui->validationCheckBox->setChecked(xmlReader.readElementText().compare("true", Qt::CaseInsensitive) == 0);
		}
	}
	if (xmlReader.hasError()) {
		qDebug() << "Config file parse error "
			<< xmlReader.error()
			<< ": "
			<< xmlReader.errorString();
	}
	xmlReader.clear();
	xmlFile.close();
}


void NautilusDlg::on_saveCfgButton_clicked()
{
	QString sel = QFileDialog::getSaveFileName(this,
		"Save Configuration File",
		QDir::currentPath(),
		"Configuration Files (*.cfg)");
	if (!sel.isEmpty())
	{
		save_config(sel);
	}
}


void NautilusDlg::save_config(QString filename)
{
	try {
		QFile file(filename);
		file.open(QIODevice::WriteOnly);
		QXmlStreamWriter stream(&file);
		stream.setAutoFormatting(true);
		stream.writeStartDocument();

		// Save GUI state of device + channel config.
		stream.writeStartElement("device");
		stream.writeTextElement("noise-reduction", ui->noiseCheckBox->isChecked() ? "true" : "false");
		stream.writeTextElement("car", ui->CARCheckBox->isChecked() ? "true" : "false");
		stream.writeTextElement("acceleration-data", ui->accelCheckBox->isChecked() ? "true" : "false");
		stream.writeTextElement("counter", ui->counterCheckBox->isChecked() ? "true" : "false");
		stream.writeTextElement("link-quality", ui->linkCheckBox->isChecked() ? "true" : "false");
		stream.writeTextElement("battery-level", ui->batteryCheckBox->isChecked() ? "true" : "false");
		stream.writeTextElement("digital-inputs", ui->digitalCheckBox->isChecked() ? "true" : "false");
		stream.writeTextElement("validation-indicator", ui->validationCheckBox->isChecked() ? "true" : "false");
		stream.writeEndElement(); // device
		stream.writeEndDocument();
		file.close();
	}
	catch (std::exception &e) {
		qDebug() << "Problem saving to config file: " << e.what();
	}
}