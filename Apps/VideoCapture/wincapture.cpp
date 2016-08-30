// much of the following is copied from MS documentation pages and sdk samples

#include "wincapture.h"
#include <windows.h>
#include <shlwapi.h>
#include <assert.h>
#include <iostream>
#include <string>
#include <sstream>




//int main(void){ return 0; }
const UINT32 TARGET_BIT_RATE = 240 * 1000;

template <class T> void SafeRelease(T **ppT)
{
	if (*ppT)
	{
		(*ppT)->Release();
		*ppT = NULL;
	}
}

HRESULT CopyAttribute(IMFAttributes *pSrc, IMFAttributes *pDest, const GUID& key);

/////////////// IUnknown methods ///////////////

//-------------------------------------------------------------------
//  AddRef
//-------------------------------------------------------------------

ULONG WinCapture::AddRef()
{
	return InterlockedIncrement(&m_nRefCount);
}


//-------------------------------------------------------------------
//  Release
//-------------------------------------------------------------------

ULONG WinCapture::Release()
{
	ULONG uCount = InterlockedDecrement(&m_nRefCount);
	if (uCount == 0)
	{
		delete this;
	}
	return uCount;
}



//-------------------------------------------------------------------
//  QueryInterface
//-------------------------------------------------------------------

HRESULT WinCapture::QueryInterface(REFIID riid, void** ppv)
{
	static const QITAB qit[] =
	{
		QITABENT(WinCapture, IMFSourceReaderCallback),
		{ 0 },
	};
	return QISearch(this, qit, riid, ppv);
}






//-------------------------------------------------------------------
//  constructor
//-------------------------------------------------------------------
//HRESULT WinCapture::CreateInstance(WinCapture **winCapture){
//
//
//	HRESULT hr;
//	// Initialize the COM library
//	hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
//
//	// Initialize Media Foundation
//	if (SUCCEEDED(hr))
//	{
//		hr = MFStartup(MF_VERSION);
//	}
//
//
//	if (winCapture == NULL)
//	{
//		return E_POINTER;
//	}
//
//	WinCapture *pWinCapture = new (std::nothrow) WinCapture();
//
//
//	if (pWinCapture == NULL)
//	{
//		return E_OUTOFMEMORY;
//	}
//
//
//	// The CCapture constructor sets the ref count to 1.
//	*winCapture = pWinCapture;
//	
//	return hr;
//
//}
//
//void WinCapture::release(WinCapture **ppWinCapture){
//	SafeRelease(ppWinCapture);
//}
//
//WinCapture::WinCapture() :
//m_pReader(NULL),
//m_pWriter(NULL),
//m_nRefCount(1),
//m_llBaseTime(0),
//m_pwszSymbolicLink(NULL),
//m_ppDevices(NULL){
//
//	InitializeCriticalSection(&m_critsec); // needed? this is windows version of a mutex object
//	// they claim it is faster than a mutex object
//}

WinCapture::WinCapture(void) :

		m_pReader(NULL),
		m_pWriter(NULL),
		m_nRefCount(1),
		m_llBaseTime(0),
		m_pwszSymbolicLink(NULL),
		m_ppDevices(NULL){

		InitializeCriticalSection(&m_critsec);

	HRESULT hr;
	// Initialize the COM library
	hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);

	// Initialize Media Foundation
	if (SUCCEEDED(hr))
	{
		hr = MFStartup(MF_VERSION);
	}

	// first time
	m_bFirstSample = TRUE;
	m_bIsCapturing = FALSE;
}
//-------------------------------------------------------------------
//  destructor
//-------------------------------------------------------------------

WinCapture::~WinCapture()
{

	finishCapture();
	assert(m_pReader == NULL);
	assert(m_pWriter == NULL);
	DeleteCriticalSection(&m_critsec);

	MFShutdown();
	CoUninitialize();
}



//-------------------------------------------------------------------
//  protected methods
//  for GUI interface
//  SelectDevice finds the camera we are interested in
//  EnumerateFormates enumerates available capture formats
//-------------------------------------------------------------------

