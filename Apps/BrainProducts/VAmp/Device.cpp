// MODULE: Device.cpp
//: written by: Phuong Nguyen
//+       date: 08-Mar-2005
//+
//+ Description:
//+		Demonstrate how to use library FirstAmp.lib in order to acquire data.
//.

#include "stdafx.h"
#include <mmsystem.h>
#include <mbstring.h>
#include <atlsafe.h>
#include "Device.h"
#include "FirstAmpDefs.h"


#include <ATLComTime.h> // Test for Simulink
#include <atlcom.h>

#define MAX_ALLOWED_DEVICES			4
#define DEVICE_GET_DATA_INTERVAL	20		// 20 >> 40ms data service interval(Test Software).
											// If you change this, please also change
											// the ringbuffer size.
											// NOTE: 20ms data service block because of correcting trigger 
											// and driver doesn't let to fetch data to much at one time.

#define DEVICE_SERVE_TIMEOUT		5000	// Idle time 5 sec.
#define DEVICE_BUFTIME				4		// 4 seconds of local ring buffer.

#define MIN_SAMPLING_RATE			100		// Min. Sampling Rate 100 Hz because 
											// of 20 x 100 / 1000 = 2 PointsPerBlock.

#define BIT(x)						(1 << x)


template <class T> void ProcessData(T& data, int nSystemStructureCounter, 
		int nEegMax, int nAuxMax, float *pfData, float *pfTrigger, 
		bool bCalibration)
	//: Map / reorganize the raw data into interleaved format. 
	//.	Each structure contains 1 sample of channel.
{
	float* pData = pfData;
	float* pTrig = pfTrigger;
		
	// copy data to arranged buffer
	for (int i = 0; i < nSystemStructureCounter; i++)
	{
		// EEG channels.
		for (int n=0; n < nEegMax; n++)
		{
			*(pData++) = float(data[i].Main[n] - (bCalibration ? 
				0 : data[i].Main[nEegMax]));
		}
		// AUX channels.
		for (int n=0; n < nAuxMax; n++)
		{
			*(pData++) = float(data[i].Aux[n]);
		}
		// Trigger channel.
		USHORT nTrigger = (data[i].Status >> 8) & 0x1;
		nTrigger |= (data[i].Status & 0xFF) << 1;
		*(pTrig++) = float(nTrigger);
	}
}

template <class T> void ProcessHighspeedData(T& data, int nSystemStructureCounter, 
		int nEegMax, float *pfData, float *pfTrigger)
	//: Map / reorganize the raw data into interleaved format. 
	//.	Each structure contains 1 sample of channel.
{
	float* pData = pfData;
	float* pTrig = pfTrigger;
	
	// copy data to arranged buffer
	for (int i = 0; i < nSystemStructureCounter; i++)
	{
		// EEG channels.
		for (int n=0; n < nEegMax; n++)
		{
			*(pData++) = float(data[i].Main[n]);
		}
		// Trigger channel.
		USHORT nTrigger = (data[i].Status >> 8) & 0x1;
		nTrigger |= (data[i].Status & 0xFF) << 1;
		*(pTrig++) = float(nTrigger);
	}
}

///////////////////////////////////////////////////////////
// class CChannelInfo
///////////////////////////////////////////////////////////
CDevice::CChannelInfo::CChannelInfo()
: nType(DEVICE_CHAN_TYPE_UNKNOWN)
, nPhysIdx(0)
, szLabel("")
, dResolution(1.0)
, bUsrUnit(FALSE)
, szUnitName("")
, fGradient(1.0f)
, fOffset(0.0f)
{
}

const CDevice::CChannelInfo & 
	CDevice::CChannelInfo::operator = (const CChannelInfo &c2)
{
	nPhysIdx = c2.nPhysIdx;
	szLabel = c2.szLabel;
	bUsrUnit = c2.bUsrUnit;
	szUnitName = c2.szUnitName;
	fGradient = c2.fGradient;
	fOffset = c2.fOffset;
	nType = c2.nType;
	return *this;
}

const BOOL CDevice::CChannelInfo::operator == (const CChannelInfo &c2)
{
	return (szLabel == c2.szLabel && 
		nPhysIdx == c2.nPhysIdx) ? TRUE : FALSE;
}

/////////////////////////////////////////////////////////////////////////
// class CDevice
/////////////////////////////////////////////////////////////////////////
CDevice CDevice::m_device; // Exists only once as static member variable.


CDevice::CDevice()
: m_bRun(FALSE)
, m_hEvent(NULL)
, m_hThread(NULL)
, m_bOpen(FALSE)
, m_nDeviceId(FA_ID_INVALID)
, m_nStartMode(IAC_AT_DATA)
, m_nEEGChannels(0)
, m_nAUXChannels(0)
, m_nRingBufferMax(0)
, m_nPointsPerBlock(0)
, m_nSamples(0)
, m_nWritePos(0)
, m_nReadPos(0)
, m_bBufferOverflow(FALSE)
, m_nErrorSeverity(0)
, m_BlockTime(0)
, m_Date(0)
, m_nSamplingRate(2000) 
, m_DataMode(dmNormal)
, m_pFilterAntiAliasing(NULL)
, m_nChannelMode(1)
{	// Reset also in Close() function.
	memset(&m_DeviceInfo, 0, sizeof(m_DeviceInfo));
	memset(&m_DeviceProp, 0, sizeof(m_DeviceProp));
	memset(&m_Settings, 0, sizeof(m_Settings));
	m_tblBuffer.clear(); m_tblChanInfo.clear();
	m_hLib = FALSE;

#ifdef _WIN64
		MyLoadLibary("FirstAmp64.dll");
#else
		MyLoadLibary("FirstAmp32.dll");
#endif
	
}

CDevice::~CDevice()
{
	Close();
	MyUnLoadLibary();
}

CDevice* CDevice::GetInstance()
{
	return &m_device;
}


