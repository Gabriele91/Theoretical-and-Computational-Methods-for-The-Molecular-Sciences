//
//  argv_parser.h
//  MPI-Test_01
//
//  Created by Gabriele on 28/03/16.
//  Copyright © 2016 Gabriele. All rights reserved.
//
#pragma once
#include "config.h"
#include <string>
#include <sstream>
#include <cstdlib>
struct argv_parameters
{
    size_t          m_mxn[2]   { 0, 0         };
    size_t          m_qxp[2]   { 0, 0         };
    size_t          m_iteration{ 0            };
    type_t          m_zoom     { (type_t)1.0L };
    bool            m_help     { false        };
    std::string     m_app;
    std::string     m_poly;
    std::string     m_kernel;
    //errors
    bool            m_success   { true        };
    std::string     m_errors;
};


inline bool argv_parse(int argc,const char* argv[],argv_parameters& output)
{
    //invalid input
    if(argc < 1) return false;
    //appname
    output.m_app = argv[0];
    //C++ args
    std::vector< std::string > v_args;
    //required attrs
    bool b_mxn      { false };
    bool b_kernel   { false };
    bool b_poly     { false };
    bool b_iteration{ false };
    //alloc args
    v_args.resize(argc-1);
    //copy args
    for(size_t i=1;i < argc; ++i)
    {
        v_args[i-1]=argv[i];
    }
    //for all args
    for(size_t i=0;i != v_args.size(); ++i)
    {
        if(v_args[i]=="-h" || v_args[i]=="--help")
        {
            output.m_help = true;
        }
        else if( v_args[i]=="-m" || v_args[i]=="--mxn" )
        {
            if(i+2 >= v_args.size())
            {
                output.m_success = false;
                output.m_errors  = "not valid input["+ std::to_string(i) +"]: " + v_args[i] + "\n";
                return false;
            }
            output.m_mxn[0] = std::strtoul(v_args[++i].c_str(),nullptr,10);
            output.m_mxn[1] = std::strtoul(v_args[++i].c_str(),nullptr,10);
            //is added
            b_mxn = true;
        }
        else if( v_args[i]=="-q" || v_args[i]=="--qxp" )
        {
            if(i+2 >= v_args.size())
            {
                output.m_success = false;
                output.m_errors  = "not valid input["+ std::to_string(i) +"]: " + v_args[i] + "\n";
                return false;
            }
            output.m_qxp[0] = std::strtoul(v_args[++i].c_str(),nullptr,10);
            output.m_qxp[1] = std::strtoul(v_args[++i].c_str(),nullptr,10);
        }
        else if( v_args[i]=="-p" || v_args[i]=="--polynomial" )
        {
            if(i+1 >= v_args.size())
            {
                output.m_success = false;
                output.m_errors  = "not valid input["+ std::to_string(i) +"]: " + v_args[i] + "\n";
                return false;
            }
            output.m_poly = v_args[++i];
            //is added
            b_poly = true;
        }
        else if( v_args[i]=="-k" || v_args[i]=="--kernel" )
        {
            if(i+1 >= v_args.size())
            {
                output.m_success = false;
                output.m_errors  = "not valid input["+ std::to_string(i) +"]: " + v_args[i] + "\n";
                return false;
            }
            output.m_kernel = v_args[++i];
            //is added
            b_kernel = true;
        }
        else if( v_args[i]=="-i" || v_args[i]=="--iteration" )
        {
            if(i+1 >= v_args.size())
            {
                output.m_success = false;
                output.m_errors  = "not valid input["+ std::to_string(i) +"]: " + v_args[i] + "\n";
                return false;
            }
            output.m_iteration = std::strtoul(v_args[++i].c_str(),nullptr,10);
            //is added
            b_iteration = true;
        }
        else if( v_args[i]=="-z" || v_args[i]=="--zoom" )
        {
            if(i+1 >= v_args.size())
            {
                output.m_success = false;
                output.m_errors  = "not valid input["+ std::to_string(i) +"]: " + v_args[i] + "\n";
                return false;
            }
            output.m_zoom = std::strtod(v_args[++i].c_str(),nullptr);
        }
        else
        {
            output.m_success = false;
            output.m_errors  = "not valid input["+ std::to_string(i) +"]: " + v_args[i] + "\n";
            return false;
        }
    }
    
    
    if(!b_poly)
    {
        output.m_success = false;
        output.m_errors += "--polynomial is required\n" ;
    }
    
    if(!b_kernel)
    {
        output.m_success = false;
        output.m_errors += "--kernel is required\n" ;
    }

    if(!b_iteration)
    {
        output.m_success = false;
        output.m_errors += "--iteration is required\n" ;
    }
    
    if(!b_mxn)
    {
        output.m_success = false;
        output.m_errors += "--mxn is required\n" ;
    }
    
    return output.m_success;
}

inline std::string get_help(const argv_parameters& params)
{
    std::stringstream stream;
    stream << params.m_app << " [options] \n";
    stream <<
    "Options:                                                                \n"
    "\t--help/-h                       help                                  \n"
    "\t--polynomial/-p <expression>    polynomial expression       [required]\n"
    "\t--kernel/-k  <kernel name>      resolution method           [required]\n"
    "\t--iteration/-i <number>         number of iteration         [required]\n"
    "\t--mxn/-m  <width> <height>      matrix resolution           [required]\n"
    "\t--qxp/-q  <width> <height>      matrix subdivision                    \n"
    "\t--zoom/-z <zoom factor>         zoom factor into center area          \n";
    
    return stream.str();
}