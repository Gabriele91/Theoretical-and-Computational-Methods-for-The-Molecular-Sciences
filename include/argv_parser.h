//
//  argv_parser.h
//  MPI-Test_01
//
//  Created by Gabriele on 28/03/16.
//  Copyright © 2016 Gabriele. All rights reserved.
//
#pragma once
#include <config.h>
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
    //required
    bool            m_require_mxn      { true  };
    bool            m_require_qxp      { false };
    bool            m_require_iteration{ true  };
    bool            m_require_zoom     { false };
    bool            m_require_poly     { true  };
    bool            m_require_kernel   { true  };
};


inline bool argv_parse(int argc,const char* argv[],
                       argv_parameters& output)
{
    //invalid input
    if(argc < 1) return false;
    //appname
    output.m_app = argv[0];
    //C++ args
    std::vector< std::string > v_args;
    //required attrs
    bool b_mxn      = false|| !output.m_require_mxn;
    bool b_kernel   = false|| !output.m_require_kernel;
    bool b_poly     = false|| !output.m_require_poly;
    bool b_iteration= false|| !output.m_require_iteration;
    bool b_zoom     = false|| !output.m_require_zoom;
    bool b_qxp      = false|| !output.m_require_qxp;
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
            //is added
            b_qxp = true;
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
            //is added
            b_zoom = true;
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
    
    if(!b_qxp)
    {
        output.m_success = false;
        output.m_errors += "--qxp is required\n" ;
    }
    
    if(!b_zoom)
    {
        output.m_success = false;
        output.m_errors += "--zoom is required\n" ;
    }
    
    return output.m_success;
}

inline std::string get_help(const argv_parameters& params)
{
    
    static const char c_str_required[]     = "[required]";
    static const char c_str_not_required[] = "          ";
    static const char c_str_options [] =
    "Options:                                                                \n"
    "\t--help/-h                       help                                  \n"
    "\t--polynomial/-p <expression>    polynomial expression               %s\n"
    "\t--kernel/-k  <kernel name>      resolution method                   %s\n"
    "\t--iteration/-i <number>         number of iteration                 %s\n"
    "\t--mxn/-m  <width> <height>      matrix resolution                   %s\n"
    "\t--qxp/-q  <width> <height>      matrix subdivision                  %s\n"
    "\t--zoom/-z <zoom factor>         zoom factor into center area        %s\n";
    
    char c_str_out_options [sizeof(c_str_options)*2];
    std::sprintf(c_str_out_options,
                 c_str_options,
                 params.m_require_poly      ? c_str_required : c_str_not_required,
                 params.m_require_kernel    ? c_str_required : c_str_not_required,
                 params.m_require_iteration ? c_str_required : c_str_not_required,
                 params.m_require_mxn       ? c_str_required : c_str_not_required,
                 params.m_require_qxp       ? c_str_required : c_str_not_required,
                 params.m_require_zoom      ? c_str_required : c_str_not_required);
    
    
    std::stringstream stream;
    stream << params.m_app << " [options] \n";
    stream << c_str_out_options;
    
    return stream.str();
}