/*********************************************************

This is the definition of the LiveAmp class. This class
makes use of the API defined in Amplifier_LIB.h in an
attempt to simplify its use in C++ programs.

Not all features are implemented.

Copyright (c) 2018 Brain Products

Released under the MIT license (see LICENSE.txt)

**********************************************************/
#pragma once

#include "LiveAmp.h"

// TODO: overload the constructor so that 
// live amp objects can be initialized by serial number etc.
//
LiveAmp::LiveAmp(std::string sSerialNumberIn, float fSamplingRateIn, bool bUseSim, int nRecordingModeIn) {
//LiveAmp::LiveAmp(){	
	int res;
	char HWI[20];
	
	m_bIsClosed = false;
	m_bHasSTE = false;
	m_bIs64 = false;
	if(bUseSim)
		strcpy_s(HWI, "SIM");
	else
		strcpy_s(HWI, "ANY"); // todo: parameterize this

	res = ampEnumerateDevices(HWI, sizeof(HWI), "LiveAmp", 0);

	if (res <= 0)			
		throw std::runtime_error("No LiveAmp connected or device wasn't properly closed. Try restarting this application.");
	else {
		for(int i=0;i<res;i++){	
			int result;

			HANDLE hndl = NULL;
			
			result = ampOpenDevice(i, &hndl);
			// set the handle
			
			if(result == AMP_OK) {
			
				h = hndl;
				char sVar[20]; 
				result = ampGetProperty(hndl, PG_DEVICE, i, DPROP_CHR_SerialNumber, sVar, sizeof(sVar));

				// got a hit!
				if (!(strcmp(sVar, sSerialNumberIn.c_str()))) {

					// set the sampling rate on the device
					result = ampSetProperty(hndl, PG_DEVICE, 0, DPROP_F32_BaseSampleRate, &fSamplingRateIn, sizeof(fSamplingRateIn));
					if (result != AMP_OK) {
						throw std::runtime_error(("Error setting sampling rate, error code:  "+ std::to_string(result)));// +boost::lexical_cast<std::string>(result)).c_str());
						return;
					}
					m_fSamplingRate = fSamplingRateIn;

					// set the device mode to recording
					result = ampSetProperty(hndl, PG_DEVICE, 0, DPROP_I32_RecordingMode, &nRecordingModeIn, sizeof(nRecordingModeIn));
					if (result != AMP_OK) {
						throw std::runtime_error(("Error setting acquisition mode, error code:  "+ std::to_string(result)));// + boost::lexical_cast<std::string>(result)).c_str());
						return;
					}
					m_nRecordingMode = nRecordingModeIn;
					m_sSerialNumber = std::string(sVar);

					// get the available channels
					result = ampGetProperty(h, PG_DEVICE, 0, DPROP_I32_AvailableChannels, &m_nAvailableChannels, sizeof(m_nAvailableChannels));
					if (result != AMP_OK)
						throw std::runtime_error(("Error getting available channel count, error code:  "+ std::to_string(result)));// +boost::lexical_cast<std::string>(result)).c_str());
					// is it a LA 64?
					else (m_nAvailableChannels >= 64) ? m_bIs64 = true : m_bIs64 = false;
						
					// set the usable channels               // this property name is misspelled in the API---we use the correct spelling locally
					result = ampGetProperty(h, PG_MODULE, 0, MPROP_I32_UseableChannels, &m_nUsableChannels, sizeof(m_nUsableChannels));
					if (result != AMP_OK)
						throw std::runtime_error(("Error getting usable channel count, error code:  "+ std::to_string(result)));// +boost::lexical_cast<std::string>(result)).c_str());

					// detect whether or not STE is connected
					char modName[100];
					result = ampGetProperty(h, PG_DEVICE, 0, DPROP_I32_AvailableModules, &m_nAvailableModules, sizeof(m_nAvailableModules));
					for (int n = 0; n < m_nAvailableModules; n++)
					{
						result = ampGetProperty(h, PG_MODULE, n, MPROP_CHR_Type, &modName, sizeof(modName));
						if (!strcmp(modName, "STE"))
						{
							m_bHasSTE = true;
							m_nSTEIdx = n;
						}
					}
					break;
				}
			}
		}
	}
}

LiveAmp::~LiveAmp(void){
	int res;
	res = ampCloseDevice(h);
}

