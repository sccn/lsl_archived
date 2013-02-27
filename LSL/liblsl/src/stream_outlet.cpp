#include "stream_outlet_impl.h"

//
// === implementation of the stream_outlet class ===
//

using namespace lsl;

// boilerplate code calling the private implementation
stream_outlet::stream_outlet(const stream_info &info, int chunk_size, int max_buffered): impl_(new stream_outlet_impl(*info.impl(),chunk_size,info.nominal_srate()?(int)(info.nominal_srate()*max_buffered):max_buffered*100)) { }
stream_outlet::~stream_outlet() { delete impl_; }
stream_info stream_outlet::info() const { return stream_info(&impl_->info()); }
void stream_outlet::push_sample(const std::vector<float> &data, double timestamp, bool pushthrough) { impl_->push_sample(data,timestamp,pushthrough); }
void stream_outlet::push_sample(const std::vector<double> &data, double timestamp, bool pushthrough) { impl_->push_sample(data,timestamp,pushthrough); }
void stream_outlet::push_sample(const std::vector<long> &data, double timestamp, bool pushthrough) { impl_->push_sample(data,timestamp,pushthrough); }
void stream_outlet::push_sample(const std::vector<int> &data, double timestamp, bool pushthrough) { impl_->push_sample(data,timestamp,pushthrough); }
void stream_outlet::push_sample(const std::vector<short> &data, double timestamp, bool pushthrough) { impl_->push_sample(data,timestamp,pushthrough); }
void stream_outlet::push_sample(const std::vector<char> &data, double timestamp, bool pushthrough) { impl_->push_sample(data,timestamp,pushthrough); }
void stream_outlet::push_sample(const std::vector<std::string> &data, double timestamp, bool pushthrough) { impl_->push_sample(data,timestamp,pushthrough); }
void stream_outlet::push_sample(const float *data, double timestamp, bool pushthrough) { impl_->push_sample(data,timestamp,pushthrough); }
void stream_outlet::push_sample(const double *data, double timestamp, bool pushthrough) { impl_->push_sample(data,timestamp,pushthrough); }
void stream_outlet::push_sample(const long *data, double timestamp, bool pushthrough) { impl_->push_sample(data,timestamp,pushthrough); }
void stream_outlet::push_sample(const int *data, double timestamp, bool pushthrough) { impl_->push_sample(data,timestamp,pushthrough); }
void stream_outlet::push_sample(const short *data, double timestamp, bool pushthrough) { impl_->push_sample(data,timestamp,pushthrough); }
void stream_outlet::push_sample(const char *data, double timestamp, bool pushthrough) { impl_->push_sample(data,timestamp,pushthrough); }
void stream_outlet::push_sample(const std::string *data, double timestamp, bool pushthrough) { impl_->push_sample(data,timestamp,pushthrough); }
void stream_outlet::push_numeric_raw(void *data, double timestamp, bool pushthrough) { impl_->push_numeric_raw(data,timestamp,pushthrough); }
bool stream_outlet::have_consumers() { return impl_->have_consumers(); }
bool stream_outlet::wait_for_consumers(double timeout) { return impl_->wait_for_consumers(timeout); }

