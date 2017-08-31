#include "gdsthread.h"
#include <QDebug>
#include <cmath>


enum runPhase
{
    phase_waitForInput,
    phase_scanForDevices,
    phase_connectToDevices,
    phase_createOutlets,
    phase_transferData,
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


// public functions
GDSThread::GDSThread(QObject *parent)
    : QThread(parent)
    , abort(false)
    , m_bGoOutlets(false)
    , m_pushCounter(0)
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


void GDSThread::setGDSParams(QString host_ip, int16_t host_port, int16_t local_port)
{
    this->mutex.lock();
    //Set member variables passed in as arguments.
    strcpy(this->m_host_endpoint.IpAddress, host_ip.toLatin1().data());
    this->m_host_endpoint.Port = host_port;
    this->m_local_endpoint.Port = local_port;
    this->m_bwaiting_to_scan = true;  // Will cause the thread to transition phases.
    this->mutex.unlock();
}


// Slots
void GDSThread::devicesAddedToStreamList(QStringList transferredList)
{
    qDebug() << "TODO: devicesAddedToStreamList";
    qDebug() << transferredList;
    this->mutex.lock();
    this->m_selectedDeviceNames << transferredList;
    this->m_bwaiting_to_connect = true;
    this->mutex.unlock();
}


void GDSThread::startStreamsRequested()
{
    this->mutex.lock();
    this->m_bwaiting_to_stream = true;
    this->mutex.unlock();
}


// Private functions
bool GDSThread::refreshDevices()
{
    bool success = true;
    GDS_DEVICE_CONNECTION_INFO* connected_devices = NULL;
    size_t count_daq_units = 0;
    this->mutex.lock();  // Probably unnecessary for simply reading endpoints
    handleResult("GDS_GetConnectedDevices",
        GDS_GetConnectedDevices(m_host_endpoint, m_local_endpoint, &connected_devices, &count_daq_units));
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
            GDS_Connect(m_host_endpoint, m_local_endpoint, device_names, n_devices, TRUE, &m_connectionHandle, &is_creator));
        this->mutex.unlock();
        delete[] device_names;
        device_names = NULL;
        emit devicesConnected(success);
        if (!success)
        {
            m_selectedDeviceNames.clear();
        }
        else
        {
            GDS_CONFIGURATION_BASE *deviceConfigurations = NULL;  // API allocates memory for us. We must free it below.
            size_t deviceConfigurationsCount;
            if (handleResult("GDS_GetConfiguration",
                GDS_GetConfiguration(m_connectionHandle, &deviceConfigurations, &deviceConfigurationsCount)))
            {
                qDebug() << "TODO: Handle configurations for " << deviceConfigurationsCount << " devices.";
                for (int dev_ix = 0; dev_ix < deviceConfigurationsCount; dev_ix++)
                {
                    GDS_CONFIGURATION_BASE cfg = deviceConfigurations[dev_ix];
                    if (cfg.DeviceInfo.DeviceType == GDS_DEVICE_TYPE_GNAUTILUS)
                    {
                        GDS_GNAUTILUS_CONFIGURATION* cfg_nautilus = (GDS_GNAUTILUS_CONFIGURATION*)cfg.Configuration;
                        qDebug() << cfg_nautilus->SamplingRate;
                    }
                }
            }

            handleResult("GDS_FreeConfigurationList",
                GDS_FreeConfigurationList(&deviceConfigurations, deviceConfigurationsCount));
        }
    }
    return success;
}

