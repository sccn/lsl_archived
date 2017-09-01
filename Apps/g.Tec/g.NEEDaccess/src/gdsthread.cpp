#include "gdsthread.h"
#include "GDSClientAPI_gUSBamp.h"
#include "GDSClientAPI_gHIamp.h"
#include "GDSClientAPI_gNautilus.h"
#include <QDebug>
#include <cmath>


enum runPhase
{
    phase_waitForInput,
    phase_scanForDevices,
    phase_connectToDevices,
    phase_startStreaming,
    phase_transferData,
	phase_stopStreaming,
    phase_shutdown
};


// static functions
bool GDSThread::handleResult(QString calling_func, GDS_RESULT ret)
{
    bool res = true;
    if (ret.ErrorCode)
    {
        qDebug() << "ERROR on " << calling_func << ":" << ret.ErrorMessage;
        res = false;
    }
    return res;
}


void GDSThread::dataReadyCallback(GDS_HANDLE connectionHandle, void* usrData)
{
	GDSThread* thisThread = reinterpret_cast< GDSThread* >(usrData);

	float* dataBuffer = thisThread->m_dataBuffer[0].data();
	size_t bufferSize = thisThread->m_dataBuffer.size() * thisThread->m_dataBuffer[0].size();
	size_t scans_available = 0;  // Read as many scans as possible, up to as many will fit in m_dataBuffer
	handleResult("GDS_GetData in callback",
		GDS_GetData(thisThread->m_connectionHandle, &scans_available, dataBuffer, bufferSize));
	if (scans_available > 0)
	{
		// Copy data to shorter vector
		std::vector<std::vector<float>> newVec(thisThread->m_dataBuffer.begin(), thisThread->m_dataBuffer.begin() + scans_available);
		thisThread->m_eegOutlet->push_chunk(newVec);
		qDebug() << "Pushed first " << scans_available << " scans from buffer with " << thisThread->m_dataBuffer.size() << " scans.";
	}
}


// public functions
GDSThread::GDSThread(QObject *parent)
    : QThread(parent)
{
    // Any other initializations
    qRegisterMetaType<std::vector<GDSThread::GDS_DeviceInfo>>();
    QMutexLocker locker(&mutex);
    // Start the thread.
    start();  // HighPriority
}


GDSThread::~GDSThread()
{
    mutex.lock();
    abort = true;           // Tell run() loop to stop.
    condition.wakeOne();    // In case thread is sleeping
    mutex.unlock();

    wait();
}


void GDSThread::setGDSParams(QString host_ip, int16_t host_port, int16_t local_port, int16_t update_rate)
{
    this->mutex.lock();
    //Set member variables passed in as arguments.
    strcpy(this->m_hostEndpoint.IpAddress, host_ip.toLatin1().data());
    this->m_hostEndpoint.Port = host_port;
    this->m_localEndpoint.Port = local_port;
    this->m_bwaitingToScan = true;  // Will cause the thread to transition phases.
	this->m_updateRate = update_rate;
    this->mutex.unlock();
}


void GDSThread::requestConnectToDevices(QStringList deviceList)
{
    this->mutex.lock();
    this->m_selectedDeviceNames << deviceList;
    this->m_bwaitingToConnect = true;
    this->mutex.unlock();
}


void GDSThread::startStreams()
{
    this->mutex.lock();
    this->m_bwaitingToStream = true;
    this->mutex.unlock();
}


void GDSThread::stopStreams()
{
	this->mutex.lock();
	this->m_bwaitingToStop = true;
	this->mutex.unlock();
}


