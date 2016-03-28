//
//  config.h
//  MPI-Test_01
//
//  Created by Gabriele on 28/03/16.
//  Copyright © 2016 Gabriele. All rights reserved.
//
#pragma once

#if defined( USE_FLOAT )
    using type_t = float;
    static const char type_t_str[] = "float";
    static const char type_t_point_str[] = "float";
#elif defined( USE_DOUBLE )
    using type_t = double;
    static const char type_t_str[] = "double";
    static const char type_t_point_str[] = "double";
#elif defined( USE_LONG_DOUBLE )
    using type_t = long double;
    static const char type_t_str[] = "long double";
    static const char type_t_point_str[] = "long.double";
#else
    #error define default type
#endif
