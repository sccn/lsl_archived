
#pragma once

class ServerCore;


// NatNetServer is a simple server for streaming Mocap data.

class DLLCLASS NatNetServer
{
public:

	NatNetServer();
	~NatNetServer();

	int Initialize(char* szLocalAddress);
	int Initialize(char* szLocalAddress, unsigned short Port);
	int Uninitialize();
	void NatNetVersion(unsigned char Version[4]);
	void SetVerbosityLevel(int level);
	int SetErrorMessageCallback(void (*CallbackFunction)(int id, char *szTraceMessage));
	int SetMessageResponseCallback(int(*CallbackFunction)(sPacket* pIncomingMessage, sPacket* pOutgoingResponse, void* pUserData), void* pUserData = 0);
	int SendPacket(sPacket* pPacket);
	int PacketizeFrameOfMocapData(sFrameOfMocapData* pData, sPacket* pOutPacket);
	int PacketizeDataDescriptions(sDataDescriptions* pDescriptions, sPacket* pOutPacket);

	static int GetLocalIPAddresses(unsigned long Addresses[], int nMax);
	static int IPAddress_LongToString(unsigned long Address, char *str);

private:

	ServerCore* m_pServerCore;
};
