#ifndef NEUROSCANACQUIREPROTOCOL_H
#define NEUROSCANACQUIREPROTOCOL_H

#include <QByteArray>
#include <QDataStream>
#include <QDebug>
#include <QtEndian>
enum {
    HeaderIdCtrl = 0x4354524c, // 'CTRL'
    HeaderIdFile = 0x46494c45, // 'FILE'
    HeaderIdData = 0x44415441, // 'DATA'

    GeneralControlCode = 1,
    RequestForVersion = 1,
    ClosingUp,
    ServerDisconnected,
    ServerControlCode = 2,
    StartAcquisition = 1,
    StopAcquisition,
    StartImpedance,
    ChangeSetup,
    DCCorrection,
    ClientControlCode = 3,
    RequestEDFHeader = 1,
    RequestAstFile,
    RequestStartData,
    RequestStopData,
    RequestBasicInfo,

    SetupFile = 1,
    NeuroscanASTFormat = 1,
    CtfDSFormat = 0x43544644, // 'CTFD', BCI2000-specific extension

    DataType_InfoBlock = 1,
    InfoType_Version = 1,
    InfoType_EdfHeader,
    InfoType_BasicInfo,

    DataType_EegData = 2,
    DataTypeRaw16bit = 1,
    DataTypeRaw32bit,
};

class NsaPacket {
private:
    qint32 mId;
    qint16 mCode;
    qint16 mValue;
    qint32 mDataSize;
    QByteArray buffer;

public:
    NsaPacket() : mId(0), mCode(0), mValue(0), mDataSize(0) {}
    NsaPacket(qint32 id, qint16 code, qint16 value) {
        mId = id;
        mCode = code;
        mValue = value;
        mDataSize = 0;
        buffer.clear();
    }

public:
    int headerSize() const { return 12; }
    int dataSize() { return mDataSize; }
    QByteArray& serialize() {
        buffer.clear();
        QDataStream stream(&buffer, QIODevice::WriteOnly);
        stream.setByteOrder(QDataStream::BigEndian);
        stream << mId << mCode << mValue << mDataSize;
        return buffer;
    }
    NsaPacket& unserialize(QByteArray data) {
        QDataStream stream(&data, QIODevice::ReadOnly);
        stream.setByteOrder(QDataStream::BigEndian);
        if (data.size() != headerSize()) {
            qWarning() << "error packet header size:" << data.size();
            return *this;
        }
        stream >> mId >> mCode >> mValue >> mDataSize;
        print();
        return *this;
    }
    QByteArray& getData() { return buffer; }
    void print() {
        qDebug() << hex << mId << mCode << mValue << mDataSize;
        return;
    }
    bool isValid() { return mId && mCode && mValue; }
    void clear() {
        mId = 0;
        mCode = 0;
        mValue = 0;
        mDataSize = 0;
        buffer.clear();
    }
    bool isInfo() { return mCode == DataType_InfoBlock && mValue == InfoType_BasicInfo; }
    int isEEG() { return mCode == DataType_EegData ? mValue : 0; }
};

class NsaBasicInfo {
public:
    qint32 mSizeField;      // Size of structure, used for version control
    qint32 mEEGChannels;    // Number of EEG channels
    qint32 mEventChannels;  // Number of event channels
    qint32 mSamplesInBlock; // Samples in block
    qint32 mSamplingRate;   // Sampling rate (in Hz)
    qint32 mDataDepth;      // 2 for "short", 4 for "int" type of data
    float mResolution;      // Resolution for LSB
    NsaBasicInfo() {}
    NsaBasicInfo(QByteArray& data) {
        if (data.size() != dataSize()) { return; }
        QDataStream stream(&data, QIODevice::ReadOnly);
        stream.setByteOrder(QDataStream::LittleEndian);
        stream.setFloatingPointPrecision(QDataStream::SinglePrecision);
        stream >> mSizeField >> mEEGChannels >> mEventChannels >> mSamplesInBlock >>
            mSamplingRate >> mDataDepth >> mResolution;
    }
    int dataSize() { return sizeof(qint32) * 6 + sizeof(float) * 1; }
};
#endif // NEUROSCANACQUIREPROTOCOL_H
