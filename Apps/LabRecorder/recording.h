#ifndef RECORDING_H
#define RECORDING_H

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <iostream>
#include <list>
#include <map>
#include <mutex>
#include <thread>
#include <type_traits>
#include <lsl_cpp.h>
#include "xdfwriter.h"

// timings in the recording process (e.g., rate of boundary chunks and for cases where a stream hangs)
// approx. interval between boundary chunks
const auto boundary_interval = std::chrono::seconds(10);
// approx. interval between offset measurements
const auto offset_interval = std::chrono::seconds(5);
// approx. interval between resolves for outstanding streams on the watchlist, in seconds
const double resolve_interval = 5;
// approx. interval between pulling chunks from outlets
const  auto chunk_interval = std::chrono::milliseconds(500);
// maximum waiting time for moving past the headers phase while recording
const auto max_headers_wait = std::chrono::seconds(10);
// maximum waiting time for moving into the footers phase while recording
const auto max_footers_wait = std::chrono::seconds(2);
// maximum waiting time for subscribing to a stream, in seconds (if exceeded, stream subscription will take place later)
const double max_open_wait = 5;
// maximum time that we wait to join a thread, in seconds
const std::chrono::seconds max_join_wait(5);

using streamid_t = uint32_t;

// pointer to a thread
using thread_p = std::unique_ptr<std::thread>;
// pointer to a stream inlet
using inlet_p = std::shared_ptr<lsl::stream_inlet>;
// a list of clock offset estimates (time,value)
using offset_list = std::list<std::pair<double,double>>;
// a map from streamid to offset_list
using offset_lists = std::map<streamid_t, offset_list>;


/**
* A recording process using the lab streaming layer.
* An instance of this class is created with a list of stream references to record from.
* Upon construction, a file is created and a recording thread is spawned which records
* data until the instance is destroyed.
*/
class recording {
public:
	/**
	* Construct a new background recording process.
	* @param filename The file name to record to (should end in .xdf).
	* @param streams  An array of LSL streaminfo's that identify the set of streams to record into the file.
	* @param watchfor An optional "watchlist" of LSL query predicates (see lsl::resolve_bypred) to resolve streams to record from.
	*		          This can be a specific stream that you know should be recorded but is not yet online, or a more generic query
	*				  (e.g., "record from everything that's out there").
	* @param collect_offsets Whether to collect time offset measurements periodically.
	*/
	recording(const std::string &filename,
			  const std::vector<lsl::stream_info> &streams,
			  const std::vector<std::string> &watchfor,
			  std::map<std::string, int> syncOptions,
			  bool collect_offsets=true);

	/** Destructor.
	* Stops the recording and closes the file.
	*/
	~recording();

private:
	// the file stream
	XDFWriter file_;	// the file output stream
	// static information
	bool offsets_enabled_;					// whether to collect time offset information alongside with the stream contents
	bool unsorted_;							// whether this file may contain unsorted chunks (e.g., of late streams)

	// streamid allocation
	std::atomic<streamid_t> streamid_;				// the highest streamid allocated so far

	// phase-of-recording state (headers, streaming data, or footers)
	std::atomic<bool> shutdown_;			// whether we are trying to shut down
	uint32_t headers_to_finish_;		// the number of streams that still need to write their header (i.e., are not yet ready to write streaming content)
	uint32_t streaming_to_finish_;	// the number of streams that still need to finish the streaming phase (i.e., are not yet ready for writing their footer)
	std::condition_variable ready_for_streaming_;	// condition variable signaling that all streams have finished writing their headers and are now ready to write streaming content
	std::condition_variable ready_for_footers_;		// condition variable signaling that all streams have finished their recording jobs and are now ready to write a footer
	std::mutex phase_mut_;				// a mutex to protect the phase state

	// data structure to collect the time offsets for every stream
	offset_lists offset_lists_;				// the clock offset lists for each stream (to be written into the footer)
	std::mutex offset_mut_;				// a mutex to protect the offset lists



	// data for shutdown / final joining
	std::list<thread_p> stream_threads_;	// the spawned stream handling threads
	thread_p boundary_thread_;				// the spawned boundary-recording thread

	// for enabling online sync options
	std::map<std::string, int> sync_options_by_stream_;

	// === recording thread functions ===

	/// record from results of a query (spawn a recording thread for every result produced by the query)
	/// @param query The query string
	void record_from_query_results(const std::string& query);

	/// record from a given stream (identified by its streaminfo)
	/// @param src the stream_info from which to record
	/// @param phase_locked whether this is a stream that is locked to the phases (1. Headers, 2. Streaming Content, 3. Footers)
	///                     Late-added streams (e.g. forgotten devices) are not phase-locked.
	void record_from_streaminfo(const lsl::stream_info& src, bool phase_locked);


	/// record boundary markers every few seconds
	void record_boundaries();

	// record ClockOffset chunks from a given stream
	void record_offsets(streamid_t streamid, const inlet_p& in, std::atomic<bool>& offset_shutdown) noexcept;


	// sample collection loop for a numeric stream
	template<class T> void typed_transfer_loop(streamid_t streamid, double srate, const inlet_p& in, double &first_timestamp, double &last_timestamp, uint64_t &sample_count);

	// === phase registration & condition checks ===
	// writing is coordinated across threads in three phases to keep the file chunks sorted

	void enter_headers_phase(bool phase_locked);

	void leave_headers_phase(bool phase_locked);

	void enter_streaming_phase(bool phase_locked);

	void leave_streaming_phase(bool phase_locked);

	void enter_footers_phase(bool phase_locked);

	void leave_footers_phase(bool) { /* Nothing to do. Ignore warning. */ }

	/// a condition that indicates that we're ready to write streaming content into the file
	bool ready_for_streaming() const { return headers_to_finish_ <= 0; }
	/// a condition that indicates that we're ready to write footers into the file
	bool ready_for_footers() const { return streaming_to_finish_ <= 0 && headers_to_finish_ <= 0; }

	/// allocate a fresh stream id
	streamid_t fresh_streamid() {
		return ++streamid_;
	}

};

#endif
