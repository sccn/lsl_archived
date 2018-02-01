#include "gUSB_dlg.h"
#include "ui_gUSB_dlg.h"
#include "GDSClientAPI.h"
#include "GDSClientAPI_gUSBamp.h"
#include <QtWidgets>




GUSBDlg::GUSBDlg(QWidget *parent)
	: QDialog(parent),
	  ui(new Ui::GUSBDlg)
{
	ui->setupUi(this);
}


void GUSBDlg::set_configs(std::vector<GDS_CONFIGURATION_BASE> configs)
{
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

		QComboBox *samplerate_box = new QComboBox;
		// TODO: Use GDS_GUSBAMP_GetSupportedSamplingRates instead of gUSBamp_sample_rates
		for each (uint32_t srate in gUSBamp_sample_rates)
		{
			samplerate_box->addItem(QString::number(srate));
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

		// TODO: Channels
		QTableWidget *chan_table = new QTableWidget(GDS_GUSBAMP_CHANNELS_MAX, 5);
		QStringList h_labels;
		h_labels << "Channel" << "Acquire" << "Bipolar" << "Bandpass" << "Notch";
		chan_table->setHorizontalHeaderLabels(h_labels);
		for (int chan_ix = 0; chan_ix < GDS_GUSBAMP_CHANNELS_MAX; chan_ix++)
		{
			chan_table->setItem(chan_ix, 0, new QTableWidgetItem(tr("%1").arg(chan_ix + 1)));
			chan_table->setCellWidget(chan_ix, 1, new QCheckBox());
			chan_table->setItem(chan_ix, 2, new QTableWidgetItem(tr("?")));
			chan_table->setItem(chan_ix, 3, new QTableWidgetItem(tr("?")));
			chan_table->setItem(chan_ix, 4, new QTableWidgetItem(tr("?")));
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


void GUSBDlg::update_ui()
{
	// Update ui with config contents
	for (size_t cfg_ix = 0; cfg_ix < m_configs.size(); cfg_ix++)
	{
		GDS_GUSBAMP_CONFIGURATION* dev_cfg = (GDS_GUSBAMP_CONFIGURATION*)m_configs[cfg_ix].Configuration;

		QVBoxLayout *dev_layout = (QVBoxLayout*)ui->devices_layout->itemAtPosition((int)cfg_ix, 0)->layout();
		QHBoxLayout *top_info_layout = (QHBoxLayout*)dev_layout->itemAt(0)->layout();
		QComboBox *samplerate_box = (QComboBox*)top_info_layout->itemAt(1)->widget();
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
		
		// TODO: Channels
		QTableWidget *chan_table = (QTableWidget*)dev_layout->itemAt(2)->widget();
		for (int chan_ix = 0; chan_ix < GDS_GUSBAMP_CHANNELS_MAX; chan_ix++)
		{
			QCheckBox *chan_acquire = (QCheckBox*)chan_table->cellWidget(chan_ix, 1);
			chan_acquire->setChecked(dev_cfg->Channels[chan_ix].Acquire);
			QTableWidgetItem *chan_bipolar = chan_table->item(chan_ix, 2);
			dev_cfg->Channels[chan_ix].BandpassFilterIndex;
			QTableWidgetItem *chan_bandpass = chan_table->item(chan_ix, 3);
			QTableWidgetItem *chan_notch = chan_table->item(chan_ix, 4);
		}
	}
}


void GUSBDlg::accept()
{
	for (size_t cfg_ix = 0; cfg_ix < m_configs.size(); cfg_ix++)
	{
		GDS_GUSBAMP_CONFIGURATION* dev_cfg = (GDS_GUSBAMP_CONFIGURATION*)m_configs[cfg_ix].Configuration;

		QVBoxLayout *dev_layout = (QVBoxLayout*)ui->devices_layout->itemAtPosition((int)cfg_ix, 0)->layout();
		QHBoxLayout *top_info_layout = (QHBoxLayout*)dev_layout->itemAt(0)->layout();
		QComboBox *samplerate_box = (QComboBox*)top_info_layout->itemAt(1)->widget();
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

		// TODO: Channels
		QTableWidget *chan_table = (QTableWidget*)dev_layout->itemAt(2)->widget();
		for (int chan_ix = 0; chan_ix < GDS_GUSBAMP_CHANNELS_MAX; chan_ix++)
		{
			QCheckBox *chan_acquire = (QCheckBox*)chan_table->cellWidget(chan_ix, 1);
			dev_cfg->Channels[chan_ix].Acquire = chan_acquire->isChecked();
		}
	}

	QDialog::accept();
}