/**
* Code for deigning camera information via 
* Windows Media Foundation
* written by David Medine
**/

#include "mf_enumerate_cameras.h"

#include <iostream>

template <class T> void SafeRelease(T **ppT)
{
	if (*ppT)
	{
		(*ppT)->Release();
		*ppT = NULL;
	}
}

// puts the names of all connected devices into a std::string vector
HRESULT WmfEnumerateCameras::EnumerateCameras(std::vector<std::string> &cameraNameVec) {
	

	
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
		cameraNameVec.push_back(*s);
		std::cout << cameraNameVec[i] << std::endl;
	}
	
	goto done;
	

done:
	SafeRelease(&pAttributes);
	return hr;

}





// once a device is selected, we can see what formats are available 
HRESULT WmfEnumerateCameras::EnumerateCaptureFormats(IMFMediaSource *pSource, std::vector<wmfCameraInfo> &cameraInfoVec) {

	// for filling the combo box fields
	// std::stringstream ss;
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
	wmfCameraInfo cameraInfo;
	//infoLen = 0;
	for (DWORD i = 0; i < cTypes; i++)
	{

		hr = pHandler->GetMediaTypeByIndex(i, &pType);

		// get available resolutions
		//ss.clear();
		hr = MFGetAttributeSize(pType, MF_MT_FRAME_SIZE, &width, &height);
		std::cout << "width " << width << "   height " << height << std::endl;
		//ss << width << "x" << height;
		//resVec.push_back(ss.str());

		// get available framerates
		//ss.clear();
		hr = MFGetAttributeRatio(pType, MF_MT_FRAME_RATE, &frameRate, &denominator);
		std::cout << "frameRate " << frameRate << "   denominator " << denominator << std::endl;
		//ss << (float)frameRate / (float)denominator;
		//frameRateVec.push_back(ss.str());

		// pop the info into a database structure
		cameraInfo.width = width;
		cameraInfo.height = height;
		cameraInfo.frameRate = frameRate;
		cameraInfo.denominator = denominator;
		cameraInfo.requestedFR = (float)frameRate / (float)denominator;
		cameraInfo.idx = i;
		cameraInfoVec.push_back(cameraInfo);
		//infoLen++;

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




//
HRESULT WmfEnumerateCameras::SelectDevice(std::string deviceName, std::vector<wmfCameraInfo> &cameraInfoVec) {


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
			EnumerateCaptureFormats(pSource, cameraInfoVec);
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


// public interface
void WmfEnumerateCameras::getCameraNames(std::vector<std::string> &cameraNameVec){
	EnumerateCameras(cameraNameVec);
}

void WmfEnumerateCameras::getCameraInfo(std::string deviceName, std::vector<wmfCameraInfo> &cameraInfoVec){
	SelectDevice(deviceName, cameraInfoVec);
}