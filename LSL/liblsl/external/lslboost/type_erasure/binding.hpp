// Boost.TypeErasure library
//
// Copyright 2011 Steven Watanabe
//
// Distributed under the Boost Software License Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.lslboost.org/LICENSE_1_0.txt)
//
// $Id$

#ifndef BOOST_TYPE_ERASURE_BINDING_HPP_INCLUDED
#define BOOST_TYPE_ERASURE_BINDING_HPP_INCLUDED

#include <lslboost/config.hpp>
#include <lslboost/shared_ptr.hpp>
#include <lslboost/make_shared.hpp>
#include <lslboost/utility/enable_if.hpp>
#include <lslboost/mpl/transform.hpp>
#include <lslboost/mpl/find_if.hpp>
#include <lslboost/mpl/and.hpp>
#include <lslboost/mpl/not.hpp>
#include <lslboost/mpl/end.hpp>
#include <lslboost/mpl/bool.hpp>
#include <lslboost/mpl/pair.hpp>
#include <lslboost/type_traits/is_same.hpp>
#include <lslboost/type_erasure/static_binding.hpp>
#include <lslboost/type_erasure/is_subconcept.hpp>
#include <lslboost/type_erasure/detail/adapt_to_vtable.hpp>
#include <lslboost/type_erasure/detail/null.hpp>
#include <lslboost/type_erasure/detail/rebind_placeholders.hpp>
#include <lslboost/type_erasure/detail/vtable.hpp>
#include <lslboost/type_erasure/detail/normalize.hpp>
#include <lslboost/type_erasure/detail/instantiate.hpp>
#include <lslboost/type_erasure/detail/check_map.hpp>

namespace lslboost {
namespace type_erasure {

namespace detail {

template<class Source, class Dest, class Map>
struct can_optimize_conversion : ::lslboost::mpl::and_<
    ::lslboost::is_same<Source, Dest>,
        ::lslboost::is_same<
            typename ::lslboost::mpl::find_if<
                Map,
                ::lslboost::mpl::not_<
                    ::lslboost::is_same<
                        ::lslboost::mpl::first< ::lslboost::mpl::_1>,
                        ::lslboost::mpl::second< ::lslboost::mpl::_1>
                    >
                >
            >::type,
            typename ::lslboost::mpl::end<Map>::type
        >
    >::type
{};

}

/**
 * Stores the binding of a @c Concept to a set of actual types.
 * @c Concept is interpreted in the same way as with @ref any.
 */
template<class Concept>
class binding
{
    typedef typename ::lslboost::type_erasure::detail::normalize_concept<
        Concept>::type normalized;
    typedef typename ::lslboost::mpl::transform<normalized,
        ::lslboost::type_erasure::detail::maybe_adapt_to_vtable< ::lslboost::mpl::_1>
    >::type actual_concept;
    typedef typename ::lslboost::type_erasure::detail::make_vtable<
        actual_concept>::type table_type;
    typedef typename ::lslboost::type_erasure::detail::get_placeholder_normalization_map<
        Concept
    >::type placeholder_subs;
public:

    /**
     * \pre @ref relaxed must be in @c Concept.
     *
     * \throws Nothing.
     */
    binding() { BOOST_MPL_ASSERT((::lslboost::type_erasure::is_relaxed<Concept>)); }
    
    /**
     * \pre @c Map must be an MPL map with an entry for each placeholder
     *      referred to by @c Concept.
     *
     * \throws Nothing.
     */
    template<class Map>
    explicit binding(const Map&)
      : impl((
            BOOST_TYPE_ERASURE_INSTANTIATE(Concept, Map),
            static_binding<Map>()
        ))
    {}
    
    /**
     * \pre @c Map must be an MPL map with an entry for each placeholder
     *      referred to by @c Concept.
     *
     * \throws Nothing.
     */
    template<class Map>
    binding(const static_binding<Map>&)
      : impl((
            BOOST_TYPE_ERASURE_INSTANTIATE(Concept, Map),
            static_binding<Map>()
        ))
    {}

