
#pragma once

class ClientCore;


// NatNetClient is a simple Client class for talking to a NatNet Server

class DLLCLASS NatNetClient
{
public:

	NatNetClient();
	~NatNetClient();

	int Initialize(char* szLocalAddress, char* szServerAddress);
	int Uninitialize();
	void NatNetVersion(unsigned char Version[4]);
	void SetVerbosityLevel(int level);

	int SetDataCallback(void (*CallbackFunction)(sFrameOfMocapData* pFrameOfData, void* pUserData), void* pUserData=0);
	int SetMessageCallback(void (*CallbackFunction)(int id, char *szTraceMessage));
	void SendMessage(char* szMessage);
	int SendMessageAndWait(char* szMessage, void** ppServerResponse, int* pResponseSize);
	
	int GetServerDescription(sServerDescription *pServerDescription);
	int GetDataDescriptions(sDataDescriptions** pDataDescriptions);
	sFrameOfMocapData* GetLastFrameOfData();

private:

	ClientCore* m_pClientCore;

};
