//////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2018-2019, Lawrence Livermore National Security, LLC.
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

#include "comb.hpp"

#ifdef COMB_ENABLE_MP

#include "comm_pol_mp.hpp"
#include "do_cycles.hpp"

namespace COMB {

void test_cycles_mp(CommInfo& comminfo, MeshInfo& info,
                       COMB::ExecContexts& exec,
                       COMB::Allocators& alloc,
                       COMB::ExecutorsAvailable& exec_avail,
                       IdxT num_vars, IdxT ncycles, Timer& tm, Timer& tm_total)
{
  CommContext<mp_pol> con_comm;

  // host allocated
  if (alloc.host.available()) {
    AllocatorInfo& mesh_aloc = alloc.host;

    char name[1024] = ""; snprintf(name, 1024, "Mesh %s", mesh_aloc.allocator().name());
    Range r0(name, Range::blue);

    if (alloc.access.cuda_device_accessible_from_host) {
      if (exec_avail.seq && exec_avail.seq && exec_avail.seq)
        do_cycles(con_comm, comminfo, info, num_vars, ncycles, exec.seq, mesh_aloc.allocator(), exec.seq, alloc.cuda_device.allocator(), exec.seq, alloc.cuda_device.allocator(), tm, tm_total);

#ifdef COMB_ENABLE_OPENMP
      if (exec_avail.omp && exec_avail.seq && exec_avail.seq)
        do_cycles(con_comm, comminfo, info, num_vars, ncycles, exec.omp, mesh_aloc.allocator(), exec.seq, alloc.cuda_device.allocator(), exec.seq, alloc.cuda_device.allocator(), tm, tm_total);

      if (exec_avail.omp && exec_avail.omp && exec_avail.seq)
        do_cycles(con_comm, comminfo, info, num_vars, ncycles, exec.omp, mesh_aloc.allocator(), exec.omp, alloc.cuda_device.allocator(), exec.seq, alloc.cuda_device.allocator(), tm, tm_total);

      if (exec_avail.omp && exec_avail.omp && exec_avail.omp)
        do_cycles(con_comm, comminfo, info, num_vars, ncycles, exec.omp, mesh_aloc.allocator(), exec.omp, alloc.cuda_device.allocator(), exec.omp, alloc.cuda_device.allocator(), tm, tm_total);
#endif
    }

#ifdef COMB_ENABLE_CUDA
    if (alloc.access.cuda_host_accessible_from_device && alloc.access.cuda_device_accessible_from_host) {
      if (exec_avail.cuda && exec_avail.seq && exec_avail.seq)
        do_cycles(con_comm, comminfo, info, num_vars, ncycles, exec.cuda, mesh_aloc.allocator(), exec.seq, alloc.cuda_device.allocator(), exec.seq, alloc.cuda_device.allocator(), tm, tm_total);

#ifdef COMB_ENABLE_OPENMP
      if (exec_avail.cuda && exec_avail.omp && exec_avail.seq)
        do_cycles(con_comm, comminfo, info, num_vars, ncycles, exec.cuda, mesh_aloc.allocator(), exec.omp, alloc.cuda_device.allocator(), exec.seq, alloc.cuda_device.allocator(), tm, tm_total);

      if (exec_avail.cuda && exec_avail.omp && exec_avail.omp)
        do_cycles(con_comm, comminfo, info, num_vars, ncycles, exec.cuda, mesh_aloc.allocator(), exec.omp, alloc.cuda_device.allocator(), exec.omp, alloc.cuda_device.allocator(), tm, tm_total);
#endif

      if (exec_avail.cuda && exec_avail.cuda && exec_avail.seq)
        do_cycles(con_comm, comminfo, info, num_vars, ncycles, exec.cuda, mesh_aloc.allocator(), exec.cuda, alloc.cuda_device.allocator(), exec.seq, alloc.cuda_device.allocator(), tm, tm_total);
    }

    if (alloc.access.cuda_host_accessible_from_device) {

      if (exec_avail.cuda && exec_avail.cuda && exec_avail.cuda)
        do_cycles(con_comm, comminfo, info, num_vars, ncycles, exec.cuda, mesh_aloc.allocator(), exec.cuda, alloc.cuda_device.allocator(), exec.cuda, alloc.cuda_device.allocator(), tm, tm_total);

      {
        if (alloc.access.cuda_device_accessible_from_host) {
          if (exec_avail.cuda && exec_avail.cuda_batch && exec_avail.seq)
            do_cycles(con_comm, comminfo, info, num_vars, ncycles, exec.cuda, mesh_aloc.allocator(), exec.cuda_batch, alloc.cuda_device.allocator(), exec.seq, alloc.cuda_device.allocator(), tm, tm_total);
        }

        if (exec_avail.cuda && exec_avail.cuda_batch && exec_avail.cuda_batch)
          do_cycles(con_comm, comminfo, info, num_vars, ncycles, exec.cuda, mesh_aloc.allocator(), exec.cuda_batch, alloc.cuda_device.allocator(), exec.cuda_batch, alloc.cuda_device.allocator(), tm, tm_total);

        if (alloc.access.cuda_device_accessible_from_host) {
          if (exec_avail.cuda && exec_avail.cuda_persistent && exec_avail.seq)
            do_cycles(con_comm, comminfo, info, num_vars, ncycles, exec.cuda, mesh_aloc.allocator(), exec.cuda_persistent, alloc.cuda_device.allocator(), exec.seq, alloc.cuda_device.allocator(), tm, tm_total);
        }

        if (exec_avail.cuda && exec_avail.cuda_persistent && exec_avail.cuda_persistent)
          do_cycles(con_comm, comminfo, info, num_vars, ncycles, exec.cuda, mesh_aloc.allocator(), exec.cuda_persistent, alloc.cuda_device.allocator(), exec.cuda_persistent, alloc.cuda_device.allocator(), tm, tm_total);


        SetReset<bool> sr_gs(get_batch_always_grid_sync(), false);

        if (alloc.access.cuda_device_accessible_from_host) {
          if (exec_avail.cuda && exec_avail.cuda_batch_fewgs && exec_avail.seq)
            do_cycles(con_comm, comminfo, info, num_vars, ncycles, exec.cuda, mesh_aloc.allocator(), exec.cuda_batch, alloc.cuda_device.allocator(), exec.seq, alloc.cuda_device.allocator(), tm, tm_total);
        }

        if (exec_avail.cuda && exec_avail.cuda_batch_fewgs && exec_avail.cuda_batch_fewgs)
          do_cycles(con_comm, comminfo, info, num_vars, ncycles, exec.cuda, mesh_aloc.allocator(), exec.cuda_batch, alloc.cuda_device.allocator(), exec.cuda_batch, alloc.cuda_device.allocator(), tm, tm_total);
        if (alloc.access.cuda_device_accessible_from_host) {
          if (exec_avail.cuda && exec_avail.cuda_persistent_fewgs && exec_avail.seq)
            do_cycles(con_comm, comminfo, info, num_vars, ncycles, exec.cuda, mesh_aloc.allocator(), exec.cuda_persistent, alloc.cuda_device.allocator(), exec.seq, alloc.cuda_device.allocator(), tm, tm_total);
        }

        if (exec_avail.cuda && exec_avail.cuda_persistent_fewgs && exec_avail.cuda_persistent_fewgs)
          do_cycles(con_comm, comminfo, info, num_vars, ncycles, exec.cuda, mesh_aloc.allocator(), exec.cuda_persistent, alloc.cuda_device.allocator(), exec.cuda_persistent, alloc.cuda_device.allocator(), tm, tm_total);
      }

#ifdef COMB_ENABLE_CUDA_GRAPH
      if (alloc.access.cuda_device_accessible_from_host) {
        if (exec_avail.cuda && exec_avail.cuda_graph && exec_avail.seq)
          do_cycles(con_comm, comminfo, info, num_vars, ncycles, exec.cuda, mesh_aloc.allocator(), exec.cuda_graph, alloc.cuda_device.allocator(), exec.seq, alloc.cuda_device.allocator(), tm, tm_total);
      }

      if (exec_avail.cuda && exec_avail.cuda_graph && exec_avail.cuda_graph)
        do_cycles(con_comm, comminfo, info, num_vars, ncycles, exec.cuda, mesh_aloc.allocator(), exec.cuda_graph, alloc.cuda_device.allocator(), exec.cuda_graph, alloc.cuda_device.allocator(), tm, tm_total);
#endif
    }
#endif

    // if (exec_avail.seq && exec_avail.mpi_type && exec_avail.mpi_type)
    //   do_cycles(con_comm, comminfo, info, num_vars, ncycles, exec.seq, mesh_aloc.allocator(), exec.mpi_type, mesh_aloc.allocator(), exec.mpi_type, mesh_aloc.allocator(), tm, tm_total);

#ifdef COMB_ENABLE_OPENMP
    // if (exec_avail.omp && exec_avail.mpi_type && exec_avail.mpi_type)
    //   do_cycles(con_comm, comminfo, info, num_vars, ncycles, exec.omp, mesh_aloc.allocator(), exec.mpi_type, mesh_aloc.allocator(), exec.mpi_type, mesh_aloc.allocator(), tm, tm_total);
#endif

#ifdef COMB_ENABLE_CUDA
    if (alloc.access.cuda_host_accessible_from_device) {
      // if (exec_avail.cuda && exec_avail.mpi_type && exec_avail.mpi_type)
      //   do_cycles(con_comm, comminfo, info, num_vars, ncycles, exec.cuda, mesh_aloc.allocator(), exec.mpi_type, mesh_aloc.allocator(), exec.mpi_type, mesh_aloc.allocator(), tm, tm_total);
    }
#endif
  }

#ifdef COMB_ENABLE_CUDA
  // host pinned allocated
  if (alloc.cuda_hostpinned.available()) {
    AllocatorInfo& mesh_aloc = alloc.cuda_hostpinned;

    char name[1024] = ""; snprintf(name, 1024, "Mesh %s", mesh_aloc.allocator().name());
    Range r0(name, Range::blue);

    if (exec_avail.seq && exec_avail.seq && exec_avail.seq)
      do_cycles(con_comm, comminfo, info, num_vars, ncycles, exec.seq, mesh_aloc.allocator(), exec.seq, alloc.cuda_device.allocator(), exec.seq, alloc.cuda_device.allocator(), tm, tm_total);

#ifdef COMB_ENABLE_OPENMP
    if (exec_avail.omp && exec_avail.seq && exec_avail.seq)
      do_cycles(con_comm, comminfo, info, num_vars, ncycles, exec.omp, mesh_aloc.allocator(), exec.seq, alloc.cuda_device.allocator(), exec.seq, alloc.cuda_device.allocator(), tm, tm_total);

    if (exec_avail.omp && exec_avail.omp && exec_avail.seq)
      do_cycles(con_comm, comminfo, info, num_vars, ncycles, exec.omp, mesh_aloc.allocator(), exec.omp, alloc.cuda_device.allocator(), exec.seq, alloc.cuda_device.allocator(), tm, tm_total);

    if (exec_avail.omp && exec_avail.omp && exec_avail.omp)
      do_cycles(con_comm, comminfo, info, num_vars, ncycles, exec.omp, mesh_aloc.allocator(), exec.omp, alloc.cuda_device.allocator(), exec.omp, alloc.cuda_device.allocator(), tm, tm_total);
#endif

    if (exec_avail.cuda && exec_avail.seq && exec_avail.seq)
      do_cycles(con_comm, comminfo, info, num_vars, ncycles, exec.cuda, mesh_aloc.allocator(), exec.seq, alloc.cuda_device.allocator(), exec.seq, alloc.cuda_device.allocator(), tm, tm_total);

#ifdef COMB_ENABLE_OPENMP
    if (exec_avail.cuda && exec_avail.omp && exec_avail.seq)
      do_cycles(con_comm, comminfo, info, num_vars, ncycles, exec.cuda, mesh_aloc.allocator(), exec.omp, alloc.cuda_device.allocator(), exec.seq, alloc.cuda_device.allocator(), tm, tm_total);

    if (exec_avail.cuda && exec_avail.omp && exec_avail.omp)
      do_cycles(con_comm, comminfo, info, num_vars, ncycles, exec.cuda, mesh_aloc.allocator(), exec.omp, alloc.cuda_device.allocator(), exec.omp, alloc.cuda_device.allocator(), tm, tm_total);
#endif

    if (exec_avail.cuda && exec_avail.cuda && exec_avail.seq)
      do_cycles(con_comm, comminfo, info, num_vars, ncycles, exec.cuda, mesh_aloc.allocator(), exec.cuda, alloc.cuda_device.allocator(), exec.seq, alloc.cuda_device.allocator(), tm, tm_total);

    if (exec_avail.cuda && exec_avail.cuda && exec_avail.cuda)
      do_cycles(con_comm, comminfo, info, num_vars, ncycles, exec.cuda, mesh_aloc.allocator(), exec.cuda, alloc.cuda_device.allocator(), exec.cuda, alloc.cuda_device.allocator(), tm, tm_total);

    {
      if (exec_avail.cuda && exec_avail.cuda_batch && exec_avail.seq)
        do_cycles(con_comm, comminfo, info, num_vars, ncycles, exec.cuda, mesh_aloc.allocator(), exec.cuda_batch, alloc.cuda_device.allocator(), exec.seq, alloc.cuda_device.allocator(), tm, tm_total);

      if (exec_avail.cuda && exec_avail.cuda_batch && exec_avail.cuda_batch)
        do_cycles(con_comm, comminfo, info, num_vars, ncycles, exec.cuda, mesh_aloc.allocator(), exec.cuda_batch, alloc.cuda_device.allocator(), exec.cuda_batch, alloc.cuda_device.allocator(), tm, tm_total);

      if (exec_avail.cuda && exec_avail.cuda_persistent && exec_avail.seq)
        do_cycles(con_comm, comminfo, info, num_vars, ncycles, exec.cuda, mesh_aloc.allocator(), exec.cuda_persistent, alloc.cuda_device.allocator(), exec.seq, alloc.cuda_device.allocator(), tm, tm_total);

      if (exec_avail.cuda && exec_avail.cuda_persistent && exec_avail.cuda_persistent)
        do_cycles(con_comm, comminfo, info, num_vars, ncycles, exec.cuda, mesh_aloc.allocator(), exec.cuda_persistent, alloc.cuda_device.allocator(), exec.cuda_persistent, alloc.cuda_device.allocator(), tm, tm_total);


      SetReset<bool> sr_gs(get_batch_always_grid_sync(), false);

      if (exec_avail.cuda && exec_avail.cuda_batch_fewgs && exec_avail.seq)
        do_cycles(con_comm, comminfo, info, num_vars, ncycles, exec.cuda, mesh_aloc.allocator(), exec.cuda_batch, alloc.cuda_device.allocator(), exec.seq, alloc.cuda_device.allocator(), tm, tm_total);

      if (exec_avail.cuda && exec_avail.cuda_batch_fewgs && exec_avail.cuda_batch_fewgs)
        do_cycles(con_comm, comminfo, info, num_vars, ncycles, exec.cuda, mesh_aloc.allocator(), exec.cuda_batch, alloc.cuda_device.allocator(), exec.cuda_batch, alloc.cuda_device.allocator(), tm, tm_total);

      if (exec_avail.cuda && exec_avail.cuda_persistent_fewgs && exec_avail.seq)
        do_cycles(con_comm, comminfo, info, num_vars, ncycles, exec.cuda, mesh_aloc.allocator(), exec.cuda_persistent, alloc.cuda_device.allocator(), exec.seq, alloc.cuda_device.allocator(), tm, tm_total);

      if (exec_avail.cuda && exec_avail.cuda_persistent_fewgs && exec_avail.cuda_persistent_fewgs)
        do_cycles(con_comm, comminfo, info, num_vars, ncycles, exec.cuda, mesh_aloc.allocator(), exec.cuda_persistent, alloc.cuda_device.allocator(), exec.cuda_persistent, alloc.cuda_device.allocator(), tm, tm_total);
    }

#ifdef COMB_ENABLE_CUDA_GRAPH
    if (exec_avail.cuda && exec_avail.cuda_graph && exec_avail.seq)
      do_cycles(con_comm, comminfo, info, num_vars, ncycles, exec.cuda, mesh_aloc.allocator(), exec.cuda_graph, alloc.cuda_device.allocator(), exec.seq, alloc.cuda_device.allocator(), tm, tm_total);

    if (exec_avail.cuda && exec_avail.cuda_graph && exec_avail.cuda_graph)
      do_cycles(con_comm, comminfo, info, num_vars, ncycles, exec.cuda, mesh_aloc.allocator(), exec.cuda_graph, alloc.cuda_device.allocator(), exec.cuda_graph, alloc.cuda_device.allocator(), tm, tm_total);
#endif

    // if (exec_avail.seq && exec_avail.mpi_type && exec_avail.mpi_type)
    //   do_cycles(con_comm, comminfo, info, num_vars, ncycles, exec.seq, mesh_aloc.allocator(), exec.mpi_type, mesh_aloc.allocator(), exec.mpi_type, mesh_aloc.allocator(), tm, tm_total);

#ifdef COMB_ENABLE_OPENMP
    // if (exec_avail.omp && exec_avail.mpi_type && exec_avail.mpi_type)
    //   do_cycles(con_comm, comminfo, info, num_vars, ncycles, exec.omp, mesh_aloc.allocator(), exec.mpi_type, mesh_aloc.allocator(), exec.mpi_type, mesh_aloc.allocator(), tm, tm_total);
#endif

    // if (exec_avail.cuda && exec_avail.mpi_type && exec_avail.mpi_type)
    //   do_cycles(con_comm, comminfo, info, num_vars, ncycles, exec.cuda, mesh_aloc.allocator(), exec.mpi_type, mesh_aloc.allocator(), exec.mpi_type, mesh_aloc.allocator(), tm, tm_total);
  }

  // device allocated
  if (alloc.cuda_device.available()) {
    AllocatorInfo& mesh_aloc = alloc.cuda_device;

    char name[1024] = ""; snprintf(name, 1024, "Mesh %s", mesh_aloc.allocator().name());
    Range r0(name, Range::blue);

    if (alloc.access.cuda_device_accessible_from_host) {
      if (exec_avail.seq && exec_avail.seq && exec_avail.seq)
        do_cycles(con_comm, comminfo, info, num_vars, ncycles, exec.seq, mesh_aloc.allocator(), exec.seq, alloc.cuda_device.allocator(), exec.seq, alloc.cuda_device.allocator(), tm, tm_total);

#ifdef COMB_ENABLE_OPENMP
      if (exec_avail.omp && exec_avail.seq && exec_avail.seq)
        do_cycles(con_comm, comminfo, info, num_vars, ncycles, exec.omp, mesh_aloc.allocator(), exec.seq, alloc.cuda_device.allocator(), exec.seq, alloc.cuda_device.allocator(), tm, tm_total);

      if (exec_avail.omp && exec_avail.omp && exec_avail.seq)
        do_cycles(con_comm, comminfo, info, num_vars, ncycles, exec.omp, mesh_aloc.allocator(), exec.omp, alloc.cuda_device.allocator(), exec.seq, alloc.cuda_device.allocator(), tm, tm_total);

      if (exec_avail.omp && exec_avail.omp && exec_avail.omp)
        do_cycles(con_comm, comminfo, info, num_vars, ncycles, exec.omp, mesh_aloc.allocator(), exec.omp, alloc.cuda_device.allocator(), exec.omp, alloc.cuda_device.allocator(), tm, tm_total);
#endif

      if (exec_avail.cuda && exec_avail.seq && exec_avail.seq)
        do_cycles(con_comm, comminfo, info, num_vars, ncycles, exec.cuda, mesh_aloc.allocator(), exec.seq, alloc.cuda_device.allocator(), exec.seq, alloc.cuda_device.allocator(), tm, tm_total);

#ifdef COMB_ENABLE_OPENMP
      if (exec_avail.cuda && exec_avail.omp && exec_avail.seq)
        do_cycles(con_comm, comminfo, info, num_vars, ncycles, exec.cuda, mesh_aloc.allocator(), exec.omp, alloc.cuda_device.allocator(), exec.seq, alloc.cuda_device.allocator(), tm, tm_total);

      if (exec_avail.cuda && exec_avail.omp && exec_avail.omp)
        do_cycles(con_comm, comminfo, info, num_vars, ncycles, exec.cuda, mesh_aloc.allocator(), exec.omp, alloc.cuda_device.allocator(), exec.omp, alloc.cuda_device.allocator(), tm, tm_total);
#endif

      if (exec_avail.cuda && exec_avail.cuda && exec_avail.seq)
        do_cycles(con_comm, comminfo, info, num_vars, ncycles, exec.cuda, mesh_aloc.allocator(), exec.cuda, alloc.cuda_device.allocator(), exec.seq, alloc.cuda_device.allocator(), tm, tm_total);
    }

    if (exec_avail.cuda && exec_avail.cuda && exec_avail.cuda)
      do_cycles(con_comm, comminfo, info, num_vars, ncycles, exec.cuda, mesh_aloc.allocator(), exec.cuda, alloc.cuda_device.allocator(), exec.cuda, alloc.cuda_device.allocator(), tm, tm_total);

    {
      if (alloc.access.cuda_device_accessible_from_host) {
        if (exec_avail.cuda && exec_avail.cuda_batch && exec_avail.seq)
          do_cycles(con_comm, comminfo, info, num_vars, ncycles, exec.cuda, mesh_aloc.allocator(), exec.cuda_batch, alloc.cuda_device.allocator(), exec.seq, alloc.cuda_device.allocator(), tm, tm_total);
      }

      if (exec_avail.cuda && exec_avail.cuda_batch && exec_avail.cuda_batch)
        do_cycles(con_comm, comminfo, info, num_vars, ncycles, exec.cuda, mesh_aloc.allocator(), exec.cuda_batch, alloc.cuda_device.allocator(), exec.cuda_batch, alloc.cuda_device.allocator(), tm, tm_total);

      if (alloc.access.cuda_device_accessible_from_host) {
        if (exec_avail.cuda && exec_avail.cuda_persistent && exec_avail.seq)
          do_cycles(con_comm, comminfo, info, num_vars, ncycles, exec.cuda, mesh_aloc.allocator(), exec.cuda_persistent, alloc.cuda_device.allocator(), exec.seq, alloc.cuda_device.allocator(), tm, tm_total);
      }

      if (exec_avail.cuda && exec_avail.cuda_persistent && exec_avail.cuda_persistent)
        do_cycles(con_comm, comminfo, info, num_vars, ncycles, exec.cuda, mesh_aloc.allocator(), exec.cuda_persistent, alloc.cuda_device.allocator(), exec.cuda_persistent, alloc.cuda_device.allocator(), tm, tm_total);


      SetReset<bool> sr_gs(get_batch_always_grid_sync(), false);

      if (alloc.access.cuda_device_accessible_from_host) {
        if (exec_avail.cuda && exec_avail.cuda_batch_fewgs && exec_avail.seq)
          do_cycles(con_comm, comminfo, info, num_vars, ncycles, exec.cuda, mesh_aloc.allocator(), exec.cuda_batch, alloc.cuda_device.allocator(), exec.seq, alloc.cuda_device.allocator(), tm, tm_total);
      }

      if (exec_avail.cuda && exec_avail.cuda_batch_fewgs && exec_avail.cuda_batch_fewgs)
        do_cycles(con_comm, comminfo, info, num_vars, ncycles, exec.cuda, mesh_aloc.allocator(), exec.cuda_batch, alloc.cuda_device.allocator(), exec.cuda_batch, alloc.cuda_device.allocator(), tm, tm_total);

      if (alloc.access.cuda_device_accessible_from_host) {
        if (exec_avail.cuda && exec_avail.cuda_persistent_fewgs && exec_avail.seq)
          do_cycles(con_comm, comminfo, info, num_vars, ncycles, exec.cuda, mesh_aloc.allocator(), exec.cuda_persistent, alloc.cuda_device.allocator(), exec.seq, alloc.cuda_device.allocator(), tm, tm_total);
      }

      if (exec_avail.cuda && exec_avail.cuda_persistent_fewgs && exec_avail.cuda_persistent_fewgs)
        do_cycles(con_comm, comminfo, info, num_vars, ncycles, exec.cuda, mesh_aloc.allocator(), exec.cuda_persistent, alloc.cuda_device.allocator(), exec.cuda_persistent, alloc.cuda_device.allocator(), tm, tm_total);
    }

#ifdef COMB_ENABLE_CUDA_GRAPH
    if (alloc.access.cuda_device_accessible_from_host) {
      if (exec_avail.cuda && exec_avail.cuda_graph && exec_avail.seq)
        do_cycles(con_comm, comminfo, info, num_vars, ncycles, exec.cuda, mesh_aloc.allocator(), exec.cuda_graph, alloc.cuda_device.allocator(), exec.seq, alloc.cuda_device.allocator(), tm, tm_total);
    }

    if (exec_avail.cuda && exec_avail.cuda_graph && exec_avail.cuda_graph)
      do_cycles(con_comm, comminfo, info, num_vars, ncycles, exec.cuda, mesh_aloc.allocator(), exec.cuda_graph, alloc.cuda_device.allocator(), exec.cuda_graph, alloc.cuda_device.allocator(), tm, tm_total);
#endif

    if (alloc.access.cuda_aware_mpi) {
      if (alloc.access.cuda_device_accessible_from_host) {
        // if (exec_avail.seq && exec_avail.mpi_type && exec_avail.mpi_type)
        //   do_cycles(con_comm, comminfo, info, num_vars, ncycles, exec.seq, mesh_aloc.allocator(), exec.mpi_type, mesh_aloc.allocator(), exec.mpi_type, mesh_aloc.allocator(), tm, tm_total);
      }

#ifdef COMB_ENABLE_OPENMP
      if (alloc.access.cuda_device_accessible_from_host) {
        // if (exec_avail.omp && exec_avail.mpi_type && exec_avail.mpi_type)
        //   do_cycles(con_comm, comminfo, info, num_vars, ncycles, exec.omp, mesh_aloc.allocator(), exec.mpi_type, mesh_aloc.allocator(), exec.mpi_type, mesh_aloc.allocator(), tm, tm_total);
      }
#endif

      // if (exec_avail.cuda && exec_avail.mpi_type && exec_avail.mpi_type)
      //   do_cycles(con_comm, comminfo, info, num_vars, ncycles, exec.cuda, mesh_aloc.allocator(), exec.mpi_type, mesh_aloc.allocator(), exec.mpi_type, mesh_aloc.allocator(), tm, tm_total);
    }
  }

  // managed allocated
  if (alloc.cuda_managed.available()) {
    AllocatorInfo& mesh_aloc = alloc.cuda_managed;

    char name[1024] = ""; snprintf(name, 1024, "Mesh %s", mesh_aloc.allocator().name());
    Range r0(name, Range::blue);

    if (exec_avail.seq && exec_avail.seq && exec_avail.seq)
      do_cycles(con_comm, comminfo, info, num_vars, ncycles, exec.seq, mesh_aloc.allocator(), exec.seq, alloc.cuda_device.allocator(), exec.seq, alloc.cuda_device.allocator(), tm, tm_total);

#ifdef COMB_ENABLE_OPENMP
    if (exec_avail.omp && exec_avail.seq && exec_avail.seq)
      do_cycles(con_comm, comminfo, info, num_vars, ncycles, exec.omp, mesh_aloc.allocator(), exec.seq, alloc.cuda_device.allocator(), exec.seq, alloc.cuda_device.allocator(), tm, tm_total);

    if (exec_avail.omp && exec_avail.omp && exec_avail.seq)
      do_cycles(con_comm, comminfo, info, num_vars, ncycles, exec.omp, mesh_aloc.allocator(), exec.omp, alloc.cuda_device.allocator(), exec.seq, alloc.cuda_device.allocator(), tm, tm_total);

    if (exec_avail.omp && exec_avail.omp && exec_avail.omp)
      do_cycles(con_comm, comminfo, info, num_vars, ncycles, exec.omp, mesh_aloc.allocator(), exec.omp, alloc.cuda_device.allocator(), exec.omp, alloc.cuda_device.allocator(), tm, tm_total);
#endif

    if (exec_avail.cuda && exec_avail.seq && exec_avail.seq)
      do_cycles(con_comm, comminfo, info, num_vars, ncycles, exec.cuda, mesh_aloc.allocator(), exec.seq, alloc.cuda_device.allocator(), exec.seq, alloc.cuda_device.allocator(), tm, tm_total);

#ifdef COMB_ENABLE_OPENMP
    if (exec_avail.cuda && exec_avail.omp && exec_avail.seq)
      do_cycles(con_comm, comminfo, info, num_vars, ncycles, exec.cuda, mesh_aloc.allocator(), exec.omp, alloc.cuda_device.allocator(), exec.seq, alloc.cuda_device.allocator(), tm, tm_total);

    if (exec_avail.cuda && exec_avail.omp && exec_avail.omp)
      do_cycles(con_comm, comminfo, info, num_vars, ncycles, exec.cuda, mesh_aloc.allocator(), exec.omp, alloc.cuda_device.allocator(), exec.omp, alloc.cuda_device.allocator(), tm, tm_total);
#endif

    if (exec_avail.cuda && exec_avail.cuda && exec_avail.seq)
      do_cycles(con_comm, comminfo, info, num_vars, ncycles, exec.cuda, mesh_aloc.allocator(), exec.cuda, alloc.cuda_device.allocator(), exec.seq, alloc.cuda_device.allocator(), tm, tm_total);

    if (exec_avail.cuda && exec_avail.cuda && exec_avail.cuda)
      do_cycles(con_comm, comminfo, info, num_vars, ncycles, exec.cuda, mesh_aloc.allocator(), exec.cuda, alloc.cuda_device.allocator(), exec.cuda, alloc.cuda_device.allocator(), tm, tm_total);

    {
      if (exec_avail.cuda && exec_avail.cuda_batch && exec_avail.seq)
        do_cycles(con_comm, comminfo, info, num_vars, ncycles, exec.cuda, mesh_aloc.allocator(), exec.cuda_batch, alloc.cuda_device.allocator(), exec.seq, alloc.cuda_device.allocator(), tm, tm_total);

      if (exec_avail.cuda && exec_avail.cuda_batch && exec_avail.cuda_batch)
        do_cycles(con_comm, comminfo, info, num_vars, ncycles, exec.cuda, mesh_aloc.allocator(), exec.cuda_batch, alloc.cuda_device.allocator(), exec.cuda_batch, alloc.cuda_device.allocator(), tm, tm_total);

      if (exec_avail.cuda && exec_avail.cuda_persistent && exec_avail.seq)
        do_cycles(con_comm, comminfo, info, num_vars, ncycles, exec.cuda, mesh_aloc.allocator(), exec.cuda_persistent, alloc.cuda_device.allocator(), exec.seq, alloc.cuda_device.allocator(), tm, tm_total);

      if (exec_avail.cuda && exec_avail.cuda_persistent && exec_avail.cuda_persistent)
        do_cycles(con_comm, comminfo, info, num_vars, ncycles, exec.cuda, mesh_aloc.allocator(), exec.cuda_persistent, alloc.cuda_device.allocator(), exec.cuda_persistent, alloc.cuda_device.allocator(), tm, tm_total);


      SetReset<bool> sr_gs(get_batch_always_grid_sync(), false);

      if (exec_avail.cuda && exec_avail.cuda_batch_fewgs && exec_avail.seq)
        do_cycles(con_comm, comminfo, info, num_vars, ncycles, exec.cuda, mesh_aloc.allocator(), exec.cuda_batch, alloc.cuda_device.allocator(), exec.seq, alloc.cuda_device.allocator(), tm, tm_total);

      if (exec_avail.cuda && exec_avail.cuda_batch_fewgs && exec_avail.cuda_batch_fewgs)
        do_cycles(con_comm, comminfo, info, num_vars, ncycles, exec.cuda, mesh_aloc.allocator(), exec.cuda_batch, alloc.cuda_device.allocator(), exec.cuda_batch, alloc.cuda_device.allocator(), tm, tm_total);

      if (exec_avail.cuda && exec_avail.cuda_persistent_fewgs && exec_avail.seq)
        do_cycles(con_comm, comminfo, info, num_vars, ncycles, exec.cuda, mesh_aloc.allocator(), exec.cuda_persistent, alloc.cuda_device.allocator(), exec.seq, alloc.cuda_device.allocator(), tm, tm_total);

      if (exec_avail.cuda && exec_avail.cuda_persistent_fewgs && exec_avail.cuda_persistent_fewgs)
        do_cycles(con_comm, comminfo, info, num_vars, ncycles, exec.cuda, mesh_aloc.allocator(), exec.cuda_persistent, alloc.cuda_device.allocator(), exec.cuda_persistent, alloc.cuda_device.allocator(), tm, tm_total);
    }

#ifdef COMB_ENABLE_CUDA_GRAPH
    if (exec_avail.cuda && exec_avail.cuda_graph && exec_avail.seq)
      do_cycles(con_comm, comminfo, info, num_vars, ncycles, exec.cuda, mesh_aloc.allocator(), exec.cuda_graph, alloc.cuda_device.allocator(), exec.seq, alloc.cuda_device.allocator(), tm, tm_total);

    if (exec_avail.cuda && exec_avail.cuda_graph && exec_avail.cuda_graph)
      do_cycles(con_comm, comminfo, info, num_vars, ncycles, exec.cuda, mesh_aloc.allocator(), exec.cuda_graph, alloc.cuda_device.allocator(), exec.cuda_graph, alloc.cuda_device.allocator(), tm, tm_total);
#endif

    if (alloc.access.cuda_aware_mpi) {
      // if (exec_avail.seq && exec_avail.mpi_type && exec_avail.mpi_type)
      //   do_cycles(con_comm, comminfo, info, num_vars, ncycles, exec.seq, mesh_aloc.allocator(), exec.mpi_type, mesh_aloc.allocator(), exec.mpi_type, mesh_aloc.allocator(), tm, tm_total);

#ifdef COMB_ENABLE_OPENMP
      // if (exec_avail.omp && exec_avail.mpi_type && exec_avail.mpi_type)
      //   do_cycles(con_comm, comminfo, info, num_vars, ncycles, exec.omp, mesh_aloc.allocator(), exec.mpi_type, mesh_aloc.allocator(), exec.mpi_type, mesh_aloc.allocator(), tm, tm_total);
#endif

      // if (exec_avail.cuda && exec_avail.mpi_type && exec_avail.mpi_type)
      //   do_cycles(con_comm, comminfo, info, num_vars, ncycles, exec.cuda, mesh_aloc.allocator(), exec.mpi_type, mesh_aloc.allocator(), exec.mpi_type, mesh_aloc.allocator(), tm, tm_total);
    }
  }

  // managed host preferred allocated
  if (alloc.cuda_managed_host_preferred.available()) {
    AllocatorInfo& mesh_aloc = alloc.cuda_managed_host_preferred;

    char name[1024] = ""; snprintf(name, 1024, "Mesh %s", mesh_aloc.allocator().name());
    Range r0(name, Range::blue);

    if (exec_avail.seq && exec_avail.seq && exec_avail.seq)
      do_cycles(con_comm, comminfo, info, num_vars, ncycles, exec.seq, mesh_aloc.allocator(), exec.seq, alloc.cuda_device.allocator(), exec.seq, alloc.cuda_device.allocator(), tm, tm_total);

#ifdef COMB_ENABLE_OPENMP
    if (exec_avail.omp && exec_avail.seq && exec_avail.seq)
      do_cycles(con_comm, comminfo, info, num_vars, ncycles, exec.omp, mesh_aloc.allocator(), exec.seq, alloc.cuda_device.allocator(), exec.seq, alloc.cuda_device.allocator(), tm, tm_total);

    if (exec_avail.omp && exec_avail.omp && exec_avail.seq)
      do_cycles(con_comm, comminfo, info, num_vars, ncycles, exec.omp, mesh_aloc.allocator(), exec.omp, alloc.cuda_device.allocator(), exec.seq, alloc.cuda_device.allocator(), tm, tm_total);

    if (exec_avail.omp && exec_avail.omp && exec_avail.omp)
      do_cycles(con_comm, comminfo, info, num_vars, ncycles, exec.omp, mesh_aloc.allocator(), exec.omp, alloc.cuda_device.allocator(), exec.omp, alloc.cuda_device.allocator(), tm, tm_total);
#endif

    if (exec_avail.cuda && exec_avail.seq && exec_avail.seq)
      do_cycles(con_comm, comminfo, info, num_vars, ncycles, exec.cuda, mesh_aloc.allocator(), exec.seq, alloc.cuda_device.allocator(), exec.seq, alloc.cuda_device.allocator(), tm, tm_total);

#ifdef COMB_ENABLE_OPENMP
    if (exec_avail.cuda && exec_avail.omp && exec_avail.seq)
      do_cycles(con_comm, comminfo, info, num_vars, ncycles, exec.cuda, mesh_aloc.allocator(), exec.omp, alloc.cuda_device.allocator(), exec.seq, alloc.cuda_device.allocator(), tm, tm_total);

    if (exec_avail.cuda && exec_avail.omp && exec_avail.omp)
      do_cycles(con_comm, comminfo, info, num_vars, ncycles, exec.cuda, mesh_aloc.allocator(), exec.omp, alloc.cuda_device.allocator(), exec.omp, alloc.cuda_device.allocator(), tm, tm_total);
#endif

    if (exec_avail.cuda && exec_avail.cuda && exec_avail.seq)
      do_cycles(con_comm, comminfo, info, num_vars, ncycles, exec.cuda, mesh_aloc.allocator(), exec.cuda, alloc.cuda_device.allocator(), exec.seq, alloc.cuda_device.allocator(), tm, tm_total);

    if (exec_avail.cuda && exec_avail.cuda && exec_avail.cuda)
      do_cycles(con_comm, comminfo, info, num_vars, ncycles, exec.cuda, mesh_aloc.allocator(), exec.cuda, alloc.cuda_device.allocator(), exec.cuda, alloc.cuda_device.allocator(), tm, tm_total);

    {
      if (exec_avail.cuda && exec_avail.cuda_batch && exec_avail.seq)
        do_cycles(con_comm, comminfo, info, num_vars, ncycles, exec.cuda, mesh_aloc.allocator(), exec.cuda_batch, alloc.cuda_device.allocator(), exec.seq, alloc.cuda_device.allocator(), tm, tm_total);

      if (exec_avail.cuda && exec_avail.cuda_batch && exec_avail.cuda_batch)
        do_cycles(con_comm, comminfo, info, num_vars, ncycles, exec.cuda, mesh_aloc.allocator(), exec.cuda_batch, alloc.cuda_device.allocator(), exec.cuda_batch, alloc.cuda_device.allocator(), tm, tm_total);

      if (exec_avail.cuda && exec_avail.cuda_persistent && exec_avail.seq)
        do_cycles(con_comm, comminfo, info, num_vars, ncycles, exec.cuda, mesh_aloc.allocator(), exec.cuda_persistent, alloc.cuda_device.allocator(), exec.seq, alloc.cuda_device.allocator(), tm, tm_total);

      if (exec_avail.cuda && exec_avail.cuda_persistent && exec_avail.cuda_persistent)
        do_cycles(con_comm, comminfo, info, num_vars, ncycles, exec.cuda, mesh_aloc.allocator(), exec.cuda_persistent, alloc.cuda_device.allocator(), exec.cuda_persistent, alloc.cuda_device.allocator(), tm, tm_total);


      SetReset<bool> sr_gs(get_batch_always_grid_sync(), false);

      if (exec_avail.cuda && exec_avail.cuda_batch_fewgs && exec_avail.seq)
        do_cycles(con_comm, comminfo, info, num_vars, ncycles, exec.cuda, mesh_aloc.allocator(), exec.cuda_batch, alloc.cuda_device.allocator(), exec.seq, alloc.cuda_device.allocator(), tm, tm_total);

      if (exec_avail.cuda && exec_avail.cuda_batch_fewgs && exec_avail.cuda_batch_fewgs)
        do_cycles(con_comm, comminfo, info, num_vars, ncycles, exec.cuda, mesh_aloc.allocator(), exec.cuda_batch, alloc.cuda_device.allocator(), exec.cuda_batch, alloc.cuda_device.allocator(), tm, tm_total);

      if (exec_avail.cuda && exec_avail.cuda_persistent_fewgs && exec_avail.seq)
        do_cycles(con_comm, comminfo, info, num_vars, ncycles, exec.cuda, mesh_aloc.allocator(), exec.cuda_persistent, alloc.cuda_device.allocator(), exec.seq, alloc.cuda_device.allocator(), tm, tm_total);

      if (exec_avail.cuda && exec_avail.cuda_persistent_fewgs && exec_avail.cuda_persistent_fewgs)
        do_cycles(con_comm, comminfo, info, num_vars, ncycles, exec.cuda, mesh_aloc.allocator(), exec.cuda_persistent, alloc.cuda_device.allocator(), exec.cuda_persistent, alloc.cuda_device.allocator(), tm, tm_total);
    }

#ifdef COMB_ENABLE_CUDA_GRAPH
    if (exec_avail.cuda && exec_avail.cuda_graph && exec_avail.seq)
      do_cycles(con_comm, comminfo, info, num_vars, ncycles, exec.cuda, mesh_aloc.allocator(), exec.cuda_graph, alloc.cuda_device.allocator(), exec.seq, alloc.cuda_device.allocator(), tm, tm_total);

    if (exec_avail.cuda && exec_avail.cuda_graph && exec_avail.cuda_graph)
      do_cycles(con_comm, comminfo, info, num_vars, ncycles, exec.cuda, mesh_aloc.allocator(), exec.cuda_graph, alloc.cuda_device.allocator(), exec.cuda_graph, alloc.cuda_device.allocator(), tm, tm_total);
#endif

    if (alloc.access.cuda_aware_mpi) {
      // if (exec_avail.seq && exec_avail.mpi_type && exec_avail.mpi_type)
      //   do_cycles(con_comm, comminfo, info, num_vars, ncycles, exec.seq, mesh_aloc.allocator(), exec.mpi_type, mesh_aloc.allocator(), exec.mpi_type, mesh_aloc.allocator(), tm, tm_total);

#ifdef COMB_ENABLE_OPENMP
      // if (exec_avail.omp && exec_avail.mpi_type && exec_avail.mpi_type)
      //   do_cycles(con_comm, comminfo, info, num_vars, ncycles, exec.omp, mesh_aloc.allocator(), exec.mpi_type, mesh_aloc.allocator(), exec.mpi_type, mesh_aloc.allocator(), tm, tm_total);
#endif

      // if (exec_avail.cuda && exec_avail.mpi_type && exec_avail.mpi_type)
      //   do_cycles(con_comm, comminfo, info, num_vars, ncycles, exec.cuda, mesh_aloc.allocator(), exec.mpi_type, mesh_aloc.allocator(), exec.mpi_type, mesh_aloc.allocator(), tm, tm_total);
    }
  }

  // managed host preferred device accessed allocated
  if (alloc.cuda_managed_host_preferred_device_accessed.available()) {
    AllocatorInfo& mesh_aloc = alloc.cuda_managed_host_preferred_device_accessed;

    char name[1024] = ""; snprintf(name, 1024, "Mesh %s", mesh_aloc.allocator().name());
    Range r0(name, Range::blue);

    if (exec_avail.seq && exec_avail.seq && exec_avail.seq)
      do_cycles(con_comm, comminfo, info, num_vars, ncycles, exec.seq, mesh_aloc.allocator(), exec.seq, alloc.cuda_device.allocator(), exec.seq, alloc.cuda_device.allocator(), tm, tm_total);

#ifdef COMB_ENABLE_OPENMP
    if (exec_avail.omp && exec_avail.seq && exec_avail.seq)
      do_cycles(con_comm, comminfo, info, num_vars, ncycles, exec.omp, mesh_aloc.allocator(), exec.seq, alloc.cuda_device.allocator(), exec.seq, alloc.cuda_device.allocator(), tm, tm_total);

    if (exec_avail.omp && exec_avail.omp && exec_avail.seq)
      do_cycles(con_comm, comminfo, info, num_vars, ncycles, exec.omp, mesh_aloc.allocator(), exec.omp, alloc.cuda_device.allocator(), exec.seq, alloc.cuda_device.allocator(), tm, tm_total);

    if (exec_avail.omp && exec_avail.omp && exec_avail.omp)
      do_cycles(con_comm, comminfo, info, num_vars, ncycles, exec.omp, mesh_aloc.allocator(), exec.omp, alloc.cuda_device.allocator(), exec.omp, alloc.cuda_device.allocator(), tm, tm_total);
#endif

    if (exec_avail.cuda && exec_avail.seq && exec_avail.seq)
      do_cycles(con_comm, comminfo, info, num_vars, ncycles, exec.cuda, mesh_aloc.allocator(), exec.seq, alloc.cuda_device.allocator(), exec.seq, alloc.cuda_device.allocator(), tm, tm_total);

#ifdef COMB_ENABLE_OPENMP
    if (exec_avail.cuda && exec_avail.omp && exec_avail.seq)
      do_cycles(con_comm, comminfo, info, num_vars, ncycles, exec.cuda, mesh_aloc.allocator(), exec.omp, alloc.cuda_device.allocator(), exec.seq, alloc.cuda_device.allocator(), tm, tm_total);

    if (exec_avail.cuda && exec_avail.omp && exec_avail.omp)
      do_cycles(con_comm, comminfo, info, num_vars, ncycles, exec.cuda, mesh_aloc.allocator(), exec.omp, alloc.cuda_device.allocator(), exec.omp, alloc.cuda_device.allocator(), tm, tm_total);
#endif

    if (exec_avail.cuda && exec_avail.cuda && exec_avail.seq)
      do_cycles(con_comm, comminfo, info, num_vars, ncycles, exec.cuda, mesh_aloc.allocator(), exec.cuda, alloc.cuda_device.allocator(), exec.seq, alloc.cuda_device.allocator(), tm, tm_total);

    if (exec_avail.cuda && exec_avail.cuda && exec_avail.cuda)
      do_cycles(con_comm, comminfo, info, num_vars, ncycles, exec.cuda, mesh_aloc.allocator(), exec.cuda, alloc.cuda_device.allocator(), exec.cuda, alloc.cuda_device.allocator(), tm, tm_total);

    {
      if (exec_avail.cuda && exec_avail.cuda_batch && exec_avail.seq)
        do_cycles(con_comm, comminfo, info, num_vars, ncycles, exec.cuda, mesh_aloc.allocator(), exec.cuda_batch, alloc.cuda_device.allocator(), exec.seq, alloc.cuda_device.allocator(), tm, tm_total);

      if (exec_avail.cuda && exec_avail.cuda_batch && exec_avail.cuda_batch)
        do_cycles(con_comm, comminfo, info, num_vars, ncycles, exec.cuda, mesh_aloc.allocator(), exec.cuda_batch, alloc.cuda_device.allocator(), exec.cuda_batch, alloc.cuda_device.allocator(), tm, tm_total);

      if (exec_avail.cuda && exec_avail.cuda_persistent && exec_avail.seq)
        do_cycles(con_comm, comminfo, info, num_vars, ncycles, exec.cuda, mesh_aloc.allocator(), exec.cuda_persistent, alloc.cuda_device.allocator(), exec.seq, alloc.cuda_device.allocator(), tm, tm_total);

      if (exec_avail.cuda && exec_avail.cuda_persistent && exec_avail.cuda_persistent)
        do_cycles(con_comm, comminfo, info, num_vars, ncycles, exec.cuda, mesh_aloc.allocator(), exec.cuda_persistent, alloc.cuda_device.allocator(), exec.cuda_persistent, alloc.cuda_device.allocator(), tm, tm_total);


      SetReset<bool> sr_gs(get_batch_always_grid_sync(), false);

      if (exec_avail.cuda && exec_avail.cuda_batch_fewgs && exec_avail.seq)
        do_cycles(con_comm, comminfo, info, num_vars, ncycles, exec.cuda, mesh_aloc.allocator(), exec.cuda_batch, alloc.cuda_device.allocator(), exec.seq, alloc.cuda_device.allocator(), tm, tm_total);

      if (exec_avail.cuda && exec_avail.cuda_batch_fewgs && exec_avail.cuda_batch_fewgs)
        do_cycles(con_comm, comminfo, info, num_vars, ncycles, exec.cuda, mesh_aloc.allocator(), exec.cuda_batch, alloc.cuda_device.allocator(), exec.cuda_batch, alloc.cuda_device.allocator(), tm, tm_total);

      if (exec_avail.cuda && exec_avail.cuda_persistent_fewgs && exec_avail.seq)
        do_cycles(con_comm, comminfo, info, num_vars, ncycles, exec.cuda, mesh_aloc.allocator(), exec.cuda_persistent, alloc.cuda_device.allocator(), exec.seq, alloc.cuda_device.allocator(), tm, tm_total);

      if (exec_avail.cuda && exec_avail.cuda_persistent_fewgs && exec_avail.cuda_persistent_fewgs)
        do_cycles(con_comm, comminfo, info, num_vars, ncycles, exec.cuda, mesh_aloc.allocator(), exec.cuda_persistent, alloc.cuda_device.allocator(), exec.cuda_persistent, alloc.cuda_device.allocator(), tm, tm_total);
    }

#ifdef COMB_ENABLE_CUDA_GRAPH
    if (exec_avail.cuda && exec_avail.cuda_graph && exec_avail.seq)
      do_cycles(con_comm, comminfo, info, num_vars, ncycles, exec.cuda, mesh_aloc.allocator(), exec.cuda_graph, alloc.cuda_device.allocator(), exec.seq, alloc.cuda_device.allocator(), tm, tm_total);

    if (exec_avail.cuda && exec_avail.cuda_graph && exec_avail.cuda_graph)
      do_cycles(con_comm, comminfo, info, num_vars, ncycles, exec.cuda, mesh_aloc.allocator(), exec.cuda_graph, alloc.cuda_device.allocator(), exec.cuda_graph, alloc.cuda_device.allocator(), tm, tm_total);
#endif

    if (alloc.access.cuda_aware_mpi) {
      // if (exec_avail.seq && exec_avail.mpi_type && exec_avail.mpi_type)
      //   do_cycles(con_comm, comminfo, info, num_vars, ncycles, exec.seq, mesh_aloc.allocator(), exec.mpi_type, mesh_aloc.allocator(), exec.mpi_type, mesh_aloc.allocator(), tm, tm_total);

#ifdef COMB_ENABLE_OPENMP
      // if (exec_avail.omp && exec_avail.mpi_type && exec_avail.mpi_type)
      //   do_cycles(con_comm, comminfo, info, num_vars, ncycles, exec.omp, mesh_aloc.allocator(), exec.mpi_type, mesh_aloc.allocator(), exec.mpi_type, mesh_aloc.allocator(), tm, tm_total);
#endif

      // if (exec_avail.cuda && exec_avail.mpi_type && exec_avail.mpi_type)
      //   do_cycles(con_comm, comminfo, info, num_vars, ncycles, exec.cuda, mesh_aloc.allocator(), exec.mpi_type, mesh_aloc.allocator(), exec.mpi_type, mesh_aloc.allocator(), tm, tm_total);
    }
  }

  // managed device preferred allocated
  if (alloc.cuda_managed_device_preferred.available()) {
    AllocatorInfo& mesh_aloc = alloc.cuda_managed_device_preferred;

    char name[1024] = ""; snprintf(name, 1024, "Mesh %s", mesh_aloc.allocator().name());
    Range r0(name, Range::blue);

    if (exec_avail.seq && exec_avail.seq && exec_avail.seq)
      do_cycles(con_comm, comminfo, info, num_vars, ncycles, exec.seq, mesh_aloc.allocator(), exec.seq, alloc.cuda_device.allocator(), exec.seq, alloc.cuda_device.allocator(), tm, tm_total);

#ifdef COMB_ENABLE_OPENMP
    if (exec_avail.omp && exec_avail.seq && exec_avail.seq)
      do_cycles(con_comm, comminfo, info, num_vars, ncycles, exec.omp, mesh_aloc.allocator(), exec.seq, alloc.cuda_device.allocator(), exec.seq, alloc.cuda_device.allocator(), tm, tm_total);

    if (exec_avail.omp && exec_avail.omp && exec_avail.seq)
      do_cycles(con_comm, comminfo, info, num_vars, ncycles, exec.omp, mesh_aloc.allocator(), exec.omp, alloc.cuda_device.allocator(), exec.seq, alloc.cuda_device.allocator(), tm, tm_total);

    if (exec_avail.omp && exec_avail.omp && exec_avail.omp)
      do_cycles(con_comm, comminfo, info, num_vars, ncycles, exec.omp, mesh_aloc.allocator(), exec.omp, alloc.cuda_device.allocator(), exec.omp, alloc.cuda_device.allocator(), tm, tm_total);
#endif

    if (exec_avail.cuda && exec_avail.seq && exec_avail.seq)
      do_cycles(con_comm, comminfo, info, num_vars, ncycles, exec.cuda, mesh_aloc.allocator(), exec.seq, alloc.cuda_device.allocator(), exec.seq, alloc.cuda_device.allocator(), tm, tm_total);

#ifdef COMB_ENABLE_OPENMP
    if (exec_avail.cuda && exec_avail.omp && exec_avail.seq)
      do_cycles(con_comm, comminfo, info, num_vars, ncycles, exec.cuda, mesh_aloc.allocator(), exec.omp, alloc.cuda_device.allocator(), exec.seq, alloc.cuda_device.allocator(), tm, tm_total);

    if (exec_avail.cuda && exec_avail.omp && exec_avail.omp)
      do_cycles(con_comm, comminfo, info, num_vars, ncycles, exec.cuda, mesh_aloc.allocator(), exec.omp, alloc.cuda_device.allocator(), exec.omp, alloc.cuda_device.allocator(), tm, tm_total);
#endif

    if (exec_avail.cuda && exec_avail.cuda && exec_avail.seq)
      do_cycles(con_comm, comminfo, info, num_vars, ncycles, exec.cuda, mesh_aloc.allocator(), exec.cuda, alloc.cuda_device.allocator(), exec.seq, alloc.cuda_device.allocator(), tm, tm_total);

    if (exec_avail.cuda && exec_avail.cuda && exec_avail.cuda)
      do_cycles(con_comm, comminfo, info, num_vars, ncycles, exec.cuda, mesh_aloc.allocator(), exec.cuda, alloc.cuda_device.allocator(), exec.cuda, alloc.cuda_device.allocator(), tm, tm_total);

    {
      if (exec_avail.cuda && exec_avail.cuda_batch && exec_avail.seq)
        do_cycles(con_comm, comminfo, info, num_vars, ncycles, exec.cuda, mesh_aloc.allocator(), exec.cuda_batch, alloc.cuda_device.allocator(), exec.seq, alloc.cuda_device.allocator(), tm, tm_total);

      if (exec_avail.cuda && exec_avail.cuda_batch && exec_avail.cuda_batch)
        do_cycles(con_comm, comminfo, info, num_vars, ncycles, exec.cuda, mesh_aloc.allocator(), exec.cuda_batch, alloc.cuda_device.allocator(), exec.cuda_batch, alloc.cuda_device.allocator(), tm, tm_total);

      if (exec_avail.cuda && exec_avail.cuda_persistent && exec_avail.seq)
        do_cycles(con_comm, comminfo, info, num_vars, ncycles, exec.cuda, mesh_aloc.allocator(), exec.cuda_persistent, alloc.cuda_device.allocator(), exec.seq, alloc.cuda_device.allocator(), tm, tm_total);

      if (exec_avail.cuda && exec_avail.cuda_persistent && exec_avail.cuda_persistent)
        do_cycles(con_comm, comminfo, info, num_vars, ncycles, exec.cuda, mesh_aloc.allocator(), exec.cuda_persistent, alloc.cuda_device.allocator(), exec.cuda_persistent, alloc.cuda_device.allocator(), tm, tm_total);


      SetReset<bool> sr_gs(get_batch_always_grid_sync(), false);

      if (exec_avail.cuda && exec_avail.cuda_batch_fewgs && exec_avail.seq)
        do_cycles(con_comm, comminfo, info, num_vars, ncycles, exec.cuda, mesh_aloc.allocator(), exec.cuda_batch, alloc.cuda_device.allocator(), exec.seq, alloc.cuda_device.allocator(), tm, tm_total);

      if (exec_avail.cuda && exec_avail.cuda_batch_fewgs && exec_avail.cuda_batch_fewgs)
        do_cycles(con_comm, comminfo, info, num_vars, ncycles, exec.cuda, mesh_aloc.allocator(), exec.cuda_batch, alloc.cuda_device.allocator(), exec.cuda_batch, alloc.cuda_device.allocator(), tm, tm_total);

      if (exec_avail.cuda && exec_avail.cuda_persistent_fewgs && exec_avail.seq)
        do_cycles(con_comm, comminfo, info, num_vars, ncycles, exec.cuda, mesh_aloc.allocator(), exec.cuda_persistent, alloc.cuda_device.allocator(), exec.seq, alloc.cuda_device.allocator(), tm, tm_total);

      if (exec_avail.cuda && exec_avail.cuda_persistent_fewgs && exec_avail.cuda_persistent_fewgs)
        do_cycles(con_comm, comminfo, info, num_vars, ncycles, exec.cuda, mesh_aloc.allocator(), exec.cuda_persistent, alloc.cuda_device.allocator(), exec.cuda_persistent, alloc.cuda_device.allocator(), tm, tm_total);
    }

#ifdef COMB_ENABLE_CUDA_GRAPH
    if (exec_avail.cuda && exec_avail.cuda_graph && exec_avail.seq)
      do_cycles(con_comm, comminfo, info, num_vars, ncycles, exec.cuda, mesh_aloc.allocator(), exec.cuda_graph, alloc.cuda_device.allocator(), exec.seq, alloc.cuda_device.allocator(), tm, tm_total);

    if (exec_avail.cuda && exec_avail.cuda_graph && exec_avail.cuda_graph)
      do_cycles(con_comm, comminfo, info, num_vars, ncycles, exec.cuda, mesh_aloc.allocator(), exec.cuda_graph, alloc.cuda_device.allocator(), exec.cuda_graph, alloc.cuda_device.allocator(), tm, tm_total);
#endif

    if (alloc.access.cuda_aware_mpi) {
      // if (exec_avail.seq && exec_avail.mpi_type && exec_avail.mpi_type)
      //   do_cycles(con_comm, comminfo, info, num_vars, ncycles, exec.seq, mesh_aloc.allocator(), exec.mpi_type, mesh_aloc.allocator(), exec.mpi_type, mesh_aloc.allocator(), tm, tm_total);

#ifdef COMB_ENABLE_OPENMP
      // if (exec_avail.omp && exec_avail.mpi_type && exec_avail.mpi_type)
      //   do_cycles(con_comm, comminfo, info, num_vars, ncycles, exec.omp, mesh_aloc.allocator(), exec.mpi_type, mesh_aloc.allocator(), exec.mpi_type, mesh_aloc.allocator(), tm, tm_total);
#endif

      // if (exec_avail.cuda && exec_avail.mpi_type && exec_avail.mpi_type)
      //   do_cycles(con_comm, comminfo, info, num_vars, ncycles, exec.cuda, mesh_aloc.allocator(), exec.mpi_type, mesh_aloc.allocator(), exec.mpi_type, mesh_aloc.allocator(), tm, tm_total);
    }
  }

  // managed device preferred host accessed allocated
  if (alloc.cuda_managed_device_preferred_host_accessed.available()) {
    AllocatorInfo& mesh_aloc = alloc.cuda_managed_device_preferred_host_accessed;

    char name[1024] = ""; snprintf(name, 1024, "Mesh %s", mesh_aloc.allocator().name());
    Range r0(name, Range::blue);

    if (exec_avail.seq && exec_avail.seq && exec_avail.seq)
      do_cycles(con_comm, comminfo, info, num_vars, ncycles, exec.seq, mesh_aloc.allocator(), exec.seq, alloc.cuda_device.allocator(), exec.seq, alloc.cuda_device.allocator(), tm, tm_total);

#ifdef COMB_ENABLE_OPENMP
    if (exec_avail.omp && exec_avail.seq && exec_avail.seq)
      do_cycles(con_comm, comminfo, info, num_vars, ncycles, exec.omp, mesh_aloc.allocator(), exec.seq, alloc.cuda_device.allocator(), exec.seq, alloc.cuda_device.allocator(), tm, tm_total);

    if (exec_avail.omp && exec_avail.omp && exec_avail.seq)
      do_cycles(con_comm, comminfo, info, num_vars, ncycles, exec.omp, mesh_aloc.allocator(), exec.omp, alloc.cuda_device.allocator(), exec.seq, alloc.cuda_device.allocator(), tm, tm_total);

    if (exec_avail.omp && exec_avail.omp && exec_avail.omp)
      do_cycles(con_comm, comminfo, info, num_vars, ncycles, exec.omp, mesh_aloc.allocator(), exec.omp, alloc.cuda_device.allocator(), exec.omp, alloc.cuda_device.allocator(), tm, tm_total);
#endif

    if (exec_avail.cuda && exec_avail.seq && exec_avail.seq)
      do_cycles(con_comm, comminfo, info, num_vars, ncycles, exec.cuda, mesh_aloc.allocator(), exec.seq, alloc.cuda_device.allocator(), exec.seq, alloc.cuda_device.allocator(), tm, tm_total);

#ifdef COMB_ENABLE_OPENMP
    if (exec_avail.cuda && exec_avail.omp && exec_avail.seq)
      do_cycles(con_comm, comminfo, info, num_vars, ncycles, exec.cuda, mesh_aloc.allocator(), exec.omp, alloc.cuda_device.allocator(), exec.seq, alloc.cuda_device.allocator(), tm, tm_total);

    if (exec_avail.cuda && exec_avail.omp && exec_avail.omp)
      do_cycles(con_comm, comminfo, info, num_vars, ncycles, exec.cuda, mesh_aloc.allocator(), exec.omp, alloc.cuda_device.allocator(), exec.omp, alloc.cuda_device.allocator(), tm, tm_total);
#endif

    if (exec_avail.cuda && exec_avail.cuda && exec_avail.seq)
      do_cycles(con_comm, comminfo, info, num_vars, ncycles, exec.cuda, mesh_aloc.allocator(), exec.cuda, alloc.cuda_device.allocator(), exec.seq, alloc.cuda_device.allocator(), tm, tm_total);

    if (exec_avail.cuda && exec_avail.cuda && exec_avail.cuda)
      do_cycles(con_comm, comminfo, info, num_vars, ncycles, exec.cuda, mesh_aloc.allocator(), exec.cuda, alloc.cuda_device.allocator(), exec.cuda, alloc.cuda_device.allocator(), tm, tm_total);

    {
      if (exec_avail.cuda && exec_avail.cuda_batch && exec_avail.seq)
        do_cycles(con_comm, comminfo, info, num_vars, ncycles, exec.cuda, mesh_aloc.allocator(), exec.cuda_batch, alloc.cuda_device.allocator(), exec.seq, alloc.cuda_device.allocator(), tm, tm_total);

      if (exec_avail.cuda && exec_avail.cuda_batch && exec_avail.cuda_batch)
        do_cycles(con_comm, comminfo, info, num_vars, ncycles, exec.cuda, mesh_aloc.allocator(), exec.cuda_batch, alloc.cuda_device.allocator(), exec.cuda_batch, alloc.cuda_device.allocator(), tm, tm_total);

      if (exec_avail.cuda && exec_avail.cuda_persistent && exec_avail.seq)
        do_cycles(con_comm, comminfo, info, num_vars, ncycles, exec.cuda, mesh_aloc.allocator(), exec.cuda_persistent, alloc.cuda_device.allocator(), exec.seq, alloc.cuda_device.allocator(), tm, tm_total);

      if (exec_avail.cuda && exec_avail.cuda_persistent && exec_avail.cuda_persistent)
        do_cycles(con_comm, comminfo, info, num_vars, ncycles, exec.cuda, mesh_aloc.allocator(), exec.cuda_persistent, alloc.cuda_device.allocator(), exec.cuda_persistent, alloc.cuda_device.allocator(), tm, tm_total);


      SetReset<bool> sr_gs(get_batch_always_grid_sync(), false);

      if (exec_avail.cuda && exec_avail.cuda_batch_fewgs && exec_avail.seq)
        do_cycles(con_comm, comminfo, info, num_vars, ncycles, exec.cuda, mesh_aloc.allocator(), exec.cuda_batch, alloc.cuda_device.allocator(), exec.seq, alloc.cuda_device.allocator(), tm, tm_total);

      if (exec_avail.cuda && exec_avail.cuda_batch_fewgs && exec_avail.cuda_batch_fewgs)
        do_cycles(con_comm, comminfo, info, num_vars, ncycles, exec.cuda, mesh_aloc.allocator(), exec.cuda_batch, alloc.cuda_device.allocator(), exec.cuda_batch, alloc.cuda_device.allocator(), tm, tm_total);

      if (exec_avail.cuda && exec_avail.cuda_persistent_fewgs && exec_avail.seq)
        do_cycles(con_comm, comminfo, info, num_vars, ncycles, exec.cuda, mesh_aloc.allocator(), exec.cuda_persistent, alloc.cuda_device.allocator(), exec.seq, alloc.cuda_device.allocator(), tm, tm_total);

      if (exec_avail.cuda && exec_avail.cuda_persistent_fewgs && exec_avail.cuda_persistent_fewgs)
        do_cycles(con_comm, comminfo, info, num_vars, ncycles, exec.cuda, mesh_aloc.allocator(), exec.cuda_persistent, alloc.cuda_device.allocator(), exec.cuda_persistent, alloc.cuda_device.allocator(), tm, tm_total);
    }

#ifdef COMB_ENABLE_CUDA_GRAPH
    if (exec_avail.cuda && exec_avail.cuda_graph && exec_avail.seq)
      do_cycles(con_comm, comminfo, info, num_vars, ncycles, exec.cuda, mesh_aloc.allocator(), exec.cuda_graph, alloc.cuda_device.allocator(), exec.seq, alloc.cuda_device.allocator(), tm, tm_total);

    if (exec_avail.cuda && exec_avail.cuda_graph && exec_avail.cuda_graph)
      do_cycles(con_comm, comminfo, info, num_vars, ncycles, exec.cuda, mesh_aloc.allocator(), exec.cuda_graph, alloc.cuda_device.allocator(), exec.cuda_graph, alloc.cuda_device.allocator(), tm, tm_total);
#endif

    if (alloc.access.cuda_aware_mpi) {
      // if (exec_avail.seq && exec_avail.mpi_type && exec_avail.mpi_type)
      //   do_cycles(con_comm, comminfo, info, num_vars, ncycles, exec.seq, mesh_aloc.allocator(), exec.mpi_type, mesh_aloc.allocator(), exec.mpi_type, mesh_aloc.allocator(), tm, tm_total);

#ifdef COMB_ENABLE_OPENMP
      // if (exec_avail.omp && exec_avail.mpi_type && exec_avail.mpi_type)
      //   do_cycles(con_comm, comminfo, info, num_vars, ncycles, exec.omp, mesh_aloc.allocator(), exec.mpi_type, mesh_aloc.allocator(), exec.mpi_type, mesh_aloc.allocator(), tm, tm_total);
#endif

      // if (exec_avail.cuda && exec_avail.mpi_type && exec_avail.mpi_type)
      //   do_cycles(con_comm, comminfo, info, num_vars, ncycles, exec.cuda, mesh_aloc.allocator(), exec.mpi_type, mesh_aloc.allocator(), exec.mpi_type, mesh_aloc.allocator(), tm, tm_total);
    }
  }
#endif // COMB_ENABLE_CUDA
}

} // namespace COMB

#endif // COMB_ENABLE_MP