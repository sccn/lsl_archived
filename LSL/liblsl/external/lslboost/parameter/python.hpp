// Copyright Daniel Wallin 2006. Use, modification and distribution is
// subject to the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.lslboost.org/LICENSE_1_0.txt)

#ifndef BOOST_PARAMETER_PYTHON_060209_HPP
# define BOOST_PARAMETER_PYTHON_060209_HPP

# include <lslboost/mpl/vector.hpp>
# include <lslboost/mpl/fold.hpp>
# include <lslboost/mpl/prior.hpp>
# include <lslboost/mpl/shift_right.hpp>
# include <lslboost/mpl/shift_left.hpp>
# include <lslboost/mpl/bitand.hpp>
# include <lslboost/mpl/pair.hpp>
# include <lslboost/mpl/size.hpp>
# include <lslboost/mpl/push_back.hpp>
# include <lslboost/mpl/or.hpp>
# include <lslboost/mpl/count_if.hpp>
# include <lslboost/mpl/transform.hpp>
# include <lslboost/mpl/front.hpp>
# include <lslboost/mpl/iterator_range.hpp>
# include <lslboost/mpl/next.hpp>
# include <lslboost/mpl/begin_end.hpp>
# include <lslboost/mpl/not.hpp>
# include <lslboost/mpl/empty.hpp>
# include <lslboost/python/def.hpp>
# include <lslboost/python/make_constructor.hpp>
# include <lslboost/python/init.hpp>
# include <lslboost/python/to_python_converter.hpp>
# include <lslboost/parameter/aux_/maybe.hpp>
# include <lslboost/parameter/aux_/python/invoker.hpp>

namespace lslboost { namespace parameter { namespace python 
{
  namespace python_ = lslboost::python;
}}}

namespace lslboost { namespace parameter { namespace python { namespace aux 
{

  inline PyObject* unspecified_type()
  {
      static PyTypeObject unspecified = {
          PyObject_HEAD_INIT(NULL)
          0,                                /* ob_size        */
          "Boost.Parameter.Unspecified",    /* tp_name        */
          PyType_Type.tp_basicsize,         /* tp_basicsize   */
          0,                                /* tp_itemsize    */
          0,                                /* tp_dealloc     */
          0,                                /* tp_print       */
          0,                                /* tp_getattr     */
          0,                                /* tp_setattr     */
          0,                                /* tp_compare     */
          0,                                /* tp_repr        */
          0,                                /* tp_as_number   */
          0,                                /* tp_as_sequence */
          0,                                /* tp_as_mapping  */
          0,                                /* tp_hash        */
          0,                                /* tp_call        */
          0,                                /* tp_str         */
          0,                                /* tp_getattro    */
          0,                                /* tp_setattro    */
          0,                                /* tp_as_buffer   */
          Py_TPFLAGS_DEFAULT,               /* tp_flags       */
          0,                                /* tp_doc         */
      };

      if (unspecified.ob_type == 0)
      {
          unspecified.ob_type = &PyType_Type;
          PyType_Ready(&unspecified);
      }

      return (PyObject*)&unspecified;
  }

  struct empty_tag {};

  struct empty_tag_to_python
  {
      static PyObject* convert(empty_tag)
      {
          return python_::xincref(unspecified_type());
      }
  };

}}}} // namespace lslboost::parameter::python::aux

namespace lslboost { namespace python 
{

  // Converts a Python value to a maybe<T>
  template <class T>
  struct arg_from_python<parameter::aux::maybe<T> >
    : arg_from_python<T>
  {
      arg_from_python(PyObject* p)
        : arg_from_python<T>(p)
        , empty(parameter::python::aux::unspecified_type() == p)
      {}

      bool convertible() const
      {
          return empty || arg_from_python<T>::convertible();
      }

      parameter::aux::maybe<T> operator()()
      {
          if (empty)
          {
              return parameter::aux::maybe<T>();
          }
          else
          {
              return parameter::aux::maybe<T>(
                  arg_from_python<T>::operator()()
              );
          }
      }

      bool empty;
  };

}} // namespace lslboost::python

namespace lslboost { namespace parameter { namespace python {

namespace aux
{

  template <class K>
  struct is_optional
    : mpl::not_<
          mpl::or_<typename K::required, typename K::optimized_default>
      >
  {};

  template <class K, class Required, class Optimized, class T>
  struct arg_spec
  {
      typedef K keyword;
      typedef Required required;
      typedef T type;
      typedef Optimized optimized_default;
  };
  
  template <class K, class T, class Optimized = mpl::false_>
  struct make_arg_spec_impl
  {
      typedef arg_spec<
          typename K::first, typename K::second, Optimized, T
      > type;
  };

