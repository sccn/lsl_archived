#ifndef CANCELLATION_H
#define CANCELLATION_H

#include <iostream>
#include <set>
#include <boost/thread.hpp>


namespace lsl { 
	/// Exception class that indicates that an operation was performed on an registry that is being shut down.
	class shutdown_error: public std::runtime_error {
	public:
		explicit shutdown_error(const std::string &msg): std::runtime_error(msg) {}
	};


	/// A registry for objects that can be cancelled.
	class cancellable_registry {
	public:
		cancellable_registry(): shutdown_issued_(false) {}
		virtual ~cancellable_registry() {};

		/// Invoke cancel() on all currently registered objects.
		void cancel_all_registered();

		/// Invoke a cancel on all currently registered objects
		/// and prevent future object registration.
		void cancel_and_shutdown();
	private:
		friend class cancellable_obj;

		/// Register a cancellable object.
		void register_cancellable(class cancellable_obj *o) {
			boost::lock_guard<boost::recursive_mutex> lock(state_mut_);
			if (shutdown_issued_)
				throw shutdown_error("The registry has begun to shut down; no new registrations possible.");
			cancellables_.insert(o);
		}
		/// Unregister a cancellable object.
		void unregister_cancellable(class cancellable_obj *o) {
			boost::lock_guard<boost::recursive_mutex> lock(state_mut_);
			cancellables_.erase(o);
		}

		bool shutdown_issued_;						// whether a shutdown has been issued
		std::set<cancellable_obj*> cancellables_;	// a set of objects that we have to cancel upon re-resolves & disengage
		boost::recursive_mutex state_mut_;			// mutex to protect the registry's state
	};


	/// A cancellable object.
	class cancellable_obj {
	public:
		/// Cancel method.
		virtual void cancel() { }

		/// Unregister at destruction.
		virtual ~cancellable_obj() { unregister_from_all(); }

		/// Register at some registry.
		/// IMPORTANT: The registry must outlive the cancellable.
		void register_at(cancellable_registry *reg) { 
			reg->register_cancellable(this);
			registered_at_.insert(reg); 
		}

		/// IMPORTANT: If your registered cancel() operation may require some resources of your 
		/// derived class(es), you must call unregister_from_all() *before* you destroy any of these resources...
		void unregister_from_all() { 
			try {
				for (std::set<cancellable_registry*>::iterator i=registered_at_.begin(); i != registered_at_.end(); i++)
					(*i)->unregister_cancellable(this);
				registered_at_.clear();
			} catch(std::exception &e) {
				std::cerr << "Unexpected error trying to unregister a cancellable object from its registry:" << e.what() << std::endl;
			}
		}

	private:
		// set of registries where we are registered
		std::set<cancellable_registry*> registered_at_;
	};

	/// Cancel all registered objects.
	inline void cancellable_registry::cancel_all_registered() {
		boost::lock_guard<boost::recursive_mutex> lock(state_mut_);
		std::set<cancellable_obj*> copy(cancellables_);
		for (std::set<cancellable_obj*>::iterator i=copy.begin(); i != copy.end(); i++)
			if (cancellables_.find(*i) != cancellables_.end())
				(*i)->cancel();
	}

	/// Cancel and prevent future object registrations.
	inline void cancellable_registry::cancel_and_shutdown() {
		boost::lock_guard<boost::recursive_mutex> lock(state_mut_);
		shutdown_issued_ = true;
		cancel_all_registered();
	}

}
#endif
