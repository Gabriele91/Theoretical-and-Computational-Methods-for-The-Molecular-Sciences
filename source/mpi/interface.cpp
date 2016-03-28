//
//  interface.cpp
//  MPI-Test_01
//
//  Created by Gabriele on 12/03/16.
//  Copyright © 2016 Gabriele. All rights reserved.
//

#include <stdio.h>
#include <mpi.h>
#include <memory>
#include <mpi/interface.h>

namespace mpi_interface
{
    
int init()
{
    return MPI_Init(nullptr, nullptr);
}
    
int init(int& argc, char** argv)
{
    return MPI_Init(&argc, &argv);
}

void finalize()
{
    MPI_Finalize();
}
    
double worker_tick()
{
    return MPI_Wtick();
}
    
double worker_time()
{
    return MPI_Wtime();
}
    
mpi_handle type_create_struct(const std::vector< mpi_data_field >& struct_type)
{
    int  n_blocks               = (int)struct_type.size();
    auto block_lengths          = std::unique_ptr<int[]>(new int[struct_type.size()]);
    auto array_of_displacements = std::unique_ptr<MPI_Aint[]>(new MPI_Aint[struct_type.size()]);
    auto array_of_types         = std::unique_ptr<MPI_Datatype[]>(new MPI_Datatype[struct_type.size()]);
    
    for(size_t i = 0 ; i!=struct_type.size(); ++i)
    {
        block_lengths[i]          = (int)struct_type[i].m_array_length;
        array_of_displacements[i] = (MPI_Aint)struct_type[i].m_offset_of;
        array_of_types[i]         = (MPI_Datatype)struct_type[i].m_mpi_type;
    }
    
    MPI_Datatype handle;
    MPI_Type_create_struct(n_blocks,
                           block_lengths.get(),
                           array_of_displacements.get(),
                           array_of_types.get(),
                           &handle);
    
    return (mpi_handle)handle;
}
    
void type_commit(mpi_handle handle)
{
    MPI_Datatype raw_handle = (MPI_Datatype)handle;
    MPI_Type_commit(&raw_handle);
}
    
namespace mpi_info
{
    
    mpi_version get_version()
    {
        mpi_version version;
        MPI_Get_version(&version.m_version, &version.m_sub_version);
        return version;
    }
    
    std::string get_processor_name()
    {
        char processor_name[MPI_MAX_PROCESSOR_NAME];
        int name_len;
        MPI_Get_processor_name(processor_name, &name_len);
        return std::string( name_len ? processor_name : "" );
    }
}

    
template<> size_t type<bool>()          { return (size_t)MPI_UNSIGNED_CHAR; }
template<> size_t type<char>()          { return (size_t)MPI_CHAR; }
template<> size_t type<unsigned char>() { return (size_t)MPI_UNSIGNED_CHAR; }
template<> size_t type<short>()         { return (size_t)MPI_SHORT; }
template<> size_t type<unsigned short>(){ return (size_t)MPI_UNSIGNED_SHORT; }
template<> size_t type<int>()           { return (size_t)MPI_INT; }
template<> size_t type<unsigned int>()  { return (size_t)MPI_UNSIGNED; }
template<> size_t type<long>()          { return (size_t)MPI_LONG; }
template<> size_t type<unsigned long>() { return (size_t)MPI_UNSIGNED_LONG; }
template<> size_t type<long long>()          { return (size_t)MPI_LONG_LONG; }
template<> size_t type<unsigned long long>() { return (size_t)MPI_UNSIGNED_LONG_LONG; }
template<> size_t type<float>()              { return (size_t)MPI_FLOAT; }
template<> size_t type<double>()             { return (size_t)MPI_DOUBLE; }
template<> size_t type<long double>()        { return (size_t)MPI_LONG_DOUBLE; }
template<> size_t type< std::complex<float> >()         { return (size_t)MPI_CXX_FLOAT_COMPLEX; }
template<> size_t type< std::complex<double> >()        { return (size_t)MPI_CXX_DOUBLE_COMPLEX; }
template<> size_t type< std::complex<long double> >()   { return (size_t)MPI_CXX_LONG_DOUBLE_COMPLEX; }
    
size_t type(bool)                   { return (size_t)MPI_UNSIGNED_CHAR; }
size_t type(char)                   { return (size_t)MPI_CHAR; }
size_t type(unsigned char)          { return (size_t)MPI_UNSIGNED_CHAR; }
size_t type(short)                  { return (size_t)MPI_SHORT; }
size_t type(unsigned short)         { return (size_t)MPI_UNSIGNED_SHORT; }
size_t type(int)                    { return (size_t)MPI_INT; }
size_t type(unsigned int)           { return (size_t)MPI_UNSIGNED; }
size_t type(long)                   { return (size_t)MPI_LONG; }
size_t type(unsigned long)          { return (size_t)MPI_UNSIGNED_LONG; }
size_t type(long long)              { return (size_t)MPI_LONG_LONG; }
size_t type(unsigned long long)     { return (size_t)MPI_UNSIGNED_LONG_LONG; }
size_t type(float)                  { return (size_t)MPI_FLOAT; }
size_t type(double)                 { return (size_t)MPI_DOUBLE; }
size_t type(long double)            { return (size_t)MPI_LONG_DOUBLE; }
size_t type(const std::complex<float>&)       { return (size_t)MPI_CXX_FLOAT_COMPLEX; }
size_t type(const std::complex<double>&)      { return (size_t)MPI_CXX_DOUBLE_COMPLEX; }
size_t type(const std::complex<long double>&) { return (size_t)MPI_CXX_LONG_DOUBLE_COMPLEX; }
}