  template <class K, class T>
  struct make_arg_spec_impl<K, T, typename K::third>
  {
      typedef arg_spec<
          typename K::first, typename K::second, typename K::third, T
      > type;
  };

  template <class K, class T>
  struct make_arg_spec
    : make_arg_spec_impl<K, T>
  {
  };

  template <class Spec, class State>
  struct combinations_op
  {
      typedef typename State::second bits;
      typedef typename State::first result0;

      typedef typename mpl::if_<
          mpl::or_<
              typename Spec::required
            , typename Spec::optimized_default
            , mpl::bitand_<bits, mpl::long_<1> >
          >
        , typename mpl::push_back<result0, Spec>::type
        , result0
      >::type result;

      typedef typename mpl::if_<
          mpl::or_<
              typename Spec::required
            , typename Spec::optimized_default
          >
        , bits
        , typename mpl::shift_right<bits, mpl::long_<1> >::type
      >::type next_bits;

      typedef mpl::pair<
          result
        , next_bits
      > type;
  };

  // Used as start value in the recursive arg() composition below.
  struct no_keywords
  {
      template <class T>
      T const& operator,(T const& x) const
      {
          return x;
      }
  };

  template <class Def, class F, class Iter, class End, class Keywords>
  void def_combination_aux0(
      Def def, F f, Iter, End, Keywords const& keywords, mpl::false_)
  {
      typedef typename mpl::deref<Iter>::type spec;
      typedef typename spec::keyword kw;

      def_combination_aux(
          def, f, typename mpl::next<Iter>::type(), End()
        , (
              keywords, lslboost::python::arg(kw::keyword_name())
          )
      );
  }

  template <class Def, class F, class Iter, class End, class Keywords>
  void def_combination_aux0(
      Def def, F f, Iter, End, Keywords const& keywords, mpl::true_)
  {
      typedef typename mpl::deref<Iter>::type spec;
      typedef typename spec::keyword kw;

      def_combination_aux(
          def, f, typename mpl::next<Iter>::type(), End()
        , (
              keywords, lslboost::python::arg(kw::keyword_name()) = empty_tag()
          )
      );
  }

  inline void initialize_converter()
  {
      static python_::to_python_converter<empty_tag, empty_tag_to_python> x;
  }

  template <class Def, class F, class Iter, class End, class Keywords>
  void def_combination_aux(
      Def def, F f, Iter, End, Keywords const& keywords)
  {
      typedef typename mpl::deref<Iter>::type spec;

      typedef typename mpl::and_<
          typename spec::optimized_default
        , mpl::not_<typename spec::required>
      >::type optimized_default;
      
      def_combination_aux0(
          def, f, Iter(), End(), keywords, optimized_default()
      );
  }

  template <class Def, class F, class End, class Keywords>
  void def_combination_aux(
      Def def, F f, End, End, Keywords const& keywords)
  {
      def(f, keywords);
  } 

  template <class Def, class F, class End>
  void def_combination_aux(
      Def def, F f, End, End, no_keywords const&)
  {
      def(f);
  }

  template <
      class Def, class Specs, class Bits, class Invoker
  >
  void def_combination(
      Def def, Specs*, Bits, Invoker*)
  {
      typedef typename mpl::fold<
          Specs
        , mpl::pair<mpl::vector0<>, Bits>
        , combinations_op<mpl::_2, mpl::_1>
      >::type combination0;

      typedef typename combination0::first combination;

      typedef typename mpl::apply_wrap1<
          Invoker, combination
      >::type invoker;

      def_combination_aux(
          def
        , &invoker::execute
        , typename mpl::begin<combination>::type()
        , typename mpl::end<combination>::type()
        , no_keywords()
      );
  }

  template <
      class Def, class Specs, class Bits, class End, class Invoker
  >
  void def_combinations(
      Def def, Specs*, Bits, End, Invoker*)
  {
      initialize_converter();

      def_combination(def, (Specs*)0, Bits(), (Invoker*)0);

      def_combinations(
          def
        , (Specs*)0
        , mpl::long_<Bits::value + 1>()
        , End()
        , (Invoker*)0
      );
  }

  template <
      class Def, class Specs, class End, class Invoker
  >
  void def_combinations(
      Def, Specs*, End, End, Invoker*)
  {}

  struct not_specified {};

  template <class CallPolicies>
  struct call_policies_as_options
  {
      call_policies_as_options(CallPolicies const& call_policies)
        : call_policies(call_policies)
      {}

      CallPolicies const& policies() const
      {
          return call_policies;
      }

      char const* doc() const
      {
          return 0;
      }

      CallPolicies call_policies;
  };

  template <class Class, class Options = not_specified>
  struct def_class
  {
      def_class(Class& cl, char const* name, Options options = Options())
        : cl(cl)
        , name(name)
        , options(options)
      {}

