#ifndef SAMPLE_H
#define SAMPLE_H

#include "../include/lsl_cpp.h"
#include "stream_info_impl.h"
#include <boost/variant.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/serialization/split_member.hpp>
#include <boost/cstdint.hpp>
#include <vector>
#include <string>

namespace lsl {
	// used in the protocol
	const char TAG_DEDUCED_TIMESTAMP = 1;
	const char TAG_TRANSMITTED_TIMESTAMP = 2;

	/// shared pointer to a sample
	typedef boost::shared_ptr<struct sample> sample_p;

	/**
	* The sample data type.
	* Used to represent samples across the library's various buffers and can be serialized (e.g., over the network).
	*/
	struct sample {
	public:
		// time stamp of the sample
		double timestamp;
		// data held by the sample
		boost::variant<
			std::vector<float>
			,std::vector<double>
			,std::vector<std::string>
			,std::vector<boost::int32_t>
			,std::vector<boost::int16_t>
			,std::vector<boost::int8_t>
#ifndef BOOST_NO_INT64_T
			,std::vector<boost::int64_t> 
#endif
			> data;
		// pushthrough flag for this sample
		bool pushthrough;


		/**
		* Construct a sample for a given stream_info.
		*/
		sample(const stream_info &info, double timestamp_, bool pushthrough_): timestamp(timestamp_), pushthrough(pushthrough_) { reset(info.channel_format(),info.channel_count()); }

		/**
		* Construct a sample for a given stream_info_impl.
		*/
		sample(const stream_info_impl &info, double timestamp_, bool pushthrough_): timestamp(timestamp_), pushthrough(pushthrough_) { reset(info.channel_format(),info.channel_count()); }

		/**
		* Construct a new sample.
		* Allocates data of the appropriate format and size.
		*/ 
		sample(channel_format_t fmt, int channel_count, double timestamp_, bool pushthrough_): timestamp(timestamp_), pushthrough(pushthrough_) { reset(fmt,channel_count); }

		/**
		* Test for equality with another sample.
		*/
		bool operator==(const sample &rhs) { return timestamp == rhs.timestamp && data == rhs.data; }

		/**
		* Assign an array of numeric values (with type conversions), in form of a pointer.
		*/
		template<class T> sample &assign_typed_ptr(T *newdata) { boost::apply_visitor(numeric_assigner<T>(newdata),data); return *this; }

		/**
		* Assign an array of string values, in form of a pointer to string.
		* This will only succeed if the sample actually holds strings.
		*/	
		sample &assign_typed_ptr(const std::string *newdata) { 
			// check if the data is actually a string
			if (std::vector<std::string> *strvec = boost::get<std::vector<std::string> >(&data)) {
				strvec->assign(newdata,newdata+strvec->size());
			} else {
				throw std::invalid_argument("Trying to assign a string vector to a sample that is not string-formatted.");
			}
			return *this;
		}

		/**
		* Assign an untyped pointer to the sample.
		* This fails when trying to assign to a string-formatted sample.
		*/
		sample &assign_untyped_ptr(void *newdata) { boost::apply_visitor(untyped_assigner(newdata),data); return *this; }

		/**
		* Retrieve an array of numeric values (with type conversions), in form of a pointer.
		*/
		template<class T> sample &retrieve_typed_ptr(T *newdata) { boost::apply_visitor(numeric_retriever<T>(newdata),data); return *this; }

		/**
		* Retrieve an array of string values, in form of a pointer to string.
		* This will only succeed if the sample actually holds strings.
		*/	
		sample &retrieve_typed_ptr(std::string *newdata) { 
			// check if the data is actually a string
			if (std::vector<std::string> *strvec = boost::get<std::vector<std::string> >(&data)) {
				for (std::size_t k=0,s=strvec->size();k<s;k++)
					newdata[k] = (*strvec)[k];
			} else {
				throw std::invalid_argument("Trying to retrieve a string vector from a sample that is not string-formatted.");
			}
			return *this;
		}

		/**
		* Retrieve an untyped pointer from the sample.
		* This fails when trying to retrieve from a string-formatted sample.
		*/
		sample &retrieve_untyped_ptr(void *newdata) { boost::apply_visitor(untyped_retriever(newdata),data); return *this; }

