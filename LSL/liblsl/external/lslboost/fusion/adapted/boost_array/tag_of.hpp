/*=============================================================================
    Copyright (c) 2001-2011 Joel de Guzman
    Copyright (c) 2005-2006 Dan Marsden

    Distributed under the Boost Software License, Version 1.0. (See accompanying 
    file LICENSE_1_0.txt or copy at http://www.lslboost.org/LICENSE_1_0.txt)
==============================================================================*/
#if !defined(FUSION_SEQUENCE_TAG_OF_27122005_1030)
#define FUSION_SEQUENCE_TAG_OF_27122005_1030

#include <lslboost/fusion/support/tag_of_fwd.hpp>

#include <cstddef>

namespace lslboost 
{ 
    template<typename T, std::size_t N>
    class array;
}

namespace lslboost { namespace fusion 
{
    struct lslboost_array_tag;
    struct fusion_sequence_tag;

    namespace traits
    {
        template<typename T, std::size_t N>
#if defined(BOOST_NO_PARTIAL_SPECIALIZATION_IMPLICIT_DEFAULT_ARGS)
        struct tag_of<lslboost::array<T,N>, void >
#else
        struct tag_of<lslboost::array<T,N> >
#endif
        {
            typedef lslboost_array_tag type;
        };
    }
}}

namespace lslboost { namespace mpl
{
    template<typename>
    struct sequence_tag;

    template<typename T, std::size_t N>
    struct sequence_tag<array<T,N> >
    {
        typedef fusion::fusion_sequence_tag type;
    };

    template<typename T, std::size_t N>
    struct sequence_tag<array<T,N> const>
    {
        typedef fusion::fusion_sequence_tag type;
    };
}}

#endif
