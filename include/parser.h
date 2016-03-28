//
//  parser.h
//  MPI-Test_01
//
//  Created by Gabriele on 28/03/16.
//  Copyright © 2016 Gabriele. All rights reserved.
//
#pragma once
#include <vector>
#include <string>
#include <complex>
#include <polynomial.h>

template < typename T >
class parser
{
public:
    
    struct pow_value
    {
        std::complex<T> m_value;
        long            m_pow;
    };
    
    struct parse_error
    {
        std::string m_error;
        size_t      m_nc;
        
        parse_error(const std::string& error,size_t nc)
        :m_error(error)
        ,m_nc(nc)
        {
        }
    };

    bool parse(const std::string& text, polynomial< T >& output)
    {
        //init count line and string pointer
        int nc=1;
        const char* c=text.c_str();
        //clear errors
        m_errors.clear();
        //parse
        return poly(output,c,nc);
    }
    
    //get errors
    const std::vector< parse_error >& get_errors() const
    {
        return m_errors;
    }
    
private:
    
    std::vector< parse_error > m_errors;
    
    void push_error(const std::string& error,size_t nc)
    {
        m_errors.push_back({error,nc});
    }
    
    static bool is_space(const char* c)
    {
        return (*c)==' '|(*c)=='\r'|(*c)=='\t';
    }
    
    static bool is_variable(const char* c)
    {
        return (*c)=='z';
    }
    
    static bool is_integer(const char* c)
    {
        return '0'<=(*c) && (*c)<='9';
    }
    
    static bool is_imag(const char* c)
    {
        return (*c)=='i';
    }
    
    static bool is_leftp(const char* c)
    {
        return (*c)=='(';
    }
    
    static bool is_rightp(const char* c)
    {
        return (*c)==')';
    }
    
    static bool is_plus(const char* c)
    {
        return (*c)=='+';
    }
    
    static bool is_min(const char* c)
    {
        return (*c)=='-';
    }
    
    static bool is_number(const char* c)
    {
        return ((*c)=='-'&&is_integer(c+1))|
               ((*c)=='+'&&is_integer(c+1))|
               ((*c)=='.'&&is_integer(c+1))|
               is_integer(c);
    }
    
    static bool is_pow(const char* c)
    {
        return (*c)=='^';
    }
    
    static void jump_space(const char*& c,int& nc)
    {
        while(is_space(c)){ ++c; ++nc; }
    }
    
    static void next(const char*& c,int& nc)
    {
        ++c; ++nc;
    }
    
    bool parse_int(long& value,const char*& c,int& nc)
    {
        //parse int
        char *cend = nullptr;
        //parse
        value=std::strtol(c, &cend, 10);
        //is not a int
        if(!(cend-c))
        {
            push_error("must to be an integer value",nc);
            return false;
        }
        //cout chars
        nc+=cend-c;
        //set next char
        c=cend;
        //return value
        return true;
    }
    
    bool parse_number(T& value,const char*& c,int& nc)
    {
        //parse int
        char *cend = nullptr;
        //parse
        value=(T)std::strtod(c, &cend);
        //is not a int
        if(!(cend-c))
        {
            push_error("must to be an number value",nc);
            return false;
        }
        //cout chars
        nc+=cend-c;
        //set next char
        c=cend;
        //return value
        return true;
    }
    
