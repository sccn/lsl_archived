#include "LiveAmp.h"
#include <boost/lexical_cast.hpp>


// TODO: overload the constructor so that 
// live amp objects can be initialized by serial number etc.
//
LiveAmp::LiveAmp(std::string serialNumberIn, float samplingRateIn, bool useSim, int recordingModeIn) {
//LiveAmp::LiveAmp(){	
	int res;
	char HWI[20];
	
	bIsClosed = false;
	bHasSTE = false;
	if(useSim)
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
				
				//std::string msg = "Cannot open device: ";
				//msg.append (boost::lexical_cast<std::string>(i).c_str());
				//msg.append("  error= " );
				//msg.append(boost::lexical_cast<std::string>(res).c_str()); // TODO: error enumeration from liveamp driver
				//throw std::runtime_error(msg);
			
				h = hndl;
				char sVar[20]; 
				result = ampGetProperty(hndl, PG_DEVICE, i, DPROP_CHR_SerialNumber, sVar, sizeof(sVar));

				// got a hit!
				if (!(strcmp(sVar, serialNumberIn.c_str()))) {

					// set the sampling rate on the device
					result = ampSetProperty(hndl, PG_DEVICE, 0, DPROP_F32_BaseSampleRate, &samplingRateIn, sizeof(samplingRateIn));
					if (result != AMP_OK) {
						throw std::runtime_error(("Error setting sampling rate, error code:  " + boost::lexical_cast<std::string>(result)).c_str());
						return;
					}
					samplingRate = samplingRateIn;

					// set the device mode to recording
					result = ampSetProperty(hndl, PG_DEVICE, 0, DPROP_I32_RecordingMode, &recordingModeIn, sizeof(recordingModeIn));
					if (result != AMP_OK) {
						throw std::runtime_error(("Error setting acquisition mode, error code:  " + boost::lexical_cast<std::string>(result)).c_str());
						return;
					}
					recordingMode = recordingModeIn;



					// set the serial number
					serialNumber = std::string(sVar);

					// set the available channels
					result = ampGetProperty(h, PG_DEVICE, 0, DPROP_I32_AvailableChannels, &availableChannels, sizeof(availableChannels));
					if (result != AMP_OK)
						throw std::runtime_error(("Error getting available channel count, error code:  " + boost::lexical_cast<std::string>(result)).c_str());

					// set the usable channels               // this property name is misspelled in the API---we use the correct spelling locally
					result = ampGetProperty(h, PG_MODULE, 0, MPROP_I32_UseableChannels, &usableChannels, sizeof(usableChannels));
					if (result != AMP_OK)
						throw std::runtime_error(("Error getting usable channel count, error code:  " + boost::lexical_cast<std::string>(result)).c_str());

					// detect whether or not STE is connected
					char modName[100];
					result = ampGetProperty(h, PG_DEVICE, 0, DPROP_I32_AvailableModules, &availableModules, sizeof(availableModules));
					for (int n = 0; n < availableModules; n++)
					{
						result = ampGetProperty(h, PG_MODULE, n, MPROP_CHR_Type, &modName, sizeof(modName));
						if (!strcmp(modName, "STE"))
						{
							bHasSTE = true;
							STEIdx = n;
						}
					}
					break;
				}
			}

		}
	}

}

LiveAmp::~LiveAmp(void){
//
//	close();
////	int res = ampCloseDevice(h);
}

