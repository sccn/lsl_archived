#ifndef STREAM_INFO_IMPL_H
#define STREAM_INFO_IMPL_H

#include "common.h"
#include <boost/cstdint.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/bimap.hpp>
#include <boost/thread.hpp>
#include "pugixml/pugixml.hpp"

namespace lsl {

	/// shared pointer to a stream_info_impl
	typedef boost::shared_ptr<class stream_info_impl> stream_info_impl_p;

	/**
	* Actual implementation of the stream_info class.
	* The stream_info class forwards all operations to an instance of this class.
	*/
	class stream_info_impl {
		/// The query cache is a (bidirectional) mapping between query-strings and pairs of (last-use-timestamp, matching-true/false)
		typedef boost::bimap<std::string,std::pair<double,bool> > query_cache;
	public:

		/**
		* Default constructor.
		*/
		stream_info_impl();

		/**
		* Construct a new stream_info object.
		* Here is where the core format and identity information of a stream is specified.
		* @param name Name of the stream (usually a CamelCase word). Describes the device (or product series) that this stream makes available (for use by programs, experimenters, or data analysts).
		* @param type Content type of the stream. Please see https://github.com/sccn/xdf/wiki/Meta-Data (or web search for: XDF meta-data) for pre-defined content-type names, but you can also make up your own.
		* @param channel_count Number of channels in a sample. Each channel in a sample has the same format and each sample in a stream has the same number of channels.
		* @param nominal_srate The sampling rate (in Hz) advertised by the device, if regular (otherwise set to IRREGULAR_SAMPLING).
		* @param channel_format Format/type of each channel. If your channels have different formats, use the largest type that can hold them all (e.g., double) or consider supplying multiple streams.
		* @param source_id Unique identifier of the source or device if available (e.g., serial number). Allows for advanced failure recovery.
		*/
		stream_info_impl(const std::string &name, const std::string &type, int channel_count, double nominal_srate, channel_format_t channel_format, const std::string &source_id);

		/**
		* Copy constructor.
		* Needs extra handling because xml_document is non-copyable.
		*/
		stream_info_impl(const stream_info_impl &rhs);

		/**
		* Assignment operator.
		* Needs extra handling because xml_document is non-copyable.
		*/
		stream_info_impl &operator=(const stream_info_impl &rhs);

		//
		// === Protocol Support Operations ===
		//

		/**
		* Get the short-info message according to this stream_info.
		* The short-info message is a shortened xml representation of the stream_info, excluding the .desc field (which can be megabytes in size).
		* This message is sent by a stream outlet in response to a variety of queries.
		*/
		std::string to_shortinfo_message();

		/**
		* Initialize a stream_info from a short-info message .
		* This functions resets all fields of the stream_info accoridng to the message. The .desc field will be empty.
		*/
		void from_shortinfo_message(const std::string &m);

		/**
		* Get the full-info message for this stream_info.
		* This is a complete XML representation of the stream_info.
		*/
		std::string to_fullinfo_message();

		/**
		* Initialize a stream_info from a full-info message.
		* This functions resets all fields of the stream_info accoridng to the message.
		*/
		void from_fullinfo_message(const std::string &m);

		/**
		* Test whether this stream info matches the given query string.
		* The info "matches" if the given XPath 1.0 query string returns a non-empty node set.
		* @return Whether stream info is matched by the query string.
		*/
		bool matches_query(const std::string &query);


		//
		// === Data Information Getters ===
		//

		/**
		* Get the name of a stream.
		*/
		const std::string &name() const { return name_; }

		/**
		* Get the content type of the stream.
		*/
		const std::string &type() const { return type_; }

		/**
		* Get the number of channels of a stream.
		*/
		int channel_count() const { return channel_count_; }

		/**
		* Get the sampling rate of a stream (in Hz) as advertised by the device.
		*/
		double nominal_srate() const { return nominal_srate_; }

		/**
		* Get the channel format of a stream.
		*/
		channel_format_t channel_format() const { return channel_format_; }

		/**
		* Get the unique source identifier of a stream, if any.
		*/
		const std::string &source_id() const { return source_id_; }

