#ifndef STREAM_OUTLET_IMPL_H
#define STREAM_OUTLET_IMPL_H

#include <boost/container/flat_set.hpp>
#include "tcp_server.h"
#include "send_buffer.h"
#include "common.h"
#include "api_config.h"
#include "udp_server.h"
#include "sample.h"



namespace lsl { 

	/// pointer to a thread
	typedef boost::shared_ptr<boost::thread> thread_p;
	/// pointer to an io_service
	typedef boost::shared_ptr<boost::asio::io_service> io_service_p;

	/**
	* A stream outlet.
	* Outlets are used to make streaming data (and the meta-data) available on the lab network.
	*/
	class stream_outlet_impl: public boost::noncopyable {
	public:
		/**
		* Establish a new stream outlet. This makes the stream discoverable.
		* @param info The stream information to use for creating this stream stays constant over the lifetime of the outlet.
		* @param chunk_size The preferred chunk size, in samples, at which data shall be transmitted over the network. Can be selectively overridden by the inlet.
		*					If 0 (=default), the chunk size is determined by the pushthrough flag in push_sample or push_chunk.
		* @param max_capacity The maximum number of samples buffered for unresponsive receivers. If more samples get pushed, the oldest will be dropped. 
		*					   The default is sufficient to hold a bit more than 15 minutes of data at 512Hz, while consuming not more than ca. 512MB of RAM.
		*/
		stream_outlet_impl(const stream_info_impl &info, int chunk_size=0, int max_capacity=512000);

		/**
		* Destructor.
		* The stream will no longer be discoverable after destruction and all paired inlets will stop delivering data.
		*/
		~stream_outlet_impl();


		//
		// === Pushing a sample into the outlet. ===
		//

		/**
		* Push a std vector of values as a sample into the outlet. 
		* Each entry in the vector corresponds to one channel. The function handles type checking & conversion.
		* @param data A vector of values to push (one for each channel).
		* @param timestamp Optionally the capture time of the sample, in agreement with local_clock(); if omitted, the current time is assumed.
		* @param pushthrough Whether to push the sample through to the receivers instead of buffering it into a chunk according to network speeds.
		*/
		void push_sample(const std::vector<float> &data, double timestamp=0.0, bool pushthrough=true) { check_numchan((int)data.size()); enqueue(&data[0],timestamp,pushthrough); }
		void push_sample(const std::vector<double> &data, double timestamp=0.0, bool pushthrough=true) { check_numchan((int)data.size()); enqueue(&data[0],timestamp,pushthrough); }
		void push_sample(const std::vector<long> &data, double timestamp=0.0, bool pushthrough=true) { check_numchan((int)data.size()); enqueue(&data[0],timestamp,pushthrough); }
		void push_sample(const std::vector<int> &data, double timestamp=0.0, bool pushthrough=true) { check_numchan((int)data.size()); enqueue(&data[0],timestamp,pushthrough); }
		void push_sample(const std::vector<short> &data, double timestamp=0.0, bool pushthrough=true) { check_numchan((int)data.size()); enqueue(&data[0],timestamp,pushthrough); }
		void push_sample(const std::vector<char> &data, double timestamp=0.0, bool pushthrough=true) { check_numchan((int)data.size()); enqueue(&data[0],timestamp,pushthrough); }
		void push_sample(const std::vector<std::string> &data, double timestamp=0.0, bool pushthrough=true) { check_numchan((int)data.size()); enqueue(&data[0],timestamp,pushthrough); }

		/**
		* Push a pointer to some values as a sample into the outlet.
		* This is a lower-level function when data is available in some buffer.
		* Handles type checking & conversion. The user needs to ensure that he/she has as many values as channels in the pointed-to location.
		* @param data A pointer to values to push. The number of values pointed to must not be less than the number of channels in the sample.
		* @param timestamp Optionally the capture time of the sample, in agreement with lsl_clock(); if omitted, the current time is assumed.
		* @param pushthrough Whether to push the sample through to the receivers instead of buffering it into a chunk according to network speeds.
		*/
		void push_sample(const float *data, double timestamp=0.0, bool pushthrough=true) { enqueue(data,timestamp,pushthrough); }
		void push_sample(const double *data, double timestamp=0.0, bool pushthrough=true) { enqueue(data,timestamp,pushthrough); }
		void push_sample(const long *data, double timestamp=0.0, bool pushthrough=true) { enqueue(data,timestamp,pushthrough); }
		void push_sample(const int *data, double timestamp=0.0, bool pushthrough=true) { enqueue(data,timestamp,pushthrough); }
		void push_sample(const short *data, double timestamp=0.0, bool pushthrough=true) { enqueue(data,timestamp,pushthrough); }
		void push_sample(const char *data, double timestamp=0.0, bool pushthrough=true) { enqueue(data,timestamp,pushthrough); }
		void push_sample(const std::string *data, double timestamp=0.0, bool pushthrough=true) { enqueue(data,timestamp,pushthrough); }

		/**
		* Push a pointer to raw numeric data as one sample into the outlet. 
		* This is the lowest-level function; does no checking of any kind. Do not use for variable-size/string data-formatted channels.
		* @param sample A pointer to the raw sample data to push.
		* @param timestamp Optionally the capture time of the sample, in agreement with lsl_clock(); if omitted, the current time is assumed.
		* @param pushthrough Whether to push the sample through to the receivers instead of buffering it into a chunk according to network speeds.
		*/
		void push_numeric_raw(void *data, double timestamp=0.0, bool pushthrough=true) { 
			if (lsl::api_config::get_instance()->force_default_timestamps())
				timestamp = 0.0;
			sample_p smp(sample_factory_->new_sample(timestamp == 0.0 ? lsl_clock() : timestamp, pushthrough));
			smp->assign_untyped(data);
			send_buffer_->push_sample(smp);
		}

