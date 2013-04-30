/*=============================================================================
    Copyright (c) 2010-2011 Christopher Schmidt

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.lslboost.org/LICENSE_1_0.txt)
==============================================================================*/

#ifndef BOOST_FUSION_ADAPTED_DETAIL_STRUCT_DEFINE_STRUCT_HPP
#define BOOST_FUSION_ADAPTED_DETAIL_STRUCT_DEFINE_STRUCT_HPP

#include <lslboost/config.hpp>
#include <lslboost/fusion/sequence/intrinsic/begin.hpp>
#include <lslboost/fusion/iterator/deref.hpp>
#include <lslboost/fusion/iterator/next.hpp>
#include <lslboost/fusion/iterator/advance.hpp>
#include <lslboost/fusion/adapted/struct/detail/adapt_base.hpp>
#include <lslboost/fusion/adapted/struct/detail/namespace.hpp>
#include <lslboost/preprocessor/inc.hpp>
#include <lslboost/preprocessor/if.hpp>
#include <lslboost/preprocessor/expr_if.hpp>
#include <lslboost/preprocessor/dec.hpp>
#include <lslboost/preprocessor/logical/not.hpp>
#include <lslboost/preprocessor/punctuation/comma_if.hpp>
#include <lslboost/preprocessor/comparison/equal.hpp>
#include <lslboost/preprocessor/seq/seq.hpp>
#include <lslboost/preprocessor/seq/for_each.hpp>
#include <lslboost/preprocessor/seq/for_each_i.hpp>
#include <lslboost/preprocessor/seq/enum.hpp>
#include <lslboost/preprocessor/seq/size.hpp>
#include <lslboost/preprocessor/tuple/elem.hpp>
#include <lslboost/preprocessor/tuple/eat.hpp>
#include <lslboost/call_traits.hpp>
#include <lslboost/type_traits/is_convertible.hpp>
#include <lslboost/utility/enable_if.hpp>

#define BOOST_FUSION_DEFINE_STRUCT_COPY_CTOR_FILLER_I(                          \
    R, ATTRIBUTE_TUPEL_SIZE, I, ATTRIBUTE)                                      \
                                                                                \
    BOOST_PP_COMMA_IF(I)                                                        \
    BOOST_PP_TUPLE_ELEM(ATTRIBUTE_TUPEL_SIZE,1,ATTRIBUTE)(                      \
        other_self.BOOST_PP_TUPLE_ELEM(ATTRIBUTE_TUPEL_SIZE,1,ATTRIBUTE))

#define BOOST_FUSION_DEFINE_STRUCT_ASSIGN_FILLER_I(                             \
    R, ATTRIBUTE_TUPEL_SIZE, I_, ATTRIBUTE)                                     \
                                                                                \
    BOOST_PP_EXPR_IF(                                                           \
        I_,                                                                     \
        typedef typename                                                        \
            lslboost::fusion::result_of::next<                                     \
                BOOST_PP_CAT(I,BOOST_PP_DEC(I_))                                \
            >::type                                                             \
        BOOST_PP_CAT(I,I_);                                                     \
        BOOST_PP_CAT(I,I_) BOOST_PP_CAT(i,I_)=                                  \
                lslboost::fusion::next(BOOST_PP_CAT(i,BOOST_PP_DEC(I_)));          \
    )                                                                           \
                                                                                \
    BOOST_PP_TUPLE_ELEM(ATTRIBUTE_TUPEL_SIZE,1,ATTRIBUTE)=                      \
        lslboost::fusion::deref(BOOST_PP_CAT(i,I_));

#define BOOST_FUSION_DEFINE_STRUCT_ASSIGN_OP(                                   \
    ATTRIBUTES_SEQ, ATTRIBUTE_TUPEL_SIZE)                                       \
                                                                                \
    template<typename Seq>                                                      \
    self_type&                                                                  \
    operator=(Seq const& seq)                                                   \
    {                                                                           \
        typedef typename                                                        \
            lslboost::fusion::result_of::begin<Seq const>::type                    \
        I0;                                                                     \
        I0 i0=lslboost::fusion::begin(seq);                                        \
                                                                                \
        BOOST_PP_SEQ_FOR_EACH_I_R(                                              \
            1,                                                                  \
            BOOST_FUSION_DEFINE_STRUCT_ASSIGN_FILLER_I,                         \
            ATTRIBUTE_TUPEL_SIZE,                                               \
            ATTRIBUTES_SEQ)                                                     \
                                                                                \
        return *this;                                                           \
    }

