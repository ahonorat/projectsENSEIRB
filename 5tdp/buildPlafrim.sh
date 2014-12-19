module load compiler/intel
mkdir -p build 
cd build
cmake .. -DUSE_MKL=parallel
make