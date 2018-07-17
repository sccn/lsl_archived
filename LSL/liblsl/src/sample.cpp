#include "sample.h"

// === implementation of the sample class ===

using namespace lsl;

/// Compare two samples for equality (based on content).
bool sample::operator==(const sample &rhs) {
	if ((timestamp != rhs.timestamp) || (format_ != rhs.format_) || (num_channels_ != rhs.num_channels_))
		return false;
	if (format_ != cf_string)
		return memcmp(&(rhs.data_),&data_,format_sizes[format_]*num_channels_) == 0;
	else {
		std::string *data = (std::string*)&data_;
		std::string *rhsdata = (std::string*)&(rhs.data_);
		for (std::size_t k=0; k<num_channels_; k++)
			if (data[k] != rhsdata[k])
				return false;
		return true;
	}
}

/// Assign an array of string values to the sample.
sample &sample::assign_typed(const std::string *s) { 
	switch (format_) {
		case cf_string:   for (std::string    *p=(std::string*)   &data_,*e=p+num_channels_; p<e; *p++ = *s++); break; 
		case cf_float32:  for (float          *p=(float*)         &data_,*e=p+num_channels_; p<e; *p++ = lslboost::lexical_cast<float>(*s++)); break;
		case cf_double64: for (double         *p=(double*)        &data_,*e=p+num_channels_; p<e; *p++ = lslboost::lexical_cast<double>(*s++)); break;
		case cf_int8:     for (lslboost::int8_t  *p=(lslboost::int8_t*) &data_,*e=p+num_channels_; p<e; *p++ = lslboost::lexical_cast<lslboost::int8_t>(*s++)); break;
		case cf_int16:    for (lslboost::int16_t *p=(lslboost::int16_t*)&data_,*e=p+num_channels_; p<e; *p++ = lslboost::lexical_cast<lslboost::int16_t>(*s++)); break;
		case cf_int32:    for (lslboost::int32_t *p=(lslboost::int32_t*)&data_,*e=p+num_channels_; p<e; *p++ = lslboost::lexical_cast<lslboost::int32_t>(*s++)); break;
#ifndef BOOST_NO_INT64_T
		case cf_int64:    for (lslboost::int64_t *p=(lslboost::int64_t*)&data_,*e=p+num_channels_; p<e; *p++ = lslboost::lexical_cast<lslboost::int64_t>(*s++)); break;
#endif
		default: throw std::invalid_argument("Unsupported channel format.");
	}
	return *this;
}

/// Retrieve an array of string values from the sample.
sample &sample::retrieve_typed(std::string *d) {
	switch (format_) {
		case cf_string:   for (std::string    *p=(std::string*)   &data_,*e=p+num_channels_; p<e; *d++ = *p++); break; 
		case cf_float32:  for (float          *p=(float*)         &data_,*e=p+num_channels_; p<e; *d++ = lslboost::lexical_cast<std::string>(*p++)); break; 
		case cf_double64: for (double         *p=(double*)        &data_,*e=p+num_channels_; p<e; *d++ = lslboost::lexical_cast<std::string>(*p++)); break; 
		case cf_int8:     for (lslboost::int8_t  *p=(lslboost::int8_t*) &data_,*e=p+num_channels_; p<e; *d++ = lslboost::lexical_cast<std::string>(*p++)); break; 
		case cf_int16:    for (lslboost::int16_t *p=(lslboost::int16_t*)&data_,*e=p+num_channels_; p<e; *d++ = lslboost::lexical_cast<std::string>(*p++)); break; 
		case cf_int32:    for (lslboost::int32_t *p=(lslboost::int32_t*)&data_,*e=p+num_channels_; p<e; *d++ = lslboost::lexical_cast<std::string>(*p++)); break; 
#ifndef BOOST_NO_INT64_T
		case cf_int64:    for (lslboost::int64_t *p=(lslboost::int64_t*)&data_,*e=p+num_channels_; p<e; *d++ = lslboost::lexical_cast<std::string>(*p++)); break; 
#endif
		default: throw std::invalid_argument("Unsupported channel format.");
	}
	return *this;
}