      template <class F>
      void def(F f, not_specified const*) const
      {
          cl.def(name, f);
      }

      template <class F>
      void def(F f, void const*) const
      {
          cl.def(name, f, options.doc(), options.policies());
      }
      
      template <class F>
      void operator()(F f) const
      {
          this->def(f, &options);
      }

      template <class F, class Keywords>
      void def(F f, Keywords const& keywords, not_specified const*) const
      {
          cl.def(name, f, keywords);
      }

      template <class F, class Keywords>
      void def(F f, Keywords const& keywords, void const*) const
      {
          cl.def(name, f, keywords, options.doc(), options.policies());
      }

      template <class F, class Keywords>
      void operator()(F f, Keywords const& keywords) const
      {
          this->def(f, keywords, &options);
      }

      Class& cl;
      char const* name;
      Options options;
  };

  template <class Class, class CallPolicies = lslboost::python::default_call_policies>
  struct def_init
  {
      def_init(Class& cl, CallPolicies call_policies = CallPolicies())
        : cl(cl)
        , call_policies(call_policies)
      {}

      template <class F>
      void operator()(F f) const
      {
          cl.def(
              "__init__"
            , lslboost::python::make_constructor(f, call_policies)
          );
      }

      template <class F, class Keywords>
      void operator()(F f, Keywords const& keywords) const
      {
          cl.def(
              "__init__"
            , lslboost::python::make_constructor(f, call_policies, keywords)
          );
      }

      Class& cl;
      CallPolicies call_policies;
  };

  struct def_function
  {
      def_function(char const* name)
        : name(name)
      {}
      
      template <class F>
      void operator()(F f) const
      {
          lslboost::python::def(name, f);
      }

      template <class F, class Keywords>
      void operator()(F f, Keywords const& keywords) const
      {
          lslboost::python::def(name, f, keywords);
      }

      char const* name;
  };

} // namespace aux

template <class M, class Signature>
void def(char const* name, Signature)
{
    typedef mpl::iterator_range<
        typename mpl::next<
            typename mpl::begin<Signature>::type
        >::type
      , typename mpl::end<Signature>::type
    > arg_types;

    typedef typename mpl::transform<
        typename M::keywords
      , arg_types
      , aux::make_arg_spec<mpl::_1, mpl::_2>
      , mpl::back_inserter<mpl::vector0<> >
    >::type arg_specs;

    typedef typename mpl::count_if<
        arg_specs
      , aux::is_optional<mpl::_1>
    >::type optional_arity;
    
    typedef typename mpl::front<Signature>::type result_type;
    typedef typename mpl::shift_left<mpl::long_<1>, optional_arity>::type upper;

    aux::def_combinations(
        aux::def_function(name)
      , (arg_specs*)0
      , mpl::long_<0>()
      , mpl::long_<upper::value>()
      , (aux::make_invoker<M, result_type>*)0
    );
}

template <class M, class Class, class Signature>
void def(Class& cl, char const* name, Signature)
{
    typedef mpl::iterator_range<
        typename mpl::next<
            typename mpl::begin<Signature>::type
        >::type
      , typename mpl::end<Signature>::type
    > arg_types;

    typedef typename mpl::transform<
        typename M::keywords
      , arg_types
      , aux::make_arg_spec<mpl::_1, mpl::_2>
      , mpl::back_inserter<mpl::vector0<> >
    >::type arg_specs;

    typedef typename mpl::count_if<
        arg_specs
      , aux::is_optional<mpl::_1>
    >::type optional_arity;
    
    typedef typename mpl::front<Signature>::type result_type;
    typedef typename mpl::shift_left<mpl::long_<1>, optional_arity>::type upper;

    aux::def_combinations(
        aux::def_class<Class>(cl, name)
      , (arg_specs*)0
      , mpl::long_<0>()
      , mpl::long_<upper::value>()
      , (aux::make_invoker<M, result_type>*)0
    );
}

namespace aux
{

  template <class K>
  struct keyword
  {
      typedef K type;
  };

  template <class K>
  struct keyword<K*>
  {
      typedef K type;
  };

  template <class K>
  struct keyword<K**>
  {
      typedef K type;
  };

  template <class K>
  struct required
  {
      typedef mpl::true_ type;
  };

  template <class K>
  struct required<K*>
  {
      typedef mpl::false_ type;
  };

  template <class K>
  struct optimized
  {
      typedef mpl::true_ type;
  };

  template <class K>
  struct optimized<K**>
  {
      typedef mpl::false_ type;
  };

  template <class T>
  struct make_kw_spec;