		//
		// === Pushing an chunk of samples into the outlet ===
		//

		/**
		* Push a chunk of multiplexed samples into the send buffer. One timestamp per sample is provided.
		* IMPORTANT: Note that the provided buffer size is measured in channel values (e.g., floats) rather than in samples.
		* @param data_buffer A buffer of channel values holding the data for zero or more successive samples to send.
		* @param timestamp_buffer A buffer of timestamp values holding time stamps for each sample in the data buffer.
		* @param data_buffer_elements The number of data values (of type T) in the data buffer. Must be a multiple of the channel count.
		* @param pushthrough Whether to push the chunk through to the receivers instead of buffering it with subsequent samples.
		*					 Note that the chunk_size, if specified at outlet construction, takes precedence over the pushthrough flag.
		*/
		template<class T> void push_chunk_multiplexed(const T *data_buffer, const double *timestamp_buffer, std::size_t data_buffer_elements, bool pushthrough=true) {
			std::size_t num_chans = info().channel_count(), num_samples = data_buffer_elements/num_chans;
			if (data_buffer_elements % num_chans != 0)
				throw std::runtime_error("The number of buffer elements to send is not a multiple of the stream's channel count.");
			if (!data_buffer)
				throw std::runtime_error("The data buffer pointer must not be NULL.");
			if (!timestamp_buffer)
				throw std::runtime_error("The timestamp buffer pointer must not be NULL.");
			for (std::size_t k=0; k<num_samples; k++)
				enqueue(&data_buffer[k*num_chans],timestamp_buffer[k],pushthrough && k==num_samples-1);
		}

		/**
		* Push a chunk of multiplexed samples into the send buffer. Single timestamp provided.
		* IMPORTANT: Note that the provided buffer size is measured in channel values (e.g., floats) rather than in samples.
		* @param buffer A buffer of channel values holding the data for zero or more successive samples to send.
		* @param buffer_elements The number of channel values (of type T) in the buffer. Must be a multiple of the channel count.
		* @param timestamp Optionally the capture time of the most recent sample, in agreement with lsl_clock(); if omitted, the current time is used.
		*					The time stamps of other samples are automatically derived based on the sampling rate of the stream.
		* @param pushthrough Whether to push the chunk through to the receivers instead of buffering it with subsequent samples.
		*					 Note that the chunk_size, if specified at outlet construction, takes precedence over the pushthrough flag.
		*/
		template<class T> void push_chunk_multiplexed(const T *buffer, std::size_t buffer_elements, double timestamp=0.0, bool pushthrough=true) {
			std::size_t num_chans = info().channel_count(), num_samples = buffer_elements/num_chans;
			if (buffer_elements % num_chans != 0)
				throw std::runtime_error("The number of buffer elements to send is not a multiple of the stream's channel count.");
			if (!buffer)
				throw std::runtime_error("The number of buffer elements to send is not a multiple of the stream's channel count.");
			if (num_samples > 0) {
				if (timestamp == 0.0)
					timestamp = lsl_clock();
				if (info().nominal_srate() != IRREGULAR_RATE)
					timestamp = timestamp - (num_samples-1)/info().nominal_srate();
				push_sample(buffer,timestamp,pushthrough && (num_samples==1));
				for (std::size_t k=1; k<num_samples; k++)
					push_sample(&buffer[k*num_chans],DEDUCED_TIMESTAMP,pushthrough && (k==num_samples-1));
			}
		}

		//
		// === Misc Features ===
		//

		/**
		* Retrieve the stream info associated with this outlet.
		* This is the constant meta-data header that was used to create the stream.
		*/ 
		const stream_info_impl &info() const;
		/**
		* Check whether consumers are currently registered.
		*/
		bool have_consumers();

		/**
		* Wait until some consumer shows up.
		*/
		bool wait_for_consumers(double timeout=FOREVER);

	private:
		/**
		* Instantiate a new server stack.
		*/
		void instantiate_stack(tcp tcp_protocol, udp udp_protocol);

		/// Run the given io_service
		void run_io(io_service_p &ios);

		/**
		* Allocate and enqueue a new sample into the send buffer.
		*/
		template<class T> void enqueue(T* data, double timestamp, bool pushthrough) { 
			if (lsl::api_config::get_instance()->force_default_timestamps())
				timestamp = 0.0;
			sample_p smp(sample_factory_->new_sample(timestamp == 0.0 ? lsl_clock() : timestamp, pushthrough));
			smp->assign_typed(data);
			send_buffer_->push_sample(smp);
		}

		/**
		* Check whether some given number of channels matches the stream's channel_count.
		* Throws an error if not.
		*/ 
		void check_numchan(int chns) {
			if (chns != info_->channel_count())
				throw std::range_error("The provided sample data has a different length than the stream's number of channels.");	
		}

		sample::factory_p sample_factory_;			// a factory for samples of appropriate type
		int chunk_size_;							// the preferred chunk size
		stream_info_impl_p info_;					// stream_info shared between the various server instances
		send_buffer_p send_buffer_;					// the single-producer, multiple-receiver send buffer
		std::vector<io_service_p> ios_;				// the IO service objects (two per stack: one for UDP and one for TCP)

		std::vector<tcp_server_p> tcp_servers_;		// the threaded TCP data server(s); two if using both IP stacks
		std::vector<udp_server_p> udp_servers_;		// the UDP timing & ident service(s); two if using both IP stacks
		std::vector<udp_server_p> responders_;		// UDP multicast responders for service discovery (time features disabled); also using only the allowed IP stacks
		std::vector<thread_p> io_threads_;			// threads that handle the I/O operations (two per stack: one for UDP and one for TCP)
	};

}

#endif

