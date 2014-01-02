// Boost.Range library
//
//  Copyright Thorsten Ottosen, Neil Groves 2006 - 2008. Use, modification and
//  distribution is subject to the Boost Software License, Version
//  1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.lslboost.org/LICENSE_1_0.txt)
//
// For more information, see http://www.lslboost.org/libs/range/
//

#ifndef BOOST_RANGE_ADAPTOR_REVERSED_HPP
#define BOOST_RANGE_ADAPTOR_REVERSED_HPP

#include <lslboost/range/iterator_range.hpp>
#include <lslboost/iterator/reverse_iterator.hpp>

namespace lslboost
{
    namespace range_detail
    {
        template< class R >
        struct reversed_range : 
            public lslboost::iterator_range< 
                      lslboost::reverse_iterator<
                        BOOST_DEDUCED_TYPENAME range_iterator<R>::type 
                                              >
                                         >
        {
        private:
            typedef lslboost::iterator_range< 
                      lslboost::reverse_iterator<
                        BOOST_DEDUCED_TYPENAME range_iterator<R>::type 
                                              >
                                         >
                base;
            
        public:
            typedef lslboost::reverse_iterator<BOOST_DEDUCED_TYPENAME range_iterator<R>::type> iterator;

            explicit reversed_range( R& r ) 
                : base( iterator(lslboost::end(r)), iterator(lslboost::begin(r)) )
            { }
        };

        struct reverse_forwarder {};
        
        template< class BidirectionalRng >
        inline reversed_range<BidirectionalRng> 
        operator|( BidirectionalRng& r, reverse_forwarder )
        {
            return reversed_range<BidirectionalRng>( r );   
        }
    
        template< class BidirectionalRng >
        inline reversed_range<const BidirectionalRng> 
        operator|( const BidirectionalRng& r, reverse_forwarder )
        {
            return reversed_range<const BidirectionalRng>( r );   
        }
        
    } // 'range_detail'
    
    using range_detail::reversed_range;

    namespace adaptors
    { 
        namespace
        {
            const range_detail::reverse_forwarder reversed = 
                                            range_detail::reverse_forwarder();
        }
        
        template<class BidirectionalRange>
        inline reversed_range<BidirectionalRange>
        reverse(BidirectionalRange& rng)
        {
            return reversed_range<BidirectionalRange>(rng);
        }
        
        template<class BidirectionalRange>
        inline reversed_range<const BidirectionalRange>
        reverse(const BidirectionalRange& rng)
        {
            return reversed_range<const BidirectionalRange>(rng);
        }
    } // 'adaptors'
    
} // 'lslboost'

#endif
