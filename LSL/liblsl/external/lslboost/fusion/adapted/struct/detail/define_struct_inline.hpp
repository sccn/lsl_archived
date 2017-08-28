/*=============================================================================
    Copyright (c) 2012 Nathan Ridge

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.lslboost.org/LICENSE_1_0.txt)
==============================================================================*/

#ifndef BOOST_FUSION_ADAPTED_STRUCT_DETAIL_DEFINE_STRUCT_INLINE_HPP
#define BOOST_FUSION_ADAPTED_STRUCT_DETAIL_DEFINE_STRUCT_INLINE_HPP

#include <lslboost/config.hpp>
#include <lslboost/fusion/support/category_of.hpp>
#include <lslboost/fusion/sequence/sequence_facade.hpp>
#include <lslboost/fusion/iterator/iterator_facade.hpp>
#include <lslboost/fusion/algorithm/auxiliary/copy.hpp>
#include <lslboost/fusion/adapted/struct/detail/define_struct.hpp>
#include <lslboost/mpl/int.hpp>
#include <lslboost/mpl/bool.hpp>
#include <lslboost/mpl/identity.hpp>
#include <lslboost/mpl/minus.hpp>
#include <lslboost/mpl/if.hpp>
#include <lslboost/type_traits/is_const.hpp>
#include <lslboost/preprocessor/comma_if.hpp>
#include <lslboost/preprocessor/facilities/is_empty.hpp>
#include <lslboost/preprocessor/repeat.hpp>
#include <lslboost/preprocessor/seq/for_each_i.hpp>
#include <lslboost/preprocessor/seq/size.hpp>
#include <lslboost/preprocessor/seq/enum.hpp>
#include <lslboost/preprocessor/seq/seq.hpp>
#include <lslboost/preprocessor/tuple/elem.hpp>

// MSVC and GCC <= 4.4 have a bug that affects partial specializations of
// nested templates under some circumstances. This affects the implementation
// of BOOST_FUSION_DEFINE_STRUCT_INLINE, which uses such specializations for
// the iterator class's 'deref' and 'value_of' metafunctions. On these compilers
// an alternate implementation for these metafunctions is used that does not 
// require such specializations. The alternate implementation takes longer
// to compile so its use is restricted to the offending compilers.
// For MSVC, the bug was was reported at https://connect.microsoft.com/VisualStudio/feedback/details/757891/c-compiler-error-involving-partial-specializations-of-nested-templates
// For GCC, 4.4 and earlier are no longer maintained so there is no need
// to report a bug.
#if defined(BOOST_MSVC) || (defined(__GNUC__) && (__GNUC__ < 4 || (__GNUC__ == 4 && __GNUC_MINOR__ <= 4)))
    #define BOOST_FUSION_NEED_NESTED_TEMPLATE_PARTIAL_SPEC_WKND 
#endif

#ifdef BOOST_FUSION_NEED_NESTED_TEMPLATE_PARTIAL_SPEC_WKND
#include <lslboost/type_traits/add_const.hpp>
#include <lslboost/type_traits/remove_const.hpp>
#include <lslboost/mpl/if.hpp>
#include <lslboost/fusion/sequence/intrinsic/at_c.hpp>
#include <lslboost/fusion/container/vector.hpp>
#endif


#define BOOST_FUSION_MAKE_DEFAULT_INIT_LIST_ENTRY(R, DATA, N, ATTRIBUTE)        \
    BOOST_PP_COMMA_IF(N) BOOST_PP_TUPLE_ELEM(2, 1, ATTRIBUTE)()

#define BOOST_FUSION_MAKE_DEFAULT_INIT_LIST(ATTRIBUTES_SEQ)                     \
            : BOOST_PP_SEQ_FOR_EACH_I(                                          \
              BOOST_FUSION_MAKE_DEFAULT_INIT_LIST_ENTRY,                        \
              ~,                                                                \
              ATTRIBUTES_SEQ)                                                   \

#define BOOST_FUSION_IGNORE_1(ARG1)
#define BOOST_FUSION_IGNORE_2(ARG1, ARG2)