void LiveAmp::enumerate(LIVEAMP_DEVICEENUMINFO &ladDevInfo, bool bUseSim){

	int res;
	char HWI[20];
	bool is64 = false;
	t_deviceInfo deviceInfo;

	if(!ladDevInfo.empty()){
		throw std::runtime_error("Input Device Data vector isn't empty");
		return;
	}


	if(bUseSim)
		strcpy_s(HWI, "SIM");
	else
		strcpy_s(HWI, "USB"); 

	res = ampEnumerateDevices(HWI, sizeof(HWI), "LiveAmp", 0);

	if (res <= 0)			
		throw std::runtime_error("No LiveAmp connected");
	else {
		for(int i=0;i<res;i++){	
			int result;
			HANDLE hndl = NULL;
			
			result = ampOpenDevice(i, &hndl);
			if(result != AMP_OK) {
				std::string msg = "Cannot open device: " + std::to_string(result);
				//msg.append (boost::lexical_cast<std::string>(i).c_str());
				msg.append("  error= " );
				//msg.append(boost::lexical_cast<std::string>(res).c_str()); // TODO: error enumeration from liveamp driver
				throw std::runtime_error(msg);
			}		
			
			char sVar[20];
			result = ampGetProperty(hndl, PG_DEVICE, i, DPROP_CHR_SerialNumber, sVar, sizeof(sVar)); 
			if(result != AMP_OK) {
				std::string msg = "Cannot get device serial number: " + std::to_string(result);
				//msg.append (boost::lexical_cast<std::string>(i).c_str());
				msg.append("  error= " );
				//msg.append(boost::lexical_cast<std::string>(res).c_str()); // TODO: error enumeration from liveamp driver
				throw std::runtime_error(msg);
			}	

			else {

				int32_t iVar;
				result = ampGetProperty(hndl, PG_DEVICE, i, DPROP_I32_AvailableChannels, &iVar, sizeof(iVar));
				if (result != AMP_OK) {
					std::string msg = "Cannot get device totla channel count: " + std::to_string(result);
					msg.append("  error= " + std::to_string(result));
					throw std::runtime_error(msg);
				}
				else
				{
					// this is the quick and dirty way to do this
					// if the hardware is incorrect, all kinds of problems might occur
					if (iVar >= 64) // it's a liveamp 64
					{
						deviceInfo.sSerialNumber = std::string(sVar);
						deviceInfo.nChannelCount = 64;
						deviceInfo.bIs64 = true;
						//ladAmpData.push_back(std::make_pair(std::string(sVar), 64));
					}
					else
					{
						result = ampGetProperty(hndl, PG_MODULE, i, MPROP_I32_UseableChannels, &iVar, sizeof(iVar));
						if (result != AMP_OK) {
							std::string msg = "Cannot get module useable channel count: " + std::to_string(result);
							msg.append("  error= " + std::to_string(result));
							throw std::runtime_error(msg);
						}
						else
						{
							deviceInfo.sSerialNumber = std::string(sVar);
							deviceInfo.nChannelCount = iVar;
							//ladAmpData.push_back(std::make_pair(std::string(sVar), iVar));
						}
					}
				}
			}
			ladDevInfo.push_back(deviceInfo);
			// immediately close the device???
			result = ampCloseDevice(hndl);
			if(result != AMP_OK) {
				std::string msg = "Cannot close device: " + std::to_string(result);
				msg.append("  error= " + std::to_string(result)); 
				throw std::runtime_error(msg);
			}
		}
	}
}


void LiveAmp::close(void){

	int result = ampCloseDevice(h);
	if(result != AMP_OK) {
		std::string msg = "Cannot close device: ";
		msg.append (m_sSerialNumber.c_str());
		msg.append("  error= " + std::to_string(result));
		throw std::runtime_error(msg);
	}
}

