#include "gUSB_dlg.h"
#include "ui_gUSB_dlg.h"
#include "GDSClientAPI.h"
#include "GDSClientAPI_gUSBamp.h"
#include <QtWidgets>
#include <algorithm>


GUSBDlg::GUSBDlg(QWidget *parent)
	: QDialog(parent),
	  ui(new Ui::GUSBDlg)
{
	ui->setupUi(this);

	load_config(default_config_fname);
}


void GUSBDlg::set_configs(GDS_HANDLE* connection_handle, std::vector<GDS_CONFIGURATION_BASE> configs, std::vector<std::string>* chan_labels)
{
	m_pHandle = connection_handle;
	m_pChannel_labels = chan_labels;
	m_configs.clear();
	for (size_t cfg_ix = 0; cfg_ix < configs.size(); cfg_ix++)
	{
		m_configs.push_back(configs[cfg_ix]);
		m_configs[cfg_ix].Configuration = configs[cfg_ix].Configuration;
	}
	create_widgets();
	update_ui();
}


void GUSBDlg::create_widgets()
{
	for (size_t cfg_ix = 0; cfg_ix < m_configs.size(); cfg_ix++)
	{	
		QLabel *name_label = new QLabel(m_configs[cfg_ix].DeviceInfo.Name);

		QLabel *samplerate_label = new QLabel("Sampling Rate:");
		QComboBox *samplerate_box = new QComboBox;
		// TODO: Use GDS_GUSBAMP_GetSupportedSamplingRates instead of gUSBamp_sample_rates
		for each (uint32_t srate in gUSBamp_sample_rates)
		{
			samplerate_box->addItem(QString::number(srate));
		}
		samplerate_box->setEnabled(cfg_ix == 0);
		if (cfg_ix == 0)
		{
			connect(samplerate_box, SIGNAL(currentIndexChanged(int)), this, SLOT(handle_sampleRateChange(int)));
		}

		// Create widgets for binary switches.
		QCheckBox *shortcut_box = new QCheckBox(tr("ShortCut"));
		QCheckBox *counter_box = new QCheckBox(tr("Counter"));
		QCheckBox *trigger_box = new QCheckBox(tr("Trigger"));
		
		// Create widgets for common grounds and references.
		QHBoxLayout *gr_layout = new QHBoxLayout;
		for (size_t grp_ix = 0; grp_ix < GDS_GUSBAMP_GROUPS_MAX; grp_ix++)
		{
			char box_label[2] = { 65 + (char)grp_ix };
			QCheckBox *ground_box = new QCheckBox(tr(box_label));
			gr_layout->addWidget(ground_box);
		}
		QHBoxLayout *ref_layout = new QHBoxLayout;
		for (size_t grp_ix = 0; grp_ix < GDS_GUSBAMP_GROUPS_MAX; grp_ix++)
		{
			char box_label[2] = { 65 + (char)grp_ix };
			QCheckBox *ref_box = new QCheckBox(tr(box_label));
			ref_layout->addWidget(ref_box);
		}

		// Channels
		QTableWidget *chan_table = new QTableWidget(GDS_GUSBAMP_CHANNELS_MAX, 5);
		QStringList h_labels;
		h_labels << "Channel" << "Acquire" << "Bipolar" << "Bandpass" << "Notch";
		chan_table->setHorizontalHeaderLabels(h_labels);
		for (int chan_ix = 0; chan_ix < GDS_GUSBAMP_CHANNELS_MAX; chan_ix++)
		{
			if (m_pChannel_labels->size() > chan_ix)
			{
				chan_table->setItem(chan_ix, 0, new QTableWidgetItem(QString::fromStdString(m_pChannel_labels->at(chan_ix))));
			}
			else
			{
				chan_table->setItem(chan_ix, 0, new QTableWidgetItem(tr("%1").arg(chan_ix + 1)));
			}
			chan_table->setCellWidget(chan_ix, 1, new QCheckBox());
			QSpinBox* bipolar_spinbox = new QSpinBox();
			chan_table->setCellWidget(chan_ix, 2, bipolar_spinbox);
			QSpinBox *bandpass_spinbox = new QSpinBox();
			chan_table->setCellWidget(chan_ix, 3, bandpass_spinbox);
			QSpinBox *notch_spinbox = new QSpinBox();
			chan_table->setCellWidget(chan_ix, 4, notch_spinbox);
		}
		
		// Put all the widgets together on a layout.
		// |name     samplerate  |
		// |switches grounds/refs|
		// |channels             |
		// switches = [shortcut; counter; trigger]
		// grounds/refs = [[groundA groundB groundC groundD]; [refA refB refC refD]]
		QVBoxLayout *dev_layout = new QVBoxLayout;

		QHBoxLayout *top_info_layout = new QHBoxLayout;
		top_info_layout->addWidget(name_label);
		top_info_layout->addWidget(samplerate_label);
		top_info_layout->addWidget(samplerate_box);
		dev_layout->addLayout(top_info_layout);
		
		QHBoxLayout *sw_gr_ref_layout = new QHBoxLayout;

		QVBoxLayout *switches_layout = new QVBoxLayout;
		switches_layout->addWidget(shortcut_box);
		switches_layout->addWidget(counter_box);
		switches_layout->addWidget(trigger_box);
		QGroupBox *switches_group = new QGroupBox(tr("Switches"));
		switches_group->setLayout(switches_layout);
		sw_gr_ref_layout->addWidget(switches_group);

		QVBoxLayout *gr_ref_layout = new QVBoxLayout;
		QGroupBox *ground_group = new QGroupBox(tr("Common Ground"));
		ground_group->setLayout(gr_layout);
		gr_ref_layout->addWidget(ground_group);
		QGroupBox *ref_group = new QGroupBox(tr("Common Reference"));
		ref_group->setLayout(ref_layout);
		gr_ref_layout->addWidget(ref_group);
		sw_gr_ref_layout->addLayout(gr_ref_layout);
		dev_layout->addLayout(sw_gr_ref_layout);

		// Channels widget
		dev_layout->addWidget(chan_table);

		// Device done. Add to parent layout.
		ui->devices_layout->addLayout(dev_layout, (int)cfg_ix, 0);
	}
}


