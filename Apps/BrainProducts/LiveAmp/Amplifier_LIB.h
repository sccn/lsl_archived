/*------------------------------------------------------------------------------------------------*/
//  Project:	Amplifier Windows Library
/*------------------------------------------------------------------------------------------------*/
//  Purpose:	Harware access library for the Brain Products amplifiers 
//  Author:		Norbert Hauser
//  Date:		2016-03-17
//
//	Change History
//	2016-03-17	Version 1.xx	Initial implementation
//				
/*------------------------------------------------------------------------------------------------*/


#pragma once

#ifdef AMPLIFIER_EXPORTS
#define AMPLIFIER_API __declspec(dllexport)
#else
#define AMPLIFIER_API __declspec(dllimport)
#endif

#ifndef WINAPI
#include <windows.h>
#endif

#include <stdint.h>

/*------------------------------------------------------------------------------------------------*/
// Defines
/*------------------------------------------------------------------------------------------------*/

#define API_MAJOR				(1)			// Application interface major version
#define API_MINOR				(0)			// Application interface minor version

/*------------------------------------------------------------------------------------------------*/
// Error codes
/*------------------------------------------------------------------------------------------------*/

#define AMP_OK					(0)			// no error
#define AMP_ERR_FAIL			(-1)		// internal error, not specified
#define AMP_ERR_PARAM			(-2)		// function parameters or property size not in range
#define AMP_ERR_VERSION			(-3)		// version not supported
#define AMP_ERR_MEMORY			(-4)		// out of memory
#define AMP_ERR_BUSY			(-5)		// the requested device/file is already in use
#define AMP_ERR_NODEVICE		(-6)		// device not available 
#define AMP_ERR_NOSUPPORT		(-7)		// this function or property is not supported by the amplifier
#define AMP_ERR_EXCEPTION		(-8)		// exception was thrown 
#define AMP_ERR_FWVERSION		(-9)		// amplifier firmware version not supported
#define AMP_ERR_TIMEOUT  		(-10)		// a timeout occurred 
#define AMP_ERR_BUFFERSIZE 		(-11)		// transfer buffer too small, there is more data available 

#define BT_ERR_FAIL				(-100-1)	// BT connection error
#define BT_ERR_BT_SERVICE		(-100-2)	// BT radio is off
#define BT_ERR_MEMORY			(-100-3)	// BT out of memory
#define BT_ERR_NODEVICE			(-100-4)	// BT interface not found
#define BT_ERR_CONNECT			(-100-5)	// BT no conncection to amplifier
#define BT_ERR_DISCONNECTED		(-100-6)	// BT amplifier disconnected
#define BT_ERR_TIMEOUT			(-100-7)	// BT command time out
#define BT_ERR_ALREADYOPEN		(-100-8)	// BT amplifier is already open
#define BT_ERR_PARAMETER		(-100-9)	// BT invalid command parameters
#define BT_ERR_ATCOMMAND		(-100-10)	// BT AT command failed

#define DEVICE_ERR_BASE			(-200)		// error number base for device errors 
#define DEVICE_ERR_FAIL			(-200-1)	// internal error, not specified
#define DEVICE_ERR_PARAM		(-200-2)    // function parameters not in range
#define DEVICE_ERR_VERSION		(-200-3)    // function is not supported in this FW version
#define DEVICE_ERR_MEMORY		(-200-4)    // out of memory
#define DEVICE_ERR_BUSY 		(-200-5)    // cannot execute function because the device is busy
#define DEVICE_ERR_SDWRITE 		(-200-6)    // SD card write error
#define DEVICE_ERR_SDREAD 		(-200-7)    // SD card read error
#define DEVICE_ERR_NOSD 		(-200-8)    // SD card not inserted
#define DEVICE_ERR_SDFS 		(-200-9)    // SD card file system corrupted
#define DEVICE_ERR_SDACQ 		(-200-10)   // SD card acquisition not started before recording to SD
#define DEVICE_ERR_AUXBOX 		(-200-11)   // AuxBox not available
#define DEVICE_ERR_SDFULL		(-200-12)   // SD card full
#define DEVICE_ERR_WSPACE		(-200-13)   // Workspace settings not available

