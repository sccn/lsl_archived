#ifndef GETDEVICEINFOWIN_H
#define GETDEVICEINFOWIN_H

#include <mfapi.h>
#include <mfidl.h>
#include <mfreadwrite.h>

#include <vector>
#include <string>



struct wmfCameraInfo {
	int width;
	int height;

	int frameRate;
	int denominator;

	float requestedFR;

	int idx;
};

struct resolution {
	int width;
	int height;
	friend bool operator == (resolution const & r1, resolution const & r2) {
		return (r1.width == r2.width) && (r1.height == r2.height);
	}
};

class GetDeviceInfo_win{


public:
	typedef std::vector<std::pair<float, std::vector<resolution>>> t_cameraSettings;
	t_cameraSettings cameraSettings;
	void getCameraNames(std::vector<std::string>& cameraNames);
	void getCameraInfo(std::string deviceName, std::vector<wmfCameraInfo>& cameraInfo);

private:
	
	// helper functions to pare the camera options
	void parseCameraOptions(std::vector<wmfCameraInfo>& cameraInfo);
	// wmf stuff
	HRESULT EnumerateCameras(std::vector<std::string>& cameraNames);
	HRESULT EnumerateCaptureFormats(IMFMediaSource* pSource, std::vector<wmfCameraInfo>& cameraInfo);
	HRESULT SelectDevice(std::string deviceName, std::vector<wmfCameraInfo>& cameraInfo);

	UINT32        m_cDevices;
	IMFActivate   **m_ppDevices;
	int           m_nSelectedDevice;

};

#endif // GETDEVICEINFOWIN_H

