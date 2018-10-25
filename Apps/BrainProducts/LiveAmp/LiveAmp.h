/*********************************************************

This is the declaration of the LiveAmp class. This class
makes use of the API defined in Amplifier_LIB.h in an
attempt to simplify its use in C++ programs.

Not all features are implemented.

Copyright (c) 2018 Brain Products

Released under the MIT license (see LICENSE.txt)

**********************************************************/

#ifndef LiveAmp_H
#define LiveAmp_H

// Header file for higher level LiveAmp connection functions

// LiveAmp API
#include "Amplifier_LIB.h"
#include <vector>
#include <string>

// typedef for convenience, readability
#define LIVEAMP_BUFF std::vector<std::vector<float>>
//#define LIVEAMP_DATA std::vector<std::pair<std::string, int>>

class LiveAmp {

private:
	// data get from device (defined in construction)
	// these are initialized in the constructor
	HANDLE h;                           // device handle
	std::string m_sSerialNumber;        // serial number
	int m_nAvailableChannels;           // number of available channels
	int m_nUsableChannels;				// whether we are liveamp 8, 16, 32, 64
	int m_nAvailableModules;            // use to detect STE box
	bool m_bHasSTE;                     // actual flag for STE connected
	int m_nSTEIdx;                      // index of which module is the STE
	bool m_bIs64;                       // is it a live amp 64?

	// these are set later on by the user
	float m_fSamplingRate;
	int m_nRecordingMode;               // defaults to record mode

	// set during configuration
	typedef struct _channelInfo {
		int nDataType;
		float fResolution;
		int nChannelType;
		std::string sChannelUnit;
	}t_channelInfo;
	// int dataTypeArray[100];
	t_channelInfo m_ciChannelInfoArray[100];
	int m_nSampleSize;

	enum t_triggerOutputMode {
		TM_DEFAULT = 0,
		TM_SYNC    = 1,
		TM_MIRROR  = 2
	};

	// channel access arrays
	std::vector<int> m_vnChannelIndexes;  
	std::vector<int> m_vnEegIndices;
	std::vector<int> m_vnBipolarIndices;
	std::vector<int> m_vnAuxIndices;
	std::vector<int> m_vnAccIndices;
	std::vector<int> m_vnTrigIndices;
	int              m_nTimIndex;

	int m_nEnabledChannelCnt;
	bool m_bIsClosed;

	bool m_bUsingTim;

public:
	// sorted data
	LIVEAMP_BUFF m_labDataContainer;

	// constructor
	// initialize by at least the serial number and a container for the enumerated devices
	LiveAmp::LiveAmp(std::string sSerialNumberIn, float fSamplingRateIn = 500.0, bool bUseSim = false, int nRecordingModeIn = RM_NORMAL);
	//LiveAmp::LiveAmp (void);
	// destructor
	~LiveAmp();
	
	// preliminary data from enumerate method
	typedef struct _deviceInfo {
		std::string sSerialNumber;
		int nChannelCount;
		bool bIs64;
	}t_deviceInfo;
	// for client convenience
	#define LIVEAMP_DEVICEENUMINFO std::vector<LiveAmp::t_deviceInfo>
	// get the serial numbers and channel counts of all available liveamps
	static void enumerate(LIVEAMP_DEVICEENUMINFO &ladDevData, bool bUseSim=false);

	// close live amp device
	void close();

	// enable requested channels: for now acc and aux are all or nothing, triggers are always on, and eeg channels can be selected
	void enableChannels(const std::vector<int>& bnEegIndicesIn, 
		const std::vector<int>& vnBipolarIndicesIn, 
		const std::vector<int>& vnAuxIndicesIn, 
		bool bAccEnable);

	// activate the configured device with enabled channels
	void startAcquisition(void);

	// activate the configured device with enabled channels
	void stopAcquisition(void);

	// get data from device
	int64_t getAmpRawData(BYTE* pBuffer, int nBufferSize);

	// push it into a vector TODO: make this a template to support any buffer type
	double repackageAmpRawData(BYTE* pBuffer, int nBufferSize, int64_t nSamplesRead);// , LIVEAMP_BUFF &labOutData);
	
	// set the output trigger output mode
	void setOutTriggerMode(t_TriggerOutputMode mode, int syncPin, int freq, int pulseWidth);

	void setupLiveAmpBuffer(int nChunkSize);
	// this clears the data buffer---this must be done
	// all it does is call .clear() on the vector
	// but we put it in the class for readability
	void clearLiveAmpBuffer(void);

	// public data access methods 	
	inline float             getSamplingRate(void){return m_fSamplingRate;}
	inline HANDLE            getHandle(void) { if (h == NULL)return NULL;else return h; }
	inline std::string&      getSerialNumber(void){return m_sSerialNumber;}
	inline std::string&      getChannelUnit(int i) {return m_ciChannelInfoArray[i].sChannelUnit;}
	inline int               getAvailableChannels(void){return m_nAvailableChannels;}
	inline int               getRecordingMode(void){return m_nRecordingMode;}
	inline std::vector<int>& getEEGIndices(void){return m_vnEegIndices;}
	inline std::vector<int>& getBipolarIndices(void){return m_vnBipolarIndices;}
	inline std::vector<int>& getAuxIndices(void){return m_vnAuxIndices;}
	inline std::vector<int>& getAccIndices(void){return m_vnAccIndices;}
	inline std::vector<int>& getTrigIndices(void){return m_vnTrigIndices;}
	inline int               getTimIndex(void) {return m_nTimIndex;}
	inline int               getEnabledChannelCnt(void){return m_nEnabledChannelCnt;}
	//inline int*              getDataTypeArray(void){return dataTypeArray;}  
	inline int               getSampleSize(void){return m_nSampleSize;}
	inline int				 getUsableChannels(void){return m_nUsableChannels;}
	inline bool				 isClosed(void){return m_bIsClosed;}
	inline bool              hasSTE(void){return m_bHasSTE;}
	inline int               getSampleCnt(void){return m_labDataContainer.size();}
	inline bool              getIs64(void){return m_bIs64;}
};

#endif //LiveAmp_H