void GUSBDlg::update_filters()
{
	// Update ui with config contents
	size_t bandpassFiltersCount;
	size_t notchFiltersCount;

	for (size_t cfg_ix = 0; cfg_ix < m_configs.size(); cfg_ix++)
	{
		GDS_GUSBAMP_CONFIGURATION* dev_cfg = (GDS_GUSBAMP_CONFIGURATION*)m_configs[cfg_ix].Configuration;

		QVBoxLayout *dev_layout = (QVBoxLayout*)ui->devices_layout->itemAtPosition((int)cfg_ix, 0)->layout();
		
		char(*device_name)[DEVICE_NAME_LENGTH_MAX] = new char[1][DEVICE_NAME_LENGTH_MAX];
		std::strcpy(device_name[0], m_configs[cfg_ix].DeviceInfo.Name);
		if (cfg_ix == 0)
		{
			QHBoxLayout *top_info_layout = (QHBoxLayout*)dev_layout->itemAt(0)->layout();
			QComboBox *samplerate_box = (QComboBox*)top_info_layout->itemAt(2)->widget();

			// Use GDS_GUSBAMP_GetBandpassFilters
			GDS_RESULT res = GDS_GUSBAMP_GetBandpassFilters(*m_pHandle, device_name, NULL, &bandpassFiltersCount);
			std::vector<GDS_FILTER_INFO> bandpassFilters(bandpassFiltersCount);
			res = GDS_GUSBAMP_GetBandpassFilters(*m_pHandle, device_name, bandpassFilters.data(), &bandpassFiltersCount);

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
				bandpass_item->setFlags(bandpassFilters[bp_ix].SamplingRate == gUSBamp_sample_rates[samplerate_box->currentIndex()] ?
					bandpass_item->flags() | Qt::ItemIsEnabled : bandpass_item->flags() & ~Qt::ItemIsEnabled);
			}

			// Use GDS_GUSBAMP_GetNotchFilters
			res = GDS_GUSBAMP_GetNotchFilters(*m_pHandle, device_name, NULL, &notchFiltersCount);
			std::vector<GDS_FILTER_INFO> notchFilters(notchFiltersCount);
			res = GDS_GUSBAMP_GetNotchFilters(*m_pHandle, device_name, notchFilters.data(), &notchFiltersCount);
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
				notch_item->setFlags(notchFilters[notch_ix].SamplingRate == gUSBamp_sample_rates[samplerate_box->currentIndex()] ?
					notch_item->flags() | Qt::ItemIsEnabled : notch_item->flags() & ~Qt::ItemIsEnabled);
			}
		}
		delete[] device_name;
		device_name = NULL;

		// Update Channels ranges
		QTableWidget *chan_table = (QTableWidget*)dev_layout->itemAt(2)->widget();
		for (int chan_ix = 0; chan_ix < GDS_GUSBAMP_CHANNELS_MAX; chan_ix++)
		{
			QSpinBox* chan_bandpass = (QSpinBox*)chan_table->cellWidget(chan_ix, 3);
			chan_bandpass->setRange(-1, (int)bandpassFiltersCount - 1);
			chan_bandpass->setValue(dev_cfg->Channels[chan_ix].BandpassFilterIndex);  // TODO: Set first enabled value
			QSpinBox* chan_notch = (QSpinBox*)chan_table->cellWidget(chan_ix, 4);
			chan_notch->setRange(-1, (int)notchFiltersCount - 1);
			chan_notch->setValue(dev_cfg->Channels[chan_ix].NotchFilterIndex);  // TODO: Set first enabled value.
		}
	}
}


