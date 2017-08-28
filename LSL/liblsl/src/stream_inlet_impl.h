#ifndef STREAM_INLET_IMPL_H
#define STREAM_INLET_IMPL_H


#include<boost/bind.hpp>
#include "data_receiver.h"
#include "time_receiver.h"
#include "common.h"
#include "inlet_connection.h"
#include "info_receiver.h"
#include "time_postprocessor.h"


namespace lsl {

	/**
	* A stream inlet.
	* Inlets are used to receive streaming data (and meta-data) from the lab network.
	* This class is just a thin wrapper (or facade) around its four components to which it adds no extra functionality.
	*/
	class stream_inlet_impl {
	public:
		/**
		* Construct a new stream inlet from a resolved stream info.
		* @param info A resolved stream info object (as coming from one of the resolver functions).
		* @param max_buflen Optionally the maximum amount of data to buffer (in seconds if there is a nominal sampling rate, otherwise x 100 in samples).
		*					Recording applications want to use a fairly large buffer size here, while real-time applications want 
		*					to only buffer as much as they need to perform their next calculation.
		* @param max_chunklen Optionally the maximum size, in samples, at which chunks are transmitted 
		*					  (the default corresponds to the chunk sizes used by the sender).
		*					  Recording applications can use a generous size here (leaving it to the network how to pack things), 
		*					  while real-time applications may want a finer (perhaps 1-sample) granularity.
		* @param recover Try to silently recover lost streams that are recoverable (=those that that have a source_id set).
		*				 In all other cases (recover is false or the stream is not recoverable) a lost_error is thrown where 
		*				 indicated if the stream's source is lost (e.g., due to an app or computer crash).
		*/
		stream_inlet_impl(const stream_info_impl &info, int max_buflen=360, int max_chunklen=0, bool recover=true): conn_(info,recover), info_receiver_(conn_), time_receiver_(conn_), data_receiver_(conn_,max_buflen,max_chunklen),
			postprocessor_(boost::bind(&time_receiver::time_correction,&time_receiver_,5), 
			boost::bind(&inlet_connection::current_srate,&conn_),
			boost::bind(&time_receiver::was_reset,&time_receiver_)) 
		{
			ensure_lsl_initialized();
			conn_.engage();
		}

		/**
		* Destructor.
		* The stream will stop reading from the source if destroyed.
		*/
		~stream_inlet_impl() {
			try {
				conn_.disengage(); 
			}
			catch(std::exception &e) {
				std::cerr << "Unexpected error during inlet shutdown: " << e.what() << std::endl;
			}
			catch(...) {
				std::cerr << "Severe error during stream inlet shutdown." << std::endl;
			}
		}

		/**
		* Pull a sample from the inlet and read it into a vector of values.
		* Handles type checking & conversion and allocates the necessary memory in the vector if necessary.
		* @param sample An STL vector to hold the resulting values.
		* @param block If true, the function will block until a sample is available; otherwise, it will return 0.0 and no new data.
		* @return The capture time of the sample on the remote machine, or 0.0 if no new sample was available.
		*		   To remap this time stamp to the local clock, add the value returned by .time_correction() to it.
		*		   This is only necessary if the clocks of the source and destination machine are not synchronized to high enough precision.
		*/
		double pull_sample(std::vector<float> &data, double timeout=FOREVER) { data.resize(conn_.type_info().channel_count()); return pull_sample(&data[0],(int)data.size(),timeout); }
		double pull_sample(std::vector<double> &data, double timeout=FOREVER) { data.resize(conn_.type_info().channel_count()); return pull_sample(&data[0],(int)data.size(),timeout); }
		double pull_sample(std::vector<long> &data, double timeout=FOREVER) { data.resize(conn_.type_info().channel_count()); return pull_sample(&data[0],(int)data.size(),timeout); }
		double pull_sample(std::vector<int> &data, double timeout=FOREVER) { data.resize(conn_.type_info().channel_count()); return pull_sample(&data[0],(int)data.size(),timeout); }
		double pull_sample(std::vector<short> &data, double timeout=FOREVER) { data.resize(conn_.type_info().channel_count()); return pull_sample(&data[0],(int)data.size(),timeout); }
		double pull_sample(std::vector<char> &data, double timeout=FOREVER) { data.resize(conn_.type_info().channel_count()); return pull_sample(&data[0],(int)data.size(),timeout); }
		double pull_sample(std::vector<std::string> &data, double timeout=FOREVER) { data.resize(conn_.type_info().channel_count()); return pull_sample(&data[0],(int)data.size(),timeout); }

