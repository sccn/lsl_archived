#ifndef LSL_CPP_H
#define LSL_CPP_H
#include "../common.h"

/**
* Header file for the legacy C++ ABI (application binary interface). This ABI has been superseded by 
* one that wraps the C ABI, and therefore is compatible across more compiler versions, e.g., MSVC20XX.
*/

#include <string>
#include <vector>
#include <stdexcept>

#ifndef _WIN32
#pragma GCC visibility push(default)
#endif

namespace pugi { class xml_node; struct xml_node_struct; }
namespace lsl {
	
	/**
	* Protocol version.
	* The major version is protocol_version() / 100;
	* The minor version is protocol_version() % 100;
	* Clients with different minor versions are protocol-compatible with each other 
	* while clients with different major versions will refuse to work together.
	*/
	LIBLSL_CPP_API int protocol_version();

	/**
	* Version of the liblsl library.
	* The major version is library_version() / 100;
	* The minor version is library_version() % 100;
	*/
	LIBLSL_CPP_API int library_version();

	/**
	* Obtain a local system time stamp in seconds. The resolution is better than a millisecond.
	* This reading can be used to assign time stamps to samples as they are being acquired. 
	* If the "age" of a sample is known at a particular time (e.g., from USB transmission 
	* delays), it can be used as an offset to local_clock() to obtain a better estimate of 
	* when a sample was actually captured. See stream_outlet::push_sample() for a use case.
	*/
	LIBLSL_CPP_API double local_clock();




	// ==========================
	// === Stream Declaration ===
	// ==========================

	/**
	* The stream_info object stores the declaration of a data stream.
	* Represents the following information:
	*  a) stream data format (#channels, channel format)
	*  b) core information (stream name, content type, sampling rate)
	*  c) optional meta-data about the stream content (channel labels, measurement units, etc.)
	*
	* Whenever a program wants to provide a new stream on the lab network it will typically first 
	* create a stream_info to describe its properties and then construct a stream_outlet with it to create
	* the stream on the network. Recipients who discover the outlet can query the stream_info; it is also
	* written to disk when recording the stream (playing a similar role as a file header).
	*/
	class LIBLSL_CPP_API xml_element;
	class LIBLSL_CPP_API stream_info {
	public:
		/**
		* Construct a new stream_info object.
		* Core stream information is specified here. Any remaining meta-data can be added later.
		* @param name Name of the stream. Describes the device (or product series) that this stream makes available 
		*			  (for use by programs, experimenters or data analysts). Cannot be empty.
		* @param type Content type of the stream. Please see https://github.com/sccn/xdf/wiki/Meta-Data (or web search for:
		*			  XDF meta-data) for pre-defined content-type names, but you can also make up your own.
		*			  The content type is the preferred way to find streams (as opposed to searching by name).
		* @param channel_count Number of channels per sample. This stays constant for the lifetime of the stream.
		* @param nominal_srate The sampling rate (in Hz) as advertised by the data source, if regular (otherwise set to IRREGULAR_RATE).
		* @param channel_format Format/type of each channel. If your channels have different formats, consider supplying 
		*                       multiple streams or use the largest type that can hold them all (such as cf_double64).
		* @param source_id Unique identifier of the device or source of the data, if available (such as the serial number). 
		*                  This is critical for system robustness since it allows recipients to recover from failure even after the 
		*				   serving app, device or computer crashes (just by finding a stream with the same source id on the network again).
		*				   Therefore, it is highly recommended to always try to provide whatever information can uniquely identify the data source itself.
		*/
		stream_info(const std::string &name, const std::string &type, int channel_count=1, double nominal_srate=IRREGULAR_RATE, channel_format_t channel_format=cf_float32, const std::string &source_id=std::string());

		// ========================
		// === Core Information ===
		// ========================
		// (these fields are assigned at construction)

		/**
		* Name of the stream.
		* This is a human-readable name. For streams offered by device modules, it refers to the type of device or product series 
		* that is generating the data of the stream. If the source is an application, the name may be a more generic or specific identifier.
		* Multiple streams with the same name can coexist, though potentially at the cost of ambiguity (for the recording app or experimenter).
		*/
		const std::string &name() const;