void LiveAmp::enableChannels(const std::vector<int>& vnEegIndicesIn, 
	const std::vector<int>& vnBipolarIndicesIn, 
	const std::vector<int>& vnAuxIndicesIn, 
	bool bAccEnable) {
	
	int res;
	int type;
	int i;	
	int enable, wasEnabled;
	int bipType = CT_BIP;
	char cValue[20]; // for determining if the aux channel is an accelerometer or not
	char cUnit[256];

	if(!m_vnEegIndices.empty())m_vnEegIndices.clear();
	if(!m_vnBipolarIndices.empty())m_vnBipolarIndices.clear();
	if(!m_vnAuxIndices.empty())m_vnAuxIndices.clear();
	if(!m_vnAccIndices.empty())m_vnAccIndices.clear();
	if(!m_vnTrigIndices.empty())m_vnTrigIndices.clear();
	m_nEnabledChannelCnt = 0;	
	int passCnt = 0;
	
	// make sure the liveamp has valid channels available
	if(m_nAvailableChannels == -1) {
		throw std::runtime_error((std::string("Invalid number of available channels on device ") + m_sSerialNumber).c_str());
		return;
	}
	// first, disable all the channels
	enable = 0;
	for(i=0;i<m_nAvailableChannels;i++)
		res = ampSetProperty(h, PG_CHANNEL, i, CPROP_B32_RecordingEnabled, &enable, sizeof(enable));

	// go through the available channels and enable them if they are chosen
	for(i=0;i<m_nAvailableChannels;i++) {

		enable = 0;
		res = ampGetProperty(h, PG_CHANNEL, i, CPROP_I32_Type, &type, sizeof(type));
		if(res != AMP_OK)					
			throw std::runtime_error(("Error getting property for channel type: error code:  " + std::to_string(res)));
		res = ampGetProperty(h, PG_CHANNEL, i, CPROP_CHR_Unit, cUnit, sizeof(cUnit));
		if (type == CT_EEG || type == CT_BIP) {
			// go through the requested eeg channel vector and enable on match
			passCnt = 0;
			for(std::vector<int>::const_iterator it= vnEegIndicesIn.begin(); it!= vnEegIndicesIn.end();++it) {
				if(*it==i){
					enable = 1;
					m_vnEegIndices.push_back(m_nEnabledChannelCnt);
					++m_nEnabledChannelCnt;
					passCnt++;
					// I'm not sure if this check is necessary, but it's in the old code, so I am assuming it is
					res = ampGetProperty(h, PG_CHANNEL, i, CPROP_B32_RecordingEnabled, &wasEnabled, sizeof(wasEnabled));
					if (res != AMP_OK)								
						throw std::runtime_error(("Error GetProperty enable for EEG channels, error: " + std::to_string(res)));
					
					if(wasEnabled!=enable) {		
						res = ampSetProperty(h, PG_CHANNEL, i, CPROP_B32_RecordingEnabled, &enable, sizeof(enable));
						if (res != AMP_OK)				
							throw std::runtime_error(("Error SetProperty enable for EEG channels, error: " + std::to_string(res)));
					}
				}
			}

			for(std::vector<int>::const_iterator it=vnBipolarIndicesIn.begin(); it!=vnBipolarIndicesIn.end();++it) {
				if(*it==i){
					enable = 1;
					m_vnBipolarIndices.push_back(m_nEnabledChannelCnt);
					++m_nEnabledChannelCnt;
					passCnt++;
					// I'm not sure if this check is necessary, but it's in the old code, so I am assuming it is
					res = ampGetProperty(h, PG_CHANNEL, i, CPROP_B32_RecordingEnabled, &wasEnabled, sizeof(wasEnabled));
					if (res != AMP_OK)								
						throw std::runtime_error(("Error GetProperty enable for EEG channels, error: " + std::to_string(res)));

					if(wasEnabled!=enable) {		
						res = ampSetProperty(h, PG_CHANNEL, i, CPROP_B32_RecordingEnabled, &enable, sizeof(enable));
						if (res != AMP_OK)				
							throw std::runtime_error(("Error SetProperty enable for EEG channels, error: " + std::to_string(res)));					
					}
								
					res = ampSetProperty(h, PG_CHANNEL, i, CPROP_I32_Type, &bipType, sizeof(bipType));
					if (res != AMP_OK)								
						throw std::runtime_error(("Error SetProperty enable BIPOLAR EEG channels, error: " + std::to_string(res)));
					
				}
			}
		}

		// if it is an aux channel see if it can/should be enabled and if it is an acc channel
		if(type == CT_AUX){		
			res = ampGetProperty(h, PG_CHANNEL, i, CPROP_CHR_Function, &cValue, sizeof(cValue));
			
			if (res != AMP_OK)			
				throw std::runtime_error(("Error GetProperty CPROP_CHR_Function error: " + std::to_string(res)));
			
			// check that this aux channel is an acc channel
			if(cValue[0] == 'X' || cValue[0] == 'Y' ||cValue[0] == 'Z' || cValue[0] == 'x' ||cValue[0] == 'y' ||cValue[0] == 'z') {
				

				enable = bAccEnable;
				if(enable == 1){
					m_vnAccIndices.push_back(m_nEnabledChannelCnt);
					++m_nEnabledChannelCnt;
				}
				res = ampGetProperty(h, PG_CHANNEL, i, CPROP_B32_RecordingEnabled, &wasEnabled, sizeof(wasEnabled));
				if (res != AMP_OK)	
					throw std::runtime_error(("Error SetProperty enable for ACC channels, error: " + std::to_string(res)));
					
				// if requested, enable it
				if(wasEnabled!=enable){
					res = ampSetProperty(h, PG_CHANNEL, i, CPROP_B32_RecordingEnabled, &enable, sizeof(enable));
					if (res != AMP_OK)				
						throw std::runtime_error(("Error SetProperty enable for ACC channels, error: " + std::to_string(res)));
				}
				
			}

			else { // this is a non-acc aux channel
				
				for (std::vector<int>::const_iterator it = vnAuxIndicesIn.begin(); it != vnAuxIndicesIn.end(); ++it) {
					if (*it == i) {

						enable = 1;
						if (enable == 1) {
							m_vnAccIndices.push_back(m_nEnabledChannelCnt);
							++m_nEnabledChannelCnt;
						}
						res = ampGetProperty(h, PG_CHANNEL, i, CPROP_B32_RecordingEnabled, &wasEnabled, sizeof(wasEnabled));
						if (res != AMP_OK)
							throw std::runtime_error(("Error SetProperty enable for AUX channels, error: " + std::to_string(res)));

						// if requested, enable it
						if (wasEnabled != enable) {
							res = ampSetProperty(h, PG_CHANNEL, i, CPROP_B32_RecordingEnabled, &enable, sizeof(enable));
							if (res != AMP_OK)
								throw std::runtime_error(("Error SetProperty enable for AUX channels, error: " + std::to_string(res)));
						}
					}
				}			
			}			
		}
		
		// the triggers are always enabled, but we need to keep track of them for channel labelling purposes
		if(type == CT_TRG) {
			m_vnTrigIndices.push_back(m_nEnabledChannelCnt);	
			++m_nEnabledChannelCnt;
		}
		
	}

	// get the sample size in bytes and make an array of sample types
	int dataType;
	float resolution;
	int channelType;
	int cnt = 0;
	m_nSampleSize=0;
	int enabled;
	char channelUnit[20];
	for(int i=0;i<m_nAvailableChannels;i++) {
		
			
		res = ampGetProperty(h, PG_CHANNEL, i, CPROP_B32_RecordingEnabled, &enabled, sizeof(enabled));
		
		if (enabled) {
			res = ampGetProperty(h, PG_CHANNEL, i, CPROP_I32_DataType, &dataType, sizeof(dataType));
			m_ciChannelInfoArray[cnt].nDataType = dataType;
			res = ampGetProperty(h, PG_CHANNEL, i, CPROP_F32_Resolution, &resolution, sizeof(resolution));
			m_ciChannelInfoArray[cnt].fResolution = resolution;
			res = ampGetProperty(h, PG_CHANNEL, i, CPROP_I32_Type, &channelType, sizeof(channelType));
			m_ciChannelInfoArray[cnt].nChannelType = channelType;
			res = ampGetProperty(h, PG_CHANNEL, i, CPROP_CHR_Unit, channelUnit, sizeof(channelUnit));
			m_ciChannelInfoArray[cnt].sChannelUnit = std::string(channelUnit);
			cnt++;

			switch (dataType)
			{
			case DT_INT16:
			case DT_UINT16:
				{
					m_nSampleSize += 2;
				}
				break;
			case DT_INT32:
			case DT_UINT32:
			case DT_FLOAT32:
				{
					m_nSampleSize += 4;				
					
				}
				break;
			case DT_INT64:
			case DT_UINT64:
			case DT_FLOAT64:
				{
					m_nSampleSize += 8;
				}
				break;
			default:
				break;
			}
		}
		
	}
	// add the sample counter size
	m_nSampleSize += 8;
	if(cnt!=m_nEnabledChannelCnt+(m_bIs64?5:3))// add two (per liveamp) because we are ignoring (for now) the CT_DIG channels, which are enabled
		// and add one for the time stamp channel
		throw std::runtime_error((std::string("Error: Enabled channel counter mismatch in device ") + m_sSerialNumber).c_str());	
}