void LiveAmp::enumerate(std::vector<std::pair<std::string, int>> &ampData, bool useSim){

	int res;
	char HWI[20];

	if(!ampData.empty()){
		throw std::runtime_error("Input ampData vector isn't empty");
		return;
	}


	if(useSim)
		strcpy_s(HWI, "SIM");
	else
		strcpy_s(HWI, "ANY"); 

	res = ampEnumerateDevices(HWI, sizeof(HWI), "LiveAmp", 0);

	if (res <= 0)			
		throw std::runtime_error("No LiveAmp connected");
	else {
		for(int i=0;i<res;i++){	
			int result;
			HANDLE hndl = NULL;

			result = ampOpenDevice(i, &hndl);
			if(result != AMP_OK) {
				std::string msg = "Cannot open device: ";
				msg.append (boost::lexical_cast<std::string>(i).c_str());
				msg.append("  error= " );
				msg.append(boost::lexical_cast<std::string>(res).c_str()); // TODO: error enumeration from liveamp driver
				throw std::runtime_error(msg);
			}		
			
			char sVar[20];
			result = ampGetProperty(hndl, PG_DEVICE, i, DPROP_CHR_SerialNumber, sVar, sizeof(sVar)); 
			if(result != AMP_OK) {
				std::string msg = "Cannot get device serial number: ";
				msg.append (boost::lexical_cast<std::string>(i).c_str());
				msg.append("  error= " );
				msg.append(boost::lexical_cast<std::string>(res).c_str()); // TODO: error enumeration from liveamp driver
				throw std::runtime_error(msg);
			}		
			else {
		
				int32_t iVar;
				result = ampGetProperty(hndl, PG_MODULE, i, MPROP_I32_UseableChannels, &iVar, sizeof(iVar)); 
				if(result != AMP_OK) {
					std::string msg = "Cannot get device channel count: ";
					msg.append (boost::lexical_cast<std::string>(i).c_str());
					msg.append("  error= " );
					msg.append(boost::lexical_cast<std::string>(res).c_str()); // TODO: error enumeration from liveamp driver
					throw std::runtime_error(msg);
				}		
				else
					ampData.push_back(std::make_pair(std::string(sVar), iVar));
			
			}

			// immediately close the device???
			result = ampCloseDevice(hndl);
			if(result != AMP_OK) {
				std::string msg = "Cannot close device: ";
				msg.append("  error= " );
				msg.append(boost::lexical_cast<std::string>(result).c_str()); 
				throw std::runtime_error(msg);
			}
		}
	}
}


void LiveAmp::close(void){

	int result = ampCloseDevice(h);
	if(result != AMP_OK) {
		std::string msg = "Cannot close device: ";
		msg.append (serialNumber.c_str());
		msg.append("  error= " );
		msg.append(boost::lexical_cast<std::string>(result).c_str()); // TODO: error enumeration from liveamp driver
		throw std::runtime_error(msg);
	}
}

