//
//  matrix.h
//  MPI-Test_01
//
//  Created by Gabriele on 27/03/16.
//  Copyright © 2016 Gabriele. All rights reserved.
//
#pragma once
#include <memory>
#include <complexmath.h>
#include <functional>
#include <color.h>
template < class type >
class matrix
{
public:
    
    using value_t   = type;
    using rgb_t     = color::rgb< value_t >;
    using complex_t = std::complex< value_t >;
    using matrix_t  = std::unique_ptr< rgb_t[] >;
    
    struct init_t
    {
        complex_t m_origin;
        size_t    m_size[2];
    };
    
    static init_t init_center(size_t width,
                              size_t height,
                              value_t factor)
    {
        return init_t
        {
            {
                -(value_t)(width)*factor/ (value_t)2.0L,
                -(value_t)(height)*factor/(value_t)2.0L
            },
            {
                (size_t)(width),
                (size_t)(height)
            }
        };
    }
    
    static init_t init_sub_center(size_t width,
                                  size_t height,
                                  size_t x,
                                  size_t y,
                                  size_t local_width,
                                  size_t local_height,
                                  value_t factor)
    {
        return init_t
        {
            {
                ((value_t)(local_width* (x)) - ((value_t)width/(value_t)2.0L) )*factor,
                ((value_t)(local_height*(y)) - ((value_t)height/(value_t)2.0L))*factor
            },
            {
                (size_t)(local_width),
                (size_t)(local_height)
            }
        };
    }
    
    class row_index_t
    {
        
        rgb_t& operator [] (size_t y)
        {
            return m_matrix[m_x+y*m_width];
        }
        
        const rgb_t& operator [] (size_t y) const
        {
            return m_matrix[m_x+y*m_width];
        }
        
    private:
        //row info
        size_t m_x;
        size_t m_width;
        //matrix
        matrix_t& m_matrix;
        //friend class
        friend class local_matrix;
    };
    
    matrix(complex_t origin,size_t size[2])
    {
        m_origin  = origin;
        m_size[0] = size[0];
        m_size[1] = size[1];
        m_matrix  = matrix_t(new rgb_t[m_size[0]*m_size[1]]);
    }
    
    matrix(const init_t& init)
    {
        m_origin  = init.m_origin;
        m_size[0] = init.m_size[0];
        m_size[1] = init.m_size[1];
        m_matrix  = matrix_t(new rgb_t[m_size[0]*m_size[1]]);
    }
    
    void clear()
    {
        m_origin  = complex_t();
        m_size[0] = 0;
        m_size[1] = 0;
        m_matrix  = matrix_t(nullptr);
    }
    
    bool get_from(size_t x,size_t y, const matrix<value_t>& in_matrix)
    {
        bool b_success = ((x + in_matrix.width())  <= width() ) &&
                         ((y + in_matrix.height()) <= height());
        //copy
        if (b_success)
        {
            for(size_t l_y=0; l_y != in_matrix.height(); ++l_y)
            {
                //copy rows
                std::memcpy( &m_matrix[ x+(y+l_y)*width() ],
                             &in_matrix.m_matrix[ l_y * in_matrix.width()  ],
                             in_matrix.width()*sizeof(rgb_t) );
            }
        }
        //return
        return b_success;
    }
    
    size_t width() const
    {
        return m_size[0];
    }
    
    size_t height() const
    {
        return m_size[1];
    }
    
    size_t get_raw_size() const
    {
        return width()*height();
    }
    
    rgb_t* get_raw_data()
    {
        return m_matrix.get();
    }
    
    const rgb_t* get_raw_data() const
    {
        return m_matrix.get();
    }
    
    const complex_t& origin() const
    {
        return m_origin;
    }
    
    rgb_t& operator()(size_t x,size_t y)
    {
        return m_matrix[ x+y*width() ];
    }
    
    const rgb_t& operator()(size_t x,size_t y) const
    {
        return m_matrix[ x+y*width() ];
    }
    
    row_index_t operator [] (size_t x)
    {
        return row_index_t{ x, width(), m_matrix };
    }
    
    const row_index_t operator [] (size_t x) const
    {
        return row_index_t{ x, width(), m_matrix };
    }
    
    void applay(std::function< rgb_t(const complex_t&,const complex_t&) > pass, value_t factor=(value_t)1.0L)
    {
        for(size_t x=0; x != width();  ++x)
        for(size_t y=0; y != height(); ++y)
        {
            complex_t plane_pos { (value_t)x*factor,(value_t)y*factor };
            m_matrix[ x+y*width() ]=pass(m_origin,plane_pos);
        }
    }
    
    class abstract_succession
    {
    public:
        virtual rgb_t pass(const complex_t&,const complex_t&) const = 0;
    };
    
    void applay(const abstract_succession& succ, value_t factor=(value_t)1.0L)
    {
        //var declaretion
        complex_t plane_pos;
        //for each
        for(size_t x=0; x != width();  ++x)
        {
            for(size_t y=0; y != height(); ++y)
            {
                plane_pos.real( (value_t)x*factor );
                plane_pos.imag( (value_t)y*factor );
                m_matrix[ x+y*width() ]=succ.pass(m_origin, plane_pos);
            }
        }
    }
private:
    
    complex_t   m_origin;
    size_t      m_size[2];
    matrix_t    m_matrix;
    
};

namespace std
{
    template < typename T >
    inline std::string to_string(const typename matrix< T >::init_t& init)
    {
        return "(" +
               to_string(init.m_origin.real()) +
               "," +
               to_string(init.m_origin.imag()) +
               "), (" +
               to_string(init.m_size[0]) +
               "," +
               to_string(init.m_size[1]) +
               ")";
    }
}