		/**
		* Pull a sample from the inlet and read it into a pointer to values.
		* Handles type checking & conversion.
		* @param buffer A pointer to hold the resulting values.
		* @param buffer_elements The number of samples allocated in the buffer. 
		*						 Note: it is the responsibility of the user to allocate enough memory.
		* @param block If true, the function will block until a sample is available; otherwise, it will return 0.0 and no new data.
		* @return The capture time of the sample on the remote machine, or 0.0 if no new sample was available.
		*		   To remap this time stamp to the local clock, add the value returned by .time_correction() to it.
		*		   This is only necessary if the clocks of the source and destination machine are not synchronized to high enough precision.
		*/
		double pull_sample(float *buffer, int buffer_elements, double timeout=FOREVER) { return postprocess(data_receiver_.pull_sample_typed(buffer,buffer_elements,timeout)); }
		double pull_sample(double *buffer, int buffer_elements, double timeout=FOREVER) { return postprocess(data_receiver_.pull_sample_typed(buffer,buffer_elements,timeout)); }
		double pull_sample(long *buffer, int buffer_elements, double timeout=FOREVER) { return postprocess(data_receiver_.pull_sample_typed(buffer,buffer_elements,timeout)); }
		double pull_sample(int *buffer, int buffer_elements, double timeout=FOREVER) { return postprocess(data_receiver_.pull_sample_typed(buffer,buffer_elements,timeout)); }
		double pull_sample(short *buffer, int buffer_elements, double timeout=FOREVER) { return postprocess(data_receiver_.pull_sample_typed(buffer,buffer_elements,timeout)); }
		double pull_sample(char *buffer, int buffer_elements, double timeout=FOREVER) { return postprocess(data_receiver_.pull_sample_typed(buffer,buffer_elements,timeout)); }
		double pull_sample(std::string *buffer, int buffer_elements, double timeout=FOREVER) { return postprocess(data_receiver_.pull_sample_typed(buffer,buffer_elements,timeout)); }

		/**
		* Pull a sample from the inlet and read it into a pointer to raw data.
		* No type checking or conversions are done (not recommended!). Do not use for variable-size/string-formatted streams.
		* @param buffer A pointer to hold the resulting raw sample data.
		* @param buffer_bytes The number of bytes allocated in the buffer. Note: it is the responsibility of the user to allocate enough memory.
		* @param block If true, the function will block until a sample is available; otherwise, it will return 0.0 and no data.
		* @return The capture time of the sample on the remote machine, or 0.0 if no new sample was available.
		*		   To remap this time stamp to the local clock, add the value returned by .time_correction() to it.
		*		   This is only necessary if the clocks of the source and destination machine are not synchronized to high enough precision.
		*/
		double pull_numeric_raw(void *sample, int buffer_bytes, double timeout=FOREVER) { return postprocess(data_receiver_.pull_sample_untyped(sample,buffer_bytes,timeout)); }

		/**
		* Pull a chunk of data from the inlet.
		* IMPORTANT: Note that the provided buffer size is measured in channel values (e.g., floats) rather than in samples.
		* @param data_buffer A pointer to a buffer of data values where the results shall be stored.
		* @param timestamp_buffer A pointer to a buffer of timestamp values where time stamps shall be stored. 
		*                         If this is NULL, no time stamps will be returned.
		* @param data_buffer_elements The size of the data buffer, in channel data elements (of type T). 
		*                             Must be a multiple of the stream's channel count.
		* @param timestamp_buffer_elements The size of the timestamp buffer. If a timestamp buffer is provided then this 
		*                                  must correspond to the same number of samples as data_buffer_elements.
		* @param timeout The timeout for this operation, if any. When the timeout expires, the function may return
		*                before the entire buffer is filled. The default value of 0.0 will retrieve only data 
		*                available for immediate pickup.
		* @return data_elements_written Number of channel data elements written to the data buffer.
		* @throws lost_error (if the stream source has been lost).
		*/
		template<class T> std::size_t pull_chunk_multiplexed(T *data_buffer, double *timestamp_buffer, std::size_t data_buffer_elements, std::size_t timestamp_buffer_elements, double timeout=0.0) {
			std::size_t samples_written=0, num_chans = info().channel_count(), max_samples = data_buffer_elements/num_chans;
			if (data_buffer_elements % num_chans != 0)
				throw std::runtime_error("The number of buffer elements must be a multiple of the stream's channel count.");
			if (timestamp_buffer && max_samples != timestamp_buffer_elements)
				throw std::runtime_error("The timestamp buffer must hold the same number of samples as the data buffer.");
			double end_time = timeout ? lsl_clock()+timeout : 0.0;
			for (samples_written=0; samples_written<max_samples; samples_written++) {
				if (double ts=pull_sample(&data_buffer[samples_written*num_chans],num_chans,timeout?end_time-lsl_clock():0.0)) {
					if (timestamp_buffer)
						timestamp_buffer[samples_written] = ts;
				} else
					break;
			}
			return samples_written*num_chans;
		}

