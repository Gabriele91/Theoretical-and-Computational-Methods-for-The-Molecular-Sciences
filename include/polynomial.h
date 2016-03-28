//
//  polynomial.h
//  MPI-Test_01
//
//  Created by Gabriele on 27/03/16.
//  Copyright © 2016 Gabriele. All rights reserved.
//
#pragma once
#include <vector>
#include <complexmath.h>

template < class T >
using polynomial = std::vector< std::complex< T > >;

template < class T >
using roots = std::vector< std::complex< T > >;

namespace roots_utilities
{
    
    static const size_t      max_pass = 5000;
    static const long double epsilon  = 1E-15;
    
    template <class T>
    bool done(const polynomial<T>& a, const polynomial<T>& b)
    {
        bool unchanged = true;
        std::complex<T> delta;
        for (int i = 0; i < a.size(); i++)
        {
            delta = a[i]-b[i];
            unchanged &= std::abs(delta) < (T)epsilon;
        }
        return unchanged;
    }
    
    template <class T>
    inline std::complex<T> dx_on_fx(const polynomial< T >& constants,  const std::complex<T>& x)
    {
        
        //exit condiction
        if(constants.size()==0) return 0;
        if(constants.size()==1) return constants[0];
        
        std::complex<T> vn=constants[0];
        std::complex<T> wn=vn;
        
        //Horner
        for(int i=1;i<constants.size()-1;++i)
        {
            vn = vn*x+constants[i];
            wn = wn*x+vn;
        }
        //return last pass
        vn = vn*x+constants[constants.size()-1];
        return wn/vn;
        
    }
    
    template <class T>
    inline roots<T> compute(const polynomial< T >& fx)
    {
        static const auto c_zero = std::complex<T>(0.0,0.0);
        static const auto c_one  = std::complex<T>(1.0,0.0);
        //temps
        std::vector< std::complex<T> > xk (fx.size()-1,c_zero);
        std::vector< std::complex<T> > xkk(fx.size()-1,c_zero);
        //calc px
#if 0
        std::complex<T> leading = fx[0];
        for (size_t i = 0; i != fx.size(); i++)
            fx[i] /=leading;
        const T offset=1.134678;
#else
        const T offset=1.144678;
#endif
        //start values
        //root of unity
        for (size_t i = 0; i != xk.size(); ++i)
        {
            xk[i]=std::complex<T>(std::cos((std::pi<T>()*(offset+2*i))/(T)xk.size()),
                                  std::sin((std::pi<T>()*(offset+2*i))/(T)xk.size()));
        }
        //count
        size_t it_count=0;
        //while
        while(true)
        {
            //succession
            for(size_t i=0; i!=xk.size(); ++i)
            {
                //p1(x) / p(x)
                auto p1_on_px=dx_on_fx(fx,xk[i]);
                //\sigma[j=1,n && j!=i]{ 1/(x[i]-x[j]) }
                std::complex<T> sum;
                for (size_t j = 0; j != xk.size(); ++j)
                {
                    if (i != j)
                        sum += (c_one/(xk[i]-xk[j]));
                }
                //calc pass
                xkk[i] = xk[i]-(c_one/(p1_on_px-sum));
            }
            //close it
            if (++it_count > max_pass || done(xk, xkk)) break;
            //next k
            xk=xkk;
        }
        //ret res
        return xkk;
    }
}
