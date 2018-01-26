#include <iostream>
#include <qfiledialog.h>
#include <QtXml>
#include <QDebug>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "nautilus_dlg.h"
// TODO: #include "usbamp_dlg.h"
#include "GDSClientAPI_gUSBamp.h"
#include "GDSClientAPI_gHIamp.h"
#include "GDSClientAPI_gNautilus.h"


// GDS_DeviceInfo contains basic identifying information for each found device.
struct GDS_DeviceInfo
{
	uint32_t index;
	bool inUse;
	QString name;
	GDS_DEVICE_TYPE type;
};


bool MainWindow::handleResult(std::string calling_func, GDS_RESULT ret)
{
	bool res = true;
	if (ret.ErrorCode)
	{
		std::cerr << "ERROR on " << calling_func << " : " << ret.ErrorMessage;
		res = false;
	}
	return res;
}


void MainWindow::dataReadyCallback(GDS_HANDLE connectionHandle, void* usrData)
{
	MainWindow* thisWin = reinterpret_cast< MainWindow* >(usrData);
	thisWin->mutex.lock();
	size_t bufferSize = thisWin->m_devInfo.scans_per_block * thisWin->m_devInfo.nsamples_per_scan;
	size_t scans_available = 0;  // Read as many scans as possible, up to as many will fit in m_dataBuffer
	GDS_RESULT res = GDS_GetData(thisWin->m_connectionHandle, &scans_available, &thisWin->m_dataBuffer[0], bufferSize);
	if (scans_available > 0)
	{
		thisWin->m_eegOutlet->push_chunk_multiplexed(&thisWin->m_dataBuffer[0], scans_available * thisWin->m_devInfo.nsamples_per_scan);
		thisWin->m_samplesPushed += scans_available;
	}
	thisWin->mutex.unlock();
}


MainWindow::MainWindow(QWidget *parent, const QString config_file)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    load_config(config_file);
	GDS_Initialize();  // Initialize the g.NEEDaccess library
	m_pTimer = new QTimer(this);
	connect(m_pTimer, SIGNAL(timeout()), this, SLOT(notify_samples_pushed()));
}


MainWindow::~MainWindow()
{
	if (m_bStreaming)
	{
		handleResult("GDS_StopAcquisition", GDS_StopAcquisition(m_connectionHandle));
		handleResult("GDS_StopStreaming", GDS_StopStreaming(m_connectionHandle));
		handleResult("GDS_SetDataReadyCallback",
			GDS_SetDataReadyCallback(m_connectionHandle, NULL, 0, NULL));
		delete this->m_eegOutlet;
		this->m_eegOutlet = NULL;
	}
	if (m_bConnected)
	{
		handleResult("GDS_Disconnect",
			GDS_Disconnect(&m_connectionHandle));
	}
	clear_dev_configs();
	GDS_Uninitialize();
    delete ui;
	delete m_pTimer;
	m_pTimer = nullptr;
}


void MainWindow::on_scanPushButton_clicked()
{
	bool success = true;

	// These temporary updates to the UI elements will never be rendered because 
	// they will change back before the function returns. I will keep them anyway
	// because they act as code comments, and they could serve a purpose if device
	// communication ever becomes asynchronous.
    ui->scanPushButton->setText("Scanning...");
    ui->scanPushButton->setDisabled(true);

	// Copy the server info from UI elements to m_hostEndpoint.
	strcpy(this->m_hostEndpoint.IpAddress, ui->lineEdit_serverip->text().toLatin1().data());
	this->m_hostEndpoint.Port = ui->serverPortSpinBox->value();
	this->m_localEndpoint.Port = ui->clientPortSpinBox->value();

	// Scan the server for connected devices.
	GDS_DEVICE_CONNECTION_INFO* connected_devices = NULL;
	size_t count_daq_units = 0;
	handleResult("GDS_GetConnectedDevices",
		GDS_GetConnectedDevices(m_hostEndpoint, m_localEndpoint, &connected_devices, &count_daq_units));

	// Add the device names to the list widget.
	// Note: While I would prefer to store the device names and keep it separate from the GUI,
	// that would require updating that list of names whenever the user drag-and-dropped in the GUI.
	// SIGNAL: ui->availableListWidget->model().rowsMoved
	ui->availableListWidget->clear();
	for (size_t i = 0; i < count_daq_units; i++)
	{
		// Each daq unit operates a different number of devices. The second "for" loop 
		// addresses the devices attached to the daq unit.
		for (size_t j = 0; j < connected_devices[i].ConnectedDevicesLength; j++)
		{
			ui->availableListWidget->addItem(QString::fromStdString(connected_devices[i].ConnectedDevices[j].Name));
		}
	}
	ui->availableListWidget->setCurrentRow(0);
	success &= handleResult("GDS_FreeConnectedDevicesList",
		GDS_FreeConnectedDevicesList(&connected_devices, count_daq_units));

	ui->scanPushButton->setText("Scan");
	ui->scanPushButton->setDisabled(false);
}


