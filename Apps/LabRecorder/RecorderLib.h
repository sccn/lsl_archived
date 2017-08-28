#ifndef RECORDERLIB_H
#define RECORDERLIB_H

///**
//* Recorder library API.
//*/
//
//#ifdef _WIN32
//	#ifdef RECORDERLIB_EXPORTS
//		#define RECORDERLIB_API __declspec(dllexport)
//	#else
//		#ifndef _DEBUG
//			#ifdef _WIN64
//				#pragma comment (lib,"recorderlib.lib")
//			#else
//				#pragma comment (lib,"recorderlib.lib")
//			#endif
//		#else
//			#ifdef _WIN64
//				#pragma comment (lib,"recorderlib-debug.lib")
//			#else
//				#pragma comment (lib,"recorderlib-debug.lib")
//			#endif
//		#endif
//		#define RECORDERLIB_API __declspec(dllimport)
//	#endif
//	#pragma warning (disable:4275)
//#else
//    #pragma GCC visibility push(default)
//    #define RECORDERLIB_API
//#endif


#ifdef __cplusplus
extern "C" {
#endif

/**
 * Handle to a running recording process.
 */
typedef struct rl_recording_* rl_recording;

/**
* Start recording from a given set of streams.
* This function starts a background recording process (in a separate thread). The streams to record from can be specified
* in two different ways (both can be specified simultaneously).
*  a) As an array of lsl_streaminfo handles, which can be obtained via the resolve functions of liblsl.
*  b) As an array of query strings that identify the stream(s) to resolve and add to the recording once available.
*
* @param filename Name of the file to record to (should end in .xdf).
* @param recordfrom An array of lsl_streaminfo handles that identify streams to record from. These can be obtained from the resolve functions of liblsl.
*					Note: the recording process does not acquire ownership of these objects but instead makes copies, so the user needs to destroy them 
*					to reclaim their memory.
* @param num_recordfrom Number of entries in the recordfrom array.
* @param watchfor An optional string that holds a list of query predicates in the same format as expected by lsl_resolve_bypred; the individual queries are separated 
*			      by the | character. These are XPath 1.0 predicates that determine which streams shall be recorded from; the most common examples would be:
*                 "name='BioSemi'" (record from any stream whose name is BioSemi) or "type='EEG' and count(info/desc/channel)=32" (record from any stream whose
*				  content-type is EEG and which has 32 channels.
* @param collect_offsets Whether to collect clock offset information (keep this set to 1 unless you record from LSL drivers that use custom mutually synchronized clocks, e.g. GPS clocks)
* @return A handle to the recording that can be used to stop once finished.
*/
//#ifdef _WIN32
//extern RECORDERLIB_API rl_recording rl_start_recording(char *filename, void **recordfrom, unsigned num_recordfrom, char *watchfor, int collect_offsets);
//#else
extern rl_recording rl_start_recording(char *filename, std::vector<lsl::stream_info> recordfrom, unsigned num_recordfrom, char *watchfor, int collect_offsets);
//#endif

/**
* End a recording and close the file.
* This stops a recording and deletes the associated recording object.
*/ 
//#ifdef _WIN32
//extern RECORDERLIB_API void rl_end_recording(rl_recording rec);
//#else
extern void rl_end_recording(rl_recording rec);
//#endif


#ifdef __cplusplus
} /* end extern "C" */
#endif

#ifndef _WIN32
    #pragma GCC visibility pop
#endif

#endif