#define BOOST_FUSION_MAKE_COPY_CONSTRUCTOR(NAME, ATTRIBUTES_SEQ)                \
    NAME(BOOST_PP_SEQ_FOR_EACH_I(                                               \
            BOOST_FUSION_MAKE_CONST_REF_PARAM,                                  \
            ~,                                                                  \
            ATTRIBUTES_SEQ))                                                    \
        : BOOST_PP_SEQ_FOR_EACH_I(                                              \
              BOOST_FUSION_MAKE_INIT_LIST_ENTRY,                                \
              ~,                                                                \
              ATTRIBUTES_SEQ)                                                   \
    {                                                                           \
    }                                                                           \

#define BOOST_FUSION_MAKE_CONST_REF_PARAM(R, DATA, N, ATTRIBUTE)                \
    BOOST_PP_COMMA_IF(N)                                                        \
    BOOST_PP_TUPLE_ELEM(2, 0, ATTRIBUTE) const&                                 \
    BOOST_PP_TUPLE_ELEM(2, 1, ATTRIBUTE)

#define BOOST_FUSION_MAKE_INIT_LIST_ENTRY_I(NAME) NAME(NAME)

#define BOOST_FUSION_MAKE_INIT_LIST_ENTRY(R, DATA, N, ATTRIBUTE)                \
    BOOST_PP_COMMA_IF(N)                                                        \
    BOOST_FUSION_MAKE_INIT_LIST_ENTRY_I(BOOST_PP_TUPLE_ELEM(2, 1, ATTRIBUTE))

#define BOOST_FUSION_ITERATOR_NAME(NAME)                                        \
    BOOST_PP_CAT(lslboost_fusion_detail_, BOOST_PP_CAT(NAME, _iterator))

// Note: all template parameter names need to be uglified, otherwise they might
//       shadow a template parameter of the struct when used with
//       BOOST_FUSION_DEFINE_TPL_STRUCT_INLINE

#define BOOST_FUSION_MAKE_ITERATOR_VALUE_OF_SPECS(Z, N, NAME)                   \
    template <typename lslboost_fusion_detail_Sq>                                  \
    struct value_of<                                                            \
               BOOST_FUSION_ITERATOR_NAME(NAME)<lslboost_fusion_detail_Sq, N>      \
           >                                                                    \
        : lslboost::mpl::identity<                                                 \
              typename lslboost_fusion_detail_Sq::t##N##_type                      \
          >                                                                     \
    {                                                                           \
    };

#define BOOST_FUSION_MAKE_ITERATOR_DEREF_SPEC(                                  \
    SPEC_TYPE, CALL_ARG_TYPE, TYPE_QUAL, ATTRIBUTE, N)                          \
                                                                                \
    template <typename lslboost_fusion_detail_Sq>                                  \
    struct deref<SPEC_TYPE, N> >                                                \
    {                                                                           \
        typedef typename lslboost_fusion_detail_Sq::t##N##_type TYPE_QUAL& type;   \
        static type call(CALL_ARG_TYPE, N> const& iter)                         \
        {                                                                       \
            return iter.seq_.BOOST_PP_TUPLE_ELEM(2, 1, ATTRIBUTE);              \
        }                                                                       \
    };

#define BOOST_FUSION_MAKE_ITERATOR_DEREF_SPECS(R, NAME, N, ATTRIBUTE)           \
    BOOST_FUSION_MAKE_ITERATOR_DEREF_SPEC(                                      \
        BOOST_FUSION_ITERATOR_NAME(NAME)<lslboost_fusion_detail_Sq,                \
        BOOST_FUSION_ITERATOR_NAME(NAME)<lslboost_fusion_detail_Sq,                \
        ,                                                                       \
        ATTRIBUTE,                                                              \
        N)                                                                      \
    BOOST_FUSION_MAKE_ITERATOR_DEREF_SPEC(                                      \
        BOOST_FUSION_ITERATOR_NAME(NAME)<const lslboost_fusion_detail_Sq,          \
        BOOST_FUSION_ITERATOR_NAME(NAME)<const lslboost_fusion_detail_Sq,          \
        const,                                                                  \
        ATTRIBUTE,                                                              \
        N)                                                                      \
    BOOST_FUSION_MAKE_ITERATOR_DEREF_SPEC(                                      \
        const BOOST_FUSION_ITERATOR_NAME(NAME)<lslboost_fusion_detail_Sq,          \
        BOOST_FUSION_ITERATOR_NAME(NAME)<lslboost_fusion_detail_Sq,                \
        ,                                                                       \
        ATTRIBUTE,                                                              \
        N)                                                                      \
    BOOST_FUSION_MAKE_ITERATOR_DEREF_SPEC(                                      \
        const BOOST_FUSION_ITERATOR_NAME(NAME)<const lslboost_fusion_detail_Sq,    \
        BOOST_FUSION_ITERATOR_NAME(NAME)<const lslboost_fusion_detail_Sq,          \
        const,                                                                  \
        ATTRIBUTE,                                                              \
        N)                                                                      \