void LiveAmp::startAcquisition(void){
	
	int res = ampStartAcquisition(h);
	if(res != AMP_OK)					
		throw std::runtime_error(("Error starting acquisition, error code:  " + std::to_string(res)));

}

void LiveAmp::stopAcquisition(void){
	
	int res = ampStopAcquisition(h);
	if(res != AMP_OK)					
		throw std::runtime_error(("Error stopping acquisition, error code:  " + std::to_string(res)));

}

int64_t LiveAmp::getAmpRawData(BYTE* buffer, int bufferSize){
	int64_t samplesRead = ampGetData(h, buffer, bufferSize, 0); 
	return samplesRead;
}

double LiveAmp::repackageAmpRawData(BYTE* pBuffer, int nBufferSize, int64_t nSamplesRead)
{
	
	int offset = 0;
	float sample = 0;
	int triggTmp = 0;
	int isSecondBit = 0;
	double time_stamp = 0.0;

	int64_t numSamples = nSamplesRead / m_nSampleSize;
		
	std::vector<float> tmpData;

	for (int s = 0; s < numSamples; s++)
	{
		offset = 0;				
		offset += 8; // sample counter offset 

		tmpData.resize(m_nEnabledChannelCnt);

		for (int i=0; i < m_nEnabledChannelCnt; i++)
		{
			switch (m_ciChannelInfoArray[i].nDataType)
			{
				case DT_INT16:
					{
						int16_t tmp = *(int16_t*)&pBuffer[s*m_nSampleSize + offset];
						sample = (float) tmp * m_ciChannelInfoArray[i].fResolution;
						offset += 2;
						break;
					}
				case DT_UINT16:
					{
					// note: in the case of liveamp, this floating point sample
					// will need to be converted back to an integer and &ed with the
					// second bit for the true value of the trigger
						uint16_t tmp = *(uint16_t*)&pBuffer[s*m_nSampleSize + offset];
						sample = (float) tmp * m_ciChannelInfoArray[i].fResolution;
						if (((int)sample>>8) == 254)
							tmp = tmp;
						offset += 2;
						break;
					}					
				case DT_INT32:
					{
						int32_t tmp = *(int32_t*)&pBuffer[s*m_nSampleSize + offset];
						sample = (float)(tmp) * m_ciChannelInfoArray[i].fResolution;
						offset += 4;
						break;
					}
				case DT_UINT32:
					{
						uint32_t tmp = *(uint32_t*)&pBuffer[s*m_nSampleSize + offset];
						sample = (float) tmp * m_ciChannelInfoArray[i].fResolution;
						offset += 4;
						break;
					}
				case DT_FLOAT32:
					{
						float tmp = *(float*)&pBuffer[s*m_nSampleSize + offset];
						sample = (float) tmp * m_ciChannelInfoArray[i].fResolution;
						offset += 4;
						break;
					}
				case DT_INT64:
					{
						int64_t tmp = *(int64_t*)&pBuffer[s*m_nSampleSize + offset];
						sample = (float) tmp * m_ciChannelInfoArray[i].fResolution;
						offset += 8;
						break;
					}
				case DT_UINT64:
					{
						uint64_t tmp = *(uint64_t*)&pBuffer[s*m_nSampleSize + offset];
						sample = (float) tmp * m_ciChannelInfoArray[i].fResolution;
						offset += 8;
						break;
					}
				case DT_FLOAT64:
					{
						
						float tmp = *(float*)&pBuffer[s*m_nSampleSize + offset];
						sample = (float) tmp * m_ciChannelInfoArray[i].fResolution;
						offset += 8;
						break;					
					}					
				default:
					break;
			}

			tmpData[i] = sample;
		}


		//labOutData.push_back(tmpData);
		m_labDataContainer.push_back(tmpData);
	}
	// since we are ignoring the digital channels, use alternative procedure
	// to harvest the timing channel
	int extraOffset = (m_bIs64 ? 12 : 6); // accounts for the digital triggers that we are skipping
	double tmp = *(double*)&pBuffer[offset + extraOffset];
	time_stamp = (double)tmp;
	return time_stamp;
}

void LiveAmp::setOutTriggerMode(t_TriggerOutputMode tmMode, int nSyncPin, int nFreq, int nPulseWidth)
{
	if (!m_bHasSTE)return;
	int nPer = m_fSamplingRate / nFreq;
	int res = ampSetProperty(h, PG_MODULE, m_nSTEIdx, MPROP_I32_TriggerOutMode, &tmMode, sizeof(tmMode));
	res = ampSetProperty(h, PG_MODULE, m_nSTEIdx, MPROP_I32_TriggerSyncPin, &nSyncPin, sizeof(nSyncPin));
	res = ampSetProperty(h, PG_MODULE, m_nSTEIdx, MPROP_I32_TriggerSyncPeriod, &nPer, sizeof(nPer));
	res = ampSetProperty(h, PG_MODULE, m_nSTEIdx, MPROP_I32_TriggerSyncPin, &nPulseWidth, sizeof(nPulseWidth));
}

void LiveAmp::setupLiveAmpBuffer(int nChunkSize)
{
	m_labDataContainer = LIVEAMP_BUFF(nChunkSize, std::vector<float>(m_nEnabledChannelCnt));
}

void LiveAmp::clearLiveAmpBuffer(void) { m_labDataContainer.clear(); }