#define BOOST_FUSION_DEFINE_STRUCT_ATTR_I(R, ATTRIBUTE_TUPEL_SIZE, ATTRIBUTE)   \
                                                                                \
    BOOST_PP_TUPLE_ELEM(ATTRIBUTE_TUPEL_SIZE,0,ATTRIBUTE)                       \
        BOOST_PP_TUPLE_ELEM(ATTRIBUTE_TUPEL_SIZE,1,ATTRIBUTE);

#define BOOST_FUSION_DEFINE_STRUCT_SEQ_CTOR_FILLER_I(                           \
    R, ATTRIBUTE_TUPEL_SIZE, I, ATTRIBUTE)                                      \
                                                                                \
    BOOST_PP_COMMA_IF(I)                                                        \
    BOOST_PP_TUPLE_ELEM(ATTRIBUTE_TUPEL_SIZE,1,ATTRIBUTE)(                      \
        lslboost::fusion::deref(lslboost::fusion::advance_c<I>(lslboost::fusion::begin(  \
            seq))))

#define BOOST_FUSION_DEFINE_STRUCT_SEQ_CTOR_DISABLER(                           \
    ATTRIBUTES_SEQ, ATTRIBUTE_TUPEL_SIZE)                                       \
                                                                                \
    , typename lslboost::disable_if<                                               \
        lslboost::is_convertible<                                                  \
            Seq const&                                                          \
          , BOOST_PP_TUPLE_ELEM(                                                \
                ATTRIBUTE_TUPEL_SIZE,                                           \
                0,                                                              \
                BOOST_PP_SEQ_HEAD(ATTRIBUTES_SEQ))                              \
        >                                                                       \
    >::type* =0

#define BOOST_FUSION_DEFINE_STRUCT_SEQ_DEFAULT_CTOR_FILLER_I(                   \
    R, ATTRIBUTE_TUPEL_SIZE, I, ATTRIBUTE)                                      \
                                                                                \
    BOOST_PP_COMMA_IF(I)                                                        \
    BOOST_PP_TUPLE_ELEM(ATTRIBUTE_TUPEL_SIZE,1,ATTRIBUTE)()

#define BOOST_FUSION_DEFINE_STRUCT_IMPL_IMPL(                                   \
    NAME, ATTRIBUTES_SEQ, ATTRIBUTE_TUPEL_SIZE)                                 \
                                                                                \
    BOOST_PP_SEQ_FOR_EACH_R(                                                    \
        1,                                                                      \
        BOOST_FUSION_DEFINE_STRUCT_ATTR_I,                                      \
        ATTRIBUTE_TUPEL_SIZE,                                                   \
        ATTRIBUTES_SEQ)                                                         \
                                                                                \
    NAME()                                                                      \
      : BOOST_PP_SEQ_FOR_EACH_I_R(                                              \
            1,                                                                  \
            BOOST_FUSION_DEFINE_STRUCT_SEQ_DEFAULT_CTOR_FILLER_I,               \
            ATTRIBUTE_TUPEL_SIZE,                                               \
            ATTRIBUTES_SEQ)                                                     \
    {}                                                                          \
                                                                                \
    NAME(self_type const& other_self)                                           \
      : BOOST_PP_SEQ_FOR_EACH_I_R(                                              \
            1,                                                                  \
            BOOST_FUSION_DEFINE_STRUCT_COPY_CTOR_FILLER_I,                      \
            ATTRIBUTE_TUPEL_SIZE,                                               \
            ATTRIBUTES_SEQ)                                                     \
    {}                                                                          \
                                                                                \
    template<typename Seq>                                                      \
    NAME(Seq const& seq                                                         \
        BOOST_PP_IF(                                                            \
            BOOST_PP_DEC(BOOST_PP_SEQ_SIZE(ATTRIBUTES_SEQ)),                    \
            BOOST_PP_TUPLE_EAT(2),                                              \
            BOOST_FUSION_DEFINE_STRUCT_SEQ_CTOR_DISABLER)(                      \
                ATTRIBUTES_SEQ, ATTRIBUTE_TUPEL_SIZE)                           \
        )                                                                       \
      : BOOST_PP_SEQ_FOR_EACH_I_R(                                              \
            1,                                                                  \
            BOOST_FUSION_DEFINE_STRUCT_SEQ_CTOR_FILLER_I,                       \
            ATTRIBUTE_TUPEL_SIZE,                                               \
            ATTRIBUTES_SEQ)                                                     \
    {}                                                                          \
                                                                                \
    BOOST_FUSION_DEFINE_STRUCT_ASSIGN_OP(ATTRIBUTES_SEQ, ATTRIBUTE_TUPEL_SIZE)

