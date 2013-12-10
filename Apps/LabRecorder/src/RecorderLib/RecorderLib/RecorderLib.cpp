#pragma once
#include "targetver.h"
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "RecorderLib.h"
#include "lsl_c.h"
#include "recording.h"
#include <boost/algorithm/string.hpp>


RECORDERLIB_API rl_recording rl_start_recording(char *filename, void **recordfrom, unsigned num_recordfrom, char *watchforc, int collect_offsets) {
	// first remap the C API streaminfo array to a vector<lsl::streaminfo>
	std::vector<lsl::stream_info> streams;
	for (std::size_t k=0; k<num_recordfrom; k++)
		streams.push_back(lsl::stream_info((lsl::stream_info_impl*)recordfrom[k]));

	// then parse split the watchfor string
	std::vector<std::string> watchfor;
	boost::algorithm::split(watchfor,watchforc,boost::algorithm::is_any_of("|"));
	for (std::size_t k=0; k<watchfor.size(); k++)
		boost::algorithm::trim(watchfor[k]);

	// lastly instantiate a new recording
	recording *result = new recording(filename,streams,watchfor,collect_offsets != 0);
	return (rl_recording)result;
}


RECORDERLIB_API void rl_end_recording(rl_recording rec) { delete (recording*)rec; }




BOOL APIENTRY DllMain( HMODULE hModule,
					  DWORD  ul_reason_for_call,
					  LPVOID lpReserved
					  )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

