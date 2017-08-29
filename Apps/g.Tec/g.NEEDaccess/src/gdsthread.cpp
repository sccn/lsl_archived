#include "gdsthread.h"
#include <QDebug>
#include <cmath>

enum runPhase
{
    phase_startLink,
    phase_scanForDevices,
    phase_createOutlets,
    phase_transferData,
    phase_shutdown
};

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

GDSThread::GDSThread(QObject *parent)
    : QThread(parent)
	, abort(false)
	, m_bGoOutlets(false)
	, m_pushCounter(0)
{
    // Any other initializations
}

GDSThread::~GDSThread()
{
    mutex.lock();
    abort = true;           // Tell run() loop to stop.
    condition.wakeOne();    // In case thread is sleeping
    mutex.unlock();

    wait();
}

void GDSThread::initGDS(QString host_ip, int16_t host_port, int16_t local_port)
{
    QMutexLocker locker(&mutex);
    //Set member variables passed in as arguments.
	strcpy(this->m_host_endpoint.IpAddress, host_ip.toLatin1().data());
	this->m_host_endpoint.Port = host_port;
	this->m_local_endpoint.Port = local_port;

    if (!isRunning()) {
        start(HighPriority);
    }
    else
    {
        qDebug() << "GDSThread is already running. Disconnecting...";
        this->abort = true;
    }

}

void GDSThread::startStreams(QStringList streamDeviceList)
{
	std::vector<uint32_t> newStreamDeviceIndices;
	if (!this->m_bGoOutlets)
	{
		if (streamDeviceList.length() == 0)
		{
			// No devices were selected. Stream all devices.
			newStreamDeviceIndices = m_deviceIndices;
		}
		else
		{
			for (QStringList::iterator it = streamDeviceList.begin();
				it != streamDeviceList.end(); ++it)
			{
				QStringList pieces = it->split(":");
				QString strIx = pieces.value(0);
				newStreamDeviceIndices.push_back(strIx.toInt());
			}
		}
	}
    this->mutex.lock();
	this->m_bGoOutlets = !this->m_bGoOutlets;
    this->m_streamDeviceIndices = newStreamDeviceIndices;
    this->mutex.unlock();
}

bool GDSThread::connectToGDS()
{
	bool result = true;
	GDS_Initialize();
    return result;
}

void GDSThread::refreshDeviceList()
{
	GDS_DEVICE_CONNECTION_INFO* connected_devices = NULL;
	size_t count_daq_units = 0;
	this->mutex.lock();  // Probably unnecessary for simply reading endpoints
	handleResult("GDS_GetConnectedDevices",
		GDS_GetConnectedDevices(m_host_endpoint, m_local_endpoint, &connected_devices, &count_daq_units));
	this->mutex.unlock();
	
    QStringList deviceList;
    std::vector<uint32_t> newDeviceIndices;
	std::vector<bool> newDeviceInUse;
	std::vector<GDS_DEVICE_TYPE> newDeviceType;
	size_t count_devices = 0;
	for (size_t i = 0; i < count_daq_units; i++)
	{
		// Each daq unit operates a different number of devices. The second "for" loop 
		// addresses the devices attached to the daq unit.
		for (size_t j = 0; j < connected_devices[i].ConnectedDevicesLength; j++)
		{
			newDeviceIndices.push_back(uint32_t(count_devices));
			newDeviceInUse.push_back(connected_devices[i].InUse == TRUE);
			newDeviceType.push_back(connected_devices[i].ConnectedDevices[j].DeviceType);
			deviceList << connected_devices[i].ConnectedDevices[j].Name;
			count_devices++;
		}
	}
	handleResult("GDS_FreeConnectedDevicesList",
		GDS_FreeConnectedDevicesList(&connected_devices, count_daq_units));
    
    if (newDeviceIndices.size() != m_deviceIndices.size()
            || newDeviceIndices != m_deviceIndices)
    {
		this->mutex.lock();
        m_deviceIndices = newDeviceIndices;
		this->mutex.unlock();
        emit deviceListUpdated(deviceList);
    }
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
    runPhase phase = phase_startLink;
	// Thread-safe copy member variables to local variables.
    this->mutex.lock();
    this->mutex.unlock();

    forever {
        this->mutex.lock();
        if (this->abort)
            phase = phase_shutdown;
        this->mutex.unlock();

        switch (phase) {
        case phase_startLink:
            if (connectToGDS())
            {
                //emit openvrConnected(true);
                phase = phase_scanForDevices;
            }
            else
            {
                phase = phase_shutdown;
            }
            break;
        case phase_scanForDevices:
            if (this->m_bGoOutlets)
            {
                phase = phase_createOutlets;
            }
            else
            {
                refreshDeviceList();
                this->msleep(100);
            }
            break;
        case phase_createOutlets:
            if (createOutlets())
            {
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
            //vr::VR_Shutdown();
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
