#ifndef SAMPLE_H
#define SAMPLE_H

#include <boost/lexical_cast.hpp>
#include <vector>
#include <string>
#include <iostream>
#include <boost/type_traits.hpp>
#include <boost/cstdint.hpp>
#include <boost/smart_ptr.hpp>
#include <boost/intrusive_ptr.hpp>
#include <boost/atomic.hpp>
#include <boost/static_assert.hpp>
#include <boost/serialization/split_member.hpp>
#include "endian/conversion.hpp"
#include "common.h"

namespace lsl {
	// if you get an error here your machine cannot represent the double-precision time-stamp format required by LSL
	BOOST_STATIC_ASSERT(sizeof(double)==8);

	// constants used in the network protocol
	const boost::uint8_t TAG_DEDUCED_TIMESTAMP = 1;
	const boost::uint8_t TAG_TRANSMITTED_TIMESTAMP = 2;

	/// channel format properties
	const int format_sizes[] = {0,sizeof(float),sizeof(double),sizeof(std::string),sizeof(boost::int32_t),sizeof(boost::int16_t),sizeof(boost::int8_t),8};
	const bool format_ieee754[] = {false,std::numeric_limits<float>::is_iec559,std::numeric_limits<double>::is_iec559,false,false,false,false,false}; 
	const bool format_subnormal[] = {false,std::numeric_limits<float>::has_denorm!=std::denorm_absent,std::numeric_limits<double>::has_denorm!=std::denorm_absent,false,false,false,false,false}; 
	const bool format_integral[] = {false,false,false,false,true,true,true,true}; 
	const bool format_float[] = {false,true,true,false,false,false,false,false}; 
 
	/// smart pointer to a sample
	typedef boost::intrusive_ptr<class sample> sample_p;
	
	/**
	* The sample data type.
	* Used to represent samples across the library's various buffers and can be serialized (e.g., over the network).
	*/
	class sample {
	public:
		class factory;					// samples can only be created by the factory
		double timestamp;				// time-stamp of the sample
		bool pushthrough;				// whether the sample shall be buffered or pushed through

	private:
		channel_format_t format_;		// the channel format
		int num_channels_;				// number of channels
		boost::atomic<int> refcount_;	// reference count used by sample_p
		boost::atomic<sample*> next_;	// linked list of samples, for use in a freelist
		factory *factory_;				// the factory used to reclaim this sample, if any
		char data_;						// the data payload begins here

	public:
		// === Construction ===

		// shared pointer to a sample factory
		typedef boost::shared_ptr<factory> factory_p;

		/// A factory to create samples of a given format/size.
		/// Must outlive all of its created samples.
		class factory {
		public:
			/// Create a new factory and optionally pre-allocate samples.
			factory(channel_format_t fmt, int num_chans, int num_reserve): fmt_(fmt), num_chans_(num_chans), 
				sample_size_(ensure_multiple(sizeof(sample)-sizeof(char)+format_sizes[fmt]*num_chans,16)), storage_size_(sample_size_*std::max(1,num_reserve)), 
				storage_(new char[storage_size_]), sentinel_(new_sample_unmanaged(fmt,num_chans,0.0,false)), head_(sentinel_), tail_(sentinel_)
			{
				// pre-construct an array of samples in the storage area and chain into a freelist
				sample *s = NULL;
				for (char *p=storage_.get(),*e=p+storage_size_;p<e;) {
					s = new((sample*)p) sample(fmt,num_chans,this);
					s->next_ = (sample*)(p += sample_size_);
				}
				s->next_ = NULL;
				head_.store(s);
				sentinel_->next_ = (sample*)storage_.get();
			}

			/// Destroy the factory and delete all of its samples.
			~factory() {
				if (sample *cur = head_)
					for (sample *next=cur->next_;next;cur=next,next=next->next_)
						delete cur;
				delete sentinel_;
			}

			/// Create a new sample with a given timestamp and pushthrough flag.
			/// Only one thread may call this function for a given factory object.
			sample_p new_sample(double timestamp, bool pushthrough) { 
				sample *result = pop_freelist();
				if (!result)
					result = new(new char[sample_size_]) sample(fmt_,num_chans_,this);
				result->timestamp = timestamp;
				result->pushthrough = pushthrough;
				return sample_p(result);
			}

			/// Reclaim a sample that's no longer used.
			void reclaim_sample(sample *s) { 
				s->next_ = NULL;
				sample *prev = head_.exchange(s);
				prev->next_ = s;
			}

