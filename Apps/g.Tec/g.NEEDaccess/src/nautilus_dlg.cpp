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


void NautilusDlg::set_config(GDS_HANDLE* connection_handle, GDS_CONFIGURATION_BASE config, std::vector<std::string>* chan_labels, std::vector<double>* impedances)
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


void NautilusDlg::create_widgets()
{
	ui->dev_label->setText(m_config.DeviceInfo.Name);

	ui->bipolar_spinBox->setRange(0, GDS_GNAUTILUS_CHANNELS_MAX);

	// Create table entries for each channel
	QTableWidget *chan_table = ui->channels_tableWidget;
	for (int chan_ix = 0; chan_ix < GDS_GNAUTILUS_CHANNELS_MAX; chan_ix++)
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
		chan_table->setCellWidget(chan_ix, 2, new QSpinBox());   // Range
		chan_table->setCellWidget(chan_ix, 3, new QSpinBox());   // Bipolar
		chan_table->setCellWidget(chan_ix, 4, new QSpinBox());   // Bandpass
		chan_table->setCellWidget(chan_ix, 5, new QSpinBox());   // Notch
		chan_table->setCellWidget(chan_ix, 6, new QCheckBox());  // CAR
		chan_table->setCellWidget(chan_ix, 7, new QCheckBox());  // Noise Reduction
		chan_table->setItem(chan_ix, 8, new QTableWidgetItem(tr("?")));  // Impedance
	}

	// Get device_name for API calls
	char(*device_name)[DEVICE_NAME_LENGTH_MAX] = new char[1][DEVICE_NAME_LENGTH_MAX];
	std::strcpy(device_name[0], m_config.DeviceInfo.Name);

	// Get Sampling Rates
	size_t supportedSamplingRatesCount;
	GDS_RESULT res = GDS_GNAUTILUS_GetSupportedSamplingRates(*m_pHandle, device_name, NULL, &supportedSamplingRatesCount);
	gnautilus_sample_rates.resize(supportedSamplingRatesCount);
	res = GDS_GNAUTILUS_GetSupportedSamplingRates(*m_pHandle, device_name, gnautilus_sample_rates.data(), &supportedSamplingRatesCount);
	ui->samplerate_comboBox->clear();
	for each (uint32_t srate in gnautilus_sample_rates)
	{
		ui->samplerate_comboBox->addItem(QString::number(srate));
	}

	// Get Network Channels
	ui->networkchan_comboBox->clear();
	size_t supportedNetworkChannelsCount;
	res = GDS_GNAUTILUS_GetSupportedNetworkChannels(*m_pHandle, device_name, NULL, &supportedNetworkChannelsCount);
	gnautilus_network_channels.resize(supportedNetworkChannelsCount);
	res = GDS_GNAUTILUS_GetSupportedNetworkChannels(*m_pHandle, device_name, gnautilus_network_channels.data(), &supportedNetworkChannelsCount);
	for each (uint32_t networkchan in gnautilus_network_channels)
	{
		ui->networkchan_comboBox->addItem(QString::number(networkchan));
	}

	// Get Supported Sensitivities
	ui->inputrange_comboBox->clear();
	size_t supportedSensitivitiesCount;
	res = GDS_GNAUTILUS_GetSupportedSensitivities(*m_pHandle, device_name, NULL, &supportedSensitivitiesCount);
	gnautilus_sensitivities.resize(supportedNetworkChannelsCount);
	res = GDS_GNAUTILUS_GetSupportedSensitivities(*m_pHandle, device_name, gnautilus_sensitivities.data(), &supportedSensitivitiesCount);
	for each (double sensitivity in gnautilus_sensitivities)
	{
		ui->inputrange_comboBox->addItem(QString::number(sensitivity));
	}
	
	delete[] device_name;
	device_name = NULL;
}


