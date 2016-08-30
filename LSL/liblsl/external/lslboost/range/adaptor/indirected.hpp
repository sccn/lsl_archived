// Boost.Range library
//
//  Copyright Thorsten Ottosen, Neil Groves 2006 - 2008. Use, modification and
//  distribution is subject to the Boost Software License, Version
//  1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.lslboost.org/LICENSE_1_0.txt)
//
// For more information, see http://www.lslboost.org/libs/range/
//

#ifndef BOOST_RANGE_ADAPTOR_INDIRECTED_HPP
#define BOOST_RANGE_ADAPTOR_INDIRECTED_HPP

#include <lslboost/range/iterator_range.hpp>
#include <lslboost/iterator/indirect_iterator.hpp>

namespace lslboost
{
    namespace range_detail
    {
        template< class R >
        struct indirected_range :
            public lslboost::iterator_range<
                        lslboost::indirect_iterator<
                            BOOST_DEDUCED_TYPENAME range_iterator<R>::type
                        >
                    >
        {
        private:
            typedef lslboost::iterator_range<
                        lslboost::indirect_iterator<
                            BOOST_DEDUCED_TYPENAME range_iterator<R>::type
                        >
                    >
                base;

        public:
            explicit indirected_range( R& r )
                : base( r )
            { }
        };

        struct indirect_forwarder {};

        template< class InputRng >
        inline indirected_range<InputRng>
        operator|( InputRng& r, indirect_forwarder )
        {
            return indirected_range<InputRng>( r );
        }

        template< class InputRng >
        inline indirected_range<const InputRng>
        operator|( const InputRng& r, indirect_forwarder )
        {
            return indirected_range<const InputRng>( r );
        }

    } // 'range_detail'

    using range_detail::indirected_range;

    namespace adaptors
    {
        namespace
        {
            const range_detail::indirect_forwarder indirected =
                                            range_detail::indirect_forwarder();
        }

        template<class InputRange>
        inline indirected_range<InputRange>
        indirect(InputRange& rng)
        {
            return indirected_range<InputRange>(rng);
        }

        template<class InputRange>
        inline indirected_range<const InputRange>
        indirect(const InputRange& rng)
        {
            return indirected_range<const InputRange>(rng);
        }
    } // 'adaptors'

}

#endif