		/**
		* Content type of the stream.
		* The content type is a short string such as "EEG", "Gaze" which describes the content carried by the channel (if known). 
		* If a stream contains mixed content this value need not be assigned but may instead be stored in the description of channel types.
		* To be useful to applications and automated processing systems using the recommended content types is preferred. 
		* Content types usually follow those pre-defined in https://github.com/sccn/xdf/wiki/Meta-Data (or web search for: XDF meta-data).
		*/
		const std::string &type() const;

		/**
		* Number of channels of the stream.
		* A stream has at least one channel; the channel count stays constant for all samples.
		*/
		int channel_count() const;

		/**
		* Sampling rate of the stream, according to the source (in Hz).
		* If a stream is irregularly sampled, this should be set to IRREGULAR_RATE.
		*
		* Note that no data will be lost even if this sampling rate is incorrect or if a device has temporary 
		* hiccups, since all samples will be recorded anyway (except for those dropped by the device itself). However, 
		* when the recording is imported into an application, a good importer may correct such errors more accurately 
		* if the advertised sampling rate was close to the specs of the device.
		*/
		double nominal_srate() const;

		/**
		* Channel format of the stream.
		* All channels in a stream have the same format. However, a device might offer multiple time-synched streams 
		* each with its own format.
		*/
		channel_format_t channel_format() const;

		/**
		* Unique identifier of the stream's source, if available.
		* The unique source (or device) identifier is an optional piece of information that, if available, allows that
		* endpoints (such as the recording program) can re-acquire a stream automatically once it is back online.
		*/
		const std::string &source_id() const;


		// ======================================
		// === Additional Hosting Information ===
		// ======================================
		// (these fields are implicitly assigned once bound to an outlet/inlet)

		/**
		* Protocol version used to deliver the stream.
		*/
		int version() const;

		/**
		* Creation time stamp of the stream.
		* This is the time stamp when the stream was first created
		* (as determined via local_clock() on the providing machine).
		*/
		double created_at() const;

		/**
		* Unique ID of the stream outlet instance (once assigned).
		* This is a unique identifier of the stream outlet, and is guaranteed to be different
		* across multiple instantiations of the same outlet (e.g., after a re-start).
		*/
		const std::string &uid() const;

		/**
		* Session ID for the given stream.
		* The session id is an optional human-assigned identifier of the recording session.
		* While it is rarely used, it can be used to prevent concurrent recording activitites 
		* on the same sub-network (e.g., in multiple experiment areas) from seeing each other's streams 
		* (assigned via a configuration file by the experimenter, see Network Connectivity in the LSL wiki).
		*/
		const std::string &session_id() const;

		/**
		* Hostname of the providing machine.
		*/
		const std::string &hostname() const;


		// ========================
		// === Data Description ===
		// ========================

		/**
		* Extended description of the stream.
		* It is highly recommended that at least the channel labels are described here. 
		* See code examples on the LSL wiki. Other information, such as amplifier settings, 
		* measurement units if deviating from defaults, setup information, subject information, etc., 
		* can be specified here, as well. Meta-Data recommendations follow the XDF file format project
		* (github.com/sccn/xdf/wiki/Meta-Data or web search for: XDF meta-data).
		*
		* Important: if you use a stream content type for which meta-data recommendations exist, please 
		* try to lay out your meta-data in agreement with these recommendations for compatibility with other applications.
		*/
		xml_element desc();
		xml_element desc() const;


		// ===============================
		// === Miscellaneous Functions ===
		// ===============================

		/**
		* Retrieve the entire stream_info in XML format.
		* This yields an XML document (in string form) whose top-level element is <info>. The info element contains
		* one element for each field of the stream_info class, including:
		*  a) the core elements <name>, <type>, <channel_count>, <nominal_srate>, <channel_format>, <source_id>
		*  b) the misc elements <version>, <created_at>, <uid>, <session_id>, <v4address>, <v4data_port>, <v4service_port>, <v6address>, <v6data_port>, <v6service_port>
		*  c) the extended description element <desc> with user-defined sub-elements.
		*/
		std::string as_xml();

		/// Number of bytes occupied by a channel (0 for string-typed channels).
		int channel_bytes() const;

		/// Number of bytes occupied by a sample (0 for string-typed channels).
		int sample_bytes() const;