		/**
		* Get the number of bytes per channel (returns 0 for string-typed channels).
		*/
		int channel_bytes() const {
			const int channel_format_sizes[] = {0,sizeof(float),sizeof(double),sizeof(std::string),sizeof(boost::uint32_t),sizeof(boost::uint16_t),sizeof(boost::uint8_t),8};
			return channel_format_sizes[channel_format_];
		}

		/**
		* Get the number of bytes per sample (returns 0 for string-typed channels).
		*/
		int sample_bytes() const { return channel_count_*channel_bytes(); }


		//
		// === Network Identity Information Getters/Setters ===
		//

		/**
		* Get/Set the info / protocol version used by the stream.
		*/
		int version() const { return version_; }
		void version(int v);

		/**
		* Get/Set the creation time stamp of a stream.
		* This is the time stamp (via now()) of when the stream was first created
		* (in the time domain of the providing machine).
		*/
		double created_at() const { return created_at_; }
		void created_at(double v);

		/**
		* Get/Set the UID of a stream instance (once assigned).
		* This is a unique identifier of the stream instance, and is guaranteed to be different
		* across multiple instantiations of the same stream (e.g., after a re-start).
		*/
		const std::string &uid() const { return uid_; }
		void uid(const std::string &v);

		/**
		* Get/Set the session id for the given stream.
		* The session ID is an optional human-assigned identifier of the recording session; only
		* inlets and outlets that have the same session id can be paired with each other to avoid
		* accidentally recording from an unrelated concurrent session on the same network.
		* The session id can be set via the configuration file (see Network Connectivity in the LSL wiki).
		*/
		const std::string &session_id() const { return session_id_; }
		void session_id(const std::string &v);

		/**
		* Get/Set the provider hostname for the given stream.
		*/
		const std::string &hostname() const { return hostname_; }
		void hostname(const std::string &v);

		/**
		* Get/Set the host name or IP address where the stream is hosted.
		* This may be a fully resolved address (such as testing.uscd.edu) or an IPv4 or IPv6 address in string form.
		*/
		const std::string &v4address() const { return v4address_; }
		void v4address(const std::string &v);

		/**
		* Get/Set the TCP data port where the stream is hosted (once assigned).
		* This port is internally used to obtain data and meta-data from a stream.
		*/
		int v4data_port() const { return v4data_port_; }
		void v4data_port(int v);

		/**
		* Get/Set the UDP service port where the stream is hosted (once assigned).
		* This port is internally used to obtain time correction information for a stream.
		*/
		int v4service_port() const { return v4service_port_; }
		void v4service_port(int v);

		/**
		* Get/Set the host name or IP address where the stream is hosted.
		* This may be a fully resolved address (such as testing.uscd.edu) or an IPv4 or IPv6 address in string form.
		*/
		const std::string &v6address() const { return v6address_; }
		void v6address(const std::string &v);

		/**
		* Get/Set the TCP data port where the stream is hosted (once assigned).
		* This port is internally used to obtain data and meta-data from a stream.
		*/
		int v6data_port() const { return v6data_port_; }
		void v6data_port(int v);

		/**
		* Get/Set the UDP service port where the stream is hosted (once assigned).
		* This port is internally used to obtain time correction information for a stream.
		*/
		int v6service_port() const { return v6service_port_; }
		void v6service_port(int v);

		/**
		* Get the (editable) XML description of a stream.
		*/
		pugi::xml_node desc();
		pugi::xml_node desc() const;

	protected:

		/**
		* Create and assign the XML DOM structure based on the class fields.
		*/
		void write_xml(pugi::xml_document &doc);

		/**
		* Read the class fields from an XML DOM structure.
		* @param strict Whether the interpretation of the XML contents is strict.
		*/
		void read_xml(pugi::xml_document &doc);

	private:
		// data information
		std::string name_;
		std::string type_;
		int channel_count_;
		double nominal_srate_;
		channel_format_t channel_format_;
		std::string source_id_;
		// auto-generated network information
		int version_;
		std::string v4address_;
		int v4data_port_;
		int v4service_port_;
		std::string v6address_;
		int v6data_port_;
		int v6service_port_;
		std::string uid_;
		double created_at_;
		std::string session_id_;
		std::string hostname_;
		// XML representation
		pugi::xml_document doc_;
		// cached query results
		query_cache cached_;
		boost::mutex cache_mut_;
	};


}

#endif

