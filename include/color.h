//
//  color.h
//  MPI-Test_01
//
//  Created by Gabriele on 27/03/16.
//  Copyright © 2016 Gabriele. All rights reserved.
//
#pragma once
#include <cmath>
#include <vector>
#include <complexmath.h>
#include <assert.h>
namespace color
{
    
template < typename T >
struct rgb
{
    T r,g,b;
    
    rgb operator * (const rgb& in)
    {
        return rgb { r*in.r, g*in.g, b*in.b };
    }
    
    const rgb operator * (const rgb& in) const
    {
        return rgb { r*in.r, g*in.g, b*in.b };
    }
    
    rgb operator * (T in)
    {
        return rgb { r*in, g*in, b*in };
    }
    
    const rgb operator * (T in) const
    {
        return rgb { r*in, g*in, b*in };
    }
};
    
template < typename T >
using vector_rgb = std::vector< rgb< T > >;
    
template < class T>
inline void hvs_to_rgb( T *r, T *g, T *b, T h, T s, T v )
{
    int i;
    T f, p, q, t;
    if( s == 0 )
    {
        // achromatic (grey)
        *r = *g = *b = v;
        return;
    }
    h /= 60;			// sector 0 to 5
    i = std::floor( h );
    f = h - i;			// factorial part of h
    p = v * ( 1 - s );
    q = v * ( 1 - s * f );
    t = v * ( 1 - s * ( 1 - f ) );
    switch( i ) {
        case 0:
            *r = v;
            *g = t;
            *b = p;
            break;
        case 1:
            *r = q;
            *g = v;
            *b = p;
            break;
        case 2:
            *r = p;
            *g = v;
            *b = t;
            break;
        case 3:
            *r = p;
            *g = q;
            *b = v;
            break;
        case 4:
            *r = t;
            *g = p;
            *b = v;
            break;
        default:		// case 5:
            *r = v;
            *g = p;
            *b = q;
            break;
    }
}
    
template < class T >
inline void rgb_to_hvs(T r,T g,T b,T *outh,T *outs,T *outv)
{
    
    T maxv = std::max(std::max(r, g), b);
    T minv = std::min(std::min(r, g), b);
    T h = 0;
    T s = 0;
    T d = maxv - minv;
    T l = (maxv + minv) / 2;
    
    if (maxv != minv)
    {
        s = l > 0.5 ? d / (2 - maxv - minv) : d / (maxv + minv);
        if (maxv == r) { h = (g - b) / d + (g < b ? 6 : 0); }
        else if (maxv == g) { h = (b - r) / d + 2; }
        else if (maxv == b) { h = (r - g) / d + 4; }
        h /= 6;
    }
    
    (*outh)=h;
    (*outs)=s;
    (*outv)=l;
}

template < class T >
inline vector_rgb<T> compute_colors(size_t n)
{
    T color_delta=360.0L/(n);
    T phase=120;
    vector_rgb<T> v_out(n);

    for(size_t i=1;i<=n;++i)
    {
        hvs_to_rgb< T >(&v_out[i-1].r,
                        &v_out[i-1].g,
                        &v_out[i-1].b,
                        std::fmod((color_delta*i)+phase,360),
                        1,
                        1);
    }
    
    return v_out;
}

template < class C, class T  >
inline rgb<T> complex_near(const std::complex< C >&                x,
                           const std::vector< std::complex< T > >& roots,
                           const vector_rgb<T>&                    colors,
                           T                                       epsilon)
{
    assert(roots.size()==colors.size());
    
    for(size_t i = 0; i != roots.size(); ++i)
    {
        if(std::complex_distance(roots[i],x,epsilon)) return colors[i];
    }
    
    return rgb<T>{0,0,0};
}
    
};