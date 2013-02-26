// Boost.Range library
//
//  Copyright Thorsten Ottosen, Neil Groves 2006 - 2008. Use, modification and
//  distribution is subject to the Boost Software License, Version
//  1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.lslboost.org/LICENSE_1_0.txt)
//
// For more information, see http://www.lslboost.org/libs/range/
//

#ifndef BOOST_RANGE_ADAPTOR_TRANSFORMED_HPP
#define BOOST_RANGE_ADAPTOR_TRANSFORMED_HPP

#include <lslboost/range/adaptor/argument_fwd.hpp>
#include <lslboost/range/iterator_range.hpp>
#include <lslboost/iterator/transform_iterator.hpp>
#include <lslboost/utility/result_of.hpp>

namespace lslboost
{
    namespace range_detail
    {

        template< class F, class R >
        struct transformed_range :
            public lslboost::iterator_range<
                      lslboost::transform_iterator< F,
                          BOOST_DEDUCED_TYPENAME range_iterator<R>::type
                                              >
                                         >
        {
        private:
            typedef lslboost::iterator_range<
                      lslboost::transform_iterator< F,
                        BOOST_DEDUCED_TYPENAME range_iterator<R>::type
                                              >
                                         >
                base;

        public:
            typedef F transform_fn_type;
            typedef R source_range_type;

            transformed_range( F f, R& r )
                : base( lslboost::make_transform_iterator( lslboost::begin(r), f ),
                        lslboost::make_transform_iterator( lslboost::end(r), f ) )

            { }
        };

        template< class T >
        struct transform_holder : holder<T>
        {
            transform_holder( T r ) : holder<T>(r)
            { }
        };

        template< class InputRng, class UnaryFunction >
        inline transformed_range<UnaryFunction,InputRng>
        operator|( InputRng& r,
                   const transform_holder<UnaryFunction>& f )
        {
            return transformed_range<UnaryFunction,InputRng>( f.val, r );
        }

        template< class InputRng, class UnaryFunction >
        inline transformed_range<UnaryFunction, const InputRng>
        operator|( const InputRng& r,
                   const transform_holder<UnaryFunction>& f )
        {
           return transformed_range<UnaryFunction, const InputRng>( f.val, r );
        }

    } // 'range_detail'

    using range_detail::transformed_range;

    namespace adaptors
    {
        namespace
        {
            const range_detail::forwarder<range_detail::transform_holder>
                    transformed =
                      range_detail::forwarder<range_detail::transform_holder>();
        }

        template<class UnaryFunction, class InputRange>
        inline transformed_range<UnaryFunction, InputRange>
        transform(InputRange& rng, UnaryFunction fn)
        {
            return transformed_range<UnaryFunction, InputRange>(fn, rng);
        }

        template<class UnaryFunction, class InputRange>
        inline transformed_range<UnaryFunction, const InputRange>
        transform(const InputRange& rng, UnaryFunction fn)
        {
            return transformed_range<UnaryFunction, const InputRange>(fn, rng);
        }
    } // 'adaptors'

}

#endif
