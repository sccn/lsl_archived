
#pragma once

#include "NatNetTypes.h"

class ClientCore;

// Data callback function prototype
typedef void ( *funcDataCallback) (sFrameOfMocapData* pFrameOfData, void* pUserData);

// NatNetClient is a complete C++ class for connecting to NatNet server applications,
// such as NaturalPoint Arena and NaturalPoint TrackingTools. 
class DLLCLASS NatNetClient
{
public:

    NatNetClient();
    NatNetClient(int iType);
    ~NatNetClient();

    int Initialize(char* szLocalAddress, char* szServerAddress);
    int Initialize(char* szLocalAddress, char* szServerAddress, int HostCommandPort);
    int Initialize(char* szLocalAddress, char* szServerAddress, int HostCommandPort, int HostDataPort);
    int Uninitialize();
    void NatNetVersion(unsigned char Version[4]);
    void SetVerbosityLevel(int level);

    int SetDataCallback(void (*CallbackFunction)(sFrameOfMocapData* pFrameOfData, void* pUserData), void* pUserData=0);
    int SetMessageCallback(void (*CallbackFunction)(int id, char *szTraceMessage));
    void SendMessage(char* szMessage);
    void SendMessage1(char* szMessage);
    int SendMessageAndWait(char* szMessage, void** ppServerResponse, int* pResponseSize);
    int SendMessageAndWait(char* szMessage, int tries, int timeout, void** ppServerResponse, int* pResponseSize);
    
    int GetServerDescription(sServerDescription *pServerDescription);
    int GetDataDescriptions(sDataDescriptions** pDataDescriptions);
    sFrameOfMocapData* GetLastFrameOfData();

    void SetMulticastAddress(char* szMulticast);

    bool DecodeTimecode(unsigned int inTimecode, unsigned int inTimecodeSubframe, int* hour, int* minute, int* second, int* frame, int* subframe);
    bool TimecodeStringify(unsigned int inTimecode, unsigned int inTimecodeSubframe, char *Buffer, int BufferSize);
    void DecodeID(unsigned int inID, int* entityID, int* memeberID);

private:

    ClientCore* m_pClientCore;
    int m_iConnectionType;

};
