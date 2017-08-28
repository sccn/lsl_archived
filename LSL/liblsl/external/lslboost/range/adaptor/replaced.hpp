// Boost.Range library
//
//  Copyright Neil Groves 2007. Use, modification and
//  distribution is subject to the Boost Software License, Version
//  1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.lslboost.org/LICENSE_1_0.txt)
//
// For more information, see http://www.lslboost.org/libs/range/
//

#ifndef BOOST_RANGE_ADAPTOR_REPLACED_IMPL_HPP_INCLUDED
#define BOOST_RANGE_ADAPTOR_REPLACED_IMPL_HPP_INCLUDED

#include <lslboost/config.hpp>
#include <lslboost/range/adaptor/argument_fwd.hpp>
#include <lslboost/range/iterator_range.hpp>
#include <lslboost/range/begin.hpp>
#include <lslboost/range/end.hpp>
#include <lslboost/range/value_type.hpp>
#include <lslboost/iterator/iterator_adaptor.hpp>
#include <lslboost/iterator/transform_iterator.hpp>

namespace lslboost
{
    namespace range_detail
    {
        template< class Value >
        class replace_value
        {
        public:
            typedef const Value& result_type;
            typedef const Value& first_argument_type;

            replace_value(const Value& from, const Value& to)
                :   m_from(from), m_to(to)
            {
            }

            const Value& operator()(const Value& x) const
            {
                return (x == m_from) ? m_to : x;
            }

        private:
            Value m_from;
            Value m_to;
        };

        template< class R >
        class replaced_range :
            public lslboost::iterator_range<
                lslboost::transform_iterator<
                    replace_value< BOOST_DEDUCED_TYPENAME range_value<R>::type >,
                    BOOST_DEDUCED_TYPENAME range_iterator<R>::type > >
        {
        private:
            typedef replace_value< BOOST_DEDUCED_TYPENAME range_value<R>::type > Fn;

            typedef lslboost::iterator_range<
                lslboost::transform_iterator<
                    replace_value< BOOST_DEDUCED_TYPENAME range_value<R>::type >,
                    BOOST_DEDUCED_TYPENAME range_iterator<R>::type > > base_t;

        public:
            typedef BOOST_DEDUCED_TYPENAME range_value<R>::type value_type;

            replaced_range( R& r, value_type from, value_type to )
                : base_t( make_transform_iterator( lslboost::begin(r), Fn(from, to) ),
                          make_transform_iterator( lslboost::end(r), Fn(from, to) ) )
            { }
        };

        template< class T >
        class replace_holder : public holder2<T>
        {
        public:
            replace_holder( const T& from, const T& to )
                : holder2<T>(from, to)
            { }
        private:
            // not assignable
            void operator=(const replace_holder&);
        };

        template< class InputRng >
        inline replaced_range<InputRng>
        operator|( InputRng& r,
                   const replace_holder<BOOST_DEDUCED_TYPENAME range_value<InputRng>::type>& f )
        {
            return replaced_range<InputRng>(r, f.val1, f.val2);
        }

        template< class InputRng >
        inline replaced_range<const InputRng>
        operator|( const InputRng& r,
                   const replace_holder<BOOST_DEDUCED_TYPENAME range_value<InputRng>::type>& f )
        {
            return replaced_range<const InputRng>(r, f.val1, f.val2);
        }
    } // 'range_detail'

    using range_detail::replaced_range;

    namespace adaptors
    {
        namespace
        {
            const range_detail::forwarder2<range_detail::replace_holder>
                replaced =
                    range_detail::forwarder2<range_detail::replace_holder>();
        }

        template<class InputRange>
        inline replaced_range<InputRange>
        replace(InputRange& rng,
                BOOST_DEDUCED_TYPENAME range_value<InputRange>::type from,
                BOOST_DEDUCED_TYPENAME range_value<InputRange>::type to)
        {
            return replaced_range<InputRange>(rng, from, to);
        }

        template<class InputRange>
        inline replaced_range<const InputRange>
        replace(const InputRange& rng,
                BOOST_DEDUCED_TYPENAME range_value<const InputRange>::type from,
                BOOST_DEDUCED_TYPENAME range_value<const InputRange>::type to)
        {
            return replaced_range<const InputRange>(rng, from ,to);
        }

    } // 'adaptors'
} // 'lslboost'

#endif // include guard