    bool const_complex(std::complex< T >& output,const char*& c,int& nc)
    {
        jump_space(c,nc);
        //(<complex> or float/real or <number>i or i
        if(!is_leftp(c))
        {
            //real or <number>i
            if(is_number(c))
            {
                //parse number
                T value = (T)(0.0L);
                //parse number
                if(!parse_number(value,c,nc))
                {
                    push_error("invalid complex constant",nc);
                    return false;
                }
                //<number>i ?
                if(is_imag(c))
                {
                    next(c,nc);
                    //ouput
                    output = std::complex< T >{0,value};
                    //return true
                    return true;
                }
                //is a real
                //ouput
                output = std::complex< T >{value,0.0};
                //return true
                return true;
            }
            //i
            else if(is_imag(c))
            {
                //next i
                next(c,nc);
                //ouput
                output = std::complex< T >{0.0,1.0};
                //return true
                return true;
            }
            else
            {
                push_error("invalid complex constant",nc);
                return false;
            }
        }
        //jump (
        next(c,nc);
        //(....<number>
        jump_space(c,nc);
        //is a number
        if(!is_number(c))
        {
            push_error("invalid complex real part",nc);
            return false;
        }
        //real part
        T real = (T)(0.0L);
        //parse
        if(!parse_number(real,c,nc))
        {
            push_error("invalid complex constant",nc);
            return false;
        }
        //imag
        T imag      = (T)(0.0L);
        T imag_sign = (T)(0.0L);
        //find +/-
        if(!is_plus(c)&&!is_min(c))
        {
            push_error("invalid complex imag part",nc);
            return false;
        }
        //get sign
        imag_sign=(T)(is_plus(c)?+1.0L:-1.0L);
        //next
        next(c,nc);
        //+/-<number>i
        if(!is_number(c))
        {
            //+/-i
            if(!is_imag(c))
            {
                push_error("invalid complex imag part",nc);
                return false;
            }
            //save value
            imag = imag_sign;
            //next
            next(c,nc);
        }
        else
        {
            //<number>
           if(!parse_number(imag,c,nc) && !is_imag(c))
           {
               push_error("invalid complex imag part",nc);
               return false;
           }
           //add value sign
           imag *= imag_sign;
           //next
           next(c,nc);
        }
        //)
        if(!is_rightp(c))
        {
            push_error("invalid complex imag part",nc);
            return false;
        }
        //next
        next(c,nc);
        //ouput
        output = std::complex<T>{real,imag};
        //return
        return true;
    }
    
    bool grade(pow_value& output,bool first,const char*& c,int& nc)
    {
        //jump
        jump_space(c,nc);
        //+/-
        if(is_plus(c))
        {
            output.m_value=(T)1.0L;
            //next
            next(c,nc);
            //jump space
            jump_space(c,nc);
        }
        else if(is_min(c))
        {
            output.m_value=(T)(-1.0L);
            //next
            next(c,nc);
            //jump space
            jump_space(c,nc);
        }
        else if(first)
        {
            output.m_value=(T)1.0L;
        }
        else
        {
            push_error("value whiout signed",nc);
            return false;
        }
        //+/- <complex>
        if(is_leftp(c)||is_number(c)||is_imag(c))
        {
            if(!const_complex(output.m_value,c,nc))
            {
                return false;
            }
            //jump space
            jump_space(c,nc);
        }
        //variable and pow
        if(is_variable(c))
        {
            //jump x
            next(c,nc);
            //jump space
            jump_space(c,nc);
            //^
            if(!is_pow(c))
            {
                //...x^1
                output.m_pow=1;
                return true;
            }
            //next
            next(c,nc);
            //parse power value
            if(!parse_int(output.m_pow,c,nc) || output.m_pow<0)
            {
                push_error("invalid power number",nc);
                return false;
            }
        }
        else
        {
            output.m_pow=0;
        }
        //value return
        return true;
    }
    
    bool poly(polynomial<T>& ouput,const char*& c,int& nc)
    {
        //init
        ouput.clear();
        //values bool
        std::vector< bool > boolout;
        //first is true
        bool first=true;
        //loop read
        while(*c)
        {
            //current complex pow
            pow_value xcurrent;
            //parse
            if(!grade(xcurrent,first,c,nc))
            {
                return false;
            }
            //size pow
            if(ouput.size()<=xcurrent.m_pow)
            {
                ouput.resize(xcurrent.m_pow+1);
                boolout.resize(xcurrent.m_pow+1);
            }
            //is olready added?
            if(boolout[xcurrent.m_pow])
            {
                push_error("pow olready added",nc);
                return false;
            }
            //look variable
            boolout[xcurrent.m_pow]=true;
            //save value
            ouput[xcurrent.m_pow]=xcurrent.m_value;
            //next is not first
            first=false;
        }
        //reverse
        std::reverse(ouput.begin(),ouput.end());
        //return value
        return  true;
    }
};