#include "recording.h"

#include <boost/iostreams/device/file_descriptor.hpp>
#include <boost/iostreams/filter/zlib.hpp>


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
	if (boost::iends_with(filename,".xdfz"))
		file_.push(boost::iostreams::zlib_compressor());
	file_.push(boost::iostreams::file_descriptor_sink(filename,std::ios::binary | std::ios::trunc));
	std::cout << "done." << std::endl;
	// [MagicCode]
	file_.rdbuf()->sputn("XDF:",4);
	// [FileHeader] chunk
	write_chunk(ct_fileheader,"<?xml version=\"1.0\"?><info><version>1.0</version></info>");
	// create a recording thread for each stream
	for (const auto& stream: streams)
		stream_threads_.push_back(std::make_shared<boost::thread>(&recording::record_from_streaminfo, this, stream, true));
	// create a resolve-and-record thread for each item in the watchlist
	for (const auto& thread: watchfor)
		stream_threads_.push_back(std::make_shared<boost::thread>(&recording::record_from_query_results, this, thread));
	// create a boundary chunk writer thread
	boundary_thread_ = std::make_shared<boost::thread>(&recording::record_boundaries, this);
}

recording::~recording() {
	try {
		// set the shutdown flag (from now on no more new streams)
		{
			boost::mutex::scoped_lock lock(phase_mut_);
			shutdown_ = true;
		}
		// stop the Boundary writer thread
		boundary_thread_->interrupt();
		boundary_thread_->timed_join(max_join_wait);
		// wait for all stream threads to join...
		for(auto& thread: stream_threads_)
			thread->timed_join(max_join_wait);
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
		std::vector<thread_p> threads;		    // our spawned threads
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
						threads.push_back(std::make_shared<boost::thread>(&recording::record_from_streaminfo, this, result, false));
						// ... and add it to the lists of known id's
						known_uids.insert(result.uid());
						if (!result.source_id().empty())
							known_source_ids.insert(result.source_id());
					}
			}
		}
		// wait for all our threads to join
		for (auto& thread: threads)
			thread->timed_join(max_join_wait);
	}
	catch(boost::thread_interrupted &) {
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
		uint32_t streamid = fresh_streamid();

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
	catch(boost::thread_interrupted &) {
	}
	catch(std::exception &e) {
		std::cout << "Error in the record_from_streaminfo thread: " << e.what() << std::endl;
	}
}

void recording::record_boundaries() {
	try {
		while (!shutdown_) {
			// sleep for the interval
			boost::this_thread::sleep(boundary_interval);
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

void recording::record_offsets(uint32_t streamid, const inlet_p& in) {
	try {
		while (!shutdown_) {
			// sleep for the interval
			boost::this_thread::sleep(offset_interval);
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
			offset_lists_[streamid].emplace_back(now-offset,offset);
		}
	}
	catch(boost::thread_interrupted &) {
	}
	catch(std::exception &e) {
		std::cout << "Error in the record_offsets thread: " << e.what() << std::endl;
	}
}

void recording::write_chunk(chunk_tag_t tag, const std::string& content) {
	// lock the file stream...
	boost::mutex::scoped_lock lock(chunk_mut_);
	// [NumLengthBytes], [Length] (variable-length integer)
	std::size_t len = 2 + content.size();
	write_varlen_int(file_.rdbuf(),len);
	// [Tag]
	write_little_endian(file_.rdbuf(), static_cast<uint16_t>(tag));
	// [Content]
	file_.rdbuf()->sputn(content.data(),content.size());
}

void recording::enter_headers_phase(bool phase_locked) {
	if (phase_locked) {
		boost::mutex::scoped_lock lock(phase_mut_);
		headers_to_finish_++;
	}
}

void recording::leave_headers_phase(bool phase_locked) {
	if (phase_locked) {
		boost::mutex::scoped_lock lock(phase_mut_);
		headers_to_finish_--;
		lock.unlock();
		ready_for_streaming_.notify_all();
	}
}

void recording::enter_streaming_phase(bool phase_locked) {
	if (phase_locked) {
		boost::mutex::scoped_lock lock(phase_mut_);
		ready_for_streaming_.timed_wait(lock, max_headers_wait, [&]() { return this->ready_for_streaming(); });
		streaming_to_finish_++;
	}
}

void recording::leave_streaming_phase(bool phase_locked) {
	if (phase_locked) {
		boost::mutex::scoped_lock lock(phase_mut_);
		streaming_to_finish_--;
		lock.unlock();
		ready_for_footers_.notify_all();
	}
}

void recording::enter_footers_phase(bool phase_locked) {
	if (phase_locked) {
		boost::mutex::scoped_lock lock(phase_mut_);
		ready_for_footers_.timed_wait(lock, max_footers_wait, [&]() {return this->ready_for_footers(); });
	}
}
