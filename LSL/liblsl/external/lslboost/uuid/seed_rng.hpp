// Boost seed_rng.hpp header file  ----------------------------------------------//

// Copyright 2007 Andy Tompkins.
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.lslboost.org/LICENSE_1_0.txt)

// Revision History
//  09 Nov 2007 - Initial Revision
//  25 Feb 2008 - moved to namespace lslboost::uuids::detail
//  28 Nov 2009 - disabled deprecated warnings for MSVC
//  28 Jul 2014 - fixed valgrind warnings and better entropy sources for MSVC

// seed_rng models a UniformRandomNumberGenerator (see Boost.Random).
// Random number generators are hard to seed well.  This is intended to provide
// good seed values for random number generators.
// It creates random numbers from a sha1 hash of data from a variary of sources,
// all of which are standard function calls.  It produces random numbers slowly.
// Peter Dimov provided the details of sha1_random_digest_().
// see http://archives.free.net.ph/message/20070507.175609.4c4f503a.en.html

#ifndef BOOST_UUID_SEED_RNG_HPP
#define BOOST_UUID_SEED_RNG_HPP

#include <lslboost/config.hpp>
#include <cstring> // for memcpy
#include <limits>
#include <ctime> // for time_t, time, clock_t, clock
#include <cstdlib> // for rand
#include <cstdio> // for FILE, fopen, fread, fclose
#include <lslboost/core/noncopyable.hpp>
#include <lslboost/uuid/sha1.hpp>
//#include <lslboost/nondet_random.hpp> //forward declare lslboost::random::random_device

// can't use lslboost::generator_iterator since lslboost::random number seed(Iter&, Iter)
// functions need a last iterator
//#include <lslboost/generator_iterator.hpp>
# include <lslboost/iterator/iterator_facade.hpp>

#if defined(_MSC_VER)
#   pragma warning(push) // Save warning settings.
#   pragma warning(disable : 4996) // Disable deprecated std::fopen
#   include <lslboost/detail/winapi/crypt.hpp> // for CryptAcquireContextA, CryptGenRandom, CryptReleaseContext
#   include <lslboost/detail/winapi/timers.hpp>
#   include <lslboost/detail/winapi/process.hpp>
#   include <lslboost/detail/winapi/thread.hpp>
#   pragma comment(lib, "advapi32.lib")
#else 
#   include <sys/time.h>  // for gettimeofday
#   include <sys/types.h> // for pid_t
#   include <unistd.h>    // for getpid()
#endif

#ifdef BOOST_NO_STDC_NAMESPACE
namespace std {
    using ::memcpy;
    using ::time_t;
    using ::time;
    using ::clock_t;
    using ::clock;
    using ::rand;
    using ::FILE;
    using ::fopen;
    using ::fread;
    using ::fclose;
} //namespace std
#endif

// forward declare random number generators
namespace lslboost { namespace random {
class random_device;
}} //namespace lslboost::random

namespace lslboost {
namespace uuids {
namespace detail {

// should this be part of Boost.Random?
class seed_rng: private lslboost::noncopyable
{
public:
    typedef unsigned int result_type;
    BOOST_STATIC_CONSTANT(bool, has_fixed_range = false);

public:
    // note: rd_ intentionally left uninitialized
    seed_rng() BOOST_NOEXCEPT
        : rd_index_(5)
        , random_(NULL)
    {
#if defined(BOOST_WINDOWS)
        if (!lslboost::detail::winapi::CryptAcquireContextA(
                    &random_,
                    NULL,
                    NULL,
                    lslboost::detail::winapi::PROV_RSA_FULL_,
                    lslboost::detail::winapi::CRYPT_VERIFYCONTEXT_ | lslboost::detail::winapi::CRYPT_SILENT_))
        {
            random_ = NULL;
        }
#else
        random_ = std::fopen( "/dev/urandom", "rb" );
#endif

        std::memset(rd_, 0, sizeof(rd_));
    }
    
    ~seed_rng() BOOST_NOEXCEPT
    {
        if (random_) {
#if defined(BOOST_WINDOWS)
            lslboost::detail::winapi::CryptReleaseContext(random_, 0);
#else
            std::fclose(random_);
#endif
        }
    }

    result_type min BOOST_PREVENT_MACRO_SUBSTITUTION () const BOOST_NOEXCEPT
    {
        return (std::numeric_limits<result_type>::min)();
    }
    result_type max BOOST_PREVENT_MACRO_SUBSTITUTION () const BOOST_NOEXCEPT
    {
        return (std::numeric_limits<result_type>::max)();
    }

    result_type operator()()
    {
        if (rd_index_ >= 5) {
            //get new digest
            sha1_random_digest_();

            rd_index_ = 0;
        }

        return rd_[rd_index_++];
    }

private:
    BOOST_STATIC_CONSTANT(std::size_t, internal_state_size = 5);
    inline void ignore_size(size_t) {}

    static unsigned int * sha1_random_digest_state_()
    {
        static unsigned int state[ internal_state_size ];
        return state;
    }