// Private functions, called from within the run loop.
bool GDSThread::refreshDevices()
{
    bool success = true;
    GDS_DEVICE_CONNECTION_INFO* connected_devices = NULL;
    size_t count_daq_units = 0;
    this->mutex.lock();  // Probably unnecessary for simply reading endpoints
    handleResult("GDS_GetConnectedDevices",
        GDS_GetConnectedDevices(m_hostEndpoint, m_localEndpoint, &connected_devices, &count_daq_units));
    this->mutex.unlock();
    
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
    handleResult("GDS_FreeConnectedDevicesList",
        GDS_FreeConnectedDevicesList(&connected_devices, count_daq_units));
    
    bool has_new = new_devices.size() != m_devices.size();
    if (!has_new)
    {
        // Device count is the same. Check to see that the devices are the same.
        for (size_t dev_ix = 0; dev_ix < new_devices.size(); dev_ix++)
        {
            has_new |= new_devices[dev_ix].name != m_devices[dev_ix].name;
            has_new |= new_devices[dev_ix].type != m_devices[dev_ix].type;
            has_new |= new_devices[dev_ix].inUse != m_devices[dev_ix].inUse;
        }
    }
    if (has_new)
    {
        this->mutex.lock();
        m_devices = new_devices;
        this->mutex.unlock();
        emit devicesUpdated(m_devices);
    }
    else
    {
        std::vector<GDS_DeviceInfo> empty;
        emit devicesUpdated(empty);
    }
    return success;
}

bool GDSThread::connectToDevices()
{
    bool success = false;
    this->mutex.lock();
    int n_devices = m_selectedDeviceNames.length();
    if (n_devices > 0)
    {
        char(*device_names)[DEVICE_NAME_LENGTH_MAX] = new char[n_devices][DEVICE_NAME_LENGTH_MAX];
        for (int dev_ix = 0; dev_ix < n_devices; dev_ix++)
        {
            std::strcpy(device_names[dev_ix], m_selectedDeviceNames[dev_ix].toLatin1().data());
        }
        BOOL is_creator = FALSE;
        success |= handleResult("GDS_Connect",
            GDS_Connect(m_hostEndpoint, m_localEndpoint, device_names, n_devices, TRUE, &m_connectionHandle, &is_creator));
        this->mutex.unlock();
        delete[] device_names;
        device_names = NULL;
        if (!success)
        {
            m_selectedDeviceNames.clear();
        }
    }
    emit devicesConnected(success);
    return success;
}

bool GDSThread::createOutlets()
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
		size_t* channels_per_device= new size_t[ndevices_with_channels];
		handleResult("GDS_GetDataInfo",
			GDS_GetDataInfo(m_connectionHandle, &n_frames, channels_per_device, &ndevices_with_channels, &nsamples_per_scan));

		double min_srate = 99999.0;
        for (int dev_ix = 0; dev_ix < deviceConfigurationsCount; dev_ix++)
        {
            dev_info_type dev_info;
			dev_info.name = m_selectedDeviceNames[dev_ix].toStdString();
			dev_info.channel_count = int(channels_per_device[dev_ix]);

			// The API wants an array of char[DEVICE_NAME_LENGTH_MAX] even though these functions are designed for only one device.
			char(*device_names)[DEVICE_NAME_LENGTH_MAX] = new char[1][DEVICE_NAME_LENGTH_MAX];
			std::strcpy(device_names[0], m_selectedDeviceNames[dev_ix].toLatin1().data());

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
				GDS_GNAUTILUS_GetChannelNames(m_connectionHandle, device_names, &mountedModulesCount, NULL, &electrodeNamesCount);
				char(*electrode_names)[GDS_GNAUTILUS_ELECTRODE_NAME_LENGTH_MAX] = new char[electrodeNamesCount][GDS_GNAUTILUS_ELECTRODE_NAME_LENGTH_MAX];
				GDS_GNAUTILUS_GetChannelNames(m_connectionHandle, device_names, &mountedModulesCount, electrode_names, &electrodeNamesCount);
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
			this->mutex.lock();
			this->m_eegOutlet = new lsl::stream_outlet(gdsInfo);
			// this->m_eventOutlet = new lsl::stream_outlet(eventInfo);
			this->mutex.unlock();
			success = true;

			min_srate = (min_srate < dev_info.nominal_srate) ? min_srate : dev_info.nominal_srate;
        }
		delete[] channels_per_device;
		channels_per_device = NULL;

		size_t scans_per_chunk = size_t(min_srate / m_updateRate);
		this->mutex.lock();
		// std::vector< std::vector<float> > m_dataBuffer(scans_per_chunk, std::vector<float>(nsamples_per_scan, 0));
		m_dataBuffer.resize(scans_per_chunk);
		for (int i = 0; i < scans_per_chunk; ++i)
		{
			m_dataBuffer[i].resize(nsamples_per_scan);
		}
		this->mutex.unlock();
		qDebug() << "GDS_SetDataReadyCallback with " << scans_per_chunk << " scans_per_chunk";
		success &= handleResult("GDS_SetDataReadyCallback",
			GDS_SetDataReadyCallback(m_connectionHandle, this->dataReadyCallback, scans_per_chunk, this));
    }
	success &= handleResult("GDS_FreeConfigurationList",
        GDS_FreeConfigurationList(&deviceConfigurations, deviceConfigurationsCount));

	success &= handleResult("GDS_StartAcquisition", GDS_StartAcquisition(m_connectionHandle));
	success &= handleResult("GDS_StartStreaming", GDS_StartStreaming(m_connectionHandle));

    return success;
}


