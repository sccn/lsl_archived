#ifndef LiveAmp_H
#define LiveAmp_H

// Header file for higher level LiveAmp connection functions

// LiveAmp API
#include "Amplifier_LIB.h"
#include <vector>
#include <string>




class LiveAmp {

private:
	// data get from device (defined in construction)
	// these are initialized in the constructor
	HANDLE h;                        // device handle
	std::string serialNumber;        // serial number
	int availableChannels;           // number of available devices

	// these are set later on by the user
	float samplingRate;
	int recordingMode;               // defaults to record mode

	// set during configuration
	int dataTypeArray[100];
	int sampleSize;


	// flexible? channel access arrays
	std::vector<int> channelIndexes;  
	std::vector<int> eegIndeces;
	std::vector<int> auxIndeces;
	std::vector<int> accIndeces;
	std::vector<int> trigIndeces;

	int enabledChannelCnt;

public:
	// constructor
	// initialize by at least the serial number and a container for the enumerated devices
	LiveAmp::LiveAmp(std::string serialNumberIn, float samplingRateIn = 500, bool useSim = false, int recordingModeIn = RM_NORMAL);
	//LiveAmp::LiveAmp (void);
	// destructor
	~LiveAmp();
	
	// get the serial numbers and channel counts of all available liveamps
	static void enumerate(std::vector<std::pair<std::string, int>> &ampData, bool useSim=false);

	// close live amp device
	void close();

	// enable requested channels: for now acc and aux are all or nothing, triggers are always on, and eeg channels can be selected
	void enableChannels(std::vector<int> eegIndecesIn, bool auxEnable, bool accEnable, bool bipolarEnable);

	// activate the configured device with enabled channels
	void startAcquisition(void);

	// activate the configured device with enabled channels
	void stopAcquisition(void);

	// get data from device
	int64_t pullAmpData(BYTE* buffer, int bufferSize);

	// push it into a vector TODO: make this a template to support any buffer type
	void pushAmpData(BYTE* buffer, int bufferSize, int64_t samplesRead, std::vector<std::vector<float>> &outData);
	
	// public data access methods 	
	inline float             getSamplingRate(void){return samplingRate;}
	inline HANDLE            getHandle(void){return h;}
	inline std::string&      getSerialNumber(void){return serialNumber;}
	inline int               getAvailableChannels(void){return availableChannels;}
	inline int               getRecordingMode(void){return recordingMode;}
	inline std::vector<int>& getEEGIndeces(void){return eegIndeces;}
	inline std::vector<int>& getAuxIndeces(void){return auxIndeces;}
	inline std::vector<int>& getAccIndeces(void){return accIndeces;}
	inline std::vector<int>& getTrigIndeces(void){return trigIndeces;}
	inline int               getEnabledChannelCnt(void){return enabledChannelCnt;}
	inline int*              getDataTypeArray(void){return dataTypeArray;}  
	inline int               getSampleSize(void){return sampleSize;}
};

#endif //LiveAmp_H