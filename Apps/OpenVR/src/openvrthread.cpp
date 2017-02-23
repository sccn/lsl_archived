#include "openvrthread.h"
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

QString GetTrackedDeviceString(vr::IVRSystem *pVrsys, vr::TrackedDeviceIndex_t deviceIx)
{
    QString devString = QString::number(deviceIx);
    devString.append(":");

    vr::ETrackedPropertyError *pError = nullptr;
    char pchValue[vr::k_unMaxPropertyStringSize];
    uint32_t nBytes =
            pVrsys->GetStringTrackedDeviceProperty(deviceIx,
                                                   vr::Prop_RenderModelName_String,
                                                   VR_OUT_STRING() pchValue,
                                                   vr::k_unMaxPropertyStringSize,
                                                   pError);
    devString += pchValue;
    devString.append(":");

    nBytes = pVrsys->GetStringTrackedDeviceProperty( deviceIx,
                                                     vr::Prop_ModelNumber_String,
                                                     VR_OUT_STRING() pchValue,
                                                     vr::k_unMaxPropertyStringSize,
                                                     pError);
    devString += pchValue;

    vr::ETrackedDeviceClass devClass = pVrsys->GetTrackedDeviceClass( deviceIx );
    if (devClass == vr::TrackedDeviceClass_HMD)
        devString.append("(HMD)");
    else if (devClass == vr::TrackedDeviceClass_Controller)
    {
        vr::ETrackedControllerRole role =
                pVrsys->GetControllerRoleForTrackedDeviceIndex( deviceIx );
        if (role == vr::TrackedControllerRole_LeftHand)
            devString.append("(L)");
        else if (role == vr::TrackedControllerRole_RightHand)
            devString.append("(R)");
    }
    else if (devClass == vr::TrackedDeviceClass_GenericTracker)
        devString.append("(generic)");

    return devString;
}

void
PrintPose(vr::HmdMatrix34_t pose_mat)
{
	double clk = lsl::local_clock();
	qDebug()
        << std::fmod(1000.0*clk, 1000)
		<< ", "
		<< pose_mat.m[0][0]
		<< ", "
		<< pose_mat.m[0][1]
		<< ", "
		<< pose_mat.m[0][2]
		<< ", "
		<< pose_mat.m[0][3]
		<< ", "
		<< pose_mat.m[1][0]
		<< ", "
		<< pose_mat.m[1][1]
		<< ", "
		<< pose_mat.m[1][2]
		<< ", "
		<< pose_mat.m[1][3]
		<< ", "
		<< pose_mat.m[2][0]
		<< ", "
		<< pose_mat.m[2][1]
		<< ", "
		<< pose_mat.m[2][2]
		<< ", "
		<< pose_mat.m[2][3];
}


OpenVRThread::OpenVRThread(QObject *parent)
    : QThread(parent)
	, abort(false)
	, m_srate(lsl::IRREGULAR_RATE)
	, m_bGoOutlets(false)
	, m_pushCounter(0)
	, m_originIndex(vr::TrackingUniverseSeated)
{
    // Any other initializations
}

OpenVRThread::~OpenVRThread()
{
    mutex.lock();
    abort = true;           // Tell run() loop to stop.
    condition.wakeOne();    // In case thread is sleeping
    mutex.unlock();

    wait();
}

void OpenVRThread::initOpenVR(int srate)
{
    QMutexLocker locker(&mutex);
    //Set member variables passed in as arguments.
    this->m_srate = srate;

    if (!isRunning()) {
        start(HighPriority);
    }
    else
    {
        qDebug() << "OpenVRThread is already running. Disconnecting...";
        this->abort = true;
    }

}

void OpenVRThread::startStreams(QStringList streamDeviceList, int originIndex)
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
	this->m_originIndex = originIndex;
    this->mutex.unlock();
}

bool OpenVRThread::connectToOpenVR()
{
    vr::EVRInitError eError = vr::VRInitError_None;
    vr::IVRSystem *vrsys = vr::VR_Init(&eError, vr::VRApplication_Other);
    if (eError != vr::VRInitError_None)
    {
        vrsys = NULL;
        qDebug() << "Unable to init VR runtime:" << vr::VR_GetVRInitErrorAsEnglishDescription(eError);
        return false;
    }

    if (vrsys->IsInputFocusCapturedByAnotherProcess())
    {
        qDebug() << "Input focus is captured by another process";
        return false;
    }

    this->mutex.lock();
    this->m_vrsys = vrsys;
    this->mutex.unlock();

    return true;
}

