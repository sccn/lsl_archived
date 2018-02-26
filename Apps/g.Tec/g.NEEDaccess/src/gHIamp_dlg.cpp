#include "gHIamp_dlg.h"
#include "ui_gHIamp_dlg.h"
#include "GDSClientAPI.h"
#include "GDSClientAPI_gHIamp.h"
#include <QtWidgets>
#include <algorithm>


GHIampDlg::GHIampDlg(QWidget *parent)
	: QDialog(parent),
	  ui(new Ui::GHIampDlg)
{
	ui->setupUi(this);
}


void GHIampDlg::set_config(GDS_HANDLE* connection_handle, GDS_CONFIGURATION_BASE config, std::vector<std::string>* chan_labels, std::vector<double>* impedances)
{
	m_pHandle = connection_handle;
	m_config = config;
	m_config.Configuration = config.Configuration;
	m_pChannel_labels = chan_labels;
	m_pChannel_impedances = impedances;
	create_widgets();
	update_ui();

	load_config(default_config_fname);
}


void GHIampDlg::create_widgets()
{
	ui->dev_label->setText(m_config.DeviceInfo.Name);

	ui->bipolar_spinBox->setRange(0, GDS_GHIAMP_CHANNELS_MAX);
	
	// Create table entries for each channel
	QTableWidget *chan_table = ui->channels_tableWidget;
	for (int chan_ix = 0; chan_ix < GDS_GHIAMP_CHANNELS_MAX; chan_ix++)
	{
		chan_table->insertRow(chan_table->rowCount());
		if (m_pChannel_labels->size() > chan_ix)
		{
			chan_table->setItem(chan_ix, 0, new QTableWidgetItem(QString::fromStdString(m_pChannel_labels->at(chan_ix))));
		}
		else
		{
			chan_table->setItem(chan_ix, 0, new QTableWidgetItem(tr("%1").arg(chan_ix + 1)));
		}
		chan_table->setCellWidget(chan_ix, 1, new QCheckBox());  // Acquire
		chan_table->setCellWidget(chan_ix, 2, new QSpinBox());   // Bipolar
		chan_table->setCellWidget(chan_ix, 3, new QSpinBox());   // Bandpass
		chan_table->setCellWidget(chan_ix, 4, new QSpinBox());   // Notch
		chan_table->setItem(chan_ix, 5, new QTableWidgetItem(tr("?")));
	}

	// TODO: Use GDS_GHIAMP_GetSupportedSamplingRates instead of gHIamp_sample_rates
	ui->samplerate_comboBox->clear();
	for each (uint32_t srate in gHIamp_sample_rates)
	{
		ui->samplerate_comboBox->addItem(QString::number(srate));
	}
}


void GHIampDlg::update_ui()
{
	// Update ui with config contents
	GDS_GHIAMP_CONFIGURATION* dev_cfg = (GDS_GHIAMP_CONFIGURATION*)m_config.Configuration;
	ui->samplerate_comboBox->setCurrentIndex(std::find(gHIamp_sample_rates.begin(), gHIamp_sample_rates.end(), dev_cfg->SamplingRate) - gHIamp_sample_rates.begin());
	ui->counter_checkBox->setChecked(dev_cfg->CounterEnabled);
	ui->trigger_checkBox->setChecked(dev_cfg->TriggerLinesEnabled);
	ui->hold_checkBox->setChecked(dev_cfg->HoldEnabled);

	// Update Channels ranges
	for (int chan_ix = 0; chan_ix < GDS_GHIAMP_CHANNELS_MAX; chan_ix++)
	{
		QCheckBox *chan_acquire = (QCheckBox*)ui->channels_tableWidget->cellWidget(chan_ix, 1);
		chan_acquire->setChecked(dev_cfg->Channels[chan_ix].Acquire);
		QSpinBox* chan_bipolar = (QSpinBox*)ui->channels_tableWidget->cellWidget(chan_ix, 2);
		chan_bipolar->setRange(0, GDS_GHIAMP_CHANNELS_MAX);
		chan_bipolar->setValue(dev_cfg->Channels[chan_ix].ReferenceChannel);
	}

	update_filters();
	update_impedance_table();
}


