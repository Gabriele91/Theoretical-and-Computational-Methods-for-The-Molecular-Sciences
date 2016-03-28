//
//  main-slb.cpp
//  MPI_Project
//
//  Created by Gabriele on 28/03/16.
//  Copyright © 2016 Gabriele. All rights reserved.
//
#include <iostream>
#include <cmath>
#include <cfloat>
#include <complex>
#include <complexmath.h>
#include <config.h>
#include <matrix.h>
#include <kernels.h>
#include <color.h>
#include <tga.h>
#include <succession.h>
#include <parser.h>
#include <argv_parser.h>
#include <mpi/interface.h>


int main(int argc,const char* argv[])
{
    //alloc input parameters
    argv_parameters parameters;
    //parse args
    if(!argv_parse(argc, argv, parameters))
    {
        std::cout << parameters.m_errors << std::endl;
        return -1;
    }
    return 0;
}