		/// Get the implementation handle.
		class stream_info_impl *impl();
		const class stream_info_impl *impl() const;

		/// Construct from implementation handle.
		stream_info(const class stream_info_impl *impl);
		stream_info(const class stream_info_impl &impl);

		/// Default contructor.
		stream_info();

		/// Copy constructor.
		stream_info(const stream_info &rhs);

		/// Assignment operator.
		stream_info &operator=(const stream_info &rhs);

		/// Destructor.
		~stream_info();
	private:
		class stream_info_impl *impl_;
	};



	// =======================
	// ==== Stream Outlet ====
	// =======================

	/**
	* A stream outlet.
	* Outlets are used to make streaming data (and the meta-data) available on the lab network.
	*/
	class LIBLSL_CPP_API stream_outlet {
	public:
		/**
		* Establish a new stream outlet. This makes the stream discoverable.
		* @param info The stream information to use for creating this stream. Stays constant over the lifetime of the outlet.
		* @param chunk_size Optionally the desired chunk granularity (in samples) for transmission. If unspecified, 
		*				    each push operation yields one chunk. Inlets can override this setting.
		* @param max_buffered Optionally the maximum amount of data to buffer (in seconds if there is a nominal 
		*                     sampling rate, otherwise x100 in samples). The default is 6 minutes of data. 
		*/
		stream_outlet(const stream_info &info, int chunk_size=0, int max_buffered=360);


		// ========================================
		// === Pushing a sample into the outlet ===
		// ========================================

		/**
		* Push a std vector of values as a sample into the outlet. 
		* Each entry in the vector corresponds to one channel. The function handles type checking & conversion.
		* @param data A vector of values to push (one for each channel).
		* @param timestamp Optionally the capture time of the sample, in agreement with local_clock(); if omitted, the current time is used.
		* @param pushthrough Whether to push the sample through to the receivers instead of buffering it with subsequent samples.
		*					 Note that the chunk_size, if specified at outlet construction, takes precedence over the pushthrough flag.
		*/
		void push_sample(const std::vector<float> &data, double timestamp=0.0, bool pushthrough=true);
		void push_sample(const std::vector<double> &data, double timestamp=0.0, bool pushthrough=true);
		void push_sample(const std::vector<long> &data, double timestamp=0.0, bool pushthrough=true);
		void push_sample(const std::vector<int> &data, double timestamp=0.0, bool pushthrough=true);
		void push_sample(const std::vector<short> &data, double timestamp=0.0, bool pushthrough=true);
		void push_sample(const std::vector<char> &data, double timestamp=0.0, bool pushthrough=true);
		void push_sample(const std::vector<std::string> &data, double timestamp=0.0, bool pushthrough=true);

		/**
		* Push a pointer to some values as a sample into the outlet.
		* This is a lower-level function for cases where data is available in some buffer.
		* Handles type checking & conversion.
		* @param data A pointer to values to push. The number of values pointed to must not be less than the number of channels in the sample.
		* @param timestamp Optionally the capture time of the sample, in agreement with local_clock(); if omitted, the current time is used.
		* @param pushthrough Whether to push the sample through to the receivers instead of buffering it with subsequent samples.
		*					 Note that the chunk_size, if specified at outlet construction, takes precedence over the pushthrough flag.
		*/
		void push_sample(const float *data, double timestamp=0.0, bool pushthrough=true);
		void push_sample(const double *data, double timestamp=0.0, bool pushthrough=true);
		void push_sample(const long *data, double timestamp=0.0, bool pushthrough=true);
		void push_sample(const int *data, double timestamp=0.0, bool pushthrough=true);
		void push_sample(const short *data, double timestamp=0.0, bool pushthrough=true);
		void push_sample(const char *data, double timestamp=0.0, bool pushthrough=true);
		void push_sample(const std::string *data, double timestamp=0.0, bool pushthrough=true);

		/**
		* Push a pointer to raw numeric data as one sample into the outlet. 
		* This is the lowest-level function; performns no checking whatsoever. Can not be used for variable-size / string-formatted channels.
		* @param sample A pointer to the raw sample data to push.
		* @param timestamp Optionally the capture time of the sample, in agreement with local_clock(); if omitted, the current time is used.
		* @param pushthrough Whether to push the sample through to the receivers instead of buffering it with subsequent samples.
		*					 Note that the chunk_size, if specified at outlet construction, takes precedence over the pushthrough flag.
		*/
		void push_numeric_raw(void *sample, double timestamp=0.0, bool pushthrough=true);


