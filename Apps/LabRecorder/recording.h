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

#ifdef XDFZ_SUPPORT
#include <boost/iostreams/filtering_stream.hpp>
using outfile_t = boost::iostreams::filtering_ostream;
#else
#include <fstream>
using outfile_t = std::ofstream;
#endif
#include <lsl_cpp.h>

#if BOOST_VERSION >= 105800
#include <boost/endian/conversion.hpp>
using boost::endian::native_to_little_inplace;
#else
#include <boost/spirit/home/support/detail/endian/endian.hpp>
template <typename T> void native_to_little_inplace(T& t) {
	T temp;
	boost::spirit::detail::store_little_endian<T, sizeof(T)>(&temp, t);
	t = temp;
}
#endif

// support for endianness and binary floating-point storage
// this import scheme is part of the portable_archive code by
// christian.pfligersdorffer@eos.info (under boost license)
#include <boost/spirit/home/support/detail/math/fpclassify.hpp>
// namespace alias fp_classify
namespace fp = boost::spirit::math;

// the currently defined chunk tags
enum chunk_tag_t {
	ct_fileheader = 1,		// FileHeader chunk
	ct_streamheader = 2,	// StreamHeader chunk
	ct_samples = 3,			// Samples chunk
	ct_clockoffset = 4,		// ClockOffset chunk
	ct_boundary = 5,		// Boundary chunk
	ct_streamfooter = 6,	// StreamFooter chunk
	ct_undefined = 0
};

// timings in the recording process (e.g., rate of boundary chunks and for cases where a stream hangs)
// approx. interval between boundary chunks
const auto boundary_interval = std::chrono::seconds(10);
// approx. interval between offset measurements
const auto offset_interval = std::chrono::seconds(5);
// approx. interval between resolves for outstanding streams on the watchlist, in seconds
const double resolve_interval = 5;
// approx. interval between resolves for outstanding streams on the watchlist
const  auto chunk_interval = std::chrono::milliseconds(500);
// maximum waiting time for moving past the headers phase while recording
const auto max_headers_wait = std::chrono::seconds(10);
// maximum waiting time for moving into the footers phase while recording
const auto max_footers_wait = std::chrono::seconds(2);
// maximum waiting time for subscribing to a stream, in seconds (if exceeded, stream subscription will take place later)
const double max_open_wait = 5;
// maximum time that we wait to join a thread, in seconds
const std::chrono::seconds max_join_wait(5);

// the signature of the boundary chunk (next chunk begins right after this)
const uint8_t boundary_uuid_c[] = {0x43,0xA5,0x46,0xDC,0xCB,0xF5,0x41,0x0F,0xB3,0x0E,0xD5,0x46,0x73,0x83,0xCB,0xE4};

// pointer to a thread
using thread_p = std::unique_ptr<std::thread>;
// pointer to a stream inlet
using inlet_p = std::shared_ptr<lsl::stream_inlet>;
// a list of clock offset estimates (time,value)
using offset_list = std::list<std::pair<double,double>>;
// a map from streamid to offset_list
using offset_lists = std::map<int,offset_list>;

using streamid_t = uint32_t;

// === writer functions ===
// write an integer value in little endian
// derived from portable archive code by christian.pfligersdorffer@eos.info (under boost license)
template <typename T>
typename std::enable_if<std::is_integral<T>::value>::type write_little_endian(std::streambuf* dst, T t) {
	native_to_little_inplace(t);
	if(sizeof(T) == 1) dst->sputc(t);
	else dst->sputn((char*)(&t), sizeof(t));
}

// write a floating-point value in little endian
// derived from portable archive code by christian.pfligersdorffer@eos.info (under boost license)
template <typename T>
typename std::enable_if<std::is_floating_point<T>::value>::type
write_little_endian(std::streambuf* dst, T t) {
	//Get a type big enough to hold
	using traits = typename fp::detail::fp_traits<T>::type;
	static_assert(sizeof(typename traits::bits) == sizeof(T), "floating point type can't be represented accurately");

	//Just copy the value if it's in the standard IEC 559 format
	if(std::numeric_limits<T>::is_iec559)
		write_little_endian(dst, *reinterpret_cast<typename traits::bits*>(&t));
	else {
		typename traits::bits bits;
		// remap to bit representation
		switch (fp::fpclassify(t)) {
		case FP_NAN: bits = traits::exponent | traits::mantissa; break;
		case FP_INFINITE: bits = traits::exponent | (t < 0) * traits::sign; break;
		case FP_SUBNORMAL:
			static_assert(std::numeric_limits<T>::has_denorm, "denormalized floats not supported");
		case FP_ZERO: // note that floats can be ±0.0
		case FP_NORMAL: traits::get_bits(t, bits); break;
		default: bits = 0; break;
		}
		write_little_endian(dst, bits);
	}
}

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
	outfile_t file_;	// the file output stream
	std::mutex chunk_mut_;
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

	// write a generic chunk
	void write_chunk(chunk_tag_t tag, const std::string &content);

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
