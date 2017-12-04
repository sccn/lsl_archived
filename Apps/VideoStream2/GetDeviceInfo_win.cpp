#include "GetDeviceInfo_win.h"

#include <iostream>
#include "saferelease.h"


// puts the name of all connected devices into a string vector
HRESULT GetDeviceInfo_win::EnumerateCameras(std::vector<std::string>& cameraNames) {

	IMFAttributes *pAttributes = NULL;
	// create attribute store
	HRESULT hr = MFCreateAttributes(&pAttributes, 1);
	if (FAILED(hr)) goto done;

	hr = pAttributes->SetGUID(
		MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE,
		MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_VIDCAP_GUID
	);
	if (FAILED(hr)) goto done;

	hr = MFEnumDeviceSources(pAttributes, &m_ppDevices, &m_cDevices);
	if (FAILED(hr)) goto done;

	if (m_cDevices == 0)
	{
		hr = E_FAIL;
		goto done;
	}

	WCHAR* szFriendlyName;
	UINT32 cchName;
	//std::wstring *ws;
	//std::string *s;
	for (int i = 0; i < m_cDevices; i++) {
		hr = m_ppDevices[i]->GetAllocatedString(
			MF_DEVSOURCE_ATTRIBUTE_FRIENDLY_NAME,
			&szFriendlyName, &cchName);
		
		std::wstring ws(szFriendlyName);
			//ws = new std::wstring(szFriendlyName);
		std::string s(ws.begin(), ws.end());
		cameraNames.push_back(s);
		std::cout << cameraNames[i] << std::endl;
	}

	goto done;
done:
	SafeRelease(&pAttributes);
	return hr;

}

HRESULT GetDeviceInfo_win::EnumerateCaptureFormats(IMFMediaSource *pSource, std::vector<wmfCameraInfo>& cameraInfo) {


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
	wmfCameraInfo ci;
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
		ci.width = width;
		ci.height = height;
		ci.frameRate = frameRate;
		ci.denominator = denominator;
		ci.requestedFR = (float)frameRate / (float)denominator;
		ci.idx = i;
		cameraInfo.push_back(ci);
		 
		if (FAILED(hr))
		{
			goto done;
		}

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
HRESULT GetDeviceInfo_win::SelectDevice(std::string deviceName, std::vector<wmfCameraInfo> &cameraInfoVec) {


	HRESULT hr;
	UINT32 cchName;
	WCHAR *szFriendlyName = NULL;
	std::wstring *ws;
	std::string *s;
	IMFMediaSource *pSource = NULL;

	for (int i = 0; i < m_cDevices; i++) {
		hr = m_ppDevices[i]->GetAllocatedString(
			MF_DEVSOURCE_ATTRIBUTE_FRIENDLY_NAME,
			&szFriendlyName, &cchName);

		// convert unicode name to ascii
		std::wstring ws(szFriendlyName);
		std::string s(ws.begin(), ws.end());

		// get the selected device 
		if (s.compare(deviceName) == 0) {
			m_nSelectedDevice = i;

			// activate the requested device
			hr = m_ppDevices[i]->ActivateObject(IID_PPV_ARGS(&pSource));


			// populate the resolution combo-box and associated format
			EnumerateCaptureFormats(pSource, cameraInfoVec);
		}
	}

	goto done;

done:
	CoTaskMemFree(szFriendlyName);
	pSource->Release();
	hr = m_ppDevices[m_nSelectedDevice]->ShutdownObject();
	return hr;
}

void GetDeviceInfo_win::parseCameraOptions(std::vector<wmfCameraInfo>& cameraInfo, t_cameraSettings& cameraSettings) {

	std::vector<float>framerates;
	std::vector<resolution>resolutions;
	// collect all possible framerates
	for (std::vector<wmfCameraInfo>::iterator it1 = cameraInfo.begin();
		it1 != cameraInfo.end();
		++it1) {
		float fr = (float)it1->frameRate / (float)it1->denominator;
		resolution res;
		res.height = it1->height;
		res.width = it1->width;

		bool not_new = false;
		for (t_cameraSettings::iterator it2 = cameraSettings.begin();
			it2 != cameraSettings.end();
			++it2) {
			if (fr == it2->first) {
				it2->second.push_back(res);
				not_new = true;
			}
			
		}
		if(!not_new||cameraSettings.empty()){
			std::pair<float, std::vector<resolution>> p;
			p.first = fr;
			p.second.push_back(res);
			cameraSettings.push_back(p);
		}
	}
}

// public interface
void GetDeviceInfo_win::getCameraNames(std::vector<std::string>& cameraNames) {

	HRESULT hr = EnumerateCameras(cameraNames);
	if (hr != S_OK)
		throw hr;
}

void GetDeviceInfo_win::getCameraInfo(std::string deviceName, std::vector<wmfCameraInfo>& cameraInfo, t_cameraSettings& cameraSettings) {

	HRESULT hr = SelectDevice(deviceName, cameraInfo);
	if (hr != S_OK)
		throw hr;


	parseCameraOptions(cameraInfo, cameraSettings);

	for (std::vector<std::pair<float, std::vector<resolution>>>::iterator it1 = cameraSettings.begin();
		it1 != cameraSettings.end();
		++it1)
		//std::cout << "\t" << it1->first << ": " << std::endl;
		for(std::vector<resolution>::iterator it2 = it1->second.begin();
			it2!=it1->second.end();
			++it2)
			std::cout << "\t" << it1->first << ": " << it2->width << "x" << it2->height << std::endl;
}