HRESULT WinCapture::EnumerateCameras(std::vector<std::string> &cameraNames) {
	//IMFMediaSource *pSource = NULL;
	//cameraNames.clear();
	IMFAttributes *pAttributes = NULL;
	// Create an attribute store to specify the enumeration parameters.
	HRESULT hr = MFCreateAttributes(&pAttributes, 1);
	if (FAILED(hr))
	{
		goto done;
	}


	// Source type: video capture devices
	hr = pAttributes->SetGUID(
		MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE,
		MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_VIDCAP_GUID
		);
	if (FAILED(hr))
	{
		goto done;
	}

	// Enumerate devices.

	hr = MFEnumDeviceSources(pAttributes, &m_ppDevices, &m_cDevices);
	if (FAILED(hr))
	{
		goto done;
	}

	if (m_cDevices == 0)
	{
		hr = E_FAIL;
		goto done;
	}

	WCHAR *szFriendlyName;
	UINT32 cchName;
	std::wstring *ws;
	std::string *s;
	for (int i = 0; i < m_cDevices; i++) {
		hr = m_ppDevices[i]->GetAllocatedString(
			MF_DEVSOURCE_ATTRIBUTE_FRIENDLY_NAME,
			&szFriendlyName, &cchName);
		ws = new std::wstring(szFriendlyName);
		s = new std::string(ws->begin(), ws->end());
		cameraNames.push_back(*s);
		std::cout << cameraNames[i] << std::endl;
	}
	
	goto done;
	

done:
	SafeRelease(&pAttributes);
	return hr;

}

HRESULT WinCapture::SelectDevice(std::string deviceName) {



	HRESULT hr;
	UINT32 cchName;
	WCHAR *szFriendlyName=NULL;
	std::wstring *ws;
	std::string *s; 
	IMFMediaSource *pSource=NULL;

	for (int i = 0; i < m_cDevices; i++) {
		hr = m_ppDevices[i]->GetAllocatedString(
			MF_DEVSOURCE_ATTRIBUTE_FRIENDLY_NAME,
			&szFriendlyName, &cchName);

		// convert unicode name to ascii
		ws = new std::wstring(szFriendlyName);
		s = new std::string(ws->begin(), ws->end());

		// get the selected device 
		if (s->compare(deviceName) == 0) {
			selectedDevice = i;

			// activate the requested device
			hr = m_ppDevices[i]->ActivateObject(IID_PPV_ARGS(&pSource));

	
			// populate the resolution combo-box and associated format
			EnumerateCaptureFormats(pSource);
		}

		delete ws;
		delete s;

	}






	goto done;

done:
	CoTaskMemFree(szFriendlyName);
	pSource->Release();
	hr = m_ppDevices[selectedDevice]->ShutdownObject();
	return hr;
}

HRESULT WinCapture::EnumerateCaptureFormats(IMFMediaSource *pSource) {

	// for filling the combo box fields
	std::stringstream ss;
	IMFPresentationDescriptor *pPD = NULL;
	IMFStreamDescriptor *pSD = NULL;
	IMFMediaTypeHandler *pHandler = NULL;
	IMFMediaType *pType = NULL;

	HRESULT hr = pSource->CreatePresentationDescriptor(&pPD);
	if (FAILED(hr))
	{
		goto done;
	}

	BOOL fSelected;
	hr = pPD->GetStreamDescriptorByIndex(0, &fSelected, &pSD);
	if (FAILED(hr))
	{
		goto done;
	}

	hr = pSD->GetMediaTypeHandler(&pHandler);
	if (FAILED(hr))
	{
		goto done;
	}

	DWORD cTypes = 0;
	hr = pHandler->GetMediaTypeCount(&cTypes);
	if (FAILED(hr))
	{
		goto done;
	}

	DWORD32 width, height;
	UINT32 frameRate;
	UINT32 denominator;
	winCameraInfo cameraInfo;
	infoLen = 0;
	for (DWORD i = 0; i < cTypes; i++)
	{

		hr = pHandler->GetMediaTypeByIndex(i, &pType);

		// get available resolutions
		ss.clear();
		hr = MFGetAttributeSize(pType, MF_MT_FRAME_SIZE, &width, &height);
		std::cout << "width " << width << "   height " << height << std::endl;
		ss << width << "x" << height;
		resVec.push_back(ss.str());

		// get available framerates
		ss.clear();
		hr = MFGetAttributeRatio(pType, MF_MT_FRAME_RATE, &frameRate, &denominator);
		std::cout << "frameRate " << frameRate << "   denominator " << denominator << std::endl;
		ss << (float)frameRate / (float)denominator;
		frameRateVec.push_back(ss.str());

		// pop the info into a database structure
		cameraInfo.width = width;
		cameraInfo.height = height;
		cameraInfo.frameRate = frameRate;
		cameraInfo.denominator = denominator;
		cameraInfo.requestedFR = (float)frameRate / (float)denominator;
		cameraInfo.idx = i;
		infoVec.push_back(cameraInfo);
		infoLen++;

		if (FAILED(hr))
		{
			goto done;
		}


		//SafeRelease(&pType);
	}

goto done;

done:
	SafeRelease(&pPD);
	SafeRelease(&pSD);
	SafeRelease(&pHandler);
	SafeRelease(&pType);
	return hr;
}

