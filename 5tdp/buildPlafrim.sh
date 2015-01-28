#!/bin/bash
module load compiler/intel mpi/openmpi/current
mkdir -p build 
cd build
cmake .. -DUSE_MKL=parallel
make