			/// Create a new sample whose memory is not managed by the factory.
			static sample *new_sample_unmanaged(channel_format_t fmt, int num_chans, double timestamp, bool pushthrough) { 
				sample *result = new(new char[ensure_multiple(sizeof(sample)-sizeof(char) + format_sizes[fmt]*num_chans,16)]) sample(fmt,num_chans,NULL);
				result->timestamp = timestamp;
				result->pushthrough = pushthrough;
				return result;
			}

		private:
			/// ensure that a given value is a multiple of some base, round up if necessary
			static boost::uint32_t ensure_multiple(boost::uint32_t v, unsigned base) { return (v%base) ? v - (v%base) + base : v; }

			// Pop a sample from the freelist
			// (multi-producer/single-consumer queue by Dmitry Vjukov)
			sample *pop_freelist(){ 
				sample *tail=tail_, *next=tail->next_;
				if (tail == sentinel_) { 
					if (!next) 
						return NULL; 
					tail_ = next; 
					tail = next; 
					next = next->next_;
				} 
				if (next) { 
					tail_ = next; 
					return tail; 
				} 
				sample *head = head_.load(); 
				if (tail != head)
					return NULL; 
				reclaim_sample(sentinel_); 
				next = tail->next_;
				if (next) {
					tail_ = next; 
					return tail; 
				}
				return NULL;
			} 

			friend class sample;
			channel_format_t fmt_;					// the channel format to construct samples with
			int num_chans_;							// the number of channels to construct samples with
			int sample_size_;						// size of a sample, in bytes
			int storage_size_;						// size of the allocated storage, in bytes
			boost::scoped_array<char> storage_;		// a slab of storage for pre-allocated samples
			sample *sentinel_;						// a sentinel element for our freelist
			boost::atomic<sample*> head_;			// head of the freelist
			sample *tail_;							// tail of the freelist
		};


		/// Destructor for a sample.
		~sample() {
			if (format_ == cf_string)
				for (std::string *p=(std::string*)&data_,*e=p+num_channels_; p<e; (p++)->~basic_string<char>());
		}

		/// Delete a sample.
		void operator delete(void *x) {
			// delete the underlying memory only if it wasn't allocated in the factory's storage area
			sample *s = (sample*)x;
			if (s && !(s->factory_ && (((char*)s) >= s->factory_->storage_.get() && ((char*)s) <= s->factory_->storage_.get()+s->factory_->storage_size_)))
				delete[] (char*)x;
		}

		/// Test for equality with another sample.
		bool operator==(const sample &rhs);

		// === type-safe accessors ===

		/// Assign an array of numeric values (with type conversions).
		template<class T> sample &assign_typed(T *s) { 
			if ((sizeof(T) == format_sizes[format_]) && ((boost::is_integral<T>::value && format_integral[format_]) || (boost::is_floating_point<T>::value && format_float[format_]))) {
				memcpy(&data_,s,format_sizes[format_]*num_channels_);
			} else {
				switch (format_) {
					case cf_float32:  for (float          *p=(float*)         &data_,*e=p+num_channels_; p<e; *p++ = (float)*s++); break;
					case cf_double64: for (double         *p=(double*)        &data_,*e=p+num_channels_; p<e; *p++ = (double)*s++); break;
					case cf_int8:     for (boost::int8_t  *p=(boost::int8_t*) &data_,*e=p+num_channels_; p<e; *p++ = (boost::int8_t)*s++); break; 
					case cf_int16:    for (boost::int16_t *p=(boost::int16_t*)&data_,*e=p+num_channels_; p<e; *p++ = (boost::int16_t)*s++); break; 
					case cf_int32:    for (boost::int32_t *p=(boost::int32_t*)&data_,*e=p+num_channels_; p<e; *p++ = (boost::int32_t)*s++); break; 
#ifndef BOOST_NO_INT64_T
					case cf_int64:    for (boost::int64_t *p=(boost::int64_t*)&data_,*e=p+num_channels_; p<e; *p++ = (boost::int64_t)*s++); break; 
#endif
					case cf_string:   for (std::string    *p=(std::string*)   &data_,*e=p+num_channels_; p<e; *p++ = boost::lexical_cast<std::string>(*s++)); break; 
					default: throw std::invalid_argument("Unsupported channel format.");
				}
			}
			return *this; 
		}

