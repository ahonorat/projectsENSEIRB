module load compiler/intel mpi/openmpi
rm -rf build
mkdir build 
cd build
cmake .. -DUSE_MKL=parallel
make
