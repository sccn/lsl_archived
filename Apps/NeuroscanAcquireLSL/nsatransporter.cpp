#include "nsatransporter.h"

NsaTransporter::NsaTransporter() {}

void NsaTransporter::init() {
    npackets = 0;
    socket = new QTcpSocket;
    connect(socket, SIGNAL(readyRead()), this, SLOT(data_handler()));
    connect(socket, SIGNAL(error(QAbstractSocket::SocketError)), this,
            SLOT(error_handler(QAbstractSocket::SocketError)));
    socket->setSocketOption(QAbstractSocket::LowDelayOption, 1);
    timer = new QTimer;
    timer->setInterval(100);
    connect(timer, SIGNAL(timeout()), this, SLOT(ping()));
    return;
}

void NsaTransporter::send(QByteArray data) {
    socket->write(data);
    return;
}

void NsaTransporter::set_host(const QString& host, const int& port) {
    // qDebug()<<host<<port;
    if (port == 0) {
        socket->disconnectFromHost();
        socket->waitForDisconnected();
        timer->stop();
        delete lsloutlet;
        qInfo() << "disconnect from " << socket->peerName() << socket->peerPort();
        return;
    }
    socket->connectToHost(host, port);
    qInfo() << "connect to" << host << port;
    return;
}

void NsaTransporter::data_handler() {
    // qDebug()<<socket->readAll();
    // read header
    if (!packet.isValid() && socket->bytesAvailable() >= packet.headerSize()) {
        packet.unserialize(socket->read(packet.headerSize()));
    }
    // read data
    if (packet.isValid()) {
        // read and process data packet
        if (packet.dataSize() != 0 && socket->bytesAvailable() >= packet.dataSize()) {
            packet.getData() = socket->read(packet.dataSize());

            // process settings packet
            if (packet.isInfo()) {
                info = NsaBasicInfo(packet.getData());
                auto chunk = vector<vector<qint32>>(
                    info.mSamplesInBlock,
                    std::vector<qint32>(info.mEEGChannels + info.mEventChannels));
                output = chunk;
                lslinfo =
                    lsl::stream_info("Neuroscan", "EEG", info.mEEGChannels + info.mEventChannels,
                                     info.mSamplingRate, lsl::cf_int32, "Neuroscan_sdetsff");
                lslinfo.desc()
                    .append_child("settings")
                    .append_child_value("Resolution", std::to_string(info.mResolution));
                lsloutlet = new lsl::stream_outlet(lslinfo);
                timer->start();
                qInfo() << "autoconfig:" << info.mEEGChannels << info.mEventChannels
                        << info.mSamplesInBlock << info.mSamplingRate << info.mDataDepth;
            } else if (packet.isEEG()) {
                // process data packet
                QDataStream stream(&packet.getData(), QIODevice::ReadOnly);
                stream.setByteOrder(QDataStream::LittleEndian);
                for (int i = 0; i < info.mSamplesInBlock; i++) {
                    for (int j = 0; j < info.mEEGChannels + info.mEventChannels; j++) {
                        switch (packet.isEEG()) {
                        case DataTypeRaw16bit:
                            qint16 temp16;
                            stream >> temp16;
                            output[i][j] = temp16;
                            break;
                        case DataTypeRaw32bit:
                            qint32 temp32;
                            stream >> temp32;
                            output[i][j] = temp32;
                            break;
                        default: break;
                        }
                    }
                }
                lsloutlet->push_chunk(output);
            }

            packet.clear();
            npackets++;
        }
        // process control packet
        if (packet.dataSize() == 0) {
            packet.clear();
            npackets++;
        }
    }

    return;
}

void NsaTransporter::error_handler(QAbstractSocket::SocketError) {
    qWarning() << socket->errorString();
    timer->stop();
    return;
}

void NsaTransporter::ping() {
    qInfo() << "transmitted" << npackets << "packets";
    qDebug() << "data in buffer:" << socket->bytesAvailable();
}
