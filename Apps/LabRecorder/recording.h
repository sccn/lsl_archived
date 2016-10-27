#ifndef RECORDING_H
#define RECORDING_H

#include <fstream>
#include <map>
#include <list>
#include <iostream>
#include <set>

//#include "thread.hpp"
#include <boost/thread.hpp>
#include <boost/thread/condition.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/bind.hpp>
#include <boost/cstdint.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/iostreams/device/file_descriptor.hpp>
#include <boost/iostreams/filtering_stream.hpp>
#include <boost/iostreams/filter/zlib.hpp>
#include <boost/algorithm/string.hpp>

#include "../../LSL/liblsl/include/lsl_cpp.h"

#include <boost/type_traits/is_integral.hpp>
#include <boost/type_traits/is_signed.hpp>
#include <boost/type_traits/is_arithmetic.hpp>
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
const float boundary_interval = 10; 
// approx. interval between offset measurements, in seconds
const float offset_interval = 5;
// approx. interval between resolves for outstanding streams on the watchlist, in seconds
const float resolve_interval = 5;
// approx. interval between resolves for outstanding streams on the watchlist, in seconds
const float chunk_interval = 0.5;
// maximum waiting time for moving past the headers phase while recording, in seconds
const float max_headers_wait = 10;
// maximum waiting time for moving into the footers phase while recording, in seconds
const float max_footers_wait = 2;
// maximum waiting time for subscribing to a stream, in seconds (if exceeded, stream subscription will take place later)
const float max_open_wait = 5;
// maximum time that we wait to join a thread, in seconds
const float max_join_wait = 5;

// the signature of the boundary chunk (next chunk begins right after this)
const unsigned char boundary_uuid[] = {0x43,0xA5,0x46,0xDC,0xCB,0xF5,0x41,0x0F,0xB3,0x0E,0xD5,0x46,0x73,0x83,0xCB,0xE4};

// pointer to a thread
typedef boost::shared_ptr<boost::thread> thread_p;
// pointer to a stream inlet
typedef boost::shared_ptr<lsl::stream_inlet> inlet_p;
// a list of clock offset estimates (time,value)
typedef std::list<std::pair<double,double> > offset_list;
// a map from streamid to offset_list
typedef std::map<int,offset_list> offset_lists;

/* The following append was created by David Medine at SCCN on 2015/1/5.
 * This was done in order to make this file compliant with Standard C++.
 * The offending lines of code (which these are copies of) occur between lines
 * 529 and 598 and are commented out.
*/

//------------------------------------------------------
//begin append:
namespace detail{

	// === writer functions ===
	// write an integer value in little endian
	// derived from portable archive code by christian.pfligersdorffer@eos.info (under boost license)
	template <typename T> typename boost::enable_if<boost::is_integral<T> >::type write_little_endian(std::streambuf *dst,const T &t) {
		T temp;
		endian::store_little_endian<T,sizeof(T)>(&temp,t);
		dst->sputn((char*)&temp,sizeof(temp));
	}

	// write a floating-point value in little endian
	// derived from portable archive code by christian.pfligersdorffer@eos.info (under boost license)
	template <typename T> typename boost::enable_if<boost::is_floating_point<T> >::type write_little_endian(std::streambuf *dst,const T &t) {
		typedef typename fp::detail::fp_traits<T>::type traits;
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
	inline void write_varlen_int(std::streambuf *dst, boost::uint64_t val) {
		if (val < 256) {
			dst->sputc(1);
			dst->sputc((boost::uint8_t)val);
		} else
		if (val <= 4294967295) {
			dst->sputc(4);
			write_little_endian(dst,(boost::uint32_t)val);
		} else {
			dst->sputc(8);
			write_little_endian(dst,(boost::uint64_t)val);
		}
	}

	// store a sample's values to a stream (numeric version) */
	template<class T> inline void write_sample_values(std::streambuf *dst, const std::vector<T> &sample) {
		// [Value1] .. [ValueN] */
		for (std::size_t c=0;c<sample.size();c++)
			write_little_endian(dst,sample[c]);
	}

	// store a sample's values to a stream (string version)
	template<> inline void write_sample_values(std::streambuf *dst, const std::vector<std::string> &sample) {
	// [Value1] .. [ValueN] */
		for (std::size_t c=0;c<sample.size();c++) {
			// [NumLengthBytes], [Length] (as varlen int)
			write_varlen_int(dst,sample[c].size());
			// [StringContent] */
			dst->sputn(sample[c].data(),sample[c].size());
		}
	}

}

using namespace detail;

// end append

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
	recording(const std::string &filename, const std::vector<lsl::stream_info> &streams, const std::vector<std::string> &watchfor, const std::map<std::string, int> &syncOptions, bool collect_offsets=true): sync_options_by_stream_(syncOptions), offsets_enabled_(collect_offsets), unsorted_(false), shutdown_(false), streamid_(0), streaming_to_finish_(0), headers_to_finish_(0) {



		// open file stream
		std::cout << "Opening file " << filename << " ... ";
		if (boost::iends_with(filename,".xdfz"))
			file_.push(boost::iostreams::zlib_compressor());
		file_.push(boost::iostreams::file_descriptor_sink(filename,std::ios::binary | std::ios::trunc));
		std::cout << "done." << std::endl;
		// [MagicCode]
		file_.rdbuf()->sputn("XDF:",4);
		// [FileHeader] chunk
		write_chunk(ct_fileheader,"<?xml version=\"1.0\"?><info><version>1.0</version></info>");
		// create a recording thread for each stream
		for (std::size_t k=0;k<streams.size();k++)
			stream_threads_.push_back(thread_p(new boost::thread(&recording::record_from_streaminfo,this,streams[k],true)));
		// create a resolve-and-record thread for each item in the watchlist
		for (std::size_t k=0;k<watchfor.size();k++)			
			stream_threads_.push_back(thread_p(new boost::thread(&recording::record_from_query_results,this,watchfor[k])));
		// create a boundary chunk writer thread
		boundary_thread_.reset(new boost::thread(&recording::record_boundaries,this));
	}

	/** Destructor.
	* Stops the recording and closes the file.
	*/
	~recording() {
		try {
			// set the shutdown flag (from now on no more new streams)
			{				
				boost::mutex::scoped_lock lock(phase_mut_);
				shutdown_ = true;
			}
			// stop the Boundary writer thread
			boundary_thread_->interrupt();
			boundary_thread_->timed_join(boost::posix_time::milliseconds((boost::int64_t)(max_join_wait*1000.0)));
			// wait for all stream threads to join...
			for (std::size_t k=0;k<stream_threads_.size();k++)
				stream_threads_[k]->timed_join(boost::posix_time::milliseconds((boost::int64_t)(max_join_wait*1000.0)));
			std::cout << "Closing the file." << std::endl;
		} 
		catch(std::exception &e) {
			std::cout << "Error while closing the recording: " << e.what() << std::endl;
		}
	}

private:
	// the file stream
	boost::iostreams::filtering_ostream file_;	// the file output stream
	boost::mutex chunk_mut_;	
	// static information
	bool offsets_enabled_;					// whether to collect time offset information alongside with the stream contents
	bool unsorted_;							// whether this file may contain unsorted chunks (e.g., of late streams)

	// streamid allocation
	boost::uint32_t streamid_;				// the highest streamid allocated so far
	boost::mutex streamid_mut_;				// a mutex to protect the streamid

	// phase-of-recording state (headers, streaming data, or footers)
	bool shutdown_;							// whether we are trying to shut down
	boost::uint32_t headers_to_finish_;		// the number of streams that still need to write their header (i.e., are not yet ready to write streaming content)
	boost::uint32_t streaming_to_finish_;	// the number of streams that still need to finish the streaming phase (i.e., are not yet ready for writing their footer)
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
	void record_from_query_results(std::string query) {
		try {
			std::set<std::string> known_uids;		// set of previously seen stream uid's
			std::set<std::string> known_source_ids;	// set of previously seen source id's
			std::vector<thread_p> threads;		    // our spawned threads
			std::cout << "Watching for a stream with properties " << query << std::endl;
			while (!shutdown_) {
				// periodically re-resolve the query
				std::vector<lsl::stream_info> results = lsl::resolve_stream(query,0,resolve_interval);
				// for each result...
				for (std::size_t k=0;k<results.size();k++) {
					// if it is a new stream...
					if (!known_uids.count(results[k].uid()))
						// and doesn't have a previously seen source id...
						if (!(!results[k].source_id().empty() && known_source_ids.count(results[k].source_id()))) {
							std::cout << "Found a new stream named " << results[k].name() << ", adding it to the recording." << std::endl;
							// start a new recording thread
							threads.push_back(thread_p(new boost::thread(&recording::record_from_streaminfo,this,results[k],false)));
							// ... and add it to the lists of known id's
							known_uids.insert(results[k].uid());
							if (!results[k].source_id().empty())
								known_source_ids.insert(results[k].source_id());
						}
				}
			}			
			// wait for all our threads to join
			for (std::size_t k=0;k<threads.size();k++)
				threads[k]->timed_join(boost::posix_time::milliseconds((boost::int64_t)(max_join_wait*1000.0)));
		} 
		catch(boost::thread_interrupted &) { 
		}
		catch(std::exception &e) {
			std::cout << "Error in the record_from_query_results thread: " << e.what() << std::endl;
		}
	}

	/// record from a given stream (identified by its streaminfo)
	/// @param src the stream_info from which to record
	/// @param phase_locked whether this is a stream that is locked to the phases (1. Headers, 2. Streaming Content, 3. Footers)
	///                     Late-added streams (e.g. forgotten devices) are not phase-locked.
	void record_from_streaminfo(lsl::stream_info src, bool phase_locked) {
		try {
			double first_timestamp, last_timestamp;
			boost::uint64_t sample_count;
			// obtain a fresh streamid
			boost::uint32_t streamid = fresh_streamid();

			inlet_p in;
			lsl::stream_info info;

			// --- headers phase
			try {
				enter_headers_phase(phase_locked);

				// open an inlet to read from (and subscribe to data immediately)
				in.reset(new lsl::stream_inlet(src));
				for(std::map<std::string, int>::iterator it=sync_options_by_stream_.begin(); it!=sync_options_by_stream_.end(); ++it) {
					if(it->first.compare(src.name() + " (" + src.hostname() + ")")==0){
						/*std::cout << it->first << std::endl;
						std::cout << it->second << std::endl;*/
						in->set_postprocessing(it->second);
					}
				}

				try {
					in->open_stream(max_open_wait);
					std::cout << "Opened the stream " << src.name() << "." << std::endl;
				} catch(lsl::timeout_error &) {
					std::cout << "Subscribing to the stream " << src.name() << " is taking relatively long; collection from this stream will be delayed." << std::endl;
				}

				// retrieve the stream header & get its XML version
				info = in->info();
				std::string as_xml = info.as_xml();
				// generate the [StreamHeader] chunk contents...
				std::ostringstream hdr_content;
				// [StreamId]
				write_little_endian(hdr_content.rdbuf(),streamid); 
				// [Content]
				hdr_content.rdbuf()->sputn(&as_xml[0],as_xml.size());
				// write the actual chunk
				write_chunk(ct_streamheader,hdr_content.str());
				std::cout << "Received header for stream " << src.name() << "." << std::endl;

				leave_headers_phase(phase_locked);
			} catch(std::exception &) { 
				leave_headers_phase(phase_locked);
				throw;
			}

			// --- streaming phase
			try {
				// this waits until we are done writing all headers for the initial set of (phase-locked) streams (any streams that are discovered later, if any, will not wait)
				// we're doing this so that all headers of the initial set of streams come first, so the XDF file is properly sorted unless we discover some streams later which 
				// someone "forgot to turn on" before the recording started; in that case the file would have to be post-processed to be in properly sorted (seekable) format
				enter_streaming_phase(phase_locked);
				std::cout << "Started data collection for stream " << src.name() << "." << std::endl;

				// now write the actual sample chunks...
				switch (src.channel_format()) {
				case lsl::cf_int8:
					typed_transfer_loop<char>(streamid,info.nominal_srate(),in,first_timestamp,last_timestamp,sample_count);
					break;
				case lsl::cf_int16:
					typed_transfer_loop<boost::int16_t>(streamid,info.nominal_srate(),in,first_timestamp,last_timestamp,sample_count);
					break;
				case lsl::cf_int32:
					typed_transfer_loop<boost::int32_t>(streamid,info.nominal_srate(),in,first_timestamp,last_timestamp,sample_count);
					break;
				case lsl::cf_float32:
					typed_transfer_loop<float>(streamid,info.nominal_srate(),in,first_timestamp,last_timestamp,sample_count);
					break;	
				case lsl::cf_double64:
					typed_transfer_loop<double>(streamid,info.nominal_srate(),in,first_timestamp,last_timestamp,sample_count);
					break;
				case lsl::cf_string:
					typed_transfer_loop<std::string>(streamid,info.nominal_srate(),in,first_timestamp,last_timestamp,sample_count);
					break;
				default:
					// unsupported channel format
					throw std::runtime_error(std::string("Unsupported channel format in stream ") += src.name());
				}

				leave_streaming_phase(phase_locked);
			} 
			catch(std::exception &) {
				leave_streaming_phase(phase_locked);
				throw;
			}

			// --- footers phase
			try {
				enter_footers_phase(phase_locked);

				// now generate the [StreamFooter] contents
				std::ostringstream footer; footer.precision(16); 
				// [StreamId]
				write_little_endian(footer.rdbuf(),streamid); 
				// [Content]
				footer << "<?xml version=\"1.0\"?><info><first_timestamp>" << first_timestamp << "</first_timestamp><last_timestamp>" << last_timestamp << "</last_timestamp><sample_count>" << sample_count << "</sample_count>";
				footer << "<clock_offsets>";
				{
					// including the clock_offset list
					boost::mutex::scoped_lock lock(offset_mut_);
					for (offset_list::iterator i=offset_lists_[streamid].begin(),e=offset_lists_[streamid].end();i!=e;i++) {
						footer << "<offset><time>" << i->first << "</time><value>" << i->second << "</value></offset>";
					}
					footer << "</clock_offsets></info>";
					write_chunk(ct_streamfooter,footer.str());
				}

				std::cout << "Wrote footer for stream " << src.name() << "." << std::endl;
				leave_footers_phase(phase_locked);
			} 
			catch(std::exception &) {
				leave_footers_phase(phase_locked);
				throw;
			}
		} 
		catch(boost::thread_interrupted &) { 
		}
		catch(std::exception &e) {
			std::cout << "Error in the record_from_streaminfo thread: " << e.what() << std::endl;
		}
	}


	/// record boundary markers every few seconds
	void record_boundaries() {
		try {
			while (!shutdown_) {
				// sleep for the interval
				boost::this_thread::sleep(boost::posix_time::milliseconds((int)(boundary_interval*1000)));
				// write a [Boundary] chunk...
				write_chunk(ct_boundary, std::string((char*)&boundary_uuid[0],16));
			}
		} 
		catch(boost::thread_interrupted &) { 
		}
		catch(std::exception &e) {
			std::cout << "Error in the record_boundaries thread: " << e.what() << std::endl;
		}
	}

	// record ClockOffset chunks from a given stream
	void record_offsets(boost::uint32_t streamid, inlet_p in) {
		try {
			while (!shutdown_) {
				// sleep for the interval
				boost::this_thread::sleep(boost::posix_time::milliseconds((int)(offset_interval*1000)));
				// query the time offset
				double offset, now;
				try {
					offset = in->time_correction(2);
					now = lsl::local_clock();
				}
				catch (lsl::timeout_error &) { continue; }				
				// generate the [ClockOffset] chunk contents
				std::ostringstream content; 
				// [StreamId]
				write_little_endian(content.rdbuf(),streamid); 
				// [CollectionTime]
				write_little_endian(content.rdbuf(),now-offset);
				// [OffsetValue]
				write_little_endian(content.rdbuf(),offset);
				// write the chunk
				write_chunk(ct_clockoffset,content.str());
				// also append to the offset lists
				boost::mutex::scoped_lock lock(offset_mut_);
				offset_lists_[streamid].push_back(std::make_pair(now-offset,offset));
			}
		} 
		catch(boost::thread_interrupted &) { 
		}
		catch(std::exception &e) {
			std::cout << "Error in the record_offsets thread: " << e.what() << std::endl;
		}
	}


	// sample collection loop for a numeric stream
	template<class T> void typed_transfer_loop(boost::uint32_t streamid, double srate, inlet_p in, double &first_timestamp, double &last_timestamp, boost::uint64_t &sample_count) {
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
					boost::this_thread::sleep(boost::posix_time::milliseconds((int)(chunk_interval*1000)));

			}

			// terminate the offset collection thread, too
			if (offset_thread) {
				offset_thread->interrupt();
				offset_thread->timed_join(boost::posix_time::milliseconds((boost::int64_t)(max_join_wait*1000.0)));
			}	
		} 
		catch(std::exception &) {
			if (offset_thread) {
				offset_thread->interrupt();
				offset_thread->timed_join(boost::posix_time::milliseconds((boost::int64_t)(max_join_wait*1000.0)));
			}
			throw;
		}
	}


	/* // === writer functions === */

	/* // store a sample's values to a stream (numeric version) */
	/* template<class T> void write_sample_values(std::streambuf *dst, const std::vector<T> &sample) { */
	/* 	// [Value1] .. [ValueN] *\/ */
	/* 	for (std::size_t c=0;c<sample.size();c++) */
	/* 		write_little_endian(dst,sample[c]); */
	/* } */

	/* // store a sample's values to a stream (string version) */
	/* template<> void write_sample_values(std::streambuf *dst, const std::vector<std::string> &sample) { */
	/* // [Value1] .. [ValueN] *\/ */
	/* 	for (std::size_t c=0;c<sample.size();c++) { */
	/* 		// [NumLengthBytes], [Length] (as varlen int) */
	/* 		write_varlen_int(dst,sample[c].size()); */
	/* 		// [StringContent] *\/ */
	/* 		dst->sputn(sample[c].data(),sample[c].size()); */
	/* 	} */
	/* } */

 	// write a generic chunk
	void write_chunk(chunk_tag_t tag, const std::string &content) {
		// lock the file stream...
		boost::mutex::scoped_lock lock(chunk_mut_);
		// [NumLengthBytes], [Length] (variable-length integer)
		std::size_t len = 2 + content.size();
		write_varlen_int(file_.rdbuf(),len);
		// [Tag]
		write_little_endian(file_.rdbuf(),(boost::uint16_t)tag);
		// [Content]
		file_.rdbuf()->sputn(content.data(),content.size());
	}

	/* // write a variable-length integer (int8, int32, or int64) */
	/* void write_varlen_int(std::streambuf *dst, boost::uint64_t val) { */
	/* 	if (val < 256) { */
	/* 		dst->sputc(1); */
	/* 		dst->sputc((boost::uint8_t)val); */
	/* 	} else */
	/* 	if (val <= 4294967295) { */
	/* 		dst->sputc(4); */
	/* 		write_little_endian(dst,(boost::uint32_t)val); */
	/* 	} else { */
	/* 		dst->sputc(8); */
	/* 		write_little_endian(dst,(boost::uint64_t)val); */
	/* 	} */
	/* } */

	/* // write an integer value in little endian */
	/* // derived from portable archive code by christian.pfligersdorffer@eos.info (under boost license) */
	/* template <typename T> typename boost::enable_if<boost::is_integral<T> >::type write_little_endian(std::streambuf *dst,const T &t) { */
	/* 	T temp; */
	/* 	endian::store_little_endian<T,sizeof(T)>(&temp,t); */
	/* 	dst->sputn((char*)&temp,sizeof(temp)); */
	/* } */

	/* // write a floating-point value in little endian */
	/* // derived from portable archive code by christian.pfligersdorffer@eos.info (under boost license) */
	/* template <typename T> typename boost::enable_if<boost::is_floating_point<T> >::type write_little_endian(std::streambuf *dst,const T &t) { */
	/* 	typedef typename fp::detail::fp_traits<T>::type traits; */
	/* 	typename traits::bits bits; */
	/* 	// remap to bit representation */
	/* 	switch (fp::fpclassify(t)) { */
	/* 		case FP_NAN: bits = traits::exponent | traits::mantissa; break; */
	/* 		case FP_INFINITE: bits = traits::exponent | (t<0) * traits::sign; break; */
	/* 		case FP_SUBNORMAL: assert(std::numeric_limits<T>::has_denorm); */
	/* 		case FP_ZERO: // note that floats can be ±0.0 */
	/* 		case FP_NORMAL: traits::get_bits(t, bits); break; */
	/* 		default: bits = 0; break; */
	/* 	} */
	/* 	write_little_endian(dst,bits); */
	/* } */

	// === phase registration & condition checks ===
	// writing is coordinated across threads in three phases to keep the file chunks sorted

	void enter_headers_phase(bool phase_locked) {
		if (phase_locked) {
			boost::mutex::scoped_lock lock(phase_mut_);
			headers_to_finish_++;
		}
	}

	void leave_headers_phase(bool phase_locked) {
		if (phase_locked) {
			boost::mutex::scoped_lock lock(phase_mut_);
			headers_to_finish_--;
			lock.unlock();
			ready_for_streaming_.notify_all();
		}
	}

	void enter_streaming_phase(bool phase_locked) {
		if (phase_locked) {
			boost::mutex::scoped_lock lock(phase_mut_);
			ready_for_streaming_.timed_wait(lock, boost::posix_time::milliseconds((boost::int64_t)(max_headers_wait*1000.0)),boost::bind(&recording::ready_for_streaming,this));
			streaming_to_finish_++;
		}
	}

	void leave_streaming_phase(bool phase_locked) {
		if (phase_locked) {
			boost::mutex::scoped_lock lock(phase_mut_);
			streaming_to_finish_--;
			lock.unlock();
			ready_for_footers_.notify_all();
		}	
	}

	void enter_footers_phase(bool phase_locked) {
		if (phase_locked) {
			boost::mutex::scoped_lock lock(phase_mut_);
			ready_for_footers_.timed_wait(lock, boost::posix_time::milliseconds((boost::int64_t)(max_footers_wait*1000.0)),boost::bind(&recording::ready_for_footers,this));
		}
	}

	void leave_footers_phase(bool phase_locked) { /* noting to do */ }


	/// a condition that indicates that we're ready to write streaming content into the file
	bool ready_for_streaming() const { return headers_to_finish_ <= 0; }
	/// a condition that indicates that we're ready to write footers into the file
	bool ready_for_footers() const { return streaming_to_finish_ <= 0 && headers_to_finish_ <= 0; }

	/// allocate a fresh stream id
	boost::uint32_t fresh_streamid() {
		boost::mutex::scoped_lock lock(streamid_mut_);
		return ++streamid_;
	}

};

#endif
