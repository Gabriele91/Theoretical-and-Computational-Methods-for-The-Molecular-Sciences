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
#include <output_help.h>
#include <mpi/interface.h>


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
//word
auto&     world = mpi::mpi_world;
//type tag
enum tag
{
    TAG_SIZES_TYPES = 1,
    TAG_INIT,
    TAG_POLY,
    TAG_ROOTS,
    TAG_RGBS,
    TAG_KERNEL_ID,
    TAG_ZOOM,
    TAG_MATRIX, 
    TAG_NONE
};


mpi::mpi_handle build_rgb_t()
{
    mpi::mpi_handle handle =
    mpi::type_create_struct({
        mpi::mpi_attr(&rgb_t::r),
        mpi::mpi_attr(&rgb_t::g),
        mpi::mpi_attr(&rgb_t::b)
    });
    mpi::type_commit(handle);
    //get handle
    return handle;
}

mpi::mpi_handle build_init_t()
{
    mpi::mpi_handle handle =
    mpi::type_create_struct({
        mpi::mpi_attr(&init_t::m_origin,2),
        mpi::mpi_attr(&init_t::m_size,  2)
    });
    mpi::type_commit(handle);
    //get handle
    return handle;
}


inline bool master(const argv_parameters&   l_params,
                   const polynomial_t&      l_polynomial,
                   const roots_t&           l_roots,
                   const vector_rgb_t&      l_rgbs,
                   const long               l_kernel_id)
{
    //alias
    const size_t& group_x    = l_params.m_qxp[0];
    const size_t& group_y    = l_params.m_qxp[1];
    const size_t& width      = l_params.m_mxn[0];
    const size_t& height     = l_params.m_mxn[1];
    //local size
    const size_t local_width  = width  / group_x;
    const size_t local_height = height / group_y;
    //build mpi types
    mpi::mpi_handle mpi_init_type = build_init_t();
    mpi::mpi_handle mpi_rgb_type = build_rgb_t();
    //async send/recv
    mpi::mpi_async_queue send_queue;
    //init size
    std::vector< init_t > v_init(group_x*group_y);
    //get time
    mpi::timer  l_timer;
    //send job to all
    for(size_t x=0; x!=group_x; ++x)
    for(size_t y=0; y!=group_y; ++y)
    {
        //id must to be a int
        int worker_id = (int)(x + y*group_x);
        //width & height
        size_t this_width  = local_width;
        size_t this_height = local_height;
        //remaning part
        if(x==group_x-1)  this_width  += width%local_width;
        if(y==group_y-1)  this_height += height%local_height;
        //compute init
        v_init[worker_id]=matrix_t::init_sub_center(//global size
                                                    width,
                                                    height,
                                                    //local size
                                                    x, y,
                                                    local_width,
                                                    local_height,
                                                    //zoom
                                                    l_params.m_zoom);
        //send...
        if(worker_id!=0) world.i_send((void*)&v_init[worker_id],  (int)1, mpi_init_type, worker_id, TAG_INIT, send_queue);
    }
    //init
    matrix_t l_matrix(v_init[0]);
    //kernel
    kernel_t l_kernel = kernels::get_kernel_from_id< value_t >(l_kernel_id);
    //succession
    succession< value_t >
    context
    {
        l_kernel,
        l_params.m_iteration,
        l_polynomial,
        l_roots,
        l_rgbs
    };
    //applay
    l_matrix.applay(context, l_params.m_zoom);
    //wait the other workers
    send_queue.wait();
    //init global matrix
    matrix_t g_matrix(matrix_t::init_t
    {
        { (value_t)0,(value_t)0 },
        { width,        height  }
    });
    //get from master
    assert(g_matrix.get_from(0, 0, l_matrix));
    //clean
    l_matrix.clear();
    //count result
    size_t n_result = v_init.size() - 1;
    //wait the results
    while (n_result)
    {
        //get data
        int arrived { 0 };
        mpi::mpi_status status;
        //from any source
        if(world.i_prob(mpi::mpi_any_source, TAG_MATRIX, arrived, status) && arrived)
        {
            int worker_id = status.get_source();
            //x & y
            size_t m_y = (size_t)(worker_id / group_x  );
            size_t m_x = (size_t)(worker_id - m_y*group_x);
            size_t y = m_y * local_height;
            size_t x = m_x * local_width;
            //info
            #if 0
            std::cout << "recv from: " <<  worker_id
                      << " at "
                      << x
                      << ", "
                      << y
                      << " size "
                      << v_init[worker_id].m_size[0]
                      << ", "
                      << v_init[worker_id].m_size[1]
                      << std::endl;
            #endif
            //local matrix buffer
            matrix_t l_matrix ( v_init[worker_id] );
            //get
            world.i_recv_lock((void*)l_matrix.get_raw_data(), (int)l_matrix.get_raw_size(), mpi_rgb_type, worker_id, TAG_MATRIX);
            //applay
            assert(g_matrix.get_from(x, y, l_matrix));
            //count
            --n_result;
        }
    }
    //save time
    double time_to_complete = l_timer.time_elapsed();
    //output name
    const std::string output_name = build_string_output(l_params,l_polynomial);
    //save metadata
    save_string(output_name+".slb.json", "{ \"time\":"+std::to_string(time_to_complete)+" }");
    //save...
    tga::save_matrix(output_name+".slb.tga", g_matrix);
    //success
    return true;
}