#define BOOST_FUSION_MAKE_VALUE_AT_SPECS(Z, N, DATA)                            \
    template <typename lslboost_fusion_detail_Sq>                                  \
    struct value_at<lslboost_fusion_detail_Sq, lslboost::mpl::int_<N> >               \
    {                                                                           \
        typedef typename lslboost_fusion_detail_Sq::t##N##_type type;              \
    };

#define BOOST_FUSION_MAKE_AT_SPECS(R, DATA, N, ATTRIBUTE)                       \
    template <typename lslboost_fusion_detail_Sq>                                  \
    struct at<lslboost_fusion_detail_Sq, lslboost::mpl::int_<N> >                     \
    {                                                                           \
        typedef typename lslboost::mpl::if_<                                       \
            lslboost::is_const<lslboost_fusion_detail_Sq>,                            \
            typename lslboost_fusion_detail_Sq::t##N##_type const&,                \
            typename lslboost_fusion_detail_Sq::t##N##_type&                       \
        >::type type;                                                           \
                                                                                \
        static type call(lslboost_fusion_detail_Sq& sq)                            \
        {                                                                       \
            return sq. BOOST_PP_TUPLE_ELEM(2, 1, ATTRIBUTE);                    \
        }                                                                       \
    };

#define BOOST_FUSION_MAKE_TYPEDEF(R, DATA, N, ATTRIBUTE)                        \
    typedef BOOST_PP_TUPLE_ELEM(2, 0, ATTRIBUTE) t##N##_type;

#define BOOST_FUSION_MAKE_DATA_MEMBER(R, DATA, N, ATTRIBUTE)                    \
    BOOST_PP_TUPLE_ELEM(2, 0, ATTRIBUTE) BOOST_PP_TUPLE_ELEM(2, 1, ATTRIBUTE);

#ifdef BOOST_FUSION_NEED_NESTED_TEMPLATE_PARTIAL_SPEC_WKND

#define BOOST_FUSION_DEFINE_ITERATOR_VALUE_OF(NAME, ATTRIBUTE_SEQ_SIZE)         \
        template <typename lslboost_fusion_detail_Iterator>                        \
        struct value_of : lslboost::fusion::result_of::at_c<                       \
                              ref_vec_t,                                        \
                              lslboost_fusion_detail_Iterator::index::value        \
                          >                                                     \
        {                                                                       \
        };

#define BOOST_FUSION_DEFINE_ITERATOR_DEREF(NAME, ATTRIBUTES_SEQ)                \
        template <typename lslboost_fusion_detail_Iterator>                        \
        struct deref                                                            \
        {                                                                       \
            typedef typename lslboost::remove_const<                               \
                lslboost_fusion_detail_Iterator                                    \
            >::type iterator_raw_type;                                          \
                                                                                \
            static const int index = iterator_raw_type::index::value;           \
                                                                                \
            typedef typename lslboost::fusion::result_of::at_c<                    \
                ref_vec_t,                                                      \
                index                                                           \
            >::type result_raw_type;                                            \
                                                                                \
            typedef typename lslboost::mpl::if_<                                   \
                lslboost::is_const<typename iterator_raw_type::sequence_type>,     \
                typename lslboost::add_const<result_raw_type>::type,               \
                result_raw_type                                                 \
            >::type type;                                                       \
                                                                                \
            static type call(iterator_raw_type const& iter)                     \
            {                                                                   \
                return lslboost::fusion::at_c<index>(iter.ref_vec);                \
            }                                                                   \
        };