void GDSThread::run()
{
    runPhase phase = phase_waitForInput;
    // Thread-safe copy member variables to local variables.
    // this->mutex.lock();
    // this->mutex.unlock();

    GDS_Initialize();

    forever {
        this->mutex.lock();
        if (this->abort && phase != phase_shutdown)
            phase = phase_stopStreaming;
        this->mutex.unlock();

        switch (phase) {
        case phase_waitForInput:
            if (this->m_bwaitingToScan)
            {
                phase = phase_scanForDevices;
            }
            else if (this->m_bwaitingToConnect)
            {
                phase = phase_connectToDevices;
            }
            else if (this->m_bwaitingToStream)
            {
                phase = phase_startStreaming;
            }
			else if (this->m_bwaitingToStop)
			{
				phase = phase_stopStreaming;
			}
            break;
        case phase_scanForDevices:
            if (refreshDevices())
            {
                this->mutex.lock();
                this->m_bwaitingToScan = false;
                this->mutex.unlock();
                phase = phase_waitForInput;
                this->msleep(1000);
            }
            break;
        case phase_connectToDevices:
            if (connectToDevices())
            {
                this->mutex.lock();
                this->m_bwaitingToConnect = false;
                this->mutex.unlock();
                phase = phase_waitForInput;
                this->msleep(1000);
            }
            break;
        case phase_startStreaming:
            if (createOutlets())
            {
                this->mutex.lock();
                this->m_bwaitingToStream = false;
                this->mutex.unlock();
                emit outletsStarted(true);
                phase = phase_transferData;
            }
            else
            {
                phase = phase_stopStreaming;
            }
            break;
        case phase_transferData:
            if (this->m_bwaitingToStop)
            {
                phase = phase_stopStreaming;
            }
			else
			{
				this->usleep(1000);
			}
            break;
		case phase_stopStreaming:
			handleResult("GDS_StopStreaming", GDS_StopStreaming(m_connectionHandle));
			handleResult("GDS_StopAcquisition", GDS_StopAcquisition(m_connectionHandle));
			this->mutex.lock();
			this->m_eegOutlet = nullptr;
			// this->m_eventOutlet = nullptr;
			this->mutex.unlock();
			emit outletsStarted(false);
			this->m_bwaitingToStop = false;
			if (this->abort)
			{
				phase = phase_shutdown;
			}
			else
			{
				phase = phase_waitForInput;
			}
			break;
        case phase_shutdown:
            handleResult("GDS_Disconnect", GDS_Disconnect(&m_connectionHandle));
            GDS_Uninitialize();
            emit devicesConnected(false);
            return;
            break;
        }
    }
}
