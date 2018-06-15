#ifndef RECORDING_H
#define RECORDING_H

#include <map>
#include <list>
#include <iostream>
#include <set>

#include <boost/thread.hpp>
#include <boost/thread/condition.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/iostreams/filtering_stream.hpp>
#include <boost/algorithm/string.hpp>

#include <lsl_cpp.h>

#include <boost/type_traits/is_integral.hpp>
#include <boost/type_traits/is_floating_point.hpp>

// support for endianness and binary floating-point storage
// this import scheme is part of the portable_archive code by
// christian.pfligersdorffer@eos.info (under boost license)
#if BOOST_VERSION < 103600
#include <boost/integer/endian.hpp>
#include <boost/math/fpclassify.hpp>
#elif BOOST_VERSION < 104800
#include <boost/spirit/home/support/detail/integer/endian.hpp>
#include <boost/spirit/home/support/detail/math/fpclassify.hpp>
#else
#include <boost/spirit/home/support/detail/endian/endian.hpp>
#include <boost/spirit/home/support/detail/math/fpclassify.hpp>
#endif

// namespace alias fp_classify
#if BOOST_VERSION < 103800
namespace fp = boost::math;
#else
namespace fp = boost::spirit::math;
#endif

// namespace alias endian
#if BOOST_VERSION < 104800
namespace endian = boost::detail;
#else
namespace endian = boost::spirit::detail;
#endif


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
// approx. interval between boundary chunks, in seconds
const auto boundary_interval = boost::posix_time::seconds(10);
// approx. interval between offset measurements, in seconds
const auto offset_interval = boost::posix_time::seconds(5);
// approx. interval between resolves for outstanding streams on the watchlist, in seconds
const double resolve_interval = 5;
// approx. interval between resolves for outstanding streams on the watchlist, in seconds
const boost::posix_time::milliseconds chunk_interval(500);
// maximum waiting time for moving past the headers phase while recording, in seconds
const auto max_headers_wait = boost::posix_time::seconds(10);
// maximum waiting time for moving into the footers phase while recording, in seconds
const auto max_footers_wait = boost::posix_time::seconds(2);
// maximum waiting time for subscribing to a stream, in seconds (if exceeded, stream subscription will take place later)
const double max_open_wait = 5;
// maximum time that we wait to join a thread, in seconds
const boost::posix_time::seconds max_join_wait(5);

// the signature of the boundary chunk (next chunk begins right after this)
const unsigned char boundary_uuid[] = {0x43,0xA5,0x46,0xDC,0xCB,0xF5,0x41,0x0F,0xB3,0x0E,0xD5,0x46,0x73,0x83,0xCB,0xE4};

// pointer to a thread
using thread_p = std::shared_ptr<boost::thread>;
// pointer to a stream inlet
using inlet_p = std::shared_ptr<lsl::stream_inlet>;
// a list of clock offset estimates (time,value)
using offset_list = std::list<std::pair<double,double>>;
// a map from streamid to offset_list
using offset_lists = std::map<int,offset_list>;


namespace detail{

	// === writer functions ===
	// write an integer value in little endian
	// derived from portable archive code by christian.pfligersdorffer@eos.info (under boost license)
	template <typename T> typename boost::enable_if<boost::is_integral<T> >::type write_little_endian(std::streambuf *dst,const T &t) {
		T temp;
		endian::store_little_endian<T,sizeof(T)>(&temp,t);
		dst->sputn((char*)(&temp),sizeof(temp));
	}

	// write a floating-point value in little endian
	// derived from portable archive code by christian.pfligersdorffer@eos.info (under boost license)
	template <typename T> typename boost::enable_if<boost::is_floating_point<T> >::type write_little_endian(std::streambuf *dst,const T &t) {
		using traits = typename fp::detail::fp_traits<T>::type;
		typename traits::bits bits;
		// remap to bit representation
		switch (fp::fpclassify(t)) {
			case FP_NAN: bits = traits::exponent | traits::mantissa; break;
			case FP_INFINITE: bits = traits::exponent | (t<0) * traits::sign; break;
			case FP_SUBNORMAL: assert(std::numeric_limits<T>::has_denorm);
			case FP_ZERO: // note that floats can be ±0.0
			case FP_NORMAL: traits::get_bits(t, bits); break;
			default: bits = 0; break;
		}
		write_little_endian(dst,bits);
	}

	// write a variable-length integer (int8, int32, or int64)
	inline void write_varlen_int(std::streambuf *dst, uint64_t val) {
		if (val < 256) {
			dst->sputc(1);
			dst->sputc(static_cast<uint8_t>(val));
		} else
			if (val <= 4294967295) {
				dst->sputc(4);
				write_little_endian(dst, static_cast<uint32_t>(val));
			} else {
				dst->sputc(8);
				write_little_endian(dst, static_cast<uint64_t>(val));
			}
	}