#define BOOST_FUSION_MAKE_ITERATOR_WKND_FIELD_NAME(R, DATA, N, ATTRIBUTE)       \
        BOOST_PP_COMMA_IF(N) seq.BOOST_PP_TUPLE_ELEM(2, 1, ATTRIBUTE)

#define BOOST_FUSION_DEFINE_ITERATOR_WKND_INIT_LIST_ENTRIES(ATTRIBUTES_SEQ)     \
        , ref_vec(BOOST_PP_SEQ_FOR_EACH_I(                                      \
                          BOOST_FUSION_MAKE_ITERATOR_WKND_FIELD_NAME,           \
                          ~,                                                    \
                          BOOST_PP_SEQ_TAIL(ATTRIBUTES_SEQ)))

#define BOOST_FUSION_MAKE_ITERATOR_WKND_REF(Z, N, DATA)                         \
        BOOST_PP_COMMA_IF(N)                                                    \
        typename lslboost::mpl::if_<                                               \
                lslboost::is_const<lslboost_fusion_detail_Seq>,                       \
                typename lslboost::add_const<                                      \
                        typename lslboost_fusion_detail_Seq::t##N##_type           \
                >::type,                                                        \
                typename lslboost_fusion_detail_Seq::t##N##_type                   \
        >::type&

#define BOOST_FUSION_DEFINE_ITERATOR_WKND_MEMBERS(ATTRIBUTES_SEQ_SIZE)          \
        typedef lslboost::fusion::vector<                                          \
            BOOST_PP_REPEAT(                                                    \
                    ATTRIBUTES_SEQ_SIZE,                                        \
                    BOOST_FUSION_MAKE_ITERATOR_WKND_REF,                        \
                    ~)                                                          \
        > ref_vec_t;                                                            \
                                                                                \
        ref_vec_t ref_vec;

#else

#define BOOST_FUSION_DEFINE_ITERATOR_VALUE_OF(NAME, ATTRIBUTES_SEQ_SIZE)        \
        template <typename lslboost_fusion_detail_T> struct value_of;              \
        BOOST_PP_REPEAT(                                                        \
            ATTRIBUTES_SEQ_SIZE,                                                \
            BOOST_FUSION_MAKE_ITERATOR_VALUE_OF_SPECS,                          \
            NAME)

#define BOOST_FUSION_DEFINE_ITERATOR_DEREF(NAME, ATTRIBUTES_SEQ)                \
        template <typename lslboost_fusion_detail_T> struct deref;                 \
        BOOST_PP_SEQ_FOR_EACH_I(                                                \
            BOOST_FUSION_MAKE_ITERATOR_DEREF_SPECS,                             \
            NAME,                                                               \
            ATTRIBUTES_SEQ)

#define BOOST_FUSION_DEFINE_ITERATOR_WKND_INIT_LIST_ENTRIES(ATTRIBUTES_SEQ)

#define BOOST_FUSION_DEFINE_ITERATOR_WKND_MEMBERS(ATTRIBUTES_SEQ_SIZE)

#endif  // BOOST_FUSION_NEED_NESTED_TEMPLATE_PARTIAL_SPEC_WKND

// Note: We can't nest the iterator inside the struct because we run into
//       a MSVC10 bug involving partial specializations of nested templates.

#define BOOST_FUSION_DEFINE_STRUCT_INLINE_IMPL(NAME, ATTRIBUTES)                \
    BOOST_FUSION_DEFINE_STRUCT_INLINE_ITERATOR(NAME, ATTRIBUTES)                \
    struct NAME : lslboost::fusion::sequence_facade<                               \
                      NAME,                                                     \
                      lslboost::fusion::random_access_traversal_tag                \
                  >                                                             \
    {                                                                           \
        BOOST_FUSION_DEFINE_STRUCT_INLINE_MEMBERS(NAME, ATTRIBUTES)             \
    };

