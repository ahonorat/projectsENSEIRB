cmake_minimum_required(VERSION 2.6)
project(TDP1)

# Write binaries to bin/
set(EXECUTABLE_OUTPUT_PATH "${PROJECT_SOURCE_DIR}/bin")

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -Wall")

if(DEFINED USE_MKL)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mkl:${USE_MKL} -DUSE_MKL")
else()
  find_package(BLAS REQUIRED)
  message("Using BLAS libs: ${BLAS_LIBRARIES}")
  find_package(LAPACK REQUIRED)
  message("Using Lapack libs: ${LAPACK_LIBRARIES}")
endif()

find_package(MPI REQUIRED)
message("Using MPI libs: ${MPI_LIBRARIES}")

file(GLOB_RECURSE mylapack_files
       "src/mylapack/*.c"
       "src/mylapack/*.h")

file(GLOB_RECURSE myblas_files
       "src/myblas/*.c"
       "src/myblas/*.h")

file(GLOB_RECURSE util_files
	"src/util/*.c"
	"src/util/*.h")

file(GLOB_RECURSE benchmark_files
	"src/benchmark/*.c"
	"src/benchmark/*.h")

find_package(Threads REQUIRED)
message("Using Threads libs: ${CMAKE_THREAD_LIBS_INIT}")


set(PROJECT_INCLUDE_DIR "src")
include_directories("${PROJECT_INCLUDE_DIR}")
include_directories("${MPI_INCLUDE_PATH}")

add_library(myblas SHARED ${myblas_files})

add_library(mylapack SHARED ${mylapack_files})
target_link_libraries(mylapack myblas)


add_executable("test-util"
			src/test/test-util.c
			${util_files})
target_link_libraries("test-util" ${MPI_LIBRARIES})

add_executable("test-myblas"
			src/test/test-myblas.c
			${util_files})
target_link_libraries("test-myblas" ${BLAS_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT} ${MPI_LIBRARIES})
target_link_libraries("test-myblas" myblas)

add_executable("test-mylapack"
			src/test/test-mylapack.c
			${util_files})
target_link_libraries("test-mylapack" ${BLAS_LIBRARIES} ${LAPACK_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT} ${MPI_LIBRARIES})
target_link_libraries("test-mylapack" myblas mylapack)

add_executable("benchmark"
			${util_files}
			${benchmark_files})
target_link_libraries("benchmark" ${BLAS_LIBRARIES} ${LAPACK_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT} ${MPI_LIBRARIES})
target_link_libraries("benchmark" myblas mylapack)