		// ===============================
		// === Miscellaneous Functions ===
		// ===============================

		/**
		* Check whether consumers are currently registered.
		* While it does not hurt, there is technically no reason to push samples if there is no consumer.
		*/
		bool have_consumers();

		/**
		* Wait until some consumer shows up (without wasting resources).
		* @return True if the wait was successful, false if the timeout expired.
		*/
		bool wait_for_consumers(double timeout);

		/**
		* Retrieve the stream info provided by this outlet.
		* This is what was used to create the stream (and also has the Additional Network Information fields assigned).
		*/ 
		stream_info info() const;

		/**
		* Destructor.
		* The stream will no longer be discoverable after destruction and all paired inlets will stop delivering data.
		*/
		~stream_outlet();

	private:
		// The outlet is a non-copyable object.
		stream_outlet(const stream_outlet &rhs);
		stream_outlet &operator=(const stream_outlet &rhs);

		class stream_outlet_impl *impl_;
	};


	// ===========================
	// ==== Resolve Functions ====
	// ===========================

	/**
	* Resolve all streams on the network.
	* This function returns all currently available streams from any outlet on the network.
	* The network is usually the subnet specified at the local router, but may also include 
	* a multicast group of machines (given that the network supports it), or list of hostnames.
	* These details may optionally be customized by the experimenter in a configuration file 
	* (see Network Connectivity in the LSL wiki).
	* This is the default mechanism used by the browsing programs and the recording program.
	* @param wait_time The waiting time for the operation, in seconds, to search for streams.
	*				   Warning: If this is too short (<0.5s) only a subset (or none) of the 
	*							outlets that are present on the network may be returned.
	* @return A vector of stream info objects (excluding their desc field), any of which can 
	*         subsequently be used to open an inlet. The full info can be retrieve from the inlet.
	*/
	LIBLSL_CPP_API std::vector<stream_info> resolve_streams(double wait_time=1.0);

	/**
	* Resolve all streams with a specific value for a given property.
	* If the goal is to resolve a specific stream, this method is preferred over resolving all streams and then selecting the desired one.
	* @param prop The stream_info property that should have a specific value (e.g., "name", "type", "source_id", or "desc/manufaturer").
	* @param value The string value that the property should have (e.g., "EEG" as the type property).
	* @param minimum Return at least this number of streams.
	* @param timeout Optionally a timeout of the operation, in seconds (default: no timeout).
	*				  If the timeout expires, less than the desired number of streams (possibly none) will be returned.
	* @return A vector of matching stream info objects (excluding their meta-data), any of 
	*         which can subsequently be used to open an inlet.
	*/
	LIBLSL_CPP_API std::vector<stream_info> resolve_stream(const std::string &prop, const std::string &value, int minimum=1, double timeout=FOREVER);

	/**
	* Resolve all streams that match a given predicate.
	* Advanced query that allows to impose more conditions on the retrieved streams; the given string is an XPath 1.0 
	* predicate for the <info> node (omitting the surrounding []'s), see also http://en.wikipedia.org/w/index.php?title=XPath_1.0&oldid=474981951.
	* @param pred The predicate string, e.g. "name='BioSemi'" or "type='EEG' and starts-with(name,'BioSemi') and count(info/desc/channel)=32"
	* @param minimum Return at least this number of streams.
	* @param timeout Optionally a timeout of the operation, in seconds (default: no timeout).
	*				  If the timeout expires, less than the desired number of streams (possibly none) will be returned.
	* @return A vector of matching stream info objects (excluding their meta-data), any of 
	*         which can subsequently be used to open an inlet.
	*/
	LIBLSL_CPP_API std::vector<stream_info> resolve_stream(const std::string &pred, int minimum=1, double timeout=FOREVER);


	/** 
	* A convenience class that resolves streams continuously in the background throughout 
	* its lifetime and which can be queried at any time for the set of streams that are currently 
	* visible on the network.
	*/
	class LIBLSL_CPP_API continuous_resolver {
	public:
		/**
		* Construct a new continuous_resolver that resolves all streams on the network. 
		* This is analogous to the functionality offered by the free function resolve_streams().
		* @param forget_after When a stream is no longer visible on the network (e.g., because it was shut down),
		*				      this is the time in seconds after which it is no longer reported by the resolver.
		*/
		continuous_resolver(double forget_after=5.0);

