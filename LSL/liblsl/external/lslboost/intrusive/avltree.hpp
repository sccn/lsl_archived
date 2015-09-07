/////////////////////////////////////////////////////////////////////////////
//
// (C) Copyright Ion Gaztanaga 2007-2013
//
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.lslboost.org/LICENSE_1_0.txt)
//
// See http://www.lslboost.org/libs/intrusive for documentation.
//
/////////////////////////////////////////////////////////////////////////////
#ifndef BOOST_INTRUSIVE_AVLTREE_HPP
#define BOOST_INTRUSIVE_AVLTREE_HPP

#include <lslboost/intrusive/detail/config_begin.hpp>
#include <lslboost/intrusive/intrusive_fwd.hpp>
#include <cstddef>
#include <lslboost/intrusive/detail/minimal_less_equal_header.hpp>
#include <lslboost/intrusive/detail/minimal_pair_header.hpp>

#include <lslboost/static_assert.hpp>
#include <lslboost/intrusive/avl_set_hook.hpp>
#include <lslboost/intrusive/detail/avltree_node.hpp>
#include <lslboost/intrusive/bstree.hpp>
#include <lslboost/intrusive/detail/tree_node.hpp>
#include <lslboost/intrusive/detail/ebo_functor_holder.hpp>
#include <lslboost/intrusive/detail/mpl.hpp>
#include <lslboost/intrusive/pointer_traits.hpp>
#include <lslboost/intrusive/detail/get_value_traits.hpp>
#include <lslboost/intrusive/avltree_algorithms.hpp>
#include <lslboost/intrusive/link_mode.hpp>
#include <lslboost/move/utility_core.hpp>

#if defined(BOOST_HAS_PRAGMA_ONCE)
#  pragma once
#endif

