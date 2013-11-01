/**
* @mainpage iViewX API Documentation
*
* The iView X SDK provides an interface for communication between 
* your application and iView X. It uses UDP over Ethernet to provide 
* maximum speed and minimum latency for data transfer. By using 
* iView X SDK the programmer does not have to take care about Ethernet 
* communication specific programming. The iView X SDK provides a large 
* set of functions to control SMI eye tracker's and retrieve data online. 
* It supports a growing number of programming languages and environments, 
* e.g. C/C++, .Net, Matlab, Visual Basic, E-Prime.
*
* Important note: To be able to exchange data between iView X and another 
* application with the iView X SDK an Ethernet connection has to be 
* established. Please consult according documentation on how to establish 
* an Ethernet connection between different computers (e.g. the iView X 
* user manual). Even when using iView X and the application on the same 
* PC an Ethernet connection has to be established. Normally this happens 
* via the so called localhost, 127.0.0.1. Please adjust IP address and 
* port settings in iView X and your application accordingly. 
*
* Important Note: This iView X SDK installer contains Windows 32bit DLLs. 
* Therefore, you can install and run the binaries on Windows 64bit OS, but 
* it is not possible to implement the binaries in Windows 64bit applications. 
*
* @author SMI GmbH
*/ 

/**
* @file iViewXAPI.h
*
* @brief The file contains the prototype declaration for all supported 
* functions and data structs the customer needs to use 
**/ 

#pragma once 

#ifndef CALLBACK
#define CALLBACK __stdcall
#endif

#define DLLExport __declspec(dllexport) 


#define RET_SUCCESS													1
#define RET_NO_VALID_DATA											2
#define RET_CALIBRATION_ABORTED										3
#define ERR_COULD_NOT_CONNECT										100
#define ERR_NOT_CONNECTED											101
#define ERR_NOT_CALIBRATED											102
#define ERR_NOT_VALIDATED											103
#define ERR_EYETRACKING_APPLICATION_NOT_RUNNING						104
#define ERR_WRONG_COMMUNICATION_PARAMETER							105
#define ERR_WRONG_DEVICE											111
#define ERR_WRONG_PARAMETER											112
#define ERR_WRONG_CALIBRATION_METHOD								113
#define ERR_CREATE_SOCKET											121
#define ERR_CONNECT_SOCKET											122
#define ERR_BIND_SOCKET												123
#define ERR_DELETE_SOCKET											124
#define ERR_NO_RESPONSE_FROM_IVIEWX									131
#define ERR_INVALID_IVIEWX_VERSION									132
#define ERR_WRONG_IVIEWX_VERSION									133
#define ERR_ACCESS_TO_FILE											171
#define ERR_SOCKET_CONNECTION										181
#define ERR_EMPTY_DATA_BUFFER										191	
#define ERR_RECORDING_DATA_BUFFER									192	
#define ERR_FULL_DATA_BUFFER										193	
#define ERR_IVIEWX_IS_NOT_READY										194	
#define ERR_IVIEWX_NOT_FOUND										201	
#define ERR_IVIEWX_PATH_NOT_FOUND									202	
#define ERR_IVIEWX_ACCESS_DENIED									203
#define ERR_IVIEWX_ACCESS_INCOMPLETE								204
#define ERR_IVIEWX_OUT_OF_MEMORY									205
#define ERR_CAMERA_NOT_FOUND										211
#define ERR_WRONG_CAMERA											212
#define ERR_WRONG_CAMERA_PORT										213
#define ERR_COULD_NOT_OPEN_PORT										220
#define ERR_COULD_NOT_CLOSE_PORT									221
#define ERR_AOI_ACCESS												222
#define ERR_AOI_NOT_DEFINED											223
#define ERR_FEATURE_NOT_LICENSED									250


/*
* With these defines it is possible to setup the logging status 
* for the function "iV_Log". With "iV_Log" it is possible to observe the 
* communication between a user’s application and iView X and/or function 
* calls. Log levels can be combined (e.g. LOG_BUG | LOG_IV_COMMAND | LOG_ETCOM). 
*/ 
#define LOG_LEVEL_BUG					1	
#define LOG_LEVEL_iV_FCT				2	
#define LOG_LEVEL_ALL_FCT				4	
#define LOG_LEVEL_IV_COMMAND			8	
#define LOG_LEVEL_RECV_IV_COMMAND		16	


/* 
* With ET_PARAM_ and function "iV_SetTrackingParameter" it is possible 
* to change iView X tracking parameters, for example pupil threshold and 
* corneal reflex thresholds, eye image contours, and other parameters.
* 
* Important note: This function can strongly affect tracking stability of 
* your iView X system. Only experienced users should use this function. 
*/
#define ET_PARAM_EYE_LEFT				0
#define ET_PARAM_EYE_RIGHT				1
#define ET_PARAM_EYE_BOTH				2
#define ET_PARAM_PUPIL_THRESHOLD		0
#define ET_PARAM_REFLEX_THRESHOLD		1
#define ET_PARAM_SHOW_AOI				2
#define ET_PARAM_SHOW_CONTOUR			3
#define ET_PARAM_SHOW_PUPIL				4
#define ET_PARAM_SHOW_REFLEX			5
#define ET_PARAM_DYNAMIC_THRESHOLD		6
#define ET_PARAM_PUPIL_AREA				11
#define ET_PARAM_PUPIL_PERIMETER		12
#define ET_PARAM_PUPIL_DENSITY			13
#define ET_PARAM_REFLEX_PERIMETER		14
#define ET_PARAM_REFLEX_PUPIL_DISTANCE	15
#define ET_PARAM_MONOCULAR				16
#define ET_PARAM_SMARTBINOCULAR			17


/*
* The enumeration ETDevice can be used in connection with 
* "iV_GetSystemInfo" to get information about which type of device is 
* connected to iView X. It is part of the "SystemInfoStruct".
* (NONE = 0, RED = 1, HiSpeed = 2, MRI/MEG = 3, HED = 4, ETG = 5, Custom = 6) 
*/
enum ETDevice {NONE, RED, REDm, HiSpeed, MRI, HED, ETG, Custom};


/*
* The enumeration ETApplication can be used to start iView X or iView X OEM 
* application dependent to the used eye tracking device. Set this as a 
* parameter in "iV_Start" function. 
* (iViewX = 0, iViewXOEM = 1) 
*/
enum ETApplication {iViewX, iViewXOEM, iViewETG};


/**
* @struct SystemInfoStruct
*
* @brief This struct provides information about the eyetracking system in use. 
*
* The struct contains the following information:
* samplerate:			sample rate of eyetracking system in use
* iV_MajorVersion:		major version number of iView X in use
* iV_MinorVersion:		minor version number of iView X in use
* iV_Buildnumber:		build number of iView X in use
* API_MajorVersion:		major version number of iView X SDK in use
* API_MinorVersion:		minor version number of iView X SDK in use
* API_Buildnumber:		build number of iView X SDK in use
* iV_ETDevice:			type of eyetracking device
*
* To update information in "SystemInfoStruct" use function "iV_GetSystemInfo".
*/ 
struct SystemInfoStruct
{
	int samplerate;					
	int iV_MajorVersion;			
	int iV_MinorVersion;			
	int iV_Buildnumber;				
	int API_MajorVersion;			
	int API_MinorVersion;			
	int API_Buildnumber;			
	enum ETDevice iV_ETDevice;		
};


