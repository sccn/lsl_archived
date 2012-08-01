#include "resolver_impl.h"
#include "version.h"

//
// === Implementation of the continuous_resolver class ===
//

using namespace lsl;

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