void MainWindow::on_devCfgPushButton_clicked()
{
	int dev_ix = ui->availableListWidget->currentRow();
	if (m_devConfigs[0].DeviceInfo.DeviceType == GDS_DEVICE_TYPE_GNAUTILUS)
	{
		NautilusDlg cfg_dlg;
		cfg_dlg.set_config(m_devConfigs[0].Configuration);
		cfg_dlg.exec();
	}
	else if (m_devConfigs[0].DeviceInfo.DeviceType == GDS_DEVICE_TYPE_GUSBAMP)
	{
		//TODO: usbamp_dlg
		//cfg_dlg.set_config(m_devConfigs[0].Configuration);
		//cfg_dlg.exec();
	}
}


void MainWindow::on_availableListWidget_itemSelectionChanged()
{
	ui->connectPushButton->setDisabled(false);
}


void MainWindow::on_connectPushButton_clicked()
{
	ui->goPushButton->setDisabled(true);

	if (!m_bConnected)
	{
		m_bConnected = do_connect();  // This updates our data.
	}
	else
	{
		m_bConnected = !handleResult("GDS_Disconnect",
			GDS_Disconnect(&m_connectionHandle));
	}

	// Update UI elements to reflect connection state.
	ui->connectPushButton->setText(m_bConnected ? "Disconnect" : "Connect");
	ui->goPushButton->setEnabled(m_bConnected);
	ui->devCfgPushButton->setEnabled(m_isCreator && m_bConnected && m_isConfigurable);
	ui->availableListWidget->setDisabled(m_bConnected);

	ui->connectPushButton->setDisabled(false);
}


bool MainWindow::do_connect()
{
	QList<QListWidgetItem *> selectedItemsList = ui->availableListWidget->selectedItems();
	// Get variables needed for GDS_Connect.
	BOOL is_creator = FALSE;
	int nSelected = selectedItemsList.size();
	char(*device_names)[DEVICE_NAME_LENGTH_MAX] = new char[nSelected][DEVICE_NAME_LENGTH_MAX];
	// Note: This API is pretty unusual in the requested format of device_names.
	// Instead of trying to figure out the sane C++ way of conforming to an unusual C API, I'm just copying their example code.
	for (int dev_ix = 0; dev_ix < nSelected; dev_ix++)
	{
		std::strcpy(device_names[dev_ix], selectedItemsList.at(dev_ix)->text().toLatin1().data());
	}

	bool success = handleResult("GDS_Connect",
		GDS_Connect(m_hostEndpoint, m_localEndpoint, device_names, nSelected, FALSE, &m_connectionHandle, &is_creator));
	if (success)
	{
		m_isCreator = bool(is_creator);
		if (!m_isCreator)
		{
			m_isCreator = handleResult("GDS_BecomeCreator",
				GDS_BecomeCreator(m_connectionHandle));
			if (!m_isCreator)
				statusBar()->showMessage("Device is first connected by another application. Cannot modify its configuration.");
		}
		success &= get_connected_devices_configs();

		if (success & m_isCreator)
		{
			// TODO: Get rid of m_isConfigurable and this loop once all the config GUI elements are built.
			m_isConfigurable = false;
			for each (GDS_CONFIGURATION_BASE cfg in m_devConfigs)
			{
				m_isConfigurable |= cfg.DeviceInfo.DeviceType == GDS_DEVICE_TYPE_GUSBAMP;
				m_isConfigurable |= cfg.DeviceInfo.DeviceType == GDS_DEVICE_TYPE_GNAUTILUS;
			}
		}

		if (!success)
		{
			GDS_Disconnect(&m_connectionHandle);  // Couldn't get configs, so disconnect.
		}
	}
	delete[] device_names;
	device_names = NULL;

	return success;
}