// **** for configuration ****************************
STDMETHODIMP CDevice::Open()
{
	FUNCTION_DEVICE_BEGIN
	
	CComCritSecLock<CComAutoCriticalSection> singleLock(m_CrSec);
	if (m_bOpen) return S_OK;

	UINT nDevCnt = FA_ID_INVALID;
	int nRetries = 0;
	while (nRetries++ < 5)
	{
		nDevCnt =  FAGetCount();	// Get the last opened Device id.
		if (nDevCnt < 1) 
		{
			Sleep(500);
		}
		else
		{
			break;
		}
	}
	if (nDevCnt == FA_ID_INVALID) 
	{
		ATLTRACE("> CDevice: faGetCount FAILED m_bOpen(%d).\n", m_bOpen);
		goto Finit;
	}

	ATLTRACE("> CDevice: faGetCount(%d), Id = %u.\n", nDevCnt, m_nDeviceId);
	m_nDeviceId = FAGetId(nDevCnt -1); 
	if (m_nDeviceId != FA_ID_INVALID)
	{
		ATLTRACE("> CDevice: Device found with ID(%u).\n", m_nDeviceId);
	}
	else 		
	{
		ATLTRACE("> CDevice: faGetId(%d) FAILED.\n", m_nDeviceId);
		goto Finit;
	}
	// Open the last device id.	
	if (FAOpen(m_nDeviceId) != FA_ERR_OK) 
	{
		ATLTRACE("> CDevice: faOpen(%d) FAILED.\n", m_nDeviceId);
		goto Finit;
	}
	// Retrieves the device information.
	memset(&m_DeviceInfo, 0, sizeof(m_DeviceInfo));
	if (FAGetInformation(m_nDeviceId, &m_DeviceInfo) != FA_ERR_OK) 
	{
		ATLTRACE("> CDevice: faGetInformation(%d) FAILED.\n", m_nDeviceId);
		goto Finit;
	}
	ATLTRACE("> CDevice: faGetInformation %s.\n", 
		m_DeviceInfo.Model ? "16 channels" : "8 channels");
	switch (m_DeviceInfo.Model)
	{	
	case FA_MODEL_8:
		m_nChannelMode = DEVICE_CHANMODE_8;
		m_nEEGChannels = FA_MODEL_8_CHANNELS_MAIN - 1; // without trigger
		m_nAUXChannels = FA_MODEL_8_CHANNELS_AUX;
		m_tblChanInfo.resize(m_nEEGChannels + m_nAUXChannels + 1); // 1 Trigger.
		break;
	case FA_MODEL_16:
		m_nChannelMode = DEVICE_CHANMODE_16;
		m_nEEGChannels = FA_MODEL_16_CHANNELS_MAIN - 1; // without trigger
		m_nAUXChannels = FA_MODEL_16_CHANNELS_AUX;
		m_tblChanInfo.resize(m_nEEGChannels + m_nAUXChannels + 1); // 1 Trigger.
		break;
	default: // Unknow device (error).
		ATLTRACE("> CDevice: Unknown Model(%d) FAILED.\n", m_nDeviceId);
		goto Finit;
	}
	// Retrieves device properties.
	memset(&m_DeviceProp, 0, sizeof(m_DeviceProp));
	if (FAGetProperty(m_nDeviceId, &m_DeviceProp) != FA_ERR_OK) 
	{
		ATLTRACE("> CDevice: faGetProperty(%d) FAILED.\n", m_nDeviceId);
		goto Finit;
	}
	// Highspeed ?
	if (m_nStartMode != IAC_AT_IMPEDANCE && m_DataMode != dmNormal)
	{
		m_nChannelMode = DEVICE_CHANMODE_4;
		m_nEEGChannels = FA_MODE_20_KHZ_CHANNELS_MAIN; // EEG
		m_nAUXChannels = 0;	// no AUX.
		m_tblChanInfo.resize(m_nEEGChannels + m_nAUXChannels + 1); // 1 Trigger.
	}
	// Channel type, channel resolution.
	for (UINT i = 0; i < m_tblChanInfo.size(); i++)
	{
		CDevice::CChannelInfo& ci = m_tblChanInfo[i];
		if (i < UINT(m_nEEGChannels)) // EEG
		{
			ci.nType = DEVICE_CHAN_TYPE_EEG;
			ci.dResolution = double(m_DeviceProp.ResolutionEeg * 1e6); // V > µV
		}
		else if (m_nAUXChannels > 0 && // AUX present
			i >= UINT(m_nEEGChannels) && i < m_tblChanInfo.size() -1 ) // AUX
		{
			ci.nType = DEVICE_CHAN_TYPE_AUX;
			ci.dResolution = double(m_DeviceProp.ResolutionAux * 1e6); // V > µV
		}
		else // Digital port.
		{
			ci.nType = DEVICE_CHAN_TYPE_TRIGGER;
			ci.dResolution = 1.0f;
		}
	}
	if (GetMaxSampleRate() < 100) // Sampling rate at least 100 Hz.
	{
		ATLTRACE("> CDevice: GetMaxSampleRate(%d) FAILED.\n", m_nDeviceId);
		goto Finit;
	}
	hr = S_OK;
	m_bOpen = TRUE;
	ATLTRACE("> CDevice: Open SUCCESS.\n");

Finit:
	// case of error.
	if (hr != S_OK && m_nDeviceId != FA_ID_INVALID)
		Close();
	FUNCTION_DEVICE_END
}

STDMETHODIMP CDevice::Close()
{
	FUNCTION_DEVICE_BEGIN
	
	CComCritSecLock<CComAutoCriticalSection> singleLock(m_CrSec);

	if (m_hEvent) CloseHandle(m_hEvent); 
	m_hEvent = NULL;
	m_bRun = FALSE; // Force serving thread exit aquiring data.
	if (m_hThread != NULL)
	{
		singleLock.Unlock();
		if (WaitForSingleObject(m_hThread, 3000) == WAIT_TIMEOUT)
		{
			TerminateThread(m_hThread, -1);
		}
		CloseHandle(m_hThread);
		m_hThread = NULL;
	}
	if (m_nDeviceId != FA_ID_INVALID)
	{
		switch (m_nStartMode)
		{
		case IAC_AT_IMPEDANCE:
			FAStopImpedance(m_nDeviceId); // Stop impdedance.
			break;
		case IAC_AT_CALIBRATION: // Calibration
			FAStopCalibration(m_nDeviceId); // Stop calibration.
			break;
		}
		FAStop(m_nDeviceId); // Stop serving data.
		FAClose(m_nDeviceId); 
	}
	m_bOpen = FALSE;
	m_nDeviceId = FA_ID_INVALID;
	m_nStartMode = IAC_AT_DATA;
	m_nEEGChannels = 0;
	m_nAUXChannels = 0;
	m_nRingBufferMax = 0;
	m_nPointsPerBlock = 0;
	m_nSamples = 0;
	m_nWritePos = 0;
	m_nReadPos = 0;
	m_bBufferOverflow = FALSE;
	m_nErrorSeverity = 0;
	m_BlockTime = 0;
	m_Date = 0;
	m_nSamplingRate = 2000;
	m_DataMode = dmNormal;
	m_nChannelMode = 1;
	memset(&m_DeviceInfo, 0, sizeof(m_DeviceInfo));
	memset(&m_DeviceProp, 0, sizeof(m_DeviceProp));
	memset(&m_Settings, 0, sizeof(m_Settings));
	m_tblBuffer.clear();
	m_tblChanInfo.clear();
	ATLTRACE("> CDevice: Close SUCCESS.\n");
	hr = S_OK;
	FUNCTION_DEVICE_END
}

HRESULT CDevice::GetChannelTypes(SAFEARRAY** ppChanTypes) // char
{
	FUNCTION_DEVICE_BEGIN
	CComCritSecLock<CComAutoCriticalSection> singleLock(m_CrSec);
	CComSafeArray<char, VT_I1> p;
	ULONG nLength = (UINT)m_tblChanInfo.size() - 1; // without Trigger.

	p.Create(nLength, 0);
	if (p.GetCount() < nLength)
	{
		hr = E_OUTOFMEMORY;
	}
	else
	{
		for (ULONG i = 0; i < p.GetCount(); i++)
		{
			CChannelInfo &ci = m_tblChanInfo[i];
			p.SetAt(i, ci.nType);
			ATLTRACE("> CDevice: Type[%d] = %d.\n", i, p.GetAt(i));
		}
		*ppChanTypes = p.Detach();
		hr = S_OK;
	}
	FUNCTION_DEVICE_END
}

