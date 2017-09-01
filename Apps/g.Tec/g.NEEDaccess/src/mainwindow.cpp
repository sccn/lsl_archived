#include <qfiledialog.h>
#include <QtXml>
#include <qdebug.h>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "GDSClientAPI_gUSBamp.h"
#include "GDSClientAPI_gHIamp.h"
#include "GDSClientAPI_gNautilus.h"

// static functions
bool MainWindow::handleResult(QString calling_func, GDS_RESULT ret)
{
	bool res = true;
	if (ret.ErrorCode)
	{
		qDebug() << "ERROR on " << calling_func << ":" << ret.ErrorMessage;
		res = false;
	}
	return res;
}


void MainWindow::dataReadyCallback(GDS_HANDLE connectionHandle, void* usrData)
{
	MainWindow* thisWin = reinterpret_cast< MainWindow* >(usrData);

	float* dataBuffer = thisWin->m_dataBuffer[0].data();
	size_t bufferSize = thisWin->m_dataBuffer.size() * thisWin->m_dataBuffer[0].size();
	size_t scans_available = 0;  // Read as many scans as possible, up to as many will fit in m_dataBuffer
	handleResult("GDS_GetData in callback",
		GDS_GetData(thisWin->m_connectionHandle, &scans_available, dataBuffer, bufferSize));
	if (scans_available > 0)
	{
		// Copy data to possibly shorter vector
		qDebug() << "Pushed first " << scans_available << " scans from buffer with " << thisWin->m_dataBuffer.size() << " scans.";
		//std::vector<std::vector<float>> newVec(thisWin->m_dataBuffer.begin(), thisWin->m_dataBuffer.begin() + scans_available);
		thisWin->m_eegOutlet->push_chunk(thisWin->m_dataBuffer);
	}
}


MainWindow::MainWindow(QWidget *parent, const QString config_file)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    load_config(config_file);
	GDS_Initialize();
}

MainWindow::~MainWindow()
{
	handleResult("GDS_Disconnect",
		GDS_Disconnect(&m_connectionHandle));
	GDS_Uninitialize();
    delete ui;
}

void MainWindow::on_scanPushButton_clicked()
{
	bool success = true;

    ui->scanPushButton->setText("Scanning...");
    ui->scanPushButton->setDisabled(true);

	strcpy(this->m_hostEndpoint.IpAddress, ui->lineEdit_serverip->text().toLatin1().data());
	this->m_hostEndpoint.Port = ui->serverPortSpinBox->value();
	this->m_localEndpoint.Port = ui->clientPortSpinBox->value();
	this->m_updateRate = ui->updateRateSpinBox->value();

	GDS_DEVICE_CONNECTION_INFO* connected_devices = NULL;
	size_t count_daq_units = 0;
	handleResult("GDS_GetConnectedDevices",
		GDS_GetConnectedDevices(m_hostEndpoint, m_localEndpoint, &connected_devices, &count_daq_units));

	// TODO: Use a map or tuple to keep track of type, inUse, etc.
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

	if (count_devices > 0)
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
	//ui->connectPushButton->setDisabled(false);
}


void MainWindow::on_availableListWidget_itemSelectionChanged()
{
	ui->connectPushButton->setDisabled(false);
}


void MainWindow::on_connectPushButton_clicked()
{
	QString deviceName = ui->availableListWidget->currentItem()->text();
	char(*device_names)[DEVICE_NAME_LENGTH_MAX] = new char[1][DEVICE_NAME_LENGTH_MAX];
	std::strcpy(device_names[0], deviceName.toLatin1().data());
	BOOL is_creator = FALSE;
	bool success = handleResult("GDS_Connect",
		GDS_Connect(m_hostEndpoint, m_localEndpoint, device_names, 1, TRUE, &m_connectionHandle, &is_creator));
	ui->connectPushButton->setDisabled(success);
	ui->goPushButton->setDisabled(!success);
}

