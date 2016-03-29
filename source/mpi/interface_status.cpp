//
//  interface_status.cpp
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
    
int mpi_status::get_source() const
{
    return MPI_SOURCE;
}
int mpi_status::get_tag() const
{
    return MPI_TAG;
}
int mpi_status::get_error() const
{
    return MPI_ERROR;
}
size_t mpi_status::get_count() const
{
    return count;
}
int mpi_status::get_count(mpi_handle type) const
{
    //unsafe...
    int type_count { 0 };
    MPI_Get_count(reinterpret_cast<const MPI_Status*>(this), (MPI_Datatype)type, &type_count);;
    return type_count;
}
int mpi_status::get_cancelled() const
{
    return cancelled;
}

};