		/**
		* Construct a new continuous_resolver that resolves all streams with a specific value for a given property.
		* This is analogous to the functionality provided by the free function resolve_stream(prop,value).
		* @param prop The stream_info property that should have a specific value (e.g., "name", "type", "source_id", or "desc/manufaturer").
		* @param value The string value that the property should have (e.g., "EEG" as the type property).
		* @param forget_after When a stream is no longer visible on the network (e.g., because it was shut down),
		*				      this is the time in seconds after which it is no longer reported by the resolver.
		*/
		continuous_resolver(const std::string &prop, const std::string &value, double forget_after=5.0);

		/**
		* Construct a new continuous_resolver that resolves all streams that match a given XPath 1.0 predicate.
		* This is analogous to the functionality provided by the free function resolve_stream(pred).
		* @param pred The predicate string, e.g. "name='BioSemi'" or "type='EEG' and starts-with(name,'BioSemi') and count(info/desc/channel)=32"
		* @param forget_after When a stream is no longer visible on the network (e.g., because it was shut down),
		*				      this is the time in seconds after which it is no longer reported by the resolver.
		*/
		continuous_resolver(const std::string &pred, double forget_after=5.0);

		/**
		* Obtain the set of currently present streams on the network (i.e. resolve result).
		* @return A vector of matching stream info objects (excluding their meta-data), any of 
		*         which can subsequently be used to open an inlet.
		*/
		std::vector<stream_info> results();

		/** 
		* Destructor.
		*/
		~continuous_resolver();

	private:
		class resolver_impl *impl_;
	};


	// ======================
	// ==== Stream Inlet ====
	// ======================

	/**
	* A stream inlet.
	* Inlets are used to receive streaming data (and meta-data) from the lab network.
	*/
	class LIBLSL_CPP_API stream_inlet {
	public:
		/**
		* Construct a new stream inlet from a resolved stream info.
		* @param info A resolved stream info object (as coming from one of the resolver functions).
		*		      Note: the stream_inlet may also be constructed with a fully-specified stream_info, 
		*					if the desired channel format and count is already known up-front, but this is 
		*					strongly discouraged and should only ever be done if there is no time to resolve the 
		*					stream up-front (e.g., due to limitations in the client program).
		* @param max_buflen Optionally the maximum amount of data to buffer (in seconds if there is a nominal 
		*					sampling rate, otherwise x100 in samples). Recording applications want to use a fairly 
		*					large buffer size here, while real-time applications would only buffer as much as 
		*					they need to perform their next calculation.
		* @param max_chunklen Optionally the maximum size, in samples, at which chunks are transmitted 
		*					  (the default corresponds to the chunk sizes used by the sender).
		*				      Recording applications can use a generous size here (leaving it to the network how 
		*					  to pack things), while real-time applications may want a finer (perhaps 1-sample) granularity.
		                      If left unspecified (=0), the sender determines the chunk granularity.
		* @param recover Try to silently recover lost streams that are recoverable (=those that that have a source_id set). 
		*				 In all other cases (recover is false or the stream is not recoverable) functions may throw a 
		*				 lost_error if the stream's source is lost (e.g., due to an app or computer crash).
		*/
		stream_inlet(const stream_info &info, int max_buflen=360, int max_chunklen=0, bool recover=true);

		/** 
		* Destructor.
		* The inlet will automatically disconnect if destroyed.
		*/
		~stream_inlet();

		/**
		* Retrieve the complete information of the given stream, including the extended description.
		* Can be invoked at any time of the stream's lifetime.
		* @param timeout Timeout of the operation (default: no timeout).
		* @throws timeout_error (if the timeout expires), or lost_error (if the stream source has been lost).
		*/
		stream_info info(double timeout=FOREVER);

