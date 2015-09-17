//
// cancellable_streambuf.hpp
// ~~~~~~~~~~~~~~~~~~~~~~~~~~
//
// This is a carbon copy of basic_socket_streambuf.hpp, adding a cancel() member function
// (an removing support for unbuffered I/O).
//
// Copyright (c) 2003-2011 Christopher M. Kohlhoff (chris at kohlhoff dot com)
// Modified by Christian A. Kothe, 2012
//
// Distributed under the boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef CANCELLABLE_STREAMBUF_HPP
#define CANCELLABLE_STREAMBUF_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include "cancellation.h"
#include <streambuf>
#include <exception>
#include <set>
#include <boost/asio/detail/config.hpp>
#include <boost/utility/base_from_member.hpp>
#include <boost/asio/basic_socket.hpp>
#include <boost/asio/detail/array.hpp>
#include <boost/asio/detail/throw_error.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/asio/stream_socket_service.hpp>
#include <boost/thread.hpp>

#ifdef LSLBOOST_NAMESPACE_DECLARED
namespace lslboost {
#else
namespace boost {
#endif
	namespace asio {

		/// Iostream streambuf for a socket.
		template <typename Protocol, typename StreamSocketService=stream_socket_service<Protocol> >
		class cancellable_streambuf: public std::streambuf, private boost::base_from_member<io_service>, public basic_socket<Protocol, StreamSocketService>, public lsl::cancellable_obj {
		public:
			/// The endpoint type.
			typedef typename Protocol::endpoint endpoint_type;

			/// Construct a cancellable_streambuf without establishing a connection.
			cancellable_streambuf(): basic_socket<Protocol, StreamSocketService>(boost::base_from_member<boost::asio::io_service>::member), cancel_issued_(false), cancel_started_(false) {
				init_buffers();
			}

			/// Destructor flushes buffered data.
			virtual ~cancellable_streambuf() {
				// no cancel() can fire after this call
				unregister_from_all();
				if (pptr() != pbase())
					overflow(traits_type::eof());
			}

			/**
			* Cancel the current stream operations destructively.
			* All blocking operations will fail after a cancel() has been issued, 
			* and the stream buffer cannot be reused.
			*/
			void cancel() {
				cancel_issued_ = true;
				boost::lock_guard<boost::recursive_mutex> lock(cancel_mut_);
				cancel_started_ = false;
				this->get_service().get_io_service().post(boost::bind(&cancellable_streambuf::close_if_open,this));
			}


			/// Establish a connection.
			/**
			* This function establishes a connection to the specified endpoint.
			*
			* @return \c this if a connection was successfully established, a null
			* pointer otherwise.
			*/
			cancellable_streambuf<Protocol, StreamSocketService>* connect(const endpoint_type& endpoint) {
				{
					boost::lock_guard<boost::recursive_mutex> lock(cancel_mut_);
					if (cancel_issued_)
						throw std::runtime_error("Attempt to connect() a cancellable_streambuf after it has been cancelled.");

					init_buffers();
					this->basic_socket<Protocol, StreamSocketService>::close(ec_);

					io_handler handler = { this };
					this->basic_socket<Protocol, StreamSocketService>::async_connect(endpoint, handler);
					this->get_service().get_io_service().reset();
				}
				ec_ = boost::asio::error::would_block;
				do this->get_service().get_io_service().run_one();
				while (!cancel_issued_ && ec_ == boost::asio::error::would_block);
				return !ec_ ? this : 0;
			}

			/// Close the connection.
			/**
			* @return \c this if a connection was successfully established, a null
			* pointer otherwise.
			*/
			cancellable_streambuf<Protocol, StreamSocketService>* close() {
				sync();
				this->basic_socket<Protocol, StreamSocketService>::close(ec_);
				if (!ec_)
					init_buffers();
				return !ec_ ? this : 0;
			}

			/// Get the last error associated with the stream buffer.
			/**
			* @return An \c error_code corresponding to the last error from the stream
			* buffer.
			*/
			const boost::system::error_code& puberror() const { return error(); }