HRESULT CDevice::GetChannelResolutions(SAFEARRAY **ppResolutions)
{
	FUNCTION_DEVICE_BEGIN
	CComCritSecLock<CComAutoCriticalSection> singleLock(m_CrSec);
	ULONG nLength = (ULONG)m_tblChanInfo.size() - 1;	// without Trigger.
	CComSafeArray<double, VT_R8> p;				// 8-byte real.
	
	p.Create(nLength, 0);
	if (p.GetCount() < nLength) 
	{
		hr = E_OUTOFMEMORY;
	}
	else
	{
		for (ULONG i = 0; i < p.GetCount(); i++)
		{
			CChannelInfo &ci = m_tblChanInfo[i];
			p.SetAt(i, ci.dResolution);
		}
		*ppResolutions = p.Detach();
		hr = S_OK;
	}
	FUNCTION_DEVICE_END
}

HRESULT CDevice::GetDeviceInfos(BSTR *pszInfo)
{
	FUNCTION_DEVICE_BEGIN
	CComCritSecLock<CComAutoCriticalSection> singleLock(m_CrSec);
	char		szStr[256];
	CStringA	szDevDescr = m_tblChanInfo.size() ? "" : "-- N o n e --------";
	
	if (pszInfo == NULL) return hr;
	if (m_tblChanInfo.size())
	{
		szDevDescr = "-- FirstAmp -------- \r\n";
		itoa(m_DeviceInfo.SerialNumber, szStr, 10);
		szDevDescr += CString("   SerialNo: ") +  CString(szStr) + CString("\r\n");
		sprintf(szStr, "   %d Channels ", m_tblChanInfo.size() - 1);
		szDevDescr += CStringA(szStr) + "\r\n";
	}
	CComBSTR s(szDevDescr);
	hr = s.CopyTo(pszInfo);
	FUNCTION_DEVICE_END
}

HRESULT CDevice::GetDeviceModel(int* pnDeviceModel)
{
	FUNCTION_DEVICE_BEGIN
	*pnDeviceModel = m_DeviceInfo.Model;
	hr = S_OK;
	FUNCTION_DEVICE_END
}

HRESULT CDevice::GetDeviceSerialNumber(int* pnSerialNumber)
{
	FUNCTION_DEVICE_BEGIN
	*pnSerialNumber = int(m_DeviceInfo.SerialNumber);
	hr = S_OK;
	FUNCTION_DEVICE_END
}

HRESULT CDevice::GetRefChannel(int* pnChanIdx)
{
	FUNCTION_DEVICE_BEGIN
	switch (m_DeviceInfo.Model)
	{
	case 0: // 8 channels.
		*pnChanIdx = FA_MODEL_8_CHANNELS_MAIN;
		break;
	case 1: // 16 channels.
		*pnChanIdx = FA_MODEL_16_CHANNELS_MAIN - 1;
		break;
	default:
		*pnChanIdx = FA_MODEL_16_CHANNELS_MAIN - 1;
		break;
	}
	hr = S_OK;
	FUNCTION_DEVICE_END
}

HRESULT CDevice::SetDataMode(int nDataMode)
{
	FUNCTION_DEVICE_BEGIN
	int nReturn=0;
	
	m_DataMode = nDataMode == 0 ? dmNormal : dm20kHz4Channels;
	if (m_DataMode == dmNormal)
	{
		nReturn = FASetDataMode(m_nDeviceId, m_DataMode, NULL);
	}
	else
	{
		nReturn = FASetDataMode(m_nDeviceId, m_DataMode, &m_Settings);
	}
	if (nReturn != FA_ERR_OK)
	{
		ATLTRACE("CDevice: faSetDataMode for Highspeed FAILED(%d).\n", nReturn);
		goto Failed;
	}
	if (FAGetProperty(m_nDeviceId, &m_DeviceProp) != FA_ERR_OK) 
	{
		ATLTRACE("CDevice: faGetProperty FAILED(%d).\n", nReturn);
		goto Failed;
	}
	switch (m_DeviceInfo.Model)
	{
	case FA_MODEL_8:
		m_nChannelMode = DEVICE_CHANMODE_8;
		m_nEEGChannels = FA_MODEL_8_CHANNELS_MAIN - 1; // without trigger
		m_nAUXChannels = FA_MODEL_8_CHANNELS_AUX;
		m_tblChanInfo.resize(m_nEEGChannels + m_nAUXChannels + 1); // 1 Trigger.
		break;
	case FA_MODEL_16:
		m_nChannelMode = DEVICE_CHANMODE_16;
		m_nEEGChannels = FA_MODEL_16_CHANNELS_MAIN - 1; // without trigger
		m_nAUXChannels = FA_MODEL_16_CHANNELS_AUX;
		m_tblChanInfo.resize(m_nEEGChannels + m_nAUXChannels + 1); // 1 Trigger.
		break;
	}
	// Highspeed ?
	if (m_DataMode != dmNormal)
	{
		m_nChannelMode = DEVICE_CHANMODE_4;
		m_nEEGChannels = FA_MODE_20_KHZ_CHANNELS_MAIN; // EEG
		m_nAUXChannels = 0;	// no AUX.
		// m_tblChanInfo.resize(m_nEEGChannels + m_nAUXChannels + 1); // 1 Trigger.
	}
	float fRate = float(GetMaxSampleRate());
	if (m_nSamplingRate > fRate) m_nSamplingRate = fRate;
	ATLTRACE("CDevice: Max. Rate (%f).\n", fRate);
	hr = S_OK;
	FUNCTION_DEVICE_END
Failed:
	return hr;
}

HRESULT CDevice::SetSamplingRate(double dCurrentRate)
{
	FUNCTION_DEVICE_BEGIN
	m_nSamplingRate = float(dCurrentRate);
	float fMaxRate = float(GetMaxSampleRate());
	if (m_nSamplingRate < MIN_SAMPLING_RATE || m_nSamplingRate > fMaxRate)
	{
		m_nSamplingRate = fMaxRate;
		hr = S_FALSE;
	}
	else
	{
		hr = S_OK;
	}
	FUNCTION_DEVICE_END
}

