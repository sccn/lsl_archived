#ifndef RDAMESSAGE_H
#define RDAMESSAGE_H

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <vector>
#include <boost/cstdint.hpp>
#include <boost/uuid/uuid.hpp>


using namespace boost;
boost::uuids::uuid RDA_UUID = { 0x8E, 0x45, 0x58, 0x43, 0x96, 0xC9, 0x86, 0x4C, 0xAF, 0x4A, 0x98, 0xBB, 0xF6, 0xC9, 0x14, 0x50};

#pragma pack(1) 
struct RDA_MessageHeader
{
	uint8_t uuid[16];				// message uuid (RDA_UUID)
	uint32_t size;					// Size of the message block in bytes including this header
	uint32_t type;					// Message type.
};
#pragma pack()

struct RDA_MessageStart
{
	uint32_t numChannels;					// number of channels
	double sampleRate;						// sampling rate in Hz
	std::vector<double> channelResolutions;	// resolution in micro volts 
	std::vector<std::string> channelLabels; // channel labels
};

struct RDA_Marker
{
	uint32_t samplePosition;				// position in samples, relative to start of current data block
	std::string type;
	std::string description;
};

class RDA_message
{
public:
	enum { header_length = sizeof(RDA_MessageHeader) };

	enum TYPE {
		START = 1,           // Setup / Start info
		DATA16 = 2,          // Block of 16-bit data
		STOP = 3,            // Data acquisition has been stopped
		DATA32 = 4,          // Block of 32-bit floating point data
		NEWSTATE = 5,        // Recorder state has been changed
		IMP_START = 6,       // Impedance measurement start
		IMP_DATA = 7,        // Impedance measurement data
		IMP_STOP = 8,        // Impedance measurement stop
		INFO = 9,            // Recorder info Header, sent after connection and when setup is changed
		KEEP_ALIVE = 10000   // Sent periodically to check whether the connection is still alive
	};


	RDA_message()
	: body_length_(0)
	{
	}

	size_t length() const
	{
		return header_length + body_length_;
	}

	void* header()
	{
		return (void*)&header_;
	}

	const std::vector<uint8_t>& body() const
	{
		return data_;
	}

	std::vector<uint8_t>& body()
	{
		return data_;
	}

	size_t body_length() const
	{
		return body_length_;
	}

	uint32_t type()
	{
		return header_.type;
	}

	bool decode_header()
	{
		body_length_ = header_.size - sizeof(RDA_MessageHeader);
		data_.resize(body_length_);
		boost::uuids::uuid u;
		memcpy(&u, header_.uuid, 16);
		if(RDA_UUID == u)
			return true;
		else
			return false;
	}

	RDA_MessageStart decode_startmessage()
	{
		RDA_MessageStart ms;
		uint8_t *data = &data_[0];
		ms.numChannels = *(uint32_t*)data;
		data += sizeof(uint32_t);
		ms.sampleRate = 1.0e6 / *(double*)data;
		data += sizeof(double);
		for( uint32_t c = 0; c < ms.numChannels; c++)
		{
			ms.channelResolutions.push_back(*(double*)data);
			data += sizeof(double);
		}
		for( uint32_t c = 0; c < ms.numChannels; c++)
		{
			ms.channelLabels.push_back(std::string((char*)data));
			data += ms.channelLabels.back().size() + 1;
		}
		return ms;
	}

	long decode_data32(std::vector<double> resolutions, std::vector<std::vector<float>>& samples, std::vector<RDA_Marker>& markers)
	{
		uint32_t numChannels = resolutions.size();
		// get data header
		uint8_t *data = &data_[0];
		uint32_t blockNumber = *(uint32_t*)data;
		data += sizeof(uint32_t);
		uint32_t numSamples = *(uint32_t*)data;
		data += sizeof(uint32_t);
		uint32_t numMarkers = *(uint32_t*)data;
		data += sizeof(uint32_t);
		// get data
		for(uint32_t sample = 0; sample < numSamples; sample++)
		{
			std::vector<float> channelData;
			for( uint32_t channel = 0; channel < numChannels; channel++)
			{
				channelData.push_back( (float)(*(float*)data * resolutions[channel]) );
				data += sizeof(float);
			}
			samples.push_back(channelData);
		}
		// get markers (if any)
		for( uint32_t marker = 0; marker < numMarkers; marker++)
		{
			RDA_Marker mkr;
			uint32_t blocksize = *(uint32_t*)data;
			size_t idx = sizeof(uint32_t);
			mkr.samplePosition = *(uint32_t*)&data[idx];
			idx += 3*sizeof(uint32_t);
			mkr.type = std::string((char*)&data[idx]);
			idx += mkr.type.size() + 1;
			mkr.description = std::string((char*)&data[idx]);
			markers.push_back(mkr);
			data += blocksize;
		}
		return (long)blockNumber;
	}


private:
	RDA_MessageHeader header_;
	std::vector<uint8_t> data_;
	size_t body_length_;
};




#endif // RDAMESSAGE_H