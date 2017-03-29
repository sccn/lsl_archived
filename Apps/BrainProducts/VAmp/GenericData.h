/*----------------------------------------------------------------------------*/
//  Project:	FaSDK
/*----------------------------------------------------------------------------*/
//  Name:		GenericData.h
//  Purpose:	Store raw data in Brain Vision File Format (for Brain Vision Analyzer).
//  Author:		Phuong Nguyen
//  Date:		24-Nov-2006
//  Version:	1.00
//  Revision:	$LastChangedRevision: 
/*----------------------------------------------------------------------------*/
#pragma once

#include <vector>
#include <atlstr.h>

using namespace std;

class CGenericData
{
public:
	CGenericData();
		//; Constructor
	
	~CGenericData(void);
		//; Destructor.
	
	struct DeviceInfo
		//; Device and channel info.
	{
		double dSamplingInterval;		// Sampling interval in microseconds.
		vector<CStringW> vChannelNames;	// channel names
		vector<float> vResolutions;		// channel resolutions
		vector<ULONG> vFormats;			// Channel data format, Signed integer(1), Floating point(2)
		vector<ULONG> vTypes;			// channel types
	
		DeviceInfo(): dSamplingInterval(0)
		{
			vChannelNames.clear();
			vResolutions.clear();
			vFormats.clear();
			vTypes.clear();
		}
	};

public:
	void SetEEGFileName(char* szFileWithoutExtension);
		//; Set new EEG file name.

	void WriteHeaderInfo(DeviceInfo devInfo);
		//: Write header info to file (*.hdr).
		//+ Return 
		//+		none.
		//+ Parameter
		//.		devInfo - structure of device and channel info.

	void WriteData(float* pADBuffer, ULONG nPointsPerBlock, int nChannels, int nIgnoreChanNo);
		//: Write A/D raw data to file (*.eeg).
		//+ Return 
		//+		none.
		//+ Parameter
		//+		pADBuffer - points to buffer of nPointsPerBlock samples.
		//+		nPointsPerBlock - size of ADBuffer in samples
		//.		nChannels - number of channels.

private:
	char	m_szFile[256];	// file name for header, data without extension.
	FILE*	m_hDataFile;	// Handle of raw data
};