		/**
		* Retrieve the complete information of the given stream, including the extended description.
		* Can be invoked at any time of the stream's lifetime.
		* @param timeout Timeout of the operation (default: no timeout).
		* @throws timeout_error (if the timeout expires), or lost_error (if the stream source has been lost).
		*/
		const stream_info_impl &info(double timeout=FOREVER) { return info_receiver_.info(timeout); }

		/**
		* Retrieve an estimated time correction offset for the given stream.
		* The first call to this function takes several msec for an initial estimate, subsequent calls are instantaneous.
		* The correction offset is periodically re-estimated in the background (once every few sec.).
		* @param remote_time The current time of the remote computer that was used to generate this time_correction. 
		* @param uncertainty. The maximum uncertainty of the given time correction.
		* @timeout Timeout for first time-correction estimate.
		* @return The time correction estimate.
		* @throws timeout_error If the initial estimate times out.
		*/
		double time_correction(double timeout=2) { return time_receiver_.time_correction(timeout); }
        double time_correction(double *remote_time, double *uncertainty, double timeout=2) { return time_receiver_.time_correction(remote_time, uncertainty, timeout); }

		/**
		* Set post-processing flags to use. By default, the inlet performs NO post-processing and returns the 
		* ground-truth time stamps, which can then be manually synchronized using time_correction(), and then 
		* smoothed/dejittered if desired. This function allows automating these two and possibly more operations.
		* Warning: when you enable this, you will no longer receive or be able to recover the original time stamps.
		* @param flags An integer that is the result of bitwise OR'ing one or more options from processing_options_t 
		*        together (e.g., post_clocksync|post_dejitter); the default is to enable all options.
		*/
		void set_postprocessing(unsigned flags=post_ALL) { postprocessor_.set_options(flags); }

		/**
		* Open a new data stream.
		* All samples pushed in at the other end from this moment onwards will be queued and
		* eventually be delivered in response to pull_sample() or pull_chunk() calls.
		* A pull call without preceding begin_feed serves as an implicit begin_feed.
		* @param timeout Optional timeout of the operation (default: no timeout).
		* @throws timeout_error (if the timeout expires), or lost_error (if the stream source has been lost).
		*/
		void open_stream(double timeout=FOREVER) { data_receiver_.open_stream(timeout); }

		/**
		* Close the current data stream.
		* All samples still buffered or in flight will be dropped and the source will halt its buffering of data for this inlet.
		* If an application stops being interested in data from a source (temporarily or not), it should call drop_stream() to not
		* pressure the source outlet to buffer unnecessarily large amounts of data (perhaps even running out of memory).
		*/
		void close_stream() { data_receiver_.close_stream(); }

		/** 
		* Query the current size of the buffer, i.e. the number of samples that are buffered.
		* Note that this value may be inaccurate and should not be relied on for program logic.
		*/
		std::size_t samples_available() { return (std::size_t)(!data_receiver_.empty()); };

		/** Query whether the clock was potentially reset since the last call to was_clock_reset().
		* This is only interesting for applications that combine multiple time_correction values to estimate clock drift
		* and which should tolerate (rare) cases where the source machine was hot-swapped or restarted.
		*/
		bool was_clock_reset() { return time_receiver_.was_reset(); }

		/// Override the half-time (forget factor) of the time-stamp smoothing.
		void smoothing_halftime(float value) { postprocessor_.smoothing_halftime(value); }

	private:
		/// post-process a time stamp
		double postprocess(double stamp) { return stamp ? postprocessor_.process_timestamp(stamp) : stamp; }

		// the inlet connection
		inlet_connection conn_;

		// the content receiver classes
		info_receiver info_receiver_;
		time_receiver time_receiver_;
		data_receiver data_receiver_;

		// class for post-processing time stamps
		time_postprocessor postprocessor_;
	};

}

#endif