void LiveAmp::enableChannels(const std::vector<int>& eegIndicesIn, const std::vector<int>& bipolarIndicesIn, const std::vector<int>& auxIndicesIn, bool accEnable) {
	
	int res;
	int type;
	int i;	
	int enable, wasEnabled;
	int bipType = CT_BIP;
	char cValue[20]; // for determining if the aux channel is an accelerometer or not
	char cUnit[256];

	if(!eegIndices.empty())eegIndices.clear();
	if(!bipolarIndices.empty())bipolarIndices.clear();
	if(!auxIndices.empty())auxIndices.clear();
	if(!accIndices.empty())accIndices.clear();
	if(!trigIndices.empty())trigIndices.clear();
	enabledChannelCnt = 0;	
	int passCnt = 0;
	
	// make sure the liveamp has valid channels available
	if(availableChannels == -1) {
		throw std::runtime_error((std::string("Invalid number of available channels on device ") + serialNumber).c_str());
		return;
	}
	// first, disable all the channels
	enable = 0;
	for(i=0;i<availableChannels;i++)
		res = ampSetProperty(h, PG_CHANNEL, i, CPROP_B32_RecordingEnabled, &enable, sizeof(enable));

	// go through the available channels and enable them if they are chosen
	for(i=0;i<availableChannels;i++) {
		enable = 0;
		res = ampGetProperty(h, PG_CHANNEL, i, CPROP_I32_Type, &type, sizeof(type));
		if(res != AMP_OK)					
			throw std::runtime_error(("Error getting property for channel type: error code:  " + boost::lexical_cast<std::string>(res)).c_str());
		res = ampGetProperty(h, PG_CHANNEL, i, CPROP_CHR_Unit, cUnit, sizeof(cUnit));
		if (type == CT_EEG || type == CT_BIP) {
			// go through the requested eeg channel vector and enable on match
			passCnt = 0;
			for(std::vector<int>::const_iterator it=eegIndicesIn.begin(); it!=eegIndicesIn.end();++it) {
				if(*it==i){
					enable = 1;
					eegIndices.push_back(i);
					++enabledChannelCnt;
					passCnt++;
					// I'm not sure if this check is necessary, but it's in the old code, so I am assuming it is
					res = ampGetProperty(h, PG_CHANNEL, i, CPROP_B32_RecordingEnabled, &wasEnabled, sizeof(wasEnabled));
					if (res != AMP_OK)								
						throw std::runtime_error(("Error GetProperty enable for EEG channels, error: "  + boost::lexical_cast<std::string>(res)).c_str());	
					
					if(wasEnabled!=enable) {		
						res = ampSetProperty(h, PG_CHANNEL, i, CPROP_B32_RecordingEnabled, &enable, sizeof(enable));
						if (res != AMP_OK)				
							throw std::runtime_error(("Error SetProperty enable for EEG channels, error: "  + boost::lexical_cast<std::string>(res)).c_str());	
					}

					//if(type == CT_BIP && bipolarEnable == true){
					//	res = ampSetProperty(h, PG_CHANNEL, i, CPROP_I32_Type, &bipType, sizeof(bipType));
					//	if (res != AMP_OK)								
					//		throw std::runtime_error(("Error SetProperty enable BIPOLAR EEG channels, error: "  + boost::lexical_cast<std::string>(res)).c_str());	
					//}
				}
			}

			for(std::vector<int>::const_iterator it=bipolarIndicesIn.begin(); it!=bipolarIndicesIn.end();++it) {
				if(*it==i){
					enable = 1;
					bipolarIndices.push_back(i);
					++enabledChannelCnt;
					passCnt++;
					// I'm not sure if this check is necessary, but it's in the old code, so I am assuming it is
					res = ampGetProperty(h, PG_CHANNEL, i, CPROP_B32_RecordingEnabled, &wasEnabled, sizeof(wasEnabled));
					if (res != AMP_OK)								
						throw std::runtime_error(("Error GetProperty enable for EEG channels, error: "  + boost::lexical_cast<std::string>(res)).c_str());	
					
					if(wasEnabled!=enable) {		
						res = ampSetProperty(h, PG_CHANNEL, i, CPROP_B32_RecordingEnabled, &enable, sizeof(enable));
						if (res != AMP_OK)				
							throw std::runtime_error(("Error SetProperty enable for EEG channels, error: "  + boost::lexical_cast<std::string>(res)).c_str());	
					}

			
					res = ampSetProperty(h, PG_CHANNEL, i, CPROP_I32_Type, &bipType, sizeof(bipType));
					if (res != AMP_OK)								
						throw std::runtime_error(("Error SetProperty enable BIPOLAR EEG channels, error: "  + boost::lexical_cast<std::string>(res)).c_str());	
					
				}
			}
		}

		// if it is an aux channel see if it can/should be enabled and if it is an acc channel
		if(type == CT_AUX){		
			res = ampGetProperty(h, PG_CHANNEL, i, CPROP_CHR_Function, &cValue, sizeof(cValue));
			
			if (res != AMP_OK)			
				throw std::runtime_error(("Error GetProperty CPROP_CHR_Function error: "  + boost::lexical_cast<std::string>(res)).c_str());
			
			// check that this aux channel is an acc channel
			if(cValue[0] == 'X' || cValue[0] == 'Y' ||cValue[0] == 'Z' || cValue[0] == 'x' ||cValue[0] == 'y' ||cValue[0] == 'z') {
				

				enable = accEnable;
				if(enable == 1){
					accIndices.push_back(i);
					++enabledChannelCnt;
				}
				res = ampGetProperty(h, PG_CHANNEL, i, CPROP_B32_RecordingEnabled, &wasEnabled, sizeof(wasEnabled));
				if (res != AMP_OK)	
					throw std::runtime_error(("Error SetProperty enable for ACC channels, error: "  + boost::lexical_cast<std::string>(res)).c_str());
					
				// if requested, enable it
				if(wasEnabled!=enable){
					res = ampSetProperty(h, PG_CHANNEL, i, CPROP_B32_RecordingEnabled, &enable, sizeof(enable));
					if (res != AMP_OK)				
						throw std::runtime_error(("Error SetProperty enable for ACC channels, error: "  + boost::lexical_cast<std::string>(res)).c_str());	
				}
				
			}

			else { // this is a non-acc aux channel
				
				for (std::vector<int>::const_iterator it = auxIndicesIn.begin(); it != auxIndicesIn.end(); ++it) {
					if (*it == i) {

						enable = 1;
						if (enable == 1) {
							accIndices.push_back(i);
							++enabledChannelCnt;
						}
						res = ampGetProperty(h, PG_CHANNEL, i, CPROP_B32_RecordingEnabled, &wasEnabled, sizeof(wasEnabled));
						if (res != AMP_OK)
							throw std::runtime_error(("Error SetProperty enable for AUX channels, error: " + boost::lexical_cast<std::string>(res)).c_str());

						// if requested, enable it
						if (wasEnabled != enable) {
							res = ampSetProperty(h, PG_CHANNEL, i, CPROP_B32_RecordingEnabled, &enable, sizeof(enable));
							if (res != AMP_OK)
								throw std::runtime_error(("Error SetProperty enable for AUX channels, error: " + boost::lexical_cast<std::string>(res)).c_str());
						}
					}
				}
				//enable = auxEnable;
				//if(enable==true) {
				//	accIndices.push_back(i);
				//	++enabledChannelCnt;
				//}
				//res = ampGetProperty(h, PG_CHANNEL, i, CPROP_B32_RecordingEnabled, &wasEnabled, sizeof(wasEnabled));
				//if (res != AMP_OK)	
				//	throw std::runtime_error(("Error SetProperty enable for AUX channels, error: "  + boost::lexical_cast<std::string>(res)).c_str());
				//	
				//// if requested, enable it
				//if(wasEnabled!=enable){
				//	res = ampSetProperty(h, PG_CHANNEL, i, CPROP_B32_RecordingEnabled, &enable, sizeof(enable));
				//	if (res != AMP_OK)				
				//		throw std::runtime_error(("Error SetProperty enable for AUX channels, error: "  + boost::lexical_cast<std::string>(res)).c_str());	
				//}
				
			}			
		}
		
		// the triggers are always enabled, but we need to keep track of them for channel labelling purposes
		if(type == CT_TRG) {
			trigIndices.push_back(i);	
			++enabledChannelCnt;
		}
	}

	// get the sample size in bytes and make an array of sample types
	int dataType;
	float resolution;
	int channelType;
	int cnt = 0;
	sampleSize=0;
	int enabled;

	for(int i=0;i<availableChannels;i++) {
		
			
		res = ampGetProperty(h, PG_CHANNEL, i, CPROP_B32_RecordingEnabled, &enabled, sizeof(enabled));

		if (enabled) {
			res = ampGetProperty(h, PG_CHANNEL, i, CPROP_I32_DataType, &dataType, sizeof(dataType));
			channelInfoArray[cnt].dataType = dataType;
			res = ampGetProperty(h, PG_CHANNEL, i, CPROP_F32_Resolution, &resolution, sizeof(resolution));
			channelInfoArray[cnt].resolution = resolution;
			res = ampGetProperty(h, PG_CHANNEL, i, CPROP_I32_Type, &channelType, sizeof(channelType));
			channelInfoArray[cnt++].channelType = channelType;

			switch (dataType)
			{
			case DT_INT16:
			case DT_UINT16:
				{
					sampleSize += 2;
				}
				break;
			case DT_INT32:
			case DT_UINT32:
			case DT_FLOAT32:
				{
					sampleSize += 4;				
					
				}
				break;
			case DT_INT64:
			case DT_UINT64:
			case DT_FLOAT64:
				{
					sampleSize += 8;
				}
				break;
			default:
				break;
			}
		}
		
	}
	// add the sample counter size
	sampleSize += 8;
	if(cnt!=enabledChannelCnt+2)// add two because we are ignoring (for now) the CT_DIG channels, which are enabled 
		throw std::runtime_error((std::string("Error: Enabled channel counter mismatch in device ") + serialNumber).c_str());	
}