		/**
		* Subscribe to the data stream.
		* All samples pushed in at the other end from this moment onwards will be queued and 
		* eventually be delivered in response to pull_sample() or pull_chunk() calls. 
		* Pulling a sample without some preceding open_stream is permitted (the stream will then be opened implicitly).
		* @param timeout Optional timeout of the operation (default: no timeout).
		* @throws timeout_error (if the timeout expires), or lost_error (if the stream source has been lost).
		*/
		void open_stream(double timeout=FOREVER);

		/**
		* Drop the current data stream.
		* All samples that are still buffered or in flight will be dropped and transmission 
		* and buffering of data for this inlet will be stopped. If an application stops being 
		* interested in data from a source (temporarily or not) but keeps the outlet alive, 
		* it should call close_stream() to not waste unnecessary system and network 
		* resources.
		*/
		void close_stream();

		/**
		* Retrieve an estimated time correction offset for the given stream.
		* The first call to this function takes several milliseconds until a reliable first estimate is obtained.
		* Subsequent calls are instantaneous (and rely on periodic background updates).
		* The precision of these estimates should be below 1 ms (empirically within +/-0.2 ms).
		* @timeout Timeout to acquire the first time-correction estimate (default: no timeout).
		* @return The time correction estimate. This is the number that needs to be added to a time stamp 
		*		  that was remotely generated via lsl_local_clock() to map it into the local clock domain of this machine.
		* @throws timeout_error (if the timeout expires), or lost_error (if the stream source has been lost).
		*/
		double time_correction(double timeout=FOREVER);


		// =======================================
		// === Pulling a sample from the inlet ===
		// =======================================

		/**
		* Pull a sample from the inlet and read it into a std vector of values.
		* Handles type checking & conversion and allocates the necessary memory in the vector if necessary.
		* @param sample An STL vector to hold the resulting values.
		* @param timeout The timeout for this operation, if any.
		* @return The capture time of the sample on the remote machine, or 0.0 if no new sample was available. 
		*		   To remap this time stamp to the local clock, add the value returned by .time_correction() to it. 
		* @throws lost_error (if the stream source has been lost).
		*/
		double pull_sample(std::vector<float> &sample, double timeout=FOREVER);
		double pull_sample(std::vector<double> &sample, double timeout=FOREVER);
		double pull_sample(std::vector<long> &sample, double timeout=FOREVER);
		double pull_sample(std::vector<int> &sample, double timeout=FOREVER);
		double pull_sample(std::vector<short> &sample, double timeout=FOREVER);
		double pull_sample(std::vector<char> &sample, double timeout=FOREVER);
		double pull_sample(std::vector<std::string> &sample, double timeout=FOREVER);

		/**
		* Pull a sample from the inlet and read it into a pointer to values.
		* Handles type checking & conversion.
		* @param buffer A pointer to hold the resulting values. 
		* @param buffer_elements The number of samples allocated in the buffer. Note: it is the responsibility of the user to allocate enough memory.
		* @param timeout The timeout for this operation, if any.
		* @return The capture time of the sample on the remote machine, or 0.0 if no new sample was available. 
		*		   To remap this time stamp to the local clock, add the value returned by .time_correction() to it. 
		* @throws lost_error (if the stream source has been lost).
		*/
		double pull_sample(float *buffer, int buffer_elements, double timeout=FOREVER);
		double pull_sample(double *buffer, int buffer_elements, double timeout=FOREVER);
		double pull_sample(long *buffer, int buffer_elements, double timeout=FOREVER);
		double pull_sample(int *buffer, int buffer_elements, double timeout=FOREVER);
		double pull_sample(short *buffer, int buffer_elements, double timeout=FOREVER);
		double pull_sample(char *buffer, int buffer_elements, double timeout=FOREVER);
		double pull_sample(std::string *buffer, int buffer_elements, double timeout=FOREVER);

		/**
		* Pull a sample from the inlet and read it into a pointer to raw data. 
		* No type checking or conversions are done (not recommended!). Do not use for variable-size/string-formatted streams.
		* @param buffer A pointer to hold the resulting raw sample data.
		* @param buffer_bytes The number of bytes allocated in the buffer. Note: it is the responsibility of the user to allocate enough memory.
		* @param timeout The timeout for this operation, if any.
		* @return The capture time of the sample on the remote machine, or 0.0 if no new sample was available. 
		*		   To remap this time stamp to the local clock, add the value returned by .time_correction() to it. 
		* @throws lost_error (if the stream source has been lost).
		*/
		double pull_numeric_raw(void *sample, int buffer_bytes, double timeout=FOREVER);