// **** Serving Data ************************************************************
HRESULT CDevice::Start(int nMode, int nDataMode, int nChannelsPos[4], int nChannelsNeg[4])
{
	FUNCTION_DEVICE_BEGIN
	DWORD dwThreadId = 0;
	int nReturn = 0;
	
	// Because of FirstAmp.dll, try to open 2nd time in order 
	//	to force m_Started to set true.
	if (m_nDeviceId != FA_ID_INVALID)
	{
		FAStop(m_nDeviceId); // Finally stop data service process.
		FAClose(m_nDeviceId);
		m_nDeviceId = FA_ID_INVALID; // Reset the device id.
	}
	UINT nDevCnt = FAGetCount(); // faGetCount();
	m_nDeviceId = FAGetId(nDevCnt - 1);
	if (FA_ERR_OK != FAOpen(m_nDeviceId)) return S_FALSE;
	
	// Starting mode.
	m_nStartMode = nMode;
	m_DataMode = nDataMode == 0 ? dmNormal : dm20kHz4Channels;
	// 20kHz highspeed channels. 
	if (m_nStartMode == IAC_AT_DATA && m_DataMode != dmNormal) 
	{
		memset(&m_Settings, 0, sizeof(m_Settings));
		for (int i=0; i < FA_MODE_20_KHZ_CHANNELS_MAIN; i++)
		{
			m_Settings.Mode20kHz4Channels.ChannelsPos[i] = nChannelsPos[i];
			m_Settings.Mode20kHz4Channels.ChannelsNeg[i] = nChannelsNeg[i];
			ATLTRACE("CDevice: 20kHz Channels(%d - %d).\n", 
				nChannelsPos[i], nChannelsNeg[i]);
		}
		SetDataMode(m_DataMode);
	}
	else
		m_DataMode = dmNormal;

#ifdef DEBUG
	t_faDataState ds;
	ds.Size = sizeof(ds);
	int error = FAGetDataState(m_nDeviceId, &ds);
	ATLTRACE("error = %u, DllBufferSize = %u\nDllBufferCount = %u\nDriverBufferSize = %u\nDriverBufferCount = %u\nDeviceBufferSize = %u\nDeviceBufferCount = %u\nDataSamples = %u\nDataCounterErrors = %u\n",
		error, ds.DllBufferSize, ds.DllBufferCount, ds.DriverBufferSize,
		ds.DriverBufferCount, ds.DeviceBufferSize, ds.DeviceBufferCount, 
		ds.DataSamples, ds.DataCounterErrors);
#endif
	// Start the data acquision.
	int errorCode = FAStart(m_nDeviceId);
	if (FA_ERR_OK != errorCode) 
	{
		ATLTRACE("CDevice: faStart FAILED(%d).\n", errorCode);
		goto Finit;
	}
	// Start the data serving mode.
	switch (nMode)
	{
	case IAC_AT_IMPEDANCE:		// Impedance.
		if (FA_ERR_OK != FAStartImpedance(m_nDeviceId)) 
		{
			ATLTRACE("CDevice: faStartImpedance FAILED.\n");
			goto Finit;
		}
		break;
	case IAC_AT_CALIBRATION:	// Calibration.
		if (FA_ERR_OK != FAStartCalibration(m_nDeviceId)) 
		{
			ATLTRACE("CDevice: faStartCalibration FAILED.\n");
			goto Finit;
		}
		break;
	}
	// Create sync. event.
	if (!(m_hEvent = CreateEvent(NULL, FALSE, FALSE, NULL)))
	{
		ATLTRACE("CDevice: CreateEvent FAILED.\n");
		goto Finit;
	}
	if ((m_hThread = CreateThread(NULL, 0, ThreadProc, (LPVOID)this, 0, &dwThreadId)))
	{
		// Wait until first sample received.
		if (WAIT_TIMEOUT == WaitForSingleObject(m_hEvent, 10000))
		{
			ATLTRACE("CDevice: Waiting for thread timeOut.\n");
			goto Finit;
		}
	}
	else
	{
		ATLTRACE("CDevice: CreateThread FAILED. Error(0x%x).\n", GetLastError());
		goto Finit;
	}
	// Eventhandle no longer needed, cleanup.
	if (m_hEvent) CloseHandle(m_hEvent);
	m_hEvent = NULL;
	ATLTRACE("> CDevice: Start SUCCESS. ThreadID(0x%x).\n", dwThreadId);
	hr = S_OK;
Finit:
	if (hr != S_OK) // case of error.
	{
		ATLTRACE("CDevice: Start FAILED.\n");
		Stop();
	}
	return hr;
	FUNCTION_DEVICE_END
}

HRESULT CDevice::Stop()
{
	return Close(); // Close device. Cleanup.
}

DWORD CDevice::ThreadProc(LPVOID pParam)
{
	((CDevice*)pParam)->Serve();
	return 0;
}