void GHIampDlg::update_filters()
{
	// Update ui with config contents
	size_t bandpassFiltersCount;
	size_t notchFiltersCount;

	GDS_GHIAMP_CONFIGURATION* dev_cfg = (GDS_GHIAMP_CONFIGURATION*)m_config.Configuration;

	char(*device_name)[DEVICE_NAME_LENGTH_MAX] = new char[1][DEVICE_NAME_LENGTH_MAX];
	std::strcpy(device_name[0], m_config.DeviceInfo.Name);

	// Fill bandpass combobox
	GDS_RESULT res = GDS_GHIAMP_GetBandpassFilters(*m_pHandle, device_name, NULL, &bandpassFiltersCount);
	std::vector<GDS_FILTER_INFO> bandpassFilters(bandpassFiltersCount);
	res = GDS_GHIAMP_GetBandpassFilters(*m_pHandle, device_name, bandpassFilters.data(), &bandpassFiltersCount);
	ui->bandpass_comboBox->clear();
	for (size_t bp_ix = 0; bp_ix < bandpassFiltersCount; bp_ix++)
	{
		ui->bandpass_comboBox->addItem(QString("%1 - %2").arg(
			QString::number(bandpassFilters[bp_ix].LowerCutoffFrequency),
			QString::number(bandpassFilters[bp_ix].UpperCutoffFrequency)));
	}
	// Disable items that don't work with currently selected frequency.
	QStandardItemModel* bandpass_model = qobject_cast<QStandardItemModel*>(ui->bandpass_comboBox->model());
	for (size_t bp_ix = 0; bp_ix < bandpassFiltersCount; bp_ix++)
	{
		QStandardItem* bandpass_item = bandpass_model->item((int)bp_ix);
		bandpass_item->setFlags(bandpassFilters[bp_ix].SamplingRate == gHIamp_sample_rates[ui->samplerate_comboBox->currentIndex()] ?
			bandpass_item->flags() | Qt::ItemIsEnabled : bandpass_item->flags() & ~Qt::ItemIsEnabled);
	}

	// Fill notch combobox
	res = GDS_GHIAMP_GetNotchFilters(*m_pHandle, device_name, NULL, &notchFiltersCount);
	std::vector<GDS_FILTER_INFO> notchFilters(notchFiltersCount);
	res = GDS_GHIAMP_GetNotchFilters(*m_pHandle, device_name, notchFilters.data(), &notchFiltersCount);
	ui->notch_comboBox->clear();
	for (size_t notch_ix = 0; notch_ix < notchFiltersCount; notch_ix++)
	{
		ui->notch_comboBox->addItem(QString("%1 - %2").arg(
			QString::number(notchFilters[notch_ix].LowerCutoffFrequency),
			QString::number(notchFilters[notch_ix].UpperCutoffFrequency)));
	}
	QStandardItemModel* notch_model = qobject_cast<QStandardItemModel*>(ui->notch_comboBox->model());
	for (size_t notch_ix = 0; notch_ix < notchFiltersCount; notch_ix++)
	{
		QStandardItem* notch_item = notch_model->item((int)notch_ix);
		notch_item->setFlags(notchFilters[notch_ix].SamplingRate == gHIamp_sample_rates[ui->samplerate_comboBox->currentIndex()] ?
			notch_item->flags() | Qt::ItemIsEnabled : notch_item->flags() & ~Qt::ItemIsEnabled);
	}
	delete[] device_name;
	device_name = NULL;

	// Update Channels ranges
	for (int chan_ix = 0; chan_ix < GDS_GHIAMP_CHANNELS_MAX; chan_ix++)
	{
		QSpinBox* chan_bandpass = (QSpinBox*)ui->channels_tableWidget->cellWidget(chan_ix, 3);
		chan_bandpass->setRange(-1, (int)bandpassFiltersCount - 1);
		chan_bandpass->setValue(dev_cfg->Channels[chan_ix].BandpassFilterIndex);  // TODO: Set first enabled value
		QSpinBox* chan_notch = (QSpinBox*)ui->channels_tableWidget->cellWidget(chan_ix, 4);
		chan_notch->setRange(-1, (int)notchFiltersCount - 1);
		chan_notch->setValue(dev_cfg->Channels[chan_ix].NotchFilterIndex);  // TODO: Set first enabled value.
	}
}


void GHIampDlg::update_impedance_table()
{
	for (int chan_ix = 0; chan_ix < GDS_GHIAMP_CHANNELS_MAX; chan_ix++)
	{
		if (m_pChannel_impedances->size() > chan_ix)
		{
			ui->channels_tableWidget->item(chan_ix, 5)->setText(QString::number(m_pChannel_impedances->at(chan_ix)));
		}
	}
}


void GHIampDlg::on_loadCfgButton_clicked()
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


