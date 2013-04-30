#ifndef STREAM_OUTLET_IMPL_H
#define STREAM_OUTLET_IMPL_H

#include "version.h"
#include "../include/lsl_cpp.h"
#include "api_config.h"
#include "tcp_server.h"
#include "udp_server.h"
#include "sample.h"
#include "send_buffer.h"


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
		* @param min_capacity The minimum number of samples reserved in the buffer. If less than this many samples have to be buffered, no unnecessary buffer reallocations will be made.
		*/
		stream_outlet_impl(const stream_info_impl &info, int chunk_size=0, int max_capacity=512000, int min_capacity=32768);

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
		* @param timestamp Optionally the capture time of the sample, in agreement with local_clock(); if omitted, the current time is assumed.
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
		* @param timestamp Optionally the capture time of the sample, in agreement with local_clock(); if omitted, the current time is assumed.
		* @param pushthrough Whether to push the sample through to the receivers instead of buffering it into a chunk according to network speeds.
		*/
		void push_numeric_raw(void *data, double timestamp=0.0, bool pushthrough=true) { 
			sample_p smp(new sample(*info_, timestamp==0.0 ? local_clock() : timestamp, pushthrough));
			smp->assign_untyped_ptr(data);
			send_buffer_->push_sample(smp);
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
			sample_p smp(new sample(*info_, timestamp==0.0 ? local_clock() : timestamp, pushthrough));
			smp->assign_typed_ptr(data);
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