void CDevice::Serve()
{
	// Copy of some member variables preventing from common access violation.
	UINT nRingBufMax = 0;			// Max ring buffer max.
	vector<t_faDataModel8>	
					tblMaxBuf8;		// 1 read cycle buffer of 8 channel system + 2 add. samples.
	vector<t_faDataModel16> 
					tblMaxBuf16;	// 1 read cycle buffer of 16 channel system + 2 add. samples.
	vector<t_faDataFormatMode20kHz> 
					tblMaxBuf4;		// 1 read cycle buffer (highspeed, 4 ch) + 2 add. samples.
	vector<float>	tblEEGData;		// 1 read cycle of only EEG and AUX signals.
	vector<float>	tblTrigger;		// 1 read cycle of only Trigger signals.
	vector<float>	tblPacket;		// 1 read cycle of EEG, AUX, Trigger signals.
			
	char *pMaxBuffer = NULL;		// Points to 4, 8, 16 channels data structure + 2 add. samples.
	int	nDeviceId = m_nDeviceId,	// Device id.
		nChannelMode = m_nChannelMode,	
									// Channel system.
		nEegMax = m_nEEGChannels,	// Max number of EEG channels.
		nAuxMax = m_nAUXChannels,	// Max number of AUX channels.
		nChannels = GetChannels(),	// Number of channels.
		nAnalogChannels = 0,		// EEG, AUX channels
		nStartMode = m_nStartMode,	// Monitoring, Calibration, Impedance mode.
		nReadLen = 0,				// Data serving length of 1 read cycle in bytes.
		nPoints = 0,				// Number of samples of 1 read cycle.
		nMaxPoints = 0,				// max. samples per read block.
		n2SamplesBytes = 0,			// used to read 2 add. samples at begin to correct trigter.
		nPointsPerBlock = m_nPointsPerBlock;
	
	// Reset variables.
	m_nWritePos = m_nReadPos = 0; m_nRingBufferMax = 0; m_bBufferOverflow = FALSE;
	m_nErrorSeverity = 0; m_Date = 0;
	if (m_nSamplingRate < MIN_SAMPLING_RATE || m_nSamplingRate > GetMaxSampleRate())
		m_nSamplingRate = float(GetMaxSampleRate());

	if (nStartMode == IAC_AT_IMPEDANCE)
	{
		nMaxPoints = m_nPointsPerBlock = nPoints = 1;
		switch (nChannelMode)
		{
		case DEVICE_CHANMODE_16:
			nChannels = FA_MODEL_16_CHANNELS_MAIN + 1; // + Gnd
			break;
		default: // DEVICE_CHANMODE_8:
			nChannels = FA_MODEL_8_CHANNELS_MAIN + 1; // + Gnd
			break;	
		}
		m_nSamples = nChannels * m_nPointsPerBlock;
		m_nRingBufferMax = nRingBufMax = m_nSamples;
		nAnalogChannels = nChannels;
	}
	else // monitoring und test signal
	{
		if (nChannelMode == DEVICE_CHANMODE_4)
		{
			nEegMax = FA_MODE_20_KHZ_CHANNELS_MAIN;
			nChannels = nEegMax + 1;
		}
		// Try around 20 ms blocks. Take care of min. SamplingRate 
		//	if serving interval changed!!!
		m_nPointsPerBlock = nPoints = 
			UINT(max(m_nSamplingRate * 0.001 * DEVICE_GET_DATA_INTERVAL, 1));
		// 4 sec data block.
		m_nSamples = nChannels * m_nPointsPerBlock;
		m_nRingBufferMax = nRingBufMax = 
			int((1000. / DEVICE_GET_DATA_INTERVAL) * DEVICE_BUFTIME * m_nSamples);
		// Prepare the read buffer, length.
		nMaxPoints = int(max(GetMaxSampleRate() * 0.001 * DEVICE_GET_DATA_INTERVAL, 1));
		// nMaxPoints += TRIG_CORRECT_LEN; // 2 correcting trigger
		nAnalogChannels = nChannels - 1;
	}
	// Get real block time in OLE DATE format, ie. normalize: one day = 1.
	m_BlockTime = 
		(double(m_nPointsPerBlock) / double(m_nSamplingRate)) / (60. * 60. * 24.);
	m_tblBuffer.resize(m_nRingBufferMax); 
	switch (nChannelMode)
	{
	case DEVICE_CHANMODE_16:
		tblMaxBuf16.resize(nMaxPoints);
		nReadLen = int(tblMaxBuf16.size() * sizeof(t_faDataModel16)); // in bytes.
		// n2SamplesBytes = TRIG_CORRECT_LEN * sizeof(t_faDataModel16); // in bytes.
		pMaxBuffer = (char *)&tblMaxBuf16[0];
		break;
	case DEVICE_CHANMODE_4: // 20kHz of 4 channels.
		tblMaxBuf4.resize(nMaxPoints);
		nReadLen = int(tblMaxBuf4.size() * sizeof(t_faDataFormatMode20kHz)); // in bytes.
		// n2SamplesBytes = TRIG_CORRECT_LEN * sizeof(t_faDataFormatMode20kHz); // in bytes.
		pMaxBuffer = (char *)&tblMaxBuf4[0];
		break;
	default: // DEVICE_CHANMODE_8
		tblMaxBuf8.resize(nMaxPoints);
		nReadLen = int(tblMaxBuf8.size() * sizeof(t_faDataModel8)); // in bytes.
		// n2SamplesBytes = TRIG_CORRECT_LEN * sizeof(t_faDataModel8); // in bytes.
		pMaxBuffer = (char *)&tblMaxBuf8[0];
		break;	
	}
	if (nStartMode != IAC_AT_IMPEDANCE)
	{
		nReadLen -= n2SamplesBytes;
	}
	ATLTRACE("> m_nSamples(%d), nChannels(%d), nReadLen(%d), .\n", 
			m_nSamples, nChannels, nReadLen);

	// Antialiasing 
	if (m_pFilterAntiAliasing != NULL) delete m_pFilterAntiAliasing;
	m_pFilterAntiAliasing = NULL;

	// for Input sampling frequency 20kHz or 2kHz.
	int nRateDivider = (int)max(GetMaxSampleRate() * 1. / m_nSamplingRate, 1);
	vector<float> tblFrequencies;
	if (nStartMode != IAC_AT_IMPEDANCE)
	{
		tblFrequencies.resize(nAnalogChannels, 0.);	// + without trigger.
		float fAntiAliasing = (float)(0.3 * m_nSamplingRate); // 0.3 Highcut as software.
		for (int i = 0; i < nAnalogChannels; i++)
		{
			tblFrequencies[i] = fAntiAliasing;
		}
		// without trigger.
		m_pFilterAntiAliasing = new CFilterButterworth64(nAnalogChannels, 
			CFilterButterworth64::eHighCutoff, 24, GetMaxSampleRate(), &tblFrequencies[0]);
	}
	m_bFirstData = TRUE;
	
	// max. available points per read cycle without trigger.
	tblEEGData.resize(nAnalogChannels * nMaxPoints); 
	tblTrigger.resize(nMaxPoints); 
	tblPacket.resize(m_nSamples); // actual results.
	float *pfEEGData = (float*)&tblEEGData[0];
	float *pfTrigger = (float*)&tblTrigger[0];
	float *pfPacket = (float*)&tblPacket[0];
	
	// Enable Start() routine continuing.
	nPointsPerBlock = m_nPointsPerBlock; // update samples per block
	if (m_hEvent) 
	{
		SetEvent(m_hEvent);
		Sleep(1); // Let Start() doing something before start
	}
	m_bRun = TRUE; // start ready

	while (m_bRun)
	{
		try
		{
			// at begin read only 2 samples, the following turns 
			// always at position of 2 samples.
			int nReturnLen = 0;
			if (nStartMode == IAC_AT_IMPEDANCE)
				nReturnLen = ReadData(nDeviceId, pMaxBuffer, nReadLen);
			else
				nReturnLen = ReadData(nDeviceId, pMaxBuffer + n2SamplesBytes, nReadLen);
			if (nReturnLen < 0) // Device broken. Restart device.
			{
				// Restart the serving routing, reset the device.
				CComCritSecLock<CComAutoCriticalSection> singleLock(m_CrSec); //Lock buf
				// Reset buffer.
				m_nWritePos = m_nReadPos = 0;
				memset(&m_tblBuffer[0], 0, m_tblBuffer.size() * sizeof(float));
				singleLock.Unlock(); // Unlock buf.
				if (Restart(nStartMode) == S_OK)
				{
					nDeviceId = m_nDeviceId; // Update new device id.
				}
				continue;				// start reread
			}
			if (nReturnLen != nReadLen) // Device broken. Restart device.
			{
				ATLTRACE("> Serve data error: %d ? %d\n", nReturnLen, nReadLen);
				continue;
			}
			// Serving data successfully. 
			// Copy data to ring buffer.
			if (nReturnLen >= nReadLen)	
			{	
				// Impedance measurement
				if (nStartMode == IAC_AT_IMPEDANCE)
				{
					UINT *pnPacket = (UINT *)&tblPacket[0];
					int nErrorCode = FAGetImpedance(nDeviceId, pnPacket, 
										nAnalogChannels * sizeof(UINT));
					if (FA_ERR_OK == nErrorCode)
					{
						// now copy data.
						CComCritSecLock<CComAutoCriticalSection> singleLock(m_CrSec);
						m_nWritePos = m_nReadPos = 0;
						void *pWrite = (void *)&m_tblBuffer[0];
						memcpy(pWrite, pfPacket, m_nSamples * sizeof(ULONG));
					}
				}
				else
				{
					// Monitoring, Testsignal measurement. 
					switch (nChannelMode)
					{
					case DEVICE_CHANMODE_16:	// 16 channels system.
						ProcessData(tblMaxBuf16, (int)tblMaxBuf16.size(), 
							nEegMax, nAuxMax, pfEEGData, pfTrigger, 
							nStartMode == IAC_AT_CALIBRATION);
						// move last 2 samples to begin for the next read
						memcpy(pMaxBuffer, pMaxBuffer + nReadLen, n2SamplesBytes);
						break;
					case DEVICE_CHANMODE_8:		// 8 channels system.
						ProcessData(tblMaxBuf8, (int)tblMaxBuf8.size(), 
							nEegMax, nAuxMax, pfEEGData, pfTrigger, 
							nStartMode == IAC_AT_CALIBRATION);
						// move last 2 samples to begin for the next read
						memcpy(pMaxBuffer, pMaxBuffer + nReadLen, n2SamplesBytes);
						break;
					case DEVICE_CHANMODE_4:		// 4 channels system (20kHz).
						ProcessHighspeedData(tblMaxBuf4, (int)tblMaxBuf4.size(), 
							nEegMax, pfEEGData, pfTrigger);
						// move last to points to begin of the next read
						memcpy(pMaxBuffer, pMaxBuffer + nReadLen, n2SamplesBytes);
						break;
					}
					
					// Antialiasing filter + downsampling
					pfPacket = (float* )&tblPacket[0];
					ReduceRate(pfEEGData, pfTrigger, pfPacket, m_bFirstData, 
							   nPointsPerBlock, nAnalogChannels, nRateDivider);

					// now copy data.
					CComCritSecLock<CComAutoCriticalSection> singleLock(m_CrSec);
					float *pWrite = (float *)&m_tblBuffer[m_nWritePos];
					memcpy(pWrite, pfPacket, m_nSamples * sizeof(float));
					m_nWritePos += m_nSamples;
#ifdef TEST
					myprintf("\tServe: Writepos = %d, Readpos = %d\n", 
						m_nWritePos, m_nReadPos);
#endif
					if (m_nWritePos >= m_nRingBufferMax - 1) 
					{
	#ifdef DEBUG
						SYSTEMTIME st;
						GetLocalTime(&st);
						ATLTRACE("CDevice: End of Ringbuffer(%u:%u).\n", 
							st.wMinute, st.wSecond);
	#endif
						m_nWritePos = 0; // Move write position to begin.
					}
					else if (m_nWritePos == m_nReadPos)
					{
						// Buffer overflow status of Refa.
						m_bBufferOverflow = TRUE;
					}
				}
			}
		}
		catch (...)
		{
			Stop();
			break;
		}
	}
	ATLTRACE("> Server: Thread Exited.\n");
}

