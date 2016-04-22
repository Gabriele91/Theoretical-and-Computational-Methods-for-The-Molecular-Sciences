//
//  output_help.h
//  MPI_Project
//
//  Created by Gabriele on 29/03/16.
//  Copyright © 2016 Gabriele. All rights reserved.
//
#pragma once
#include <string>
#include <fstream>
#include <polynomial.h>
#include <argv_parser.h>

template < class T >
inline std::string build_string_output(const argv_parameters& l_params,
                                       const polynomial< T >& l_polynomial)
{
    std::string
    output = std::to_string((unsigned long long)l_params.m_mxn[0]);
    output+= "x";
    output+= std::to_string((unsigned long long)l_params.m_mxn[0]);
    output+= "x";
    output+= std::to_string((unsigned long long)l_polynomial.size());
    output+= "x";
    output+= std::to_string((unsigned long long)l_params.m_iteration);
    output+= "x";
    output+= std::to_string((unsigned long long)l_params.m_factor);
    output+= "x";
    output+= std::to_string((long double)l_params.m_zoom);
    output+= ".";
    output+= type_t_point_str;
    output+= ".";
    output+= l_params.m_kernel;
    
    return output;
}

inline bool save_string(const std::string& path,const std::string& data)
{
    //open
    std::ofstream ofile(path);
    //test
    if(!ofile.is_open()) return false;
    //output
    ofile << data;
    //return...
    return true;
}