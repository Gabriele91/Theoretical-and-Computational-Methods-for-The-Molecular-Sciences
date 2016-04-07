//
//  interface_request.cpp
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
    
mpi_request::mpi_request()
{
    //none
}

bool mpi_request::wait()
{
    bool success =  MPI_Wait((MPI_Request*)&m_request,MPI_STATUS_IGNORE) == MPI_SUCCESS;
    if(success) m_request = nullptr;
    return success;
}

bool mpi_request::wait(mpi_status& status)
{
    bool success =  MPI_Wait((MPI_Request*)&m_request,(MPI_Status*)&status) == MPI_SUCCESS;
    if(success) m_request = nullptr;
    return success;
}

bool mpi_request::cancel()
{
    return MPI_Cancel((MPI_Request*)&m_request) == MPI_SUCCESS;
}

bool mpi_request::free()
{
    bool success = MPI_Request_free((MPI_Request*)&m_request) == MPI_SUCCESS;
    if(success) m_request = nullptr;
    return success;
}

mpi_request::~mpi_request()
{
    if(valid()) free();
}

bool mpi_request::valid() const
{
    return m_request!=nullptr;
}

bool mpi_request::test(int& flag,mpi_status& r_status)
{
    //temp var
    MPI_Status status;
    //test
    bool success = MPI_Test((MPI_Request*)&m_request, &flag, (MPI_Status*)&status) == MPI_SUCCESS;
    //copy
    r_status.MPI_TAG = status.MPI_TAG;
    r_status.MPI_SOURCE = status.MPI_SOURCE;
    r_status.MPI_ERROR = status.MPI_ERROR;
    #ifndef DIABLE_HIDE_MPI_STATUS
    r_status.count = status._ucount;
    r_status.cancelled = status._cancelled;
    #endif
    //is complete
    if(success && flag)
    {
        m_request = nullptr;
    }
    //return
    return success;
}
    
};