void OpenVRThread::refreshDeviceList()
{
    QStringList deviceList;
    std::vector<uint32_t> newDeviceIndices;
    for (vr::TrackedDeviceIndex_t deviceIx=0; deviceIx<vr::k_unMaxTrackedDeviceCount; deviceIx++)
    {
        vr::ETrackedDeviceClass devClass = m_vrsys->GetTrackedDeviceClass( deviceIx );
        if (devClass == vr::TrackedDeviceClass_HMD
                || devClass == vr::TrackedDeviceClass_Controller
                || devClass == vr::TrackedDeviceClass_GenericTracker)
        {
            newDeviceIndices.push_back(deviceIx);
            deviceList << GetTrackedDeviceString(m_vrsys, deviceIx);
        }
    }
    if (newDeviceIndices.size() != m_deviceIndices.size()
            || newDeviceIndices != m_deviceIndices)
    {
        m_deviceIndices = newDeviceIndices;
        emit deviceListUpdated(deviceList);
    }
}

bool OpenVRThread::createOutlets()
{
    // Safely copy member variables to local variables.
    this->mutex.lock();
    int desiredSRate = this->m_srate;
    std::vector<uint32_t> devInds = this->m_streamDeviceIndices;
    this->mutex.unlock();

    // Count channels
    uint32_t nPoseChans = 0;
    for(auto it=devInds.begin(); it < devInds.end(); it++ )
    {
        // Every tracked device gets pose channels -> 12 for full transformation matrix.
        nPoseChans += 12;
    }

    if (nPoseChans <= 0)
    {
        return false;
    }

    QStringList poseChanLabels;
    poseChanLabels
            << "00" << "01" << "02" << "X"
            << "10" << "11" << "12" << "Y"
            << "20" << "21" << "22" << "Z";

    // Create lsl pose stream info
    lsl::stream_info poseInfo(
		"OpenVRPoses", "MoCap",
		nPoseChans, desiredSRate,
		lsl::cf_float32, "_openvrposes");
    // Append device meta-data
    poseInfo.desc().append_child("acquisition")
            .append_child_value("manufacturer", "Valve Software")
            .append_child_value("model", "OpenVR");
    // Append channel info
    lsl::xml_element poseInfoChannels = poseInfo.desc().append_child("channels");
    for(auto it=devInds.begin(); it < devInds.end(); it++ )
    {
        QString devStr = QString::number(*it);
        devStr += "_";
        for (int poseAx = 0; poseAx < poseChanLabels.size(); poseAx++)
        {
			QString chLabel = devStr;
			chLabel.append(poseChanLabels[poseAx]);
            poseInfoChannels.append_child("channel")
                    .append_child_value("label", chLabel.toStdString())
                    .append_child_value("type", "Pose")
                    .append_child_value("unit", "m");
        }
    }

    // Create lsl button stream info
	// Up to 16 devices [0-15], non-contiguous vr::EVREventType [100-19999], up to 64 vr::EVRButtonId [0-64]
    lsl::stream_info eventInfo("OpenVREvents", "Events", 1,
                                lsl::IRREGULAR_RATE, lsl::cf_int32, "_openvrevents");
    // Append device meta-data
	eventInfo.desc().append_child("acquisition")
            .append_child_value("manufacturer", "Valve Software")
            .append_child_value("model", "OpenVR");
    // Append channel info
    lsl::xml_element eventInfoChannel = eventInfo.desc().append_child("channels");
	eventInfoChannel.append_child("channel")
		.append_child_value("label", "OpenVREvent")
		.append_child_value("type", "EventCode")
		.append_child_value("unit", "uint16_code");
	//TODO: MetaData to describe the event code structure.

    this->mutex.lock();
    this->m_poseOutlet = new lsl::stream_outlet(poseInfo);
    this->m_buttonOutlet = new lsl::stream_outlet(eventInfo);
    this->mutex.unlock();

    return true;
}

