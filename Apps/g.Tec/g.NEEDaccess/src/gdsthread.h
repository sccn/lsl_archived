#ifndef GDSTHREAD_H
#define GDSTHREAD_H

#include <map>
#include <QMutex>
#include <QThread>
#include <QWaitCondition>
#include "lsl_cpp.h"
#include <GDSClientAPI_gNautilus.h>


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
    void setGDSParams(QString host_ip, int16_t host_port, int16_t local_port);  // Passes parameters from GUI to GDSThread member variables.

signals:
    void devicesUpdated(const std::vector<GDSThread::GDS_DeviceInfo>); // Emitted after a new device is detected.
    void devicesConnected(bool);
    void outletsStarted(bool result);				// Emitted after LSL outlets are created.

public slots:
    void devicesAddedToStreamList(QStringList);
    void startStreamsRequested();

protected:
    void run() override;

private:
    static bool handleResult(QString calling_func, GDS_RESULT ret);

    bool refreshDevices();		// Scan for devices.
    bool connectToDevices();	// Connect to devices.
    bool createOutlets();       // Create the outlets.
    bool pollAndPush();

    // member variables
    // thread management
    QMutex mutex;
    QWaitCondition condition;
    bool abort;
    bool m_bwaiting_to_scan = false;
    bool m_bwaiting_to_connect = false;
    bool m_bwaiting_to_stream = false;
    bool m_bGoOutlets;								// Request to start streams has been made.
    bool m_bdevices_updated = false;
    // GDS communication
    GDS_HANDLE m_connectionHandle = 0;
    GDS_ENDPOINT m_host_endpoint = { "127.0.0.1", 50223 };
    GDS_ENDPOINT m_local_endpoint = { "127.0.0.1", 50224 };
    std::vector<GDS_DeviceInfo> m_devices;
    QStringList m_selectedDeviceNames;
    // LSL variables
    std::vector<uint32_t> m_deviceIndices;          // List of found devices indices.
    std::vector<uint32_t> m_streamDeviceIndices;    // List of device indices for streams.
    lsl::stream_outlet* m_eegOutlet;
    lsl::stream_outlet* m_eventOutlet;
    uint64_t m_pushCounter;
    double m_startTime;
    int m_originIndex;
};

Q_DECLARE_METATYPE(std::vector<GDSThread::GDS_DeviceInfo>)

#endif // CERELINKTHREAD_H