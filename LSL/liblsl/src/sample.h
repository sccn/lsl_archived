#ifndef SAMPLE_H
#define SAMPLE_H

#include <boost/lexical_cast.hpp>
#include <vector>
#include <string>
#include <streambuf>
#include <boost/smart_ptr.hpp>
#include <boost/intrusive_ptr.hpp>
#include <boost/atomic.hpp>
#include <boost/static_assert.hpp>
#include <boost/serialization/split_member.hpp>
#include "endian/conversion.hpp"
#include "common.h"

namespace eos {
class portable_iarchive;
class portable_oarchive;
}

namespace lsl {
	// if you get an error here your machine cannot represent the double-precision time-stamp format required by LSL
	BOOST_STATIC_ASSERT(sizeof(double)==8);

	// constants used in the network protocol
	const uint8_t TAG_DEDUCED_TIMESTAMP = 1;
	const uint8_t TAG_TRANSMITTED_TIMESTAMP = 2;

	/// channel format properties
	const int format_sizes[] = {0,sizeof(float),sizeof(double),sizeof(std::string),sizeof(int32_t),sizeof(int16_t),sizeof(int8_t),8};
	const bool format_ieee754[] = {false,std::numeric_limits<float>::is_iec559,std::numeric_limits<double>::is_iec559,false,false,false,false,false}; 
	const bool format_subnormal[] = {false,std::numeric_limits<float>::has_denorm!=std::denorm_absent,std::numeric_limits<double>::has_denorm!=std::denorm_absent,false,false,false,false,false}; 
	const bool format_integral[] = {false,false,false,false,true,true,true,true}; 
	const bool format_float[] = {false,true,true,false,false,false,false,false}; 
 
	/// smart pointer to a sample
	typedef lslboost::intrusive_ptr<class sample> sample_p;
	
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
		lslboost::atomic<int> refcount_;	// reference count used by sample_p
		lslboost::atomic<sample*> next_;	// linked list of samples, for use in a freelist
		factory *factory_;				// the factory used to reclaim this sample, if any
		BOOST_ALIGNMENT(8) char data_;	// the data payload begins here

	public:
		// === Construction ===

		// shared pointer to a sample factory
		typedef lslboost::shared_ptr<factory> factory_p;

		/// A factory to create samples of a given format/size.
		/// Must outlive all of its created samples.
		class factory {
		public:
			/// Create a new factory and optionally pre-allocate samples.
			factory(channel_format_t fmt, int num_chans, int num_reserve);

			/// Destroy the factory and delete all of its samples.
			~factory() {
				if (sample *cur = head_)
					for (sample *next=cur->next_;next;cur=next,next=next->next_)
						delete cur;
				delete sentinel_;
			}

			/// Create a new sample with a given timestamp and pushthrough flag.
			/// Only one thread may call this function for a given factory object.
			sample_p new_sample(double timestamp, bool pushthrough);

			/// Reclaim a sample that's no longer used.
			void reclaim_sample(sample *s) { 
				s->next_ = NULL;
				sample *prev = head_.exchange(s);
				prev->next_ = s;
			}

			/// Create a new sample whose memory is not managed by the factory.
			static sample *new_sample_unmanaged(channel_format_t fmt, int num_chans, double timestamp, bool pushthrough);

		private:
			/// ensure that a given value is a multiple of some base, round up if necessary
			static uint32_t ensure_multiple(uint32_t v, unsigned base) { return (v%base) ? v - (v%base) + base : v; }

			// Pop a sample from the freelist
			// (multi-producer/single-consumer queue by Dmitry Vjukov)
			sample *pop_freelist();

			friend class sample;
			channel_format_t fmt_;					// the channel format to construct samples with
			int num_chans_;							// the number of channels to construct samples with
			int sample_size_;						// size of a sample, in bytes
			int storage_size_;						// size of the allocated storage, in bytes
			lslboost::scoped_array<char> storage_;		// a slab of storage for pre-allocated samples
			sample *sentinel_;						// a sentinel element for our freelist
			lslboost::atomic<sample*> head_;			// head of the freelist
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
		bool operator==(const sample &rhs) const BOOST_NOEXCEPT ;

		// === type-safe accessors ===

		/// Assign an array of numeric values (with type conversions).
		template<class T> sample &assign_typed(const T *s) { 
			if ((sizeof(T) == format_sizes[format_]) && ((lslboost::is_integral<T>::value && format_integral[format_]) || (lslboost::is_floating_point<T>::value && format_float[format_]))) {
				memcpy(&data_,s,format_sizes[format_]*num_channels_);
			} else {
				switch (format_) {
					case cf_float32:  for (float          *p=(float*)         &data_,*e=p+num_channels_; p<e; *p++ = (float)*s++); break;
					case cf_double64: for (double         *p=(double*)        &data_,*e=p+num_channels_; p<e; *p++ = (double)*s++); break;
					case cf_int8:     for (int8_t  *p=(int8_t*) &data_,*e=p+num_channels_; p<e; *p++ = (int8_t)*s++); break;
					case cf_int16:    for (int16_t *p=(int16_t*)&data_,*e=p+num_channels_; p<e; *p++ = (int16_t)*s++); break;
					case cf_int32:    for (int32_t *p=(int32_t*)&data_,*e=p+num_channels_; p<e; *p++ = (int32_t)*s++); break;
#ifndef BOOST_NO_INT64_T
					case cf_int64:    for (int64_t *p=(int64_t*)&data_,*e=p+num_channels_; p<e; *p++ = (int64_t)*s++); break;
#endif
					case cf_string:   for (std::string    *p=(std::string*)   &data_,*e=p+num_channels_; p<e; *p++ = lslboost::lexical_cast<std::string>(*s++)); break; 
					default: throw std::invalid_argument("Unsupported channel format.");
				}
			}
			return *this; 
		}