int CDevice::ReadData(int nDeviceId, char *pBuffer, int nReadLen)
{
	int nReturnLen = 0,			// Current return length in bytes.
		nLenToRead = nReadLen;	// len in bytes.
	UINT nTimeOut = timeGetTime() + DEVICE_SERVE_TIMEOUT; 

	do
	{
		nReturnLen = FAGetData(nDeviceId, pBuffer, nLenToRead);
		if (nReturnLen < 0) // Device error.
		{
			return -1;
		}
		else if (nReturnLen == 0)
		{
			Sleep(DEVICE_GET_DATA_INTERVAL); // previous 1 ms: causes trigger problem. 
		}
		else
		{
			nLenToRead -= nReturnLen; // decrement the read length.
			pBuffer += nReturnLen;
		}
		if (timeGetTime() >= nTimeOut) // Timeout
		{
			return -1;
		}
	} while (m_bRun && nLenToRead > 0);
	return (nLenToRead <= 0) ? nReadLen : (nReadLen - nLenToRead);
}

void CDevice::ReduceRate(float* pEEGData, float* pTrigger, float* pPacket, 
						BOOL& bFirstData, int nCurPointsPerBlock, 
						int nAnalogChannels, int nRateDivider)
{

	if (m_pFilterAntiAliasing == NULL) return;
	if (nRateDivider != 1)
	{
		// Filter EEG, AUX data
		m_pFilterAntiAliasing->m_pnData = pEEGData;
		if (bFirstData == TRUE)

		{
			m_pFilterAntiAliasing->StoreDCOffsets();
			bFirstData = FALSE;
		}
		int nMaxPoints = int(max(GetMaxSampleRate() * 0.001 * DEVICE_GET_DATA_INTERVAL, 1));
		m_pFilterAntiAliasing->DoFilter(0, nMaxPoints);
	}

	vector<double> fValues;
	float* pfEEGData = pEEGData;
	float* pfTrigger = pTrigger;
	float* pfDestination = pPacket;
	
	fValues.resize(nAnalogChannels);
	// Check for input port and reduce data
	for (int i = 0; i < nCurPointsPerBlock; i++)
	{
		// Marker channel?
		ZeroMemory((void*)&fValues[0], fValues.size() * sizeof(double));
		// EEG, AUX signals
		for (int j = 0; j < nRateDivider; j++)
		{
			// average of each channel.
			for (int k = 0; k < nAnalogChannels; k++)
			{ 
				fValues[k] += *(pfEEGData + k);
			}
			// goto next sample.
			pfEEGData += nAnalogChannels;
		}
		// copy analog signal results
		for (int k = 0; k < nAnalogChannels; k++)
		{
			*(pfDestination++) = float(fValues[k] / float(nRateDivider));
		}
		// copy trigger signal
		*(pfDestination++) = *pfTrigger;
		pfTrigger += nRateDivider; // goto to next sample.
	}
}

STDMETHODIMP CDevice::GetData(SAFEARRAY **ppfData, int* pnChannels, 
							  int* pnPoints, DATE *pDate, int* pnErrorSeverity)
{
	FUNCTION_DEVICE_BEGIN
	CComSafeArray<float, VT_R4> p;
	int nLength = 0; // in bytes.
		
	*pnChannels = *pnPoints = *pnErrorSeverity = 0; *pDate = 0;

	CComCritSecLock<CComAutoCriticalSection> singleLock(m_CrSec);

#ifdef TEST	
	myprintf("GetData: %s\n", m_bRun ? "RUN" : "NOT Run");
#endif
	if (!m_bRun) 
	{
		goto Finit; // device not started.
	}
	// Reset error.
	*pnErrorSeverity = m_nErrorSeverity; m_nErrorSeverity = 0; // reset
	if (m_Date < 1.) // sync with the start date.
	{
		EmptyBuffer();
		goto Finit;
	}
	// Is data available.
	int delta = Delta();
	if (delta < m_nSamples) 
	{
#ifdef TEST	
		myprintf("\tData not available (%d, %d), Writepos = %d, Readpos = %d\n", 
			delta, m_nSamples, m_nWritePos, m_nReadPos);
#endif
		goto Finit;
	}
	p.Create(m_nSamples);
	if (p.GetCount() < ULONG(m_nSamples))
	{
		hr = E_OUTOFMEMORY;
#ifdef TEST	
		myprintf("\tE_OUTOFMEMORY\n");
#endif
		goto Finit;
	}
	// Copy data to result.
	*pnChannels = GetChannels(); *pnPoints = m_nPointsPerBlock; 
	*pDate = GetCurrentOleTime();
	nLength = m_nSamples * sizeof(float); // in bytes.
	memcpy(p.m_psa->pvData, (void *)&m_tblBuffer[m_nReadPos], nLength);
	m_nReadPos += m_nSamples; // goto next readblock.
	if (m_nReadPos >= m_nRingBufferMax - 1)
	{
		m_nReadPos = 0; 
	}
	*ppfData = p.Detach();
	hr = S_OK;
#ifdef TEST	
	myprintf("\tSUCCESS, ReadPos = %d, WritePos = %d, Ringbuffer = %d, m_nPointsPerBlock = %d, %d\n", 
		m_nReadPos, m_nWritePos, m_nRingBufferMax, m_nPointsPerBlock, m_nSamples);
#endif
	FUNCTION_DEVICE_END
Finit:
	return S_FALSE;
}