// blocking method for reading/writing video, may not work with 
// typical LSL threading procedure

HRESULT WinCapture::rwFunction(void) {

	HRESULT hr;
	DWORD streamIndex, flags;
	LONGLONG llTimeStamp;
	IMFSample *pSample = NULL;

	EnterCriticalSection(&m_critsec);
	// Read another sample.
	hr = m_pReader->ReadSample(
		(DWORD)MF_SOURCE_READER_FIRST_VIDEO_STREAM,
		0,
		&streamIndex,   // actual
		&flags,//NULL,   // flags
		&llTimeStamp,//NULL,   // timestamp
		&pSample    // sample
		);

	if (FAILED(hr)) { goto done; }

	hr = m_pWriter->WriteSample(0, pSample);

	goto done;

done:
	return hr;
	SafeRelease(&pSample);
	LeaveCriticalSection(&m_critsec);
}


/////////////// IMFSourceReaderCallback methods ///////////////

//-------------------------------------------------------------------
// OnReadSample
//
// Called when the IMFMediaSource::ReadSample method completes.
//-------------------------------------------------------------------

HRESULT WinCapture::OnReadSample(
	HRESULT hrStatus,
	DWORD /*dwStreamIndex*/,
	DWORD /*dwStreamFlags*/,
	LONGLONG llTimeStamp,
	IMFSample *pSample      // Can be NULL
	)
{
	EnterCriticalSection(&m_critsec);

	if (!IsCapturing() || m_bIsCapturing == false)
	{
		LeaveCriticalSection(&m_critsec);
		return S_OK;
	}

	HRESULT hr = S_OK;

	if (FAILED(hrStatus))
	{
		hr = hrStatus;
		goto done;
	}

	if (pSample)
	{
		if (m_bFirstSample)
		{
			m_llBaseTime = llTimeStamp;
			m_bFirstSample = FALSE;
		}

		// rebase the time stamp
		llTimeStamp -= m_llBaseTime;

		hr = pSample->SetSampleTime(llTimeStamp);

		if (FAILED(hr)) { goto done; }

		hr = m_pWriter->WriteSample(0, pSample);

		if (FAILED(hr)) { goto done; }
	}

	// Read another sample.
	hr = m_pReader->ReadSample(
		(DWORD)MF_SOURCE_READER_FIRST_VIDEO_STREAM,
		0,
		NULL,   // actual
		NULL,   // flags
		NULL,   // timestamp
		NULL    // sample
		);

done:
	if (FAILED(hr))
	{
		//NotifyError(hr);
	}

	LeaveCriticalSection(&m_critsec);
	return hr;
}

HRESULT WinCapture::initCapture(const WCHAR *pwszFileName, IMFActivate *pActivate) {

	HRESULT hr = S_OK;
	IMFMediaSource *pSource = NULL;
	EncodingParameters params;
	params.subtype = MFVideoFormat_WMV3; // TODO, paramterize this
	params.bitrate = TARGET_BIT_RATE;
	m_llBaseTime = 0;
	IMFMediaType *pType = NULL;
	DWORD sink_stream = 0;

	EnterCriticalSection(&m_critsec);

	// activate the selected device	
	GetDevice(selectedDevice, &pActivate);
	//hr = m_ppDevices[selectedDevice]->ActivateObject(IID_PPV_ARGS(&pSource));
	//m_bIsCapturing = false; // this is set externally here
	hr = pActivate->ActivateObject(
		__uuidof(IMFMediaSource),
		(void**)&pSource
		);
	
	if (SUCCEEDED(hr))
		hr = OpenMediaSource(pSource); // also creates the reader


	if (SUCCEEDED(hr))
	{
		hr = m_pReader->GetCurrentMediaType(
			(DWORD)MF_SOURCE_READER_FIRST_VIDEO_STREAM,
			&pType
			);
	}


	// Create the sink writer 
	if (SUCCEEDED(hr))
	{
		hr = MFCreateSinkWriterFromURL(
			pwszFileName,
			NULL,
			NULL,
			&m_pWriter
			);
	}

	if (SUCCEEDED(hr))
		hr = ConfigureEncoder(params, pType, m_pWriter, &sink_stream);


	// kick off the recording
	if (SUCCEEDED(hr))
	{

		m_llBaseTime = 0;
		m_bIsCapturing = TRUE;

		hr = m_pReader->ReadSample(
			(DWORD)MF_SOURCE_READER_FIRST_VIDEO_STREAM,
			0,
			NULL,
			NULL,
			NULL,
			NULL
			);


	}



	SafeRelease(&pType);
	SafeRelease(&pSource);
	pType = NULL;
	LeaveCriticalSection(&m_critsec);
	return hr;
}