void MainWindow::on_goPushButton_clicked()
{
	ui->goPushButton->setDisabled(true);  // re-enable when process complete.
	QString pbtext = ui->goPushButton->text();
	if (pbtext == "Stop!")
	{
		handleResult("GDS_SetDataReadyCallback",
			GDS_SetDataReadyCallback(m_connectionHandle, NULL, 0, this));
		handleResult("GDS_StopStreaming", GDS_StopStreaming(m_connectionHandle));
		handleResult("GDS_StopAcquisition", GDS_StopAcquisition(m_connectionHandle));
		this->m_eegOutlet = nullptr;
		
		ui->goPushButton->setText("Go!");
	}
	else
	{
		bool success = false;
		struct chan_info_type {
			bool enabled;
			std::string label;
			std::string type;
			std::string unit;
		};
		struct dev_info_type {
			std::string name;
			int channel_count = 0;
			double nominal_srate = lsl::IRREGULAR_RATE;
			lsl::channel_format_t channel_format = lsl::cf_float32;
			std::vector<chan_info_type> channel_infos;
		};

		// Get the device configuration(s) so we can prepare the LSL streams.
		GDS_CONFIGURATION_BASE *deviceConfigurations = NULL;
		size_t deviceConfigurationsCount;
		// Note: API allocates memory during GDS_GetConfiguration.
		// We must free it with GDS_FreeConfigurationList.
		if (handleResult("GDS_GetConfiguration",
			GDS_GetConfiguration(m_connectionHandle, &deviceConfigurations, &deviceConfigurationsCount)))
		{
			// Get the number of devices and the number of samples per frame (aka samples per frame)
			size_t n_frames = 1;
			size_t ndevices_with_channels = 0;
			size_t nsamples_per_scan = 0;
			handleResult("GDS_GetDataInfo",
				GDS_GetDataInfo(m_connectionHandle, &n_frames, NULL, &ndevices_with_channels, &nsamples_per_scan));

			// Now that we know ndevices_with_channels, get the number of channels in each device.
			size_t* channels_per_device = new size_t[ndevices_with_channels];
			handleResult("GDS_GetDataInfo",
				GDS_GetDataInfo(m_connectionHandle, &n_frames, channels_per_device, &ndevices_with_channels, &nsamples_per_scan));

			double min_srate = 99999.0;
			for (int dev_ix = 0; dev_ix < deviceConfigurationsCount; dev_ix++)
			{
				dev_info_type dev_info;
				dev_info.name = ui->availableListWidget->currentItem()->text().toStdString();
				dev_info.channel_count = int(channels_per_device[dev_ix]);

				// The API wants an array of char[DEVICE_NAME_LENGTH_MAX] even though these functions are designed for only one device.
				char(*device_names)[DEVICE_NAME_LENGTH_MAX] = new char[1][DEVICE_NAME_LENGTH_MAX];
				std::strcpy(device_names[0], ui->availableListWidget->currentItem()->text().toLatin1().data());

				GDS_CONFIGURATION_BASE cfg = deviceConfigurations[dev_ix];
				if (cfg.DeviceInfo.DeviceType == GDS_DEVICE_TYPE_GUSBAMP)
				{
					qDebug() << "TODO: Get g.USBamp configuration. I do not have a device to test...";
					GDS_GUSBAMP_CONFIGURATION* cfg_dev = (GDS_GUSBAMP_CONFIGURATION*)cfg.Configuration;
					dev_info.nominal_srate = double(cfg_dev->SampleRate);
				}
				else if (cfg.DeviceInfo.DeviceType == GDS_DEVICE_TYPE_GHIAMP)
				{
					qDebug() << "TODO: Get g.HIamp configuration. I do not have a device to test...";
					GDS_GHIAMP_CONFIGURATION* cfg_dev = (GDS_GHIAMP_CONFIGURATION*)cfg.Configuration;
					dev_info.nominal_srate = double(cfg_dev->SamplingRate);
				}
				else if (cfg.DeviceInfo.DeviceType == GDS_DEVICE_TYPE_GNAUTILUS)
				{
					GDS_GNAUTILUS_CONFIGURATION* cfg_dev = (GDS_GNAUTILUS_CONFIGURATION*)cfg.Configuration;
					dev_info.nominal_srate = double(cfg_dev->SamplingRate);
					uint32_t mountedModulesCount = 0;
					size_t electrodeNamesCount = 0;
					handleResult("GDS_GNAUTILUS_GetChannelNames",
						GDS_GNAUTILUS_GetChannelNames(m_connectionHandle, device_names, &mountedModulesCount, NULL, &electrodeNamesCount));
					char(*electrode_names)[GDS_GNAUTILUS_ELECTRODE_NAME_LENGTH_MAX] = new char[electrodeNamesCount][GDS_GNAUTILUS_ELECTRODE_NAME_LENGTH_MAX];
					handleResult("GDS_GNAUTILUS_GetChannelNames", 
						GDS_GNAUTILUS_GetChannelNames(m_connectionHandle, device_names, &mountedModulesCount, electrode_names, &electrodeNamesCount));
					for (int chan_ix = 0; chan_ix < channels_per_device[dev_ix]; chan_ix++)
					{
						chan_info_type new_chan_info;
						new_chan_info.enabled = cfg_dev->Channels[chan_ix].Enabled;
						if (chan_ix < electrodeNamesCount)
						{
							new_chan_info.label = electrode_names[chan_ix];
							new_chan_info.type = "EEG";
							new_chan_info.unit = "uV";
						}
						else
						{
							// TODO: digital / other inputs. Maybe a separate stream?
							new_chan_info.type = "Unknown";
							new_chan_info.unit = "Unknown";
						}
						dev_info.channel_infos.push_back(new_chan_info);
					}
					delete[] electrode_names;
					electrode_names = NULL;
				}
				else
				{
					qDebug() << "Unknown device type.";
				}
				delete[] device_names;
				device_names = NULL;

				// Create stream for this device.
				// Create lsl stream info
				lsl::stream_info gdsInfo(
					"gNEEDaccessData", "EEG",
					dev_info.channel_count, dev_info.nominal_srate,
					dev_info.channel_format, dev_info.name);
				// Append device meta-data
				gdsInfo.desc().append_child("acquisition")
					.append_child_value("manufacturer", "g.Tec")
					.append_child_value("model", "g.NEEDaccess client");
				// Append channel info
				lsl::xml_element channels_element = gdsInfo.desc().append_child("channels");
				for (auto it = dev_info.channel_infos.begin(); it < dev_info.channel_infos.end(); it++)
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
				// this->m_eventOutlet = new lsl::stream_outlet(eventInfo);
				success = true;

				min_srate = (min_srate < dev_info.nominal_srate) ? min_srate : dev_info.nominal_srate;
			}
			delete[] channels_per_device;
			channels_per_device = NULL;

			size_t scans_per_chunk = size_t(min_srate / m_updateRate);
			// std::vector< std::vector<float> > m_dataBuffer(scans_per_chunk, std::vector<float>(nsamples_per_scan, 0));
			m_dataBuffer.resize(scans_per_chunk);
			for (int i = 0; i < scans_per_chunk; ++i)
			{
				m_dataBuffer[i].resize(nsamples_per_scan);
			}

			// TODO: Use a very simple thread to set the callback.
			success &= handleResult("GDS_SetDataReadyCallback",
				GDS_SetDataReadyCallback(m_connectionHandle, this->dataReadyCallback, scans_per_chunk, this));
		}
		success &= handleResult("GDS_FreeConfigurationList",
			GDS_FreeConfigurationList(&deviceConfigurations, deviceConfigurationsCount));

		success &= handleResult("GDS_StartAcquisition", GDS_StartAcquisition(m_connectionHandle));
		success &= handleResult("GDS_StartStreaming", GDS_StartStreaming(m_connectionHandle));

		if (success)
		{
			ui->goPushButton->setText("Stop!");
		}
	}
	ui->goPushButton->setDisabled(false);
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
            if (elname == "todo-setting-xml-element")
            {
                // TODO: Do something with the setting.
            }
        }
    }
    if (xmlReader->hasError()) {
        qDebug() << "Config file parse error "
            << xmlReader->error()
            << ": "
            << xmlReader->errorString();
    }
    xmlReader->clear();
    xmlFile->close();
}