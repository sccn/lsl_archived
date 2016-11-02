/**
* Code for deigning camera information via 
* Windows Media Foundation
* written by David Medine
**/

#include <mfapi.h> // for enumerating cameras etc.
#include <mfidl.h>
#include <mfreadwrite.h>

#include <vector>
#include <string>


// for external use
struct wmfCameraInfo {

	int width;
	int height; // screen resolution

	int frameRate;
	int denominator; // windows' FR method

	float requestedFR; // actual FR

	int idx; // format index number

};

class WmfEnumerateCameras {


// public interface for this object
public:

	// param 1: reference to vector of strings to shove available camera names into
	void getCameraNames(std::vector<std::string> &cameraNames);

	// param 1: string representing the name of the camera to select
	// param 2: reference to vector of info structs to shove the available formats into
	void getCameraInfo(std::string deviceName, std::vector<wmfCameraInfo> &cameraInfoVec);
	
private:

	// separated this out to avoid putting any MF burden on the user
	HRESULT EnumerateCameras(std::vector<std::string> &cameraNames);
	HRESULT EnumerateCaptureFormats(IMFMediaSource *pSource, std::vector<wmfCameraInfo> &cameraInfoVec);
	HRESULT SelectDevice(std::string deviceName, std::vector<wmfCameraInfo> &cameraInfoVec);

	UINT32                  m_cDevices;
	IMFActivate             **m_ppDevices;
	int                     selectedDevice;

};