inline bool slave(const argv_parameters&   l_params,
                  const polynomial_t&      l_polynomial,
                  const roots_t&           l_roots,
                  const vector_rgb_t&      l_rgbs,
                  const long               l_kernel_id)
{
    //default root id
    const int worker_id_root = 0;
    //build mpi types
    mpi::mpi_handle mpi_init_type = build_init_t();
    mpi::mpi_handle mpi_rgb_type = build_rgb_t();
    //recv values
    init_t  l_init;
    //recv: GET META DATA
    world.i_recv_lock((void*)&l_init, (int)1, mpi_init_type, worker_id_root, TAG_INIT);
    //init
    matrix_t matrix(l_init);
    //kernel
    kernel_t l_kernel = kernels::get_kernel_from_id< value_t >(l_kernel_id);
    //succession
    succession< value_t >
    context
    {
        l_kernel,
        l_params.m_iteration,
        l_polynomial,
        l_roots,
        l_rgbs
    };
    //applay
    matrix.applay(context, l_params.m_zoom);
    //send result
    world.i_send_lock((void*)matrix.get_raw_data(), (int)matrix.get_raw_size(), mpi_rgb_type, worker_id_root, TAG_MATRIX);
    //end
    return true;
}

inline bool init(int argc,const char* argv[],bool(*task) (const argv_parameters&   l_params,
                                                          const polynomial_t&      l_polynomial,
                                                          const roots_t&           l_roots,
                                                          const vector_rgb_t&      l_rgbs,
                                                          const long               l_kernel_id))
{
    //alloc input parameters
    argv_parameters l_params;
    //pxq required
    l_params.m_require_qxp = true;
    //parse args
    if(!argv_parse(argc, argv, l_params))
    {
        std::cout << l_params.m_errors << std::endl;
        return false;
    }
    //cack size word
    size_t group_size = l_params.m_qxp[0]*l_params.m_qxp[1];
    //bad cases
    if(group_size > world.get_size())
    {
        std::cout << "world is size: "
        <<  world.get_size()
        << ", but you request "
        << group_size;
        return false;
    }
    else if(world.get_rank() >= group_size)
    {
        //do not work
        return true;
    }
    //show help
    if(l_params.m_help)
    {
        std::cout << get_help(l_params) << std::endl;
    }
    //kernel
    long l_kernel_id = kernels::get_kernel_id_from_name< value_t >(l_params.m_kernel);
    //find kernel ?
    if(l_kernel_id < 0)
    {
        std::cout << "kernel: " << l_params.m_kernel << " not found" << std::endl;
        return false;
    }
    //poly
    polynomial_t l_polynomial;
    //parse
    praser_t parser;
    //execute parser
    if(!parser.parse(l_params.m_poly,l_polynomial))
    {
        for(auto& error:parser.get_errors())
        {
            std::cout << error.m_nc << ": " << error.m_error;
        }
        return false;
    }
    //roots
    roots_t      l_roots = roots_utilities::compute(l_polynomial);
    vector_rgb_t l_rgbs  = color::compute_colors< value_t >(l_roots.size());
    //call master
    return task( l_params,
                 l_polynomial,
                 l_roots,
                 l_rgbs,
                 l_kernel_id);
}

int main(int argc,const char** argv)
{
    bool b_success = 0;
    //init
    mpi::init();
    //cases
    if(world.get_rank() == 0)
    {
        b_success = init(argc, argv,master);
    }
    else
    {
        b_success = init(argc, argv, slave);
    }
    //end
    mpi::finalize();
    return b_success ? 0 : 1;
}