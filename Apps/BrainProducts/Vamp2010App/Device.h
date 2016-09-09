// MODULE: Device.h
//: written by: Phuong Nguyen
//+       date: 08-Mar-2005
//+
//+ Description:
//.		Declaration of class CDevice.

#pragma once

#include "stdafx.h"
#include <math.h>
#include "FirstAmp.h"
#include "FilterButterworth64.h"

using namespace std;

#define DEVICE_DIGITMAX				9	// 9 bit trigger port.
#define DEVICE_MAX_IMP_RANGES		5	// 5 main impedance groups.
#define DEVICE_CHAN_TYPE_UNKNOWN	0	// Unknown channel type.
#define DEVICE_CHAN_TYPE_EEG		1	// EEG channel type.
#define DEVICE_CHAN_TYPE_AUX		2	// Auxiliary channel type.
#define DEVICE_CHAN_TYPE_TRIGGER	3	// Trigger port type.

#define DEVICE_CHANMODE_8			0	// 8 EEG channels system.
#define DEVICE_CHANMODE_16			1	// 16 EEG channels system.
#define DEVICE_CHANMODE_4			2	// 4 EEG channels, joints the FA systems.

/****** Acquisition types (nAcquisitionType), used by IAmplifierControl::Start(): ******/
#define IAC_AT_IMPEDANCE			0		// Impedance check
#define IAC_AT_DATA					1		// Standard data aquisition
#define IAC_AT_CALIBRATION			2		// Calibration

