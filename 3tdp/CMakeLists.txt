cmake_minimum_required(VERSION 2.6)
project(TDP3)

# Write binaries to bin/
set(EXECUTABLE_OUTPUT_PATH "${PROJECT_SOURCE_DIR}/bin")

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -Wall")

if(DEFINED USE_MKL)
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mkl:${USE_MKL} -DUSE_MKL")
endif()

file(GLOB_RECURSE mult_files
       "src/fox_mult/*.c"
       "src/fox_mult/*.h")

file(GLOB_RECURSE util_files
	"src/util/*.c"
	"src/util/*.h")

find_package(BLAS)
message("Using BLAS libs: ${BLAS_LIBRARIES}")

find_package(MPI)
message("Using MPI libs: ${MPI_LIBRARIES}")

set(PROJECT_INCLUDE_DIR "src")
include_directories("${PROJECT_INCLUDE_DIR}")
include_directories("${MPI_INCLUDE_PATH}")

add_executable("main"	src/main.c
			${util_files}
			${mult_files})
target_link_libraries("main" ${BLAS_LIBRARIES} ${MPI_LIBRARIES} m)

add_executable("test"	src/test.c
			${util_files}
			${mult_files})
target_link_libraries("test" ${BLAS_LIBRARIES} ${MPI_LIBRARIES} m)

add_executable("bench"	src/bench.c
			${util_files}
			${mult_files})
target_link_libraries("bench" ${BLAS_LIBRARIES} ${MPI_LIBRARIES} m)







