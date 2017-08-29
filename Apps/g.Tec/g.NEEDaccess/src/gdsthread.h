#ifndef GDSTHREAD_H
#define GDSTHREAD_H

#include <QMutex>
#include <QThread>
#include <QWaitCondition>
#include "lsl_cpp.h"
#include <GDSClientAPI_gNautilus.h>

class GDSThread : public QThread
{
    Q_OBJECT

public:
	GDSThread(QObject *parent = 0);
    ~GDSThread();

    void initGDS(QString host_ip, int16_t host_port, int16_t local_port); // Starts the thread. Passes parameters from GUI to GDSThread member variables.
    void startStreams(
		QStringList streamDeviceList = QStringList());  // Starts streams for indicated devices. Default is all devices.

signals:
    void gdsConnected(bool result);              // Emitted after successful GDS initialization.
    void deviceListUpdated(QStringList deviceList); // Emitted after a new device is detected.
    void outletsStarted(bool result);				// Emitted after LSL outlets are created.

protected:
    void run() override;

private:
	static bool handleResult(QString calling_func, GDS_RESULT ret);
    bool connectToGDS();     // Initialize OpenVR. If successful, device scanning will begin.
    void refreshDeviceList();   // Scan for devices.
    bool createOutlets();       // Create the outlets.
	bool pollAndPush();

	// member variables
	// thread management
    QMutex mutex;
    QWaitCondition condition;
    bool abort;
	// GDS communication
	GDS_HANDLE m_connectionHandle = 0;
	GDS_ENDPOINT m_host_endpoint = { "127.0.0.1", 50223 };
	GDS_ENDPOINT m_local_endpoint = { "127.0.0.1", 50224 };
	// LSL variables
    bool m_bGoOutlets;								// Request to start streams has been made.
    std::vector<uint32_t> m_deviceIndices;          // List of found devices indices.
    std::vector<uint32_t> m_streamDeviceIndices;    // List of device indices for streams.
    lsl::stream_outlet* m_eegOutlet;
    lsl::stream_outlet* m_eventOutlet;
	uint64_t m_pushCounter;
	double m_startTime;
	int m_originIndex;
};

#endif // CERELINKTHREAD_H