namespace lslboost {
namespace intrusive {

/// @cond

struct default_avltree_hook_applier
{  template <class T> struct apply{ typedef typename T::default_avltree_hook type;  };  };

template<>
struct is_default_hook_tag<default_avltree_hook_applier>
{  static const bool value = true;  };

struct avltree_defaults
   : bstree_defaults
{
   typedef default_avltree_hook_applier proto_value_traits;
};

/// @endcond

//! The class template avltree is an intrusive AVL tree container, that
//! is used to construct intrusive avl_set and avl_multiset containers.
//! The no-throw guarantee holds only, if the key_compare object
//! doesn't throw.
//!
//! The template parameter \c T is the type to be managed by the container.
//! The user can specify additional options and if no options are provided
//! default options are used.
//!
//! The container supports the following options:
//! \c base_hook<>/member_hook<>/value_traits<>,
//! \c constant_time_size<>, \c size_type<> and
//! \c compare<>.
#if defined(BOOST_INTRUSIVE_DOXYGEN_INVOKED)
template<class T, class ...Options>
#else
template<class ValueTraits, class VoidOrKeyOfValue, class VoidOrKeyComp, class SizeType, bool ConstantTimeSize, typename HeaderHolder>
#endif
class avltree_impl
   /// @cond
   :  public bstree_impl<ValueTraits, VoidOrKeyOfValue, VoidOrKeyComp, SizeType, ConstantTimeSize, AvlTreeAlgorithms, HeaderHolder>
   /// @endcond
{
   public:
   typedef ValueTraits value_traits;
   /// @cond
   typedef bstree_impl< ValueTraits, VoidOrKeyOfValue, VoidOrKeyComp, SizeType
                      , ConstantTimeSize, AvlTreeAlgorithms
                      , HeaderHolder>                                tree_type;
   typedef tree_type                                                 implementation_defined;
   /// @endcond

   typedef typename implementation_defined::pointer                  pointer;
   typedef typename implementation_defined::const_pointer            const_pointer;
   typedef typename implementation_defined::value_type               value_type;
   typedef typename implementation_defined::key_type                 key_type;
   typedef typename implementation_defined::key_of_value             key_of_value;
   typedef typename implementation_defined::reference                reference;
   typedef typename implementation_defined::const_reference          const_reference;
   typedef typename implementation_defined::difference_type          difference_type;
   typedef typename implementation_defined::size_type                size_type;
   typedef typename implementation_defined::value_compare            value_compare;
   typedef typename implementation_defined::key_compare              key_compare;
   typedef typename implementation_defined::iterator                 iterator;
   typedef typename implementation_defined::const_iterator           const_iterator;
   typedef typename implementation_defined::reverse_iterator         reverse_iterator;
   typedef typename implementation_defined::const_reverse_iterator   const_reverse_iterator;
   typedef typename implementation_defined::node_traits              node_traits;
   typedef typename implementation_defined::node                     node;
   typedef typename implementation_defined::node_ptr                 node_ptr;
   typedef typename implementation_defined::const_node_ptr           const_node_ptr;
   typedef typename implementation_defined::node_algorithms          node_algorithms;

   static const bool constant_time_size = implementation_defined::constant_time_size;
   /// @cond
   private:

   //noncopyable
   BOOST_MOVABLE_BUT_NOT_COPYABLE(avltree_impl)

   /// @endcond

   public:

   typedef typename implementation_defined::insert_commit_data insert_commit_data;


   //! @copydoc ::lslboost::intrusive::bstree::bstree(const key_compare &,const value_traits &)
   explicit avltree_impl( const key_compare &cmp = key_compare()
                       , const value_traits &v_traits = value_traits())
      :  tree_type(cmp, v_traits)
   {}

   //! @copydoc ::lslboost::intrusive::bstree::bstree(bool,Iterator,Iterator,const key_compare &,const value_traits &)
   template<class Iterator>
   avltree_impl( bool unique, Iterator b, Iterator e
              , const key_compare &cmp     = key_compare()
              , const value_traits &v_traits = value_traits())
      : tree_type(unique, b, e, cmp, v_traits)
   {}

   //! @copydoc ::lslboost::intrusive::bstree::bstree(bstree &&)
   avltree_impl(BOOST_RV_REF(avltree_impl) x)
      :  tree_type(BOOST_MOVE_BASE(tree_type, x))
   {}

   //! @copydoc ::lslboost::intrusive::bstree::operator=(bstree &&)
   avltree_impl& operator=(BOOST_RV_REF(avltree_impl) x)
   {  return static_cast<avltree_impl&>(tree_type::operator=(BOOST_MOVE_BASE(tree_type, x))); }

   #ifdef BOOST_INTRUSIVE_DOXYGEN_INVOKED

   //! @copydoc ::lslboost::intrusive::bstree::~bstree()
   ~avltree_impl();

   //! @copydoc ::lslboost::intrusive::bstree::begin()
   iterator begin();

   //! @copydoc ::lslboost::intrusive::bstree::begin()const
   const_iterator begin() const;

   //! @copydoc ::lslboost::intrusive::bstree::cbegin()const
   const_iterator cbegin() const;

   //! @copydoc ::lslboost::intrusive::bstree::end()
   iterator end();

   //! @copydoc ::lslboost::intrusive::bstree::end()const
   const_iterator end() const;

   //! @copydoc ::lslboost::intrusive::bstree::cend()const
   const_iterator cend() const;

   //! @copydoc ::lslboost::intrusive::bstree::rbegin()
   reverse_iterator rbegin();

   //! @copydoc ::lslboost::intrusive::bstree::rbegin()const
   const_reverse_iterator rbegin() const;

   //! @copydoc ::lslboost::intrusive::bstree::crbegin()const
   const_reverse_iterator crbegin() const;

   //! @copydoc ::lslboost::intrusive::bstree::rend()
   reverse_iterator rend();

   //! @copydoc ::lslboost::intrusive::bstree::rend()const
   const_reverse_iterator rend() const;

   //! @copydoc ::lslboost::intrusive::bstree::crend()const
   const_reverse_iterator crend() const;

   //! @copydoc ::lslboost::intrusive::bstree::container_from_end_iterator(iterator)
   static avltree_impl &container_from_end_iterator(iterator end_iterator);

   //! @copydoc ::lslboost::intrusive::bstree::container_from_end_iterator(const_iterator)
   static const avltree_impl &container_from_end_iterator(const_iterator end_iterator);

   //! @copydoc ::lslboost::intrusive::bstree::container_from_iterator(iterator)
   static avltree_impl &container_from_iterator(iterator it);

   //! @copydoc ::lslboost::intrusive::bstree::container_from_iterator(const_iterator)
   static const avltree_impl &container_from_iterator(const_iterator it);

   //! @copydoc ::lslboost::intrusive::bstree::key_comp()const
   key_compare key_comp() const;

   //! @copydoc ::lslboost::intrusive::bstree::value_comp()const
   value_compare value_comp() const;

   //! @copydoc ::lslboost::intrusive::bstree::empty()const
   bool empty() const;

   //! @copydoc ::lslboost::intrusive::bstree::size()const
   size_type size() const;

   //! @copydoc ::lslboost::intrusive::bstree::swap
   void swap(avltree_impl& other);

   //! @copydoc ::lslboost::intrusive::bstree::clone_from(const bstree&,Cloner,Disposer)
   template <class Cloner, class Disposer>
   void clone_from(const avltree_impl &src, Cloner cloner, Disposer disposer);

   #else //BOOST_INTRUSIVE_DOXYGEN_INVOKED

   using tree_type::clone_from;

   #endif   //#ifdef BOOST_INTRUSIVE_DOXYGEN_INVOKED

   //! @copydoc ::lslboost::intrusive::bstree::clone_from(bstree&&,Cloner,Disposer)
   template <class Cloner, class Disposer>
   void clone_from(BOOST_RV_REF(avltree_impl) src, Cloner cloner, Disposer disposer)
   {  tree_type::clone_from(BOOST_MOVE_BASE(tree_type, src), cloner, disposer);  }

   #ifdef BOOST_INTRUSIVE_DOXYGEN_INVOKED

   //! @copydoc ::lslboost::intrusive::bstree::insert_equal(reference)
   iterator insert_equal(reference value);

   //! @copydoc ::lslboost::intrusive::bstree::insert_equal(const_iterator,reference)
   iterator insert_equal(const_iterator hint, reference value);

   //! @copydoc ::lslboost::intrusive::bstree::insert_equal(Iterator,Iterator)
   template<class Iterator>
   void insert_equal(Iterator b, Iterator e);

   //! @copydoc ::lslboost::intrusive::bstree::insert_unique(reference)
   std::pair<iterator, bool> insert_unique(reference value);

   //! @copydoc ::lslboost::intrusive::bstree::insert_unique(const_iterator,reference)
   iterator insert_unique(const_iterator hint, reference value);

   //! @copydoc ::lslboost::intrusive::bstree::insert_unique_check(const KeyType&,KeyTypeKeyCompare,insert_commit_data&)
   template<class KeyType, class KeyTypeKeyCompare>
   std::pair<iterator, bool> insert_unique_check
      (const KeyType &key, KeyTypeKeyCompare comp, insert_commit_data &commit_data);

   //! @copydoc ::lslboost::intrusive::bstree::insert_unique_check(const_iterator,const KeyType&,KeyTypeKeyCompare,insert_commit_data&)
   template<class KeyType, class KeyTypeKeyCompare>
   std::pair<iterator, bool> insert_unique_check
      (const_iterator hint, const KeyType &key
      ,KeyTypeKeyCompare comp, insert_commit_data &commit_data);

   //! @copydoc ::lslboost::intrusive::bstree::insert_unique_commit
   iterator insert_unique_commit(reference value, const insert_commit_data &commit_data);

   //! @copydoc ::lslboost::intrusive::bstree::insert_unique(Iterator,Iterator)
   template<class Iterator>
   void insert_unique(Iterator b, Iterator e);

   //! @copydoc ::lslboost::intrusive::bstree::insert_before
   iterator insert_before(const_iterator pos, reference value);

   //! @copydoc ::lslboost::intrusive::bstree::push_back
   void push_back(reference value);

   //! @copydoc ::lslboost::intrusive::bstree::push_front
   void push_front(reference value);

   //! @copydoc ::lslboost::intrusive::bstree::erase(const_iterator)
   iterator erase(const_iterator i);

   //! @copydoc ::lslboost::intrusive::bstree::erase(const_iterator,const_iterator)
   iterator erase(const_iterator b, const_iterator e);

   //! @copydoc ::lslboost::intrusive::bstree::erase(const key_type &)
   size_type erase(const key_type &key);

   //! @copydoc ::lslboost::intrusive::bstree::erase(const KeyType&,KeyTypeKeyCompare)
   template<class KeyType, class KeyTypeKeyCompare>
   size_type erase(const KeyType& key, KeyTypeKeyCompare comp);

   //! @copydoc ::lslboost::intrusive::bstree::erase_and_dispose(const_iterator,Disposer)
   template<class Disposer>
   iterator erase_and_dispose(const_iterator i, Disposer disposer);

   //! @copydoc ::lslboost::intrusive::bstree::erase_and_dispose(const_iterator,const_iterator,Disposer)
   template<class Disposer>
   iterator erase_and_dispose(const_iterator b, const_iterator e, Disposer disposer);

   //! @copydoc ::lslboost::intrusive::bstree::erase_and_dispose(const key_type &, Disposer)
   template<class Disposer>
   size_type erase_and_dispose(const key_type &key, Disposer disposer);

   //! @copydoc ::lslboost::intrusive::bstree::erase_and_dispose(const KeyType&,KeyTypeKeyCompare,Disposer)
   template<class KeyType, class KeyTypeKeyCompare, class Disposer>
   size_type erase_and_dispose(const KeyType& key, KeyTypeKeyCompare comp, Disposer disposer);

   //! @copydoc ::lslboost::intrusive::bstree::clear
   void clear();

   //! @copydoc ::lslboost::intrusive::bstree::clear_and_dispose
   template<class Disposer>
   void clear_and_dispose(Disposer disposer);

   //! @copydoc ::lslboost::intrusive::bstree::count(const key_type &ke)const
   size_type count(const key_type &key) const;

   //! @copydoc ::lslboost::intrusive::bstree::count(const KeyType&,KeyTypeKeyCompare)const
   template<class KeyType, class KeyTypeKeyCompare>
   size_type count(const KeyType& key, KeyTypeKeyCompare comp) const;

   //! @copydoc ::lslboost::intrusive::bstree::lower_bound(const key_type &)
   iterator lower_bound(const key_type &key);

   //! @copydoc ::lslboost::intrusive::bstree::lower_bound(const KeyType&,KeyTypeKeyCompare)
   template<class KeyType, class KeyTypeKeyCompare>
   iterator lower_bound(const KeyType& key, KeyTypeKeyCompare comp);

   //! @copydoc ::lslboost::intrusive::bstree::lower_bound(const key_type &)const
   const_iterator lower_bound(const key_type &key) const;

   //! @copydoc ::lslboost::intrusive::bstree::lower_bound(const KeyType&,KeyTypeKeyCompare)const
   template<class KeyType, class KeyTypeKeyCompare>
   const_iterator lower_bound(const KeyType& key, KeyTypeKeyCompare comp) const;

   //! @copydoc ::lslboost::intrusive::bstree::upper_bound(const key_type &key)
   iterator upper_bound(const key_type &key);

   //! @copydoc ::lslboost::intrusive::bstree::upper_bound(const KeyType&,KeyTypeKeyCompare)
   template<class KeyType, class KeyTypeKeyCompare>
   iterator upper_bound(const KeyType& key, KeyTypeKeyCompare comp);

   //! @copydoc ::lslboost::intrusive::bstree::upper_bound(const key_type &)const
   const_iterator upper_bound(const key_type &key) const;

   //! @copydoc ::lslboost::intrusive::bstree::upper_bound(const KeyType&,KeyTypeKeyCompare)const
   template<class KeyType, class KeyTypeKeyCompare>
   const_iterator upper_bound(const KeyType& key, KeyTypeKeyCompare comp) const;

   //! @copydoc ::lslboost::intrusive::bstree::find(const key_type &)
   iterator find(const key_type &key);

   //! @copydoc ::lslboost::intrusive::bstree::find(const KeyType&,KeyTypeKeyCompare)
   template<class KeyType, class KeyTypeKeyCompare>
   iterator find(const KeyType& key, KeyTypeKeyCompare comp);

   //! @copydoc ::lslboost::intrusive::bstree::find(const key_type &)const
   const_iterator find(const key_type &key) const;

   //! @copydoc ::lslboost::intrusive::bstree::find(const KeyType&,KeyTypeKeyCompare)const
   template<class KeyType, class KeyTypeKeyCompare>
   const_iterator find(const KeyType& key, KeyTypeKeyCompare comp) const;

   //! @copydoc ::lslboost::intrusive::bstree::equal_range(const key_type &)
   std::pair<iterator,iterator> equal_range(const key_type &key);

   //! @copydoc ::lslboost::intrusive::bstree::equal_range(const KeyType&,KeyTypeKeyCompare)
   template<class KeyType, class KeyTypeKeyCompare>
   std::pair<iterator,iterator> equal_range(const KeyType& key, KeyTypeKeyCompare comp);

   //! @copydoc ::lslboost::intrusive::bstree::equal_range(const key_type &)const
   std::pair<const_iterator, const_iterator>
      equal_range(const key_type &key) const;

   //! @copydoc ::lslboost::intrusive::bstree::equal_range(const KeyType&,KeyTypeKeyCompare)const
   template<class KeyType, class KeyTypeKeyCompare>
   std::pair<const_iterator, const_iterator>
      equal_range(const KeyType& key, KeyTypeKeyCompare comp) const;

   //! @copydoc ::lslboost::intrusive::bstree::bounded_range(const key_type &,const key_type &,bool,bool)
   std::pair<iterator,iterator> bounded_range
      (const key_type &lower, const key_type &upper_key, bool left_closed, bool right_closed);

   //! @copydoc ::lslboost::intrusive::bstree::bounded_range(const KeyType&,const KeyType&,KeyTypeKeyCompare,bool,bool)
   template<class KeyType, class KeyTypeKeyCompare>
   std::pair<iterator,iterator> bounded_range
      (const KeyType& lower_key, const KeyType& upper_key, KeyTypeKeyCompare comp, bool left_closed, bool right_closed);

   //! @copydoc ::lslboost::intrusive::bstree::bounded_range(const key_type &,const key_type &,bool,bool)const
   std::pair<const_iterator, const_iterator>
      bounded_range(const key_type &lower_key, const key_type &upper_key, bool left_closed, bool right_closed) const;

   //! @copydoc ::lslboost::intrusive::bstree::bounded_range(const KeyType&,const KeyType&,KeyTypeKeyCompare,bool,bool)const
   template<class KeyType, class KeyTypeKeyCompare>
   std::pair<const_iterator, const_iterator> bounded_range
         (const KeyType& lower_key, const KeyType& upper_key, KeyTypeKeyCompare comp, bool left_closed, bool right_closed) const;

   //! @copydoc ::lslboost::intrusive::bstree::s_iterator_to(reference)
   static iterator s_iterator_to(reference value);

   //! @copydoc ::lslboost::intrusive::bstree::s_iterator_to(const_reference)
   static const_iterator s_iterator_to(const_reference value);

   //! @copydoc ::lslboost::intrusive::bstree::iterator_to(reference)
   iterator iterator_to(reference value);

   //! @copydoc ::lslboost::intrusive::bstree::iterator_to(const_reference)const
   const_iterator iterator_to(const_reference value) const;

   //! @copydoc ::lslboost::intrusive::bstree::init_node(reference)
   static void init_node(reference value);

   //! @copydoc ::lslboost::intrusive::bstree::unlink_leftmost_without_rebalance
   pointer unlink_leftmost_without_rebalance();

   //! @copydoc ::lslboost::intrusive::bstree::replace_node
   void replace_node(iterator replace_this, reference with_this);

   //! @copydoc ::lslboost::intrusive::bstree::remove_node
   void remove_node(reference value);

   friend bool operator< (const avltree_impl &x, const avltree_impl &y);

   friend bool operator==(const avltree_impl &x, const avltree_impl &y);

   friend bool operator!= (const avltree_impl &x, const avltree_impl &y);

   friend bool operator>(const avltree_impl &x, const avltree_impl &y);

   friend bool operator<=(const avltree_impl &x, const avltree_impl &y);

   friend bool operator>=(const avltree_impl &x, const avltree_impl &y);

   friend void swap(avltree_impl &x, avltree_impl &y);
   #endif   //#ifdef BOOST_INTRUSIVE_DOXYGEN_INVOKED
};


//! Helper metafunction to define a \c avltree that yields to the same type when the
//! same options (either explicitly or implicitly) are used.
#if defined(BOOST_INTRUSIVE_DOXYGEN_INVOKED) || defined(BOOST_INTRUSIVE_VARIADIC_TEMPLATES)
template<class T, class ...Options>
#else
template<class T, class O1 = void, class O2 = void
                , class O3 = void, class O4 = void
                , class O5 = void, class O6 = void>
#endif
struct make_avltree
{
   /// @cond
   typedef typename pack_options
      < avltree_defaults,
      #if !defined(BOOST_INTRUSIVE_VARIADIC_TEMPLATES)
      O1, O2, O3, O4, O5, O6
      #else
      Options...
      #endif
      >::type packed_options;

