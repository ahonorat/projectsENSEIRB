module load compiler/intel mpi/openmpi
mkdir -p build 
cd build
cmake .. -DUSE_MKL=single
make
