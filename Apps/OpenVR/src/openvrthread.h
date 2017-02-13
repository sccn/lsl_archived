#ifndef CERELINKTHREAD_H
#define CERELINKTHREAD_H

#include <QMutex>
#include <QThread>
#include <QWaitCondition>
#include "lsl_cpp.h"
#include "openvr.h"

class OpenVRThread : public QThread
{
    Q_OBJECT

public:
	OpenVRThread(QObject *parent = 0);
    ~OpenVRThread();

    void initOpenVR(int srate);                         // Starts the thread. Passes parameters from GUI to OpenVRThread member variables.
    void startStreams(
		QStringList streamDeviceList = QStringList(),
		int originIndex = vr::TrackingUniverseSeated);  // Starts streams for indicated devices. Default is all devices.

signals:
    void openvrConnected(bool result);              // Emitted after successful OpenVR initialization.
    void deviceListUpdated(QStringList deviceList); // Emitted after a new device is detected.
    void outletsStarted(bool result);				// Emitted after LSL outlets are created.

protected:
    void run() override;

private:
    bool connectToOpenVR();     // Initialize OpenVR. If successful, device scanning will begin.
    void refreshDeviceList();   // Scan for devices.
    bool createOutlets();       // Create the outlets.
	bool pollAndPush(vr::TrackedDevicePose_t *poseBuffers);

    QMutex mutex;
    QWaitCondition condition;
    bool abort;
    vr::IVRSystem *m_vrsys;                         // The openvr subsysem.
    int m_srate;                                    // Desired pose sampling rate.
    bool m_bGoOutlets;								// Request to start streams has been made.
    std::vector<uint32_t> m_deviceIndices;          // List of found devices indices.
    std::vector<uint32_t> m_streamDeviceIndices;    // List of device indices for streams.
    lsl::stream_outlet* m_poseOutlet;
    lsl::stream_outlet* m_buttonOutlet;
	uint64_t m_pushCounter;
	double m_startTime;
	int m_originIndex;
};

#endif // CERELINKTHREAD_H
