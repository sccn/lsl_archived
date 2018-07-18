#pragma once

#include "conversions.h"

#include <cassert>
#include <mutex>
#include <sstream>
#include <thread>
#include <type_traits>
#include <vector>

#ifdef XDFZ_SUPPORT
#include <boost/iostreams/filtering_stream.hpp>
using outfile_t = boost::iostreams::filtering_ostream;
#else
#include <fstream>
using outfile_t = std::ofstream;
#endif

using streamid_t = uint32_t;

// the currently defined chunk tags
enum class chunk_tag_t : uint16_t {
	fileheader = 1,   // FileHeader chunk
	streamheader = 2, // StreamHeader chunk
	samples = 3,      // Samples chunk
	clockoffset = 4,  // ClockOffset chunk
	boundary = 5,     // Boundary chunk
	streamfooter = 6, // StreamFooter chunk
	undefined = 0
};

template <typename T> class PtrArrayWrapper {
	const T* ptr_;
	const std::size_t len_;

public:
	using value_type = T;
	PtrArrayWrapper(const T* ptr, std::size_t len) : ptr_(ptr), len_(len) {}
	PtrArrayWrapper(const std::vector<T>& vec) : ptr_(vec.cbegin()), len_(vec.size()) {}
	std::size_t size() const noexcept { return len_; }
	const T* cend() const noexcept { return ptr_ + len_; }
	const T* cbegin() const noexcept { return ptr_; }
	PtrArrayWrapper(const PtrArrayWrapper&) noexcept = default;
	PtrArrayWrapper(PtrArrayWrapper&&) noexcept = default;
	~PtrArrayWrapper() noexcept = default;
};

class XDFWriter {
private:
	outfile_t file_;
	void _write_chunk_header(chunk_tag_t tag, std::size_t length,
	                         const streamid_t* streamid_p = nullptr);

	std::mutex write_mut;

	template <typename T>
	void _sample_chunk_writer(streamid_t streamid, const double* timestamps, const T* chunk,
	                          std::size_t n_samples, std::size_t n_channels);

	// write a generic chunk
	void _write_chunk(chunk_tag_t tag, const std::string& content,
	                  const streamid_t* streamid_p = nullptr);

public:
	XDFWriter(const std::string& filename);

	template <typename T>
	void write_data_chunk(streamid_t streamid, const std::vector<double>& timestamps,
	                             const T* chunk, uint32_t n_samples, uint32_t n_channels);
	template <typename T>
	void write_data_chunk(streamid_t streamid, const std::vector<double>& timestamps,
	                             const std::vector<T>& chunk, uint32_t n_channels) {
		assert(timestamps.size() * n_channels == chunk.size());
		write_data_chunk(streamid, timestamps, chunk.data(), timestamps.size(), n_channels);
	}
	template <typename T>
	void write_data_chunk_nested(streamid_t streamid, const std::vector<double>& timestamps,
	                             const std::vector<std::vector<T>>& chunk);

	void write_stream_header(streamid_t streamid, const std::string& content);
	void write_stream_footer(streamid_t streamid, const std::string& content);
	void write_stream_offset(streamid_t streamid, double now, double offset);
	void write_boundary_chunk();
};

inline void write_ts(std::ostream& out, double ts) {
	// write timestamp
	if (ts == 0)
		out.put(0);
	else {
		// [TimeStampBytes]
		out.put(8);
		// [TimeStamp]
		write_little_endian(out, ts);
	}
}

template <typename T>
void XDFWriter::write_data_chunk(streamid_t streamid, const std::vector<double>& timestamps,
                                        const T* chunk, uint32_t n_samples, uint32_t n_channels) {
	/**
	  Samples data chunk: [Tag 3] [VLA ChunkLen] [StreamID] [VLA NumSamples]
	  [NumSamples x [VLA TimestampLen] [TimeStampLen]
	  [NumSamples x NumChannels Sample]
	  */
	if (n_samples == 0) return;
	if (timestamps.size() != n_samples) throw std::runtime_error("timestamp / sample count mismatch");

	// generate [Samples] chunk contents...

	auto len = 1 + sizeof(n_samples);
	std::ostringstream out;
	for (double ts: timestamps) {
		write_ts(out, ts);
		// write sample, get the current position in the chunk array back
		chunk = write_sample_values(out, chunk, n_channels);
	}
	const std::string outstr(out.str());
	len += outstr.size();

	std::lock_guard<std::mutex> lock(write_mut);
	_write_chunk_header(chunk_tag_t::samples, len, &streamid);
	write_fixlen_int(file_, n_samples);
	file_ << outstr;
}

template <typename T>
void XDFWriter::write_data_chunk_nested(streamid_t streamid, const std::vector<double>& timestamps,
                                        const std::vector<std::vector<T>>& chunk) {
	if (chunk.size() == 0) return;
	if (timestamps.size() != chunk.size()) throw std::runtime_error("timestamp / sample count mismatch");
	auto n_channels = chunk[0].size();

	// generate [Samples] chunk contents...

	auto len = 1 + sizeof(chunk.size());
	std::ostringstream out;
	auto sample_it = chunk.cbegin();
	for (double ts: timestamps) {
		assert(n_channels == sample_it->size());
		write_ts(out, ts);
		// write sample, get the current position in the chunk array back
		write_sample_values(out, sample_it->data(), n_channels);
		sample_it++;
	}
	const std::string outstr(out.str());
	len += outstr.size();

	std::lock_guard<std::mutex> lock(write_mut);
	_write_chunk_header(chunk_tag_t::samples, len, &streamid);
	write_fixlen_int(file_, chunk.size());
	file_ << outstr;
}