#define BOOST_FUSION_DEFINE_STRUCT_CTOR_1(                                      \
        NAME, ATTRIBUTES_SEQ, ATTRIBUTE_TUPEL_SIZE)                             \
                                                                                \
        explicit                                                                \
        NAME(lslboost::call_traits<                                                \
                BOOST_PP_TUPLE_ELEM(                                            \
                    ATTRIBUTE_TUPEL_SIZE,0,BOOST_PP_SEQ_HEAD(ATTRIBUTES_SEQ))   \
            >::param_type arg)                                                  \
          : BOOST_PP_TUPLE_ELEM(                                                \
                ATTRIBUTE_TUPEL_SIZE,1,BOOST_PP_SEQ_HEAD(ATTRIBUTES_SEQ))(arg)  \
        {}

#define BOOST_FUSION_DEFINE_TPL_STRUCT_CTOR_1(                                  \
        TEMPLATE_PARAMS_SEQ, NAME, ATTRIBUTES_SEQ, ATTRIBUTE_TUPEL_SIZE)        \
                                                                                \
        explicit                                                                \
        NAME(typename lslboost::call_traits<                                       \
                typename lslboost::fusion::detail::get_first_arg<                  \
                    BOOST_PP_TUPLE_ELEM(                                        \
                        ATTRIBUTE_TUPEL_SIZE,                                   \
                        0,                                                      \
                        BOOST_PP_SEQ_HEAD(ATTRIBUTES_SEQ))                      \
                  , BOOST_PP_SEQ_HEAD(TEMPLATE_PARAMS_SEQ)                      \
                >::type                                                         \
            >::param_type arg)                                                  \
          : BOOST_PP_TUPLE_ELEM(                                                \
                ATTRIBUTE_TUPEL_SIZE,1,BOOST_PP_SEQ_HEAD(ATTRIBUTES_SEQ))(arg)  \
        {}

#define BOOST_FUSION_DEFINE_STRUCT_CTOR_FILLER_I(                               \
    R, ATTRIBUTE_TUPEL_SIZE, I, ATTRIBUTE)                                      \
                                                                                \
    BOOST_PP_COMMA_IF(I)                                                        \
    BOOST_PP_TUPLE_ELEM(ATTRIBUTE_TUPEL_SIZE,1,ATTRIBUTE)(BOOST_PP_CAT(_,I))

#define BOOST_FUSION_DEFINE_TPL_STRUCT_CTOR_ARG_I(R, DATA, I, ATTRIBUTE)        \
                                                                                \
        BOOST_PP_COMMA_IF(I)                                                    \
        typename lslboost::call_traits<                                            \
            typename lslboost::fusion::detail::get_first_arg<                      \
                BOOST_PP_TUPLE_ELEM(                                            \
                    BOOST_PP_TUPLE_ELEM(3,0,DATA),                              \
                    0,                                                          \
                    ATTRIBUTE)                                                  \
              , BOOST_PP_TUPLE_ELEM(3,2,DATA)                                   \
            >::type                                                             \
        >::param_type BOOST_PP_CAT(_,I)

#define BOOST_FUSION_DEFINE_TPL_STRUCT_CTOR_N(                                  \
    TEMPLATE_PARAMS_SEQ, NAME, ATTRIBUTES_SEQ, ATTRIBUTE_TUPEL_SIZE)            \
                                                                                \
        NAME(BOOST_PP_SEQ_FOR_EACH_I_R(                                         \
                1,                                                              \
                BOOST_FUSION_DEFINE_TPL_STRUCT_CTOR_ARG_I,                      \
                (                                                               \
                    ATTRIBUTE_TUPEL_SIZE,                                       \
                    BOOST_PP_SEQ_SIZE(ATTRIBUTES_SEQ),                          \
                    BOOST_PP_SEQ_HEAD(TEMPLATE_PARAMS_SEQ)                      \
                ),                                                              \
                ATTRIBUTES_SEQ))                                                \
          : BOOST_PP_SEQ_FOR_EACH_I_R(                                          \
                1,                                                              \
                BOOST_FUSION_DEFINE_STRUCT_CTOR_FILLER_I,                       \
                ATTRIBUTE_TUPEL_SIZE,                                           \
                ATTRIBUTES_SEQ)                                                 \
        {}