void MainWindow::clear_dev_configs()
{
	for (size_t dev_ix = 0; dev_ix < m_devConfigs.size(); dev_ix++)
	{
		switch (m_devConfigs[dev_ix].DeviceInfo.DeviceType)
		{
		case GDS_DEVICE_TYPE_NOT_SUPPORTED:
			break;
		case GDS_DEVICE_TYPE_GUSBAMP:
			delete (GDS_GUSBAMP_CONFIGURATION*)m_devConfigs[dev_ix].Configuration;
			break;
		case GDS_DEVICE_TYPE_GHIAMP:
			delete (GDS_GHIAMP_CONFIGURATION*)m_devConfigs[dev_ix].Configuration;
			break;
		case GDS_DEVICE_TYPE_GNAUTILUS:
			delete (GDS_GNAUTILUS_CONFIGURATION*)m_devConfigs[dev_ix].Configuration;
			break;
		default:
			break;
		}
	}
	m_devConfigs.clear();
}


bool MainWindow::get_connected_devices_configs()
{
	bool success = true;

	GDS_CONFIGURATION_BASE *deviceConfigurations = NULL;
	size_t deviceConfigurationsCount = 0;
	success &= handleResult("GDS_GetConfiguration",
		GDS_GetConfiguration(m_connectionHandle, &deviceConfigurations, &deviceConfigurationsCount));
	// Note: API allocates memory during GDS_GetConfiguration. We must free it with GDS_FreeConfigurationList below.

	// Copy device configs to our local map.
	clear_dev_configs();
	for (size_t dev_ix = 0; dev_ix < deviceConfigurationsCount; dev_ix++)
	{
		m_devConfigs.push_back(deviceConfigurations[dev_ix]);
		// We must also copy the data pointed to by the device-specific .Configuration
		size_t copy_bytes = 0;
		switch (deviceConfigurations[dev_ix].DeviceInfo.DeviceType)
		{
		case GDS_DEVICE_TYPE_NOT_SUPPORTED:
			break;
		case GDS_DEVICE_TYPE_GUSBAMP:
			copy_bytes = sizeof(GDS_GUSBAMP_CONFIGURATION);
			m_devConfigs[dev_ix].Configuration = new GDS_GUSBAMP_CONFIGURATION();  // Allocate memory.
			break;
		case GDS_DEVICE_TYPE_GHIAMP:
			copy_bytes = sizeof(GDS_GHIAMP_CONFIGURATION);
			m_devConfigs[dev_ix].Configuration = new GDS_GHIAMP_CONFIGURATION();  // Allocate memory.
			break;
		case GDS_DEVICE_TYPE_GNAUTILUS:
			copy_bytes = sizeof(GDS_GNAUTILUS_CONFIGURATION);
			m_devConfigs[dev_ix].Configuration = new GDS_GNAUTILUS_CONFIGURATION();  // Allocate memory.
			break;
		default:
			break;
		}
		memcpy(m_devConfigs[dev_ix].Configuration, deviceConfigurations[dev_ix].Configuration, copy_bytes);
	}

	// Free the resources used by the config
	success &= handleResult("GDS_FreeConfigurationList",
		GDS_FreeConfigurationList(&deviceConfigurations, deviceConfigurationsCount));
	
	return success;
}


