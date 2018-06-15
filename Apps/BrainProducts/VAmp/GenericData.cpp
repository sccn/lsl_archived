/*----------------------------------------------------------------------------*/
//  Project:	FaSDK
/*----------------------------------------------------------------------------*/
//  Name:		GenericData.cpp
//  Purpose:	Store raw data in Brain Vision File Format (for Brain Vision Analyzer).
//  Author:		Phuong Nguyen
//  Date:		24-Nov-2006
//  Version:	1.00
//  Revision:	$LastChangedRevision: 
/*----------------------------------------------------------------------------*/
#include "stdafx.h"
#include "GenericData.h"
#include <windows.h>


CGenericData::CGenericData()
{
	
}

CGenericData::~CGenericData(void)
{
	if (m_hDataFile != NULL)
	{
		fclose(m_hDataFile);
		m_hDataFile = NULL;
	}
}

void CGenericData::SetEEGFileName(char* szFileWithoutExtension)
{
	strcpy(m_szFile, szFileWithoutExtension);
	m_hDataFile = NULL;
	char szDataFile[_MAX_PATH];
	sprintf(szDataFile, "%s.eeg", m_szFile);
	m_hDataFile = fopen(szDataFile, "wb");
}

void CGenericData::WriteHeaderInfo(DeviceInfo devInfo)
{
	char szHeaderFile[256], szDataFile[256];
	char szValue[128];

	sprintf(szHeaderFile, "%s.vhdr", m_szFile);
	sprintf(szDataFile, "%s.eeg", m_szFile);

	char drive[_MAX_DRIVE];
	char dir[_MAX_DIR];
	char fname[_MAX_FNAME];
	char ext[_MAX_EXT];
	_splitpath(szDataFile, drive, dir, fname, ext);

	
	FILE* hFile = fopen(szHeaderFile, "wt");
	if (hFile == NULL) return;
	fprintf(hFile, "Brain Vision Data Exchange Header File Version 1.0\n; Data created by the Vision Recorder\n");
	fclose(hFile);
	//
	// Common Infos section
	//

	char* szSection = "Common Infos";
	sprintf(szValue, "%s%s", fname, ext);
	WritePrivateProfileStringA(szSection, "Codepage", "UTF-8", szHeaderFile);
	WritePrivateProfileStringA(szSection, "DataFile", szValue, szHeaderFile);
	WritePrivateProfileStringA(szSection, "DataFormat", "BINARY", szHeaderFile);
	WritePrivateProfileStringA(szSection, "DataOrientation", "MULTIPLEXED", szHeaderFile);
	
	sprintf(szValue, "%d", devInfo.vChannelNames.size());
	WritePrivateProfileStringA(szSection, "NumberOfChannels", szValue, szHeaderFile);
	
	sprintf(szValue, "%.0f", devInfo.dSamplingInterval);
	WritePrivateProfileStringA(szSection, "SamplingInterval", szValue, szHeaderFile);
	//
	// Binary Infos Section
	//
	szSection = "Binary Infos";
	// IEEE_FLOAT_32 or INT_16
	WritePrivateProfileStringA(szSection, "BinaryFormat", "IEEE_FLOAT_32", szHeaderFile);
	//
	// Channel Infos Section
	//
	szSection = "Channel Infos";
	for (UINT n = 0; n < devInfo.vChannelNames.size(); n++)
	{
		char szKey[80];
		sprintf(szKey, "Ch%d", n+1);
		sprintf(szValue, "%d,,%.7f,µS", n+1, devInfo.vResolutions[n]);
		WritePrivateProfileStringA(szSection, szKey, szValue, szHeaderFile);
	}
}

void CGenericData::WriteData(float* pADBuffer, ULONG nPointsPerBlock, int nChannels, int nIgnoreChanNo)
{
	if (m_hDataFile == NULL)
	{
		return;
	}
	float* pData = pADBuffer;
	for (ULONG n = 0; n < nPointsPerBlock; n++)
	{
		for (int i = 0; i < nChannels; i++)
		{
			fwrite(pData + i, 1, sizeof(float), m_hDataFile);
		}
		pData += nChannels + nIgnoreChanNo;
	}
}