void GUSBDlg::update_ui()
{
	// Update ui with config contents
	for (size_t cfg_ix = 0; cfg_ix < m_configs.size(); cfg_ix++)
	{
		GDS_GUSBAMP_CONFIGURATION* dev_cfg = (GDS_GUSBAMP_CONFIGURATION*)m_configs[cfg_ix].Configuration;

		QVBoxLayout *dev_layout = (QVBoxLayout*)ui->devices_layout->itemAtPosition((int)cfg_ix, 0)->layout();
		QHBoxLayout *top_info_layout = (QHBoxLayout*)dev_layout->itemAt(0)->layout();
		QComboBox *samplerate_box = (QComboBox*)top_info_layout->itemAt(2)->widget();
		samplerate_box->setCurrentIndex(std::find(gUSBamp_sample_rates.begin(), gUSBamp_sample_rates.end(), dev_cfg->SampleRate) - gUSBamp_sample_rates.begin());

		QHBoxLayout *sw_gr_ref_layout = (QHBoxLayout*)dev_layout->itemAt(1)->layout();
		
		QGroupBox *switches_group = (QGroupBox*)sw_gr_ref_layout->itemAt(0)->widget();
		QCheckBox *shortcut_box = (QCheckBox*)switches_group->layout()->itemAt(0)->widget();
		shortcut_box->setChecked(dev_cfg->ShortCutEnabled);
		QCheckBox *counter_box = (QCheckBox*)switches_group->layout()->itemAt(1)->widget();
		counter_box->setChecked(dev_cfg->CounterEnabled);
		QCheckBox *trigger_box = (QCheckBox*)switches_group->layout()->itemAt(2)->widget();
		trigger_box->setChecked(dev_cfg->TriggerEnabled);

		QVBoxLayout *gr_ref_layout = (QVBoxLayout*)sw_gr_ref_layout->itemAt(1)->layout();
		QGroupBox *ground_group = (QGroupBox*)gr_ref_layout->itemAt(0)->widget();
		QHBoxLayout *gr_layout = (QHBoxLayout*)ground_group->layout();
		QGroupBox *ref_group = (QGroupBox*)gr_ref_layout->itemAt(1)->widget();
		QHBoxLayout *ref_layout = (QHBoxLayout*)ref_group->layout();
		for (int grp_ix = 0; grp_ix < GDS_GUSBAMP_GROUPS_MAX; grp_ix++)
		{
			QCheckBox *ground_box = (QCheckBox*)gr_layout->itemAt(grp_ix)->widget();
			ground_box->setChecked(dev_cfg->CommonGround[grp_ix]);
			QCheckBox *ref_box = (QCheckBox*)ref_layout->itemAt(grp_ix)->widget();
			ref_box->setChecked(dev_cfg->CommonReference[grp_ix]);
		}

		QTableWidget *chan_table = (QTableWidget*)dev_layout->itemAt(2)->widget();
		for (int chan_ix = 0; chan_ix < GDS_GUSBAMP_CHANNELS_MAX; chan_ix++)
		{
			QCheckBox *chan_acquire = (QCheckBox*)chan_table->cellWidget(chan_ix, 1);
			chan_acquire->setChecked(dev_cfg->Channels[chan_ix].Acquire);
			QSpinBox* chan_bipolar = (QSpinBox*)chan_table->cellWidget(chan_ix, 2);
			chan_bipolar->setRange(0, GDS_GUSBAMP_CHANNELS_MAX);
			chan_bipolar->setValue(dev_cfg->Channels[chan_ix].BipolarChannel);
		}

		update_filters();
	}
}


