//
//  succession.h
//  MPI-Test_01
//
//  Created by Gabriele on 27/03/16.
//  Copyright © 2016 Gabriele. All rights reserved.
//
#pragma once
#include <limits>
#include <color.h>
#include <kernels.h>
#include <matrix.h>
#include <polynomial.h>

template < typename T >
std::function< color::rgb<T>(const std::complex<T>&,const std::complex<T>&) >
succession_gen(const kernel< T >&            l_kernel,
               const size_t&                 l_n_iteration,
               const polynomial< T >&        l_polynomial,
                     roots< T >&             l_results,
                     color::vector_rgb< T >& l_colors )
{
    
    return [&](const std::complex<T>& origin,const std::complex<T>& pos) -> color::rgb<T>
    {
        //values
        size_t          n = l_n_iteration;
        std::complex<T> x = origin+pos;
        std::complex<T> x_next;
        //epsilon
        const T epsilon = std::numeric_limits< T >::epsilon()*(T)(10.0L);
        //n...
        while (--n)
        {
            //pass
            x_next = l_kernel(l_polynomial,x);
            //escape cases
            if( std::complex_distance(x, x_next, epsilon) || std::isinf(x_next) )
            {
                return color::complex_near<T> ( x_next,
                                               l_results,
                                               l_colors,
                                               epsilon) * (((T)n)/l_n_iteration);
            }
            //next pass
            x=x_next;
        }
        
        return color::rgb<T>{ 0,0,0 };
    };
}

template < typename T >
class succession : public matrix< T >::abstract_succession
{
public:
    
    const kernel< T >&            m_kernel;
    const size_t&                 m_n_iteration;
    const polynomial< T >&        m_polynomial;
    const roots< T >&             m_results;
    const color::vector_rgb< T >& m_colors;
    
    succession(const kernel< T >&            l_kernel,
               const size_t&                 l_n_iteration,
               const polynomial< T >&        l_polynomial,
               const roots< T >&             l_results,
               const color::vector_rgb< T >& l_colors )
    :m_kernel(l_kernel)
    ,m_n_iteration(l_n_iteration)
    ,m_polynomial(l_polynomial)
    ,m_results(l_results)
    ,m_colors(l_colors)
    {
        
    }
    
    typename matrix< T >::rgb_t pass(const typename matrix< T >::complex_t& origin,
                                     const typename matrix< T >::complex_t& pos) const
    {
        //values
        size_t          n = m_n_iteration;
        std::complex<T> x = origin+pos;
        std::complex<T> x_next;
        //epsilon
        static const T epsilon = std::numeric_limits< T >::epsilon()*(T)(10.0L);
        //n...
        while (--n)
        {
            //pass
            x_next = m_kernel(m_polynomial,x);
            //escape cases
            if( std::complex_distance(x, x_next, epsilon) || std::isinf(x_next) )
            {
                return color::complex_near<T> (x_next,
                                               m_results,
                                               m_colors,
                                               epsilon) * (((T)n)/m_n_iteration);
            }
            //next pass
            x=x_next;
        }
        
        return color::rgb<T>{ 0,0,0 };
    };
};