	// store a sample's values to a stream (numeric version) */
	template<class T> inline void write_sample_values(std::streambuf *dst, const std::vector<T> &sample) {
		// [Value1] .. [ValueN] */
		for(const T s: sample)
			write_little_endian(dst, s);
	}

	// store a sample's values to a stream (string version)
	template<> inline void write_sample_values(std::streambuf *dst, const std::vector<std::string> &sample) {
	// [Value1] .. [ValueN] */
		for(const std::string& s: sample) {
			// [NumLengthBytes], [Length] (as varlen int)
			write_varlen_int(dst, s.size());
			// [StringContent] */
			dst->sputn(s.data(), s.size());
		}
	}

}

using namespace detail;

//------------------------------------------------------

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
	boost::iostreams::filtering_ostream file_;	// the file output stream
	boost::mutex chunk_mut_;
	// static information
	bool offsets_enabled_;					// whether to collect time offset information alongside with the stream contents
	bool unsorted_;							// whether this file may contain unsorted chunks (e.g., of late streams)

	// streamid allocation
	uint32_t streamid_;				// the highest streamid allocated so far
	boost::mutex streamid_mut_;				// a mutex to protect the streamid

	// phase-of-recording state (headers, streaming data, or footers)
	bool shutdown_;							// whether we are trying to shut down
	uint32_t headers_to_finish_;		// the number of streams that still need to write their header (i.e., are not yet ready to write streaming content)
	uint32_t streaming_to_finish_;	// the number of streams that still need to finish the streaming phase (i.e., are not yet ready for writing their footer)
	boost::condition ready_for_streaming_;	// condition variable signaling that all streams have finished writing their headers and are now ready to write streaming content
	boost::condition ready_for_footers_;	// condition variable signaling that all streams have finished their recording jobs and are now ready to write a footer
	boost::mutex phase_mut_;				// a mutex to protect the phase state

	// data structure to collect the time offsets for every stream
	offset_lists offset_lists_;				// the clock offset lists for each stream (to be written into the footer)
	boost::mutex offset_mut_;				// a mutex to protect the offset lists



	// data for shutdown / final joining
	std::vector<thread_p> stream_threads_;	// the spawned stream handling threads
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
	void record_offsets(boost::uint32_t streamid, const inlet_p& in);


	// sample collection loop for a numeric stream
	template<class T> void typed_transfer_loop(boost::uint32_t streamid, double srate, const inlet_p& in, double &first_timestamp, double &last_timestamp, boost::uint64_t &sample_count) {
		thread_p offset_thread;
		try {
			// optionally start an offset collection thread for this stream
			if (offsets_enabled_)
				offset_thread.reset(new boost::thread(&recording::record_offsets,this,streamid,in));

			first_timestamp = -1.0;
			last_timestamp = 0.0;
			sample_count = 0;
			double sample_interval = srate ? 1.0/srate : 0;

			// temporary data
			std::vector<std::vector<T> > chunk;
			std::vector<double> timestamps;
			while (true) {
				// check for shutdown condition
				{
					boost::mutex::scoped_lock lock(phase_mut_);
					if (shutdown_)
						break;
				}

				// get a chunk from the stream
				if (in->pull_chunk(chunk,timestamps)) {
					if (first_timestamp == -1.0)
						first_timestamp = timestamps[0];
					// generate [Samples] chunk contents...
					std::ostringstream content;
					// [StreamId]
					write_little_endian(content.rdbuf(),streamid);
					// [NumSamplesBytes], [NumSamples]
					write_varlen_int(content.rdbuf(),chunk.size());
					// for each sample...
					for (std::size_t s=0;s<chunk.size();s++) {
						// if the time stamp can be deduced from the previous one...
						if (last_timestamp + sample_interval == timestamps[s]) {
							// [TimeStampBytes] (0 for no time stamp)
							content.rdbuf()->sputc(0);
						} else {
							// [TimeStampBytes]
							content.rdbuf()->sputc(8);
							// [TimeStamp]
							write_little_endian(content.rdbuf(),timestamps[s]);
						}
						// [Sample1] .. [SampleN]
						write_sample_values<T>(content.rdbuf(),chunk[s]);
						last_timestamp = timestamps[s];
						sample_count++;
					}
					// write the actual chunk
					write_chunk(ct_samples,content.str());
				} else
					boost::this_thread::sleep(chunk_interval);

			}

			// terminate the offset collection thread, too
			if (offset_thread) {
				offset_thread->interrupt();
				offset_thread->timed_join(max_join_wait);
			}
		}
		catch(std::exception &) {
			if (offset_thread) {
				offset_thread->interrupt();
				offset_thread->timed_join(max_join_wait);
			}
			throw;
		}
	}

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
	uint32_t fresh_streamid() {
		boost::mutex::scoped_lock lock(streamid_mut_);
		return ++streamid_;
	}

};

#endif