void GUSBDlg::accept()
{
	for (size_t cfg_ix = 0; cfg_ix < m_configs.size(); cfg_ix++)
	{
		GDS_GUSBAMP_CONFIGURATION* dev_cfg = (GDS_GUSBAMP_CONFIGURATION*)m_configs[cfg_ix].Configuration;

		QVBoxLayout *dev_layout = (QVBoxLayout*)ui->devices_layout->itemAtPosition((int)cfg_ix, 0)->layout();
		QHBoxLayout *top_info_layout = (QHBoxLayout*)dev_layout->itemAt(0)->layout();
		QComboBox *samplerate_box = (QComboBox*)top_info_layout->itemAt(2)->widget();
		dev_cfg->SampleRate = gUSBamp_sample_rates[samplerate_box->currentIndex()];

		QHBoxLayout *sw_gr_ref_layout = (QHBoxLayout*)dev_layout->itemAt(1)->layout();

		QGroupBox *switches_group = (QGroupBox*)sw_gr_ref_layout->itemAt(0)->widget();
		QCheckBox *shortcut_box = (QCheckBox*)switches_group->layout()->itemAt(0)->widget();
		dev_cfg->ShortCutEnabled = shortcut_box->isChecked();
		QCheckBox *counter_box = (QCheckBox*)switches_group->layout()->itemAt(1)->widget();
		dev_cfg->CounterEnabled = counter_box->isChecked();
		QCheckBox *trigger_box = (QCheckBox*)switches_group->layout()->itemAt(2)->widget();
		dev_cfg->TriggerEnabled = trigger_box->isChecked();

		QVBoxLayout *gr_ref_layout = (QVBoxLayout*)sw_gr_ref_layout->itemAt(1)->layout();
		QGroupBox *ground_group = (QGroupBox*)gr_ref_layout->itemAt(0)->widget();
		QHBoxLayout *gr_layout = (QHBoxLayout*)ground_group->layout();
		QGroupBox *ref_group = (QGroupBox*)gr_ref_layout->itemAt(1)->widget();
		QHBoxLayout *ref_layout = (QHBoxLayout*)ref_group->layout();
		for (int grp_ix = 0; grp_ix < GDS_GUSBAMP_GROUPS_MAX; grp_ix++)
		{
			QCheckBox *ground_box = (QCheckBox*)gr_layout->itemAt(grp_ix)->widget();
			dev_cfg->CommonGround[grp_ix] = ground_box->isChecked();
			QCheckBox *ref_box = (QCheckBox*)ref_layout->itemAt(grp_ix)->widget();
			dev_cfg->CommonReference[grp_ix] = ref_box->isChecked();
		}

		m_pChannel_labels->clear();
		QTableWidget *chan_table = (QTableWidget*)dev_layout->itemAt(2)->widget();
		for (int chan_ix = 0; chan_ix < GDS_GUSBAMP_CHANNELS_MAX; chan_ix++)
		{
			QTableWidgetItem *chan_label = chan_table->item(chan_ix, 0);
			m_pChannel_labels->push_back(chan_label->text().toStdString());
			QCheckBox *chan_acquire = (QCheckBox*)chan_table->cellWidget(chan_ix, 1);
			dev_cfg->Channels[chan_ix].Acquire = chan_acquire->isChecked();
			QSpinBox* chan_bipolar = (QSpinBox*)chan_table->cellWidget(chan_ix, 2);
			dev_cfg->Channels[chan_ix].BipolarChannel = chan_bipolar->value();
			QSpinBox* chan_bandpass = (QSpinBox*)chan_table->cellWidget(chan_ix, 3);
			dev_cfg->Channels[chan_ix].BandpassFilterIndex = chan_bandpass->value();
			QSpinBox* chan_notch = (QSpinBox*)chan_table->cellWidget(chan_ix, 4);
			dev_cfg->Channels[chan_ix].NotchFilterIndex = chan_notch->value();
		}
	}

	QDialog::accept();
}