void NautilusDlg::update_ui()
{
	GDS_GNAUTILUS_CONFIGURATION* dev_cfg = (GDS_GNAUTILUS_CONFIGURATION*)m_config.Configuration;

	ui->samplerate_comboBox->setCurrentIndex(std::find(gnautilus_sample_rates.begin(), gnautilus_sample_rates.end(), dev_cfg->SamplingRate) - gnautilus_sample_rates.begin());
	ui->networkchan_comboBox->setCurrentIndex(std::find(gnautilus_network_channels.begin(), gnautilus_network_channels.end(), dev_cfg->NetworkChannel) - gnautilus_network_channels.begin());
	ui->CARCheckBox->setChecked(dev_cfg->CAR);
	ui->noiseCheckBox->setChecked(dev_cfg->NoiseReduction);
	ui->accelCheckBox->setChecked(dev_cfg->AccelerationData);
	ui->counterCheckBox->setChecked(dev_cfg->Counter);
	ui->linkCheckBox->setChecked(dev_cfg->LinkQualityInformation);
	ui->batteryCheckBox->setChecked(dev_cfg->BatteryLevel);
	ui->digitalCheckBox->setChecked(dev_cfg->DigitalIOs);
	ui->validationCheckBox->setChecked(dev_cfg->ValidationIndicator);

	// Update table values for a few columns (those which are not sampling rate dependent)
	for (int chan_ix = 0; chan_ix < GDS_GNAUTILUS_CHANNELS_MAX; chan_ix++)
	{
		QCheckBox *chan_acquire = (QCheckBox*)ui->channels_tableWidget->cellWidget(chan_ix, 1);
		chan_acquire->setChecked(dev_cfg->Channels[chan_ix].Enabled);

		QSpinBox* chan_range = (QSpinBox*)ui->channels_tableWidget->cellWidget(chan_ix, 2);
		chan_range->setRange(0, gnautilus_sensitivities.size());
		chan_range->setValue(std::find(gnautilus_sensitivities.begin(), gnautilus_sensitivities.end(),
			dev_cfg->Channels[chan_ix].Sensitivity) - gnautilus_sensitivities.begin());
		
		QSpinBox* chan_bipolar = (QSpinBox*)ui->channels_tableWidget->cellWidget(chan_ix, 3);
		chan_bipolar->setRange(0, GDS_GNAUTILUS_CHANNELS_MAX);
		chan_bipolar->setValue(dev_cfg->Channels[chan_ix].BipolarChannel);

		QCheckBox *chan_car = (QCheckBox*)ui->channels_tableWidget->cellWidget(chan_ix, 6);
		chan_car->setChecked(dev_cfg->Channels[chan_ix].UsedForCar);

		QCheckBox *chan_noisered = (QCheckBox*)ui->channels_tableWidget->cellWidget(chan_ix, 7);
		chan_noisered->setChecked(dev_cfg->Channels[chan_ix].UsedForNoiseReduction);
	}

	update_filters();  // Separate func because it is sampling-rate dependent.
}