/**
* @struct CalibrationPointStruct
*
* @brief This struct provides information about the current calibration point. 
*
* The struct contains the following information:
* number:		number of calibration point that is currently active
* positionX:	horizontal position of calibration point that is currently active
* positionY:	vertical position of calibration point that is currently active
* 
* To update information in "CalibrationPointStruct" use function 
* "iV_GetCurrentCalibrationPoint" during a calibration or validation procedure. 
*/ 
struct CalibrationPointStruct
{
	int number;							
	int positionX;						
	int positionY;						
};


/**
* @struct EyeDataStruct
*
* @brief This struct provides information about eye data.
*
* The struct contains the following information:
* gazeX:			horizontal gaze position [pixel]
* gazeY:			vertical gaze position [pixel]
* diam:				pupil diameter [pixel, mm] (mm for RED devices)
* eyePositionX:		horizontal eye position relative to camera (only for RED)
* eyePositionY: 	vertical eye position relative to camera (only for RED)
* eyePositionZ:		distance to camera (only for RED)
*
* "EyeDataStruct" is part of "SampleStruct". To update information 
* in "SampleStruct" use function "iV_GetSample".
*/ 
struct EyeDataStruct
{
	double gazeX;					
	double gazeY;					
	double diam;					
	double eyePositionX;			
	double eyePositionY;			
	double eyePositionZ;			
};


/**
* @struct SampleStruct
*
* @brief This struct provides information about gaze data samples. 
*
* The struct contains the following information:
* timestamp:		timestamp of current gaze data sample [microseconds]
* leftEye:			eye data left eye
* rightEye:			eye data right eye
* planeNumber:		plane number of gaze data sample (only for HED HT)
*
* The data describes the last gaze data sample that has been calculated. 
* It will be updated when a new gaze data sample has been calculated.
* To update information in "SampleStruct" use function "iV_GetSample".
*/ 
struct SampleStruct
{
	long long timestamp;				
	EyeDataStruct leftEye;				
	EyeDataStruct rightEye;				
	int	planeNumber;					
};


/**
* @struct SampleStruct32
*
* @brief This struct provides information about gaze data samples. 
*
* The struct contains the following information:
* timestamp:		timestamp of the last gaze data sample [microseconds]
* leftEye:			eye data left eye
* rightEye:			eye data right eye
* planeNumber:		plane number of gaze data sample
*
* The data describes the last gaze data sample that has been calculated. 
* It will be updated when a new gaze data sample has been calculated.
* To update information in "SampleStruct32" use function "iV_GetSample32".
*/ 
struct SampleStruct32
{
	double timestamp;					
	EyeDataStruct leftEye;				
	EyeDataStruct rightEye;				
	int	planeNumber;					
};


/**
* @struct EventStruct
*
* @brief This struct provides information about the last eye event that has 
* been calculated. 
*
* The struct contains the following information:
* eventType:	type of eye event, 'F' for fixation (at the moment only 
*				fixations are supported)
* eye:			related eye, 'l' for left eye, 'r' for right eye
* startTime: 	start time of the event in microseconds
* endTime: 		end time of the event in microseconds
* duration: 	duration of the event in microseconds
* positionX:	horizontal position of the fixation event [pixel]
* positionY:	vertical position of the fixation event [pixel]
*
* The data describes the last eye event that has been calculated. It will be 
* updated when a new event has been calculated.
* To update information in "EventStruct" use function "iV_GetEvent".
*/ 
struct EventStruct
{
	char eventType;						
	char eye;							
	long long startTime;				
	long long endTime;					
	long long duration;					
	double positionX;					
	double positionY;					
};


/**
* @struct EventStruct32
*
* @brief This struct provides information about the last eye event that has 
* been calculated. 
*
* The struct contains the following information:
* eventType:	type of eye event, 'F' for fixation (at the moment only 
*				fixations are supported)
* eye:			related eye, 'l' for left eye, 'r' for right eye
* startTime: 	start time of the event in microseconds
* endTime: 		end time of the event in microseconds
* duration: 	duration of the event in microseconds
* positionX:	horizontal position of the fixation event [pixel]
* positionY:	vertical position of the fixation event [pixel]
*
* The data describes the last eye event that has been calculated. It will be 
* updated when a new event has been calculated.
* To update information in "EventStruct32" use function "iV_GetEvent32".
*/ 
struct EventStruct32
{
	double startTime;					
	double endTime;						
	double duration;					
	double positionX;					
	double positionY;					
	char eventType;						
	char eye;							
};


/**
* @struct AccuracyStruct
*
* @brief This struct provides information about the last validation. 
*
* The struct contains the following information:
* deviationLX:	horizontal deviation target - gaze position for left eye [°]
* deviationLY:	vertical deviation target - gaze position for left eye [°]
* deviationRX:	horizontal deviation target - gaze position for right eye [°]
* deviationRY:	vertical deviation target - gaze position for right eye [°]
*
* To update information in "AccuracyStruct" use function "iV_GetAccuracy".
*/ 
struct AccuracyStruct
{
	double deviationLX;				
	double deviationLY;				
	double deviationRX;				
	double deviationRY;				
};


/**
* @struct CalibrationStruct
*
* @brief Use this struct to customize calibration behavior.
*
* The struct contains the following information:
* method:				Select Calibration Method (default: 5) 
* visualization:		Set Visualization Status [0: visualization by external 
*						stimulus program 1: visualization by SDK (default)]  
* displayDevice:		Set Display Device and resolution 
*						[0: primary device (default), 1: secondary device]
* speed:				Set Calibration/Validation Speed [0: slow (default), 1: fast] 
* autoAccept:			Set Calibration/Validation Point Acceptance 
*						[1: automatic (default) 0: manual] 
* foregroundBrightness:	Set Calibration/Validation Target Brightness 
*						[0..255] (default: 20) 
* backgroundBrightness:	Set Calibration/Validation Background Brightness 
*						[0..255] (default: 239) 
* targetShape:			Set Calibration/Validation Target Shape 
*						[IMAGE = 0, CIRCLE1 = 1 (default), CIRCLE2 = 2, CROSS = 3]
* targetSize:			Set Calibration/Validation Target Size 
*						(default: 10 pixels) 
* targetFilename: 		Select Custom Calibration/Validation Target
*
* To set calibration parameters with "CalibrationStruct" use function 
* "iV_SetupCalibration".
*/ 
struct CalibrationStruct
{
	int method;	
	int visualization;	
	int displayDevice;	
	int speed;	
	int autoAccept;	
	int foregroundBrightness;
	int backgroundBrightness;
	int targetShape;
	int targetSize;	
	char targetFilename[256];
};


/**
* @struct StandAloneModeGeometryStruct
*
* @brief Use this struct to customize RED operation mode. 
*		(see chapter 'RED Stand Alone Mode' in the iView X SDK Manual) 
*
* The struct contains the following information:
* setupName:			name of the profile 
* stimX:				horizontal stimulus calibration size [mm] 
* stimY:				vertical stimulus calibration size [mm] 
* stimHeightOverFloor:	distance floor to stimulus screen [mm]
* redHeightOverFloor:	distance floor to RED [mm]
* redStimDist:			distance RED to stimulus screen [mm]
* redInclAngle:			RED inclination angle [°]
*
* For setting up RED operation mode parameters with 
* "StandAloneModeGeometryStruct" use function "iV_SetupStandAloneMode". 
*/ 
struct StandAloneModeGeometryStruct
{
	char setupName[256];
	int stimX;
	int stimY;
	int stimHeightOverFloor;
	int redHeightOverFloor;
	int redStimDist;
	int redInclAngle;
};

