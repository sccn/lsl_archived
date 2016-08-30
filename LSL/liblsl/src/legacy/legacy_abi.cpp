#include "../common.h"
#include "../stream_outlet_impl.h"
#include "../stream_inlet_impl.h"
#include "../stream_info_impl.h"
#include "legacy_abi.h"
#include "../resolver_impl.h"
#include "../pugixml/pugixml.hpp"

#define NO_EXPLICIT_TEMPLATE_INSTANTIATION

using namespace lsl;
using namespace pugi;


// === Implementation of the stream_info class ===

// boilerplate code calling the private implementation
stream_info::stream_info(): impl_(new stream_info_impl())  {  }
stream_info::stream_info(const stream_info &rhs): impl_(new stream_info_impl(*rhs.impl_)) {  }
stream_info::stream_info(const std::string &name, const std::string &type, int channel_count, double nominal_srate, channel_format_t channel_format, const std::string &source_id): impl_(new stream_info_impl(name,type,channel_count,nominal_srate,channel_format,source_id)) { }
stream_info::stream_info(const stream_info_impl *impl): impl_(new stream_info_impl(*impl)) { }
stream_info::stream_info(const stream_info_impl &impl): impl_(new stream_info_impl(impl)) { }
stream_info::~stream_info() { delete impl_; }
const std::string &stream_info::name() const { return impl_->name(); }
const std::string &stream_info::type() const { return impl_->type(); }
int stream_info::channel_count() const { return impl_->channel_count(); }
double stream_info::nominal_srate() const { return impl_->nominal_srate(); }
channel_format_t stream_info::channel_format() const { return impl_->channel_format(); }
const std::string &stream_info::source_id() const { return impl_->source_id(); }
int stream_info::version() const { return impl_->version(); }
const std::string &stream_info::uid() const { return impl_->uid(); }
double stream_info::created_at() const { return impl_->created_at(); }
const std::string &stream_info::session_id() const { return impl_->session_id(); }
const std::string &stream_info::hostname() const { return impl_->hostname(); }
int stream_info::channel_bytes() const { return impl_->channel_bytes(); }
int stream_info::sample_bytes() const { return impl_->sample_bytes(); }
xml_element stream_info::desc() { return impl_->desc(); }
xml_element stream_info::desc() const { return impl_->desc(); }
std::string stream_info::as_xml() { return impl_->to_fullinfo_message(); }
stream_info_impl *stream_info::impl() { return impl_; }
const stream_info_impl *stream_info::impl() const { return impl_; }
stream_info &stream_info::operator=(const stream_info &rhs) { if (this != &rhs) impl_ = new stream_info_impl(*rhs.impl_); return *this; }


// === implementation of the xml_element class ===

// boilerplate code calling the private implementation
xml_element::xml_element(): node_(0) {}
xml_element::xml_element(const xml_node &node): node_(node.internal_object()) {}
xml_element xml_element::first_child() const { return xml_node(node_).first_child(); }
xml_element xml_element::last_child() const { return xml_node(node_).last_child(); }
xml_element xml_element::next_sibling() const { return xml_node(node_).next_sibling(); }
xml_element xml_element::previous_sibling() const { return xml_node(node_).previous_sibling(); }
xml_element xml_element::parent() const { return xml_node(node_).parent(); }
xml_element xml_element::child(const char *name) const { return xml_node(node_).child(name); }
xml_element xml_element::next_sibling(const char *name) const { return xml_node(node_).next_sibling(name); }
xml_element xml_element::previous_sibling(const char *name) const { return xml_node(node_).previous_sibling(name); }
bool xml_element::empty() const { return xml_node(node_).empty(); }
bool xml_element::is_text() const { return xml_node(node_).type() != node_element; }
const char *xml_element::name() const { return xml_node(node_).name(); }
const char *xml_element::value() const  { return xml_node(node_).value(); }
const char_t* xml_element::child_value() const { return xml_node(node_).child_value(); }
const char_t* xml_element::child_value(const char *name) const { return xml_node(node_).child_value(name); }
bool xml_element::set_name(const char *rhs) { return xml_node(node_).set_name(rhs); }
bool xml_element::set_value(const char *rhs) { return xml_node(node_).set_value(rhs); }
bool xml_element::set_child_value(const char *name,const char *rhs) {
	xml_node n = xml_node(node_).child(name);
	if (xml_node n2 = n.first_child())
		return n2.set_value(rhs);
	else
		return n.append_child(node_pcdata).set_value(rhs);
}
xml_element xml_element::append_child(const char *name) { return xml_node(node_).append_child(name); }
xml_element xml_element::prepend_child(const char *name) { return xml_node(node_).prepend_child(name); }
xml_element xml_element::append_copy(const xml_element &e) { return xml_node(node_).append_copy(xml_node(e.node_)); }
xml_element xml_element::prepend_copy(const xml_element &e) { return xml_node(node_).prepend_copy(xml_node(e.node_)); }
void xml_element::remove_child(const char *name) { xml_node(node_).remove_child(name); }
void xml_element::remove_child(const xml_element &e) { xml_node(node_).remove_child(xml_node(e.node_)); }
pugi::xml_node_struct *xml_element::ptr() { return node_; }

