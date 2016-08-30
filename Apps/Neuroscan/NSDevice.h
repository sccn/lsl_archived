#ifndef _NS_DEVICE_INCLUDED
#define _NS_DEVICE_INCLUDED

// Define the amplifiers supported
#define NS_AMP_SN2			1	//Synamp II
#define NS_AMP_SNWL			2	//Synamp Wireless

#ifdef __cplusplus
extern "C" {
#endif

// DLL Export defines
#if defined(EXPORT_NSDEVICE)
#  define NSDllInterface __declspec(dllexport)  // On Win32 platform building the DLL
#elif defined(IMPORT_NSDEVICE)
#  define NSDllInterface __declspec(dllimport)  // On Win32 platform using the DLL 
#else
#  define NSDllInterface // Going Static 
#endif


// Define the interface functions
///////////////////////////////////////////////////////////////////////////////
// Getting started
//
// 1. Install the Access SDK and license dongle
// 2. Define IMPORT_NSDEVICE in your application's compiler pre-processor settings
// 3. Add NSDevice.lib to your project
// 4. Include this header file
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// long Connect(int nType,char*);
//
// This function connects to the amplifier driver (not the actuall amplifier)
//
// Parameters:
//	INPUT int	nType - Amplifier type (see above definitins of valid types)
//  INPUT char* pId - Identifier for the amplifier. In the case of synamp2/RT the
//				parameter is ignored. For network amplifiers the valid IP
//				address of the amplifier is required
//
// Return value:
//	long - Handle to amplifier, 0 if connection failed. 
//
// Notes:
//	This function will fail if the licensing fails
///////////////////////////////////////////////////////////////////////////////
NSDllInterface long __stdcall NSDConnect(int nType = NS_AMP_SN2,const char* pId = 0);


///////////////////////////////////////////////////////////////////////////////
// int Disconnect();
//
// This function disconnects from the amplifier driver and frees all resources
//
// Parameters: None
//
// Return value:
//	int - 1 succeeded.
//        0 failed
//
///////////////////////////////////////////////////////////////////////////////
NSDllInterface int __stdcall NSDDisconnect(void);


///////////////////////////////////////////////////////////////////////////////
// int GetCapabilities(char*);
//
// This function returns the amplifier's capabilities in XML format
//
// Parameters:
// IN/OUT - char* - pointer to a string buffer that will hold the XML containing
//			the amplifier capabilities
//
// Return value:
//	int - length of the capabilitities XML string
//        0 failed
//
// Notes:
// See the Access SDK "CMPDevice::GetCapabilities" method documention for XML 
// specifications 
//
///////////////////////////////////////////////////////////////////////////////
NSDllInterface int __stdcall NSDGetCapabilities(char* pCapabilities);


///////////////////////////////////////////////////////////////////////////////
// int GetBlocksPerSecond();
//
// This function returns the blocks per second retrieved by the driver
// Use this value to determine the block size of the data returned by the
// GetData function.(see GetData() for details.
//
// Parameters:None
//
// Return value:
//	int - Number of blocks per second.
//        0 failed
//
// Notes:
//
///////////////////////////////////////////////////////////////////////////////
NSDllInterface int __stdcall NSDGetBlocksPerSecond(void);


// Configuration functions not yet implemented
NSDllInterface int __stdcall NSDGetConfiguration(char* pConfiguration);
NSDllInterface int __stdcall NSDSetConfiguration(char* pConfiguration);


///////////////////////////////////////////////////////////////////////////////
// int CreateConfiguration(int,int);
//
// This function initializes the amplifier to the specified number of channels
// and sampling rate.
//
// Parameters:
// INPUT int - Number of channels, defaults to 70 (SN2 headbox)
// INPUT int - Acquisition rate, defaults to 1000
//
// Return value:
//	int - 1 Succeeded
//        0 failed
//
// Notes:
// The GetConfiguration() function retrieves valid values that can be sent either to this
// function or SetConfiguration().
// SynAmp2 valid number of channels is from 1 to 70 X number of headboxes
// SynAmp2 valid rates are 100,200,250,500,1000,1250,2000,2500,5000,10000,20000
//
///////////////////////////////////////////////////////////////////////////////
NSDllInterface int __stdcall NSDCreateConfiguration(int nChannels = 70, int nRate = 1000);


///////////////////////////////////////////////////////////////////////////////
// int GetData(float*);
//
// This function returns a block of data from the amplifier. This function should
// be polled by it's own thread. The first value an unsigned long currently not used
// the remaining data is de-multiplexed, i.e. the data is in blocks by channel
// pData[0] = index value currently not used in SN2
// pData[1] = first sample of channel 1
// pData[2] = second sample of channel 1
// ...
// pData[n] = first sample of channel 2 where n = number of samples in block
// ...
// To determine the block size:
// int nBlocks = GetBlocksPerSecond();
// int nPoints = nRate/nBlocks;
// int nPointsInBlock = nChannels * nPoints;
// int nBlockSize = nPointsInBlock * sizeof(float) + 4;// 4 bytes added for first element
//
// Parameters:
// IN/OUT float - Block of data pre-allocated by client
//
// Return value:
//	int - 1 Succeeded
//        0 No Data is available
//
// Notes:
//
///////////////////////////////////////////////////////////////////////////////
NSDllInterface int __stdcall NSDGetData(float* pData);

#ifdef __cplusplus
}
#endif

#endif