/**
* @struct REDStandAloneModeStruct
*
* @brief Please use StandAloneModeGeometryStruct struct to customize RED operation mode. 
*/ 
struct REDStandAloneModeStruct
{
	int stimX;
	int stimY;
	int stimHeightOverFloor;
	int redHeightOverFloor;
	int redStimDist;
	int redInclAngle;
};


/**
* @struct MonitorAttachedGeometryStruct
*
* @brief Use this struct to customize RED-m operation mode. 
*		(see chapter 'RED Monitor Attached Mode' in the iView X SDK Manual) 
*
* The struct contains the following information:
* setupName:			name of the profile 
* stimX:				horizontal stimulus calibration size [mm] 
* stimY:				vertical stimulus calibration size [mm] 
* redStimDistHeight:	vertical distance RED-m to stimulus screen [mm]
* redStimDistDepth:		horizontal distance RED-m to stimulus screen [mm]
* redInclAngle:			RED-m inclination angle [°]
*
* For setting up RED-m operation mode parameters with 
* "MonitorAttachedGeometryStruct" use function "iV_SetupMonitorAttachedGeometry".
*/ 
struct MonitorAttachedGeometryStruct
{
	char setupName[256];
	int stimX;
	int stimY;
	int redStimDistHeight;
	int redStimDistDepth;
	int redInclAngle;
};

/**
* @struct REDMonitorAttachedGeometryStruct
*
* @brief Please use MonitorAttachedGeometryStruct struct to customize RED-m operation mode. 
*/ 
struct REDMonitorAttachedGeometryStruct
{
	int stimX;
	int stimY;
	int redStimDistHeight;
	int redStimDistDepth;
	int redInclAngle;
};


/**
* @struct ImageStruct
*
* @brief Use this struct to get raw eye image and scene video data 
*
* The struct contains the following information:
* imageHeight:		vertical size of the image [pixel]
* imageWidth:		horizontal size of the image [pixel]
* imageSize:		image data size [byte]
* imageBuffer:		pointer to image data
*
* receives online eye image, tracking monitor or scene video with "iV_GetEyeImage", 
* "iV_GetTrackingMonitor", "iV_GetSceneVideo", "iV_SetEyeImageCallback", 
* "iV_SetTrackingMonitorCallback", or "iV_SetScenevideoCallback" function 
*/ 
struct ImageStruct
{
	int imageHeight;
	int imageWidth;
	int imageSize;
	char* imageBuffer;

	DLLExport ImageStruct();
};


/**
* @struct DateStruct
*
* @brief Use this struct to get the license due date 
*
* The struct contains the following information:
* day:			day of license expiration 
* month:		month of license expiration 
* year:			year of license expiration 
*
* receives the license due date with "iV_GetLicenseDueDate" function 
*/ 
struct DateStruct
{
	int day;
	int month;
	int year;
};


/**
* @struct AOIRectangleStruct
*
* @brief Use this struct to customize AOI position on screen 
*
* The struct contains the following information:
* x1:		x-coordinate of left border of the AOI
* x2:		x-coordinate of right border of the AOI
* y1:		y-coordinate of upper border of the AOI
* y2:		y-coordinate of lower border of the AOI
*
* defines position of AOIs on screen. Use "iV_DefineAOI" to setup an AOI 
*/ 
struct AOIRectangleStruct
{
	int x1;
	int x2;
	int y1;
	int y2;
};


/**
* @struct AOIStruct
*
* @brief Use this struct to customize trigger AOIs 
*
* The struct contains the following information:
* enabled:			enable/disable trigger functionality [1: enabled, 0: disabled] 
* aoiName[256]:		name of AOI 
* aoiGroup[256]:	group name of AOI 
* position:			position of AOI 
* fixationHit:		uses fixations or raw data as trigger 
*					[1: fixation hit, 0: raw data hit]
* eye:				["left", "right"]
* outputValue:		TTL output value 
* outputMessage:	message in idf data stream 
*
* defines AOIs on screen, trigger parameter and trigger values to synchronize 
* raw or fixation AOI hits with external devices. Use "iV_DefineAOIPort" and 
* "iV_DefineAOI" functions to setup 
*/ 
struct AOIStruct
{
	int enabled;
	char aoiName[256];
	char aoiGroup[256];
	AOIRectangleStruct position;
	int fixationHit;
	int outputValue;
	char outputMessage[256];
	char eye;
};



typedef int (CALLBACK *pDLLSetCalibrationPoint)(struct CalibrationPointStruct calibrationPoint);
typedef int (CALLBACK *pDLLSetSample)(struct SampleStruct rawDataSample);
typedef int (CALLBACK *pDLLSetEvent)(struct EventStruct eventDataSample);
typedef int (CALLBACK *pDLLSetEyeImage)(struct ImageStruct eyeImage);
typedef int (CALLBACK *pDLLSetSceneVideo)(struct ImageStruct sceneVideo);
typedef int (CALLBACK *pDLLSetTrackingMonitor)(struct ImageStruct trackingMonitor);



/**
* @brief	aborts a calibration or validation 
* 
* @return	RET_SUCCESS				- intended functionality has been fulfilled 
* @return	ERR_NOT_CONNECTED		- no connection established
* @return	ERR_WRONG_DEVICE		- eye tracking device required for this 
*									function is not connected
**/ 
DLLExport int __stdcall iV_AbortCalibration();


/**
* @brief	accepts a calibration or validation point (participant has to be tracked; 
*										only during a calibration or validation) 
* 
* @return	RET_SUCCESS				- intended functionality has been fulfilled 
* @return	ERR_NOT_CONNECTED		- no connection established
* @return	ERR_WRONG_DEVICE		- eye tracking device required for this 
*									function is not connected
**/ 
DLLExport int __stdcall iV_AcceptCalibrationPoint();


/**
* @brief 	starts a calibration procedure. Change calibration and validation parameter with 
*			"iV_SetupCalibration". 
*			If "CalibrationStruct::visualization" is set to "1" with "iV_SetupCalibration" 
*			"iV_Calibrate" will not return until the calibration has been finished or aborted. 
*
* @return	RET_SUCCESS					- intended functionality has been fulfilled
* @return	ERR_NOT_CONNECTED			- no connection established
* @return	ERR_WRONG_DEVICE 			- eye tracking device required for this function 
*										is not connected
* @return	ERR_WRONG_CALIBRATION_METHOD - eye tracking device required for this 
*										calibration method is not connected
**/
DLLExport int __stdcall iV_Calibrate();


/**
* @brief	changes the position of a calibration point 
* 
* @param	number		- selected calibration point
* @param	positionX	- new X position on screen
* @param	positionY	- new Y position on screen
*
* @return	RET_SUCCESS					- intended functionality has been fulfilled 
* @return	ERR_NOT_CONNECTED			- no connection established
* @return	ERR_NO_RESPONSE_FROM_IVIEWX - no response from iView X; check 
*										calibration name / identifier
**/ 
DLLExport int __stdcall iV_ChangeCalibrationPoint(int number, int positionX, int positionY);


/**
* @brief	removes all AOIs  
* 
* @return	RET_SUCCESS				- intended functionality has been fulfilled 
* @return	ERR_AOI_ACCESS			- failed to access AOI data 
**/ 
DLLExport int __stdcall iV_ClearAOI();


/**
* @brief 	clears the data buffer and scene video buffer (if connected 
*			eyetracking device is "HED").
*
* @return	RET_SUCCESS					- intended functionality has been fulfilled
* @return	ERR_NOT_CONNECTED			- no connection established
* @return	ERR_WRONG_DEVICE			- eye tracking device required for this function 
*										is not connected
* @return	ERR_EMPTY_DATA_BUFFER		- recording buffer is empty 
* @return	ERR_RECORDING_DATA_BUFFER	- recording is activated 
**/
DLLExport int __stdcall iV_ClearRecordingBuffer();


