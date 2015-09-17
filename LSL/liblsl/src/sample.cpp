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
		case cf_float32:  for (float          *p=(float*)         &data_,*e=p+num_channels_; p<e; *p++ = boost::lexical_cast<float>(*s++)); break;
		case cf_double64: for (double         *p=(double*)        &data_,*e=p+num_channels_; p<e; *p++ = boost::lexical_cast<double>(*s++)); break;
		case cf_int8:     for (boost::int8_t  *p=(boost::int8_t*) &data_,*e=p+num_channels_; p<e; *p++ = boost::lexical_cast<boost::int8_t>(*s++)); break;
		case cf_int16:    for (boost::int16_t *p=(boost::int16_t*)&data_,*e=p+num_channels_; p<e; *p++ = boost::lexical_cast<boost::int16_t>(*s++)); break;
		case cf_int32:    for (boost::int32_t *p=(boost::int32_t*)&data_,*e=p+num_channels_; p<e; *p++ = boost::lexical_cast<boost::int32_t>(*s++)); break;
#ifndef BOOST_NO_INT64_T
		case cf_int64:    for (boost::int64_t *p=(boost::int64_t*)&data_,*e=p+num_channels_; p<e; *p++ = boost::lexical_cast<boost::int64_t>(*s++)); break;
#endif
		default: throw std::invalid_argument("Unsupported channel format.");
	}
	return *this;
}

/// Retrieve an array of string values from the sample.
sample &sample::retrieve_typed(std::string *d) {
	switch (format_) {
		case cf_string:   for (std::string    *p=(std::string*)   &data_,*e=p+num_channels_; p<e; *d++ = *p++); break; 
		case cf_float32:  for (float          *p=(float*)         &data_,*e=p+num_channels_; p<e; *d++ = boost::lexical_cast<std::string>(*p++)); break; 
		case cf_double64: for (double         *p=(double*)        &data_,*e=p+num_channels_; p<e; *d++ = boost::lexical_cast<std::string>(*p++)); break; 
		case cf_int8:     for (boost::int8_t  *p=(boost::int8_t*) &data_,*e=p+num_channels_; p<e; *d++ = boost::lexical_cast<std::string>(*p++)); break; 
		case cf_int16:    for (boost::int16_t *p=(boost::int16_t*)&data_,*e=p+num_channels_; p<e; *d++ = boost::lexical_cast<std::string>(*p++)); break; 
		case cf_int32:    for (boost::int32_t *p=(boost::int32_t*)&data_,*e=p+num_channels_; p<e; *d++ = boost::lexical_cast<std::string>(*p++)); break; 
#ifndef BOOST_NO_INT64_T
		case cf_int64:    for (boost::int64_t *p=(boost::int64_t*)&data_,*e=p+num_channels_; p<e; *d++ = boost::lexical_cast<std::string>(*p++)); break; 
#endif
		default: throw std::invalid_argument("Unsupported channel format.");
	}
	return *this;
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
				data[k] = boost::lexical_cast<std::string>((k + 10) * (k%2==0 ? 1 : -1));
			break;
					   }
		case cf_int32: {
			boost::int32_t *data = (boost::int32_t*)&data_;
			for (int k=0; k<num_channels_; k++)
				data[k] = ((k + offset + 65537)%2147483647) * (k%2==0 ? 1 : -1);
			break;
					   }
		case cf_int16: {
			boost::int16_t *data = (boost::int16_t*)&data_;
			for (int k=0; k<num_channels_; k++)
				data[k] = (boost::int16_t)(((k + offset + 257)%32767) * (k%2==0 ? 1 : -1));
			break;
					   }
		case cf_int8: {
			boost::int8_t *data = (boost::int8_t*)&data_;
			for (int k=0; k<num_channels_; k++)
				data[k] = (boost::int8_t)(((k + offset + 1)%127) * (k%2==0 ? 1 : -1));
			break;
					  }
#ifndef BOOST_NO_INT64_T
		case cf_int64:{
			boost::int64_t *data = (boost::int64_t*)&data_;
			for (int k=0; k<num_channels_; k++)
				data[k] = ((2 + k + (boost::int64_t)offset + 2147483647)) * (boost::int64_t)(k%2==0 ? 1 : -1);
			break;
					  }
#endif
		default:
			throw std::invalid_argument("Unsupported channel format used to construct a sample.");
	}

	return *this;
}