    /**
     * Converts from another set of bindings.
     *
     * \pre Map must be an MPL map with an entry for each placeholder
     *      referred to by @c Concept.  The mapped type should be the
     *      corresponding placeholder in Concept2.
     *
     * \throws std::bad_alloc
     */
    template<class Concept2, class Map>
    binding(const binding<Concept2>& other, const Map&
#ifndef BOOST_TYPE_ERASURE_DOXYGEN
        , typename ::lslboost::enable_if<
            ::lslboost::mpl::and_<
                ::lslboost::type_erasure::detail::check_map<Concept, Map>,
                ::lslboost::type_erasure::is_subconcept<Concept, Concept2, Map>
            >
        >::type* = 0
#endif
        )
      : impl(
            other,
            static_binding<Map>(),
            ::lslboost::type_erasure::detail::can_optimize_conversion<Concept2, Concept, Map>()
        )
    {}

    /**
     * Converts from another set of bindings.
     *
     * \pre Map must be an MPL map with an entry for each placeholder
     *      referred to by @c Concept.  The mapped type should be the
     *      corresponding placeholder in Concept2.
     *
     * \throws std::bad_alloc
     */
    template<class Concept2, class Map>
    binding(const binding<Concept2>& other, const static_binding<Map>&
#ifndef BOOST_TYPE_ERASURE_DOXYGEN
        , typename ::lslboost::enable_if<
            ::lslboost::mpl::and_<
                ::lslboost::type_erasure::detail::check_map<Concept, Map>,
                ::lslboost::type_erasure::is_subconcept<Concept, Concept2, Map>
            >
        >::type* = 0
#endif
        )
      : impl(
            other,
            static_binding<Map>(),
            ::lslboost::type_erasure::detail::can_optimize_conversion<Concept2, Concept, Map>()
        )
    {}

    /**
     * \return true iff the sets of types that the placeholders
     *         bind to are the same for both arguments.
     *
     * \throws Nothing.
     */
    friend bool operator==(const binding& lhs, const binding& rhs)
    { return *lhs.impl.table == *rhs.impl.table; }
    
    /**
     * \return true iff the arguments do not map to identical
     *         sets of types.
     *
     * \throws Nothing.
     */
    friend bool operator!=(const binding& lhs, const binding& rhs)
    { return !(lhs == rhs); }

    /** INTERNAL ONLY */
    template<class T>
    typename T::type find() const { return impl.table->lookup((T*)0); }
private:
    template<class C2>
    friend class binding;
    /** INTERNAL ONLY */
    struct impl_type
    {
        impl_type() {
            table = &::lslboost::type_erasure::detail::make_vtable_init<
                typename ::lslboost::mpl::transform<
                    actual_concept,
                    ::lslboost::type_erasure::detail::get_null_vtable_entry<
                        ::lslboost::mpl::_1
                    >
                >::type,
                table_type
            >::type::value;
        }
        template<class Map>
        impl_type(const static_binding<Map>&)
        {
            table = &::lslboost::type_erasure::detail::make_vtable_init<
                typename ::lslboost::mpl::transform<
                    actual_concept,
                    ::lslboost::type_erasure::detail::rebind_placeholders<
                        ::lslboost::mpl::_1,
                        typename ::lslboost::type_erasure::detail::add_deductions<
                            Map,
                            placeholder_subs
                        >::type
                    >
                >::type,
                table_type
            >::type::value;
        }
        template<class Concept2, class Map>
        impl_type(const binding<Concept2>& other, const static_binding<Map>&, lslboost::mpl::false_)
          : manager(new table_type)
        {
            manager->template convert_from<
                typename ::lslboost::type_erasure::detail::convert_deductions<
                    Map,
                    placeholder_subs,
                    typename binding<Concept2>::placeholder_subs
                >::type
            >(*other.impl.table);
            table = manager.get();
        }
        template<class Concept2, class Map>
        impl_type(const binding<Concept2>& other, const static_binding<Map>&, lslboost::mpl::true_)
          : table(other.impl.table),
            manager(other.impl.manager)
        {}
        const table_type* table;
        ::lslboost::shared_ptr<table_type> manager;
    } impl;
};

}
}

#endif