/**
* @brief 	configures the API data filter 
*
* @param	filter	- IP address of iView X computer 
* @param	data	- port being used by iView X SDK for sending data to 
*
* @return	RET_SUCCESS					- intended functionality has been fulfilled
* @return	ERR_NOT_CONNECTED			- no connection established
**/
DLLExport int __stdcall iV_ConfigureFilter(int filter, void* data);


/**
* @brief	establishes a UDP connection to iView X.
*			"iV_Connect" will not return until connection has been established. 
*			If no connection can be established it will return after three seconds.
* 
* @param	SendIPAddress	- IP address of iView X computer 
* @param	SendPort		- port being used by iView X SDK for sending data to 
*							iView X 
* @param	RecvIPAddress	- IP address of local computer 
* @param	ReceivePort		- port being used by iView X SDK for receiving data 
*							from iView X
*
* @return	RET_SUCCESS				- intended functionality has been fulfilled 
* @return	ERR_WRONG_PARAMETER		- parameter out of range
* @return	ERR_COULD_NOT_CONNECT	- failed to establish connection
**/ 
DLLExport int __stdcall iV_Connect(char sendIPAddress[16], int sendPort, char recvIPAddress[16], int receivePort);


/**
* @brief	continues eye tracking and calculating gaze data. eye tracking can be 
*			paused with "iV_PauseEyetracking" 
* 
* @return	RET_SUCCESS				- intended functionality has been fulfilled 
* @return	ERR_NOT_CONNECTED		- no connection established
**/ 
DLLExport int __stdcall iV_ContinueEyetracking();


/**
* @brief	continues gaze data recording and scene video recording (if connected 
*			eyetracking device is "HED")
*			"iV_ContinueRecording" does not return until gaze and scene video 
*			recording is continued 
*
* @param	etMessage	- text message to be written to data file
*
* @return	RET_SUCCESS				- intended functionality has been fulfilled
* @return	ERR_NOT_CONNECTED		- no connection established
* @return	ERR_WRONG_DEVICE		- eye tracking device required for this function 
*									is not connected
* @return	ERR_EMPTY_DATA_BUFFER	- recording buffer is empty 
**/
DLLExport int __stdcall iV_ContinueRecording(char etMessage[256]);


/**
* @brief	defines an AOI. The API can handle up to 20 AOIs. 
* 
* @param	aoiData - see reference information for "AOIStruct"
* 
* @return	RET_SUCCESS				- intended functionality has been fulfilled 
* @return	ERR_WRONG_PARAMETER		- parameter out of range
**/ 
DLLExport int __stdcall iV_DefineAOI(struct AOIStruct *aoiData);


/**
* @brief	selects a port for sending out TTL trigger 
* 
* @param	port - port address 
* 
* @return	RET_SUCCESS				- intended functionality has been fulfilled 
* @return	ERR_WRONG_PARAMETER		- parameter out of range
* @return	ERR_COULD_NOT_OPEN_PORT	- failed to establish TTL port
**/ 
DLLExport int __stdcall iV_DefineAOIPort(int port);


/**
* @brief 	deletes the RED-m monitor attached geometry setup  
*			(see chapter 'RED Monitor Attached Mode' in the iView X SDK Manual) 
*
* @param	name	- name of the geometry setup which will be deleted 
*
* @return RET_SUCCESS			- intended functionality has been fulfilled
* @return ERR_NOT_CONNECTED		- no connection established
* @return ERR_WRONG_PARAMETER	- parameter out of range
* @return ERR_WRONG_DEVICE		- eye tracking device required for this function 
*								is not connected
**/
DLLExport int __stdcall iV_DeleteMonitorAttachedGeometry(char name[256]);


/**
* @brief 	deletes the RED stand alone mode 
*
* @param	name	- name of the geometry setup which will be deleted 
*
* @return RET_SUCCESS			- intended functionality has been fulfilled
* @return ERR_NOT_CONNECTED		- no connection established
* @return ERR_WRONG_PARAMETER	- parameter out of range
* @return ERR_WRONG_DEVICE		- eye tracking device required for this function 
*								is not connected
**/
DLLExport int __stdcall iV_DeleteStandAloneGeometry(char name[256]);


/**
* @brief	disables all AOIs with the given name 
* 
* @param	aoiName - name of the AOI which will be disabled  
* 
* @return	RET_SUCCESS				- intended functionality has been fulfilled 
* @return	RET_NO_VALID_DATA		- no data available
* @return	ERR_AOI_ACCESS			- failed to access AOI data
**/ 
DLLExport int __stdcall iV_DisableAOI(char* aoiName);


/**
* @brief	disables an AOI group  
* 
* @param	aoiGroup - name of the AOI group which will be disabled  
* 
* @return	RET_SUCCESS				- intended functionality has been fulfilled 
* @return	RET_NO_VALID_DATA		- no data available
* @return	ERR_AOI_ACCESS			- failed to access AOI data
**/ 
DLLExport int __stdcall iV_DisableAOIGroup(char* aoiGroup);


/**
* @brief 	disables the raw data filter
*
* @return	RET_SUCCESS		- intended functionality has been fulfilled
**/
DLLExport int __stdcall iV_DisableGazeDataFilter();


/**
* @brief 	enables a CPU high performance mode  
*
* @return	RET_SUCCESS			- intended functionality has been fulfilled
* @return	ERR_NOT_CONNECTED	- no connection established
**/
DLLExport int __stdcall iV_DisableProcessorHighPerformanceMode();


/**
* @brief	disconnects from iView X
*			"iV_Disconnect" will not return until the connection 
*			has been disconnected.
* 
* @return	RET_SUCCESS				- intended functionality has been fulfilled 
* @return	ERR_DELETE_SOCKET		- failed to delete sockets 
**/ 
DLLExport int __stdcall iV_Disconnect();


/**
* @brief	enables all AOIs with the given name 
* 
* @param	aoiName - name of the AOI which will be enabled 
* 
* @return	RET_SUCCESS				- intended functionality has been fulfilled 
* @return	RET_NO_VALID_DATA		- no data available
* @return	ERR_AOI_ACCESS			- failed to access AOI data
**/ 
DLLExport int __stdcall iV_EnableAOI(char* aoiName);


/**
* @brief	enables an AOI group  
* 
* @param	aoiGroup - name of the AOI group which will be enabled 
* 
* @return	RET_SUCCESS				- intended functionality has been fulfilled 
* @return	RET_NO_VALID_DATA		- no data available
* @return	ERR_AOI_ACCESS			- failed to access AOI data
**/ 
DLLExport int __stdcall iV_EnableAOIGroup(char* aoiGroup);


/**
* @brief 	enables a gaze data filter. This API bilateral filter was implemented
*			due to special HCI application requirements 
*
* @return	RET_SUCCESS		- intended functionality has been fulfilled
**/
DLLExport int __stdcall iV_EnableGazeDataFilter();


/**
* @brief 	enables a CPU high performance mode  
*
* @return	RET_SUCCESS			- intended functionality has been fulfilled
* @return	ERR_NOT_CONNECTED	- no connection established
**/
DLLExport int __stdcall iV_EnableProcessorHighPerformanceMode();