void NautilusDlg::update_filters()
{
	// Update ui with config contents
	size_t bandpassFiltersCount;
	size_t notchFiltersCount;

	GDS_GNAUTILUS_CONFIGURATION* dev_cfg = (GDS_GNAUTILUS_CONFIGURATION*)m_config.Configuration;

	char(*device_name)[DEVICE_NAME_LENGTH_MAX] = new char[1][DEVICE_NAME_LENGTH_MAX];
	std::strcpy(device_name[0], m_config.DeviceInfo.Name);

	// Fill bandpass combobox
	GDS_RESULT res = GDS_GNAUTILUS_GetBandpassFilters(*m_pHandle, device_name, NULL, &bandpassFiltersCount);
	std::vector<GDS_FILTER_INFO> bandpassFilters(bandpassFiltersCount);
	res = GDS_GNAUTILUS_GetBandpassFilters(*m_pHandle, device_name, bandpassFilters.data(), &bandpassFiltersCount);
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
		bandpass_item->setFlags(bandpassFilters[bp_ix].SamplingRate == gnautilus_sample_rates[ui->samplerate_comboBox->currentIndex()] ?
			bandpass_item->flags() | Qt::ItemIsEnabled : bandpass_item->flags() & ~Qt::ItemIsEnabled);
	}

	// Fill notch combobox
	res = GDS_GNAUTILUS_GetNotchFilters(*m_pHandle, device_name, NULL, &notchFiltersCount);
	std::vector<GDS_FILTER_INFO> notchFilters(notchFiltersCount);
	res = GDS_GNAUTILUS_GetNotchFilters(*m_pHandle, device_name, notchFilters.data(), &notchFiltersCount);
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
		notch_item->setFlags(notchFilters[notch_ix].SamplingRate == gnautilus_sample_rates[ui->samplerate_comboBox->currentIndex()] ?
			notch_item->flags() | Qt::ItemIsEnabled : notch_item->flags() & ~Qt::ItemIsEnabled);
	}
	delete[] device_name;
	device_name = NULL;

	// Update Channels' filters ranges and values
	for (int chan_ix = 0; chan_ix < GDS_GNAUTILUS_CHANNELS_MAX; chan_ix++)
	{
		QSpinBox* chan_bandpass = (QSpinBox*)ui->channels_tableWidget->cellWidget(chan_ix, 4);
		chan_bandpass->setRange(-1, (int)bandpassFiltersCount - 1);
		chan_bandpass->setValue(dev_cfg->Channels[chan_ix].BandpassFilterIndex);  // TODO: Set first enabled value
		QSpinBox* chan_notch = (QSpinBox*)ui->channels_tableWidget->cellWidget(chan_ix, 5);
		chan_notch->setRange(-1, (int)notchFiltersCount - 1);
		chan_notch->setValue(dev_cfg->Channels[chan_ix].NotchFilterIndex);  // TODO: Set first enabled value.
	}
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

	int channel_ix = -1;
	bool label_is_channel = false;

	while (!xmlReader.atEnd() && !xmlReader.hasError()) {
		xmlReader.readNext();
		if (xmlReader.isStartElement())
		{
			QStringRef elname = xmlReader.name();
			if (elname == "device")
				label_is_channel = false;
			else if (elname == "samplerate")
				ui->samplerate_comboBox->setCurrentIndex(std::find(gnautilus_sample_rates.begin(), gnautilus_sample_rates.end(),
					xmlReader.readElementText().toInt()) - gnautilus_sample_rates.begin());
			else if (elname == "network-channel")
				ui->networkchan_comboBox->setCurrentIndex(std::find(gnautilus_network_channels.begin(), gnautilus_network_channels.end(),
					xmlReader.readElementText().toInt()) - gnautilus_network_channels.begin());
			else if (elname == "noise-reduction")
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
			else if (elname == "channels")
				channel_ix = -1;
			else if (elname == "channel")
			{
				channel_ix++;
				label_is_channel = true;
			}
			// 0: Label, 1: Acquire, 2: Range/Sensitivity, 3: Bipolar, 4: Bandpass, 5: Notch, 6: CAR (check), 7: Noise Reduction (check) (, 8: Impedance)
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
			else if (elname == "Range")
			{
				QSpinBox *chan_range = (QSpinBox*)ui->channels_tableWidget->cellWidget(channel_ix, 2);
				chan_range->setValue(std::find(gnautilus_sensitivities.begin(), gnautilus_sensitivities.end(),
					xmlReader.readElementText().toDouble()) - gnautilus_sensitivities.begin());
			}
			else if (elname == "BipolarChannel")
			{
				QSpinBox* chan_bipolar = (QSpinBox*)ui->channels_tableWidget->cellWidget(channel_ix, 3);
				chan_bipolar->setValue(xmlReader.readElementText().toInt());
			}
			else if (elname == "BandpassFilterIndex")
			{
				QSpinBox* chan_bandpass = (QSpinBox*)ui->channels_tableWidget->cellWidget(channel_ix, 4);
				chan_bandpass->setValue(xmlReader.readElementText().toInt());
			}
			else if (elname == "NotchFilterIndex")
			{
				QSpinBox* chan_notch = (QSpinBox*)ui->channels_tableWidget->cellWidget(channel_ix, 5);
				chan_notch->setValue(xmlReader.readElementText().toInt());
			}
			else if (elname == "InCAR")
			{
				QCheckBox* chan_car = (QCheckBox*)ui->channels_tableWidget->cellWidget(channel_ix, 6);
				chan_car->setChecked(xmlReader.readElementText().compare("true", Qt::CaseInsensitive) == 0);
			}
			else if (elname == "InNoiseRed")
			{
				QCheckBox* chan_noisered = (QCheckBox*)ui->channels_tableWidget->cellWidget(channel_ix, 7);
				chan_noisered->setChecked(xmlReader.readElementText().compare("true", Qt::CaseInsensitive) == 0);
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

		stream.writeTextElement("samplerate", QString::number(gnautilus_sample_rates[ui->samplerate_comboBox->currentIndex()]));
		stream.writeTextElement("network-channel", QString::number(gnautilus_network_channels[ui->networkchan_comboBox->currentIndex()]));
		stream.writeTextElement("noise-reduction", ui->noiseCheckBox->isChecked() ? "true" : "false");
		stream.writeTextElement("car", ui->CARCheckBox->isChecked() ? "true" : "false");
		stream.writeTextElement("acceleration-data", ui->accelCheckBox->isChecked() ? "true" : "false");
		stream.writeTextElement("counter", ui->counterCheckBox->isChecked() ? "true" : "false");
		stream.writeTextElement("link-quality", ui->linkCheckBox->isChecked() ? "true" : "false");
		stream.writeTextElement("battery-level", ui->batteryCheckBox->isChecked() ? "true" : "false");
		stream.writeTextElement("digital-inputs", ui->digitalCheckBox->isChecked() ? "true" : "false");
		stream.writeTextElement("validation-indicator", ui->validationCheckBox->isChecked() ? "true" : "false");

		stream.writeStartElement("channels");
		for (int chan_ix = 0; chan_ix < GDS_GNAUTILUS_CHANNELS_MAX; chan_ix++)
		{
			// 0: Label, 1: Acquire, 2: Range/Sensitivity, 3: Bipolar, 4: Bandpass, 5: Notch, 6: CAR (check), 7: Noise Reduction (check) (, 8: Impedance)
			stream.writeStartElement("channel");
			QTableWidgetItem *chan_label = ui->channels_tableWidget->item(chan_ix, 0);
			stream.writeTextElement("Label", chan_label->text());
			QCheckBox *chan_acquire = (QCheckBox*)ui->channels_tableWidget->cellWidget(chan_ix, 1);
			stream.writeTextElement("Acquire", chan_acquire->isChecked() ? "true" : "false");
			QSpinBox* chan_range = (QSpinBox*)ui->channels_tableWidget->cellWidget(chan_ix, 2);
			stream.writeTextElement("Range", QString::number(gnautilus_sensitivities[chan_range->value()]));
			QSpinBox* chan_bipolar = (QSpinBox*)ui->channels_tableWidget->cellWidget(chan_ix, 3);
			stream.writeTextElement("BipolarChannel", QString::number(chan_bipolar->value()));
			QSpinBox* chan_bandpass = (QSpinBox*)ui->channels_tableWidget->cellWidget(chan_ix, 4);
			stream.writeTextElement("BandpassFilterIndex", QString::number(chan_bandpass->value()));
			QSpinBox* chan_notch = (QSpinBox*)ui->channels_tableWidget->cellWidget(chan_ix, 5);
			stream.writeTextElement("NotchFilterIndex", QString::number(chan_notch->value()));
			QCheckBox *chan_CAR = (QCheckBox*)ui->channels_tableWidget->cellWidget(chan_ix, 6);
			stream.writeTextElement("InCAR", chan_CAR->isChecked() ? "true" : "false");
			QCheckBox *chan_NoiseRed = (QCheckBox*)ui->channels_tableWidget->cellWidget(chan_ix, 7);
			stream.writeTextElement("InNoiseRed", chan_NoiseRed->isChecked() ? "true" : "false");
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


void NautilusDlg::on_impedance_pushButton_clicked()
{
	char(*device_name)[DEVICE_NAME_LENGTH_MAX] = new char[1][DEVICE_NAME_LENGTH_MAX];
	std::strcpy(device_name[0], m_config.DeviceInfo.Name);

	double(*impedances)[GDS_GNAUTILUS_CHANNELS_MAX] = new double[1][GDS_GNAUTILUS_CHANNELS_MAX];

	GDS_RESULT res = GDS_GNAUTILUS_GetImpedance(*m_pHandle, device_name, impedances);
	if (res.ErrorCode)
	{
		qDebug() << "Impedance Error.";
	}
	else
	{
		m_pChannel_impedances->clear();
		for (size_t chan_ix = 0; chan_ix < GDS_GNAUTILUS_CHANNELS_MAX; chan_ix++)
		{
			m_pChannel_impedances->push_back(impedances[0][chan_ix]);
		}
		delete[] device_name;
		device_name = NULL;
		delete[] impedances;
		impedances = NULL;

		update_impedance_table();
	}
}


void NautilusDlg::update_impedance_table()
{
	for (int chan_ix = 0; chan_ix < GDS_GNAUTILUS_CHANNELS_MAX; chan_ix++)
	{
		if (m_pChannel_impedances->size() > chan_ix)
		{
			ui->channels_tableWidget->item(chan_ix, 8)->setText(QString::number(m_pChannel_impedances->at(chan_ix)));
		}
	}
}


void NautilusDlg::on_samplerate_comboBox_currentIndexChanged(int index)
{
	update_filters();
}


void NautilusDlg::on_networkchan_comboBox_currentIndexChanged(int index)
{
	// update_filters();
}


void NautilusDlg::on_inputrange_pushButton_clicked()
{
	apply_value_to_enabled_chans(ui->inputrange_comboBox->currentIndex(), 2);
}


void NautilusDlg::on_bipolar_pushButton_clicked()
{
	apply_value_to_enabled_chans(ui->bipolar_spinBox->value(), 3);
}


void NautilusDlg::on_bandpass_pushButton_clicked()
{
	apply_value_to_enabled_chans(ui->bandpass_comboBox->currentIndex(), 4);
}


void NautilusDlg::on_notch_pushButton_clicked()
{
	apply_value_to_enabled_chans(ui->notch_comboBox->currentIndex(), 5);
}


void NautilusDlg::apply_value_to_enabled_chans(int value, int widget_ix)
{
	for (int chan_ix = 0; chan_ix < GDS_GNAUTILUS_CHANNELS_MAX; chan_ix++)
	{
		QCheckBox *chan_acquire = (QCheckBox*)ui->channels_tableWidget->cellWidget(chan_ix, 1);
		if (chan_acquire->isChecked())
		{
			QSpinBox* chan_spbox = (QSpinBox*)ui->channels_tableWidget->cellWidget(chan_ix, widget_ix);
			chan_spbox->setValue(value);
		}
	}
}


void NautilusDlg::accept()
{
	GDS_GNAUTILUS_CONFIGURATION* dev_cfg = (GDS_GNAUTILUS_CONFIGURATION*)m_config.Configuration;

	// 
	// GDS_GNAUTILUS_SetNetworkChannel
	dev_cfg->SamplingRate = gnautilus_sample_rates[ui->samplerate_comboBox->currentIndex()];
	dev_cfg->NetworkChannel = gnautilus_network_channels[ui->networkchan_comboBox->currentIndex()];
	dev_cfg->CAR = ui->CARCheckBox->isChecked();
	dev_cfg->NoiseReduction = ui->noiseCheckBox->isChecked();
	dev_cfg->AccelerationData = ui->accelCheckBox->isChecked();
	dev_cfg->Counter = ui->counterCheckBox->isChecked();
	dev_cfg->LinkQualityInformation = ui->linkCheckBox->isChecked();
	dev_cfg->BatteryLevel = ui->batteryCheckBox->isChecked();
	dev_cfg->DigitalIOs = ui->digitalCheckBox->isChecked();
	dev_cfg->ValidationIndicator = ui->validationCheckBox->isChecked();

	m_pChannel_labels->clear();
	for (int chan_ix = 0; chan_ix < GDS_GNAUTILUS_CHANNELS_MAX; chan_ix++)
	{
		// 0: Label, 1: Acquire, 2: Range/Sensitivity, 3: Bipolar, 4: Bandpass, 5: Notch, 6: CAR (check), 7: Noise Reduction (check) (, 8: Impedance)
		QTableWidgetItem *chan_label = ui->channels_tableWidget->item(chan_ix, 0);
		m_pChannel_labels->push_back(chan_label->text().toStdString());
		QCheckBox *chan_acquire = (QCheckBox*)ui->channels_tableWidget->cellWidget(chan_ix, 1);
		dev_cfg->Channels[chan_ix].Enabled = chan_acquire->isChecked();
		QSpinBox* chan_range = (QSpinBox*)ui->channels_tableWidget->cellWidget(chan_ix, 2);
		dev_cfg->Channels[chan_ix].Sensitivity = gnautilus_sensitivities[chan_range->value()];
		QSpinBox* chan_bipolar = (QSpinBox*)ui->channels_tableWidget->cellWidget(chan_ix, 3);
		dev_cfg->Channels[chan_ix].BipolarChannel = chan_bipolar->value();
		QSpinBox* chan_bandpass = (QSpinBox*)ui->channels_tableWidget->cellWidget(chan_ix, 4);
		dev_cfg->Channels[chan_ix].BandpassFilterIndex = chan_bandpass->value();
		QSpinBox* chan_notch = (QSpinBox*)ui->channels_tableWidget->cellWidget(chan_ix, 5);
		dev_cfg->Channels[chan_ix].NotchFilterIndex = chan_notch->value();
		QCheckBox *chan_CAR = (QCheckBox*)ui->channels_tableWidget->cellWidget(chan_ix, 6);
		dev_cfg->Channels[chan_ix].UsedForCar = chan_CAR->isChecked();
		QCheckBox *chan_noise = (QCheckBox*)ui->channels_tableWidget->cellWidget(chan_ix, 7);
		dev_cfg->Channels[chan_ix].UsedForNoiseReduction = chan_noise->isChecked();
	}


	QDialog::accept();
}
