//////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2018-2020, Lawrence Livermore National Security, LLC.
//
// Produced at the Lawrence Livermore National Laboratory
//
// LLNL-CODE-758885
//
// All rights reserved.
//
// This file is part of Comb.
//
// For details, see https://github.com/LLNL/Comb
// Please also see the LICENSE file for MIT license.
//////////////////////////////////////////////////////////////////////////////

#ifndef _FOR_ALL_HPP
#define _FOR_ALL_HPP

#include "config.hpp"

#include <cstdio>
#include <cstdlib>
#include <cassert>

#include <type_traits>

#include "exec_utils.hpp"
#include "memory.hpp"
#include "ExecContext.hpp"


inline bool& comb_allow_per_message_pack_fusing()
{
  static bool allow = true;
  return allow;
}

inline bool& comb_allow_pack_loop_fusion()
{
  static bool allow = true;
  return allow;
}

namespace detail {

template < typename body_type >
struct adapter_2d {
  IdxT begin0, begin1;
  IdxT len1;
  body_type body;
  template < typename body_type_ >
  adapter_2d(IdxT begin0_, IdxT end0_, IdxT begin1_, IdxT end1_, body_type_&& body_)
    : begin0(begin0_)
    , begin1(begin1_)
    , len1(end1_ - begin1_)
    , body(std::forward<body_type_>(body_))
  { COMB::ignore_unused(end0_); }
  COMB_HOST COMB_DEVICE
  void operator() (IdxT, IdxT idx) const
  {
    IdxT i0 = idx / len1;
    IdxT i1 = idx - i0 * len1;

    //FGPRINTF(FileGroup::proc, "adapter_2d (%i+%i %i+%i)%i\n", i0, begin0, i1, begin1, idx);
    //assert(0 <= i0 + begin0 && i0 + begin0 < 3);
    //assert(0 <= i1 + begin1 && i1 + begin1 < 3);

    body(i0 + begin0, i1 + begin1, idx);
  }
};

template < typename body_type >
struct adapter_3d {
  IdxT begin0, begin1, begin2;
  IdxT len2, len12;
  body_type body;
  template < typename body_type_ >
  adapter_3d(IdxT begin0_, IdxT end0_, IdxT begin1_, IdxT end1_, IdxT begin2_, IdxT end2_, body_type_&& body_)
    : begin0(begin0_)
    , begin1(begin1_)
    , begin2(begin2_)
    , len2(end2_ - begin2_)
    , len12((end1_ - begin1_) * (end2_ - begin2_))
    , body(std::forward<body_type_>(body_))
  { COMB::ignore_unused(end0_); }
  COMB_HOST COMB_DEVICE
  void operator() (IdxT, IdxT idx) const
  {
    IdxT i0 = idx / len12;
    IdxT idx12 = idx - i0 * len12;

    IdxT i1 = idx12 / len2;
    IdxT i2 = idx12 - i1 * len2;

    //FGPRINTF(FileGroup::proc, "adapter_3d (%i+%i %i+%i %i+%i)%i\n", i0, begin0, i1, begin1, i2, begin2, idx);
    //assert(0 <= i0 + begin0 && i0 + begin0 < 3);
    //assert(0 <= i1 + begin1 && i1 + begin1 < 3);
    //assert(0 <= i2 + begin2 && i2 + begin2 < 13);

    body(i0 + begin0, i1 + begin1, i2 + begin2, idx);
  }
};

} // namespace detail

#include "exec_pol_seq.hpp"
#include "exec_pol_omp.hpp"
#include "exec_pol_cuda.hpp"
#include "exec_pol_cuda_graph.hpp"
#include "exec_pol_mpi_type.hpp"

namespace COMB {

template < typename my_context_type >
struct ContextHolder
{
  using context_type = my_context_type;

  bool m_available = false;

  bool available() const
  {
    return m_available;
  }

  template < typename ... Ts >
  void create(Ts&&... args)
  {
    destroy();
    m_context = new context_type(std::forward<Ts>(args)...);
  }

  context_type& get()
  {
    assert(m_context != nullptr);
    return *m_context;
  }

  void destroy()
  {
    if (m_context) {
      delete m_context;
      m_context = nullptr;
    }
  }

  ~ContextHolder()
  {
    destroy();
  }

private:
  context_type* m_context = nullptr;
};

struct Executors
{
  Executors()
  { }

  Executors(Executors const&) = delete;
  Executors(Executors &&) = delete;
  Executors& operator=(Executors const&) = delete;
  Executors& operator=(Executors &&) = delete;

  void create_executors(Allocators& alocs)
  {
    base_cpu.create();
#ifdef COMB_ENABLE_MPI
    base_mpi.create();
#endif
#ifdef COMB_ENABLE_CUDA
    base_cuda.create();
#endif

    seq.create(base_cpu.get(), alocs.host.allocator());
#ifdef COMB_ENABLE_OPENMP
    omp.create(base_cpu.get(), alocs.host.allocator());
#endif
#ifdef COMB_ENABLE_CUDA
    cuda.create(base_cuda.get(), (alocs.access.use_device_preferred_for_cuda_util_aloc) ? alocs.cuda_managed_device_preferred_host_accessed.allocator() : alocs.cuda_hostpinned.allocator());
#endif
#ifdef COMB_ENABLE_CUDA_GRAPH
    cuda_graph.create(base_cuda.get(), (alocs.access.use_device_preferred_for_cuda_util_aloc) ? alocs.cuda_managed_device_preferred_host_accessed.allocator() : alocs.cuda_hostpinned.allocator());
#endif
#ifdef COMB_ENABLE_MPI
    mpi_type.create(base_mpi.get(), alocs.host.allocator());
#endif
  }

  ContextHolder<CPUContext> base_cpu;
#ifdef COMB_ENABLE_MPI
  ContextHolder<MPIContext> base_mpi;
#endif
#ifdef COMB_ENABLE_CUDA
  ContextHolder<CudaContext> base_cuda;
#endif

  ContextHolder<ExecContext<seq_pol>> seq;
#ifdef COMB_ENABLE_OPENMP
  ContextHolder<ExecContext<omp_pol>> omp;
#else
  ContextHolder<void> omp;
#endif
#ifdef COMB_ENABLE_CUDA
  ContextHolder<ExecContext<cuda_pol>> cuda;
#ifdef COMB_ENABLE_CUDA_GRAPH
  ContextHolder<ExecContext<cuda_graph_pol>> cuda_graph;
#endif
#endif
#ifdef COMB_ENABLE_MPI
  ContextHolder<ExecContext<mpi_type_pol>> mpi_type;
#endif
};

} // namespace COMB

#endif // _FOR_ALL_HPP