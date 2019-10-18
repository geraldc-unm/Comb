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

#include "comm_pol_mpi.hpp"
#include "do_cycles.hpp"

namespace COMB {

void do_cycles_basic(CommInfo& comm_info, MeshInfo& info,
                     COMB::ExecContexts& exec,
                     COMB::Allocators& alloc,
                     COMB::ExecutorsAvailable& /* exec_avail */,
                     IdxT num_vars, IdxT ncycles, Timer& tm, Timer& tm_total)
{
  using pol_comm = mpi_pol;
  using pol_mesh = seq_pol;
  using pol_many = seq_pol;
  using pol_few  = seq_pol;

  static_assert(std::is_same<pol_many, pol_few>::value, "do_cycles_basic expects pol_many and pol_few to be the same");
  static_assert(std::is_same<pol_many, seq_pol>::value, "do_cycles_basic expects pol_many to be seq_pol");

  CommContext<pol_comm> con_comm_in{exec.base_mpi};

  ExecContext<pol_mesh>& con_mesh = exec.seq;
  COMB::Allocator& aloc_mesh = alloc.host.allocator();

  ExecContext<pol_many>& con_many = exec.seq;
  COMB::Allocator& aloc_many = alloc.host.allocator();

  ExecContext<pol_few>& con_few = exec.seq;
  COMB::Allocator& aloc_few = alloc.host.allocator();

  ExecContext<seq_pol> tm_con;
  tm_total.clear();
  tm.clear();

  char test_name[1024] = ""; snprintf(test_name, 1024, "Basic\nComm %s Mesh %s %s Buffers %s %s %s %s",
                                                        pol_comm::get_name(),
                                                        pol_mesh::get_name(), aloc_mesh.name(),
                                                        pol_many::get_name(), aloc_many.name(), pol_few::get_name(), aloc_few.name());
  fgprintf(FileGroup::all, "Starting test %s\n", test_name);

  {
    Range r0(test_name, Range::orange);

    // make a copy of comminfo to duplicate the MPI communicator
    CommInfo comminfo(comm_info);

    using comm_type = Comm<pol_many, pol_few, pol_comm>;
    using message_type = typename comm_type::message_type;
    using policy_comm  = typename comm_type::policy_comm;

    // set name of communicator
    // include name of memory space if using mpi datatypes for pack/unpack
    char comm_name[MPI_MAX_OBJECT_NAME] = "";
    snprintf(comm_name, MPI_MAX_OBJECT_NAME, "COMB_MPI_CART_COMM%s%s",
        (comm_type::use_mpi_type) ? "_"              : "",
        (comm_type::use_mpi_type) ? aloc_mesh.name() : "");

    comminfo.set_name(comm_name);

    CommContext<pol_comm> con_comm(con_comm_in, comminfo.cart.comm);

    // if policies are the same set cutoff to 0 (always use pol_many) to simplify algorithms
    if (std::is_same<pol_many, pol_few>::value) {
      comminfo.cutoff = 0;
    }

    // make communicator object
    comm_type comm(con_comm, comminfo, aloc_mesh, aloc_many, aloc_few);

    comm.barrier();

    tm_total.start(tm_con, "start-up");

    std::vector<MeshData> vars;
    vars.reserve(num_vars);

    {
      CommFactory factory(comminfo);

      for (IdxT i = 0; i < num_vars; ++i) {

        vars.push_back(MeshData(info, aloc_mesh));

        vars[i].allocate();

        DataT* data = vars[i].data();
        IdxT totallen = info.totallen;

        con_mesh.for_all(0, totallen,
                            detail::set_n1(data));

        factory.add_var(vars[i]);

        con_mesh.synchronize();
      }

      factory.populate(comm, con_many, con_few);
    }

    // do_cycles_basic expects that all sends and receives have_many (use pol_many)
    for (message_type& recv : comm.m_recvs) {
      assert(recv.have_many()) ;
    }
    for (message_type& send : comm.m_sends) {
      assert(send.have_many()) ;
    }

    tm_total.stop(tm_con);

   /**************************************************************************
    **************************************************************************
    *
    * Perform test communication steps to ensure communication gives
    * the right answer
    *
    **************************************************************************
    **************************************************************************/

    comm.barrier();

    Range r1("test comm", Range::indigo);

    tm_total.start(tm_con, "test-comm");

    IdxT ntestcycles = std::max(IdxT{1}, ncycles/IdxT{10});
    for (IdxT test_cycle = 0; test_cycle < ntestcycles; ++test_cycle) { // test comm

      Range r2("cycle", Range::cyan);

      bool mock_communication = comm.mock_communication();
      IdxT imin = info.min[0];
      IdxT jmin = info.min[1];
      IdxT kmin = info.min[2];
      IdxT imax = info.max[0];
      IdxT jmax = info.max[1];
      IdxT kmax = info.max[2];
      IdxT ilen = info.len[0];
      IdxT jlen = info.len[1];
      IdxT klen = info.len[2];
      IdxT iglobal_offset = info.global_offset[0];
      IdxT jglobal_offset = info.global_offset[1];
      IdxT kglobal_offset = info.global_offset[2];
      IdxT ilen_global = info.global.sizes[0];
      IdxT jlen_global = info.global.sizes[1];
      IdxT klen_global = info.global.sizes[2];
      IdxT iperiodic = info.global.periodic[0];
      IdxT jperiodic = info.global.periodic[1];
      IdxT kperiodic = info.global.periodic[2];
      IdxT ighost_width = info.ghost_widths[0];
      IdxT jghost_width = info.ghost_widths[1];
      IdxT kghost_width = info.ghost_widths[2];
      IdxT ijlen = info.stride[2];
      IdxT ijlen_global = ilen_global * jlen_global;


      Range r3("pre-comm", Range::red);
      // tm.start(tm_con, "pre-comm");

      for (IdxT i = 0; i < num_vars; ++i) {

        DataT* data = vars[i].data();
        IdxT var_i = i + 1;

        con_mesh.for_all_3d(0, klen,
                               0, jlen,
                               0, ilen,
                               [=] COMB_HOST COMB_DEVICE (IdxT k, IdxT j, IdxT i, IdxT idx) {
          COMB::ignore_unused(idx);
          IdxT zone = i + j * ilen + k * ijlen;
          IdxT iglobal = i + iglobal_offset;
          if (iperiodic) {
            iglobal = iglobal % ilen_global;
            if (iglobal < 0) iglobal += ilen_global;
          }
          IdxT jglobal = j + jglobal_offset;
          if (jperiodic) {
            jglobal = jglobal % jlen_global;
            if (jglobal < 0) jglobal += jlen_global;
          }
          IdxT kglobal = k + kglobal_offset;
          if (kperiodic) {
            kglobal = kglobal % klen_global;
            if (kglobal < 0) kglobal += klen_global;
          }
          IdxT zone_global = iglobal + jglobal * ilen_global + kglobal * ijlen_global;
          DataT expected, found, next;
          int branchid = -1;
          if (k >= kmin+kghost_width && k < kmax-kghost_width &&
              j >= jmin+jghost_width && j < jmax-jghost_width &&
              i >= imin+ighost_width && i < imax-ighost_width) {
            // interior non-communicated zones
            expected = -1.0; found = data[zone]; next =-(zone_global+var_i);
            branchid = 0;
          } else if (k >= kmin && k < kmax &&
                     j >= jmin && j < jmax &&
                     i >= imin && i < imax) {
            // interior communicated zones
            expected = -1.0; found = data[zone]; next = zone_global + var_i;
            branchid = 1;
          } else if (iglobal < 0 || iglobal >= ilen_global ||
                     jglobal < 0 || jglobal >= jlen_global ||
                     kglobal < 0 || kglobal >= klen_global) {
            // out of global bounds exterior zones, some may be owned others not
            // some may be communicated if at least one dimension is periodic
            // and another is non-periodic
            expected = -1.0; found = data[zone]; next = zone_global + var_i;
            branchid = 2;
          } else {
            // in global bounds exterior zones
            expected = -1.0; found = data[zone]; next =-(zone_global+var_i);
            branchid = 3;
          }
          if (!mock_communication) {
            if (found != expected) {
              FGPRINTF(FileGroup::proc, "%p %i zone %i(%i %i %i) g%i(%i %i %i) = %f expected %f next %f\n", data, branchid, zone, i, j, k, zone_global, iglobal, jglobal, kglobal, found, expected, next);
            }
            // FGPRINTF(FileGroup::proc, "%p[%i] = %f\n", data, zone, 1.0);
            assert(found == expected);
          }
          data[zone] = next;
        });
      }

      con_mesh.synchronize();

      // tm.stop(tm_con);
      r3.restart("post-recv", Range::pink);
      // tm.start(tm_con, "post-recv");

     /************************************************************************
      *
      * Allocate receive buffers and post receives
      *
      ************************************************************************/
      comm.postRecv(con_many, con_few);

      // tm.stop(tm_con);
      r3.restart("post-send", Range::pink);
      // tm.start(tm_con, "post-send");

     /************************************************************************
      *
      * Allocate send buffers, pack, and post sends
      *
      ************************************************************************/
      comm.postSend(con_many, con_few);

      // tm.stop(tm_con);
      r3.stop();

      // for (IdxT i = 0; i < num_vars; ++i) {

      //   DataT* data = vars[i].data();
      //   IdxT var_i = i + 1;

      //   con_mesh.for_all_3d(0, klen,
      //                          0, jlen,
      //                          0, ilen,
      //                          [=] COMB_HOST COMB_DEVICE (IdxT k, IdxT j, IdxT i, IdxT idx) {
      //     COMB::ignore_unused(idx);
      //     IdxT zone = i + j * ilen + k * ijlen;
      //     IdxT iglobal = i + iglobal_offset;
      //     if (iperiodic) {
      //       iglobal = iglobal % ilen_global;
      //       if (iglobal < 0) iglobal += ilen_global;
      //     }
      //     IdxT jglobal = j + jglobal_offset;
      //     if (jperiodic) {
      //       jglobal = jglobal % jlen_global;
      //       if (jglobal < 0) jglobal += jlen_global;
      //     }
      //     IdxT kglobal = k + kglobal_offset;
      //     if (kperiodic) {
      //       kglobal = kglobal % klen_global;
      //       if (kglobal < 0) kglobal += klen_global;
      //     }
      //     IdxT zone_global = iglobal + jglobal * ilen_global + kglobal * ijlen_global;
      //     DataT expected, found, next;
      //     int branchid = -1;
      //     if (k >= kmin+kghost_width && k < kmax-kghost_width &&
      //         j >= jmin+jghost_width && j < jmax-jghost_width &&
      //         i >= imin+ighost_width && i < imax-ighost_width) {
      //       // interior non-communicated zones should not have changed value
      //       expected =-(zone_global+var_i); found = data[zone]; next = -1.0;
      //       branchid = 0;
      //       if (!mock_communication) {
      //         if (found != expected) {
      //           FGPRINTF(FileGroup::proc, "%p %i zone %i(%i %i %i) g%i(%i %i %i) = %f expected %f next %f\n", data, branchid, zone, i, j, k, zone_global, iglobal, jglobal, kglobal, found, expected, next);
      //         }
      //         // FGPRINTF(FileGroup::proc, "%p[%i] = %f\n", data, zone, 1.0);
      //         assert(found == expected);
      //       }
      //       data[zone] = next;
      //     }
      //     // other zones may be participating in communication, do not access
      //   });
      // }

      // con_mesh.synchronize();


      r3.start("wait-recv", Range::pink);
      // tm.start(tm_con, "wait-recv");

     /************************************************************************
      *
      * Wait on receives, unpack, and deallocate receive buffers
      *
      ************************************************************************/
      comm.waitRecv(con_many, con_few);

      // tm.stop(tm_con);
      r3.restart("wait-send", Range::pink);
      // tm.start(tm_con, "wait-send");

     /************************************************************************
      *
      * Wait on sends and deallocate send buffers
      *
      ************************************************************************/
      comm.waitSend(con_many, con_few);

      // tm.stop(tm_con);
      r3.restart("post-comm", Range::red);
      // tm.start(tm_con, "post-comm");

      for (IdxT i = 0; i < num_vars; ++i) {

        DataT* data = vars[i].data();
        IdxT var_i = i + 1;

        con_mesh.for_all_3d(0, klen,
                               0, jlen,
                               0, ilen,
                               [=] COMB_HOST COMB_DEVICE (IdxT k, IdxT j, IdxT i, IdxT idx) {
          COMB::ignore_unused(idx);
          IdxT zone = i + j * ilen + k * ijlen;
          IdxT iglobal = i + iglobal_offset;
          if (iperiodic) {
            iglobal = iglobal % ilen_global;
            if (iglobal < 0) iglobal += ilen_global;
          }
          IdxT jglobal = j + jglobal_offset;
          if (jperiodic) {
            jglobal = jglobal % jlen_global;
            if (jglobal < 0) jglobal += jlen_global;
          }
          IdxT kglobal = k + kglobal_offset;
          if (kperiodic) {
            kglobal = kglobal % klen_global;
            if (kglobal < 0) kglobal += klen_global;
          }
          IdxT zone_global = iglobal + jglobal * ilen_global + kglobal * ijlen_global;
          DataT expected, found, next;
          int branchid = -1;
          if (k >= kmin+kghost_width && k < kmax-kghost_width &&
              j >= jmin+jghost_width && j < jmax-jghost_width &&
              i >= imin+ighost_width && i < imax-ighost_width) {
            // interior non-communicated zones should not have changed value
            expected =-(zone_global+var_i); found = data[zone]; next = -1.0;
            branchid = 0;
          } else if (k >= kmin && k < kmax &&
                     j >= jmin && j < jmax &&
                     i >= imin && i < imax) {
            // interior communicated zones should not have changed value
            expected = zone_global + var_i; found = data[zone]; next = -1.0;
            branchid = 1;
          } else if (iglobal < 0 || iglobal >= ilen_global ||
                     jglobal < 0 || jglobal >= jlen_global ||
                     kglobal < 0 || kglobal >= klen_global) {
            // out of global bounds exterior zones should not have changed value
            // some may have been communicated, but values should be the same
            expected = zone_global + var_i; found = data[zone]; next = -1.0;
            branchid = 2;
          } else {
            // in global bounds exterior zones should have changed value
            // should now be populated with data from another rank
            expected = zone_global + var_i; found = data[zone]; next = -1.0;
            branchid = 3;
          }
          if (!mock_communication) {
            if (found != expected) {
              FGPRINTF(FileGroup::proc, "%p %i zone %i(%i %i %i) g%i(%i %i %i) = %f expected %f next %f\n", data, branchid, zone, i, j, k, zone_global, iglobal, jglobal, kglobal, found, expected, next);
            }
            // FGPRINTF(FileGroup::proc, "%p[%i] = %f\n", data, zone, 1.0);
            assert(found == expected);
          }
          data[zone] = next;
        });
      }

      con_mesh.synchronize();

      // tm.stop(tm_con);
      r3.stop();

      r2.stop();
    }

    comm.barrier();

    tm_total.stop(tm_con);

    tm.clear();


   /**************************************************************************
    **************************************************************************
    *
    * Perform "real" communication steps to benchmark performance
    *
    **************************************************************************
    **************************************************************************/

    r1.restart("bench comm", Range::magenta);

    tm_total.start(tm_con, "bench-comm");

    for(IdxT cycle = 0; cycle < ncycles; cycle++) {

      Range r2("cycle", Range::yellow);

      IdxT imin = info.min[0];
      IdxT jmin = info.min[1];
      IdxT kmin = info.min[2];
      IdxT imax = info.max[0];
      IdxT jmax = info.max[1];
      IdxT kmax = info.max[2];
      IdxT ilen = info.len[0];
      IdxT jlen = info.len[1];
      IdxT klen = info.len[2];
      IdxT ijlen = info.stride[2];


      Range r3("pre-comm", Range::red);
      tm.start(tm_con, "pre-comm");

      for (IdxT i = 0; i < num_vars; ++i) {

        DataT* data = vars[i].data();

        con_mesh.for_all_3d(kmin, kmax,
                            jmin, jmax,
                            imin, imax,
                            detail::set_1(ilen, ijlen, data));
      }

      con_mesh.synchronize();

      tm.stop(tm_con);
      r3.restart("post-recv", Range::pink);
      tm.start(tm_con, "post-recv");

     /************************************************************************
      *
      * Allocate receive buffers and post receives
      *
      ************************************************************************/
      // comm.postRecv(con_many, con_few);
      {
        // FGPRINTF(FileGroup::proc, "posting receives\n");

        IdxT num_recvs = comm.m_recvs.size();

        comm.m_recv_requests.resize(num_recvs, comm.con_comm.recv_request_null());

        for (IdxT i = 0; i < num_recvs; ++i) {

          comm.m_recvs[i].allocate(comm.m_recv_contexts_many[i], comm.con_comm, comm.many_aloc);
          comm.m_recvs[i].Irecv(comm.m_recv_contexts_many[i], comm.con_comm, &comm.m_recv_requests[i]);
        }
      }

      tm.stop(tm_con);
      r3.restart("post-send", Range::pink);
      tm.start(tm_con, "post-send");

     /************************************************************************
      *
      * Allocate send buffers, pack, and post sends
      *
      ************************************************************************/
      // comm.postSend(con_many, con_few);
      {
        // FGPRINTF(FileGroup::proc, "posting sends\n");

        const IdxT num_sends = comm.m_sends.size();

        comm.m_send_requests.resize(num_sends, comm.con_comm.send_request_null());

        for (IdxT i = 0; i < num_sends; ++i) {

          comm.m_sends[i].allocate(comm.m_send_contexts_many[i], comm.con_comm, comm.many_aloc);
          comm.m_sends[i].pack(comm.m_send_contexts_many[i], comm.con_comm);
          message_type::wait_pack_complete(con_many, comm.con_comm);
          comm.m_sends[i].Isend(comm.m_send_contexts_many[i], comm.con_comm, &comm.m_send_requests[i]);
        }
      }

      tm.stop(tm_con);
      r3.stop();

      /*
      for (IdxT i = 0; i < num_vars; ++i) {

        DataT* data = vars[i].data();

        con_mesh.for_all_3d(0, klen,
                               0, jlen,
                               0, ilen,
                               [=] COMB_HOST COMB_DEVICE (IdxT k, IdxT j, IdxT i, IdxT idx) {
          IdxT zone = i + j * ilen + k * ijlen;
          DataT expected, found, next;
          if (k >= kmin && k < kmax &&
              j >= jmin && j < jmax &&
              i >= imin && i < imax) {
            expected = 1.0; found = data[zone]; next = 1.0;
          } else {
            expected = -1.0; found = data[zone]; next = -1.0;
          }
          // if (found != expected) {
          //   FGPRINTF(FileGroup::proc, "zone %i(%i %i %i) = %f expected %f\n", zone, i, j, k, found, expected);
          // }
          //FGPRINTF(FileGroup::proc, "%p[%i] = %f\n", data, zone, 1.0);
          data[zone] = next;
        });
      }
      */

      r3.start("wait-recv", Range::pink);
      tm.start(tm_con, "wait-recv");

     /************************************************************************
      *
      * Wait on receives, unpack, and deallocate receive buffers
      *
      ************************************************************************/
      // comm.waitRecv(con_many, con_few);
      {
        // FGPRINTF(FileGroup::proc, "waiting receives\n");

        IdxT num_recvs = comm.m_recvs.size();

        typename policy_comm::recv_status_type status = comm.con_comm.recv_status_null();

        IdxT num_done = 0;
        while (num_done < num_recvs) {

          IdxT idx = message_type::wait_recv_any(comm.con_comm, num_recvs, &comm.m_recv_requests[0], &status);

          comm.m_recvs[idx].unpack(comm.m_recv_contexts_many[idx], comm.con_comm);
          comm.m_recvs[idx].deallocate(comm.m_recv_contexts_many[idx], comm.con_comm, comm.many_aloc);

          num_done += 1;

        }

        comm.m_recv_requests.clear();

        con_many.synchronize();
      }

      tm.stop(tm_con);
      r3.restart("wait-send", Range::pink);
      tm.start(tm_con, "wait-send");

     /************************************************************************
      *
      * Wait on sends and deallocate send buffers
      *
      ************************************************************************/
      // comm.waitSend(con_many, con_few);
      {
        // FGPRINTF(FileGroup::proc, "posting sends\n");

        IdxT num_sends = comm.m_sends.size();

        typename policy_comm::send_status_type status = comm.con_comm.send_status_null();

        IdxT num_done = 0;
        while (num_done < num_sends) {

          IdxT idx = message_type::wait_send_any(comm.con_comm, num_sends, &comm.m_send_requests[0], &status);

          comm.m_sends[idx].deallocate(comm.m_send_contexts_many[idx], comm.con_comm, comm.many_aloc);

          num_done += 1;
        }

        comm.m_send_requests.clear();
      }

      tm.stop(tm_con);
      r3.restart("post-comm", Range::red);
      tm.start(tm_con, "post-comm");

      for (IdxT i = 0; i < num_vars; ++i) {

        DataT* data = vars[i].data();

        con_mesh.for_all_3d(0, klen,
                               0, jlen,
                               0, ilen,
                               detail::reset_1(ilen, ijlen, data, imin, jmin, kmin, imax, jmax, kmax));
      }

      con_mesh.synchronize();

      tm.stop(tm_con);
      r3.stop();

      r2.stop();

    }

    comm.barrier();

    tm_total.stop(tm_con);

    r1.stop();

    print_timer(comminfo, tm);
    print_timer(comminfo, tm_total);

    comm.depopulate();
  }

  tm.clear();
  tm_total.clear();

  // print_proc_memory_stats(comminfo);
}

} // namespace COMB