#define BOOST_FUSION_DEFINE_TPL_STRUCT_INLINE_IMPL(                             \
    TEMPLATE_PARAMS_SEQ, NAME, ATTRIBUTES)                                      \
                                                                                \
    BOOST_FUSION_DEFINE_STRUCT_INLINE_ITERATOR(NAME, ATTRIBUTES)                \
                                                                                \
    template <                                                                  \
        BOOST_FUSION_ADAPT_STRUCT_UNPACK_TEMPLATE_PARAMS_IMPL(                  \
            (0)TEMPLATE_PARAMS_SEQ)                                             \
    >                                                                           \
    struct NAME : lslboost::fusion::sequence_facade<                               \
                      NAME<                                                     \
                          BOOST_PP_SEQ_ENUM(TEMPLATE_PARAMS_SEQ)                \
                      >,                                                        \
                      lslboost::fusion::random_access_traversal_tag                \
                  >                                                             \
    {                                                                           \
        BOOST_FUSION_DEFINE_STRUCT_INLINE_MEMBERS(NAME, ATTRIBUTES)             \
    };

#define BOOST_FUSION_DEFINE_STRUCT_INLINE_MEMBERS(NAME, ATTRIBUTES)             \
    BOOST_FUSION_DEFINE_STRUCT_MEMBERS_IMPL(                                    \
        NAME,                                                                   \
        BOOST_PP_CAT(BOOST_FUSION_ADAPT_STRUCT_FILLER_0 ATTRIBUTES,_END))

// Note: can't compute BOOST_PP_SEQ_SIZE(ATTRIBUTES_SEQ) directly because
//       ATTRIBUTES_SEQ may be empty and calling BOOST_PP_SEQ_SIZE on an empty
//       sequence produces warnings on MSVC.
#define BOOST_FUSION_DEFINE_STRUCT_MEMBERS_IMPL(NAME, ATTRIBUTES_SEQ)           \
    BOOST_FUSION_DEFINE_STRUCT_INLINE_MEMBERS_IMPL_IMPL(                        \
        NAME,                                                                   \
        ATTRIBUTES_SEQ,                                                         \
        BOOST_PP_DEC(BOOST_PP_SEQ_SIZE((0)ATTRIBUTES_SEQ)))

#define BOOST_FUSION_DEFINE_STRUCT_INLINE_ITERATOR(NAME, ATTRIBUTES)            \
    BOOST_FUSION_DEFINE_STRUCT_ITERATOR_IMPL(                                   \
        NAME,                                                                   \
        BOOST_PP_CAT(BOOST_FUSION_ADAPT_STRUCT_FILLER_0 ATTRIBUTES,_END))

#define BOOST_FUSION_DEFINE_STRUCT_ITERATOR_IMPL(NAME, ATTRIBUTES_SEQ)          \
    BOOST_FUSION_DEFINE_STRUCT_INLINE_ITERATOR_IMPL_IMPL(                       \
        NAME,                                                                   \
        ATTRIBUTES_SEQ,                                                         \
        BOOST_PP_DEC(BOOST_PP_SEQ_SIZE((0)ATTRIBUTES_SEQ)))

