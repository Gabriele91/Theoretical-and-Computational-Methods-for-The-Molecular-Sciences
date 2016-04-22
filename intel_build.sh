#!/bin/bash
make COMPILER=icpc\
     IS_INTEL=true\
     HIDE_MPI_STATUS=true\
     MPI_DIR=/opt/lib/mpi/intel/14.0.2/mvapich2/1.9\
     MPI_CH=true\
	 PRECISION=$( [ -z "$1" ] && echo "DOUBLE" ||  echo "$1" )\
     ARCH=i686