void sample::save_streambuf(std::streambuf& sb, int protocol_version, int use_byte_order, void* scratchpad) const {
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
				save_value(sb,(lslboost::uint8_t)sizeof(lslboost::uint8_t),use_byte_order);
				save_value(sb,(lslboost::uint8_t)p->size(),use_byte_order);
			} else {
				if (p->size() <= 0xFFFFFFFF) {
					save_value(sb,(lslboost::uint8_t)sizeof(lslboost::uint32_t),use_byte_order);
					save_value(sb,(lslboost::uint32_t)p->size(),use_byte_order);
				} else {
#ifndef BOOST_NO_INT64_T
					save_value(sb,(lslboost::uint8_t)sizeof(lslboost::uint64_t),use_byte_order);
					save_value(sb,(lslboost::uint64_t)p->size(),use_byte_order);
#else
					save_value(sb,(lslboost::uint8_t)sizeof(std::size_t),use_byte_order);
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

void sample::load_streambuf(std::streambuf& sb, int protocol_version, int use_byte_order,
                            bool suppress_subnormals) {
	// read sample header
	lslboost::uint8_t tag;
	load_value(sb, tag, use_byte_order);
	if (tag == TAG_DEDUCED_TIMESTAMP) {
		// deduce the timestamp
		timestamp = DEDUCED_TIMESTAMP;
	} else {
		// read the time stamp
		load_value(sb, timestamp, use_byte_order);
	}
	// read channel data
	if (format_ == cf_string) {
		for (std::string *p = (std::string*)&data_, *e = p + num_channels_; p < e; p++) {
			// read string length as variable-length integer
			std::size_t len = 0;
			lslboost::uint8_t lenbytes;
			load_value(sb, lenbytes, use_byte_order);
			if (sizeof(std::size_t) < 8 && lenbytes > sizeof(std::size_t))
				throw std::runtime_error(
				    "This platform does not support strings of 64-bit length.");
			switch (lenbytes) {
			case sizeof(lslboost::uint8_t): {
				lslboost::uint8_t tmp;
				load_value(sb, tmp, use_byte_order);
				len = tmp;
			}; break;
			case sizeof(lslboost::uint16_t): {
				lslboost::uint16_t tmp;
				load_value(sb, tmp, use_byte_order);
				len = tmp;
			}; break;
			case sizeof(lslboost::uint32_t): {
				lslboost::uint32_t tmp;
				load_value(sb, tmp, use_byte_order);
				len = tmp;
			}; break;
#ifndef BOOST_NO_INT64_T
			case sizeof(lslboost::uint64_t): {
				lslboost::uint64_t tmp;
				load_value(sb, tmp, use_byte_order);
				len = tmp;
			}; break;
#endif
			default: throw std::runtime_error("Stream contents corrupted (invalid varlen int).");
			}
			// read string contents
			p->resize(len);
			if (len > 0) load_raw(sb, &(*p)[0], len);
		}
	} else {
		// read numeric channel data
		load_raw(sb, &data_, format_sizes[format_] * num_channels_);
		if (use_byte_order != BOOST_BYTE_ORDER && format_sizes[format_] > 1) convert_endian(&data_);
		if (suppress_subnormals && format_float[format_]) {
			if (format_ == cf_float32) {
				for (lslboost::uint32_t *p = (lslboost::uint32_t*)&data_, *e = p + num_channels_;
				     p < e; p++)
					if (*p && ((*p & UINT32_C(0x7fffffff)) <= UINT32_C(0x007fffff)))
						*p &= UINT32_C(0x80000000);
			} else {
#ifndef BOOST_NO_INT64_T
				for (lslboost::uint64_t *p = (lslboost::uint64_t*)&data_, *e = p + num_channels_;
				     p < e; p++)
					if (*p && ((*p & UINT64_C(0x7fffffffffffffff)) <= UINT64_C(0x000fffffffffffff)))
						*p &= UINT64_C(0x8000000000000000);
#endif
			}
		}
	}
}

/// Assign a test pattern to the sample.
sample &sample::assign_test_pattern(int offset) { 
	pushthrough = 1; 
	timestamp = 123456.789; 

	switch (format_) {
		case cf_float32: {
			float *data = (float*)&data_;
			for (int k=0; k<num_channels_; k++)
				data[k] = ((float)k + (float)offset) * (k%2==0 ? 1 : -1);			
			break;
						 }
		case cf_double64: {
			double *data = (double*)&data_;
			for (int k=0; k<num_channels_; k++)
				data[k] = (k + offset + 16777217) * (k%2==0 ? 1 : -1);
			break;
						  }
		case cf_string:{
			std::string *data = (std::string*)&data_;
			for (int k=0; k<num_channels_; k++)
				data[k] = lslboost::lexical_cast<std::string>((k + 10) * (k%2==0 ? 1 : -1));
			break;
					   }
		case cf_int32: {
			lslboost::int32_t *data = (lslboost::int32_t*)&data_;
			for (int k=0; k<num_channels_; k++)
				data[k] = ((k + offset + 65537)%2147483647) * (k%2==0 ? 1 : -1);
			break;
					   }
		case cf_int16: {
			lslboost::int16_t *data = (lslboost::int16_t*)&data_;
			for (int k=0; k<num_channels_; k++)
				data[k] = (lslboost::int16_t)(((k + offset + 257)%32767) * (k%2==0 ? 1 : -1));
			break;
					   }
		case cf_int8: {
			lslboost::int8_t *data = (lslboost::int8_t*)&data_;
			for (int k=0; k<num_channels_; k++)
				data[k] = (lslboost::int8_t)(((k + offset + 1)%127) * (k%2==0 ? 1 : -1));
			break;
					  }
#ifndef BOOST_NO_INT64_T
		case cf_int64:{
			lslboost::int64_t *data = (lslboost::int64_t*)&data_;
			for (int k=0; k<num_channels_; k++)
				data[k] = ((2 + k + (lslboost::int64_t)offset + 2147483647)) * (lslboost::int64_t)(k%2==0 ? 1 : -1);
			break;
					  }
#endif
		default:
			throw std::invalid_argument("Unsupported channel format used to construct a sample.");
	}

	return *this;
}

sample::factory::factory(channel_format_t fmt, int num_chans, int num_reserve)
    : fmt_(fmt), num_chans_(num_chans),
      sample_size_(
          ensure_multiple(sizeof(sample) - sizeof(char) + format_sizes[fmt] * num_chans, 16)),
      storage_size_(sample_size_ * std::max(1, num_reserve)), storage_(new char[storage_size_]),
      sentinel_(new_sample_unmanaged(fmt, num_chans, 0.0, false)), head_(sentinel_),
      tail_(sentinel_) {
	// pre-construct an array of samples in the storage area and chain into a freelist
	sample* s = NULL;
	for (char *p = storage_.get(), *e = p + storage_size_; p < e;) {
#pragma warning(suppress : 4291)
		s = new ((sample*)p) sample(fmt, num_chans, this);
		s->next_ = (sample*)(p += sample_size_);
	}
	s->next_ = NULL;
	head_.store(s);
	sentinel_->next_ = (sample*)storage_.get();
}

sample_p sample::factory::new_sample(double timestamp, bool pushthrough) {
	sample* result = pop_freelist();
	if (!result)
#pragma warning(suppress : 4291)
		result = new (new char[sample_size_]) sample(fmt_, num_chans_, this);
	result->timestamp = timestamp;
	result->pushthrough = pushthrough;
	return sample_p(result);
}

sample* sample::factory::new_sample_unmanaged(channel_format_t fmt, int num_chans, double timestamp,
                                              bool pushthrough) {
#pragma warning(suppress : 4291)
	sample* result =
	    new (new char[ensure_multiple(sizeof(sample) - sizeof(char) + format_sizes[fmt] * num_chans,
	                                  16)]) sample(fmt, num_chans, NULL);
	result->timestamp = timestamp;
	result->pushthrough = pushthrough;
	return result;
}

sample* sample::factory::pop_freelist() {
	sample *tail = tail_, *next = tail->next_;
	if (tail == sentinel_) {
		if (!next) return NULL;
		tail_ = next;
		tail = next;
		next = next->next_;
	}
	if (next) {
		tail_ = next;
		return tail;
	}
	sample* head = head_.load();
	if (tail != head) return NULL;
	reclaim_sample(sentinel_);
	next = tail->next_;
	if (next) {
		tail_ = next;
		return tail;
	}
	return NULL;
}