void MainWindow::on_goPushButton_clicked()
{
	ui->goPushButton->setDisabled(true);  // re-enable when process complete.

	// Lock everything to prevent the GDS thread from stomping on our data during callback.
	QMutexLocker locker(&mutex);  // will unlock itself when function scope resolves.
		
	if (m_bStreaming)  // Already streaming --> Stop!
	{
		m_pTimer->stop();
		handleResult("GDS_StopStreaming", GDS_StopStreaming(m_connectionHandle));
		if (m_isCreator)
			handleResult("GDS_StopAcquisition", GDS_StopAcquisition(m_connectionHandle));
		handleResult("GDS_SetDataReadyCallback",
			GDS_SetDataReadyCallback(m_connectionHandle, NULL, 0, NULL));
		m_bStreaming = false;
		delete this->m_eegOutlet;
		this->m_eegOutlet = NULL;
		ui->goPushButton->setText("Go!");
	}
	else  // Not already streaming --> Start!
	{
		bool success = true;

		// If we are the daq unit creator, set config
		if (m_isCreator)
		{
			success &= handleResult("GDS_SetConfiguration",
				GDS_SetConfiguration(m_connectionHandle, &m_devConfigs[0], m_devConfigs.size()));
		}

		// TODO: GDS_SetDataAcquisitionErrorCallback
		// TODO: GDS_SetServerDiedCallback

		// Get all device-specific parameters into a convenient structure for LSL info
		m_devInfo = dev_info_type();  // reset m_devInfo;

		for (int dev_ix = 0; dev_ix < m_devConfigs.size(); dev_ix++)
		{
			if (dev_ix == 0)
			{
				// With the exception of gUSBamps, there should only be one device.
				// For gUSBamps, we'll take the details of the first (master)
				m_devInfo.name = m_devConfigs[dev_ix].DeviceInfo.Name;
			}
			

			// Some API functions require a device_names argument.
			// The API wants an _array_ of char[DEVICE_NAME_LENGTH_MAX] even though these functions are designed for only one device.
			char(*device_names)[DEVICE_NAME_LENGTH_MAX] = new char[1][DEVICE_NAME_LENGTH_MAX];
			std::strcpy(device_names[0], m_devConfigs[dev_ix].DeviceInfo.Name);

			GDS_DEVICE_TYPE dev_type = m_devConfigs[dev_ix].DeviceInfo.DeviceType;
			if (dev_type == GDS_DEVICE_TYPE_NOT_SUPPORTED)
			{
				success = false;
				qDebug() << "Unknown device type.";
			}
			else if (dev_type == GDS_DEVICE_TYPE_GUSBAMP)
			{
				GDS_GUSBAMP_CONFIGURATION* cfg_dev = (GDS_GUSBAMP_CONFIGURATION*)m_devConfigs[dev_ix].Configuration;

				if (dev_ix == 0)
				{
					// I hope all USBamps in a master/slave set have the same srate and scans_per_block
					m_devInfo.nominal_srate = double(cfg_dev->SampleRate);
					m_devInfo.scans_per_block = cfg_dev->NumberOfScans;
				}

				GDS_GUSBAMP_SCALING scaling;
				handleResult("GDS_GUSBAMP_GetScaling",
					GDS_GUSBAMP_GetScaling(m_connectionHandle, device_names, &scaling));

				for (size_t chan_ix = 0; chan_ix < GDS_GUSBAMP_CHANNELS_MAX; chan_ix++)
				{
					chan_info_type new_chan_info;
					new_chan_info.enabled = cfg_dev->Channels[chan_ix].Acquire;
					new_chan_info.label = std::to_string(chan_ix + 1);  // Device does not have access to custom channel labels.
					new_chan_info.type = "EEG";
					new_chan_info.unit = "uV";
					new_chan_info.scaling_offset = scaling.Offset[chan_ix];
					new_chan_info.scaling_factor = scaling.ScalingFactor[chan_ix];
					m_devInfo.channel_infos.push_back(new_chan_info);
				}
				// Add the trigger channel last.
				if (cfg_dev->TriggerEnabled)
				{
					chan_info_type new_chan_info;
					new_chan_info.enabled = true;
					new_chan_info.type = "DigitalIOs";
					new_chan_info.unit = "unknown";
					new_chan_info.label = "Digital IOs";
					m_devInfo.channel_infos.push_back(new_chan_info);
				}

				// TODO: Add info to metadata.
				// ShortCutEnabled, CounterEnabled, CommonGround[GDS_GUSBAMP_GROUPS_MAX], CommonReference[GDS_GUSBAMP_GROUPS_MAX]
				// List of all devices incl master/slave
			}
			else if (dev_type == GDS_DEVICE_TYPE_GHIAMP)
			{
				GDS_GHIAMP_CONFIGURATION* cfg_dev = (GDS_GHIAMP_CONFIGURATION*)m_devConfigs[dev_ix].Configuration;
				m_devInfo.nominal_srate = double(cfg_dev->SamplingRate);
				m_devInfo.scans_per_block = size_t(cfg_dev->NumberOfScans);
				for (size_t chan_ix = 0; chan_ix < GDS_GHIAMP_CHANNELS_MAX; chan_ix++)
				{
					chan_info_type new_chan_info;
					new_chan_info.enabled = cfg_dev->Channels[chan_ix].Acquire;
					new_chan_info.label = std::to_string(chan_ix + 1);
					new_chan_info.type = "EEG";
					new_chan_info.unit = "uV";
					m_devInfo.channel_infos.push_back(new_chan_info);
				}
			}
			else if (dev_type == GDS_DEVICE_TYPE_GNAUTILUS)
			{
				GDS_GNAUTILUS_CONFIGURATION* cfg_dev = (GDS_GNAUTILUS_CONFIGURATION*)m_devConfigs[dev_ix].Configuration;

				m_devInfo.nominal_srate = double(cfg_dev->SamplingRate);
				m_devInfo.scans_per_block = size_t(cfg_dev->NumberOfScans);

				// Get channel names

				// First determine how many channel names there are.
				uint32_t mountedModulesCount = 0;
				size_t electrodeNamesCount = 0;
				success &= handleResult("GDS_GNAUTILUS_GetChannelNames",
					GDS_GNAUTILUS_GetChannelNames(m_connectionHandle, device_names, &mountedModulesCount, NULL, &electrodeNamesCount));
				// Allocate memory to store the channel names.
				char(*electrode_names)[GDS_GNAUTILUS_ELECTRODE_NAME_LENGTH_MAX] = new char[electrodeNamesCount][GDS_GNAUTILUS_ELECTRODE_NAME_LENGTH_MAX];
				success &= handleResult("GDS_GNAUTILUS_GetChannelNames",
					GDS_GNAUTILUS_GetChannelNames(m_connectionHandle, device_names, &mountedModulesCount, electrode_names, &electrodeNamesCount));

				// Copy the result into thisDevInfo
				for (int chan_ix = 0; chan_ix < electrodeNamesCount; chan_ix++)
				{
					chan_info_type new_chan_info;
					new_chan_info.enabled = cfg_dev->Channels[chan_ix].Enabled;
					new_chan_info.label = electrode_names[chan_ix];
					new_chan_info.type = "EEG";
					new_chan_info.unit = "uV";
					m_devInfo.channel_infos.push_back(new_chan_info);
				}
				delete[] electrode_names;
				electrode_names = NULL;

				if (cfg_dev->AccelerationData)
				{
					chan_info_type new_chan_info;
					new_chan_info.enabled = true;
					new_chan_info.type = "Accelerometer";
					new_chan_info.unit = "g";
					new_chan_info.label = "ACC X";
					m_devInfo.channel_infos.push_back(new_chan_info);

					new_chan_info.label = "ACC Y";
					m_devInfo.channel_infos.push_back(new_chan_info);

					new_chan_info.label = "ACC Z";
					m_devInfo.channel_infos.push_back(new_chan_info);
				}

				if (cfg_dev->Counter)
				{
					chan_info_type new_chan_info;
					new_chan_info.enabled = true;
					new_chan_info.type = "Counter";
					new_chan_info.unit = "samples";
					new_chan_info.label = "Counter";
					m_devInfo.channel_infos.push_back(new_chan_info);
				}

				if (cfg_dev->LinkQualityInformation)
				{
					chan_info_type new_chan_info;
					new_chan_info.enabled = true;
					new_chan_info.type = "LinkQuality";
					new_chan_info.unit = "unknown";
					new_chan_info.label = "Link Quality";
					m_devInfo.channel_infos.push_back(new_chan_info);
				}

				if (cfg_dev->BatteryLevel)
				{
					chan_info_type new_chan_info;
					new_chan_info.enabled = true;
					new_chan_info.type = "BatteryLevel";
					new_chan_info.unit = "unknown";
					new_chan_info.label = "Battery Level";
					m_devInfo.channel_infos.push_back(new_chan_info);
				}

				if (cfg_dev->DigitalIOs)
				{
					chan_info_type new_chan_info;
					new_chan_info.enabled = true;
					new_chan_info.type = "DigitalIOs";
					new_chan_info.unit = "unknown";
					new_chan_info.label = "Digital IOs";
					m_devInfo.channel_infos.push_back(new_chan_info);
				}

				if (cfg_dev->ValidationIndicator)
				{
					chan_info_type new_chan_info;
					new_chan_info.enabled = true;
					new_chan_info.type = "ValidationIndicator";
					new_chan_info.unit = "unknown";
					new_chan_info.label = "Validation Indicator";
					m_devInfo.channel_infos.push_back(new_chan_info);
				}
			}

			// Cleanup device_names used in device-specific API calls
			delete[] device_names;
			device_names = NULL;
		}
		
		// We need to know how many samples we will retrieve per callback and allocate a buffer.
		// = scans_per_block * samples_per_scan;

		// Get the number of devices and the number of samples per 1 scan (aka samples per frame)
		size_t n_scans = 1;
		size_t n_devices_with_channels = 0;
		success &= handleResult("GDS_GetDataInfo",
			GDS_GetDataInfo(m_connectionHandle, &n_scans, NULL, &n_devices_with_channels, &m_devInfo.nsamples_per_scan));

		// Now that we know ndevices_with_channels, get the number of channels in each device.
		std::vector<size_t> channels_per_device(n_devices_with_channels);
		success &= handleResult("GDS_GetDataInfo",
			GDS_GetDataInfo(m_connectionHandle, &n_scans, &channels_per_device[0], &n_devices_with_channels, &m_devInfo.nsamples_per_scan));
				
		// Prepare the buffer with the appropriate amount of data.
		m_dataBuffer.clear();
		m_dataBuffer.resize(m_devInfo.scans_per_block * m_devInfo.nsamples_per_scan);  // Allocate only as much as is needed in a single block.
		// m_dataBuffer.resize(buffer_duration * m_devInfo.nominal_srate * m_devInfo.nsamples_per_scan);  // Allocate only as much as is needed in a single block.

		// TODO: Check that m_devInfo.nsamples_per_scan == channel_sum
		size_t channel_sum = 0;
		for (auto& n : channels_per_device)
			channel_sum += n;

		// Create an LSL outlet.
		// First we need stream info.
		lsl::stream_info gdsInfo(
			m_devInfo.name, "EEG",
			channel_sum, m_devInfo.nominal_srate,
			m_devInfo.channel_format, m_devInfo.name);
		// Append device meta-data
		gdsInfo.desc().append_child("acquisition")
			.append_child_value("manufacturer", "g.Tec")
			.append_child_value("model", m_devInfo.name);
		// Append channel info
		lsl::xml_element channels_element = gdsInfo.desc().append_child("channels");
		for (auto it = m_devInfo.channel_infos.begin(); it < m_devInfo.channel_infos.end(); it++)
		{
			if (it->enabled)
			{
				channels_element.append_child("channel")
					.append_child_value("label", it->label)
					.append_child_value("type", it->type)
					.append_child_value("unit", it->unit);
			}
		}
		this->m_eegOutlet = new lsl::stream_outlet(gdsInfo);
		m_samplesPushed = 0;

		success &= handleResult("GDS_SetDataReadyCallback",
			GDS_SetDataReadyCallback(m_connectionHandle, dataReadyCallback, m_devInfo.scans_per_block, this));
		
		if (m_isCreator)
			success &= handleResult("GDS_StartAcquisition", GDS_StartAcquisition(m_connectionHandle));
		success &= handleResult("GDS_StartStreaming", GDS_StartStreaming(m_connectionHandle));
		m_bStreaming = success;
		
		statusBar()->showMessage("LSL stream created. Waiting for device to return samples...");
		m_pTimer->start(500);
		ui->goPushButton->setText("Stop!");
	}
	ui->goPushButton->setDisabled(false);
}