		/// Retrieve an array of numeric values (with type conversions).
		template<class T> sample &retrieve_typed(T *d) { 
			if ((sizeof(T) == format_sizes[format_]) && ((boost::is_integral<T>::value && format_integral[format_]) || (boost::is_floating_point<T>::value && format_float[format_]))) {
				memcpy(d,&data_,format_sizes[format_]*num_channels_);
			} else {
				switch (format_) {
					case cf_float32:  for (float          *p=(float*)         &data_,*e=p+num_channels_; p<e; *d++ = (T)*p++); break; 
					case cf_double64: for (double         *p=(double*)        &data_,*e=p+num_channels_; p<e; *d++ = (T)*p++); break; 
					case cf_int8:     for (boost::int8_t  *p=(boost::int8_t*) &data_,*e=p+num_channels_; p<e; *d++ = (T)*p++); break; 
					case cf_int16:    for (boost::int16_t *p=(boost::int16_t*)&data_,*e=p+num_channels_; p<e; *d++ = (T)*p++); break; 
					case cf_int32:    for (boost::int32_t *p=(boost::int32_t*)&data_,*e=p+num_channels_; p<e; *d++ = (T)*p++); break; 
#ifndef BOOST_NO_INT64_T
					case cf_int64:    for (boost::int64_t *p=(boost::int64_t*)&data_,*e=p+num_channels_; p<e; *d++ = (T)*p++); break; 
#endif
					case cf_string:   for (std::string    *p=(std::string*)   &data_,*e=p+num_channels_; p<e; *d++ = boost::lexical_cast<T>(*p++)); break; 
					default: throw std::invalid_argument("Unsupported channel format.");
				}
			}
			return *this; 
		}

		/// Assign an array of string values to the sample.
		sample &assign_typed(const std::string *s);

		/// Retrieve an array of string values from the sample.
		sample &retrieve_typed(std::string *d);

		// === untyped accessors ===

		/// Assign numeric data to the sample.
		sample &assign_untyped(void *newdata) { 
			if (format_ != cf_string)
				memcpy(&data_,newdata,format_sizes[format_]*num_channels_);
			else
				throw std::invalid_argument("Cannot assign untyped data to a string-formatted sample.");
			return *this; 
		}

		/// Retrieve numeric data from the sample.
		sample &retrieve_untyped(void *newdata) { 
			if (format_ != cf_string)
				memcpy(newdata,&data_,format_sizes[format_]*num_channels_);
			else
				throw std::invalid_argument("Cannot retrieve untyped data from a string-formatted sample.");
			return *this; 
		}

		// === serialization functions ===

		/// Helper function to save raw binary data to a stream buffer.
		template<class StreamBuf> static void save_raw(StreamBuf &sb, const void *address, std::size_t count) {
			if ((std::size_t)sb.sputn((char*)address,(std::streamsize)count) != count)
				throw std::runtime_error("Output stream error.");
		}

		/// Helper function to load raw binary data from a stream buffer.
		template<class StreamBuf> static void load_raw(StreamBuf &sb, const void *address, std::size_t count) {
			if ((std::size_t)sb.sgetn((char*)address,(std::streamsize)count) != count)
				throw std::runtime_error("Input stream error.");
		}

		/// Save a value to a stream buffer with correct endian treatment.
		template<class StreamBuf, typename T> static void save_value(StreamBuf &sb, const T &v, int use_byte_order) {
			if (sizeof(T)>1 && use_byte_order != BOOST_BYTE_ORDER) {
				T temp = lslboost::endian::reverse_value(v);
				save_raw(sb,&temp,sizeof(temp));
			} else
				save_raw(sb,&v,sizeof(v));
		}

		/// Load a value from a stream buffer with correct endian treatment.
		template<class StreamBuf, typename T> static void load_value(StreamBuf &sb, T &v, int use_byte_order) {
			load_raw(sb,&v,sizeof(v));
			if (use_byte_order != BOOST_BYTE_ORDER)
				lslboost::endian::reverse(v);
		}

		/// Load a value from a stream buffer; specialization of the above.
		template<class StreamBuf> void load_value(StreamBuf &sb, boost::uint8_t &v, int use_byte_order) { load_raw(sb,&v,sizeof(v)); }