  template <class K, class T>
  struct make_kw_spec<K(T)>
  {
      typedef arg_spec<
          typename keyword<K>::type
        , typename required<K>::type
        , typename optimized<K>::type
        , T
      > type;
  };

} // namespace aux

template <class ParameterSpecs, class CallPolicies = lslboost::python::default_call_policies>
struct init 
  : lslboost::python::def_visitor<init<ParameterSpecs, CallPolicies> >
{
    init(CallPolicies call_policies = CallPolicies())
      : call_policies(call_policies)
    {}

    template <class CallPolicies1>
    init<ParameterSpecs, CallPolicies1> 
    operator[](CallPolicies1 const& call_policies) const
    {
        return init<ParameterSpecs, CallPolicies1>(call_policies);
    }

    template <class Class>
    void visit_aux(Class& cl, mpl::true_) const
    {
        cl.def(lslboost::python::init<>()[call_policies]);
    }

    template <class Class>
    void visit_aux(Class& cl, mpl::false_) const
    {
        typedef typename mpl::transform<
            ParameterSpecs
          , aux::make_kw_spec<mpl::_>
          , mpl::back_inserter<mpl::vector0<> >
        >::type arg_specs;

        typedef typename mpl::count_if<
            arg_specs
          , aux::is_optional<mpl::_>
        >::type optional_arity;

        typedef typename mpl::shift_left<mpl::long_<1>, optional_arity>::type upper;

        aux::def_combinations(
            aux::def_init<Class, CallPolicies>(cl, call_policies)
          , (arg_specs*)0
          , mpl::long_<0>()
          , mpl::long_<upper::value>()
          , (aux::make_init_invoker<typename Class::wrapped_type>*)0
        );
    }

    template <class Class>
    void visit(Class& cl) const
    {
        visit_aux(cl, mpl::empty<ParameterSpecs>());
    }

    CallPolicies call_policies;
};

template <class ParameterSpecs, class CallPolicies = lslboost::python::default_call_policies>
struct call 
  : lslboost::python::def_visitor<call<ParameterSpecs, CallPolicies> >
{
    call(CallPolicies const& call_policies = CallPolicies())
      : call_policies(call_policies)
    {}

    template <class CallPolicies1>
    call<ParameterSpecs, CallPolicies1>
    operator[](CallPolicies1 const& call_policies) const
    {
        return call<ParameterSpecs, CallPolicies1>(call_policies);
    }

    template <class Class>
    void visit(Class& cl) const
    {
        typedef mpl::iterator_range<
            typename mpl::next<
                typename mpl::begin<ParameterSpecs>::type
            >::type
          , typename mpl::end<ParameterSpecs>::type
        > arg_types;

        typedef typename mpl::front<ParameterSpecs>::type result_type;

        typedef typename mpl::transform<
            arg_types
          , aux::make_kw_spec<mpl::_>
          , mpl::back_inserter<mpl::vector0<> >
        >::type arg_specs;

        typedef typename mpl::count_if<
            arg_specs
          , aux::is_optional<mpl::_>
        >::type optional_arity;

        typedef typename mpl::shift_left<mpl::long_<1>, optional_arity>::type upper;

        typedef aux::call_policies_as_options<CallPolicies> options;

        aux::def_combinations(
            aux::def_class<Class, options>(cl, "__call__", options(call_policies))
          , (arg_specs*)0
          , mpl::long_<0>()
          , mpl::long_<upper::value>()
          , (aux::make_call_invoker<typename Class::wrapped_type, result_type>*)0
        );
    }

    CallPolicies call_policies;
};

template <class Fwd, class ParameterSpecs>
struct function 
  : lslboost::python::def_visitor<function<Fwd, ParameterSpecs> >
{
    template <class Class, class Options>
    void visit(Class& cl, char const* name, Options const& options) const
    {
        typedef mpl::iterator_range<
            typename mpl::next<
                typename mpl::begin<ParameterSpecs>::type
            >::type
          , typename mpl::end<ParameterSpecs>::type
        > arg_types;

        typedef typename mpl::front<ParameterSpecs>::type result_type;

        typedef typename mpl::transform<
            arg_types
          , aux::make_kw_spec<mpl::_>
          , mpl::back_inserter<mpl::vector0<> >
        >::type arg_specs;

        typedef typename mpl::count_if<
            arg_specs
          , aux::is_optional<mpl::_>
        >::type optional_arity;

        typedef typename mpl::shift_left<mpl::long_<1>, optional_arity>::type upper;

        aux::def_combinations(
            aux::def_class<Class, Options>(cl, name, options)
          , (arg_specs*)0
          , mpl::long_<0>()
          , mpl::long_<upper::value>()
          , (aux::make_member_invoker<
                Fwd, result_type, typename Class::wrapped_type
            >*)0
        );
    }
};

}}} // namespace lslboost::parameter::python

#endif // BOOST_PARAMETER_PYTHON_060209_HPP