		/**
		* Serialization functions.
		* These are called to serialize the sample contents over the network.
		*/
		template<class Archive> void save(Archive &ar, const unsigned int) const {
			if (timestamp == DEDUCED_TIMESTAMP) {
				// just encode that it shall be deduced at the other end
				ar & TAG_DEDUCED_TIMESTAMP;
			} else {
				// encode that the time stamp will be transmitted, and transmit it
				ar & TAG_TRANSMITTED_TIMESTAMP & timestamp;
			}
			// serialize the rest of the data
			boost::apply_visitor(serializer<Archive>(ar),data);
		}

		template<class Archive> void load(Archive &ar, const unsigned int) {
			// deserialize the tag
			char tag; ar & tag;
			if (tag == TAG_DEDUCED_TIMESTAMP) {
				// deduce the timestamp
				timestamp = DEDUCED_TIMESTAMP;
			} else {
				// read the time stamp
				ar & timestamp;
			}
			// deserialize the rest of the data
			boost::apply_visitor(serializer<Archive>(ar),data);
		}

		BOOST_SERIALIZATION_SPLIT_MEMBER()

		/**
		* Assign a test pattern to the sample.
		* This can be used to verify that the data is transferred properly.
		*/
		sample &assign_test_pattern(int offset=1) { 
			pushthrough = 1; 
			timestamp = 123456.789; 
			boost::apply_visitor(testpattern_assigner(offset),data);
			return *this;
		}

	private:

		/**
		* Initialize for a given format.
		*/
		void reset(channel_format_t fmt, int channel_count) {
			// initialize the data, according to the format & channel count
			switch (fmt) {
			case cf_float32:
				data = std::vector<float>(channel_count);
				break;
			case cf_double64:
				data = std::vector<double>(channel_count);
				break;
			case cf_string:
				data = std::vector<std::string>(channel_count);
				break;
			case cf_int32:
				data = std::vector<boost::int32_t>(channel_count);
				break;
			case cf_int16:
				data = std::vector<boost::int16_t>(channel_count);
				break;
			case cf_int8:
				data = std::vector<boost::int8_t>(channel_count);
				break;
#ifndef BOOST_NO_INT64_T
			case cf_int64:
				data = std::vector<boost::int64_t>(channel_count);
				break;
#endif
			default:
				throw std::invalid_argument("Unsupported channel format used to construct a sample.");
			}
		}

		/**
		* This is a visitor object that serializes or deserializes the data array (in a typed manner).
		*/
		template <typename Archive> class serializer: public boost::static_visitor<> {
		public:
			serializer(Archive &ar): ar_(ar) {}
			template <typename T> void operator()(T &formatted_holder) const {
				for (std::size_t k=0,s=formatted_holder.size(); k<s; k++)
					ar_ & formatted_holder[k];
			}
		private:
			Archive &ar_;
		};

		/**
		* This is a visitor object that is initialized with a pointer to some numeric values of type T.
		* When applied to the sample's variant content, it assigns the supplied data to the variant.
		* This includes type conversion or compile-time errors if that is impossible.
		*/
		template <typename T> class numeric_assigner: public boost::static_visitor<> {
		public:
			numeric_assigner(T *newdata): newdata_(newdata) {}
			template <typename U> void operator()(U &formatted_holder) const {
				for (std::size_t k=0,s=formatted_holder.size(); k<s; k++)
					formatted_holder[k] = (typename U::value_type)newdata_[k]; // type conversion happens here
			}
			void operator()(std::vector<std::string> &) const { throw std::invalid_argument("Trying to assign a numeric vector to a sample that is string-formatted."); }
		private:
			T *newdata_;
		};

		/**
		* This is a visitor object that is initialized with a pointer to some untyped data.
		* When applied to the sample's variant content, it copies it into the respective holder's memory.
		* This yields a compile-time error if the content of the sample are strings.
		*/
		class untyped_assigner: public boost::static_visitor<> {
		public:
			untyped_assigner(void *newdata): newdata_(newdata) {}
			template <typename U> void operator()(U &formatted_holder) const { memcpy(&formatted_holder[0],newdata_,formatted_holder.size()*sizeof(formatted_holder[0])); }
			void operator()(std::vector<std::string> &) const { throw std::invalid_argument("Cannot assign untyped data to a string-formatted sample."); }
		private:
			void *newdata_;
		};

