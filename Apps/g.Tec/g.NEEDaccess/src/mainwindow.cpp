#include <iostream>
#include <qfiledialog.h>
#include <QtXml>
#include <QDebug>
#include "mainwindow.h"
#include "ui_mainwindow.h"
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
	GDS_RESULT res = GDS_GetData(thisWin->m_connectionHandle, &scans_available, thisWin->m_dataBuffer, bufferSize);
	if (scans_available > 0)
	{
		thisWin->m_eegOutlet->push_chunk_multiplexed(thisWin->m_dataBuffer, scans_available * thisWin->m_devInfo.nsamples_per_scan);
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
		delete[] m_dataBuffer;
		m_dataBuffer = NULL;
		delete this->m_eegOutlet;
		this->m_eegOutlet = NULL;
	}
	if (m_bConnected)
	{
		handleResult("GDS_Disconnect",
			GDS_Disconnect(&m_connectionHandle));
	}

	GDS_Uninitialize();
    delete ui;
	delete m_pTimer;
	m_pTimer = nullptr;
}


void MainWindow::on_scanPushButton_clicked()
{
	bool success = true;

    ui->scanPushButton->setText("Scanning...");
    ui->scanPushButton->setDisabled(true);

	strcpy(this->m_hostEndpoint.IpAddress, ui->lineEdit_serverip->text().toLatin1().data());
	this->m_hostEndpoint.Port = ui->serverPortSpinBox->value();
	this->m_localEndpoint.Port = ui->clientPortSpinBox->value();	

	GDS_DEVICE_CONNECTION_INFO* connected_devices = NULL;
	size_t count_daq_units = 0;
	handleResult("GDS_GetConnectedDevices",
		GDS_GetConnectedDevices(m_hostEndpoint, m_localEndpoint, &connected_devices, &count_daq_units));

	std::vector<GDS_DeviceInfo> new_devices;
	uint32_t count_devices = 0;
	for (size_t i = 0; i < count_daq_units; i++)
	{
		// Each daq unit operates a different number of devices. The second "for" loop 
		// addresses the devices attached to the daq unit.
		for (size_t j = 0; j < connected_devices[i].ConnectedDevicesLength; j++)
		{
			GDS_DeviceInfo new_info;
			new_info.index = count_devices;
			new_info.inUse = bool(connected_devices[i].InUse) == true;
			new_info.name = connected_devices[i].ConnectedDevices[j].Name;
			new_info.type = connected_devices[i].ConnectedDevices[j].DeviceType;
			new_devices.push_back(new_info);
			count_devices++;
		}
	}
	success &= handleResult("GDS_FreeConnectedDevicesList",
		GDS_FreeConnectedDevicesList(&connected_devices, count_daq_units));

	if (new_devices.size() > 0)
	{
		ui->availableListWidget->clear();
		QStringList newDeviceNames;
		for (auto it = new_devices.begin(); it != new_devices.end(); ++it) {
			ui->availableListWidget->addItem(it->name);
			// TODO: Use other GDS_DeviceInfo fields
		}
		ui->availableListWidget->setCurrentRow(0);
	}
	ui->scanPushButton->setText("Scan");
	ui->scanPushButton->setDisabled(false);
}


void MainWindow::on_availableListWidget_itemSelectionChanged()
{
	ui->connectPushButton->setDisabled(false);
}


void MainWindow::on_connectPushButton_clicked()
{
	ui->goPushButton->setDisabled(true);
	QString pbtext = ui->connectPushButton->text();
	if (pbtext == "Connect")
	{
		QString deviceName = ui->availableListWidget->currentItem()->text();
		// A normal API would ask for N different char arrays, each with DEVICE_NAME_LENGTH_MAX characters.
		// This API instead asks for DEVICE_NAME_LENGTH_MAX char arrays, each with N characters. i.e. it's transposed.
		// So instead of trying to figure out the sane C++ way of conforming to an insane C API, I'm just copying their example code.
		char(*device_names)[DEVICE_NAME_LENGTH_MAX] = new char[1][DEVICE_NAME_LENGTH_MAX];
		std::strcpy(device_names[0], deviceName.toLatin1().data());
		BOOL is_creator = FALSE;
		if (handleResult("GDS_Connect",
			GDS_Connect(m_hostEndpoint, m_localEndpoint, device_names, 1, FALSE, &m_connectionHandle, &is_creator)))
		{
			ui->connectPushButton->setText("Disconnect");
			ui->goPushButton->setDisabled(false);
			enable_config_elements(false);
			m_bConnected = true;
			m_devInfo.is_creator = bool(is_creator);

			if (!m_devInfo.is_creator)
			{
				m_devInfo.is_creator = handleResult("GDS_BecomeCreator",
					GDS_BecomeCreator(m_connectionHandle));
				if (!m_devInfo.is_creator)
					statusBar()->showMessage("Device is first connected by another application. Checkboxes will be ignored.");
			}
			ui->devCfgGrpBox->setEnabled(m_devInfo.is_creator);
		}
		delete[] device_names;
		device_names = NULL;
	}
	else
	{
		bool success = handleResult("GDS_Disconnect",
			GDS_Disconnect(&m_connectionHandle));
		if (success)
		{
			enable_config_elements(true);
			ui->connectPushButton->setText("Connect");
			m_bConnected = false;
		}
		ui->goPushButton->setDisabled(true);
	}
	ui->connectPushButton->setDisabled(false);
}