HRESULT WinCapture::restartCapture(void){
	EnterCriticalSection(&m_critsec);
	// Request the first video frame.

	HRESULT hr = m_pReader->ReadSample(
				(DWORD)MF_SOURCE_READER_FIRST_VIDEO_STREAM,
				0,
				NULL,
				NULL,
				NULL,
				NULL
				);
	LeaveCriticalSection(&m_critsec);
	return hr;
}

//-------------------------------------------------------------------
//  protected methods
//  for initializing capture
//  GetDevice activates the device (turns it on or something?)
//
//  OpenMediaSource sets up the source (video sample grabber)
//  so that it has the requested format and maximum framerate for
//  that format, taken from https://msdn.microsoft.com/en-us/library/windows/desktop/ff485858(v=vs.85).aspx
//
//  ConfigureEnocder sets up the sink (video sample writer) with the
//  same parameters as the video source has
//-------------------------------------------------------------------

HRESULT WinCapture::GetDevice(UINT32 index, IMFActivate **ppActivate)
{
	if (index >= Count())
	{
		return E_INVALIDARG;
	}

	*ppActivate = m_ppDevices[index];
	(*ppActivate)->AddRef();

	return S_OK;
}

HRESULT WinCapture::OpenMediaSource(IMFMediaSource *pSource)
{
	HRESULT hr = S_OK;

	IMFAttributes *pAttributes = NULL;

	hr = MFCreateAttributes(&pAttributes, 2);


	// use a callback
	if (SUCCEEDED(hr))
	{
		hr = pAttributes->SetUnknown(MF_SOURCE_READER_ASYNC_CALLBACK, this);
	}

	// set the desired format type
	DWORD dwFormatIndex = (DWORD)formatIdx;

	IMFPresentationDescriptor *pPD = NULL;
	IMFStreamDescriptor *pSD = NULL;
	IMFMediaTypeHandler *pHandler = NULL;
	IMFMediaType *pType = NULL;

	// create the source reader
	if (SUCCEEDED(hr))
	{
		hr = MFCreateSourceReaderFromMediaSource(
			pSource,
			pAttributes,
			&m_pReader
			);
	}

	// steps to set the selected format type
	if (SUCCEEDED(hr)) hr = pSource->CreatePresentationDescriptor(&pPD);
	if (FAILED(hr))
	{
		goto done;
	}

	BOOL fSelected;
	hr = pPD->GetStreamDescriptorByIndex(0, &fSelected, &pSD);
	if (FAILED(hr))
	{
		goto done;
	}

	hr = pSD->GetMediaTypeHandler(&pHandler);
	if (FAILED(hr))
	{
		goto done;
	}

	hr = pHandler->GetMediaTypeByIndex(dwFormatIndex, &pType);
	if (FAILED(hr))
	{
		goto done;
	}

	hr = pHandler->SetCurrentMediaType(pType);
	if (FAILED(hr))
	{
		goto done;
	}

	// print current resolutions
	DWORD32 width, height;
	UINT32 frameRate;
	UINT32 denominator;
	std::cout << "setting media type" << std::endl;
	hr = MFGetAttributeSize(pType, MF_MT_FRAME_SIZE, &width, &height);
	std::cout << "width " << width << "   height " << height << std::endl;


	// get available framerates
	hr = MFGetAttributeRatio(pType, MF_MT_FRAME_RATE, &frameRate, &denominator);
	std::cout << "frameRate " << frameRate << "   denominator " << denominator << std::endl;


	hr = m_pReader->SetCurrentMediaType(
		(DWORD)MF_SOURCE_READER_FIRST_VIDEO_STREAM,
		NULL,
		pType
		);

	// set to maximum framerate?
	hr = pHandler->GetCurrentMediaType(&pType);
	if (FAILED(hr))
	{
		goto done;
	}

	//// Get the maximum frame rate for the selected capture format.

	//// Note: To get the minimum frame rate, use the 
	//// MF_MT_FRAME_RATE_RANGE_MIN attribute instead.

	//PROPVARIANT var;
	//if (SUCCEEDED(pType->GetItem(MF_MT_FRAME_RATE_RANGE_MAX, &var)))
	//{
	//	hr = pType->SetItem(MF_MT_FRAME_RATE, var);

	//	PropVariantClear(&var);

	//	if (FAILED(hr))
	//	{
	//		goto done;
	//	}

	//	hr = pHandler->SetCurrentMediaType(pType);
	//	if (FAILED(hr))
	//	{
	//		goto done;
	//	}

	//	hr = m_pReader->SetCurrentMediaType(
	//		(DWORD)MF_SOURCE_READER_FIRST_VIDEO_STREAM,
	//		NULL,
	//		pType
	//		);
	//}




	goto done;

done:
	SafeRelease(&pPD);
	SafeRelease(&pSD);
	SafeRelease(&pHandler);
	SafeRelease(&pType);
	SafeRelease(&pAttributes);
	return hr;
}