class CDevice
//; Represents the FirstAmp device.
{
	CDevice();
	~CDevice();
	static CDevice m_device;

public:
	class CChannelInfo
	//; Represents a single channel info.
	{
	public :
		char	nType;			// Channel type.
		int		nPhysIdx;		// Physical channel index.
		CString szLabel;		// Label of channel.
		double	dResolution;	// Resolution [µV] for all analog channels.
		BOOL	bUsrUnit;		// User unit used if TRUE otherwise FALSE.
		CString szUnitName;		// Unit name.
		float	fGradient;		// Slope of equation for usr unit (e.g. [mV/C]).
		float	fOffset;		// Offset of equation (constant) in [mV].
		
		CChannelInfo();
			//; Constructor.
		const CChannelInfo & operator = (const CChannelInfo &);
			//; Assign operator.
		const BOOL operator == (const CChannelInfo &);
			//; Compare operator.
		_inline float ToUserUnit(float fRawData)
			//: Conversion to the digitized raw data, equation U(T) = m*T + Uoff,
			//+		with m [mV/C], U[mV], Uoff[mV]
			//+	Return
			//+		Digitized sample as result of user equation.
			//+	Parameters
			//.		fRawData - digitized raw data of this channel (direct from driver).
		{
			// Conversion only allowed for AUX.
			if (nType != DEVICE_CHAN_TYPE_AUX || !bUsrUnit) return fRawData;
			if (fabs(fGradient) < 1e-6) fGradient = 1; // default.
			// [µV] => [mV] because of gradient.
			return float(((fRawData * dResolution * 1e-3) - fOffset) / fGradient);
		}
	};

private:
	CComAutoCriticalSection	
					m_CrSec;
	CComAutoCriticalSection	
					m_CrSecBmp;
	volatile BOOL	m_bRun;			// Running flag for serving thread.
	HANDLE			m_hEvent;		// Used for synchronizing serving thread.
	HANDLE			m_hThread;		// Handle of serving thread.
	BOOL			m_bDaqInUse;	// Data acquisition in use?

	// *** Device and Channels information ****************
	volatile BOOL	m_bOpen;			// Indicate whether device already opened.
	int				m_nDeviceId;		// Device identifier.
	volatile int	m_nStartMode;		// Monitoring, Calibration, Impedance mode.
	int				m_nEEGChannels;		// Max. number of EEG.
	int				m_nAUXChannels;		// Max. number of AUX.
	t_faInformation m_DeviceInfo;		// Device info.
	t_faProperty	m_DeviceProp;		// Channel properties.
	vector<CDevice::CChannelInfo> 
					m_tblChanInfo;		// table of all channel with it's info.

	// *** Buffer information *****************************
	vector <float> 
			m_tblBuffer;		// Local ring buffer.
	UINT	m_nRingBufferMax;	// Max ring buffer length.
	int		m_nPointsPerBlock;	// Number of point per read cycle of ACC for 1 channel. 
	int		m_nSamples;			// Total number of samples of all channels 
								// in 1 m_nPointsPerBlock.
	UINT	m_nWritePos;		// Write pos in ring buffer.
	UINT	m_nReadPos;			// Read pos in ring buffer.
								//	for all channels.
	BOOL	m_bBufferOverflow;	// Buffer overflow flag.
	int		m_nErrorSeverity;	// Error severity: 0 = warning, 1 = error.
	DATE	m_BlockTime;		// Time for a single data block.
	DATE	m_Date;				// Recording date / time.
	float	m_nSamplingRate;	// current used sampling rate.
	
	// *** Highspeed 20 kHz ***********************************
	t_faDataMode			
			m_DataMode;
	t_faDataModeSettings	
			m_Settings;
	char	m_nChannelMode;		// channel mode: 0(8 channels), 1(16 channels), 2(4 channels).

	// *** Antialiasing filter ***************
	CFilterButterworth64*	m_pFilterAntiAliasing;

	BOOL	m_bFirstData;
	HMODULE	m_hLib;	

	static DWORD WINAPI ThreadProc(LPVOID pParam);
		//; Serving process thread.
	void Serve();
		//; Serve monitoring, calibration data.
	HRESULT EmptyBuffer();
		//; Empty the buffer in device driver.
	int Delta();
		//; Retrieve the absolute length from between read and write pointer.
	int ReadData(int nDeviceId, char *pBuffer, int nReadLen);
		//; Try to read data from device until nReadLen reached.
	void ReduceRate(float* pEEGData, float* pTrigger, float* pPacket, BOOL& bFirstData, 
		int nCurPointsPerBlock, int nAnalogChannels, int nRateDivider);
		//; Antialiasing filter + decimating filter. It is not used for impedance mode.
	HRESULT Restart(int nStartMode);
	BOOL MyLoadLibary(const char* szFullPath);
	BOOL MyUnLoadLibary();

public:
	static CDevice* GetInstance();
		//; Return the current used instance of device.
	STDMETHODIMP Open();
		//; Open the first device, successful if S_OK else S_FALSE.
	STDMETHODIMP Close();
		//; Close the current device, successful if S_OK else S_FALSE.
	int GetEEGMaxChannels() const
		//; Retrieves the max. number of EEG channels.
		{ return m_nEEGChannels; }
	double GetMaxSampleRate(void) const
		//; Return the max. Rate valid respected with both EEG and AUX channels.
		{ return double(max(m_DeviceProp.RateEeg, m_DeviceProp.RateAux)); }
	int GetAnalogChannels() const
		//; Return the max. analog channels.
		{ return m_nEEGChannels + m_nAUXChannels; }
	UINT GetSamplesPerBlock() const
		//; Return current number of samples of 1 read cycle for each channel.
		{ return UINT(m_nPointsPerBlock);}
	int GetChannels() const
		//; Get the current number of channels(Channel + digital port).
		{ return int(m_nEEGChannels + m_nAUXChannels + 1); }
	DATE GetCurrentOleTime()
		//; Return Ole Date time for 1 reading data block.
		{ return (m_Date += m_BlockTime); }
	HRESULT GetChannelTypes(SAFEARRAY** ppnChanTypes);
		//; Returns table of analog channel type of type char.
	HRESULT GetChannelResolutions(SAFEARRAY **ppdResolutions);
		//; Returns table of analog channel resolution of type double (8-bytes).
	HRESULT GetDeviceInfos(BSTR *pszInfo);
		//; Retrieve formatted string of FirstAmp information of all devices.
	HRESULT GetDeviceSerialNumber(int* pnSerialNumber);
		//; Retrieve the device serial number.
	HRESULT GetDeviceModel(int* pnDeviceModel);
		//: Retrieving the device model (16 channels, 8 channels, etc..).
		//+	Return 
		//+		S_OK	- Successfull.
		//+		Not S_OK - Failed. 
		//+	Parameter
		//.		pnDeviceModel - Type of device (0 = 8 channels, 1 = 16 channels, etc..).
	HRESULT GetRefChannel(int* pnChanIdx);
		//: Retrieving the phys. reference channel index.
		//+	Return 
		//+		S_OK	- Successfull.
		//+		Not S_OK - Failed. 
		//+	Parameter
		//.		pnChanIdx - index of phys. reference channel.
	HRESULT SetDataMode(int nDataMode);
		//; Set the data mode, nDataMode = 0 (normal mode), else 20kHz mode.
	HRESULT SetSamplingRate(double dCurrentRate);
		//: Set current sampling rate. max. rate used if return FALSE.
		//+ Return
		//+		S_OK - successful.
		//+		S_FALSE or E_FAIL - failed because out of range.
		//+	Parameter
		//.		dCurrentRate - current sampling rate.
	// **** Serving Data ************************************************************
	HRESULT Start(int nMode, int nDataMode, int nChannelsPos[4], int nChannelsNeg[4]);
		//: Start the device.
		//+ Return
		//+		S_OK	- Successfull.
		//+     E_FAIL	- Can't find any device.
		//+ Parameter(s)
		//+		nMode	- IAC_AT_IMPEDANCE, IAC_AT_DATA, IAC_AT_CALIBRATION.
		//+		nDataMode - 0(normal), otherwise dm20kHz4Channels
		//+		nChannelsPos - Positive input in pair.
		//.		nChannelsNeg - Negative input in pair.
	HRESULT Stop();
		//; Stop the device. Returns TRUE if successful else returns FALSE.
	STDMETHODIMP GetData(SAFEARRAY **ppfData, int* pnChannels, 
				int* pnPoints, DATE *pDate, int* pnErrorSeverity);
		//: Retrieving data with actual 10 ms samples.
		//+		Reorganize order of data (EEG, AUX, Trig) of type float (4-bytes).
		//+ Return
		//+		S_OK	- Successful.
		//+     S_FALSE	- No data.
		//+ Parameter
		//+		ppfData		- Data buffer.
		//+		pnChannels	- Channel number.
		//+		pnPoints	- Number of samples for each channel.
		//+		pDate		- Current date.
		//.		pnErrorSeverity - 0 = warning, 1 = error.
	STDMETHODIMP GetImpedance(SAFEARRAY **ppfData, int* pnChannels, 
				int* pnEegMax, DATE *pDate, int* pnErrorSeverity);
		//: Peek up the current impedance with order EEG, Ref, Gnd and data unit in Ohm
		//+		of type ULONG(4-bytes).
		//+ Return
		//+		S_OK	- Successful.
		//+     S_FALSE	- No data.
		//+ Parameter
		//+		ppfData		- Data buffer.
		//+		pnChannels	- Number of channels.
		//+		pnEegMax	- Max. number of EEG channels.
		//+		pDate		- Current date.
		//.		nErrorSeverity - 0 = warning, 1 = error.
	HRESULT GetDigitalPort(ULONG nTimeInterval, USHORT* pnSample);
		//: Get current sample of trigger1, trigger2. 
		//+	First bit for trigger1, following trigger2.
		//+ Return
		//+		FALSE - If no sample available.
		//+		TRUE - OK.
		//+ Parameter
		//+		nTimeInterval - trigger serving time interval(at least 10ms).
		//.		pnSample - current 2 bytes trigger sample.
	int GetBufferState();
		//: Get the current buffer filling state.
		//+ Return
		//+		(-1) - If the buffer overflows.
		//.		> 0	 - Current filled buffer size in percent.
	HRESULT GetUserData(char *pBuffer, UINT* pnCount);
		//: Read user (application software) data from device nonvolatile memory.
		//+	Return 
		//+		S_OK - sucessfull written.
		//+		Otherwise failed.
		//+	Parameters
		//+		pBuffer - Pointer to buffer for data. Max. size = 512 bytes.
		//.		pnCount - Returns number of bytes successfully read from device.
	HRESULT SetUserData(char *pBuffer, UINT nSize, UINT* pnCount);
		//: Write user (application software) data to device nonvolatile memory.
		//+	Return 
		//+		S_OK - sucessfull written.
		//+		Otherwise failed.
		//+	Parameters
		//+		pBuffer - Pointer to buffer for data. Max. size = 512 bytes.
		//+		nSize	- Length of data in bytes
		//.		pnCount - Returns number of bytes successfully written to device.
	HRESULT GetIo(UINT* pnInputs, UINT* pnOutputs);
		//; Get digital input / output line.
	HRESULT SetIo(UINT nInputs, UINT nOutputs);
		//; Set digital input / output line.
	HRESULT GetContrast(UINT* pnContrast);
		//; Get constrast of LCD display, range(0..100%).
	HRESULT SetContrast(UINT nConstrast);
		//; Set constrast of LCD display, range(0..100%).
	HRESULT GetBrightness(UINT* pnBrightness);
		//; Get brightness of LCD display, range(0..100%).
	HRESULT SetBrightness(UINT nBrightness);
		//; Set brightness of LCD display, range(0..100%).
};

#define FUNCTION_DEVICE_BEGIN	HRESULT hr = S_FALSE; \
								try {
#define FUNCTION_DEVICE_END			} \
								catch(...) \
								{ \
									hr = E_FAIL; \
									Stop(); \
								} \
								return hr;