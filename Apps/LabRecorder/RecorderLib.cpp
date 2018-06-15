#ifdef _WIN32
#pragma once
#include "targetver.h"
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

#include "RecorderLib.h"
#include "../../LSL/liblsl/include/lsl_c.h"
#include "recording.h"
#include <boost/algorithm/string.hpp>

//#ifdef _WIN32
//RECORDERLIB_API rl_recording rl_start_recording(char *filename, void **recordfrom, unsigned num_recordfrom, char *watchforc, int collect_offsets) {
//#else
rl_recording rl_start_recording(char *filename, std::vector<lsl::stream_info>recordfrom, unsigned num_recordfrom, char *watchforc, int collect_offsets) {
//#endif
	// first remap the C API streaminfo array to a vector<lsl::streaminfo>
	//std::vector<lsl::stream_info> streams;
	//for (std::size_t k=0; k<num_recordfrom; k++)
	//	streams.push_back(lsl::stream_info((lsl_streaminfo)recordfrom[k]));

	// then parse split the watchfor string
	std::vector<std::string> watchfor;
	boost::algorithm::split(watchfor,watchforc,boost::algorithm::is_any_of("|"));
	for (std::size_t k=0; k<watchfor.size(); k++)
		boost::algorithm::trim(watchfor[k]);

	// lastly instantiate a new recording
	recording *result = new recording(filename,streams,watchfor,collect_offsets != 0);
	return (rl_recording)result;
}

//#ifdef _WIN32
//RECORDERLIB_API void rl_end_recording(rl_recording rec) { delete (recording*)rec; }
//#else
void rl_end_recording(rl_recording rec) { delete (recording*)rec; }
//#endif


//#ifdef _WIN32
//BOOL APIENTRY DllMain( HMODULE hModule,
//					  DWORD  ul_reason_for_call,
//					  LPVOID lpReserved
//					  )
//{
//	switch (ul_reason_for_call)
//	{
//	case DLL_PROCESS_ATTACH:
//	case DLL_THREAD_ATTACH:
//	case DLL_THREAD_DETACH:
//	case DLL_PROCESS_DETACH:
//		break;
//	}
//	return TRUE;
//}
//#endif
// .so (shared object) and .dylib (Mac's dynamic library) don't use a DLLMain