HRESULT WinCapture::ConfigureEncoder(
	const EncodingParameters& params,
	IMFMediaType *pType,
	IMFSinkWriter *pWriter,
	DWORD *pdwStreamIndex
	)
{
	HRESULT hr = S_OK;

	IMFMediaType *pType2 = NULL;

	hr = MFCreateMediaType(&pType2);

	if (SUCCEEDED(hr))
	{
		hr = pType2->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Video);
	}

	if (SUCCEEDED(hr))
	{
		hr = pType2->SetGUID(MF_MT_SUBTYPE, params.subtype);
	}

	if (SUCCEEDED(hr))
	{
		hr = pType2->SetUINT32(MF_MT_AVG_BITRATE, params.bitrate);
	}

	if (SUCCEEDED(hr))
	{
		hr = CopyAttribute(pType, pType2, MF_MT_FRAME_SIZE);
	}

	if (SUCCEEDED(hr))
	{
		hr = CopyAttribute(pType, pType2, MF_MT_FRAME_RATE);
	}

	if (SUCCEEDED(hr))
	{
		hr = CopyAttribute(pType, pType2, MF_MT_PIXEL_ASPECT_RATIO);
	}

	if (SUCCEEDED(hr))
	{
		hr = CopyAttribute(pType, pType2, MF_MT_INTERLACE_MODE);
	}

	if (SUCCEEDED(hr))
	{
		hr = pWriter->AddStream(pType2, pdwStreamIndex);
	}

	SafeRelease(&pType2);
	return hr;
}


BOOL WinCapture::IsCapturing()
{
	EnterCriticalSection(&m_critsec);

	BOOL bIsCapturing = (m_pWriter != NULL);
	
	LeaveCriticalSection(&m_critsec);

	return bIsCapturing;
}

void WinCapture::finishCapture(void){

	//SafeRelease(&pSource);
	EndCaptureInternal();
	//ClearSource(pSource);
	ClearDevices();



}
//-------------------------------------------------------------------
//  protected methods
//  for cleanup
//-------------------------------------------------------------------

void WinCapture::ClearDevices(){
	// release everything
	for (DWORD i = 0; i < m_cDevices; i++)
	{
		m_ppDevices[i]->Release();
	}
	CoTaskMemFree(m_ppDevices);
	m_ppDevices = NULL;

	m_cDevices = 0;
}


void WinCapture::ClearSource(IMFMediaSource *pSource) {
	pSource->Release();
}

HRESULT WinCapture::EndCaptureInternal()
{
	HRESULT hr = S_OK;
	
	EnterCriticalSection(&m_critsec);

	if (m_pWriter)
	{
		hr = m_pWriter->Finalize();
	}

	SafeRelease(&m_pWriter);
	SafeRelease(&m_pReader);

	CoTaskMemFree(m_pwszSymbolicLink);
	m_pwszSymbolicLink = NULL;

	LeaveCriticalSection(&m_critsec);

	return hr;
}