/*------------------------------------------------------------------------------------------------*/
// Enums
/*------------------------------------------------------------------------------------------------*/

// The group to which a property belongs
typedef enum PropertyGroup
{
	PG_DEVICE = 0,				// Device properties
	PG_MODULE = 1,				// Amplifier module properties
	PG_CHANNEL = 2				// Amplifier channel properties
} t_PropertyGroup;

// Recording modes
typedef enum RecordingMode
{
	RM_STOPPED = 0,				// Inactive
	RM_NORMAL = 1,				// Normal recording
	RM_IMPEDANCE = 2,			// Impedance measurement
	RM_TEST = 3					// Amplifier test signals
} t_RecordingMode;

// Channel types
typedef enum ChannelType
{
	CT_EEG = 0,					// EEG channel
	CT_BIP = 1,					// Bipolar channel
	CT_AUX = 2,					// Auxiliary channel
	CT_TRG = 3,					// Trigger channel
	CT_DIG = 4					// Digital channel
} t_ChannelType;

// Channel data types
typedef enum ChannelDataType
{
	DT_INT16 = 0,
	DT_UINT16 = 1,
	DT_INT32 = 2,
	DT_UINT32 = 3,
	DT_INT64 = 4,
	DT_UINT64 = 5,
	DT_FLOAT32 = 6,
	DT_FLOAT64 = 7
} t_ChannelDataType;

// Electrode types
typedef enum ElectrodeType
{
	EL_NONE = 0,				// No electrode connected (e.g. for AUX channels)
	EL_PASSIVE = 1,				// Passive electrode
	EL_ACTIVE = 2				// Active electrode
} t_ElectrodeType;


// Battery filling level
typedef enum BatteryState
{
	BS_UNKNOWN = 0,
	BS_EMPTY = 1,
	BS_MEDIUM = 2,
	BS_FULL = 3,
	BS_CHARGING = 4
} t_BatteryState;

// Connection state
typedef enum ConnectionState
{
	CS_DISCONNECTED = 0,		// Device is disconnected
	CS_RECONNECT = 1,			// Device is currently disconnected and the library tries to reconnect the device
	CS_CONNECTED = 2			// Device is connected
} t_ConnectionState;

// Signal quality
typedef enum SignalQuality
{
	SQ_NOINFO = 0,
	SQ_GOOD = 1,
	SQ_MEDIUM = 2,
	SQ_BAD = 3
} t_SignalQuality;

// Property range type
typedef enum PropertyRangeType
{
	RT_READONLY	= 0,			// This property is read only
	RT_MINMAX = 1,				// The returned property range array contains two values for minimum and maximum
	RT_DISCRETE = 2				// The returned property range array contains discrete values
}t_PropertyRangeType;

/*------------------------------------------------------------------------------------------------*/
// Properties
// 
// Each property label contains the property data type:
//   I32 = int32_t
//   F32 = float 32 bit
//   B32 = BOOL 32 bit
//   CHR = zero terminated ASCII string 
//   TVN = t_VersionNumber
/*------------------------------------------------------------------------------------------------*/

// override default packing for structures
#pragma pack(push, 1) 

// Version numbers
typedef struct VersionNumber
{
	int32_t Major;
	int32_t Minor;
	int32_t Build;
	int32_t Revision;
} t_VersionNumber;