/**
* @brief 	updates "accuracyData" with current accuracy data 
*			If parameter "visualization" is set to "1" the accuracy data will be 
*			visualized in a dialog window
*			iV_GetAccuracy will not return until "AccuracyStruct" is updated 
*
* @param	accuracyData	- see reference information for "AccuracyStruct"
* @param	visualization	- 0: no visualization 
*							1: accuracy data will be visualized in a dialog window 
*
* @return	RET_SUCCESS			- intended functionality has been fulfilled
* @return	RET_NO_VALID_DATA	- no data available
* @return	ERR_NOT_CONNECTED	- no connection established
* @return	ERR_NOT_CALIBRATED	- system is not calibrated
* @return	ERR_NOT_VALIDATED	- system is not validated
* @return	ERR_WRONG_PARAMETER	- parameter out of range
**/
DLLExport int __stdcall iV_GetAccuracy(struct AccuracyStruct *accuracyData, int visualization);


/**
* @brief 	updates "imageData" with validation visualization 
*
* @param	imageData	- see reference information for "ImageStruct"
*
* @return	RET_SUCCESS			- intended functionality has been fulfilled
* @return	ERR_NOT_CONNECTED	- no connection established
* @return	ERR_NOT_CALIBRATED	- system is not calibrated
* @return	ERR_NOT_VALIDATED	- system is not validated
**/
DLLExport int __stdcall iV_GetAccuracyImage(struct ImageStruct *imageData);


/**
* @brief 	gives back the AOI value 
*
* @param	aoiOutputValue		- provides the AOI output value 
*
* @return	RET_SUCCESS			- intended functionality has been fulfilled 
* @return	ERR_NOT_CONNECTED	- no connection established 
* @return	ERR_AOI_NOT_DEFINED	- no defined AOI found 
**/
DLLExport int __stdcall iV_GetAOIOutputValue(int *aoiOutputValue);


/**
* @brief	"iV_GetCalibrationParameter" updates the current calibration information stored in "CalibrationStruct" 
*
* @param	calibrationData	- see reference information for "CalibrationStruct"
*
* @return	RET_SUCCESS			- intended functionality has been fulfilled
* @return	ERR_NOT_CONNECTED	- no connection established
**/
DLLExport int __stdcall iV_GetCalibrationParameter(struct CalibrationStruct *calibrationData);


/**
* @brief	"iV_GetCalibrationPoint" delivers the information of a calibration point 
*
* @param	calibrationPointNumber	- number of requested calibration point 
* @param	calibrationPoint		- information of requested calibration point, stored in "CalibrationPointStruct" 
*
* @return	RET_SUCCESS			- intended functionality has been fulfilled 
* @return	ERR_NOT_CONNECTED	- no connection established 
**/
DLLExport int __stdcall iV_GetCalibrationPoint(int calibrationPointNumber, struct CalibrationPointStruct *calibrationPoint);


/**
* @brief 	updates "currentCalibrationPoint" with current calibration point data
*
* @param	actualCalibrationPoint	- see reference information for "CalibrationPointStruct"
*
* @return	RET_SUCCESS 		- intended functionality has been fulfilled
* @return	RET_NO_VALID_DATA	- no new data available
* @return	ERR_NOT_CONNECTED	- no connection established
**/
DLLExport int __stdcall iV_GetCurrentCalibrationPoint(struct CalibrationPointStruct *actualCalibrationPoint);


/**
* @brief 	requests the eye tracker timestamp
*
* @param	currentTimestamp	- provides the internal time stamp 
*
* @return RET_SUCCESS			- intended functionality has been fulfilled
* @return RET_NO_VALID_DATA		- no new data available
* @return ERR_NOT_CONNECTED		- no connection established
**/
DLLExport int __stdcall iV_GetCurrentTimestamp(long long *currentTimestamp);


/**
* @brief 	updates "eventDataSample" with current event data
*
* @param	eventDataSample		- see reference information for "EventStruct"
*
* @return	RET_SUCCESS			- intended functionality has been fulfilled
* @return	RET_NO_VALID_DATA	- no new data available
* @return	ERR_NOT_CONNECTED	- no connection established
**/
DLLExport int __stdcall iV_GetEvent(struct EventStruct *eventDataSample);


/**
* @brief 	updates "eventDataSample" with current event data
*
* @param	eventDataSample		- see reference information for "EventStruct32"
*
* @return	RET_SUCCESS			- intended functionality has been fulfilled
* @return	RET_NO_VALID_DATA	- no new data available
* @return	ERR_NOT_CONNECTED	- no connection established
**/
DLLExport int __stdcall iV_GetEvent32(struct EventStruct32 *eventDataSample);


/**
* @brief 	updates "imageData" with current eye image 
*
* @param	imageData - see reference information for "ImageStruct"
*
* @return RET_SUCCESS		- intended functionality has been fulfilled
* @return RET_NO_VALID_DATA	- no new data available
* @return ERR_NOT_CONNECTED	- no connection established
* @return ERR_WRONG_DEVICE	- eye tracking device required for this function 
*							is not connected
**/
DLLExport int __stdcall iV_GetEyeImage(struct ImageStruct *imageData);


/**
* @brief 	gets the device specific feature key 
*
* @return	RET_SUCCESS			- intended functionality has been fulfilled
* @return	ERR_NOT_CONNECTED	- no connection established
**/
DLLExport int __stdcall iV_GetFeatureKey(long long *featureKey);


/**
* @brief 	"iV_GetGeometryProfiles" gets all available profiles (only for RED and REDm devices)
*
* @param	maxSize -			the length of the string profileNames 
* @param	profileNames -		an empty string where are profile names will be put in 
*
* @return	RET_SUCCESS			- intended functionality has been fulfilled 
* @return	ERR_NOT_CONNECTED	- no connection established
* @return	ERR_WRONG_DEVICE	- eye tracking device required for this 
*								function is not connected  
**/
DLLExport int __stdcall iV_GetGeometryProfiles(int maxSize, char *profileNames);


/**
* @brief 	gets the system license due date. If the license is set to 00.00.0000 the license does not expire. 
*
* @return	RET_SUCCESS			- intended functionality has been fulfilled
* @return	ERR_NOT_CONNECTED	- no connection established
**/
DLLExport int __stdcall iV_GetLicenseDueDate(struct DateStruct *licenseDueDate);


/**
* @brief 	"iV_GetMonitorAttachedGeometry" gets the data of a selected profile (only REDm devices) without selecting them. 
*
* @param	profileName -				the name of the requested profile 
* @param	monitorAttachedGeometry -	the data of the requested profile 
*
* @return	RET_SUCCESS			- intended functionality has been fulfilled 
* @return	ERR_NOT_CONNECTED	- no connection established
* @return	ERR_WRONG_DEVICE	- eye tracking device required for this 
*								function is not connected  
**/
DLLExport int __stdcall iV_GetMonitorAttachedGeometry(char profileName[256], struct MonitorAttachedGeometryStruct *monitorAttachedGeometry);


/**
* @brief 	updates "rawDataSample" with current eyetracking data
*
* @param	rawDataSample	- see reference information for "SampleStruct"
*
* @return	RET_SUCCESS			- intended functionality has been fulfilled
* @return	RET_NO_VALID_DATA	- no new data available
* @return	ERR_NOT_CONNECTED	- no connection established
**/
DLLExport int __stdcall iV_GetSample(struct SampleStruct *rawDataSample);


/**
* @brief 	updates "rawDataSample" with current eyetracking data
*
* @param	rawDataSample	- see reference information for "SampleStruct32"
*
* @return	RET_SUCCESS			- intended functionality has been fulfilled
* @return	RET_NO_VALID_DATA	- no new data available
* @return	ERR_NOT_CONNECTED	- no connection established
**/
DLLExport int __stdcall iV_GetSample32(struct SampleStruct32 *rawDataSample);