		/**
		* This is a visitor object that is initialized with a pointer to some numeric values of type T.
		* When applied to the sample's variant content, it retrieves data from the variant and stores it at the pointer.
		* This includes type conversion or compile-time errors if that is impossible.
		*/
		template <typename T> class numeric_retriever: public boost::static_visitor<> {
		public:
			numeric_retriever(T *newdata): newdata_(newdata) {}
			template <typename U> void operator()(U &formatted_holder) const {
				for (std::size_t k=0,s=formatted_holder.size(); k<s; k++)
					newdata_[k] = (T)formatted_holder[k]; // type conversion happens here
			}
			void operator()(std::vector<std::string> &) const { throw std::invalid_argument("Trying to retrieve a numeric vector from a sample that is string-formatted."); }
		private:
			T *newdata_;
		};

		/**
		* This is a visitor object that is initialized with a pointer to some untyped data.
		* When applied to the sample's variant content, it copies it from the respective holder's memory to the pointer.
		* This yields a compile-time error if the content of the sample are strings.
		*/
		class untyped_retriever: public boost::static_visitor<> {
		public:
			untyped_retriever(void *newdata): newdata_(newdata) {}
			template <typename U> void operator()(U &formatted_holder) const { memcpy(newdata_,&formatted_holder[0],formatted_holder.size()*sizeof(formatted_holder[0])); }
			void operator()(std::vector<std::string> &) const { throw std::invalid_argument("Cannot retrieve untyped data from a string-formatted sample."); }
		private:
			void *newdata_;
		};

		/**
		* This is a visitor object that assigns a test pattern to the data.
		*/
		class testpattern_assigner: public boost::static_visitor<> {
		public:
			testpattern_assigner(int offset): offset_(offset) {}
			// the following are the default test patterns for various data types; covers range, sign, type, and order of the values.
			void operator()(std::vector<float> &formatted_holder) const { 
				for (int k=0,s=(int)formatted_holder.size(); k<s; k++)
					formatted_holder[k] = ((float)k + (float)offset_) * (k%2==0 ? 1 : -1);
			}
			void operator()(std::vector<double> &formatted_holder) const { 
				for (int k=0,s=(int)formatted_holder.size(); k<s; k++)
					formatted_holder[k] = (k + offset_ + 16777217) * (k%2==0 ? 1 : -1);
			}
			void operator()(std::vector<std::string> &formatted_holder) const { 
				for (int k=0,s=(int)formatted_holder.size(); k<s; k++)
					formatted_holder[k] = boost::lexical_cast<std::string>((k + 10) * (k%2==0 ? 1 : -1));
			}
			void operator()(std::vector<boost::int32_t> &formatted_holder) const { 
				for (int k=0,s=(int)formatted_holder.size(); k<s; k++)
					formatted_holder[k] = ((k + offset_ + 65537)%2147483647) * (k%2==0 ? 1 : -1);
			}
			void operator()(std::vector<boost::int16_t> &formatted_holder) const { 
				for (int k=0,s=(int)formatted_holder.size(); k<s; k++)
					formatted_holder[k] = (boost::int16_t)(((k + offset_ + 257)%32767) * (k%2==0 ? 1 : -1));
			}
			void operator()(std::vector<boost::int8_t> &formatted_holder) const { 
				for (int k=0,s=(int)formatted_holder.size(); k<s; k++)
					formatted_holder[k] = (boost::int8_t)(((k + offset_ + 1)%127) * (k%2==0 ? 1 : -1));
			}
#ifndef BOOST_NO_INT64_T
			void operator()(std::vector<boost::int64_t> &formatted_holder) const { 
				for (int k=0,s=(int)formatted_holder.size(); k<s; k++)
					formatted_holder[k] = ((2 + k + (boost::int64_t)offset_ + 2147483647)) * (boost::int64_t)(k%2==0 ? 1 : -1);
			}
#endif
		private:
			int offset_;
		};
	};

}

#endif