// Channel properties
typedef enum ChannelPropertyID
{
	CPROP_I32_Type = 0,								// channel type (t_ChannelType)
	CPROP_I32_ChannelNumber = 1,					// physical channel number (zero based)
	CPROP_I32_ModuleNumber = 2,						// amplifier module to which the channel belongs (zero based)
	CPROP_CHR_Function = 3,							// function name for specialized AUX channels (like ACC X,Y,Z for accelerometer channels)
	CPROP_I32_Electrode = 4,						// electrode type (t_ElectrodeType)
	CPROP_I32_DataType = 5,							// channel data type in the received data stream (t_ChannelDataType)
	CPROP_F32_Resolution = 6,						// signal resolution in units/bit
	CPROP_CHR_Unit = 7,								// zero terminated ASCII string for the SI unit with optional prefix for multiple or sub-multiple of the unit
	CPROP_F32_Gain = 8,								// amplifier gain (0=setting not available)
	CPROP_F32_HighPass = 9,							// applied high-pass filter frequency in Hz (0=DC)
	CPROP_F32_LowPass = 10,							// applied low-pass filter frequency in Hz (0=off)
	CPROP_F32_NotchFilter = 11,						// notch filter frequency in Hz (0=off)
	CPROP_B32_ReferenceChannel = 12,				// 1 = can be used as reference channel or is used as reference channel
	CPROP_B32_ImpedanceMeasurement = 13,			// 1 = impedance measurement available for this channel
	CPROP_B32_RecordingEnabled = 14,				// 1 = channel is enabled for recording
} t_ChannelPropertyID;

// Amplifier module properties
typedef enum ModulePropertyID
{
	MPROP_CHR_Type = 1,								// the module type name (zero terminated ASCII string)
	MPROP_CHR_SerialNumber = 2,						// serial number in human readable format
	MPROP_TVN_HardwareRevision = 3,					// hardware revision
	MPROP_TVN_FirmwareVersion = 4,					// firmware version
} t_ModulePropertyID;

// Amplifier device properties
typedef enum DevicePropertyID
{
	DPROP_CHR_Family = 0,						// the amplifier family name (zero terminated ASCII string)
	DPROP_CHR_Type = 1,							// the amplifier type name (zero terminated ASCII string)
	DPROP_CHR_Interface = 2,					// hardware interface used by the amplifier ("ANY", "USB", "BT", "SIMULATIOM", ...)
	DPROP_CHR_Address = 3,					    // address identifier of the hardware interface (e.g. MAC address, USB or COM port)
	DPROP_CHR_SerialNumber = 4,					// serial number in human readable format
	DPROP_TVN_HardwareRevision = 5,				// hardware revision
	DPROP_TVN_FirmwareVersion = 6,				// firmware version
	DPROP_TVN_DriverVersion = 7,				// device driver version
	DPROP_I32_AvailableModules = 8,				// number of available modules
	DPROP_I32_AvailableChannels = 9,			// total number of available channels (for all modules)
	// device status
	DPROP_F32_BatteryVoltage = 100,				// current battery voltage in [V]
	DPROP_I32_BatteryLevel = 101,				// battery filling level (t_BatteryState)
	DPROP_I32_ConnectionState = 102,			// connection info (t_ConnectionState)
	DPROP_I32_SignalQuality = 103,				// signal quality (t_SignalQuality if available)
	DPROP_UI32_RecordingState = 104,			// device specific flags if recording to internal memory (if available)
	DPROP_UI32_ErrorFlags = 105,					// device specific error flags
	// recording parameters
	DPROP_I32_RecordingMode = 200,				// the recording mode (t_RecordingMode)
	DPROP_F32_BaseSampleRate = 201,				// base sampling frequency of the amplifier
	DPROP_F32_SubSampleDivisor = 202,			// sub sampling divisor
	DPROP_I32_GoodImpedanceLevel = 203,			// Good impedance level in Ohm. Impedances below this level will be shown in green, else yellow 
	DPROP_I32_BadImpedanceLevel = 204			// Bad impedance level in Ohm. Impedances above this level will be shown in red
} t_DevicePropertyID;


// restore default packing for structures 
#pragma pack(pop)


/*------------------------------------------------------------------------------------------------*/
// Interface functions
// All of the interface functions will either return a value >= 0 as a valid response 
// or a negative number in case of an error.
/*------------------------------------------------------------------------------------------------*/

#ifdef __cplusplus
extern "C" {  // only need to export C interface if
	// used by C++ source code
#endif

#define AMPAPI AMPLIFIER_API int WINAPI 

/// <summary>	Get the application interface version number. </summary>
/// <param name="pAPIVersion">	[out] The API version.
///			Revision = 0 
///			Build = 0
///			Minor = minor version 
///			Major = major version.</param>
/// <returns>Error code</returns>
AMPAPI GetAPIVersion(t_VersionNumber *pAPIVersion);

/// <summary>	Get the library version number. </summary>
/// <param name="pLibraryVersion">	[out] The library version. 
///			Revision = release day 
///			Build = release month 
///			Minor = release year 
///			Major = major version.</param>
/// <returns>Error code</returns>
AMPAPI GetLibraryVersion(t_VersionNumber *pLibraryVersion);

/// <summary>	Enumerate available devices. </summary>
/// <param name="HWI">				Select an hardware communiction interface or let the library
/// 								select one by using "ANY". If this parameter is set to "ANY", the
/// 								automatically selected interface is returned by this variable. 
/// 								Possible values are "ANY", "USB", "BT" and "SIM"</param>
/// <param name="HWISize">			Size of the HWI string buffer. </param>
/// <param name="DeviceAddress">	a pre-selected device adress. </param>
/// <param name="flags">			device depending flags. </param>
/// <returns>	The number of available devices. </returns>
AMPAPI ampEnumerateDevices(char *HWI, int32_t HWISize, const char *DeviceAddress, uint32_t flags);

/// <summary>	Gets a device address. </summary>
/// <param name="DeviceNr">			The zero based device number. </param>
/// <param name="DeviceAddress">	The device address buffer. </param>
/// <param name="BufferSize">   	Size of the buffer. </param>
/// <returns>	. </returns>
AMPAPI ampGetDeviceAddress(int32_t DeviceNr, char *DeviceAddress, int32_t BufferSize);

/// <summary>	Opens a device. </summary>
/// <param name="DeviceNr">	   	The zero based device number. </param>
/// <param name="DeviceHandle">	Returns the handle of the device. </param>
/// <returns>	. </returns>
AMPAPI ampOpenDevice(int32_t DeviceNr, HANDLE *DeviceHandle);


/// <summary>	Get a property value. </summary>
/// <param name="DeviceHandle"> 	Handle of the device. </param>
/// <param name="PropertyGroup">	Property group selection </param>
/// <param name="Index">			The index is not necessary  for the device property group and should be zero.
/// 								For the channel property group this is the zero based channel number. 
/// 								For the module property group this is the zero based module number.</param>
/// <param name="PropertyID">		Property identifier. </param>
/// <param name="PropertyValue">	Property value buffer. </param>
/// <param name="ValueByteSize">	Size of the value buffer in byte. </param>
AMPAPI ampGetProperty(HANDLE DeviceHandle, t_PropertyGroup PropertyGroup, uint32_t Index, int32_t PropertyID, void *PropertyValue, uint32_t ValueByteSize);

/// <summary>	Set a property value. </summary>
/// <param name="DeviceHandle"> 	Handle of the device. </param>
/// <param name="PropertyGroup">	Property group selection </param>
/// <param name="Index">			The index is not necessary  for the device property group and should be zero.
/// 								For the channel property group this is the zero based channel number. 
/// 								For the module property group this is the zero based module number.</param>
/// <param name="PropertyID">		Property identifier. </param>
/// <param name="PropertyValue">	Property value buffer. </param>
/// <param name="ValueByteSize">	Size of the value buffer in byte. </param>
AMPAPI ampSetProperty(HANDLE DeviceHandle, t_PropertyGroup PropertyGroup, uint32_t Index, int32_t PropertyID, void *PropertyValue, uint32_t ValueByteSize);


/// <summary>	Get the property range and the range type. </summary>
/// <param name="DeviceHandle"> 	Handle of the device. </param>
/// <param name="PropertyGroup">	Property group selection </param>
/// <param name="Index">			The index is not necessary  for the device property group and should be zero.
/// 								For the channel property group this is the zero based channel number. 
/// 								For the module property group this is the zero based module number.</param>
/// <param name="PropertyID">		Property identifier. </param>
/// <param name="RangeArray">		Property range array buffer. 
/// 								The array elements have the same datatype as the property itself.
/// 								String properties are returned as a zero terminated string with elements separated by a LF character.</param>
/// <param name="ArrayByteSize">	Size of the array buffer in byte. </param>
/// <param name="RangeType">		Type of the property range. </param>
AMPAPI ampGetPropertyRange(HANDLE DeviceHandle, t_PropertyGroup PropertyGroup, uint32_t Index, int32_t PropertyID, void *RangeArray, uint32_t *ArrayByteSize, t_PropertyRangeType *RangeType);


/// <summary>	Start the data acquisition. </summary>
/// <param name="DeviceHandle">	Handle of the device. </param>
/// <returns>	. </returns>
AMPAPI ampStartAcquisition(HANDLE DeviceHandle);

/// <summary>	Stop the data acquisition. </summary>
/// <param name="DeviceHandle">	Handle of the device. </param>
/// <returns>	. </returns>
AMPAPI ampStopAcquisition(HANDLE DeviceHandle);

/// <summary>	Closes a device. </summary>
/// <param name="DeviceHandle">	Handle of the device. </param>
/// <returns>	. </returns>
AMPAPI ampCloseDevice(HANDLE DeviceHandle);

/// <summary>	Sets a digital port. </summary>
/// <param name="DeviceHandle">	Handle of the device. </param>
/// <param name="PortNumber">  	The port number. </param>
/// <param name="value">	   	The value. </param>
/// <returns>	. </returns>
AMPAPI ampSetDigitalPort(HANDLE DeviceHandle, int32_t PortNumber, uint32_t value);


/// <summary>	Read acquired data from the device. 
/// 			The channel order in the buffer is
///				S1_SAMPLECOUNTER, S1_CH1 .. S1_CHn,
///				S2_SAMPLECOUNTER, S2_CH1 .. S2_CHn,
///				...
///				Sn_SAMPLECOUNTER, Sn_CH1 .. Sn_CHn
///				The sample size depends on the number of enabled channels and the channels data type.
///				The sample counter is a 64 bit unsigned integer. 
/// 			</summary>
/// <param name="DeviceHandle">			Handle of the device. </param>
/// <param name="Buffer">				Receive buffer. </param>
/// <param name="BufferSize">			Size of the receive buffer in bytes. </param>
/// <param name="RequestedSamples">		Number of requested samples (not yet supported). </param>
/// <returns>	Number of bytes written into the receive buffer. </returns>
AMPAPI ampGetData(HANDLE DeviceHandle, void *Buffer, int32_t BufferSize, int32_t RequestedSamples);

/// <summary>	Gets the impedance data of the selected channels. </summary>
/// 			The channel order in the buffer is
///				GND impedance, REF impedance,  CH1+, CH1-, CH2+, CH2-, .. CHn+, CHn-
///				All value types are float and in [Ohm].
///				CH- values are only valid for bipolar electrodes and -1 if not available 
/// <param name="DeviceHandle">	Handle of the device. </param>
/// <param name="Buffer">	   	Receive buffer. </param>
/// <param name="BufferSize">  	Size of the receive buffer in bytes. </param>
/// <returns>	Number of bytes written into the receive buffer.  </returns>
AMPAPI ampGetImpedanceData(HANDLE DeviceHandle, void *Buffer, int32_t BufferSize);


#ifdef __cplusplus
}
#endif