#define BOOST_FUSION_DEFINE_STRUCT_CTOR_ARG_I(                                  \
        R, ATTRIBUTE_TUPEL_SIZE, I, ATTRIBUTE)                                  \
                                                                                \
        BOOST_PP_COMMA_IF(I)                                                    \
        lslboost::call_traits<                                                     \
            BOOST_PP_TUPLE_ELEM(ATTRIBUTE_TUPEL_SIZE,0,ATTRIBUTE)               \
        >::param_type BOOST_PP_CAT(_,I)

#define BOOST_FUSION_DEFINE_STRUCT_CTOR_N(                                      \
    NAME, ATTRIBUTES_SEQ, ATTRIBUTE_TUPEL_SIZE)                                 \
                                                                                \
        NAME(BOOST_PP_SEQ_FOR_EACH_I_R(                                         \
                1,                                                              \
                BOOST_FUSION_DEFINE_STRUCT_CTOR_ARG_I,                          \
                ATTRIBUTE_TUPEL_SIZE,                                           \
                ATTRIBUTES_SEQ))                                                \
          : BOOST_PP_SEQ_FOR_EACH_I_R(                                          \
                1,                                                              \
                BOOST_FUSION_DEFINE_STRUCT_CTOR_FILLER_I,                       \
                ATTRIBUTE_TUPEL_SIZE,                                           \
                ATTRIBUTES_SEQ)                                                 \
        {}

#define BOOST_FUSION_DEFINE_STRUCT_CTOR(                                        \
    NAME, ATTRIBUTES_SEQ, ATTRIBUTE_TUPEL_SIZE)                                 \
                                                                                \
    BOOST_PP_IF(BOOST_PP_DEC(BOOST_PP_SEQ_SIZE(ATTRIBUTES_SEQ)),                \
        BOOST_FUSION_DEFINE_STRUCT_CTOR_N,                                      \
        BOOST_FUSION_DEFINE_STRUCT_CTOR_1)(                                     \
            NAME, ATTRIBUTES_SEQ, ATTRIBUTE_TUPEL_SIZE)

#define BOOST_FUSION_DEFINE_TPL_STRUCT_CTOR(                                    \
    TEMPLATE_PARAMS_SEQ, NAME, ATTRIBUTES_SEQ, ATTRIBUTE_TUPEL_SIZE)            \
                                                                                \
    BOOST_PP_IF(BOOST_PP_DEC(BOOST_PP_SEQ_SIZE(ATTRIBUTES_SEQ)),                \
        BOOST_FUSION_DEFINE_TPL_STRUCT_CTOR_N,                                  \
        BOOST_FUSION_DEFINE_TPL_STRUCT_CTOR_1)(                                 \
            TEMPLATE_PARAMS_SEQ, NAME, ATTRIBUTES_SEQ, ATTRIBUTE_TUPEL_SIZE)

#define BOOST_FUSION_DEFINE_NONEMPTY_STRUCT_IMPL(                               \
    NAME, ATTRIBUTES_SEQ, ATTRIBUTE_TUPEL_SIZE)                                 \
                                                                                \
        BOOST_FUSION_DEFINE_STRUCT_IMPL_IMPL(                                   \
            NAME, BOOST_PP_SEQ_TAIL(ATTRIBUTES_SEQ), ATTRIBUTE_TUPEL_SIZE)      \
                                                                                \
        BOOST_FUSION_DEFINE_STRUCT_CTOR(                                        \
            NAME, BOOST_PP_SEQ_TAIL(ATTRIBUTES_SEQ), ATTRIBUTE_TUPEL_SIZE)

#define BOOST_FUSION_DEFINE_EMPTY_STRUCT_IMPL(                                  \
    NAME, ATTRIBUTES_SEQ, ATTRIBUTE_TUPEL_SIZE)                                 \
                                                                                \
        template<typename Seq>                                                  \
        NAME(Seq const&)                                                        \
        {}                                                                      \
                                                                                \
        template<typename Seq>                                                  \
        self_type&                                                              \
        operator=(Seq const& seq)                                               \
        {                                                                       \
            return *this;                                                       \
        }

