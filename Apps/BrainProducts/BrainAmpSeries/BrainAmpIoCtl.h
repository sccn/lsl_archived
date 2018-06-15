// MODULE: BrainAmpIoCtl.h
//: written by: Henning Nordholz
//+       date: 15-Nov-99
//+ 
//+ Description:
//. 	Declarations of IOCTL codes and structs for the BrainAmp device driver.

#pragma once

// Send setup, Parameter [IN]: BA_SETUP struct.
#define IOCTL_BA_SETUP	\
			CTL_CODE(FILE_DEVICE_UNKNOWN, 0x801, METHOD_BUFFERED, FILE_WRITE_DATA)

// Start acquisition, Parameter [IN]: long nType, 0 = Impedance check, 1 = Data aquisition, 2 = calibration
#define IOCTL_BA_START	\
			CTL_CODE(FILE_DEVICE_UNKNOWN, 0x802, METHOD_BUFFERED, FILE_WRITE_DATA)

// Stop acquisition
#define IOCTL_BA_STOP	\
			CTL_CODE(FILE_DEVICE_UNKNOWN, 0x803, METHOD_BUFFERED, FILE_WRITE_DATA)

// Get buffer filling state, Parameter [OUT]: long nState, < 0 = Overflow, 0 - 100 = Filling state in percent.
#define IOCTL_BA_BUFFERFILLING_STATE	\
			CTL_CODE(FILE_DEVICE_UNKNOWN, 0x804, METHOD_BUFFERED, FILE_READ_DATA)

// Get battery voltage of one unit, Parameter [IN]: USHORT nUnit, Parameter [OUT]: long nVoltage in millivolts
#define IOCTL_BA_BATTERY_VOLTAGE	\
			CTL_CODE(FILE_DEVICE_UNKNOWN, 0x805, METHOD_BUFFERED, FILE_WRITE_DATA | FILE_READ_DATA)

// Set impedance check sine wave frequency, Parameter [IN]: LONG nFrequency in Hertz
#define IOCTL_BA_IMPEDANCE_FREQUENCY	\
			CTL_CODE(FILE_DEVICE_UNKNOWN, 0x806, METHOD_BUFFERED, FILE_WRITE_DATA)

// Set impedance test mode, i.e. even in recording mode, the amplifier is still in impedance check mode
// Parameter [IN]: long bTestMode, != 0 testmode, 0 normal mode
#define IOCTL_BA_IMPEDANCE_TESTMODE		\
			CTL_CODE(FILE_DEVICE_UNKNOWN, 0x807, METHOD_BUFFERED, FILE_WRITE_DATA)

// Set impedance group and range
// Parameter [IN]: long nGroupRange, 0 = 100kOhm Data (IMP1 | CAL), 1 = 10kOhm Data (IMP1), 
//									 2 = 100 kOhm Reference (IMP2 | CAL), 3 = 10 kOhm Reference (IMP2)
//									 4 = Ground (IMP2)
#define IOCTL_BA_IMPEDANCE_GROUPRANGE		\
			CTL_CODE(FILE_DEVICE_UNKNOWN, 0x808, METHOD_BUFFERED, FILE_WRITE_DATA)

// Get error state, Parameter [OUT]: long nState, 
//										Highword (amplifier):	Bit 0 - 3: amplifier number(s)
//										Loword:  (error code):	0 = no error, 1 = loss lock, 2 = low power, 
//																3 = can't establish communication at start.
//																4 = synchronisation error
#define IOCTL_BA_ERROR_STATE	\
			CTL_CODE(FILE_DEVICE_UNKNOWN, 0x809, METHOD_BUFFERED, FILE_READ_DATA)

// Disable DC offset correction for testing purposes.
// Parameter [IN]: long bTestMode, != 0 DC offset correction off, 0 normal mode
#define IOCTL_BA_DCOFFSET_TESTMODE		\
			CTL_CODE(FILE_DEVICE_UNKNOWN, 0x80A, METHOD_BUFFERED, FILE_WRITE_DATA)

// Send calibration settings, Parameter [IN]: BA_CALIBRATION_SETTINGS struct.
#define IOCTL_BA_CALIBRATION_SETTINGS	\
			CTL_CODE(FILE_DEVICE_UNKNOWN, 0x80B, METHOD_BUFFERED, FILE_WRITE_DATA)

// Set pullup/pulldown resistors for the digital input (default is pulldown).
// This can not be done for each bit, but for 2 groups.
// Parameter [IN]: USHORT bPullup, (low byte for bit 0 - 7, high byte for bit 8 - 15) 
//				   != 0 DC pullup, 0 pulldown
#define IOCTL_BA_DIGITALINPUT_PULL_UP	\
			CTL_CODE(FILE_DEVICE_UNKNOWN, 0x80C, METHOD_BUFFERED, FILE_WRITE_DATA)

// Get digital input value, Parameter [OUT]: USHORT nValue
#define IOCTL_BA_DIGITALINPUT_VALUE	\
			CTL_CODE(FILE_DEVICE_UNKNOWN, 0x80D, METHOD_BUFFERED, FILE_READ_DATA)

// Get driver version, Parameter [OUT]: ULONG nValue
// The version is coded as Major.Minor.DLL.
// The "DLL" part is used for intermediate versions and contains 4 digits.
// The minor part contains 2 digits.
// The number 1010041 for example means version 1.01.0041.
// If the highest bit (bit 31) is set, it means "test version".
#define IOCTL_BA_DRIVERVERSION	\
			CTL_CODE(FILE_DEVICE_UNKNOWN, 0x80E, METHOD_BUFFERED, FILE_READ_DATA)