//-------------------------------------------------------------------
// CopyAttribute
//
// Copy an attribute value from one attribute store to another.
//-------------------------------------------------------------------

HRESULT CopyAttribute(IMFAttributes *pSrc, IMFAttributes *pDest, const GUID& key)
{
	PROPVARIANT var;
	PropVariantInit(&var);

	HRESULT hr = S_OK;

	hr = pSrc->GetItem(key, &var);
	if (SUCCEEDED(hr))
	{
		hr = pDest->SetItem(key, var);
	}

	PropVariantClear(&var);
	return hr;
}

























////-------------------------------------------------------------------
////  CheckDeviceLost
////  Checks whether the video capture device was removed.
////
////  The application calls this method when is receives a 
////  WM_DEVICECHANGE message.
////-------------------------------------------------------------------
//
//HRESULT WinCapture::CheckDeviceLost(DEV_BROADCAST_HDR *pHdr, BOOL *pbDeviceLost)
//{
//	if (pbDeviceLost == NULL)
//	{
//		return E_POINTER;
//	}
//
//	EnterCriticalSection(&m_critsec);
//
//	DEV_BROADCAST_DEVICEINTERFACE *pDi = NULL;
//
//	*pbDeviceLost = FALSE;
//
//	if (!IsCapturing())
//	{
//		goto done;
//	}
//	if (pHdr == NULL)
//	{
//		goto done;
//	}
//	if (pHdr->dbch_devicetype != DBT_DEVTYP_DEVICEINTERFACE)
//	{
//		goto done;
//	}
//
//	// Compare the device name with the symbolic link.
//
//	pDi = (DEV_BROADCAST_DEVICEINTERFACE*)pHdr;
//
//	if (m_pwszSymbolicLink)
//	{
//		if (_wcsicmp(m_pwszSymbolicLink, pDi->dbcc_name) == 0)
//		{
//			*pbDeviceLost = TRUE;
//		}
//	}
//
//done:
//	LeaveCriticalSection(&m_critsec);
//	return S_OK;
//}



////////////////////////////////////////////
//QList<qreal> WinCapture::supportedFrameRates(){
//
//	QList<qreal> frameRates;
//	qreal *fr;
//	qreal rate; // for iterating through frameRates to check for duplicates
//	bool push_it;
//
//	for (std::vector<winCameraInfo>::iterator it = infoVec.begin();
//		it != infoVec.end();
//		++it) {
//		fr = new qreal(it->frameRate);
//		// check if we already have it
//		push_it = true;
//		foreach(rate, frameRates) {
//			if (*fr == rate)push_it = false;
//		}
//		if (push_it == true)frameRates.push_back(*fr);
//		delete(fr);
//	}
//
//	return frameRates;
//
//}


// copied from Windows documentation:
// https://msdn.microsoft.com/en-us/library/windows/desktop/ff485858(v=vs.85).aspx
// with some slight adjustments

//HRESULT WinCapture::setDeviceFormat(IMFMediaSource *pSource, int idx) {
////HRESULT WinCapture::setDeviceFormat(int idx) {
//
//
//	DWORD dwFormatIndex = (DWORD)idx;
//	IMFPresentationDescriptor *pPD = NULL;
//	IMFStreamDescriptor *pSD = NULL;
//	IMFMediaTypeHandler *pHandler = NULL;
//	IMFMediaType *pType = NULL;
//
//	HRESULT hr = pSource->CreatePresentationDescriptor(&pPD);
//	if (FAILED(hr))
//	{
//		goto done;
//	}
//
//	BOOL fSelected;
//	hr = pPD->GetStreamDescriptorByIndex(0, &fSelected, &pSD);
//	if (FAILED(hr))
//	{
//		goto done;
//	}
//
//	hr = pSD->GetMediaTypeHandler(&pHandler);
//	if (FAILED(hr))
//	{
//		goto done;
//	}
//
//	hr = pHandler->GetMediaTypeByIndex(dwFormatIndex, &pType);
//	if (FAILED(hr))
//	{
//		goto done;
//	}
//
//	hr = pHandler->SetCurrentMediaType(pType);
//	
//done:
//	SafeRelease(&pPD);
//	SafeRelease(&pSD);
//	SafeRelease(&pHandler);
//	SafeRelease(&pType);
//	return hr;
//
//}
// unused: