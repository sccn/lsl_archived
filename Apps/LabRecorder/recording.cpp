#include "recording.h"

#include <set>
#include <sstream>
#ifdef XDFZ_SUPPORT
#include <boost/iostreams/device/file_descriptor.hpp>
#include <boost/iostreams/filter/zlib.hpp>
#include <boost/algorithm/string/predicate.hpp>
#endif

static const std::string boundary_uuid(reinterpret_cast<const char*>(boundary_uuid_c), 16);

// Thread utilities

/**
 * @brief try_join_once		joins and deconstructs the thread if possible
 * @param thread			unique_ptr to a std::tread. Will be reset on success
 * @return					true if the thread was successfully joined, false otherwise
 */
inline bool try_join_once(std::unique_ptr<std::thread>& thread) {
	if(thread && thread->joinable()) {
		thread->join();
		thread.reset();
		return true;
	}
	return false;
}

/**
 * @brief timed_join	Tries to join the passed thread until it succeeds or duration passes
 * @param thread		unique_ptr to a std::tread. Will be reset on success
 * @param duration		max duration to try joining
 * @return true on success, false otherwise
 */
inline bool timed_join(thread_p& thread, std::chrono::milliseconds duration = max_join_wait) {
	const auto start = std::chrono::high_resolution_clock::now();
	while(std::chrono::high_resolution_clock::now() - start < duration) {
		if(try_join_once(thread)) return true;
		std::this_thread::sleep_for(std::chrono::milliseconds(500));
	}
	return false;
}

/**
 * @brief timed_join_or_detach	Join the thread or detach it if not possible within specified duration
 * @param thread				unique_ptr to a std::tread. Will be reset on success
 * @param duration max			duration to try joining
 */
inline void timed_join_or_detach(thread_p& thread, std::chrono::milliseconds duration = max_join_wait) {
	if(!timed_join(thread, duration)) {
		thread->detach();
		std::cerr << "Thread didn't join in time!" << std::endl;
	}
}

/**
 * @brief timed_join_or_detach	Join the thread or detach it if not possible within specified duration
 * @param threads				list of unique_ptrs to std::threads. Guaranteed to be empty afterwards.
 * @param duration				duration to try joining
 */
inline void timed_join_or_detach(std::list<thread_p>& threads, std::chrono::milliseconds duration = max_join_wait) {
	const auto start = std::chrono::high_resolution_clock::now();
	while(std::chrono::high_resolution_clock::now() - start < duration && !threads.empty()) {
		for(auto it = threads.begin(); it!=threads.end();) {
			if(try_join_once(*it)) it = threads.erase(it);
			else ++it;
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(500));
	}
	if(!threads.empty()) {
		std::cout << threads.size() << " stream threads still running!" << std::endl;
		for(auto& t: threads) t->detach();
		threads.clear();
	}
}

// Utilies for writing binary data to files

// write a variable-length integer (int8, int32, or int64)
inline void write_varlen_int(std::streambuf* dst, uint64_t val) {
	if (val < 256) {
		dst->sputc(1);
		dst->sputc(static_cast<uint8_t>(val));
	} else if (val <= 4294967295) {
		dst->sputc(4);
		write_little_endian(dst, static_cast<uint32_t>(val));
	} else {
		dst->sputc(8);
		write_little_endian(dst, static_cast<uint64_t>(val));
	}
}

// store a sample's values to a stream (numeric version) */
template <class T>
inline void write_sample_values(std::streambuf* dst, const std::vector<T>& sample) {
	// [Value1] .. [ValueN] */
	for (const T s : sample) write_little_endian(dst, s);
}

// store a sample's values to a stream (string version)
template <>
inline void write_sample_values(std::streambuf* dst, const std::vector<std::string>& sample) {
	// [Value1] .. [ValueN] */
	for (const std::string& s : sample) {
		// [NumLengthBytes], [Length] (as varlen int)
		write_varlen_int(dst, s.size());
		// [StringContent] */
		dst->sputn(s.data(), s.size());
	}
}



recording::recording(const std::string& filename, const std::vector<lsl::stream_info>& streams, const std::vector<std::string>& watchfor, std::map<std::string, int> syncOptions, bool collect_offsets):
    offsets_enabled_(collect_offsets),
    unsorted_(false),
    streamid_(0),
    shutdown_(false),
    headers_to_finish_(0),
    streaming_to_finish_(0),
    sync_options_by_stream_(std::move(syncOptions))
{
	// open file stream
	std::cout << "Opening file " << filename << " ... ";
#ifdef XDFZ_SUPPORT
	if (boost::iends_with(filename,".xdfz"))
		file_.push(boost::iostreams::zlib_compressor());
	file_.push(boost::iostreams::file_descriptor_sink(filename,std::ios::binary | std::ios::trunc));
#else
	file_.open(filename, std::ios::binary | std::ios::trunc);
#endif

	std::cout << "done." << std::endl;
	// [MagicCode]
	file_ << "XDF:";
	// [FileHeader] chunk
	write_chunk(ct_fileheader,"<?xml version=\"1.0\"?><info><version>1.0</version></info>");
	// create a recording thread for each stream
	for (const auto& stream: streams)
		stream_threads_.emplace_back(new std::thread(&recording::record_from_streaminfo, this, stream, true));
	// create a resolve-and-record thread for each item in the watchlist
	for (const auto& thread: watchfor)
		stream_threads_.emplace_back(new std::thread(&recording::record_from_query_results, this, thread));
	// create a boundary chunk writer thread
	boundary_thread_.reset(new std::thread(&recording::record_boundaries, this));
}

recording::~recording() {
	try {
		// set the shutdown flag (from now on no more new streams)
		shutdown_ = true;

		// stop the threads
		timed_join_or_detach(stream_threads_, max_join_wait);
		if(!timed_join(boundary_thread_, max_join_wait+boundary_interval)) {
			std::cout << "boundary_thread didn't finish in time!" << std::endl;
			boundary_thread_->detach();
		}
		std::cout << "Closing the file." << std::endl;
	}
	catch(std::exception &e) {
		std::cout << "Error while closing the recording: " << e.what() << std::endl;
	}
}

void recording::record_from_query_results(const std::string& query) {
	try {
		std::set<std::string> known_uids;		// set of previously seen stream uid's
		std::set<std::string> known_source_ids;	// set of previously seen source id's
		std::list<thread_p> threads;		    // our spawned threads
		std::cout << "Watching for a stream with properties " << query << std::endl;
		while (!shutdown_) {
			// periodically re-resolve the query
			std::vector<lsl::stream_info> results = lsl::resolve_stream(query,0,resolve_interval);
			// for each result...
			for(auto& result: results) {
				// if it is a new stream...
				if (!known_uids.count(result.uid()))
					// and doesn't have a previously seen source id...
					if (!(!result.source_id().empty() && (!known_source_ids.count(result.source_id())))) {
						std::cout << "Found a new stream named " << result.name() << ", adding it to the recording." << std::endl;
						// start a new recording thread
						threads.emplace_back(new std::thread(&recording::record_from_streaminfo, this, result, false));
						// ... and add it to the lists of known id's
						known_uids.insert(result.uid());
						if (!result.source_id().empty())
							known_source_ids.insert(result.source_id());
					}
			}
		}
		// wait for all our threads to join
		timed_join_or_detach(threads, max_join_wait);
	}
	catch(std::exception &e) {
		std::cout << "Error in the record_from_query_results thread: " << e.what() << std::endl;
	}
}

void recording::record_from_streaminfo(const lsl::stream_info& src, bool phase_locked) {
	try {
		double first_timestamp, last_timestamp;
		uint64_t sample_count;
		// obtain a fresh streamid
		streamid_t streamid = fresh_streamid();

		inlet_p in;
		lsl::stream_info info;

		// --- headers phase
		try {
			enter_headers_phase(phase_locked);

			// open an inlet to read from (and subscribe to data immediately)
			in.reset(new lsl::stream_inlet(src));
			auto it = sync_options_by_stream_.find(src.name() + " (" + src.hostname() + ")");
			if(it != sync_options_by_stream_.end())
				in->set_postprocessing(it->second);

			try {
				in->open_stream(max_open_wait);
				std::cout << "Opened the stream " << src.name() << "." << std::endl;
			} catch(lsl::timeout_error &) {
				std::cout << "Subscribing to the stream " << src.name() << " is taking relatively long; collection from this stream will be delayed." << std::endl;
			}

			// generate the [StreamHeader] chunk contents...
			std::ostringstream hdr_content;
			// [StreamId]
			write_little_endian(hdr_content.rdbuf(),streamid);
			// [Content]
			// retrieve the stream header & get its XML version
			info = in->info();
			hdr_content << info.as_xml();
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
				std::lock_guard<std::mutex> lock(offset_mut_);
				for(const offset_list::value_type pair: offset_lists_[streamid]) {
					footer << "<offset><time>" << pair.first << "</time><value>" << pair.second << "</value></offset>";
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
	catch(std::exception &e) {
		std::cout << "Error in the record_from_streaminfo thread: " << e.what() << std::endl;
	}
}

void recording::record_boundaries() {
	try {
		while (!shutdown_) {
			// sleep for the interval
			std::this_thread::sleep_for(boundary_interval);
			// write a [Boundary] chunk...
			write_chunk(ct_boundary, boundary_uuid);
		}
	}
	catch(std::exception &e) {
		std::cout << "Error in the record_boundaries thread: " << e.what() << std::endl;
	}
}

void recording::record_offsets(streamid_t streamid, const inlet_p& in, std::atomic<bool>& offset_shutdown) noexcept {
	try {
		while (!shutdown_ && !offset_shutdown) {
			// sleep for the interval
			std::this_thread::sleep_for(offset_interval);
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
			std::lock_guard<std::mutex> lock(offset_mut_);
			offset_lists_[streamid].emplace_back(now-offset,offset);
		}
	}
	catch(std::exception &e) {
		std::cout << "Error in the record_offsets thread: " << e.what() << std::endl;
	}
	std::cout << "Offsets thread is finished" << std::endl;
}

void recording::write_chunk(chunk_tag_t tag, const std::string& content) {
	// lock the file stream...
	std::lock_guard<std::mutex> lock(chunk_mut_);
	// [NumLengthBytes], [Length] (variable-length integer)
	std::size_t len = 2 + content.size();
	write_varlen_int(file_.rdbuf(),len);
	// [Tag]
	write_little_endian(file_.rdbuf(), static_cast<uint16_t>(tag));
	// [Content]
	file_ << content;
}

void recording::enter_headers_phase(bool phase_locked) {
	if (phase_locked) {
		std::lock_guard<std::mutex> lock(phase_mut_);
		headers_to_finish_++;
	}
}

void recording::leave_headers_phase(bool phase_locked) {
	if (phase_locked) {
		std::unique_lock<std::mutex> lock(phase_mut_);
		headers_to_finish_--;
		lock.unlock();
		ready_for_streaming_.notify_all();
	}
}

void recording::enter_streaming_phase(bool phase_locked) {
	if (phase_locked) {
		std::unique_lock<std::mutex> lock(phase_mut_);
		ready_for_streaming_.wait_for(lock, max_headers_wait, [this]() { return this->ready_for_streaming(); });
		streaming_to_finish_++;
	}
}

void recording::leave_streaming_phase(bool phase_locked) {
	if (phase_locked) {
		std::unique_lock<std::mutex> lock(phase_mut_);
		streaming_to_finish_--;
		lock.unlock();
		ready_for_footers_.notify_all();
	}
}

void recording::enter_footers_phase(bool phase_locked) {
	if (phase_locked) {
		std::unique_lock<std::mutex> lock(phase_mut_);
		ready_for_footers_.wait_for(lock, max_footers_wait, [this]() {return this->ready_for_footers(); });
	}
}

template <class T>
void recording::typed_transfer_loop(streamid_t streamid, double srate, const inlet_p& in,
                                    double& first_timestamp, double& last_timestamp,
                                    uint64_t& sample_count) {
	// optionally start an offset collection thread for this stream
	std::atomic<bool> offset_shutdown{false};
	thread_p offset_thread(offsets_enabled_
	                       ? new std::thread(&recording::record_offsets, this, streamid, in, std::ref(offset_shutdown))
	                       : nullptr);
	try {
		first_timestamp = -1.0;
		last_timestamp = 0.0;
		sample_count = 0;
		double sample_interval = srate ? 1.0 / srate : 0;

		// temporary data
		std::vector<std::vector<T>> chunk;
		std::vector<double> timestamps;
		while (!shutdown_) {
			// get a chunk from the stream
			if (in->pull_chunk(chunk, timestamps)) {
				if (first_timestamp == -1.0) first_timestamp = timestamps[0];
				// generate [Samples] chunk contents...
				std::ostringstream content;
				// [StreamId]
				write_little_endian(content.rdbuf(), streamid);
				// [NumSamplesBytes], [NumSamples]
				write_varlen_int(content.rdbuf(), chunk.size());
				// for each sample...
				for (std::size_t s = 0; s < chunk.size(); s++) {
					// if the time stamp can be deduced from the previous one...
					if (last_timestamp + sample_interval == timestamps[s]) {
						// [TimeStampBytes] (0 for no time stamp)
						content.rdbuf()->sputc(0);
					} else {
						// [TimeStampBytes]
						content.rdbuf()->sputc(8);
						// [TimeStamp]
						write_little_endian(content.rdbuf(), timestamps[s]);
					}
					// [Sample1] .. [SampleN]
					write_sample_values<T>(content.rdbuf(), chunk[s]);
					last_timestamp = timestamps[s];
					sample_count++;
				}
				// write the actual chunk
				write_chunk(ct_samples, content.str());
			} else
				std::this_thread::sleep_for(chunk_interval);
		}
	} catch (std::exception& e) {
		std::cerr << "Error in transfer thread: " << e.what() << std::endl;
		offset_shutdown = true;
		timed_join_or_detach(offset_thread);
		throw;
	}
	timed_join_or_detach(offset_thread);
}