		/// Serialize a sample to a stream buffer (protocol 1.10).
		template<class StreamBuf> void save_streambuf(StreamBuf &sb, int protocol_version, int use_byte_order, void *scratchpad=NULL) const {
			// write sample header
			if (timestamp == DEDUCED_TIMESTAMP) {
				save_value(sb,TAG_DEDUCED_TIMESTAMP,use_byte_order);
			} else {
				save_value(sb,TAG_TRANSMITTED_TIMESTAMP,use_byte_order);
				save_value(sb,timestamp,use_byte_order);
			}
			// write channel data
			if (format_ == cf_string) {
				for (std::string *p=(std::string*)&data_,*e=p+num_channels_; p<e; p++) {
					// write string length as variable-length integer
					if (p->size() <= 0xFF) {
						save_value(sb,(boost::uint8_t)sizeof(boost::uint8_t),use_byte_order);
						save_value(sb,(boost::uint8_t)p->size(),use_byte_order);
					} else {
						if (p->size() <= 0xFFFFFFFF) {
							save_value(sb,(boost::uint8_t)sizeof(boost::uint32_t),use_byte_order);
							save_value(sb,(boost::uint32_t)p->size(),use_byte_order);
						} else {
#ifndef BOOST_NO_INT64_T
							save_value(sb,(boost::uint8_t)sizeof(boost::uint64_t),use_byte_order);
							save_value(sb,(boost::uint64_t)p->size(),use_byte_order);
#else
							save_value(sb,(boost::uint8_t)sizeof(std::size_t),use_byte_order);
							save_value(sb,(std::size_t)p->size(),use_byte_order);
#endif
						}
					}
					// write string contents
					if (!p->empty())
						save_raw(sb,p->data(),p->size());
				}
			} else {
				// write numeric data in binary
				if (use_byte_order == BOOST_BYTE_ORDER || format_sizes[format_]==1) {
					save_raw(sb,&data_,format_sizes[format_]*num_channels_);
				} else {
					memcpy(scratchpad,&data_,format_sizes[format_]*num_channels_);
					convert_endian(scratchpad);
					save_raw(sb,scratchpad,format_sizes[format_]*num_channels_);
				}
			}
		}

		/// Deserialize a sample from a stream buffer (protocol 1.10).
		template<class StreamBuf> void load_streambuf(StreamBuf &sb, int protocol_version, int use_byte_order, bool suppress_subnormals) {
			// read sample header
			boost::uint8_t tag; load_value(sb,tag,use_byte_order);
			if (tag == TAG_DEDUCED_TIMESTAMP) {
				// deduce the timestamp
				timestamp = DEDUCED_TIMESTAMP;
			} else {
				// read the time stamp
				load_value(sb,timestamp,use_byte_order);
			}
			// read channel data
			if (format_ == cf_string) {
				for (std::string *p=(std::string*)&data_,*e=p+num_channels_; p<e; p++) {
					// read string length as variable-length integer
					std::size_t len = 0;
					boost::uint8_t lenbytes; load_value(sb,lenbytes,use_byte_order);
					switch (lenbytes) {
						case sizeof(boost::uint8_t):  { boost::uint8_t tmp;  load_value(sb,tmp,use_byte_order); len = tmp; }; break; 
						case sizeof(boost::uint16_t): { boost::uint16_t tmp; load_value(sb,tmp,use_byte_order); len = tmp; }; break; 
						case sizeof(boost::uint32_t): { boost::uint32_t tmp; load_value(sb,tmp,use_byte_order); len = tmp; }; break; 
#ifndef BOOST_NO_INT64_T
						case sizeof(boost::uint64_t): { boost::uint64_t tmp; load_value(sb,tmp,use_byte_order); len = tmp; }; break;
#else
						case 8: throw std::runtime_error("This platform does not support strings of 64-bit length.");
#endif
						default: throw std::runtime_error("Stream contents corrupted (invalid varlen int).");
					}
					// read string contents
					p->resize(len);
					if (len>0)
						load_raw(sb,&(*p)[0],len);
				}
			} else {
				// read numeric channel data
				load_raw(sb,&data_,format_sizes[format_]*num_channels_);
				if (use_byte_order != BOOST_BYTE_ORDER && format_sizes[format_]>1)
					convert_endian(&data_);
				if (suppress_subnormals && format_float[format_]) {
					if (format_ == cf_float32) {
						for (boost::uint32_t *p=(boost::uint32_t*)&data_,*e=p+num_channels_; p<e; p++)
							if (*p && ((*p & UINT32_C(0x7fffffff)) <= UINT32_C(0x007fffff)))
								*p &= UINT32_C(0x80000000);
					} else {
#ifndef BOOST_NO_INT64_T
						for (boost::uint64_t *p=(boost::uint64_t*)&data_,*e=p+num_channels_; p<e; p++)
							if (*p && ((*p & UINT64_C(0x7fffffffffffffff)) <= UINT64_C(0x000fffffffffffff)))
								*p &= UINT64_C(0x8000000000000000);
#endif
					}
				}
			}
		}