/// convenience function for document construction
xml_element xml_element::append_child_value(const char *name, const char *value) { 
	xml_node result = xml_node(node_).append_child(name);
	result.append_child(node_pcdata).set_value(value);
	return xml_node(node_);
}

/// convenience function for document construction
xml_element xml_element::prepend_child_value(const char *name, const char *value) { 
	xml_node result = xml_node(node_).prepend_child(name);
	result.append_child(node_pcdata).set_value(value);
	return xml_node(node_);
}


// === implementation of the stream_outlet class ===

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



// === implementation of the stream_inlet class ===

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



// === Implementation of the free-standing functions in lsl_cpp.h ===
using std::string;
using std::vector;

LIBLSL_CPP_API int lsl::protocol_version() { return api_config::get_instance()->use_protocol_version(); }

LIBLSL_CPP_API int lsl::library_version() { return LSL_LIBRARY_VERSION; }

LIBLSL_CPP_API double lsl::local_clock() { return lsl_clock(); }

LIBLSL_CPP_API std::vector<stream_info> lsl::resolve_streams(double wait_time) {
	// create a new resolver
	resolver_impl resolver;
	// build a new query
	std::ostringstream os; os << "session_id='" << api_config::get_instance()->session_id() << "'";
	// invoke it
	std::vector<stream_info_impl> tmp = resolver.resolve_oneshot(os.str(),0,wait_time);
	return std::vector<stream_info>(tmp.begin(),tmp.end());
}

LIBLSL_CPP_API std::vector<stream_info> lsl::resolve_stream(const std::string &prop, const std::string &value, int minimum, double timeout) {
	resolver_impl resolver;
	std::ostringstream os; os << "session_id='" << api_config::get_instance()->session_id() << "' and " << prop << "='" << value << "'";
	std::vector<stream_info_impl> tmp = resolver.resolve_oneshot(os.str(),minimum,timeout);
	return std::vector<stream_info>(tmp.begin(),tmp.end());
}

LIBLSL_CPP_API std::vector<stream_info> lsl::resolve_stream(const std::string &pred, int minimum, double timeout) {
	resolver_impl resolver;
	std::ostringstream os; os << "session_id='" << api_config::get_instance()->session_id() << "' and " << pred;
	std::vector<stream_info_impl> tmp = resolver.resolve_oneshot(os.str(),minimum,timeout);
	return std::vector<stream_info>(tmp.begin(),tmp.end());
}



// === Implementation of the continuous_resolver class ===

continuous_resolver::continuous_resolver(double forget_after):  impl_(new resolver_impl()) { 
	std::ostringstream os; os << "session_id='" << api_config::get_instance()->session_id() << "'";
	impl_->resolve_continuous(os.str(),forget_after);
}

continuous_resolver::continuous_resolver(const std::string &prop, const std::string &value, double forget_after):  impl_(new resolver_impl()) { 
	std::ostringstream os; os << "session_id='" << api_config::get_instance()->session_id() << "' and " << prop << "='" << value << "'";
	impl_->resolve_continuous(os.str(),forget_after);
}

continuous_resolver::continuous_resolver(const std::string &pred, double forget_after):  impl_(new resolver_impl()) { 
	std::ostringstream os; os << "session_id='" << api_config::get_instance()->session_id() << "' and " << pred;
	impl_->resolve_continuous(os.str(),forget_after);
}

continuous_resolver::~continuous_resolver() { delete impl_; }

std::vector<stream_info> continuous_resolver::results() { 
	std::vector<stream_info_impl> tmp = impl_->results();
	return std::vector<stream_info>(tmp.begin(),tmp.end());
}
