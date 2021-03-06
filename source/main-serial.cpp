//
//  main_newton.cpp
//  MPI-Test_01
//
//  Created by Gabriele on 26/03/16.
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
#include <output_help.h>
#include <mpi/interface.h>
//schroeder
//4000 4000
//"(6+2i)z^0+(3+9i)z^1+(3+78i)z^2+(4+9i)z^3+(8+9i)z^4+z^5+z^8"
//0.001

int main(int argc,const char* argv[])
{
    //types
    using matrix_t     = matrix< type_t >;
    //types easy access
    using init_t       = matrix_t::init_t;
    using value_t      = matrix_t::value_t;
    using complex_t    = matrix_t::complex_t;
    using polynomial_t = polynomial< value_t >;
    using roots_t      = roots< value_t >;
    using rgb_t        = matrix_t::rgb_t;
    using vector_rgb_t = color::vector_rgb< value_t >;
    using praser_t     = parser< value_t >;
    using kernel_t     = kernel< value_t >;
    //mpi types
    namespace mpi   = mpi_interface;
    namespace info  = mpi::mpi_info;
    //world
    auto&     world = mpi::mpi_world;    //init mpi
    mpi::init();
    //if not
    if(!world.get_rank())
    {
        mpi::finalize();
        return 0;
    }
    //alloc input parameters
    argv_parameters parameters;
    //parse args
    if(!argv_parse(argc, argv, parameters))
    {
        std::cout << parameters.m_errors << std::endl;
        return -1;
    }
    //show help
    if(parameters.m_help)
    {
        std::cout << get_help(parameters) << std::endl;
    }
    //kernel
    kernel_t method = kernels::get_kernel_from_name< value_t >(parameters.m_kernel);
    //ceack kernel
    if(!method)
    {
        std::cout << "kernel: " << parameters.m_kernel << " not found" << std::endl;
        return -1;
    }
    //poly
    polynomial_t p_constants;
    //parse
    praser_t parser;
    //execute parser
    if(!parser.parse(parameters.m_poly,p_constants))
    {
        for(auto& error:parser.get_errors())
        {
            std::cout << error.m_nc << ": " << error.m_error;
        }
        return -1;
    }
    //roots
    roots_t      r_results = roots_utilities::compute(p_constants);
    vector_rgb_t r_colors = color::compute_colors< value_t >(r_results.size());
    //get time
    mpi_interface::timer l_timer;
    //init
    matrix_t matrix(matrix_t::init_center(parameters.m_mxn[0],
                                          parameters.m_mxn[1],
                                          parameters.m_zoom));
    //succession
    succession< value_t >
    context
    {
        kernels::schroeder< value_t >,
        parameters.m_iteration,
        p_constants,
        r_results,
        r_colors
    };
    //applay
    matrix.applay(context, parameters.m_zoom);
    //save time
    double time_to_complete = l_timer.time_elapsed();
    //output name
    const std::string output_name = build_string_output(parameters,p_constants);
    //save metadata
    save_string(output_name+".serial.json", "{ \"time\":"+std::to_string((long double)time_to_complete)+" }");
    //save...
    tga::save_matrix(output_name+".serial.tga", matrix);
    //end of MPI
    mpi::finalize();
    //return
    return 0;
}