   typedef typename detail::get_value_traits
      <T, typename packed_options::proto_value_traits>::type value_traits;

   typedef avltree_impl
         < value_traits
         , typename packed_options::key_of_value
         , typename packed_options::compare
         , typename packed_options::size_type
         , packed_options::constant_time_size
         , typename packed_options::header_holder_type
         > implementation_defined;
   /// @endcond
   typedef implementation_defined type;
};


#ifndef BOOST_INTRUSIVE_DOXYGEN_INVOKED

#if !defined(BOOST_INTRUSIVE_VARIADIC_TEMPLATES)
template<class T, class O1, class O2, class O3, class O4, class O5, class O6>
#else
template<class T, class ...Options>
#endif
class avltree
   :  public make_avltree<T,
      #if !defined(BOOST_INTRUSIVE_VARIADIC_TEMPLATES)
      O1, O2, O3, O4, O5, O6
      #else
      Options...
      #endif
      >::type
{
   typedef typename make_avltree
      <T,
      #if !defined(BOOST_INTRUSIVE_VARIADIC_TEMPLATES)
      O1, O2, O3, O4, O5, O6
      #else
      Options...
      #endif
      >::type   Base;
   BOOST_MOVABLE_BUT_NOT_COPYABLE(avltree)

   public:
   typedef typename Base::key_compare        key_compare;
   typedef typename Base::value_traits       value_traits;
   typedef typename Base::iterator           iterator;
   typedef typename Base::const_iterator     const_iterator;
   typedef typename Base::reverse_iterator           reverse_iterator;
   typedef typename Base::const_reverse_iterator     const_reverse_iterator;

   //Assert if passed value traits are compatible with the type
   BOOST_STATIC_ASSERT((detail::is_same<typename value_traits::value_type, T>::value));

   explicit avltree( const key_compare &cmp = key_compare()
                   , const value_traits &v_traits = value_traits())
      :  Base(cmp, v_traits)
   {}

   template<class Iterator>
   avltree( bool unique, Iterator b, Iterator e
         , const key_compare &cmp = key_compare()
         , const value_traits &v_traits = value_traits())
      :  Base(unique, b, e, cmp, v_traits)
   {}

   avltree(BOOST_RV_REF(avltree) x)
      :  Base(BOOST_MOVE_BASE(Base, x))
   {}

   avltree& operator=(BOOST_RV_REF(avltree) x)
   {  return static_cast<avltree &>(this->Base::operator=(BOOST_MOVE_BASE(Base, x)));  }

   template <class Cloner, class Disposer>
   void clone_from(const avltree &src, Cloner cloner, Disposer disposer)
   {  Base::clone_from(src, cloner, disposer);  }

   template <class Cloner, class Disposer>
   void clone_from(BOOST_RV_REF(avltree) src, Cloner cloner, Disposer disposer)
   {  Base::clone_from(BOOST_MOVE_BASE(Base, src), cloner, disposer);  }

   static avltree &container_from_end_iterator(iterator end_iterator)
   {  return static_cast<avltree &>(Base::container_from_end_iterator(end_iterator));   }

   static const avltree &container_from_end_iterator(const_iterator end_iterator)
   {  return static_cast<const avltree &>(Base::container_from_end_iterator(end_iterator));   }

   static avltree &container_from_iterator(iterator it)
   {  return static_cast<avltree &>(Base::container_from_iterator(it));   }

   static const avltree &container_from_iterator(const_iterator it)
   {  return static_cast<const avltree &>(Base::container_from_iterator(it));   }
};

#endif

} //namespace intrusive
} //namespace lslboost

#include <lslboost/intrusive/detail/config_end.hpp>

#endif //BOOST_INTRUSIVE_AVLTREE_HPP
