module load compiler/intel compiler/mkl mpi/openmpi
mkdir -p build 
cd build
cmake .. -DUSE_MKL=single
make