#define BOOST_FUSION_DEFINE_STRUCT_INLINE_ITERATOR_IMPL_IMPL(                   \
    NAME, ATTRIBUTES_SEQ, ATTRIBUTES_SEQ_SIZE)                                  \
                                                                                \
    template <typename lslboost_fusion_detail_Seq, int N>                          \
    struct BOOST_FUSION_ITERATOR_NAME(NAME)                                     \
        : lslboost::fusion::iterator_facade<                                       \
              BOOST_FUSION_ITERATOR_NAME(NAME)<lslboost_fusion_detail_Seq, N>,     \
              lslboost::fusion::random_access_traversal_tag                        \
          >                                                                     \
    {                                                                           \
        typedef lslboost::mpl::int_<N> index;                                      \
        typedef lslboost_fusion_detail_Seq sequence_type;                          \
                                                                                \
        BOOST_FUSION_ITERATOR_NAME(NAME)(lslboost_fusion_detail_Seq& seq)          \
            : seq_(seq)                                                         \
              BOOST_FUSION_DEFINE_ITERATOR_WKND_INIT_LIST_ENTRIES(              \
                      (0)ATTRIBUTES_SEQ)                                        \
        {}                                                                      \
                                                                                \
        lslboost_fusion_detail_Seq& seq_;                                          \
                                                                                \
        BOOST_FUSION_DEFINE_ITERATOR_WKND_MEMBERS(ATTRIBUTES_SEQ_SIZE)          \
                                                                                \
        BOOST_FUSION_DEFINE_ITERATOR_VALUE_OF(NAME, ATTRIBUTES_SEQ_SIZE)        \
                                                                                \
        BOOST_FUSION_DEFINE_ITERATOR_DEREF(NAME, ATTRIBUTES_SEQ)                \
                                                                                \
        template <typename lslboost_fusion_detail_It>                              \
        struct next                                                             \
        {                                                                       \
            typedef BOOST_FUSION_ITERATOR_NAME(NAME)<                           \
                typename lslboost_fusion_detail_It::sequence_type,                 \
                lslboost_fusion_detail_It::index::value + 1                        \
            > type;                                                             \
                                                                                \
            static type call(lslboost_fusion_detail_It const& it)                  \
            {                                                                   \
                return type(it.seq_);                                           \
            }                                                                   \
        };                                                                      \
                                                                                \
        template <typename lslboost_fusion_detail_It>                              \
        struct prior                                                            \
        {                                                                       \
            typedef BOOST_FUSION_ITERATOR_NAME(NAME)<                           \
                typename lslboost_fusion_detail_It::sequence_type,                 \
                lslboost_fusion_detail_It::index::value - 1                        \
            > type;                                                             \
                                                                                \
            static type call(lslboost_fusion_detail_It const& it)                  \
            {                                                                   \
                return type(it.seq_);                                           \
            }                                                                   \
        };                                                                      \
                                                                                \
        template <                                                              \
            typename lslboost_fusion_detail_It1,                                   \
            typename lslboost_fusion_detail_It2                                    \
        >                                                                       \
        struct distance                                                         \
        {                                                                       \
            typedef typename lslboost::mpl::minus<                                 \
                typename lslboost_fusion_detail_It2::index,                        \
                typename lslboost_fusion_detail_It1::index                         \
            >::type type;                                                       \
                                                                                \
             static type call(lslboost_fusion_detail_It1 const& it1,               \
                              lslboost_fusion_detail_It2 const& it2)               \
            {                                                                   \
                return type();                                                  \
            }                                                                   \
        };                                                                      \
                                                                                \
        template <                                                              \
            typename lslboost_fusion_detail_It,                                    \
            typename lslboost_fusion_detail_M                                      \
        >                                                                       \
        struct advance                                                          \
        {                                                                       \
            typedef BOOST_FUSION_ITERATOR_NAME(NAME)<                           \
                typename lslboost_fusion_detail_It::sequence_type,                 \
                lslboost_fusion_detail_It::index::value                            \
                    + lslboost_fusion_detail_M::value                              \
            > type;                                                             \
                                                                                \
            static type call(lslboost_fusion_detail_It const& it)                  \
            {                                                                   \
                return type(it.seq_);                                           \
            }                                                                   \
        };                                                                      \
    };


