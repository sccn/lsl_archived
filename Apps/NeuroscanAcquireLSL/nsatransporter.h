#ifndef NSATRANSPORTER_H
#define NSATRANSPORTER_H

#include <QObject>
#include <QTcpSocket>
#include <QThread>
#include <QTimer>
#include <lsl_cpp.h>

#include <vector>
using std::vector;

#include "NeuroscanAcquireProtocol.h"

class NsaTransporter : public QObject {
    Q_OBJECT
public:
    NsaTransporter();

private:
    QTcpSocket* socket;
    NsaPacket packet;
    NsaBasicInfo info;
    QTimer* timer;
    qint64 npackets;
    lsl::stream_info lslinfo;
    lsl::stream_outlet* lsloutlet;

    vector<vector<qint32>> output;

public slots:
    void init();
    void set_host(const QString&, const int&);
    void send(QByteArray);
    void data_handler();
    void error_handler(QAbstractSocket::SocketError);
    void ping();
};

#endif // NSATRANSPORTER_H
