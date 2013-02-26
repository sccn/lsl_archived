#include "stream_info_impl.h"

//
// === Implementation of the stream_info class ===
//

namespace boost = lslboost;
using namespace lsl;

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

stream_info &stream_info::operator=(const stream_info &rhs) {
	if (this == &rhs) 
		return *this;
	impl_ = new stream_info_impl(*rhs.impl_);
	return *this;
}

