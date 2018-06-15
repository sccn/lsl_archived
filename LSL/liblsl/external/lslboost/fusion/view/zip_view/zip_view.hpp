/*=============================================================================
    Copyright (c) 2001-2011 Joel de Guzman
    Copyright (c) 2006 Dan Marsden

    Distributed under the Boost Software License, Version 1.0. (See accompanying 
    file LICENSE_1_0.txt or copy at http://www.lslboost.org/LICENSE_1_0.txt)
==============================================================================*/
#if !defined(FUSION_ZIP_VIEW_23012006_0813)
#define FUSION_ZIP_VIEW_23012006_0813

#include <lslboost/fusion/support/sequence_base.hpp>
#include <lslboost/fusion/support/unused.hpp>
#include <lslboost/fusion/iterator/equal_to.hpp>
#include <lslboost/fusion/view/detail/strictest_traversal.hpp>
#include <lslboost/fusion/view/zip_view/detail/begin_impl.hpp>
#include <lslboost/fusion/view/zip_view/detail/end_impl.hpp>
#include <lslboost/fusion/view/zip_view/detail/size_impl.hpp>
#include <lslboost/fusion/view/zip_view/detail/at_impl.hpp>
#include <lslboost/fusion/view/zip_view/detail/value_at_impl.hpp>
#include <lslboost/fusion/container/vector/convert.hpp>
#include <lslboost/fusion/algorithm/query/find_if.hpp>
#include <lslboost/fusion/sequence/intrinsic/end.hpp>
#include <lslboost/fusion/sequence/intrinsic/size.hpp>
#include <lslboost/fusion/mpl.hpp>
#include <lslboost/fusion/algorithm/transformation/remove.hpp>

#include <lslboost/mpl/assert.hpp>
#include <lslboost/mpl/not.hpp>
#include <lslboost/mpl/placeholders.hpp>
#include <lslboost/mpl/transform_view.hpp>
#include <lslboost/mpl/at.hpp>
#include <lslboost/mpl/find_if.hpp>
#include <lslboost/mpl/equal_to.hpp>
#include <lslboost/mpl/bool.hpp>
#include <lslboost/mpl/eval_if.hpp>

#include <lslboost/type_traits/remove_reference.hpp>
#include <lslboost/type_traits/is_reference.hpp>

#include <lslboost/config.hpp>

namespace lslboost { namespace fusion {

    namespace detail
    {
        template<typename Sequences>
        struct all_references
            : fusion::result_of::equal_to<typename fusion::result_of::find_if<Sequences, mpl::not_<is_reference<mpl::_> > >::type, typename fusion::result_of::end<Sequences>::type>
        {};

        struct seq_ref_size
        {
            template<typename Params>
            struct result;

            template<typename Seq>
            struct result<seq_ref_size(Seq)>
            {
                static int const high_int = static_cast<int>(
                    (static_cast<unsigned>(~0) >> 1) - 1);

                typedef typename remove_reference<Seq>::type SeqClass;

                typedef typename mpl::eval_if<
                    traits::is_forward<SeqClass>,
                    result_of::size<SeqClass>,
                    mpl::int_<high_int> >::type type;
            };

            // never called, but needed for decltype-based result_of (C++0x)
#ifndef BOOST_NO_CXX11_RVALUE_REFERENCES
            template<typename Seq>
            typename result<seq_ref_size(Seq)>::type
            operator()(Seq&&) const;
#endif
        };

        struct poly_min
        {
            template<typename T>
            struct result;

            template<typename Lhs, typename Rhs>
            struct result<poly_min(Lhs, Rhs)>
            {
                typedef typename remove_reference<Lhs>::type lhs;
                typedef typename remove_reference<Rhs>::type rhs;
                typedef typename mpl::min<lhs, rhs>::type type;
            };

            // never called, but needed for decltype-based result_of (C++0x)
#ifndef BOOST_NO_CXX11_RVALUE_REFERENCES
            template<typename Lhs, typename Rhs>
            typename result<poly_min(Lhs, Rhs)>::type
            operator()(Lhs&&, Rhs&&) const;
#endif
        };

        template<typename Sequences>
        struct min_size
        {
            typedef typename result_of::transform<Sequences, detail::seq_ref_size>::type sizes;
            typedef typename result_of::fold<sizes, typename result_of::front<sizes>::type, detail::poly_min>::type type;
        };
    }

    struct zip_view_tag;
    struct fusion_sequence_tag;

    template<typename Sequences>
    struct zip_view : sequence_base< zip_view<Sequences> >
    {
        typedef typename result_of::remove<Sequences, unused_type const&>::type real_sequences;
        BOOST_MPL_ASSERT((detail::all_references<Sequences>));
        typedef typename detail::strictest_traversal<real_sequences>::type category;
        typedef zip_view_tag fusion_tag;
        typedef fusion_sequence_tag tag; // this gets picked up by MPL
        typedef mpl::true_ is_view;
        typedef typename fusion::result_of::as_vector<Sequences>::type sequences;
        typedef typename detail::min_size<real_sequences>::type size;

        zip_view(
            const Sequences& seqs)
            : sequences_(seqs)
        {};

        sequences sequences_;
    };
}}

#endif
