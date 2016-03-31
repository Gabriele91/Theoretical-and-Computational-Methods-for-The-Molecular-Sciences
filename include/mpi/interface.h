//
//  interface.h
//  MPI-Test_01
//
//  Created by Gabriele on 12/03/16.
//  Copyright © 2016 Gabriele. All rights reserved.
//
#pragma once
#include <string>
#include <vector>
#include <complex>
#include <assert.h>

namespace mpi_interface
{
    int  init();
    int  init(int& argc, char** argv);
    void finalize();
    
    double worker_tick();
    double worker_time();
    //class time
    class timer
    {
    public:
        
        void reset();
        double time_elapsed() const;
    
    protected:
        
        //attributes
        double m_start{ worker_time() };

    };
    //class declaretion
    class mpi_comm;
    class mpi_status;
    class mpi_request;
    class mpi_async_queue;
    //structs declaretion
    struct mpi_data_field;
    //define type
    using mpi_handle = size_t;
    using mpi_request_raw = void*;
    //redefine mpi type
    class mpi_status
    {
    public:
        
        int    get_source()          const;
        int    get_tag()             const;
        int    get_error()           const;
        size_t get_count()           const;
        int    get_count(mpi_handle) const;
        int    get_cancelled()       const;
        
    private:
        
        //attributes
        size_t count;
        int    cancelled;
        int    MPI_SOURCE;
        int    MPI_TAG;
        int    MPI_ERROR;
        
        //friend class
        friend class mpi_comm;
    };
    
    class mpi_request
    {
    public:
        
        mpi_request();
        bool wait();
        bool wait(mpi_status& status);
        bool cancel();
        bool free();
        bool valid() const;
        bool test(int& flag,mpi_status& status) const;
        ~mpi_request();
        
    private:
        
        //attributes
        mpi_request_raw m_request{ nullptr };
        
        //friend class
        friend class mpi_comm;
        
    };
    //costum datatype field
    struct mpi_data_field
    {
        size_t m_mpi_type       { 0 };
        size_t m_offset_of      { 0 };
        size_t m_array_length   { 1 };
        
        mpi_data_field(){}
        mpi_data_field(size_t mpi_type,
                       size_t offset_of,
                       size_t array_length = 1)
        {
            m_mpi_type     = mpi_type;
            m_offset_of    = offset_of;
            m_array_length = array_length;
        }
    };
    
    mpi_handle type_create_struct(const std::vector< mpi_data_field >& struct_type);
    void type_commit(mpi_handle handle);
    
    //async queue
    class mpi_async_queue
    {
    public:
        
        using mpi_request_uptr = std::unique_ptr< mpi_request >;
        using mpi_requests     = std::vector  < mpi_request_uptr >;
        using mpi_task         = std::function< bool (mpi_request&) >;
        
        void append(mpi_task task);
        void wait();
        void cancel();
        void free();
        
    private:
        
        //queue
        mpi_requests m_queue;
        
        //get request to append
        mpi_request& get_a_request();
        
        //friend class
        friend class mpi_comm;
        
    };
    
    //mpi comm manager
    class mpi_comm
    {
        
        size_t m_type{ 0 };
        
    public:
        
        mpi_comm(size_t comm);
        int  get_rank();
        int  get_size();
        void abort(int errorcode);
        
        ////////////////////////////////////
        //sync calls
        bool prob(int source,
                  int tag,
                  mpi_status& status);
        
        bool bcast(void* buffer,
                   int count,
                   mpi_handle type,
                   int root = 0);
        
        bool send(void* buffer,
                  int count,
                  mpi_handle type,
                  int dest,
                  int tag);
        
        bool recv(void* buffer,
                  int count,
                  mpi_handle type,
                  int source,
                  int tag,
                  mpi_status& status);
        
        bool recv(void* buffer,
                  int count,
                  mpi_handle type,
                  int source,
                  int tag);
        
        ////////////////////////////////////
        //async calls
        bool i_prob(int  source,
                    int  tag,
                    int& flag,
                    mpi_status& status);
        
        bool i_bcast_lock(void* buffer,
                          int count,
                          mpi_handle type,
                          int root = 0);
        
        bool i_bcast(void* buffer,
                     int count,
                     mpi_handle type,
                     int root,
                     mpi_request& request);
        
        bool i_bcast(void* buffer,
                     int count,
                     mpi_handle type,
                     int root,
                     mpi_async_queue& q_request);
        
        bool i_send_lock(void* buffer,
                         int count,
                         mpi_handle type,
                         int dest,
                         int tag);
        
        bool i_send(void* buffer,
                    int count,
                    mpi_handle type,
                    int dest,
                    int tag,
                    mpi_request& request);
        
        bool i_send(void* buffer,
                    int count,
                    mpi_handle type,
                    int dest,
                    int tag,
                    mpi_async_queue& q_request);
        