void MainWindow::on_loadConfigPushButton_clicked()
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


void MainWindow::load_config(const QString filename)
{
    QFile* xmlFile = new QFile(filename);
    if (!xmlFile->open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Could not load XML from file " << filename;
        return;
    }
    QXmlStreamReader* xmlReader = new QXmlStreamReader(xmlFile);
    while (!xmlReader->atEnd() && !xmlReader->hasError()) {
        // Read next element
        xmlReader->readNext();
        if (xmlReader->isStartElement() && xmlReader->name() != "settings")
        {
            QStringRef elname = xmlReader->name();
			if (elname == "server-ip")
				ui->lineEdit_serverip->setText(xmlReader->readElementText());
			else if (elname == "server-port")
				ui->serverPortSpinBox->setValue(xmlReader->readElementText().toInt());
			else if (elname == "client-port")
				ui->clientPortSpinBox->setValue(xmlReader->readElementText().toInt());
			/*
			else if (elname == "noise-reduction")
				ui->noiseCheckBox->setChecked(xmlReader->readElementText().compare("true", Qt::CaseInsensitive) == 0);
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
        }
    }
    if (xmlReader->hasError()) {
        qDebug() << "Config file parse error "
            << xmlReader->error()
            << ": "
            << xmlReader->errorString();
    }
    xmlReader->clear();
	delete xmlReader;
    xmlFile->close();
	delete xmlFile;
}


void MainWindow::on_saveConfigPushButton_clicked()
{
	QString sel = QFileDialog::getOpenFileName(this,
		"Save Configuration File",
		"",//QDir::currentPath()
		"Configuration Files (*.cfg)");
	if (!sel.isEmpty())
	{
		// TODO: Confirm overwrite
		save_config(sel);
	}
}


void MainWindow::save_config(const QString filename)
{
	
}


void MainWindow::enable_config_elements(bool enabled)
{
	ui->lineEdit_serverip->setEnabled(enabled);
	ui->serverPortSpinBox->setEnabled(enabled);
	ui->clientPortSpinBox->setEnabled(enabled);
	/*
	ui->noiseCheckBox->setEnabled(enabled);
	ui->CARCheckBox->setEnabled(enabled);
	ui->accelCheckBox->setEnabled(enabled);
	ui->counterCheckBox->setEnabled(enabled);
	ui->linkCheckBox->setEnabled(enabled);
	ui->batteryCheckBox->setEnabled(enabled);
	ui->digitalCheckBox->setEnabled(enabled);
	ui->validationCheckBox->setEnabled(enabled);
	ui->loadConfigPushButton->setEnabled(enabled);
	*/
}


void MainWindow::notify_samples_pushed()
{
	if (m_samplesPushed > 0)
	{
		QString message = QString("Total samples pushed: ") + QString::number(m_samplesPushed);
		statusBar()->showMessage(message, 500);
	}
}