//
//  interface_timer.cpp
//  MPI_Project
//
//  Created by Gabriele on 29/03/16.
//  Copyright © 2016 Gabriele. All rights reserved.
//

#include <stdio.h>
#include <mpi/interface.h>

namespace mpi_interface
{
    void timer::reset()
    {
        m_start = worker_time();
    }
    double timer::time_elapsed() const
    {
        return worker_time()-m_start;
    }
};