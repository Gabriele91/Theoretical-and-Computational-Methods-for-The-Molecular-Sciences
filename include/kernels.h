//
//  kernels.h
//  MPI-Test_01
//
//  Created by Gabriele on 27/03/16.
//  Copyright © 2016 Gabriele. All rights reserved.
//
#pragma once
#include <vector>
#include <string>
#include <complexmath.h>
#include <polynomial.h>

template<class T>
using kernel = std::complex<T>(*)(const polynomial< T >&, const std::complex<T>&);

namespace kernels
{
    //kernels
    template< class T >
    inline std::complex< T > newton (const polynomial< T >& constants, const std::complex< T >& x)
    {
        std::complex< T >   vn        = constants[0];
        std::complex< T >   wn        = vn;
        //Horner
        for(size_t i=1;i < constants.size()-1;++i)
        {
            vn = vn*x+constants[i];
            wn = wn*x+vn;
        }
        //last pass
        vn = vn*x+constants[constants.size()-1];
        //newton
        return x-(vn/wn);
    };
    
    //kernels
    template< class T >
    inline std::complex< T > schroeder (const polynomial< T >& constants, const std::complex< T >& x)
    {
        if(constants.size()==0) return 0;
        if(constants.size()==1) return constants[0];
        
        std::complex<T> vn=constants[0];
        std::complex<T> wn=vn;
        std::complex<T> un=wn;
        
        //Horner
        for(size_t i=1;i<(size_t)(constants.size())-2;++i)
        {
            vn = vn*x+constants[i];
            wn = wn*x+vn;
            un = un*x+wn;
        }
        
        vn = vn*x+constants[constants.size()-2];
        wn = wn*x+vn;
        vn = vn*x+constants[constants.size()-1];
        
        //schroeder pass
        auto q=vn/wn;
        auto s1=q+q*q*un/wn;
        
        //return schroeder
        return x-s1;
    };
    
    //get kernel field
    template < class T >
    struct kernel_field
    {
        std::string m_name;
        kernel<T>   m_kernel;
    };
    
    template < class T >
    using kernels_table = std::vector< kernel_field<T> >;
    
    template < class T >
    inline const kernels_table<T>& get_kernels_table()
    {
        static kernels_table<T> kernels_table=
        {
            {"newton"   ,kernels::newton< T >   },
            {"schroeder",kernels::schroeder< T >}
        };
        return kernels_table;
    }
    
    template < class T >
    inline long get_kernel_id_from_name(const std::string& k_name)
    {
        //id count
        long id_count = 0;
        //search
        for(auto& field : get_kernels_table< T >())
        {
            if(field.m_name == k_name) return id_count;
            //count...
            ++id_count;
        }
        return -1;
    }
    
    template < class T >
    inline kernel<T> get_kernel_from_id(long id)
    {
        return  get_kernels_table< T >()[id].m_kernel;
    }
    
    template < class T >
    inline kernel<T> get_kernel_from_name(const std::string& k_name)
    {
        for(auto& field: get_kernels_table< T >())
        {
            if(field.m_name == k_name) return field.m_kernel;
        }
        return nullptr;
    }
    
}