		/**
		* Query whether samples are currently available for immediate pickup.
		* Note that it is not a good idea to use samples_available() to determine whether 
		* a pull_*() call would block: to be sure, set the pull timeout to 0.0 or an acceptably
		* low value. If the underlying implementation supports it, the value will be the number of 
		* samples available (otherwise it will be 1 or 0).
		*/
		std::size_t samples_available();

		/**
		* Query whether the clock was potentially reset since the last call to was_clock_reset().
		* This is a rarely-used function that is only useful to applications that combine multiple time_correction 
		* values to estimate precise clock drift; it allows to tolerate cases where the source machine was 
		* hot-swapped or restarted in between two measurements.
		*/
		bool was_clock_reset();
	private:
		// The inlet is a non-copyable object.
		stream_inlet(const stream_inlet &rhs);
		stream_inlet &operator=(const stream_inlet &rhs);

		class stream_inlet_impl *impl_;
	};


	// =====================
	// ==== XML Element ====
	// =====================

	/**
	* A lightweight XML element tree; models the .desc() field of stream_info.
	* Has a name and can have multiple named children or have text content as value; attributes are omitted.
	* Insider note: The interface is modeled after a subset of pugixml's node type and is compatible with it.
	* See also http://pugixml.googlecode.com/svn/tags/latest/docs/manual/access.html for additional documentation.
	*/
	class LIBLSL_CPP_API xml_element {
	public:
		/// Constructor.
		xml_element();
		xml_element(const pugi::xml_node &node);


		// === Tree Navigation ===

		/// Get the first child of the element.
		xml_element first_child() const;

		/// Get the last child of the element.
		xml_element last_child() const;

		/// Get the next sibling in the children list of the parent node.
		xml_element next_sibling() const;

		/// Get the previous sibling in the children list of the parent node.
		xml_element previous_sibling() const;

		/// Get the parent node.
		xml_element parent() const;


		// === Tree Navigation by Name ===

		/// Get a child with a specified name.
		xml_element child(const char *name) const;

		/// Get the next sibling with the specified name.
		xml_element next_sibling(const char *name) const;

		/// Get the previous sibling with the specified name.
		xml_element previous_sibling(const char *name) const;


		// === Content Queries ===

		/// Whether this node is empty.
		bool empty() const;

		/// Whether this is a text body (instead of an XML element). True both for plain char data and CData.
		bool is_text() const;

		/// Name of the element.
		const char *name() const;

		/// Value of the element.
		const char *value() const;

		/// Get child value (value of the first child that is text).
		const char* child_value() const;

		/// Get child value of a child with a specified name.
		const char* child_value(const char *name) const;


		// === Modification ===

		/**
		* Append a child node with a given name, which has a (nameless) plain-text child with the given text value.
		*/
		xml_element append_child_value(const char *name, const char *value);

		/**
		* Prepend a child node with a given name, which has a (nameless) plain-text child with the given text value.
		*/
		xml_element prepend_child_value(const char *name, const char *value);

		/**
		* Set the text value of the (nameless) plain-text child of a named child node.
		*/
		bool set_child_value(const char *name, const char *value);

		/**
		* Set the element's name.
		* @return False if the node is empty (or if out of memory).
		*/
		bool set_name(const char *rhs);

		/**
		* Set the element's value.
		* @return False if the node is empty (or if out of memory).
		*/
		bool set_value(const char *rhs);

		/// Append a child element with the specified name.
		xml_element append_child(const char *name);

		/// Prepend a child element with the specified name.
		xml_element prepend_child(const char *name);

		/// Append a copy of the specified element as a child.
		xml_element append_copy(const xml_element &e);

		/// Prepend a child element with the specified name.
		xml_element prepend_copy(const xml_element &e);

		/// Remove a child element with the specified name.
		void remove_child(const char *name);

		/// Remove a specified child element.
		void remove_child(const xml_element &e);

		/// Get access to the internal pointer
		pugi::xml_node_struct *ptr();
	private:
		pugi::xml_node_struct *node_;
	};

} // end namespace

#ifndef _WIN32
    #pragma GCC visibility pop
#endif

#endif // LSL_CPP_H