		/// Convert the endianness of channel data in-place.
		void convert_endian(void *data) const {
			switch (format_sizes[format_]) {
				case 1: break;
				case sizeof(boost::int16_t): for (boost::int16_t *p=(boost::int16_t*)data,*e=p+num_channels_; p<e; lslboost::endian::reverse(*p++)); break;
				case sizeof(boost::int32_t): for (boost::int32_t *p=(boost::int32_t*)data,*e=p+num_channels_; p<e; lslboost::endian::reverse(*p++)); break;
#ifndef BOOST_NO_INT64_T
				case sizeof(boost::int64_t): for (boost::int64_t *p=(boost::int64_t*)data,*e=p+num_channels_; p<e; lslboost::endian::reverse(*p++)); break;
#else
				case sizeof(double): for (double *p=(double*)data,*e=p+num_channels_; p<e; lslboost::endian::reverse(*p++)); break;
#endif
				default: throw std::runtime_error("Unsupported channel format for endian conversion.");
			}
		}

		/// Serialize a sample into a portable archive (protocol 1.00).
		template<class Archive> void save(Archive &ar, const unsigned int archive_version) const {
			// write sample header
			if (timestamp == DEDUCED_TIMESTAMP) {
				ar & TAG_DEDUCED_TIMESTAMP;
			} else {
				ar & TAG_TRANSMITTED_TIMESTAMP & timestamp;
			}
			// write channel data
			const_cast<sample*>(this)->serialize_channels(ar,archive_version);
		}

		/// Deserialize a sample from a portable archive (protocol 1.00).
		template<class Archive> void load(Archive &ar, const unsigned int archive_version) {
			// read sample header
			char tag; ar & tag;
			if (tag == TAG_DEDUCED_TIMESTAMP) {
				// deduce the timestamp
				timestamp = DEDUCED_TIMESTAMP;
			} else {
				// read the time stamp
				ar & timestamp;
			}
			// read channel data
			serialize_channels(ar,archive_version);
		}

		/// Serialize (read/write) the channel data.
		template<class Archive> void serialize_channels(Archive &ar, const unsigned int archive_version) {
			switch (format_) {
				case cf_float32:  for (float          *p=(float*)         &data_,*e=p+num_channels_; p<e; ar & *p++); break;
				case cf_double64: for (double         *p=(double*)        &data_,*e=p+num_channels_; p<e; ar & *p++); break;
				case cf_string:   for (std::string    *p=(std::string*)   &data_,*e=p+num_channels_; p<e; ar & *p++); break;
				case cf_int8:     for (boost::int8_t  *p=(boost::int8_t*) &data_,*e=p+num_channels_; p<e; ar & *p++); break;
				case cf_int16:    for (boost::int16_t *p=(boost::int16_t*)&data_,*e=p+num_channels_; p<e; ar & *p++); break;
				case cf_int32:    for (boost::int32_t *p=(boost::int32_t*)&data_,*e=p+num_channels_; p<e; ar & *p++); break;
#ifndef BOOST_NO_INT64_T
				case cf_int64:    for (boost::int64_t *p=(boost::int64_t*)&data_,*e=p+num_channels_; p<e; ar & *p++); break;
#endif
				default: throw std::runtime_error("Unsupported channel format.");
			}
		}

		BOOST_SERIALIZATION_SPLIT_MEMBER()		

		/// Assign a test pattern to the sample (for protocol validation)
		sample &assign_test_pattern(int offset=1);

	private:
		/// Construct a new sample for a given channel format/count combination.
		sample(channel_format_t fmt, int num_channels, factory *fact): format_(fmt), num_channels_(num_channels), refcount_(0), next_(NULL), factory_(fact) { 
			if (format_ == cf_string)
				for (std::string *p=(std::string*)&data_,*e=p+num_channels_; p<e; new(p++)std::string());
		}

		/// Increment ref count.
		friend void intrusive_ptr_add_ref(sample *s) {
			s->refcount_.fetch_add(1,boost::memory_order_relaxed);
		}

		/// Decrement ref count and reclaim if unreferenced.
		friend void intrusive_ptr_release(sample *s) {
			if (s->refcount_.fetch_sub(1,boost::memory_order_release) == 1) {
				boost::atomic_thread_fence(boost::memory_order_acquire);
				s->factory_->reclaim_sample(s);
			}
		}
	};

}

#endif
