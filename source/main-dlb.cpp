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

//clamp
inline type_t clamp(type_t value,type_t min,type_t max)
{
    return std::min(std::max(value,min), max);
}
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
    TAG_INIT = 1,
    TAG_MATRIX,
    TAG_END,
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
    //...
    mpi::mpi_handle handle =
    mpi::type_create_struct({
        mpi::mpi_attr(&init_t::m_origin),
        mpi::mpi_attr(&init_t::m_size,2)
    });
    mpi::type_commit(handle);
    //get handle
    return handle;
}

class manager_slaves
{
public:
    
    manager_slaves()
    :m_states(world.get_size())
    {
        assert(m_states.size()>1);
    }
    
    int get_avaliable(long cell_id)
    {
        for ( int worker_id = 1; worker_id < m_states.size() ; ++worker_id)
        {
            if( m_states[worker_id].m_state )
            {
                //became unavaliable
                m_states[worker_id].m_state  = false;
                m_states[worker_id].m_cell_id= cell_id;
                //return id
                return worker_id;
            }
        }
        return -1;
    }
    
    long set_avaliable(int woker_id)
    {
        assert(woker_id);
               m_states[woker_id].m_state = true;
        return m_states[woker_id].m_cell_id;
    }
    
private:
    
    struct woker_state
    {
        bool  m_state   { true };
        long  m_cell_id { -1   };
    };
    std::vector < woker_state > m_states;
    
};