bool OpenVRThread::pollAndPush(vr::TrackedDevicePose_t *poseBuffers)
{
	bool pushedAny = false;

	this->mutex.lock();
//	int desiredSRate = this->m_srate;
	std::vector<uint32_t> devInds = this->m_streamDeviceIndices;
	this->mutex.unlock();

	// Events go first because we may be spinning waiting for poses later.
	vr::VREvent_t event;
	while (this->m_vrsys->PollNextEvent(&event, sizeof(event)))
	{
		if (std::find(m_streamDeviceIndices.begin(), m_streamDeviceIndices.end(), event.trackedDeviceIndex) != m_streamDeviceIndices.end())
		{
			uint32_t evCode = event.trackedDeviceIndex * 10000000;
			evCode += event.eventType * 100;
			//vr::EVREventType evType = (vr::EVREventType) event.eventType;

			vr::ETrackedDeviceClass devClass = this->m_vrsys->GetTrackedDeviceClass(event.trackedDeviceIndex);
			//TrackedDeviceClass_HMD, TrackedDeviceClass_Controller, TrackedDeviceClass_GenericTracker
			if (devClass == vr::TrackedDeviceClass_Controller)
			{
				evCode += event.data.controller.button;
				//vr::EVRButtonId buttonId = (vr::EVRButtonId) event.data.controller.button;
			}
			const int sample = evCode;
			m_buttonOutlet->push_sample(&sample, lsl::local_clock() - event.eventAgeSeconds);
			pushedAny = true;
		}
	}

	// Poses
	double poseTime = lsl::local_clock();
	if ((poseTime - m_startTime) * m_srate > m_pushCounter)
	{
        std::vector<float> poseSample(vr::k_unMaxTrackedDeviceCount * 3 * 4);
        int deviceCountIx = 0;
		this->m_vrsys->GetDeviceToAbsoluteTrackingPose((vr::ETrackingUniverseOrigin)m_originIndex, 0.0f, VR_ARRAY_COUNT(vr::k_unMaxTrackedDeviceCount) poseBuffers, vr::k_unMaxTrackedDeviceCount);
        poseTime = lsl::local_clock();  // Update poseTime.
        for (auto it = devInds.begin(); it < devInds.end(); it++, deviceCountIx++)
		{
			if (poseBuffers[*it].bPoseIsValid && poseBuffers[*it].bDeviceIsConnected)
			{
				vr::HmdMatrix34_t poseMat = poseBuffers[*it].mDeviceToAbsoluteTracking;
				for (int row_ix = 0; row_ix < 3; row_ix++)
				{
					for (int col_ix = 0; col_ix < 4; col_ix++)
					{
                        poseSample[(4*3*deviceCountIx) + 4*row_ix + col_ix] =
                                poseMat.m[row_ix][col_ix];
					}
				}
			}
		}
        poseSample.resize(devInds.size() * 3 * 4);
        m_poseOutlet->push_sample(poseSample, poseTime);
        pushedAny = true;
        m_pushCounter++;
	}
	return pushedAny;
}

void OpenVRThread::run()
{
    runPhase phase = phase_startLink;
	vr::TrackedDevicePose_t rTrackedDevicePose[vr::k_unMaxTrackedDeviceCount];
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
            if (connectToOpenVR())
            {
                emit openvrConnected(true);
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
            if (!this->m_poseOutlet)
            {
                qDebug() << "Something went wrong. m_poseOutlet lost unexpectedly.";
                phase = phase_shutdown;
				break;
            }
			if (!this->m_bGoOutlets)
			{
				qDebug() << "Instructed to stop streaming.";
				this->mutex.lock();
				this->m_poseOutlet = nullptr;
				this->m_buttonOutlet = nullptr;
				phase = phase_scanForDevices;
				this->mutex.unlock();
				emit outletsStarted(false);
				break;
			}
			if (!pollAndPush(rTrackedDevicePose))
			{
				this->usleep(1);
			}
            break;
        case phase_shutdown:
            vr::VR_Shutdown();
            this->mutex.lock();
            this->m_poseOutlet = nullptr;
            this->m_buttonOutlet = nullptr;
            this->mutex.unlock();
			emit outletsStarted(false);
            emit openvrConnected(false);
            return;
            break;
        }
    }
}