bool GDSThread::createOutlets()
{
    // Safely copy member variables to local variables.
    this->mutex.lock();
    int desiredSRate = 512;
    std::vector<uint32_t> devInds = this->m_streamDeviceIndices;
    this->mutex.unlock();

    // Count channels
    uint32_t nChans = 0;
    QStringList chanLabels;

    // Create lsl pose stream info
    lsl::stream_info gdsInfo(
        "gNEEDaccessData", "EEG",
        nChans, desiredSRate,
        lsl::cf_float32, "_gneedaccesseeg");
    // Append device meta-data
    gdsInfo.desc().append_child("acquisition")
            .append_child_value("manufacturer", "g.Tec")
            .append_child_value("model", "g.NEEDaccess client");
    // Append channel info
    lsl::xml_element poseInfoChannels = gdsInfo.desc().append_child("channels");
    for(auto it=devInds.begin(); it < devInds.end(); it++ )
    {
        QString devStr = QString::number(*it);
        devStr += "_";
        for (int poseAx = 0; poseAx < chanLabels.size(); poseAx++)
        {
            QString chLabel = devStr;
            chLabel.append(chanLabels[poseAx]);
            poseInfoChannels.append_child("channel")
                    .append_child_value("label", chLabel.toStdString())
                    .append_child_value("type", "EEG")
                    .append_child_value("unit", "uV");
        }
    }

    // Create lsl button stream info
    // Up to 16 devices [0-15], non-contiguous vr::EVREventType [100-19999], up to 64 vr::EVRButtonId [0-64]
    lsl::stream_info eventInfo("gNEEDaccessEvents", "Events", 1,
                                lsl::IRREGULAR_RATE, lsl::cf_int32, "_gneedaccessevents");
    // Append device meta-data
    eventInfo.desc().append_child("acquisition")
            .append_child_value("manufacturer", "g.Tec")
            .append_child_value("model", "g.NEEDaccess client");
    // Append channel info
    lsl::xml_element eventInfoChannel = eventInfo.desc().append_child("channels");
    eventInfoChannel.append_child("channel")
        .append_child_value("label", "Event")
        .append_child_value("type", "EventCode")
        .append_child_value("unit", "uint16_code");
    //TODO: MetaData to describe the event code structure.

    this->mutex.lock();
    this->m_eegOutlet = new lsl::stream_outlet(gdsInfo);
    this->m_eventOutlet = new lsl::stream_outlet(eventInfo);
    this->mutex.unlock();

    return true;
}

bool GDSThread::pollAndPush()
{
    bool pushedAny = false;

    this->mutex.lock();
//	int desiredSRate = this->m_srate;
    std::vector<uint32_t> devInds = this->m_streamDeviceIndices;
    this->mutex.unlock();

    // Events go first because we may be spinning waiting for poses later.
    
    // Data
    double sampleTime = lsl::local_clock();
    if ((sampleTime - m_startTime) * 512 > m_pushCounter)
    {
        pushedAny = true;
        m_pushCounter++;
    }
    return pushedAny;
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
        if (this->abort)
            phase = phase_shutdown;
        this->mutex.unlock();

        switch (phase) {
        case phase_waitForInput:
            if (this->m_bwaiting_to_scan)
            {
                phase = phase_scanForDevices;
            }
            else if (this->m_bwaiting_to_connect)
            {
                phase = phase_connectToDevices;
            }
            else if (this->m_bwaiting_to_stream)
            {
                phase = phase_createOutlets;
            }
            break;
        case phase_scanForDevices:
            if (refreshDevices())
            {
                this->mutex.lock();
                this->m_bwaiting_to_scan = false;
                this->mutex.unlock();
                phase = phase_waitForInput;
                this->msleep(1000);
            }
            break;
        case phase_connectToDevices:
            if (connectToDevices())
            {
                this->mutex.lock();
                this->m_bwaiting_to_connect = false;
                this->mutex.unlock();
                phase = phase_waitForInput;
                this->msleep(1000);
            }
            break;
        case phase_createOutlets:
            if (createOutlets())
            {
                this->mutex.lock();
                this->m_bwaiting_to_stream = false;
                this->mutex.unlock();
                emit outletsStarted(true);
                m_startTime = lsl::local_clock();
                phase = phase_transferData;
            }
            else
            {
                phase = phase_shutdown;
            }
            break;
        case phase_transferData:
            if (!this->m_eegOutlet)
            {
                qDebug() << "Something went wrong. m_poseOutlet lost unexpectedly.";
                phase = phase_shutdown;
                break;
            }
            if (!this->m_bGoOutlets)
            {
                qDebug() << "Instructed to stop streaming.";
                this->mutex.lock();
                this->m_eegOutlet = nullptr;
                this->m_eventOutlet = nullptr;
                phase = phase_scanForDevices;
                this->mutex.unlock();
                emit outletsStarted(false);
                break;
            }
            if (!pollAndPush())
            {
                this->usleep(1);
            }
            break;
        case phase_shutdown:
            handleResult("GDS_StopStreaming", GDS_StopStreaming(m_connectionHandle));
            handleResult("GDS_StopAcquisition", GDS_StopAcquisition(m_connectionHandle));
            handleResult("GDS_Disconnect", GDS_Disconnect(&m_connectionHandle));
            GDS_Uninitialize();

            this->mutex.lock();
            this->m_eegOutlet = nullptr;
            this->m_eventOutlet = nullptr;
            this->mutex.unlock();
            //emit outletsStarted(false);
            //emit openvrConnected(false);
            return;
            break;
        }
    }
}