#define BOOST_FUSION_DEFINE_STRUCT_IMPL(                                        \
    NAMESPACE_SEQ, NAME, ATTRIBUTES_SEQ, ATTRIBUTE_TUPEL_SIZE)                  \
                                                                                \
    BOOST_FUSION_ADAPT_STRUCT_NAMESPACE_DEFINITION_BEGIN(NAMESPACE_SEQ)         \
                                                                                \
    struct NAME                                                                 \
    {                                                                           \
        typedef NAME self_type;                                                 \
                                                                                \
        BOOST_PP_IF(BOOST_PP_DEC(BOOST_PP_SEQ_SIZE(ATTRIBUTES_SEQ)),            \
            BOOST_FUSION_DEFINE_NONEMPTY_STRUCT_IMPL,                           \
            BOOST_FUSION_DEFINE_EMPTY_STRUCT_IMPL)(                             \
                NAME, ATTRIBUTES_SEQ, ATTRIBUTE_TUPEL_SIZE)                     \
    };                                                                          \
                                                                                \
    BOOST_FUSION_ADAPT_STRUCT_NAMESPACE_DEFINITION_END(NAMESPACE_SEQ)

#define BOOST_FUSION_DEFINE_NONEMPTY_TPL_STRUCT_IMPL(                           \
    TEMPLATE_PARAMS_SEQ, NAME, ATTRIBUTES_SEQ, ATTRIBUTE_TUPEL_SIZE)            \
                                                                                \
        BOOST_FUSION_DEFINE_STRUCT_IMPL_IMPL(                                   \
            NAME, BOOST_PP_SEQ_TAIL(ATTRIBUTES_SEQ), ATTRIBUTE_TUPEL_SIZE)      \
                                                                                \
        BOOST_FUSION_DEFINE_TPL_STRUCT_CTOR(                                    \
            TEMPLATE_PARAMS_SEQ,                                                \
            NAME,                                                               \
            BOOST_PP_SEQ_TAIL(ATTRIBUTES_SEQ),                                  \
            ATTRIBUTE_TUPEL_SIZE)

#define BOOST_FUSION_DEFINE_EMPTY_TPL_STRUCT_IMPL(                              \
    TEMPLATE_PARAMS_SEQ, NAME, ATTRIBUTES_SEQ, ATTRIBUTE_TUPEL_SIZE)            \
                                                                                \
        BOOST_FUSION_DEFINE_EMPTY_STRUCT_IMPL(                                  \
            NAME, ATTRIBUTES_SEQ, ATTRIBUTE_TUPEL_SIZE)

#define BOOST_FUSION_DEFINE_TPL_STRUCT_IMPL(                                    \
    TEMPLATE_PARAMS_SEQ,                                                        \
    NAMESPACE_SEQ,                                                              \
    NAME,                                                                       \
    ATTRIBUTES_SEQ,                                                             \
    ATTRIBUTE_TUPEL_SIZE)                                                       \
                                                                                \
    BOOST_FUSION_ADAPT_STRUCT_NAMESPACE_DEFINITION_BEGIN(NAMESPACE_SEQ)         \
                                                                                \
    template<                                                                   \
        BOOST_FUSION_ADAPT_STRUCT_UNPACK_TEMPLATE_PARAMS_IMPL(                  \
            (0)TEMPLATE_PARAMS_SEQ)                                             \
    >                                                                           \
    struct NAME                                                                 \
    {                                                                           \
        typedef NAME self_type;                                                 \
                                                                                \
        BOOST_PP_IF(BOOST_PP_DEC(BOOST_PP_SEQ_SIZE(ATTRIBUTES_SEQ)),            \
            BOOST_FUSION_DEFINE_NONEMPTY_TPL_STRUCT_IMPL,                       \
            BOOST_FUSION_DEFINE_EMPTY_TPL_STRUCT_IMPL)(                         \
                TEMPLATE_PARAMS_SEQ, NAME, ATTRIBUTES_SEQ, ATTRIBUTE_TUPEL_SIZE)\
    };                                                                          \
                                                                                \
    BOOST_FUSION_ADAPT_STRUCT_NAMESPACE_DEFINITION_END(NAMESPACE_SEQ)

namespace lslboost { namespace fusion { namespace detail
{
    template<typename A1, typename A2>
    struct get_first_arg
    {
        typedef A1 type;
    };
}}}

#endif