// Send Commands during recording
// Parameter [IN]: USHORT nCommand (1 = DC Correction), USHORT nChannel (-1 = all channels)
#define IOCTL_BA_COMMAND \
			CTL_CODE(FILE_DEVICE_UNKNOWN, 0x80F, METHOD_BUFFERED, FILE_WRITE_DATA)


// Set Control register directly (WRCON)
// Parameter [IN]: USHORT nAddress, ulong[4] channel mask
#define IOCTL_BA_CONTROLREGISTER \
			CTL_CODE(FILE_DEVICE_UNKNOWN, 0x810, METHOD_BUFFERED, FILE_WRITE_DATA)

// Set DC offset value for potentiometer A (coarse)
// Parameter [IN]: USHORT nChannel, UCHAR nValue
#define IOCTL_BA_DCOFFSET_COARSE \
			CTL_CODE(FILE_DEVICE_UNKNOWN, 0x811, METHOD_BUFFERED, FILE_WRITE_DATA)


// Do DC Offset correction for potentiometer A (coarse) 
#define IOCTL_BA_DCOFFSET_CORRECTION_COARSE \
			CTL_CODE(FILE_DEVICE_UNKNOWN, 0x812, METHOD_NEITHER, FILE_WRITE_DATA)


// Set DC offset value for potentiometer B (fine)
// Parameter [IN]: USHORT nChannel, UCHAR nValue
#define IOCTL_BA_DCOFFSET_FINE \
			CTL_CODE(FILE_DEVICE_UNKNOWN, 0x813, METHOD_BUFFERED, FILE_WRITE_DATA)


// Do DC Offset correction for potentiometer B (fine)
#define IOCTL_BA_DCOFFSET_CORRECTION_FINE \
			CTL_CODE(FILE_DEVICE_UNKNOWN, 0x814, METHOD_NEITHER, FILE_WRITE_DATA)

// Set Cal, Imp1, and Imp2 lines (testing only!)
// Parameter [IN]: BYTE Cal, BYTE, Imp1, Byte Imp2
#define IOCTL_BA_SET_CAL_IMP_LINES \
			CTL_CODE(FILE_DEVICE_UNKNOWN, 0x815, METHOD_BUFFERED, FILE_WRITE_DATA)

// Get amplifier type of all 4 units, 
// Parameter [IN]: USHORT nUnit, Parameter [OUT]: USHORT Type[4]: 0 no Amp, 1 BrainAmp, 2 MR, 3 DC
#define IOCTL_BA_AMPLIFIER_TYPE	\
			CTL_CODE(FILE_DEVICE_UNKNOWN, 0x816, METHOD_BUFFERED, FILE_WRITE_DATA | FILE_READ_DATA)

// Do DC Offset correction, no parameters
#define IOCTL_BA_DCOFFSET_CORRECTION \
			CTL_CODE(FILE_DEVICE_UNKNOWN, 0x817, METHOD_NEITHER, FILE_WRITE_DATA)

// Retrieve serial number of device. Parameter [OUT]: ULONG 
#define IOCTL_BA_GET_SERIALNUMBER	\
			CTL_CODE(FILE_DEVICE_UNKNOWN, 0x818, METHOD_BUFFERED, FILE_READ_DATA)

// Retrieve number of supported amplifiers for the device. Parameter [OUT]: USHORT 
#define IOCTL_BA_GET_SUPPORTED_AMPLIFIERS	\
			CTL_CODE(FILE_DEVICE_UNKNOWN, 0x819, METHOD_BUFFERED, FILE_READ_DATA)

// Prestart, used only if more than device is involved.
#define IOCTL_BA_PRESTART \
			CTL_CODE(FILE_DEVICE_UNKNOWN, 0x81a, METHOD_BUFFERED, FILE_WRITE_DATA)

// Poststart, used only if more than device is involved.
#define IOCTL_BA_POSTSTART \
			CTL_CODE(FILE_DEVICE_UNKNOWN, 0x81b, METHOD_BUFFERED, FILE_WRITE_DATA)

// Disable busmastering
#define IOCTL_BA_DISABLE_BUSMASTERING \
			CTL_CODE(FILE_DEVICE_UNKNOWN, 0x81c, METHOD_BUFFERED, FILE_WRITE_DATA)

// DBID 1440
// Get missing buffer size in ms, Parameter [OUT]: long nSize, 
//  0 = no overflow detected
//  -1 = Device/FW Overflow, 
//  1 - n = missing buffer size in ms.
#define IOCTL_BA_BUFFERMISSING_MS	\
			CTL_CODE(FILE_DEVICE_UNKNOWN, 0x81d, METHOD_BUFFERED, FILE_READ_DATA)


#pragma pack(1)
struct BA_SETUP
//; Brain Amp Setup.
{
	long	nChannels;			// Number of channels
	CHAR	nChannelList[256];	// Channel lookup table, -1 to -8 means PolyBox channels
	long	nPoints;			// Number of points per block
	USHORT 	nHoldValue;			// Hold value for digital input, low byte = user response, 
								// high byte = stimulus input
	// Version 1.01.0003 and higher
	// The following tables are based on logical channel positions.
	UCHAR	n250Hertz[256];		// Low pass 250 Hz (0 = 1000Hz)
	UCHAR	nResolution[256];	// 0 = 100 nV, 1 = 500 nV, 2 = 10 µV, 3 = 152.6 µV
	UCHAR	nDCCoupling[256];	// DC coupling (0 = AC)
	UCHAR	nLowImpedance;		// Low impedance i.e. 10 MOhm, (0 = > 100MOhm)
};


struct BA_CALIBRATION_SETTINGS
//; Brain Calibration settings (default: square waves, 5 Hz).
{
	USHORT	nWaveForm;			// 0 = ramp, 1 = triangle, 2 = square, 3 = sine wave
	ULONG	nFrequency;			// Frequency in millihertz.
};

#pragma pack()
