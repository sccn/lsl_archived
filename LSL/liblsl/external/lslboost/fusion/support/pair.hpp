/*=============================================================================
    Copyright (c) 2001-2011 Joel de Guzman
    Copyright (c) 2006 Tobias Schwinger

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.lslboost.org/LICENSE_1_0.txt)
==============================================================================*/
#if !defined(FUSION_PAIR_07222005_1203)
#define FUSION_PAIR_07222005_1203

#include <iosfwd>

#include <lslboost/fusion/support/detail/access.hpp>
#include <lslboost/fusion/support/detail/as_fusion_element.hpp>
#include <lslboost/config.hpp>
#include <lslboost/utility/enable_if.hpp>
#include <lslboost/type_traits/is_convertible.hpp>

#if defined (BOOST_MSVC)
#  pragma warning(push)
#  pragma warning (disable: 4512) // assignment operator could not be generated.
#endif

namespace lslboost { namespace fusion
{
    // A half runtime pair where the first type does not have data
    template <typename First, typename Second>
    struct pair
    {
        pair()
            : second() {}

        pair(pair const& rhs)
            : second(rhs.second) {}

#if !defined(BOOST_NO_CXX11_RVALUE_REFERENCES)
        pair(pair&& rhs)
            : second(std::forward<Second>(rhs.second)) {}
#endif

        pair(typename detail::call_param<Second>::type val)
            : second(val) {}

#if !defined(BOOST_NO_CXX11_RVALUE_REFERENCES)

        template <typename Second2>
        pair(Second2&& val
          , typename lslboost::enable_if<is_convertible<Second2, Second> >::type* /*dummy*/ = 0
        ) : second(std::forward<Second2>(val)) {}

#endif

        template <typename Second2>
        pair(pair<First, Second2> const& rhs)
            : second(rhs.second) {}

        template <typename Second2>
        pair& operator=(pair<First, Second2> const& rhs)
        {
            second = rhs.second;
            return *this;
        }

        pair& operator=(pair const& rhs)
        {
            second = rhs.second;
            return *this;
        }

#if !defined(BOOST_NO_CXX11_RVALUE_REFERENCES)
        pair& operator=(pair&& rhs)
        {
            second = std::forward<Second>(rhs.second);
            return *this;
        }
#endif

        typedef First first_type;
        typedef Second second_type;
        Second second;
    };

    namespace result_of
    {
        template<typename First, typename Second>
        struct make_pair
        {
            typedef fusion::pair<First,
                        typename detail::as_fusion_element<Second>::type> type;
        };

        template<class Pair>
        struct first
        {
            typedef typename Pair::first_type type;
        };

        template<class Pair>
        struct second
        {
            typedef typename Pair::second_type type;
        };
    }

    template <typename First, typename Second>
    inline typename result_of::make_pair<First,Second>::type
    make_pair(Second const& val)
    {
        return pair<First, typename detail::as_fusion_element<Second>::type>(val);
    }

    template <typename First, typename Second>
    inline std::ostream&
    operator<<(std::ostream& os, pair<First, Second> const& p)
    {
        os << p.second;
        return os;
    }

    template <typename First, typename Second>
    inline std::istream&
    operator>>(std::istream& is, pair<First, Second>& p)
    {
        is >> p.second;
        return is;
    }

    template <typename First, typename SecondL, typename SecondR>
    inline bool
    operator==(pair<First, SecondL> const& l, pair<First, SecondR> const& r)
    {
        return l.second == r.second;
    }

    template <typename First, typename SecondL, typename SecondR>
    inline bool
    operator!=(pair<First, SecondL> const& l, pair<First, SecondR> const& r)
    {
        return l.second != r.second;
    }

    template <typename First, typename SecondL, typename SecondR>
    inline bool
    operator<(pair<First, SecondL> const& l, pair<First, SecondR> const& r)
    {
        return l.second < r.second;
    }
}}

#if defined (BOOST_MSVC)
#  pragma warning(pop)
#endif

#endif