inline bool master(const argv_parameters&   l_params,
                   const polynomial_t&      l_polynomial,
                   const roots_t&           l_roots,
                   const vector_rgb_t&      l_rgbs,
                   const long               l_kernel_id)
{
    //alias
    const size_t& g_group_x  = l_params.m_qxp[0];
    const size_t& g_group_y  = l_params.m_qxp[1];
    const size_t& width      = l_params.m_mxn[0];
    const size_t& height     = l_params.m_mxn[1];
    const type_t& factor     = l_params.m_factor;
    //build mpi types
    mpi::mpi_handle mpi_init_type = build_init_t();
    mpi::mpi_handle mpi_rgb_type  = build_rgb_t();
    //async send/recv
    mpi::mpi_async_queue send_queue;
    //compute sub groups
    const size_t sub_group_x = std::floor( g_group_x / factor );
    const size_t sub_group_y = std::floor( g_group_y / factor );
    //local size
    const size_t local_width  = (width  / sub_group_x);
    const size_t local_height = (height / sub_group_y);
    //init manager
    manager_slaves m_slaves;
    //init size
    std::vector< init_t > task_init(sub_group_x * sub_group_y);
    //id last work sended
    long last_task_sended     { 0                        };
    long count_remaning_task  { (long)(task_init.size()) };
    //get time
    mpi::timer  l_timer;
    //send job to all
    for(size_t x=0; x!=sub_group_x; ++x)
    for(size_t y=0; y!=sub_group_y; ++y)
    {
        //id must to be a int
        long cell_id = (long)(x + y*sub_group_x);
        //width & height
        size_t this_width  = local_width;
        size_t this_height = local_height;
        //remaning part
        if(x==sub_group_x-1)  this_width  += width%local_width;
        if(y==sub_group_y-1)  this_height += height%local_height;
        //compute init
        task_init[cell_id]=
            matrix_t::init_sub_center(//global size
                                      width,
                                      height,
                                      //local size
                                      x, y,
                                      local_width,
                                      local_height,
                                      //zoom
                                      l_params.m_zoom);
    }
    //init global matrix
    matrix_t g_matrix(matrix_t::init_t
                      {
                          { (value_t)0,(value_t)0 },
                          { width,        height  }
                      });
    //send/done work
    while(count_remaning_task)
    {
        //send all task
        if(last_task_sended < task_init.size())
        {
            //get worker
            int worker_id = m_slaves.get_avaliable(last_task_sended);
            //send to
            //is avaliable?
            if(worker_id > 0)
            {
                //send
                world.i_send((void*)&task_init[last_task_sended++],
                             (int)1,
                             mpi_init_type,
                             worker_id,
                             TAG_INIT,
                             send_queue);
            }
        }
        //status
        mpi::mpi_status status;
        //flag
        int msg_arrived = false;
        //from any source
        if(world.i_prob(mpi::mpi_any_source, TAG_MATRIX, msg_arrived, status) && msg_arrived)
        {
            //
            long cell_it = m_slaves.set_avaliable( status.get_source() );
            //x & y
            size_t m_y = (size_t)(cell_it / sub_group_x  );
            size_t m_x = (size_t)(cell_it - m_y*sub_group_x);
            size_t y = m_y * local_height;
            size_t x = m_x * local_width;
            //local matrix buffer
            matrix_t l_matrix ( task_init[cell_it] );
            //get
            world.i_recv_lock((void*)l_matrix.get_raw_data(),
                              (int)  l_matrix.get_raw_size(),
                              mpi_rgb_type,
                              status.get_source(),
                              TAG_MATRIX);
            //applay
            assert(g_matrix.get_from(x, y, l_matrix));
            //dec count
            --count_remaning_task;
        }
    }
    //end event
    bool b_end = true;
    //send to all
    for(int worker_id = 1; worker_id!=world.get_size(); ++worker_id)
    {
        world.i_send((void*)&b_end,
                     (int)1,
                     mpi::type<bool>(),
                     worker_id,
                     TAG_END,
                     send_queue);
    }
    //wait all message sended
    send_queue.wait();
    //save time
    double time_to_complete = l_timer.time_elapsed();
    //output name
    const std::string output_name = build_string_output(l_params,l_polynomial);
    //save metadata
    save_string(output_name+".dlb.json", "{ \"time\":"+std::to_string(time_to_complete)+" }");
    //save...
    tga::save_matrix(output_name+".dlb.tga", g_matrix);
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
    //kernel
    kernel_t l_kernel = kernels::get_kernel_from_id< value_t >(l_kernel_id);
    //succession init
    succession< value_t >
    context
    {
        l_kernel,
        l_params.m_iteration,
        l_polynomial,
        l_roots,
        l_rgbs
    };
    //request end
    bool    b_end{false};
    mpi::mpi_request request_end;
    world.i_recv((void*)&b_end,  (int)1, mpi::type<bool>(), worker_id_root, TAG_END, request_end);
    //request init
    init_t           l_init{0};
    int              b_init { false };
    mpi::mpi_status  status_init;
    mpi::mpi_request request_init;
    world.i_recv((void*)&l_init, (int)1, mpi_init_type, worker_id_root, TAG_INIT, request_init);
    //loop computing
    while(!b_end)
    {
        //test
        request_init.test(b_init, status_init);
        //have a init message
        if(b_init)
        {
            //init
            matrix_t matrix(l_init);
            //applay
            matrix.applay(context, l_params.m_zoom);
            //send result
            world.i_send_lock((void*)matrix.get_raw_data(), (int)matrix.get_raw_size(), mpi_rgb_type, worker_id_root, TAG_MATRIX);
            //recv: GET NEXT DATA
            world.i_recv((void*)&l_init, (int)1, mpi_init_type, worker_id_root, TAG_INIT, request_init);
        }
    }
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
    //factor required
    l_params.m_require_factor = true;
    //parse args
    if(!argv_parse(argc, argv, l_params))
    {
        std::cout << l_params.m_errors << std::endl;
        return false;
    }
    //clamp factor btw [0.0,1.0]
    l_params.m_factor = clamp(l_params.m_factor,type_t(0.0L),type_t(1.0L));
    //bad cases
    if(world.get_size() < 2)
    {
        std::cout << "world is size: "
                  <<  world.get_size()
                  << ", but DLB require 2 or more wokers ";
        return false;
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
    return task(l_params,
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


















