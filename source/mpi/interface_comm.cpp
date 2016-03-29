//
//  interface_comm.cpp
//  MPI-Test_01
//
//  Created by Gabriele on 25/03/16.
//  Copyright © 2016 Gabriele. All rights reserved.
//

#include <stdio.h>
#include <mpi.h>
#include <memory>
#include <mpi/interface.h>

namespace mpi_interface
{
    
    
extern const int mpi_any_tag = MPI_ANY_TAG;
extern const int mpi_any_source = MPI_ANY_SOURCE;
mpi_comm mpi_world((size_t)MPI_COMM_WORLD);
mpi_comm mpi_self((size_t)MPI_COMM_SELF);
    
mpi_comm::mpi_comm(size_t type)
{
    m_type = type;
}

int mpi_comm::get_rank()
{
    int world_rank;
    MPI_Comm_rank((MPI_Comm)m_type, &world_rank);
    return world_rank;
}

int mpi_comm::get_size()
{
    int world_size;
    MPI_Comm_size((MPI_Comm)m_type, &world_size);
    return world_size;
}

void mpi_comm::abort(int errorcode)
{
    MPI_Abort((MPI_Comm)m_type, errorcode);
}

bool mpi_comm::prob(int source,
                    int tag,
                    mpi_status& r_status)
{
    //temp
    MPI_Status status;
    //get
    if(MPI_Probe(source,
                 tag,
                 (MPI_Comm)m_type,
                 (MPI_Status*)&status) != MPI_SUCCESS) return false;
    //copy
    r_status.MPI_TAG = status.MPI_TAG;
    r_status.MPI_SOURCE = status.MPI_SOURCE;
    r_status.MPI_ERROR = status.MPI_ERROR;
    r_status.count = status._ucount;
    r_status.cancelled = status._cancelled;
    //true
    return true;
}

bool mpi_comm::send(void* buffer,
                    int count,
                    mpi_handle type,
                    int dest,
                    int tag)
{
    return MPI_Send(buffer,
                    count,
                    (MPI_Datatype)type,
                    dest,
                    tag,
                    (MPI_Comm)m_type) == MPI_SUCCESS;
}

bool mpi_comm::recv(void* buffer,
                    int count,
                    mpi_handle type,
                    int source,
                    int tag)
{
    return
    MPI_Recv(buffer,
             count,
             (MPI_Datatype)type,
             source,
             tag,
             (MPI_Comm)m_type,
             MPI_STATUS_IGNORE) == MPI_SUCCESS;
}

bool mpi_comm::recv(void* buffer,
                    int count,
                    mpi_handle type,
                    int source,
                    int tag,
                    mpi_status& r_status)
{
    return
    MPI_Recv(buffer,
             count,
             (MPI_Datatype)type,
             source,
             tag,
             (MPI_Comm)m_type,
             (MPI_Status*)&r_status) == MPI_SUCCESS;
}


bool mpi_comm::i_prob(int source,
                      int tag,
                      int& flag,
                      mpi_status& r_status)
{
    //temp
    MPI_Status status;
    //get
    if(MPI_Iprobe(source,
                  tag,
                  (MPI_Comm)m_type,
                  &flag,
                  (MPI_Status*)&status) != MPI_SUCCESS) return false;
    //copy
    r_status.MPI_TAG = status.MPI_TAG;
    r_status.MPI_SOURCE = status.MPI_SOURCE;
    r_status.MPI_ERROR = status.MPI_ERROR;
    r_status.count = status._ucount;
    r_status.cancelled = status._cancelled;
    //true
    return true;
}

bool mpi_comm::i_send_lock(void* buffer,
                           int count,
                           mpi_handle type,
                           int dest,
                           int tag)
{
    mpi_request request;
    if(MPI_Isend(buffer,
                 count,
                 (MPI_Datatype)type,
                 dest,
                 tag,
                 (MPI_Comm)m_type,
                 (MPI_Request*)&(request.m_request)) != MPI_SUCCESS) return false;
    return request.wait();
}

bool mpi_comm::i_send(void* buffer,
                      int count,
                      mpi_handle type,
                      int dest,
                      int tag,
                      mpi_request& request)
{
    return
    MPI_Isend(buffer,
              count,
              (MPI_Datatype)type,
              dest,
              tag,
              (MPI_Comm)m_type,
              (MPI_Request*)&(request.m_request)) == MPI_SUCCESS;
}


bool mpi_comm::i_send(void* buffer,
                      int count,
                      mpi_handle type,
                      int dest,
                      int tag,
                      mpi_async_queue& q_request)
{
    return
    MPI_Isend(buffer,
              count,
              (MPI_Datatype)type,
              dest,
              tag,
              (MPI_Comm)m_type,
              (MPI_Request*)&(q_request.get_a_request().m_request)) == MPI_SUCCESS;
}
    
bool mpi_comm::i_recv_lock(void* buffer,
                           int count,
                           mpi_handle type,
                           int source,
                           int tag)
{
    mpi_request request;
    if(MPI_Irecv(buffer,
                 count,
                 (MPI_Datatype)type,
                 source,
                 tag,
                 (MPI_Comm)m_type,
                 (MPI_Request*)&(request.m_request)) != MPI_SUCCESS) return false;
    return request.wait();
}

bool mpi_comm::i_recv(void* buffer,
                      int count,
                      mpi_handle type,
                      int source,
                      int tag,
                      mpi_request& request)
{
    return
    MPI_Irecv(buffer,
              count,
              (MPI_Datatype)type,
              source,
              tag,
              (MPI_Comm)m_type,
              (MPI_Request*)&(request.m_request)) == MPI_SUCCESS;
}

bool mpi_comm::i_recv(void* buffer,
                      int count,
                      mpi_handle type,
                      int source,
                      int tag,
                      mpi_async_queue& q_request)
{
    return
    MPI_Irecv(buffer,
              count,
              (MPI_Datatype)type,
              source,
              tag,
              (MPI_Comm)m_type,
              (MPI_Request*)&(q_request.get_a_request().m_request)) == MPI_SUCCESS;
}
    
};