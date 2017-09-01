#ifndef GDSTHREAD_H
#define GDSTHREAD_H

#include <map>
#include <QMutex>
#include <QThread>
#include <QWaitCondition>
#include "lsl_cpp.h"
#include "GDSClientAPI.h"


class GDSThread : public QThread
{
    Q_OBJECT

public:
    struct GDS_DeviceInfo
    {
        uint32_t index;
        bool inUse;
        QString name;
        GDS_DEVICE_TYPE type;
    };

    GDSThread(QObject *parent = 0);
    ~GDSThread();
    void setGDSParams(QString host_ip, int16_t host_port, int16_t local_port, int16_t update_rate);  // Passes parameters from GUI to GDSThread, sets m_bwaiting_to_scan
    void requestConnectToDevices(QStringList);                                  // Sets m_bwaiting_to_connect
    void startStreams();                                                        // Sets m_bwaiting_to_stream
	void stopStreams();															// Sets m_bwaitingToStop

signals:
    void devicesUpdated(const std::vector<GDSThread::GDS_DeviceInfo>);  // Emitted after a new device is detected.
    void devicesConnected(bool);
    void outletsStarted(bool result);                                   // Emitted after LSL outlets are created.

protected:
    void run() override;

private:
    static bool handleResult(QString calling_func, GDS_RESULT ret);
	static void dataReadyCallback(GDS_HANDLE connectionHandle, void* usrData);

    bool refreshDevices();		// Scan for devices.
    bool connectToDevices();	// Connect to devices.
    bool createOutlets();       // Create the outlets.

    // member variables
    // thread management
    QMutex mutex;
    QWaitCondition condition;
    bool abort = false;
    bool m_bwaitingToScan = false;
    bool m_bwaitingToConnect = false;     
    bool m_bwaitingToStream = false;
    bool m_bwaitingToStop = false;
    // GDS communication
    GDS_HANDLE m_connectionHandle = 0;
    GDS_ENDPOINT m_hostEndpoint = { "127.0.0.1", 50223 };
    GDS_ENDPOINT m_localEndpoint = { "127.0.0.1", 50224 };
    std::vector<GDS_DeviceInfo> m_devices;
    QStringList m_selectedDeviceNames;
	int16_t m_updateRate;
    // LSL variables
	std::vector<std::vector<float>> m_dataBuffer;
    lsl::stream_outlet* m_eegOutlet;
    // lsl::stream_outlet* m_eventOutlet;
};

Q_DECLARE_METATYPE(std::vector<GDSThread::GDS_DeviceInfo>)

#endif // CERELINKTHREAD_H