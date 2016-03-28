//
//  tga.h
//  MPI-Test_01
//
//  Created by Gabriele on 27/03/16.
//  Copyright © 2016 Gabriele. All rights reserved.
//
#pragma once
#include <fstream>
#include <string>
#include <memory>
#include <matrix.h>

#ifdef _MSC_VER
    #define ASPACKED( __Declaration__ ) __pragma( pack(push,1) ) __Declaration__   __pragma( pack(pop) )
#else
    #define ASPACKED( __Declaration__ ) __Declaration__ __attribute__((packed))
#endif

namespace tga
{
    typedef unsigned char byte;
    
    ASPACKED(struct tga_header
    {
        byte  idlength;                // size of ID field that follows 18 byte header (0 usually)
        byte  colourmaptype;           // type of colour map 0=none, 1=has palette
        byte  imagetype;               // type of image 0=none,1=indexed,2=rgb,3=grey,+8=rle packed
        short int colourmaporigin;     // first colour map entry in palette
        short int colourmaplength;     // number of colours in palette
        byte  colourmapbits;           // number of bits per palette entry 15,16,24,32
        short int xorigin;             // image x origin
        short int yorigin;             // image x origin
        short width;                   // image width in pixels
        short height;                  // image height in pixels
        byte  bitsperpixel;            // image bits per pixel 8,16,24,32
        byte  descriptor;              // image descriptor bits (vh flip bits)
    });
    
    template< class T >
    bool save_matrix(const std::string& path,const matrix<T>& matrix)
    {
        //open file
        std::ofstream file(path, std::ios::binary);
        //is open?
        if(!file.is_open()) return false;
        //write
        tga_header header{0};
        //channels
        const size_t channels = 3;
        //set size
        header.width        = matrix.width();
        header.height       = matrix.height();
        header.bitsperpixel = channels*8;
        header.imagetype    = 2;
        //size of image
        size_t size = header.width * header.height * channels;
        //alloc
        byte* data = (byte*) std::calloc(size,1);
        //save image
        for(size_t y=0;y!=header.height; ++y)
        for(size_t x=0;x!=header.width; ++x)
        {
            auto& rgb = matrix(x,y);
            data[(x + y*header.width)*channels + 0] = (byte)(rgb.b*((T)(255.0L)));
            data[(x + y*header.width)*channels + 1] = (byte)(rgb.g*((T)(255.0L)));
            data[(x + y*header.width)*channels + 2] = (byte)(rgb.r*((T)(255.0L)));
        }
        //write
        file.write(reinterpret_cast<const char*>(&header), sizeof(struct tga_header));
        //write raw image
        file.write(reinterpret_cast<const char*>(data), size);
        //dealloc
        free(data);
        
        return true;
    }
}


