#include "stream_inlet_impl.h"


// === implementation of the stream_inlet class ===

using namespace lsl;

// boilerplate code calling the private implementation
stream_inlet::stream_inlet(const stream_info &info, int max_buflen, int max_chunklen, bool recover): impl_(new stream_inlet_impl(*info.impl(), info.nominal_srate()?(int)(info.nominal_srate()*max_buflen):max_buflen*100, max_chunklen, recover)) { }
stream_inlet::~stream_inlet() { delete impl_; }
stream_info stream_inlet::info(double timeout) { return stream_info(&impl_->info(timeout)); }
void stream_inlet::open_stream(double timeout) { impl_->open_stream(timeout); }
void stream_inlet::close_stream() { impl_->close_stream(); }
double stream_inlet::time_correction(double timeout) { return impl_->time_correction(timeout); }
double stream_inlet::pull_sample(std::vector<float> &sample, double timeout) { return impl_->pull_sample(sample,timeout); }
double stream_inlet::pull_sample(std::vector<double> &sample, double timeout) { return impl_->pull_sample(sample,timeout); }
double stream_inlet::pull_sample(std::vector<long> &sample, double timeout) { return impl_->pull_sample(sample,timeout); }
double stream_inlet::pull_sample(std::vector<int> &sample, double timeout) { return impl_->pull_sample(sample,timeout); }
double stream_inlet::pull_sample(std::vector<short> &sample, double timeout) { return impl_->pull_sample(sample,timeout); }
double stream_inlet::pull_sample(std::vector<char> &sample, double timeout) { return impl_->pull_sample(sample,timeout); }
double stream_inlet::pull_sample(std::vector<std::string> &sample, double timeout) { return impl_->pull_sample(sample,timeout); }
double stream_inlet::pull_sample(float *buffer, int buffer_elements, double timeout) { return impl_->pull_sample(buffer,buffer_elements,timeout); }
double stream_inlet::pull_sample(double *buffer, int buffer_elements, double timeout) { return impl_->pull_sample(buffer,buffer_elements,timeout); }
double stream_inlet::pull_sample(long *buffer, int buffer_elements, double timeout) { return impl_->pull_sample(buffer,buffer_elements,timeout); }
double stream_inlet::pull_sample(int *buffer, int buffer_elements, double timeout) { return impl_->pull_sample(buffer,buffer_elements,timeout); }
double stream_inlet::pull_sample(short *buffer, int buffer_elements, double timeout) { return impl_->pull_sample(buffer,buffer_elements,timeout); }
double stream_inlet::pull_sample(char *buffer, int buffer_elements, double timeout) { return impl_->pull_sample(buffer,buffer_elements,timeout); }
double stream_inlet::pull_sample(std::string *buffer, int buffer_elements, double timeout) { return impl_->pull_sample(buffer,buffer_elements,timeout); }
double stream_inlet::pull_numeric_raw(void *sample, int buffer_bytes, double timeout) { return impl_->pull_numeric_raw(sample,buffer_bytes,timeout); }
std::size_t stream_inlet::samples_available() { return impl_->samples_available(); };
bool stream_inlet::was_clock_reset() { return impl_->was_clock_reset(); };

