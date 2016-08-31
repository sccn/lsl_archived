// Copyright Daniel Wallin 2006. Use, modification and distribution is
// subject to the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.lslboost.org/LICENSE_1_0.txt)

#ifndef BOOST_PARAMETER_NAME_060806_HPP
# define BOOST_PARAMETER_NAME_060806_HPP

# include <lslboost/parameter/keyword.hpp>
# include <lslboost/parameter/value_type.hpp>
# include <lslboost/detail/workaround.hpp>
# include <lslboost/preprocessor/cat.hpp>
# include <lslboost/preprocessor/stringize.hpp>
# include <lslboost/preprocessor/control/iif.hpp>
# include <lslboost/preprocessor/tuple/eat.hpp>
# include <lslboost/preprocessor/tuple/elem.hpp>
# include <lslboost/mpl/placeholders.hpp>

# if !defined(BOOST_NO_SFINAE) \
  && !defined(BOOST_NO_TEMPLATE_PARTIAL_SPECIALIZATION) \
  && !BOOST_WORKAROUND(__BORLANDC__, BOOST_TESTED_AT(0x592))

#  include <lslboost/utility/enable_if.hpp>
#  include <lslboost/mpl/lambda.hpp>

namespace lslboost { namespace parameter { namespace aux {

// Tag type passed to MPL lambda.
struct lambda_tag;

struct name_tag_base
{};

template <class Tag>
struct name_tag
{};

template <class T>
struct is_name_tag
  : mpl::false_
{};

}}} // namespace lslboost::parameter::aux

namespace lslboost { namespace mpl {

template <class T>
struct lambda<
    T
  , typename lslboost::enable_if<
        parameter::aux::is_name_tag<T>, parameter::aux::lambda_tag
    >::type
>
{
    typedef true_ is_le;
    typedef bind3< quote3<parameter::value_type>, arg<2>, T, void> result_;
    typedef result_ type;
};

}} // namespace lslboost::mpl

# endif

# if BOOST_WORKAROUND(__BORLANDC__, BOOST_TESTED_AT(0x564))
# include <lslboost/preprocessor/detail/split.hpp>
// From Paul Mensonides
#  define BOOST_PARAMETER_IS_BINARY(x) \
    BOOST_PP_SPLIT(1, BOOST_PARAMETER_IS_BINARY_C x BOOST_PP_COMMA() 0) \
    /**/
#  define BOOST_PARAMETER_IS_BINARY_C(x,y) \
    ~, 1 BOOST_PP_RPAREN() \
    BOOST_PP_TUPLE_EAT(2) BOOST_PP_LPAREN() ~ \
    /**/
# else
#  include <lslboost/preprocessor/detail/is_binary.hpp>
#  define BOOST_PARAMETER_IS_BINARY(x) BOOST_PP_IS_BINARY(x)
# endif

# if BOOST_WORKAROUND(BOOST_MSVC, < 1300)
#  define BOOST_PARAMETER_NAME_OBJECT(tag, name)                    \
    static ::lslboost::parameter::keyword<tag> const& name             \
       = ::lslboost::parameter::keyword<tag>::instance;
# else
#  define BOOST_PARAMETER_NAME_OBJECT(tag, name)                    \
    namespace                                                       \
    {                                                               \
       ::lslboost::parameter::keyword<tag> const& name                 \
       = ::lslboost::parameter::keyword<tag>::instance;                \
    }
# endif

# define BOOST_PARAMETER_BASIC_NAME(tag_namespace, tag, name)       \
    namespace tag_namespace                                         \
    {                                                               \
      struct tag                                                    \
      {                                                             \
          static char const* keyword_name()                         \
          {                                                         \
              return BOOST_PP_STRINGIZE(tag);                       \
          }                                                         \
                                                                    \
          typedef lslboost::parameter::value_type<                     \
              lslboost::mpl::_2, tag, lslboost::parameter::void_          \
          > _;                                                      \
                                                                    \
          typedef lslboost::parameter::value_type<                     \
              lslboost::mpl::_2, tag, lslboost::parameter::void_          \
          > _1;                                                     \
      };                                                            \
    }                                                               \
    BOOST_PARAMETER_NAME_OBJECT(tag_namespace::tag, name)

# define BOOST_PARAMETER_COMPLEX_NAME_TUPLE1(tag,namespace)         \
    (tag, namespace), ~

# define BOOST_PARAMETER_COMPLEX_NAME_TUPLE(name)                   \
    BOOST_PP_TUPLE_ELEM(2, 0, (BOOST_PARAMETER_COMPLEX_NAME_TUPLE1 name))

# define BOOST_PARAMETER_COMPLEX_NAME_TAG(name)                     \
    BOOST_PP_TUPLE_ELEM(2, 0, BOOST_PARAMETER_COMPLEX_NAME_TUPLE(name))

# define BOOST_PARAMETER_COMPLEX_NAME_NAMESPACE(name)               \
    BOOST_PP_TUPLE_ELEM(2, 1, BOOST_PARAMETER_COMPLEX_NAME_TUPLE(name))

# define BOOST_PARAMETER_COMPLEX_NAME(name)                         \
    BOOST_PARAMETER_BASIC_NAME(                                     \
        BOOST_PARAMETER_COMPLEX_NAME_NAMESPACE(name)                \
      , BOOST_PP_TUPLE_EAT(2) name                                  \
      , BOOST_PARAMETER_COMPLEX_NAME_TAG(name)                      \
    )                                                               \
/**/

# define BOOST_PARAMETER_SIMPLE_NAME(name)                          \
    BOOST_PARAMETER_BASIC_NAME(tag, name, BOOST_PP_CAT(_, name))

# define BOOST_PARAMETER_NAME(name)                                 \
    BOOST_PP_IIF(                                                   \
        BOOST_PARAMETER_IS_BINARY(name)                             \
      , BOOST_PARAMETER_COMPLEX_NAME                                \
      , BOOST_PARAMETER_SIMPLE_NAME                                 \
    )(name)                                                         \
/**/


# define BOOST_PARAMETER_TEMPLATE_KEYWORD(name)                     \
    namespace tag                                                   \
    {                                                               \
      struct name;                                                  \
    }                                                               \
    template <class T>                                              \
    struct name                                                     \
      : lslboost::parameter::template_keyword<tag::name, T>            \
    {};                                                             \
/**/

#endif // BOOST_PARAMETER_NAME_060806_HPP