        bool i_recv_lock(void* buffer,
                         int count,
                         mpi_handle type,
                         int source,
                         int tag);
        
        bool i_recv(void* buffer,
                    int count,
                    mpi_handle type,
                    int source,
                    int tag,
                    mpi_request& request);
        
        bool i_recv(void* buffer,
                    int count,
                    mpi_handle type,
                    int source,
                    int tag,
                    mpi_async_queue& q_request);
        
    };
    extern const int mpi_any_tag;
    extern const int mpi_any_source;
    extern mpi_comm  mpi_world;
    extern mpi_comm  mpi_self;
    
    
    namespace mpi_info
    {
        struct mpi_version
        {
            int m_version    { 0 };
            int m_sub_version{ 0 };
        };
        
        mpi_version get_version();
        std::string get_processor_name();
        
    };
    
    template< typename T  > size_t type(){ return (size_t)-1; }
    
    template<> size_t type<bool>();
    template<> size_t type<char>();
    template<> size_t type<unsigned char>();
    template<> size_t type<short>();
    template<> size_t type<unsigned short>();
    template<> size_t type<int>();
    template<> size_t type<unsigned int>();
    template<> size_t type<long>();
    template<> size_t type<unsigned long>();
    template<> size_t type<long long>();
    template<> size_t type<unsigned long long>();
    template<> size_t type<float>();
    template<> size_t type<double>();
    template<> size_t type<long double>();
    template<> size_t type< std::complex<float> >();
    template<> size_t type< std::complex<double> >();
    template<> size_t type< std::complex<long double> >();
    
    //dynamic array types
    template<> size_t inline type<bool*>()                        { return type<bool>(); }
    template<> size_t inline type<char*>()                        { return type<char>(); }
    template<> size_t inline type<unsigned char*>()               { return type<unsigned char>(); }
    template<> size_t inline type<short*>()                       { return type<short>(); }
    template<> size_t inline type<unsigned short*>()              { return type<unsigned short>(); }
    template<> size_t inline type<int*>()                         { return type<int>(); }
    template<> size_t inline type<unsigned int*>()                { return type<unsigned int>(); }
    template<> size_t inline type<long*>()                        { return type<long>(); }
    template<> size_t inline type<unsigned long*>()               { return type<unsigned long>(); }
    template<> size_t inline type<long long*>()                   { return type<long long>(); }
    template<> size_t inline type<unsigned long long*>()          { return type<unsigned long long>(); }
    template<> size_t inline type<float*>()                       { return type<float>(); }
    template<> size_t inline type<double*>()                      { return type<double>(); }
    template<> size_t inline type<long double*>()                 { return type<long double>(); }
    template<> size_t inline type< std::complex<float>* >()       { return type< std::complex<float> >(); }
    template<> size_t inline type< std::complex<double>* >()      { return type< std::complex<double> >(); }
    template<> size_t inline type< std::complex<long double>* >() { return type< std::complex<long double> >(); }
    

    
    //types
    size_t type(bool);
    size_t type(char);
    size_t type(unsigned char);
    size_t type(short);
    size_t type(unsigned short);
    size_t type(int);
    size_t type(unsigned int);
    size_t type(long);
    size_t type(unsigned long);
    size_t type(long long);
    size_t type(unsigned long long);
    size_t type(float);
    size_t type(double);
    size_t type(long double);
    size_t type(const std::complex<float>&);
    size_t type(const std::complex<double>&);
    size_t type(const std::complex<long double>&);
    
    
#if 1
    
    template < typename C, typename R>
    mpi_data_field mpi_attr(R C::*M, const size_t size = 1)
    {
        using  attr_type = decltype( ((C*)0)->*M );
        using  attr_array= typename std::remove_reference< attr_type >::type;
        using  attr      = typename std::remove_extent< attr_array >::type;
        
        return
        {
            type< attr >(),
            reinterpret_cast<std::size_t>(&(((C*)0)->*M)),
            size
        };
    }
    
    template < typename C, typename R>
    mpi_data_field mpi_attr(mpi_handle type, R C::*M, const size_t size = 1)
    {
        return
        {
            type,
            reinterpret_cast<std::size_t>(&(((C*)0)->*M)),
            size
        };
    }
    
#else

    //attribute offset
#define mpi_attr(c,m)\
    ::mpi_interface::mpi_data_field{  ::mpi_interface::type< decltype(c::m) >() , offsetof(c,m) }
#define mpi_attr_n(c,m,n)\
    ::mpi_interface::mpi_data_field{  ::mpi_interface::type< decltype(&c::m) >() , offsetof(c,m), n }
    
#endif
};

namespace std
{
    inline std::string to_string(const mpi_interface::mpi_info::mpi_version& version)
    {
        return to_string(version.m_version) + "." + to_string(version.m_sub_version);
    }
}