void LiveAmp::startAcquisition(void){
	
	int res = ampStartAcquisition(h);
	if(res != AMP_OK)					
		throw std::runtime_error(("Error starting acquisition, error code:  " + boost::lexical_cast<std::string>(res)).c_str());

}

void LiveAmp::stopAcquisition(void){
	
	int res = ampStopAcquisition(h);
	if(res != AMP_OK)					
		throw std::runtime_error(("Error stopping acquisition, error code:  " + boost::lexical_cast<std::string>(res)).c_str());

}

int64_t LiveAmp::pullAmpData(BYTE* buffer, int bufferSize){
	int64_t samplesRead = ampGetData(h, buffer, bufferSize, 0); 
	return samplesRead;
}

// TODO: overload this function to support other data types, maybe use a std::vector<T> for this?
void LiveAmp::pushAmpData(BYTE* buffer, int bufferSize, int64_t samplesRead, std::vector<std::vector<float>> &outData)
{
	uint64_t sc;
	
	int offset = 0;
	float sample = 0;
	int triggTmp = 0;
	int isSecondBit = 0;

	int64_t numSamples = samplesRead / sampleSize;
		
	std::vector<float> tmpData;

	for (int s = 0; s < numSamples; s++)
	{
		offset = 0;
		sc = *(uint64_t*)&buffer[s*sampleSize + offset];				
		offset += 8; // sample counter offset 

		tmpData.resize(enabledChannelCnt);

		for (int i=0; i < enabledChannelCnt; i++)
		{
			switch (channelInfoArray[i].dataType)
			{
				case DT_INT16:
					{
						int16_t tmp = *(int16_t*)&buffer[s*sampleSize + offset];
						sample = (float) tmp * channelInfoArray[i].resolution;
						offset += 2;
						break;
					}
				case DT_UINT16:
					{
					// note: in the case of liveamp, this floating point sample
					// will need to be converted back to an integer and &ed with the
					// second bit for the true value of the trigger
						uint16_t tmp = *(uint16_t*)&buffer[s*sampleSize + offset];
						sample = (float) tmp * channelInfoArray[i].resolution;
						if (((int)sample>>8) == 254)
							tmp = tmp;
						offset += 2;
						break;
					}					
				case DT_INT32:
					{
						int32_t tmp = *(int32_t*)&buffer[s*sampleSize + offset];
						sample = (float)(tmp) * channelInfoArray[i].resolution;
						offset += 4;
						break;
					}
				case DT_UINT32:
					{
						uint32_t tmp = *(uint32_t*)&buffer[s*sampleSize + offset];
						sample = (float) tmp * channelInfoArray[i].resolution;
						offset += 4;
						break;
					}
				case DT_FLOAT32:
					{
						float tmp = *(float*)&buffer[s*sampleSize + offset];
						sample = (float) tmp * channelInfoArray[i].resolution;
						offset += 4;
						break;
					}
				case DT_INT64:
					{
						int64_t tmp = *(int64_t*)&buffer[s*sampleSize + offset];
						sample = (float) tmp * channelInfoArray[i].resolution;
						offset += 8;
						break;
					}
				case DT_UINT64:
					{
						uint64_t tmp = *(uint64_t*)&buffer[s*sampleSize + offset];
						sample = (float) tmp * channelInfoArray[i].resolution;
						offset += 8;
						break;
					}
				case DT_FLOAT64:
					{
						
						float tmp = *(float*)&buffer[s*sampleSize + offset];
						sample = (float) tmp * channelInfoArray[i].resolution;
						offset += 8;
						break;					
					}					
				default:
					break;
			}

			tmpData[i] = sample;
		}
		outData.push_back(tmpData);
	}
}

void LiveAmp::setOutTriggerMode(t_TriggerOutputMode mode, int syncPin, int freq, int pulseWidth)
{
	if (!bHasSTE)return;
	int per = samplingRate / freq;
	int res = ampSetProperty(h, PG_MODULE, STEIdx, MPROP_I32_TriggerOutMode, &mode, sizeof(mode));
	res = ampSetProperty(h, PG_MODULE, STEIdx, MPROP_I32_TriggerSyncPin, &syncPin, sizeof(syncPin));
	res = ampSetProperty(h, PG_MODULE, STEIdx, MPROP_I32_TriggerSyncPeriod, &per, sizeof(per));
	res = ampSetProperty(h, PG_MODULE, STEIdx, MPROP_I32_TriggerSyncPin, &pulseWidth, sizeof(pulseWidth));
}

