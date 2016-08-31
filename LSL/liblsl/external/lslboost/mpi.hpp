// Copyright (C) 2006 Douglas Gregor <doug.gregor -at- gmail.com>.

// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.lslboost.org/LICENSE_1_0.txt)

// Message Passing Interface

//  See www.lslboost.org/libs/mpi for documentation.

/** @file mpi.hpp
 *
 *  This file is a top-level convenience header that includes all of
 *  the Boost.MPI library headers. Users concerned about compile time
 *  may wish to include only specific headers from the Boost.MPI
 *  library.
 *
 */
#ifndef BOOST_MPI_HPP
#define BOOST_MPI_HPP

#include <lslboost/mpi/allocator.hpp>
#include <lslboost/mpi/collectives.hpp>
#include <lslboost/mpi/communicator.hpp>
#include <lslboost/mpi/datatype.hpp>
#include <lslboost/mpi/environment.hpp>
#include <lslboost/mpi/graph_communicator.hpp>
#include <lslboost/mpi/group.hpp>
#include <lslboost/mpi/intercommunicator.hpp>
#include <lslboost/mpi/nonblocking.hpp>
#include <lslboost/mpi/operations.hpp>
#include <lslboost/mpi/skeleton_and_content.hpp>
#include <lslboost/mpi/timer.hpp>

#endif // BOOST_MPI_HPP