void MainWindow::on_goPushButton_clicked()
{
	// Lock everything to prevent the GDS thread from stomping on our data during callback.
	QMutexLocker locker(&mutex);  // will unlock itself when function scope resolves.

	ui->goPushButton->setDisabled(true);  // re-enable when process complete.
	QString pbtext = ui->goPushButton->text();
	if (pbtext == "Stop!")
	{
		m_pTimer->stop();
		handleResult("GDS_StopStreaming", GDS_StopStreaming(m_connectionHandle));
		if (m_devInfo.is_creator)
			handleResult("GDS_StopAcquisition", GDS_StopAcquisition(m_connectionHandle));
		handleResult("GDS_SetDataReadyCallback",
			GDS_SetDataReadyCallback(m_connectionHandle, NULL, 0, NULL));
		m_bStreaming = false;
		delete[] m_dataBuffer;
		m_dataBuffer = NULL;
		delete this->m_eegOutlet;
		this->m_eegOutlet = NULL;
		ui->goPushButton->setText("Go!");
	}
	else  //pbtext == "Go!"
	{
		bool success = true;

		// TODO: GDS_SetDataAcquisitionErrorCallback
		// TODO: GDS_SetServerDiedCallback

		// Get the device configuration(s)
		GDS_CONFIGURATION_BASE *deviceConfigurations = NULL;
		size_t deviceConfigurationsCount = 0;
		// Note: API allocates memory during GDS_GetConfiguration. We must free it with GDS_FreeConfigurationList.
		success &= handleResult("GDS_GetConfiguration",
			GDS_GetConfiguration(m_connectionHandle, &deviceConfigurations, &deviceConfigurationsCount));

		// Fill in some of dev_info for the selected device.
		int dev_ix = 0;
		m_devInfo.name = ui->availableListWidget->currentItem()->text().toStdString();
			
		// GDS_<DEVICE>_GetChannelNames requires a device_names argument.
		// The API wants an array of char[DEVICE_NAME_LENGTH_MAX] even though these functions are designed for only one device.
		char(*device_names)[DEVICE_NAME_LENGTH_MAX] = new char[1][DEVICE_NAME_LENGTH_MAX];
		std::strcpy(device_names[0], ui->availableListWidget->currentItem()->text().toLatin1().data());

		GDS_CONFIGURATION_BASE cfg = deviceConfigurations[dev_ix];
		if (cfg.DeviceInfo.DeviceType == GDS_DEVICE_TYPE_NOT_SUPPORTED)
		{
			success = false;
			qDebug() << "Unknown device type.";
		}
		else if (cfg.DeviceInfo.DeviceType == GDS_DEVICE_TYPE_GUSBAMP)
		{
			GDS_GUSBAMP_CONFIGURATION* cfg_dev = (GDS_GUSBAMP_CONFIGURATION*)cfg.Configuration;
			m_devInfo.nominal_srate = double(cfg_dev->SampleRate);
			m_devInfo.scans_per_block = cfg_dev->NumberOfScans;
			qDebug() << "TODO: Get g.USBamp configuration. I do not have a device to test...";
			
			for (size_t chan_ix = 0; chan_ix < GDS_GUSBAMP_CHANNELS_MAX; chan_ix++)
			{
				chan_info_type new_chan_info;
				new_chan_info.enabled = cfg_dev->Channels[chan_ix].Acquire;
				new_chan_info.label = std::to_string(chan_ix + 1);
				new_chan_info.type = "EEG";
				new_chan_info.unit = "uV";
				m_devInfo.channel_infos.push_back(new_chan_info);
			}
		}
		else if (cfg.DeviceInfo.DeviceType == GDS_DEVICE_TYPE_GHIAMP)
		{
			GDS_GHIAMP_CONFIGURATION* cfg_dev = (GDS_GHIAMP_CONFIGURATION*)cfg.Configuration;
			m_devInfo.nominal_srate = double(cfg_dev->SamplingRate);
			m_devInfo.scans_per_block = size_t(cfg_dev->NumberOfScans);
			qDebug() << "TODO: Get g.HIamp configuration. I do not have a device to test...";
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
		else if (cfg.DeviceInfo.DeviceType == GDS_DEVICE_TYPE_GNAUTILUS)
		{
			GDS_GNAUTILUS_CONFIGURATION* cfg_dev = (GDS_GNAUTILUS_CONFIGURATION*)cfg.Configuration;
			m_devInfo.nominal_srate = double(cfg_dev->SamplingRate);
			m_devInfo.scans_per_block = size_t(cfg_dev->NumberOfScans);

			if (m_devInfo.is_creator)
			{
				cfg_dev->CAR = ui->CARCheckBox->isChecked();
				cfg_dev->NoiseReduction = ui->noiseCheckBox->isChecked();
				cfg_dev->AccelerationData = ui->accelCheckBox->isChecked();
				cfg_dev->Counter = ui->counterCheckBox->isChecked();
				cfg_dev->LinkQualityInformation = ui->linkCheckBox->isChecked();
				cfg_dev->BatteryLevel = ui->batteryCheckBox->isChecked();
				cfg_dev->DigitalIOs = ui->digitalCheckBox->isChecked();
				cfg_dev->ValidationIndicator = ui->validationCheckBox->isChecked();
				handleResult("GDS_SetConfiguration",
					GDS_SetConfiguration(m_connectionHandle, &cfg, 1));
			}
			else
			{
				ui->CARCheckBox->setChecked(cfg_dev->CAR);
				ui->noiseCheckBox->setChecked(cfg_dev->NoiseReduction);
				ui->accelCheckBox->setChecked(cfg_dev->AccelerationData);
				ui->counterCheckBox->setChecked(cfg_dev->Counter);
				ui->linkCheckBox->setChecked(cfg_dev->LinkQualityInformation);
				ui->batteryCheckBox->setChecked(cfg_dev->BatteryLevel);
				ui->digitalCheckBox->setChecked(cfg_dev->DigitalIOs);
				ui->validationCheckBox->setChecked(cfg_dev->ValidationIndicator);
			}

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

			// Copy the result into m_devInfo
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
		delete[] device_names;
		device_names = NULL;

		success &= handleResult("GDS_SetDataReadyCallback",
			GDS_SetDataReadyCallback(m_connectionHandle, dataReadyCallback, m_devInfo.scans_per_block, this));

		// Free the resources used by the config
		success &= handleResult("GDS_FreeConfigurationList",
			GDS_FreeConfigurationList(&deviceConfigurations, deviceConfigurationsCount));

		// Get the number of devices and the number of samples per 1 scan (aka samples per frame)
		size_t n_scans = 1;
		size_t ndevices_with_channels = 0;
		success &= handleResult("GDS_GetDataInfo",
			GDS_GetDataInfo(m_connectionHandle, &n_scans, NULL, &ndevices_with_channels, &(m_devInfo.nsamples_per_scan)));

		// Now that we know ndevices_with_channels, get the number of channels in each device.
		size_t* channels_per_device = new size_t[ndevices_with_channels];
		success &= handleResult("GDS_GetDataInfo",
			GDS_GetDataInfo(m_connectionHandle, &n_scans, channels_per_device, &ndevices_with_channels, &(m_devInfo.nsamples_per_scan)));
		m_devInfo.channel_count = int(channels_per_device[dev_ix]);
		delete[] channels_per_device;
		channels_per_device = NULL;

		m_dataBuffer = new float[m_devInfo.scans_per_block * m_devInfo.nsamples_per_scan];  // Allocate only as much as is needed in a single block.
		// m_dataBuffer = new float[buffer_duration * m_devInfo.nominal_srate * m_devInfo.nsamples_per_scan];  // Allocate a longer buffer
		
		// Create outlet.
		lsl::stream_info gdsInfo(
			m_devInfo.name, "EEG",
			m_devInfo.channel_count, m_devInfo.nominal_srate,
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
		statusBar()->showMessage("LSL stream created. Waiting for device to return samples...");
		m_pTimer->start(500);

		if (m_devInfo.is_creator)
			success &= handleResult("GDS_StartAcquisition", GDS_StartAcquisition(m_connectionHandle));
		success &= handleResult("GDS_StartStreaming", GDS_StartStreaming(m_connectionHandle));
		m_bStreaming = success;
		
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


void MainWindow::enable_config_elements(bool enabled)
{
	ui->lineEdit_serverip->setEnabled(enabled);
	ui->serverPortSpinBox->setEnabled(enabled);
	ui->clientPortSpinBox->setEnabled(enabled);
	ui->noiseCheckBox->setEnabled(enabled);
	ui->CARCheckBox->setEnabled(enabled);
	ui->accelCheckBox->setEnabled(enabled);
	ui->counterCheckBox->setEnabled(enabled);
	ui->linkCheckBox->setEnabled(enabled);
	ui->batteryCheckBox->setEnabled(enabled);
	ui->digitalCheckBox->setEnabled(enabled);
	ui->validationCheckBox->setEnabled(enabled);
	ui->loadConfigPushButton->setEnabled(enabled);
}

void MainWindow::notify_samples_pushed()
{
	if (m_samplesPushed > 0)
	{
		QString message = QString("Total samples pushed: ") + QString::number(m_samplesPushed);
		statusBar()->showMessage(message, 500);
	}
}