// Copyright Daniel Wallin 2006. Use, modification and distribution is
// subject to the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.lslboost.org/LICENSE_1_0.txt)

#ifndef BOOST_PARAMETER_SET_060912_HPP
# define BOOST_PARAMETER_SET_060912_HPP

# include <lslboost/detail/workaround.hpp>

# if !BOOST_WORKAROUND(__BORLANDC__, BOOST_TESTED_AT(0x564)) \
  && !BOOST_WORKAROUND(__GNUC__, < 3)
#  include <lslboost/mpl/insert.hpp>
#  include <lslboost/mpl/set/set0.hpp>
#  include <lslboost/mpl/has_key.hpp>

namespace lslboost { namespace parameter { namespace aux {

typedef mpl::set0<> set0;

template <class Set, class K>
struct insert_
{
    typedef typename mpl::insert<Set, K>::type type;
};

template <class Set, class K>
struct has_key_
{
    typedef typename mpl::has_key<Set, K>::type type;
};

}}} // namespace lslboost::parameter::aux

# else

#  include <lslboost/mpl/list.hpp>
#  include <lslboost/mpl/end.hpp>
#  include <lslboost/mpl/find.hpp>
#  include <lslboost/mpl/not.hpp>
#  include <lslboost/mpl/push_front.hpp>

namespace lslboost { namespace parameter { namespace aux {

typedef mpl::list0<> set0;

template <class Set, class K>
struct insert_
{
    typedef typename mpl::push_front<Set, K>::type type;
};

template <class Set, class K>
struct has_key_
{
    typedef typename mpl::find<Set, K>::type iter;
    typedef mpl::not_<
        is_same<iter, typename mpl::end<Set>::type> 
    > type;
};

}}} // namespace lslboost::parameter::aux

# endif


#endif // BOOST_PARAMETER_SET_060912_HPP