/**
* @brief 	updates "imageData" with current scene video image  
*
* @param	imageData - see reference information for "ImageStruct"
*
* @return RET_SUCCESS		- intended functionality has been fulfilled
* @return RET_NO_VALID_DATA	- no new data available
* @return ERR_NOT_CONNECTED	- no connection established
* @return ERR_WRONG_DEVICE	- eye tracking device required for this function 
*							is not connected
**/
DLLExport int __stdcall iV_GetSceneVideo(struct ImageStruct *imageData);


/**
* @brief	gets back serial number information of the connected device 
*
* @param	serialNumber - 
*
* @return	RET_SUCCESS					- intended functionality has been fulfilled 
* @return	RET_NO_VALID_DATA			- no data available 
* @return	ERR_NOT_CONNECTED			- no connection established
* @return	ERR_WRONG_DEVICE			- eye tracking device required for this function 
*										is not connected
* @return	ERR_WRONG_IVIEWX_VERSION	- wrong version of iView X  
**/
DLLExport int __stdcall iV_GetSerialNumber(char serialNumber[64]);


/**
* @brief	updates "systemInfoData" with current system information 
*
* @param	SystemInfoStruct - see reference information for "SystemInfoStruct"
*
* @return	RET_SUCCESS			- intended functionality has been fulfilled
* @return	RET_NO_VALID_DATA	- no data available
**/
DLLExport int __stdcall iV_GetSystemInfo(struct SystemInfoStruct *systemInfoData);


/**
* @brief 	updates "imageData" with current tracking monitor image  
*
* @param	imageData - see reference information for "ImageStruct"
*
* @return RET_SUCCESS		- intended functionality has been fulfilled
* @return RET_NO_VALID_DATA	- no new data available
* @return ERR_NOT_CONNECTED	- no connection established
* @return ERR_WRONG_DEVICE	- eye tracking device required for this function 
*							is not connected
**/
DLLExport int __stdcall iV_GetTrackingMonitor(struct ImageStruct *imageData);


/**
* @brief	checks if connection to iView X is still established 
* 
* @return	RET_SUCCESS			- intended functionality has been fulfilled 
* @return	ERR_NOT_CONNECTED	- no connection established
**/ 
DLLExport int __stdcall iV_IsConnected();


/**
* @brief 	loads a saved calibration 
*			a calibration has to be previously saved by using "iV_SaveCalibration"
*			can only be used with iView X version of 2.3 or higher 
*
* @param	name - calibration name / identifier 
*
* @return	RET_SUCCESS					- intended functionality has been fulfilled
* @return	ERR_NOT_CONNECTED			- no connection established
* @return	ERR_WRONG_IVIEWX_VERSION	- wrong version of iView X  
* @return	ERR_WRONG_DEVICE			- eye tracking device required for this 
*										function is not connected
* @return	ERR_NO_RESPONSE_FROM_IVIEWX - no response from iView X; check 
*										calibration name / identifier
**/
DLLExport int __stdcall iV_LoadCalibration(char name[256]);


/**
* @brief 	writes "logMessage" to log file 
*
* @param	logMessage	- message that shall be written to the log file 
*
* @return RET_SUCCESS			- intended functionality has been fulfilled
* @return ERR_ACCESS_TO_FILE	- failed to access log file
**/
DLLExport int __stdcall iV_Log(char logMessage[256]);


/**
* @brief	pauses eye tracking and calculating gaze data. eye tracking can be 
*			continues with "iV_ContinueEyetracking" 
* 
* @return	RET_SUCCESS				- intended functionality has been fulfilled 
* @return	ERR_NOT_CONNECTED		- no connection established
**/ 
DLLExport int __stdcall iV_PauseEyetracking();


/**
* @brief 	pauses gaze data recording and scene video recording (if connected 
*			eyetracking device is "HED")
*			"iV_PauseRecording" does not return until gaze and scene video 
*			recording is paused
*
* @return	RET_SUCCESS				- intended functionality has been fulfilled
* @return	ERR_NOT_CONNECTED		- no connection established
* @return	ERR_WRONG_DEVICE		- eye tracking device required for this function 
*									is not connected
* @return	ERR_EMPTY_DATA_BUFFER	- recording buffer is empty 
* @return	ERR_FULL_DATA_BUFFER	- data buffer is full 
**/
DLLExport int __stdcall iV_PauseRecording();


/**
* @brief	disconnects and closes iViewX
*			can only be used with iView X version of 2.8.7 or higher 
* 
* @return	RET_SUCCESS				- intended functionality has been fulfilled 
* @return	ERR_DELETE_SOCKET		- failed to delete sockets 
* @return	ERR_WRONG_IVIEWX_VERSION - wrong version of iView X 
**/ 
DLLExport int __stdcall iV_Quit();


/**
* @brief	releases the port for sending out TTL trigger 
* 
* @return	RET_SUCCESS				- intended functionality has been fulfilled 
* @return	ERR_COULD_NOT_CLOSE_PORT - failed to close TTL port
**/ 
DLLExport int __stdcall iV_ReleaseAOIPort();


/**
* @brief	removes all AOIs with the given name 
* 
* @param	aoiName - name of the AOI which will be removed 
* 
* @return	RET_SUCCESS				- intended functionality has been fulfilled 
* @return	RET_NO_VALID_DATA		- no data available
* @return	ERR_AOI_ACCESS			- failed to access AOI data
**/ 
DLLExport int __stdcall iV_RemoveAOI(char aoiName[256]);


/**
* @brief	resets all calibration points to default positions 
* 
* @return	RET_SUCCESS				- intended functionality has been fulfilled 
* @return	ERR_NOT_CONNECTED		- no connection established
**/ 
DLLExport int __stdcall iV_ResetCalibrationPoints();


/**
* @brief 	stores a performed calibration 
*
* @param	name - calibration name / identifier 
*			can only be used with iView X version of 2.3 or higher 
*
* @return	RET_SUCCESS					- intended functionality has been fulfilled
* @return	ERR_NOT_CONNECTED			- no connection established
* @return	ERR_NOT_CALIBRATED			- system is not calibrated
* @return	ERR_WRONG_IVIEWX_VERSION	- wrong version of iView X  
* @return	ERR_WRONG_DEVICE			- eye tracking device required for this 
*										function is not connected
**/
DLLExport int __stdcall iV_SaveCalibration(char name[256]);


/**
* @brief 	writes data buffer and scene video buffer (if connected eyetracking 
*			device is "HED") to file "filename"
*			"iV_SaveData" will not return until the data has been saved
*
* @param	filename		- filename of data files being created 
*							(.idf: eyetracking data, .avi: scene video data) 
* @param	description	- optional experiment description 
* @param	user			- optional name of test person
* @param	overwrite		- 0: do not overwrite file “filename” if it already exists 
*							1: overwrite file “filename” if it already exists
*
* @return	RET_SUCCESS				- intended functionality has been fulfilled 
* @return	ERR_NOT_CONNECTED		- no connection established 
* @return	ERR_WRONG_PARAMETER		- parameter out of range
* @return	ERR_EMPTY_DATA_BUFFER	- recording buffer is empty 
* @return	ERR_RECORDING_DATA_BUFFER - recording is activated 
**/
DLLExport int __stdcall iV_SaveData(char filename[256], char description[64], char user[64], int overwrite);


/**
* @brief	sends a remote command to iView X. Please refer to the iView X
*			help file for further information about remote commands.
*			Important Note: This command is preliminary and will not be 
*			supported in the following versions 
* 
* @param	ETMessage - iView X remote command
*
* @return	RET_SUCCESS			- intended functionality has been fulfilled
* @return	ERR_NOT_CONNECTED	- no connection established
* @return	ERR_WRONG_PARAMETER	- parameter out of range
**/ 
DLLExport int __stdcall iV_SendCommand(char etMessage[256]);


