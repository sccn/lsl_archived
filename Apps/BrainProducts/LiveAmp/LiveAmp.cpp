#include "LiveAmp.h"
#include <boost/lexical_cast.hpp>


// TODO: overload the constructor so that 
// live amp objects can be initialized by serial number etc.
//
LiveAmp::LiveAmp(std::string serialNumberIn, float samplingRateIn, bool useSim, int recordingModeIn) {
//LiveAmp::LiveAmp(){	
	int res;
	char HWI[20];
	

	if(useSim)
		strcpy_s(HWI, "SIM");
	else
		strcpy_s(HWI, "ANY"); // todo: parameterize this

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

			// got a hit!
			if(!(strcmp(sVar, serialNumberIn.c_str()))) {
				
				// set the sampling rate on the device
				result = ampSetProperty(hndl, PG_DEVICE, 0, DPROP_F32_BaseSampleRate, &samplingRateIn, sizeof(samplingRateIn));
				if(result != AMP_OK){				
					throw std::runtime_error(("Error setting sampling rate, error code:  " + boost::lexical_cast<std::string>(result)).c_str());
					return;
				}
				samplingRate = samplingRateIn;
				
				// set the device mode to recording
				result = ampSetProperty(hndl, PG_DEVICE, 0, DPROP_I32_RecordingMode, &recordingModeIn, sizeof(recordingModeIn));
				if(result != AMP_OK){			
					throw std::runtime_error(("Error setting acquisition mode, error code:  " + boost::lexical_cast<std::string>(result)).c_str());	
					return;	
				}
				recordingMode = recordingModeIn;

				// set the handle
				h = hndl;

				// set the serial number
				serialNumber = std::string(sVar);

				// set the available channels
				result = ampGetProperty(h, PG_DEVICE, 0, DPROP_I32_AvailableChannels, &availableChannels, sizeof(availableChannels));
				if(result != AMP_OK)					
					throw std::runtime_error(("Error getting available channel count, error code:  " + boost::lexical_cast<std::string>(result)).c_str());	

				break;
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
				result = ampGetProperty(hndl, PG_DEVICE, i, DPROP_I32_AvailableChannels, &iVar, sizeof(iVar)); 
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

void LiveAmp::enableChannels(std::vector<int> eegIndecesIn, bool auxEnable, bool accEnable, bool bipolarEnable) {
	
	int res;
	int type;
	int i;	
	int enable, wasEnabled;
	int bipType = CT_BIP;
	char cValue[20]; // for determining if the aux channel is an accelerometer or not

	if(!eegIndeces.empty())eegIndeces.clear();
	if(!auxIndeces.empty())auxIndeces.clear();
	if(!accIndeces.empty())accIndeces.clear();
	if(!trigIndeces.empty())trigIndeces.clear();
	enabledChannelCnt = 0;	
	int passCnt = 0;
	
	// make sure the liveamp has valid channels available
	if(availableChannels == -1) {
		throw std::runtime_error((std::string("Invalid number of available channels on device ") + serialNumber).c_str());
		return;
	}

	// go through the available channels and enable them if they are chosen
	for(i=0;i<availableChannels;i++) {
		enable = false;
		res = ampGetProperty(h, PG_CHANNEL, i, CPROP_I32_Type, &type, sizeof(type));
		if(res != AMP_OK)					
			throw std::runtime_error(("Error getting property for channel type: error code:  " + boost::lexical_cast<std::string>(res)).c_str());
	
		if (type == CT_EEG || type == CT_BIP) {
			// go through the requested eeg channel vector and enable on match
			passCnt = 0;
			for(std::vector<int>::iterator it=eegIndecesIn.begin(); it!=eegIndecesIn.end();++it) {
				if(*it==i){
					enable = true;
					eegIndeces.push_back(i);
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

					if(type == CT_BIP && bipolarEnable == true){
						res = ampSetProperty(h, PG_CHANNEL, i, CPROP_I32_Type, &bipType, sizeof(bipType));
						if (res != AMP_OK)								
							throw std::runtime_error(("Error SetProperty enable BIPOLAR EEG channels, error: "  + boost::lexical_cast<std::string>(res)).c_str());	
					}
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
				if(enable == true){
					accIndeces.push_back(i);
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
				

				enable = auxEnable;
				if(enable==true) {
					accIndeces.push_back(i);
					++enabledChannelCnt;
				}
				res = ampGetProperty(h, PG_CHANNEL, i, CPROP_B32_RecordingEnabled, &wasEnabled, sizeof(wasEnabled));
				if (res != AMP_OK)	
					throw std::runtime_error(("Error SetProperty enable for AUX channels, error: "  + boost::lexical_cast<std::string>(res)).c_str());
					
				// if requested, enable it
				if(wasEnabled!=enable){
					res = ampSetProperty(h, PG_CHANNEL, i, CPROP_B32_RecordingEnabled, &enable, sizeof(enable));
					if (res != AMP_OK)				
						throw std::runtime_error(("Error SetProperty enable for AUX channels, error: "  + boost::lexical_cast<std::string>(res)).c_str());	
				}
				
			}			
		}
		
		// the triggers are always enabled, but we need to keep track of them for channel labelling purposes
		if(type == CT_TRG || type == CT_DIG) {
			trigIndeces.push_back(i);	
			++enabledChannelCnt;
		}
	}

	// get the sample size in bytes and make an array of sample types
	int datatype;
	int cnt = 0;
	sampleSize=0;
	int enabled;

	for(int i=0;i<availableChannels;i++) {
		
			
		res = ampGetProperty(h, PG_CHANNEL, i, CPROP_B32_RecordingEnabled, &enabled, sizeof(enabled));
		if (enabled) {
			res = ampGetProperty(h, PG_CHANNEL, i, CPROP_I32_DataType, &datatype, sizeof(datatype));
			dataTypeArray[cnt++] = datatype;

			switch (datatype)
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
	if(cnt!=enabledChannelCnt)
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
			switch (dataTypeArray[i])
			{
				case DT_INT16:
					{
						int16_t tmp = *(int16_t*)&buffer[s*sampleSize + offset];
						sample = (float) tmp;
						offset += 2;
						break;
					}
				case DT_UINT16:
					{
						uint16_t tmp = *(uint16_t*)&buffer[s*sampleSize + offset];
						sample = (float) tmp;
						offset += 2;
						break;
					}					
				case DT_INT32:
					{
						int32_t tmp = *(int32_t*)&buffer[s*sampleSize + offset];
						sample = (float) tmp;
						offset += 4;
						break;
					}
				case DT_UINT32:
					{
						uint32_t tmp = *(uint32_t*)&buffer[s*sampleSize + offset];
						sample = (float) tmp;
						offset += 4;
						break;
					}
				case DT_FLOAT32:
					{
						float tmp = *(float*)&buffer[s*sampleSize + offset];
						sample = (float) tmp;
						offset += 4;
						break;
					}
				case DT_INT64:
					{
						int64_t tmp = *(int64_t*)&buffer[s*sampleSize + offset];
						sample = (float) tmp;
						offset += 8;
						break;
					}
				case DT_UINT64:
					{
						uint64_t tmp = *(uint64_t*)&buffer[s*sampleSize + offset];
						sample = (float) tmp;
						offset += 8;
						break;
					}
				case DT_FLOAT64:
					{
						
						float tmp = *(float*)&buffer[s*sampleSize + offset];
						sample = (float) tmp;
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