HRESULT CDevice::EmptyBuffer()
{
	FUNCTION_DEVICE_BEGIN
	
	vector<t_faDataModel16> tblBuf16;
	vector<t_faDataModel8> tblBuf8;
	vector<t_faDataFormatMode20kHz> tblBuf4;
	char	*pBuffer;	// Points to 8 or 16 channels data structure.
	const	UINT nTimeInterval = 2000;	// 2 second
	UINT	nTimeOut = 0, nCount = 0;
	int		nReadLen = 0;
	int		nSamplesCnt = 	//sample count
				int(DEVICE_GET_DATA_INTERVAL * m_nSamplingRate);


	switch (m_nChannelMode)
	{
	case DEVICE_CHANMODE_16:
		tblBuf16.resize(nSamplesCnt);
		nReadLen = int(tblBuf16.size() * sizeof(t_faDataModel16)); // in bytes.
		pBuffer = (char *)&tblBuf16[0];
		break;
	case DEVICE_CHANMODE_8:
		tblBuf8.resize(nSamplesCnt);
		nReadLen = int(tblBuf8.size() * sizeof(t_faDataModel8)); // in bytes.
		pBuffer = (char *)&tblBuf8[0];
		break;
	case DEVICE_CHANMODE_4:
		tblBuf4.resize(nSamplesCnt);
		nReadLen = int(tblBuf4.size() * sizeof(t_faDataFormatMode20kHz)); // in bytes.
		pBuffer = (char *)&tblBuf4[0];
		break;
	default: // error.
		return S_FALSE;
	}
	nTimeOut = timeGetTime() + nTimeInterval;
	int nTotalLen = nReadLen;
	do
	{
		if (timeGetTime() >= nTimeOut) 
		{
			break;
		}
		int nReturnLen = FAGetData(m_nDeviceId, pBuffer, nReadLen);
		if (nReturnLen < 0) // Device error.
		{
			break;
		}
		else if (nReturnLen == 0)
		{
			break;
		}
		else
		{
			nReadLen -= nReturnLen;
			if (nReadLen <= 0) nReadLen = nTotalLen;
			nCount++;
		}
	} while (m_bRun && nReadLen > 0);

#ifdef TEST	
	myprintf("EmptyBuffer: Count %d, nReadLen %d\n", nCount, nReadLen);
#endif

	// Get actual time.
	SYSTEMTIME st;
	GetLocalTime(&st);
	COleDateTime ti(st);
	m_Date = ti.m_dt;
	m_nWritePos = m_nReadPos = 0; // Reset the local read / write pointer.
	FUNCTION_DEVICE_END
}

int CDevice::Delta()
{
	if (m_nWritePos == m_nReadPos) return 0;
	return (m_nWritePos > m_nReadPos) ? int(m_nWritePos - m_nReadPos) : 
			int((m_nRingBufferMax - m_nReadPos) + m_nWritePos);
}

int CDevice::GetBufferState(void)
{
	CComCritSecLock<CComAutoCriticalSection> singleLock(m_CrSec);

	//For impedance measurement. Since this mode needs only the current samples.
	if (m_nStartMode == IAC_AT_IMPEDANCE)
	{
		return 0; 
	}
	if (m_bBufferOverflow)
	{
		m_bBufferOverflow = FALSE; // Reset flag
		return -1;
	}
	// ACC buffer.
	float fValue = float(Delta());	// ACC ringbuffer state.
	fValue /= float(m_nRingBufferMax);
	fValue *= 100;
	return int(fValue);
}

STDMETHODIMP CDevice::GetImpedance(SAFEARRAY **ppfData, int* pnChannels, 
						int* pnEegMax, DATE *pDate, int* pnErrorSeverity)
{
	FUNCTION_DEVICE_BEGIN
	UINT nLength = 0;
	
	*pnChannels = *pnEegMax = *pnErrorSeverity = 0; *pDate = 0;

	CComCritSecLock<CComAutoCriticalSection> singleLock(m_CrSec);
	if (!m_bRun) goto Finit; // device not started.
	*pnChannels = m_nEEGChannels + 2; // + 1 Ref + 1 GND channel.
	*pnEegMax = GetEEGMaxChannels();
	*pnErrorSeverity = m_nErrorSeverity;
	*pDate = GetCurrentOleTime();
	m_nErrorSeverity = 0; // Reset error indicator.
			
	nLength = *pnChannels;
	CComSafeArray<UINT, VT_UI4> p;
	p.Create(nLength);
	if (p.GetCount() < nLength) 
	{
		hr = E_OUTOFMEMORY;
		goto Finit;
	}
	vector<UINT> tblData;
	tblData.resize(nLength);
	memcpy((void*)&tblData[0], (void*)&m_tblBuffer[0], nLength * sizeof(UINT));
	singleLock.Unlock(); // release lock.
	for (UINT i=0; i < nLength; i++)
	{
		UINT nData = tblData[i];
		p.SetAt(i, nData);
		ATLTRACE(" Z[%d] = %f.", i, float(nData / 1000.f)); 
	}
	*ppfData = p.Detach();
	hr = S_OK;
	FUNCTION_DEVICE_END
Finit:
	return hr;
}

HRESULT CDevice::GetDigitalPort(ULONG nTimeInterval, USHORT* pnSample)
{
	FUNCTION_DEVICE_BEGIN
	int			nChannels=0, nPoints=0, nErrorSeverity=0;
	DATE		date=0;
	ULONG		nLength=0;
	LPSAFEARRAY	pData;
	
	hr = GetData(&pData, &nChannels, &nPoints, &date, &nErrorSeverity);
	if (hr == S_OK && nErrorSeverity != 1)
	{
		CComSafeArray<float, VT_R4> p;
		p.Attach(pData);
		nLength = p.GetCount();
		*pnSample = UINT(p.GetAt(nLength-1));
	}
	FUNCTION_DEVICE_END
}