/**
* @brief 	sends a text message to iView X. "etMessage" will be written 
*			to the data file. If "etMessage" ends on .jpg, .bmp, .png, or .avi 
*			BeGaze will separate the data buffer into according trials.
*
* @param	etMessage	- Any text message to separate trials (image name 
*			containing extensions) or any idf data marker 
*
* @return	RET_SUCCESS			- intended functionality has been fulfilled
* @return	ERR_NOT_CONNECTED	- no connection established
**/
DLLExport int __stdcall iV_SendImageMessage(char etMessage[256]);


/**
* @brief 	"iV_CalibrationCallback" function will be called if a calibration 
*			point has changed, the calibration has been finished or aborted. 
*			This callback allows drawing a customized calibration routine. 
*
* @param	pCalibrationCallbackFunction - pointer to CalibrationCallbackFunction
*
* @return	none
**/
DLLExport void __stdcall iV_SetCalibrationCallback(pDLLSetCalibrationPoint pCalibrationCallbackFunction); 


/**
* @brief 	a customized timeout how long "iV_Connect" tries to connect to iView X 
*
* @param	time - the time [sec] iV_Connect is waiting for iView X response 
*
* @return	none
**/
DLLExport void __stdcall iV_SetConnectionTimeout(int time);


/**
* @brief 	"iV_SetCameraFrameRate" sets the frame rate for REDm device 
*
* @param	fps - chosen frames per second [30..60]
*
* @return	RET_SUCCESS			- intended functionality has been fulfilled 
* @return	ERR_NOT_CONNECTED	- no connection established
* @return	ERR_WRONG_DEVICE	- eye tracking device required for this 
*								function is not connected  
* @return	ERR_WRONG_PARAMETER	- parameter out of range 
**/
DLLExport int __stdcall iV_SetFrameRate(int fps); 


/**
* @brief 	"iV_SetGeometryProfile" selects a defined profile (only for RED and REDm devices)
*
* @param	profileName - 
*
* @return	RET_SUCCESS			- intended functionality has been fulfilled 
* @return	ERR_NOT_CONNECTED	- no connection established
* @return	ERR_WRONG_DEVICE	- eye tracking device required for this 
*								function is not connected  
**/
DLLExport int __stdcall iV_SetGeometryProfile(char* profileName);


/**
* @brief 	"iV_SetPrecisionLevel" sets the precision level 
*
* @param	precision - [0: standard, 1: high]
*
* @return	RET_SUCCESS			- intended functionality has been fulfilled 
* @return	ERR_NOT_CONNECTED	- no connection established
* @return	ERR_WRONG_DEVICE	- eye tracking device required for this 
*								function is not connected  
* @return	ERR_WRONG_PARAMETER	- parameter out of range 
**/
DLLExport int __stdcall iV_SetPrecisionLevel(int precision);


/**
* @brief 	"iV_SetResolution" function defines a fixed resolution independent to the 
*			screen resolution of chosen display device defined in "iV_SetupCalibration" function. 
*
* @param	stimulusWidth		- horizontal resolution of stimulus screen [pixel] 
*			stimulusHeight		- vertical resolution of stimulus screen [pixel] 
*
* @return	RET_SUCCESS			- intended functionality has been fulfilled 
* @return	ERR_WRONG_PARAMETER	- parameter out of range 
**/
DLLExport int __stdcall iV_SetResolution(int stimulusWidth, int stimulusHeight);


/**
* @brief 	"iV_EventCallback" function will be called if an real-time 
*			detected fixation has started or ended. 
*
* @param	pEventCallbackFunction - pointer to EventCallbackFunction
**/
DLLExport void __stdcall iV_SetEventCallback(pDLLSetEvent pEventCallbackFunction); 


/**
* @brief 	defines detection parameter for online fixation detection algorithm
*
* @param	minDuration			- minimum fixation duration [ms]
*			maxDispersion		- maximum dispersion [pixel] for head tracking systems
*								or [deg] for non head tracking systems
*
* @return	RET_SUCCESS			- intended functionality has been fulfilled
* @return	ERR_WRONG_PARAMETER	- parameter out of range
**/
DLLExport int __stdcall iV_SetEventDetectionParameter(int minDuration, int maxDispersion);


/**
* @brief 	"iV_SetEyeImageCallback" function will be called if an new eye 
*			image is available. 
*
* @param	pEyeImageCallbackFunction - pointer to EyeImageCallbackFunction
**/
DLLExport void __stdcall iV_SetEyeImageCallback(pDLLSetEyeImage pEyeImageCallbackFunction);


/**
* @brief	validates the customer license (only for OEM devices) 
*
* @param	licenseKey			- provided license key
* 
* @return	RET_SUCCESS			- intended functionality has been fulfilled 
**/ 
DLLExport int __stdcall iV_SetLicense(char licenseKey[16]);


/**
* @brief 	defines the logging behavior of iView X SDK
*
* @param	logLevel	- see "Explanations for Defines" in this manual 
*						for further information
* @param	filename	- filename of log file
*
* @return RET_SUCCESS			- intended functionality has been fulfilled
* @return ERR_WRONG_PARAMETER	- parameter out of range
* @return ERR_ACCESS_TO_FILE	- failed to access log file
**/
DLLExport int __stdcall iV_SetLogger(int logLevel, char filename[256]);


/**
* @brief 	"iV_SampleCallback" function will be called if iView X has 
*			generated a new raw data sample. 
*			Important note: Dependent on the sample rate critical algorithms 
*			with high processor usage shouldn't be running within this callback 
*
* @param	pSampleCallbackFunction - pointer to SampleCallbackFunction
**/
DLLExport void __stdcall iV_SetSampleCallback(pDLLSetSample pSampleCallbackFunction); 


/**
* @brief 	"iV_SetSceneVideoCallback" function will be called if an new scene 
*			video image is available. 
*
* @param	pSceneVideoCallbackFunction - pointer to SceneVideoCallbackFunction
**/
DLLExport void __stdcall iV_SetSceneVideoCallback(pDLLSetSceneVideo pSceneVideoCallbackFunction);


/**
* @brief 	"iV_SetTrackingMonitorCallback" function will be called if an new 
*			RED tracking monitor image is available. 
*
* @param	pTrackingMonitorCallbackFunction - pointer to TrackingMonitorCallbackFunction
**/
DLLExport void __stdcall iV_SetTrackingMonitorCallback(pDLLSetTrackingMonitor pTrackingMonitorCallbackFunction);


/**
* @brief	sets iView X tracking parameters
*
* @param	ET_PARAM_EYE		- select specific eye 
* @param	ET_PARAM			- select parameter that shall be set 
* @param	value				- new value for selected parameter
*
* @return	RET_SUCCESS			- intended functionality has been fulfilled
* @return	ERR_NOT_CONNECTED	- no connection established
* @return	ERR_WRONG_PARAMETER	- parameter out of range
**/
DLLExport int __stdcall iV_SetTrackingParameter(int ET_PARAM_EYE, int ET_PARAM, int value);


/**
* @brief 	sets the calibration parameters
*
* @param	calibrationData	- see reference information for "CalibrationStruct"
*
* @return	RET_SUCCESS						- intended functionality has been fulfilled
* @return	ERR_WRONG_PARAMETER				- parameter out of range
* @return	ERR_WRONG_DEVICE				- eye tracking device required for this 
*											function is not connected  
* @return	ERR_WRONG_CALIBRATION_METHOD	- eye tracking device required for this 
*											calibration method is not connected
**/
DLLExport int __stdcall iV_SetupCalibration(struct CalibrationStruct *calibrationData);


