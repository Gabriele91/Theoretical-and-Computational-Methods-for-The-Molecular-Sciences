//
//  complexmath.cpp
//  MPI-Test_01
//
//  Created by Gabriele on 27/03/16.
//  Copyright © 2016 Gabriele. All rights reserved.
//
#include <math.h>
#include <complexmath.h>

template<> float       std::pi<float>()      { return 3.14159265358979323846f; };
template<> double      std::pi<double>()     { return 3.14159265358979323846;  };
template<> long double std::pi<long double>(){ return 3.14159265358979323846L; };

template<> std::complex< float >       std::pi< std::complex< float > >()       { return std::complex< float >      { std::pi<float>()       }; };
template<> std::complex< double >      std::pi< std::complex< double > >()      { return std::complex< double >     { std::pi<double>()      }; };
template<> std::complex< long double > std::pi< std::complex< long double > >() { return std::complex< long double >{ std::pi<long double>() }; };