#define BOOST_FUSION_DEFINE_STRUCT_INLINE_MEMBERS_IMPL_IMPL(                    \
    NAME, ATTRIBUTES_SEQ, ATTRIBUTES_SEQ_SIZE)                                  \
                                                                                \
    /* Note: second BOOST_PP_IF is necessary to avoid MSVC warning when */      \
    /*       calling BOOST_FUSION_IGNORE_1 with no arguments.           */      \
    NAME()                                                                      \
        BOOST_PP_IF(                                                            \
            ATTRIBUTES_SEQ_SIZE,                                                \
            BOOST_FUSION_MAKE_DEFAULT_INIT_LIST,                                \
            BOOST_FUSION_IGNORE_1)                                              \
                (BOOST_PP_IF(                                                   \
                    ATTRIBUTES_SEQ_SIZE,                                        \
                    ATTRIBUTES_SEQ,                                             \
                    0))                                                         \
    {                                                                           \
    }                                                                           \
                                                                                \
    BOOST_PP_IF(                                                                \
        ATTRIBUTES_SEQ_SIZE,                                                    \
        BOOST_FUSION_MAKE_COPY_CONSTRUCTOR,                                     \
        BOOST_FUSION_IGNORE_2)                                                  \
            (NAME, ATTRIBUTES_SEQ)                                              \
                                                                                \
    template <typename lslboost_fusion_detail_Seq>                                 \
    NAME(const lslboost_fusion_detail_Seq& rhs)                                    \
    {                                                                           \
        lslboost::fusion::copy(rhs, *this);                                        \
    }                                                                           \
                                                                                \
    template <typename lslboost_fusion_detail_Seq>                                 \
    NAME& operator=(const lslboost_fusion_detail_Seq& rhs)                         \
    {                                                                           \
        lslboost::fusion::copy(rhs, *this);                                        \
        return *this;                                                           \
    }                                                                           \
                                                                                \
    template <typename lslboost_fusion_detail_Sq>                                  \
    struct begin                                                                \
    {                                                                           \
        typedef BOOST_FUSION_ITERATOR_NAME(NAME)<lslboost_fusion_detail_Sq, 0>     \
             type;                                                              \
                                                                                \
        static type call(lslboost_fusion_detail_Sq& sq)                            \
        {                                                                       \
            return type(sq);                                                    \
        }                                                                       \
    };                                                                          \
                                                                                \
    template <typename lslboost_fusion_detail_Sq>                                  \
    struct end                                                                  \
    {                                                                           \
        typedef BOOST_FUSION_ITERATOR_NAME(NAME)<                               \
            lslboost_fusion_detail_Sq,                                             \
            ATTRIBUTES_SEQ_SIZE                                                 \
        > type;                                                                 \
                                                                                \
        static type call(lslboost_fusion_detail_Sq& sq)                            \
        {                                                                       \
            return type(sq);                                                    \
        }                                                                       \
    };                                                                          \
                                                                                \
    template <typename lslboost_fusion_detail_Sq>                                  \
    struct size : lslboost::mpl::int_<ATTRIBUTES_SEQ_SIZE>                         \
    {                                                                           \
    };                                                                          \
                                                                                \
    template <typename lslboost_fusion_detail_Sq>                                  \
    struct empty : lslboost::mpl::bool_<ATTRIBUTES_SEQ_SIZE == 0>                  \
    {                                                                           \
    };                                                                          \
                                                                                \
    template <                                                                  \
        typename lslboost_fusion_detail_Sq,                                        \
        typename lslboost_fusion_detail_N                                          \
    >                                                                           \
    struct value_at : value_at<                                                 \
                          lslboost_fusion_detail_Sq,                               \
                          lslboost::mpl::int_<lslboost_fusion_detail_N::value>        \
                      >                                                         \
    {                                                                           \
    };                                                                          \
                                                                                \
    BOOST_PP_REPEAT(                                                            \
        ATTRIBUTES_SEQ_SIZE,                                                    \
        BOOST_FUSION_MAKE_VALUE_AT_SPECS,                                       \
        ~)                                                                      \
                                                                                \
    template <                                                                  \
        typename lslboost_fusion_detail_Sq,                                        \
        typename lslboost_fusion_detail_N                                          \
    >                                                                           \
    struct at : at<                                                             \
                    lslboost_fusion_detail_Sq,                                     \
                    lslboost::mpl::int_<lslboost_fusion_detail_N::value>              \
                >                                                               \
    {                                                                           \
    };                                                                          \
                                                                                \
    BOOST_PP_SEQ_FOR_EACH_I(BOOST_FUSION_MAKE_AT_SPECS, ~, ATTRIBUTES_SEQ)      \
                                                                                \
    BOOST_PP_SEQ_FOR_EACH_I(BOOST_FUSION_MAKE_TYPEDEF, ~, ATTRIBUTES_SEQ)       \
                                                                                \
    BOOST_PP_SEQ_FOR_EACH_I(                                                    \
        BOOST_FUSION_MAKE_DATA_MEMBER,                                          \
        ~,                                                                      \
        ATTRIBUTES_SEQ)

#endif