void GHIampDlg::load_config(const QString filename)
{
	QFile xmlFile(filename);
	if (!xmlFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
		qDebug() << "Could not load XML from file " << filename;
		return;
	}
	QXmlStreamReader xmlReader(&xmlFile);
	int channel_ix = -1;
	bool label_is_channel = false;

	while (!xmlReader.atEnd() && !xmlReader.hasError()) {
		xmlReader.readNext();
		if (xmlReader.isStartElement())
		{
			QStringRef elname = xmlReader.name();
			if (elname == "device")
			{
				label_is_channel = false;
			}
			else if (elname == "IsActive")
			{
				ui->active_checkBox->setChecked(xmlReader.readElementText().compare("true", Qt::CaseInsensitive) == 0);
			}
			else if (elname == "SampleRate")
			{
				ui->samplerate_comboBox->setCurrentIndex(std::find(gHIamp_sample_rates.begin(), gHIamp_sample_rates.end(),
					xmlReader.readElementText().toInt()) - gHIamp_sample_rates.begin());
			}
			else if (elname == "CounterEnabled")
			{
				ui->counter_checkBox->setChecked(xmlReader.readElementText().compare("true", Qt::CaseInsensitive) == 0);
			}
			else if (elname == "TriggerEnabled")
			{
				ui->trigger_checkBox->setChecked(xmlReader.readElementText().compare("true", Qt::CaseInsensitive) == 0);
			}
			else if (elname == "HoldEnabled")
			{
				ui->hold_checkBox->setChecked(xmlReader.readElementText().compare("true", Qt::CaseInsensitive) == 0);
			}
			else if (elname == "channels")
				channel_ix = -1;
			else if (elname == "channel")
			{
				channel_ix++;
				label_is_channel = true;
			}
			else if (elname == "Label" && label_is_channel)
			{
				QTableWidgetItem *chan_label = ui->channels_tableWidget->item(channel_ix, 0);
				chan_label->setText(xmlReader.readElementText());
			}
			else if (elname == "Acquire")
			{
				QCheckBox *chan_acquire = (QCheckBox*)ui->channels_tableWidget->cellWidget(channel_ix, 1);
				chan_acquire->setChecked(xmlReader.readElementText().compare("true", Qt::CaseInsensitive) == 0);
			}
			else if (elname == "BipolarChannel")
			{
				QSpinBox* chan_bipolar = (QSpinBox*)ui->channels_tableWidget->cellWidget(channel_ix, 2);
				chan_bipolar->setValue(xmlReader.readElementText().toInt());
			}
			else if (elname == "BandpassFilterIndex")
			{
				QSpinBox* chan_bandpass = (QSpinBox*)ui->channels_tableWidget->cellWidget(channel_ix, 3);
				chan_bandpass->setValue(xmlReader.readElementText().toInt());
			}
			else if (elname == "NotchFilterIndex")
			{
				QSpinBox* chan_notch = (QSpinBox*)ui->channels_tableWidget->cellWidget(channel_ix, 4);
				chan_notch->setValue(xmlReader.readElementText().toInt());
			}
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


void GHIampDlg::on_saveCfgButton_clicked()
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


void GHIampDlg::save_config(QString filename)
{
	try {
		QFile file(filename);
		file.open(QIODevice::WriteOnly);
		QXmlStreamWriter stream(&file);
		stream.setAutoFormatting(true);
		stream.writeStartDocument();

		// Save GUI state of device + channel config.
		stream.writeStartElement("device");
		stream.writeTextElement("Label", ui->dev_label->text());
		stream.writeTextElement("IsActive", ui->active_checkBox->isChecked() ? "true" : "false");
		stream.writeTextElement("SampleRate", QString::number(gHIamp_sample_rates[ui->samplerate_comboBox->currentIndex()]));
		stream.writeTextElement("HoldEnabled", ui->hold_checkBox->isChecked() ? "true" : "false");
		stream.writeTextElement("CounterEnabled", ui->counter_checkBox->isChecked() ? "true" : "false");
		stream.writeTextElement("TriggerEnabled", ui->trigger_checkBox->isChecked() ? "true" : "false");

		stream.writeStartElement("channels");
		for (int chan_ix = 0; chan_ix < GDS_GHIAMP_CHANNELS_MAX; chan_ix++)
		{
			stream.writeStartElement("channel");
			QTableWidgetItem *chan_label = ui->channels_tableWidget->item(chan_ix, 0);
			stream.writeTextElement("Label", chan_label->text());
			QCheckBox *chan_acquire = (QCheckBox*)ui->channels_tableWidget->cellWidget(chan_ix, 1);
			stream.writeTextElement("Acquire", chan_acquire->isChecked() ? "true" : "false");
			QSpinBox* chan_bipolar = (QSpinBox*)ui->channels_tableWidget->cellWidget(chan_ix, 2);
			stream.writeTextElement("BipolarChannel", QString::number(chan_bipolar->value()));
			QSpinBox* chan_bandpass = (QSpinBox*)ui->channels_tableWidget->cellWidget(chan_ix, 3);
			stream.writeTextElement("BandpassFilterIndex", QString::number(chan_bandpass->value()));
			QSpinBox* chan_notch = (QSpinBox*)ui->channels_tableWidget->cellWidget(chan_ix, 4);
			stream.writeTextElement("NotchFilterIndex", QString::number(chan_notch->value()));
			stream.writeEndElement(); // channel
		}
		stream.writeEndElement(); // channels
		stream.writeEndElement(); // device
		stream.writeEndDocument();
		file.close();
	}
	catch (std::exception &e) {
		qDebug() << "Problem saving to config file: " << e.what();
	}
}


void GHIampDlg::on_samplerate_comboBox_currentIndexChanged(int index)
{
	update_filters();
}


void GHIampDlg::on_impedance_pushButton_clicked()
{
	char(*device_name)[DEVICE_NAME_LENGTH_MAX] = new char[1][DEVICE_NAME_LENGTH_MAX];
	std::strcpy(device_name[0], m_config.DeviceInfo.Name);

	BOOL(*isActiveElectrode)[GDS_GHIAMP_CHANNELS_MAX] = new BOOL[1][GDS_GHIAMP_CHANNELS_MAX];
	for (size_t chan_ix = 0; chan_ix < GDS_GHIAMP_CHANNELS_MAX; chan_ix++)
	{
		isActiveElectrode[0][chan_ix] = ui->active_checkBox->isChecked();
	}

	double(*impedances)[GDS_GHIAMP_CHANNELS_MAX] = new double[1][GDS_GHIAMP_CHANNELS_MAX];
	
	GDS_RESULT res = GDS_GHIAMP_GetImpedance(*m_pHandle, device_name, isActiveElectrode, impedances);
	m_pChannel_impedances->clear();
	for (size_t chan_ix = 0; chan_ix < GDS_GHIAMP_CHANNELS_MAX; chan_ix++)
	{
		m_pChannel_impedances->push_back(impedances[0][chan_ix]);
	}
	delete[] device_name;
	device_name = NULL;
	delete[] isActiveElectrode;
	isActiveElectrode = NULL;
	delete[] impedances;
	impedances = NULL;

	update_impedance_table();
}


void GHIampDlg::on_bipolar_pushButton_clicked()
{
	apply_value_to_enabled_chans(ui->bipolar_spinBox->value(), 2);
}


void GHIampDlg::on_bandpass_pushButton_clicked()
{
	apply_value_to_enabled_chans(ui->bandpass_comboBox->currentIndex(), 3);
}


void GHIampDlg::on_notch_pushButton_clicked()
{
	apply_value_to_enabled_chans(ui->notch_comboBox->currentIndex(), 4);
}


void GHIampDlg::apply_value_to_enabled_chans(int value, int widget_ix)
{
	GDS_GHIAMP_CONFIGURATION* dev_cfg = (GDS_GHIAMP_CONFIGURATION*)m_config.Configuration;
for (int chan_ix = 0; chan_ix < GDS_GHIAMP_CHANNELS_MAX; chan_ix++)
	{
		QCheckBox *chan_acquire = (QCheckBox*)ui->channels_tableWidget->cellWidget(chan_ix, 1);
		if (chan_acquire->isChecked())
		{
			QSpinBox* chan_spbox = (QSpinBox*)ui->channels_tableWidget->cellWidget(chan_ix, widget_ix);
			chan_spbox->setValue(value);
		}
	}
}


void GHIampDlg::accept()
{
	GDS_GHIAMP_CONFIGURATION* dev_cfg = (GDS_GHIAMP_CONFIGURATION*)m_config.Configuration;
	dev_cfg->SamplingRate = gHIamp_sample_rates[ui->samplerate_comboBox->currentIndex()];
	dev_cfg->CounterEnabled = ui->counter_checkBox->isChecked();
	dev_cfg->TriggerLinesEnabled = ui->trigger_checkBox->isChecked();
	dev_cfg->HoldEnabled = ui->hold_checkBox->isChecked();

	m_pChannel_labels->clear();
	for (int chan_ix = 0; chan_ix < GDS_GHIAMP_CHANNELS_MAX; chan_ix++)
	{
		QTableWidgetItem *chan_label = ui->channels_tableWidget->item(chan_ix, 0);
		m_pChannel_labels->push_back(chan_label->text().toStdString());
		QCheckBox *chan_acquire = (QCheckBox*)ui->channels_tableWidget->cellWidget(chan_ix, 1);
		dev_cfg->Channels[chan_ix].Acquire = chan_acquire->isChecked();
		QSpinBox* chan_bipolar = (QSpinBox*)ui->channels_tableWidget->cellWidget(chan_ix, 2);
		dev_cfg->Channels[chan_ix].ReferenceChannel = chan_bipolar->value();
		QSpinBox* chan_bandpass = (QSpinBox*)ui->channels_tableWidget->cellWidget(chan_ix, 3);
		dev_cfg->Channels[chan_ix].BandpassFilterIndex = chan_bandpass->value();
		QSpinBox* chan_notch = (QSpinBox*)ui->channels_tableWidget->cellWidget(chan_ix, 4);
		dev_cfg->Channels[chan_ix].NotchFilterIndex = chan_notch->value();
	}

	QDialog::accept();
}