void GUSBDlg::on_loadCfgButton_clicked()
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


void GUSBDlg::load_config(const QString filename)
{
	QFile xmlFile(filename);
	if (!xmlFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
		qDebug() << "Could not load XML from file " << filename;
		return;
	}
	QXmlStreamReader xmlReader(&xmlFile);
	size_t device_ix = -1;
	int channel_ix = -1;
	std::vector<std::string> gr_blocks = { "A", "B", "C", "D" };
	bool label_is_channel = false;

	// Convenience references to GUI items that change depending on which device, ground or ref, etc.
	QVBoxLayout *dev_layout;
	QGroupBox *switches_group;
	QVBoxLayout *gr_ref_layout;
	QHBoxLayout *gr_or_ref_layout;
	QTableWidget *chan_table;
	

	while (!xmlReader.atEnd() && !xmlReader.hasError()) {
		xmlReader.readNext();
		if (xmlReader.isStartElement())
		{
			QStringRef elname = xmlReader.name();
			if (elname == "device")
			{
				device_ix++;
				label_is_channel = false;
				dev_layout = (QVBoxLayout*)ui->devices_layout->itemAtPosition((int)device_ix, 0)->layout();
				QHBoxLayout *sw_gr_ref_layout = (QHBoxLayout*)dev_layout->itemAt(1)->layout();
				switches_group = (QGroupBox*)sw_gr_ref_layout->itemAt(0)->widget();
				gr_ref_layout = (QVBoxLayout*)sw_gr_ref_layout->itemAt(1)->layout();
				chan_table = (QTableWidget*)dev_layout->itemAt(2)->widget();
			}
			else if (elname == "grounds")
			{
				QGroupBox *ground_group = (QGroupBox*)gr_ref_layout->itemAt(0)->widget();
				gr_or_ref_layout = (QHBoxLayout*)ground_group->layout();
			}
			else if (elname == "references")
			{
				QGroupBox *ref_group = (QGroupBox*)gr_ref_layout->itemAt(1)->widget();
				gr_or_ref_layout = (QHBoxLayout*)ref_group->layout();
			}
			else if (elname == "SampleRate" && device_ix == 0)
			{
				QHBoxLayout *top_info_layout = (QHBoxLayout*)dev_layout->itemAt(0)->layout();
				QComboBox *samplerate_box = (QComboBox*)top_info_layout->itemAt(2)->widget();
				samplerate_box->setCurrentIndex(std::find(gUSBamp_sample_rates.begin(), gUSBamp_sample_rates.end(),
					xmlReader.readElementText().toInt()) - gUSBamp_sample_rates.begin());
			}
			else if (elname == "ShortCutEnabled")
			{
				QCheckBox *shortcut_box = (QCheckBox*)switches_group->layout()->itemAt(0)->widget();
				shortcut_box->setChecked(xmlReader.readElementText().compare("true", Qt::CaseInsensitive) == 0);
			}
			else if (elname == "CounterEnabled")
			{
				QCheckBox *counter_box = (QCheckBox*)switches_group->layout()->itemAt(1)->widget();
				counter_box->setChecked(xmlReader.readElementText().compare("true", Qt::CaseInsensitive) == 0);
			}
			else if (elname == "TriggerEnabled")
			{
				QCheckBox *trigger_box = (QCheckBox*)switches_group->layout()->itemAt(2)->widget();
				trigger_box->setChecked(xmlReader.readElementText().compare("true", Qt::CaseInsensitive) == 0);
			}
			else if (std::find(gr_blocks.begin(), gr_blocks.end(), elname.toString().toStdString()) != gr_blocks.end())  // A, B, C, D
			{
				ptrdiff_t block_ix = std::distance(gr_blocks.begin(), std::find(gr_blocks.begin(), gr_blocks.end(), elname.toString().toStdString()));
				QCheckBox *check_box = (QCheckBox*)gr_or_ref_layout->itemAt(block_ix)->widget();
				check_box->setChecked(xmlReader.readElementText().compare("true", Qt::CaseInsensitive) == 0);
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
				QTableWidgetItem *chan_label = chan_table->item(channel_ix, 0);
				chan_label->setText(xmlReader.readElementText());
			}
			else if (elname == "Acquire")
			{
				QCheckBox *chan_acquire = (QCheckBox*)chan_table->cellWidget(channel_ix, 1);
				chan_acquire->setChecked(xmlReader.readElementText().compare("true", Qt::CaseInsensitive) == 0);
			}
			else if (elname == "BipolarChannel")
			{
				QSpinBox* chan_bipolar = (QSpinBox*)chan_table->cellWidget(channel_ix, 2);
				chan_bipolar->setValue(xmlReader.readElementText().toInt());
			}
			else if (elname == "BandpassFilterIndex")
			{
				QSpinBox* chan_bandpass = (QSpinBox*)chan_table->cellWidget(channel_ix, 3);
				chan_bandpass->setValue(xmlReader.readElementText().toInt());
			}
			else if (elname == "NotchFilterIndex")
			{
				QSpinBox* chan_notch = (QSpinBox*)chan_table->cellWidget(channel_ix, 4);
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


void GUSBDlg::on_saveCfgButton_clicked()
{
	QString sel = QFileDialog::getSaveFileName(this,
		"Save Configuration File",
		QDir::currentPath(),
		"Configuration Files (*.cfg)");
	if (!sel.isEmpty())
	{
		// TODO: Confirm overwrite
		save_config(sel);
	}
}


void GUSBDlg::save_config(QString filename)
{
	try {
		QFile file(filename);
		file.open(QIODevice::WriteOnly);
		QXmlStreamWriter stream(&file);
		stream.setAutoFormatting(true);
		stream.writeStartDocument();
		// Save GUI state of device + channel config.
		stream.writeStartElement("devices");
		for (size_t cfg_ix = 0; cfg_ix < m_configs.size(); cfg_ix++)
		{
			stream.writeStartElement("device");
			QVBoxLayout *dev_layout = (QVBoxLayout*)ui->devices_layout->itemAtPosition((int)cfg_ix, 0)->layout();

			QHBoxLayout *top_info_layout = (QHBoxLayout*)dev_layout->itemAt(0)->layout();
			QLabel *dev_label = (QLabel*)top_info_layout->itemAt(0)->widget();
			stream.writeTextElement("Label", dev_label->text());
			QComboBox *samplerate_box = (QComboBox*)top_info_layout->itemAt(2)->widget();
			stream.writeTextElement("SampleRate", QString::number(gUSBamp_sample_rates[samplerate_box->currentIndex()]));

			QHBoxLayout *sw_gr_ref_layout = (QHBoxLayout*)dev_layout->itemAt(1)->layout();

			QGroupBox *switches_group = (QGroupBox*)sw_gr_ref_layout->itemAt(0)->widget();
			
			QCheckBox *shortcut_box = (QCheckBox*)switches_group->layout()->itemAt(0)->widget();
			stream.writeTextElement("ShortCutEnabled", shortcut_box->isChecked() ? "true" : "false");
			
			QCheckBox *counter_box = (QCheckBox*)switches_group->layout()->itemAt(1)->widget();
			stream.writeTextElement("CounterEnabled", counter_box->isChecked() ? "true" : "false");
			
			QCheckBox *trigger_box = (QCheckBox*)switches_group->layout()->itemAt(2)->widget();
			stream.writeTextElement("TriggerEnabled", trigger_box->isChecked() ? "true" : "false");

			QStringList gr_blocks;
			gr_blocks << "A" << "B" << "C" << "D";

			stream.writeStartElement("grounds");
			QVBoxLayout *gr_ref_layout = (QVBoxLayout*)sw_gr_ref_layout->itemAt(1)->layout();
			QGroupBox *ground_group = (QGroupBox*)gr_ref_layout->itemAt(0)->widget();
			QHBoxLayout *gr_layout = (QHBoxLayout*)ground_group->layout();
			for (int grp_ix = 0; grp_ix < GDS_GUSBAMP_GROUPS_MAX; grp_ix++)
			{
				QCheckBox *ground_box = (QCheckBox*)gr_layout->itemAt(grp_ix)->widget();
				stream.writeTextElement(gr_blocks.at(grp_ix), ground_box->isChecked() ? "true" : "false");
			}
			stream.writeEndElement(); // grounds

			stream.writeStartElement("references");
			QGroupBox *ref_group = (QGroupBox*)gr_ref_layout->itemAt(1)->widget();
			QHBoxLayout *ref_layout = (QHBoxLayout*)ref_group->layout();
			for (int grp_ix = 0; grp_ix < GDS_GUSBAMP_GROUPS_MAX; grp_ix++)
			{
				QCheckBox *ref_box = (QCheckBox*)ref_layout->itemAt(grp_ix)->widget();
				stream.writeTextElement(gr_blocks.at(grp_ix), ref_box->isChecked() ? "true" : "false");
			}
			stream.writeEndElement(); // references

			stream.writeStartElement("channels");
			QTableWidget *chan_table = (QTableWidget*)dev_layout->itemAt(2)->widget();
			for (int chan_ix = 0; chan_ix < GDS_GUSBAMP_CHANNELS_MAX; chan_ix++)
			{
				stream.writeStartElement("channel");
				QTableWidgetItem *chan_label = chan_table->item(chan_ix, 0);
				stream.writeTextElement("Label", chan_label->text());
				QCheckBox *chan_acquire = (QCheckBox*)chan_table->cellWidget(chan_ix, 1);
				stream.writeTextElement("Acquire", chan_acquire->isChecked() ? "true" : "false");
				QSpinBox* chan_bipolar = (QSpinBox*)chan_table->cellWidget(chan_ix, 2);
				stream.writeTextElement("BipolarChannel", QString::number(chan_bipolar->value()));
				QSpinBox* chan_bandpass = (QSpinBox*)chan_table->cellWidget(chan_ix, 3);
				stream.writeTextElement("BandpassFilterIndex", QString::number(chan_bandpass->value()));
				QSpinBox* chan_notch = (QSpinBox*)chan_table->cellWidget(chan_ix, 4);
				stream.writeTextElement("NotchFilterIndex", QString::number(chan_notch->value()));
				stream.writeEndElement(); // channel
			}
			stream.writeEndElement(); // channels
			stream.writeEndElement(); // device
		}
		stream.writeEndElement(); // devices
		stream.writeEndDocument();
		file.close();
	}
	catch (std::exception &e) {
		qDebug() << "Problem saving to config file: " << e.what();
	}
}


void GUSBDlg::handle_sampleRateChange(int index)
{
	for (size_t cfg_ix = 1; cfg_ix < m_configs.size(); cfg_ix++)
	{
		QVBoxLayout *dev_layout = (QVBoxLayout*)ui->devices_layout->itemAtPosition((int)cfg_ix, 0)->layout();
		QHBoxLayout *top_info_layout = (QHBoxLayout*)dev_layout->itemAt(0)->layout();
		QComboBox *samplerate_box = (QComboBox*)top_info_layout->itemAt(2)->widget();
		samplerate_box->setCurrentIndex(index);
	}
	update_filters();
}


void GUSBDlg::on_bandpass_pushButton_clicked()
{
	apply_filter_to_enabled_chans(3, ui->bandpass_comboBox->currentIndex());
}


void GUSBDlg::on_notch_pushButton_clicked()
{
	apply_filter_to_enabled_chans(4, ui->notch_comboBox->currentIndex());
}


void GUSBDlg::apply_filter_to_enabled_chans(int widget_ix, int value)
{
	for (size_t cfg_ix = 0; cfg_ix < m_configs.size(); cfg_ix++)
	{
		GDS_GUSBAMP_CONFIGURATION* dev_cfg = (GDS_GUSBAMP_CONFIGURATION*)m_configs[cfg_ix].Configuration;

		QVBoxLayout *dev_layout = (QVBoxLayout*)ui->devices_layout->itemAtPosition((int)cfg_ix, 0)->layout();
		QTableWidget *chan_table = (QTableWidget*)dev_layout->itemAt(2)->widget();
		for (int chan_ix = 0; chan_ix < GDS_GUSBAMP_CHANNELS_MAX; chan_ix++)
		{
			QCheckBox *chan_acquire = (QCheckBox*)chan_table->cellWidget(chan_ix, 1);
			if (chan_acquire->isChecked())
			{
				QSpinBox* chan_spbox = (QSpinBox*)chan_table->cellWidget(chan_ix, widget_ix);
				chan_spbox->setValue(value);
			}
		}
	}
}
