//////////////////////////////////////////////////////////////////////////////
//
// (C) Copyright Ion Gaztanaga 2007-2011. Distributed under the Boost
// Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at http://www.lslboost.org/LICENSE_1_0.txt)
//
// See http://www.lslboost.org/libs/interprocess for documentation.
//
//////////////////////////////////////////////////////////////////////////////

#ifndef BOOST_INTERPROCESS_DETAIL_INTERPROCESS_TESTER_HPP
#define BOOST_INTERPROCESS_DETAIL_INTERPROCESS_TESTER_HPP

namespace lslboost{
namespace interprocess{
namespace ipcdetail{

class interprocess_tester
{
   public:
   template<class T>
   static void dont_close_on_destruction(T &t)
   {  t.dont_close_on_destruction(); }
};

}  //namespace ipcdetail{
}  //namespace interprocess{
}  //namespace lslboost{

#endif   //#ifndef BOOST_INTERPROCESS_DETAIL_INTERPROCESS_TESTER_HPP