/**
* @brief 	defines remotely the RED-m monitor attached geometry  
*			(see chapter 'RED Monitor Attached Mode' in the iView X SDK Manual) 
*
* @param	monitorAttachedGeometry	- see reference information for 
*									"MonitorAttachedGeometryStruct"
*
* @return RET_SUCCESS			- intended functionality has been fulfilled
* @return ERR_NOT_CONNECTED		- no connection established
* @return ERR_WRONG_PARAMETER	- parameter out of range
* @return ERR_WRONG_DEVICE		- eye tracking device required for this function 
*								is not connected
**/
DLLExport int __stdcall iV_SetupMonitorAttachedGeometry(struct MonitorAttachedGeometryStruct *monitorAttachedGeometry);


/**
* @brief 	Important Note: this function will not be supported from iView X API 3.1. 
*			Please use "iV_SetupMonitorAttachedGeometry" for definition of stand alone geometry 
*			defines remotely the RED-m monitor attached geometry 
*			(see chapter 'RED Monitor Attached Mode' in the iView X SDK Manual) 
*
* @param	attachedModeGeometry	- see reference information for 
*									"REDMonitorAttachedGeometryStruct"
*
* @return RET_SUCCESS			- intended functionality has been fulfilled
* @return ERR_NOT_CONNECTED		- no connection established
* @return ERR_WRONG_PARAMETER	- parameter out of range
* @return ERR_WRONG_DEVICE		- eye tracking device required for this function 
*								is not connected
**/
DLLExport int __stdcall iV_SetupREDMonitorAttachedGeometry(struct REDMonitorAttachedGeometryStruct *attachedModeGeometry);


/**
* @brief 	Important Note: this function will not be supported from iView X API 3.1. 
*			Please use "iV_SetupStandAloneMode" for definition of stand alone geometry 
*			defines remotely the RED stand alone mode 
*			(see chapter 'RED Stand Alone Mode' in the iView X SDK Manual) 
*
* @param	standAloneModeGeometry	- see reference information for 
*									"REDStandAloneModeStruct"
*
* @return RET_SUCCESS			- intended functionality has been fulfilled
* @return ERR_NOT_CONNECTED		- no connection established
* @return ERR_WRONG_PARAMETER	- parameter out of range
* @return ERR_WRONG_DEVICE		- eye tracking device required for this function 
*								is not connected
**/
DLLExport int __stdcall iV_SetupREDStandAloneMode(struct REDStandAloneModeStruct *standAloneModeGeometry);


/**
* @brief 	defines remotely the RED stand alone mode 
*			(see chapter 'RED Stand Alone Mode' in the iView X SDK Manual) 
*
* @param	standAloneModeGeometry	- see reference information for 
*										"StandAloneModeGeometryStruct"
*
* @return RET_SUCCESS			- intended functionality has been fulfilled
* @return ERR_NOT_CONNECTED		- no connection established
* @return ERR_WRONG_PARAMETER	- parameter out of range
* @return ERR_WRONG_DEVICE		- eye tracking device required for this function 
*								is not connected
**/
DLLExport int __stdcall iV_SetupStandAloneMode(struct StandAloneModeGeometryStruct *standAloneModeGeometry);


/**
* @brief 	the accuracy data will be visualized in a dialog window 
*
* @return	RET_SUCCESS			- intended functionality has been fulfilled
* @return	RET_NO_VALID_DATA	- no data available
* @return	ERR_NOT_CONNECTED	- no connection established
* @return	ERR_NOT_CALIBRATED	- system is not calibrated
* @return	ERR_NOT_VALIDATED	- system is not validated
**/
DLLExport int __stdcall iV_ShowAccuracyMonitor();

/**
* @brief 	visualizes eye image in separate window (available for all devices except RED)
*
* @return RET_SUCCESS			- intended functionality has been fulfilled
* @return ERR_NOT_CONNECTED		- no connection established
* @return ERR_WRONG_DEVICE		- eye tracking device required for this function 
*								is not connected
**/
DLLExport int __stdcall iV_ShowEyeImageMonitor();


/**
* @brief 	visualizes scene video in separate window (available for HED devices only) 
*
* @return RET_SUCCESS			- intended functionality has been fulfilled
* @return ERR_NOT_CONNECTED		- no connection established
* @return ERR_WRONG_DEVICE		- eye tracking device required for this function 
*								is not connected
**/
DLLExport int __stdcall iV_ShowSceneVideoMonitor();


/**
* @brief 	visualizes RED tracking monitor in separate window (available for RED devices only)
*
* @return RET_SUCCESS		- intended functionality has been fulfilled
* @return ERR_NOT_CONNECTED	- no connection established
* @return ERR_WRONG_DEVICE	- eye tracking device required for this function 
*							is not connected
**/
DLLExport int __stdcall iV_ShowTrackingMonitor(); 


/**
* @brief	starts and connects automatically to iView X (only if iView X is running on the same PC) 
* 
* @return	RET_SUCCESS				- intended functionality has been fulfilled 
* @return	ERR_COULD_NOT_CONNECT	- failed to establish connection 
* @return	ERR_IVIEWX_NOT_FOUND	- failed to start iViewX application 
**/ 
DLLExport int __stdcall iV_Start(enum ETApplication etApplication);


/**
* @brief	starts gaze data recording and scene video recording (if connected 
*			eye tracking device is "HED")
*			"iV_StartRecording" does not return until gaze and scene video 
*			recording is started
*
* @return	RET_SUCCESS					- intended functionality has been fulfilled
* @return	ERR_NOT_CONNECTED			- no connection established
* @return	ERR_WRONG_DEVICE			- eye tracking device required for this 
*										function is not connected
* @return	ERR_RECORDING_DATA_BUFFER	- recording is activated 
**/
DLLExport int __stdcall iV_StartRecording();


/**
* @brief 	stops gaze data recording and scene video recording (if connected 
*			eye tracking device is "HED")
*			"iV_StopRecording" does not return until gaze and scene video 
*			recording is stopped 
*
* @return	RET_SUCCESS				- intended functionality has been fulfilled
* @return	ERR_NOT_CONNECTED		- no connection established
* @return	ERR_WRONG_DEVICE		- eye tracking device required for this function 
*									is not connected
* @return	ERR_EMPTY_DATA_BUFFER	- recording buffer is empty 
**/
DLLExport int __stdcall iV_StopRecording();


/**
* @brief	sends a TTL value to defined port. Define a port with "iV_DefineAOIPort" 
* 
* @param	value - value which will be written to defined port 
* 
* @return	RET_SUCCESS				- intended functionality has been fulfilled 
* @return	ERR_WRONG_PARAMETER		- parameter out of range
**/ 
DLLExport int __stdcall iV_TestTTL(long value);


/**
* @brief 	starts a validation procedure.
*			If "CalibrationStruct::visualization" is set to "1" with "iV_SetupCalibration" 
*			"iV_Validatate" will not return until the validation has been finished or aborted. 
*
* @return RET_SUCCESS			- intended functionality has been fulfilled
* @return ERR_NOT_CONNECTED		- no connection established
* @return ERR_NOT_CALIBRATED	- system is not calibrated
* @return ERR_WRONG_DEVICE		- eye tracking device required for this function is not connected
**/
DLLExport int __stdcall iV_Validate();


/**
* @brief Test	- test routine 
*
* @return 42
**/
DLLExport int __stdcall Test(); 