		protected:
			/// Close the socket if it's open.
			void close_if_open() {
				if (!cancel_started_ && this->is_open()) {
					cancel_started_ = true;
					close();
				}
			}

			/// This function makes sure that a cancellation, if issued, is not being eaten by the io_service reset()
			void protected_reset() {
				boost::lock_guard<boost::recursive_mutex> lock(cancel_mut_);
				// if the cancel() comes between completion of a run_one() and this call, close will be issued right here at the next opportunity
				if (cancel_issued_)
					close_if_open();
				this->get_service().get_io_service().reset();
				// if the cancel() comes between this call and a completion of run_one(), the posted close will be processed by the run_one
			}

			int_type underflow() {
				if (gptr() == egptr()) {
					io_handler handler = { this };
					this->get_service().async_receive(this->get_implementation(),
						boost::asio::buffer(boost::asio::buffer(get_buffer_) + putback_max),
						0, handler);

					ec_ = boost::asio::error::would_block;
					protected_reset(); // line changed for lsl
					do this->get_service().get_io_service().run_one();
					while (ec_ == boost::asio::error::would_block);
					if (ec_)
						return traits_type::eof();

					setg(&get_buffer_[0], &get_buffer_[0] + putback_max,
						&get_buffer_[0] + putback_max + bytes_transferred_);
					return traits_type::to_int_type(*gptr());
				}
				else
					return traits_type::eof();
			}

			int_type overflow(int_type c) {
				// Send all data in the output buffer.
				boost::asio::const_buffer buffer =
					boost::asio::buffer(pbase(), pptr() - pbase());
				while (boost::asio::buffer_size(buffer) > 0) {
					io_handler handler = { this };
					this->get_service().async_send(this->get_implementation(),
						boost::asio::buffer(buffer), 0, handler);
					ec_ = boost::asio::error::would_block;
					protected_reset(); // line changed for lsl
					do this->get_service().get_io_service().run_one();
					while (ec_ == boost::asio::error::would_block);
					if (ec_)
						return traits_type::eof();
					buffer = buffer + bytes_transferred_;
				}
				setp(&put_buffer_[0], &put_buffer_[0] + put_buffer_.size());

				// If the new character is eof then our work here is done.
				if (traits_type::eq_int_type(c, traits_type::eof()))
					return traits_type::not_eof(c);

				// Add the new character to the output buffer.
				*pptr() = traits_type::to_char_type(c);
				pbump(1);
				return c;
			}

			int sync() { return overflow(traits_type::eof()); }

			std::streambuf* setbuf(char_type*, std::streamsize) {
				// this feature was stripped out...
				return 0;
			}

			/// Get the last error associated with the stream buffer.
			/**
			* @return An \c error_code corresponding to the last error from the stream
			* buffer.
			*/
			virtual const boost::system::error_code& error() const { return ec_; }

			//private:
			void init_buffers() {
				setg(&get_buffer_[0],
					&get_buffer_[0] + putback_max,
					&get_buffer_[0] + putback_max);
				setp(&put_buffer_[0], &put_buffer_[0] + put_buffer_.size());
			}

			struct io_handler;
			friend struct io_handler;
			struct io_handler {
				cancellable_streambuf* this_;
				void operator()(const boost::system::error_code& ec, std::size_t bytes_transferred = 0) {
					this_->ec_ = ec;
					this_->bytes_transferred_ = bytes_transferred;
				}
			};

			enum { putback_max = 8 };
			enum { buffer_size = 512 };
			boost::asio::detail::array<char, buffer_size> get_buffer_;
			boost::asio::detail::array<char, buffer_size> put_buffer_;
			boost::system::error_code ec_;
			std::size_t bytes_transferred_;
			bool cancel_issued_;
			bool cancel_started_;
			boost::recursive_mutex cancel_mut_;
		};

	} // namespace asio
} // namespace boost

#endif // CANCELLABLE_STREAMBUF_HPP