    void sha1_random_digest_()
    {
        lslboost::uuids::detail::sha1 sha;


        if (random_)
        {
            // intentionally left uninitialized
            unsigned char state[ 20 ];
#if defined(BOOST_WINDOWS)
            lslboost::detail::winapi::CryptGenRandom(random_, sizeof(state), state);
#else
            ignore_size(std::fread( state, 1, sizeof(state), random_ ));
#endif
            sha.process_bytes( state, sizeof( state ) );
        }

        {
            // Getting enropy from some system specific sources
#if defined(BOOST_WINDOWS)
            lslboost::detail::winapi::DWORD_ procid = lslboost::detail::winapi::GetCurrentProcessId();
            sha.process_bytes( (unsigned char const*)&procid, sizeof( procid ) );

            lslboost::detail::winapi::DWORD_ threadid = lslboost::detail::winapi::GetCurrentThreadId();
            sha.process_bytes( (unsigned char const*)&threadid, sizeof( threadid ) );

            lslboost::detail::winapi::LARGE_INTEGER_ ts;
            ts.QuadPart = 0;
            lslboost::detail::winapi::QueryPerformanceCounter( &ts );
            sha.process_bytes( (unsigned char const*)&ts, sizeof( ts ) );

            std::time_t tm = std::time( 0 );
            sha.process_bytes( (unsigned char const*)&tm, sizeof( tm ) );
#else
            pid_t pid = getpid();
            sha.process_bytes( (unsigned char const*)&pid, sizeof( pid ) );

            timeval ts;
            gettimeofday(&ts, NULL); // We do not use `clock_gettime` to avoid linkage with -lrt
            sha.process_bytes( (unsigned char const*)&ts, sizeof( ts ) );
#endif
        }


        unsigned int * ps = sha1_random_digest_state_();
        sha.process_bytes( ps, internal_state_size * sizeof( unsigned int ) );
        sha.process_bytes( (unsigned char const*)&ps, sizeof( ps ) );

        {
            std::clock_t ck = std::clock();
            sha.process_bytes( (unsigned char const*)&ck, sizeof( ck ) );
        }

        {
            unsigned int rn[] =
                { static_cast<unsigned int>(std::rand())
                , static_cast<unsigned int>(std::rand())
                , static_cast<unsigned int>(std::rand())
                };
            sha.process_bytes( (unsigned char const*)rn, sizeof( rn ) );
        }

        {
            unsigned int * p = new unsigned int;
            sha.process_bytes( (unsigned char const*)&p, sizeof( p ) );
            delete p;

            const seed_rng* this_ptr = this;
            sha.process_bytes( (unsigned char const*)&this_ptr, sizeof( this_ptr ) );
            sha.process_bytes( (unsigned char const*)&std::rand, sizeof( void(*)() ) );
        }

        sha.process_bytes( (unsigned char const*)rd_, sizeof( rd_ ) );

        unsigned int digest[ 5 ];
        sha.get_digest( digest );

        for( int i = 0; i < 5; ++i )
        {
            // harmless data race
            ps[ i ] ^= digest[ i ];
            rd_[ i ] ^= digest[ i ];
        }
    }

private:
    unsigned int rd_[5];
    int rd_index_;

#if defined(BOOST_WINDOWS)
    lslboost::detail::winapi::HCRYPTPROV_ random_;
#else
    std::FILE * random_;
#endif
};

// almost a copy of lslboost::generator_iterator
// but default constructor sets m_g to NULL
template <class Generator>
class generator_iterator
  : public iterator_facade<
        generator_iterator<Generator>
      , typename Generator::result_type
      , single_pass_traversal_tag
      , typename Generator::result_type const&
    >
{
    typedef iterator_facade<
        generator_iterator<Generator>
      , typename Generator::result_type
      , single_pass_traversal_tag
      , typename Generator::result_type const&
    > super_t;

 public:
    generator_iterator() : m_g(NULL), m_value(0) {}
    generator_iterator(Generator* g) : m_g(g), m_value((*m_g)()) {}

    void increment()
    {
        m_value = (*m_g)();
    }

    const typename Generator::result_type&
    dereference() const
    {
        return m_value;
    }

    bool equal(generator_iterator const& y) const
    {
        return this->m_g == y.m_g && this->m_value == y.m_value;
    }

 private:
    Generator* m_g;
    typename Generator::result_type m_value;
};

// seed() seeds a random number generator with good seed values

template <typename UniformRandomNumberGenerator>
inline void seed(UniformRandomNumberGenerator& rng)
{
    seed_rng seed_gen;
    generator_iterator<seed_rng> begin(&seed_gen);
    generator_iterator<seed_rng> end;
    rng.seed(begin, end);
}

// random_device does not / can not be seeded
template <>
inline void seed<lslboost::random::random_device>(lslboost::random::random_device&) {}

// random_device does not / can not be seeded
template <>
inline void seed<seed_rng>(seed_rng&) {}

}}} //namespace lslboost::uuids::detail

#if defined(_MSC_VER)
#pragma warning(pop) // Restore warnings to previous state.
#endif

#endif
