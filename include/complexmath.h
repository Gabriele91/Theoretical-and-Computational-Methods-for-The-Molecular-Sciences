//
//  complexmath.h
//  MPI-Test_01
//
//  Created by Gabriele on 27/03/16.
//  Copyright © 2016 Gabriele. All rights reserved.
//
#pragma once
#include <complex>

namespace std
{
    template< typename T >  T pi(){ return (T)0; };
    
    template<> float pi<float>();
    template<> double pi<double>();
    template<> long double pi<long double>();
    
    template<> std::complex< float > pi< std::complex< float > >();
    template<> std::complex< double > pi< std::complex< double > >();
    template<> std::complex< long double > pi< std::complex< long double > >();
    
    template < class T >
    inline bool isinf(const std::complex<T>& a)
    {
        return std::isinf(a.real())||std::isinf(a.imag());
    }
    template < class T >
    inline bool isnan(const std::complex<T>& a)
    {
        return std::isnan(a.real())||std::isnan(a.imag());
    }
    template < class T >
    inline bool complex_distance(const std::complex<T>& left,const std::complex<T>& right,T max_distance)
    {
        return std::abs(left-right) < max_distance;
    }
}