HRESULT CDevice::Restart(int nStartMode)
{
	HRESULT hr=S_FALSE;
	HWND	hForeground=NULL;
	BOOL	bRestarted=FALSE;
	try
	{
		// Restart Loop.
		bRestarted=FALSE;
		m_bFirstData = TRUE;
		while (!bRestarted)
		{
			CComCritSecLock<CComAutoCriticalSection> singleLock(m_CrSec);
			// Try to stop background service.
			if (m_nDeviceId != FA_ID_INVALID)
			{
				switch (nStartMode)
				{
				case IAC_AT_IMPEDANCE:
					FAStopImpedance(m_nDeviceId); // Stop impdedance.
					break;
				case IAC_AT_CALIBRATION: // Calibration
					FAStopCalibration(m_nDeviceId); // Stop calibration.
					break;
				}
				FAStop(m_nDeviceId); // Finally stop data service process.
				FAClose(m_nDeviceId);
				m_nDeviceId = FA_ID_INVALID; // Reset the device id.
			}
			// Try to find the first visible device id.
			UINT nDevCnt =  FAGetCount(); //faGetCount(); 
			for (UINT n=0; n < nDevCnt; n++)
			{
				m_nDeviceId = FAGetId(n);
				if (m_nDeviceId != FA_ID_INVALID)
				{
					break; // Device found.
				}
			}
			// Try to open the device, and restart the serving mode.
			if (m_nDeviceId != FA_ID_INVALID)
			{
				if (FA_ERR_OK == FAOpen(m_nDeviceId))
				{
					// set the correct normal / highspeed mode.
					if (nStartMode == IAC_AT_DATA)
					{
						SetDataMode(m_DataMode);
					}
					// Start the data service process.
					if (FA_ERR_OK != FAStart(m_nDeviceId)) continue;
					// Start data serving mode.
					switch (nStartMode)
					{
					case IAC_AT_IMPEDANCE:		// Start impedance mode.
						if (FA_ERR_OK != FAStartImpedance(m_nDeviceId)) continue;
						break;
					case IAC_AT_CALIBRATION:	// Start calibration mode.
						if (FA_ERR_OK != FAStartCalibration(m_nDeviceId)) continue;
						break;
					}
					ATLTRACE("> Restart successful.\n");
					bRestarted = TRUE; // Terminate while loop.
					m_nErrorSeverity = 0; // Reset error code.
					break; // Terminate the restart loop.
				}
			}
			if (!m_bRun) break;
			if (!bRestarted)
			{
				singleLock.Unlock();
				Sleep(500); // Wait for 500 milisecond.
			}
		}
		hr = bRestarted ? S_OK : S_FALSE;
	}
	catch (...)
	{
		hr = E_FAIL;
		// Stop();
	}
	return hr;
}

HRESULT CDevice::GetUserData(char *pBuffer, UINT* pnCount)
{
	FUNCTION_DEVICE_BEGIN
	*pnCount = 0;
	if (m_nDeviceId == FA_ID_INVALID) return hr;
	UINT nSize = 512;
	if (FA_ERR_OK == FAGetUserData(m_nDeviceId, pBuffer, nSize, pnCount))
		hr = S_OK;
	FUNCTION_DEVICE_END
}

HRESULT CDevice::SetUserData(char *pBuffer, UINT nSize, UINT* pnCount)
{
	FUNCTION_DEVICE_BEGIN
	if (m_nDeviceId == FA_ID_INVALID) return hr;
	if (nSize > FA_USER_DATA_SIZE) nSize = FA_USER_DATA_SIZE;
	*pnCount = 0;
	if (FA_ERR_OK == FASetUserData(m_nDeviceId, (void *)pBuffer, nSize, pnCount))
		hr = S_OK;
	FUNCTION_DEVICE_END
}

HRESULT CDevice::GetIo(UINT* pnInputs, UINT* pnOutputs)
{
	FUNCTION_DEVICE_BEGIN
	if (m_nDeviceId == FA_ID_INVALID) return hr;
	*pnInputs = *pnOutputs = 0x00;
	t_faIo io; 
	if (FA_ERR_OK == FAGetIo(m_nDeviceId, &io))
	{
		hr = S_OK;
		*pnInputs = io.Inputs; *pnOutputs = io.Outputs;
	}
	FUNCTION_DEVICE_END
}

HRESULT CDevice::SetIo(UINT nInputs, UINT nOutputs)
{
	FUNCTION_DEVICE_BEGIN
	if (m_nDeviceId == FA_ID_INVALID) return hr;
	t_faIo io; io.Inputs = nInputs; io.Outputs = nOutputs;
	if (FA_ERR_OK == FASetIo(m_nDeviceId, &io))
		hr = S_OK;
	FUNCTION_DEVICE_END
}

HRESULT CDevice::GetContrast(UINT* pnContrast)
{
	FUNCTION_DEVICE_BEGIN
	if (m_nDeviceId == FA_ID_INVALID) return hr;
	if (FA_ERR_OK == FAGetContrast(m_nDeviceId, pnContrast))
		hr = S_OK;
	FUNCTION_DEVICE_END
}

HRESULT CDevice::SetContrast(UINT nConstrast)
{
	FUNCTION_DEVICE_BEGIN
	if (m_nDeviceId == FA_ID_INVALID) return hr;
	if (nConstrast < 0) nConstrast = 0;
	if (nConstrast > 100) nConstrast = 100;
	if (FA_ERR_OK == FASetContrast(m_nDeviceId, nConstrast))
		hr = S_OK;
	FUNCTION_DEVICE_END
}

HRESULT CDevice::GetBrightness(UINT* pnBrightness)
{
	FUNCTION_DEVICE_BEGIN
	if (m_nDeviceId == FA_ID_INVALID) return hr;
	if (FA_ERR_OK == FAGetBrightness(m_nDeviceId, pnBrightness))
		hr = S_OK;
	FUNCTION_DEVICE_END
}

HRESULT CDevice::SetBrightness(UINT nBrightness)
{
	FUNCTION_DEVICE_BEGIN
	if (m_nDeviceId == FA_ID_INVALID) return hr;
	if (nBrightness < 0) nBrightness = 0;
	if (nBrightness > 100) nBrightness = 100;
	if (FA_ERR_OK == FASetBrightness(m_nDeviceId, nBrightness))
		hr = S_OK;
	FUNCTION_DEVICE_END
}

BOOL CDevice::MyLoadLibary(const char* szFullPath)
{
	BOOL bRet = FALSE;
	// TODO:
	if (m_hLib != NULL) return TRUE;
	m_hLib = LoadLibraryA(szFullPath);
	if (m_hLib == NULL || m_hLib == INVALID_HANDLE_VALUE)
	{
		m_hLib = NULL;
		return FALSE;
	}
	Import(m_hLib);
	return bRet;
}

BOOL CDevice::MyUnLoadLibary()
{
	BOOL bRet = FALSE;
	// TODO:
	if (m_hLib != NULL)
		FreeLibrary(m_hLib);
	m_hLib = NULL;
	return bRet;
}