		/// Retrieve an array of numeric values (with type conversions).
		template<class T> sample &retrieve_typed(T *d) { 
			if ((sizeof(T) == format_sizes[format_]) && ((lslboost::is_integral<T>::value && format_integral[format_]) || (lslboost::is_floating_point<T>::value && format_float[format_]))) {
				memcpy(d,&data_,format_sizes[format_]*num_channels_);
			} else {
				switch (format_) {
					case cf_float32:  for (float          *p=(float*)         &data_,*e=p+num_channels_; p<e; *d++ = (T)*p++); break; 
					case cf_double64: for (double         *p=(double*)        &data_,*e=p+num_channels_; p<e; *d++ = (T)*p++); break; 
					case cf_int8:     for (int8_t  *p=(int8_t*) &data_,*e=p+num_channels_; p<e; *d++ = (T)*p++); break;
					case cf_int16:    for (int16_t *p=(int16_t*)&data_,*e=p+num_channels_; p<e; *d++ = (T)*p++); break;
					case cf_int32:    for (int32_t *p=(int32_t*)&data_,*e=p+num_channels_; p<e; *d++ = (T)*p++); break;
#ifndef BOOST_NO_INT64_T
					case cf_int64:    for (int64_t *p=(int64_t*)&data_,*e=p+num_channels_; p<e; *d++ = (T)*p++); break;
#endif
					case cf_string:   for (std::string    *p=(std::string*)   &data_,*e=p+num_channels_; p<e; *d++ = lslboost::lexical_cast<T>(*p++)); break; 
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
		sample &assign_untyped(const void *newdata) {
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
		static void save_raw(std::streambuf &sb, const void *address, std::size_t count) {
			if ((std::size_t)sb.sputn((const char*)address,(std::streamsize)count) != count)
				throw std::runtime_error("Output stream error.");
		}

		/// Helper function to load raw binary data from a stream buffer.
		static void load_raw(std::streambuf &sb, void *address, std::size_t count) {
			if ((std::size_t)sb.sgetn((char*)address,(std::streamsize)count) != count)
				throw std::runtime_error("Input stream error.");
		}

		/// Save a value to a stream buffer with correct endian treatment.
		template<typename T> static void save_value(std::streambuf &sb, const T &v, int use_byte_order) {
			if (sizeof(T)>1 && use_byte_order != BOOST_BYTE_ORDER) {
				T temp = lslboost::endian::reverse_value(v);
				save_raw(sb,&temp,sizeof(temp));
			} else
				save_raw(sb,&v,sizeof(v));
		}

		/// Load a value from a stream buffer with correct endian treatment.
		template<typename T> static void load_value(std::streambuf &sb, T &v, int use_byte_order) {
			load_raw(sb,&v,sizeof(v));
			if (use_byte_order != BOOST_BYTE_ORDER)
				lslboost::endian::reverse(v);
		}

		/// Load a value from a stream buffer; specialization of the above.
		void load_value(std::streambuf &sb, uint8_t &v, int use_byte_order) { load_raw(sb,&v,sizeof(v)); }

		/// Serialize a sample to a stream buffer (protocol 1.10).
		void save_streambuf(std::streambuf &sb, int protocol_version, int use_byte_order, void *scratchpad=NULL) const;

		/// Deserialize a sample from a stream buffer (protocol 1.10).
		void load_streambuf(std::streambuf &sb, int protocol_version, int use_byte_order, bool suppress_subnormals);

		/// Convert the endianness of channel data in-place.
		void convert_endian(void *data) const {
			switch (format_sizes[format_]) {
				case 1: break;
				case sizeof(int16_t): for (int16_t *p=(int16_t*)data,*e=p+num_channels_; p<e; lslboost::endian::reverse(*p++)); break;
				case sizeof(int32_t): for (int32_t *p=(int32_t*)data,*e=p+num_channels_; p<e; lslboost::endian::reverse(*p++)); break;
#ifndef BOOST_NO_INT64_T
				case sizeof(int64_t): for (int64_t *p=(int64_t*)data,*e=p+num_channels_; p<e; lslboost::endian::reverse(*p++)); break;
#else
				case sizeof(double): for (double *p=(double*)data,*e=p+num_channels_; p<e; lslboost::endian::reverse(*p++)); break;
#endif
				default: throw std::runtime_error("Unsupported channel format for endian conversion.");
			}
		}
		/// Serialize a sample into a portable archive (protocol 1.00).
		void save(eos::portable_oarchive &ar, const uint32_t archive_version) const;

		/// Deserialize a sample from a portable archive (protocol 1.00).
		void load(eos::portable_iarchive& ar, const uint32_t archive_version);

		/// Serialize (read/write) the channel data.
		template<class Archive> void serialize_channels(Archive &ar, const uint32_t archive_version);

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
			s->refcount_.fetch_add(1,lslboost::memory_order_relaxed);
		}

		/// Decrement ref count and reclaim if unreferenced.
		friend void intrusive_ptr_release(sample *s) {
			if (s->refcount_.fetch_sub(1,lslboost::memory_order_release) == 1) {
				lslboost::atomic_thread_fence(lslboost::memory_order_acquire);
				s->factory_->reclaim_sample(s);
			}
		}
	};

}

#endif
