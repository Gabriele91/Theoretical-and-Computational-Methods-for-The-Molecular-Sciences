//
//  interface_async_queue.cpp
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
    
void mpi_async_queue::append(mpi_task task)
{
    mpi_request *request_ptr = new mpi_request;
    assert(task(*request_ptr));
    m_queue.push_back(mpi_request_uptr{ request_ptr });
}

void mpi_async_queue::wait()
{
    for(auto& request : m_queue) request->wait();
    m_queue.clear();
}

void mpi_async_queue::cancel()
{
    for(auto& request : m_queue) request->cancel();
}

void mpi_async_queue::free()
{
    for(auto& request : m_queue) request->free();
    m_queue.clear();
}


//get request to append
mpi_request& mpi_async_queue::get_a_request()
{
    mpi_request *request_ptr = new mpi_request;
    m_queue.push_back(mpi_request_uptr{ request_ptr });
    return *request_ptr;
}
    
};