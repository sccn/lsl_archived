#ifndef WINCAPTURE_H
#define WINCAPTURE_H

// qt 5 does not support video writing under windows
// so we have to roll our own

// largely copied and repurposed from Windows SDK sample code



#pragma once



//#include <windowsx.h>
#define WIN32_LEAN_AND_MEAN
#include <new>
//#include <Wmcodecdsp.h>
#include <mfapi.h>
#include <mfidl.h>
#include <mfreadwrite.h>

//#include <strsafe.h>
//#include <shlwapi.h>
//#include <Dbt.h>
//#include <ks.h>
//#include <ksmedia.h>

#include <vector>
#include <string>

// for external use
struct winCameraInfo {

	int width;
	int height; // screen resolution

	int frameRate;
	int denominator; // windows' FR method

	float requestedFR; // actual FR

	int idx; // format index number

};


// from Windows example code


struct EncodingParameters
{
	GUID    subtype;
	UINT32  bitrate;
};

class WinCapture : public IMFSourceReaderCallback{
//class WinCapture {
public:


	WinCapture();
	~WinCapture();



	static HRESULT CreateInstance(
		WinCapture **winCapture	
		);
	void release(WinCapture **ppWinCapture);

	//IUnknown methods
	STDMETHODIMP QueryInterface(REFIID iid, void** ppv);
	STDMETHODIMP_(ULONG) AddRef();
	STDMETHODIMP_(ULONG) Release();

	// IMFSourceReaderCallback methods
	STDMETHODIMP OnReadSample(
		HRESULT hrStatus,
		DWORD dwStreamIndex,
		DWORD dwStreamFlags,
		LONGLONG llTimestamp,
		IMFSample *pSample
		);

	STDMETHODIMP OnEvent(DWORD, IMFMediaEvent *)
	{
		return S_OK;
	}

	STDMETHODIMP OnFlush(DWORD)
	{
		return S_OK;
	}


	HRESULT EnumerateCameras(std::vector<std::string> &cameraNames);
	HRESULT SelectDevice(std::string deviceName);

	// public access to protected methods
	HRESULT rwFunction(void);
	HRESULT initCapture(const WCHAR *pwszFileName, IMFActivate *pActivate); // TODO: error checking
	void finishCapture(); // should be protected...

	// public access to protected fields 
	IMFSourceReader * getSourceReader(void) {
		return m_pReader;
	}
	std::vector<winCameraInfo> getInfoVec() const { return infoVec; }

	BOOL IsCapturing();
	
	void setIsCapturing(BOOL isCapturing) { m_bIsCapturing = isCapturing; }
	//BOOL isCapturing(void) { return m_bIsCapturing; }
	void setFormatIdx(int idx){ formatIdx = idx; }
	BOOL isFirstStart(void){ return m_bFirstSample;  }
	UINT32  Count() const { return m_cDevices; }

	HRESULT restartCapture(void);


protected:

	//WinCapture();
	//virtual ~WinCapture();

//private:

	// methods
	// interfacing with GUI:
	// find the devices and choose the one we have selected


	// called by the above, enumerate the available formats
	HRESULT EnumerateCaptureFormats(IMFMediaSource *pSource);
	



	HRESULT GetDevice(UINT32 index, IMFActivate **ppActivate);
	HRESULT OpenMediaSource(IMFMediaSource *pSource);


	HRESULT ConfigureCapture(const EncodingParameters& param);
	HRESULT ConfigureEncoder(
		const EncodingParameters& params,
		IMFMediaType *pType,
		IMFSinkWriter *pWriter,
		DWORD *pdwStreamIndex
		);

	//BOOL IsCapturing();


	HRESULT EndCaptureInternal();
	void ClearDevices();
	void ClearSource(IMFMediaSource *pSource);
	void    NotifyError(HRESULT hr){ ;} 


	UINT32                  m_cDevices;
	IMFActivate             **m_ppDevices;
	IMFSample               *m_pSample;

	long                    m_nRefCount;        // Reference count.
	CRITICAL_SECTION        m_critsec;

	IMFSourceReader         *m_pReader;
	IMFSinkWriter           *m_pWriter;

	LONGLONG                m_llBaseTime;
	BOOL                    m_bFirstSample;
	BOOL                    m_bIsCapturing;

	WCHAR                   *m_pwszSymbolicLink;

	// for filling combo boxes in the gui
	std::vector<winCameraInfo>infoVec; // for referencing valid frs and resolutions
	//std::vector<std::string> cameraNames;
	std::vector<std::string>resVec;
	std::vector<std::string>frameRateVec;
	int infoLen;

	std::vector<std::string> devNamesASCII; // vector of ascii strings to compare with Qt devices

	int formatIdx; // index  of the format we wish to record from
	int selectedDevice; // index of the device we want to read from

};


#endif // WINCAPTURE_H