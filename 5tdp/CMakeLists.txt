cmake_minimum_required(VERSION 2.6)
project(TDP1)

# Write binaries to bin/
set(EXECUTABLE_OUTPUT_PATH "${PROJECT_SOURCE_DIR}/bin")

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O2 -Wall")

if(DEFINED USE_MKL)
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mkl:${USE_MKL} -DUSE_MKL")
endif()

file(GLOB_RECURSE myblas_files
       "src/myblas/*.c"
       "src/myblas/*.h")

file(GLOB_RECURSE util_files
	"src/util/*.c"
	"src/util/*.h")

file(GLOB_RECURSE benchmark_files
	"src/benchmark/*.c"
	"src/benchmark/*.h")

find_package(BLAS)
message("Using BLAS libs: ${BLAS_LIBRARIES}")
find_package (Threads)
message("Using Threads libs: ${CMAKE_THREAD_LIBS_INIT}")


set(PROJECT_INCLUDE_DIR "src")
include_directories("${PROJECT_INCLUDE_DIR}")

add_library(myblas SHARED ${myblas_files})



add_executable("test-util"
			src/test/test-util.c
			${util_files})
target_link_libraries("test-util" myblas)
target_link_libraries ("test-util" ${CMAKE_THREAD_LIBS_INIT})

add_executable("test-myblas"
			src/test/test-myblas.c
			${util_files})
target_link_libraries("test-myblas" myblas ${BLAS_LIBRARIES})
target_link_libraries ("test-myblas" ${CMAKE_THREAD_LIBS_INIT})

add_executable("benchmark"
			${util_files}
			${benchmark_files})
target_link_libraries("benchmark" myblas ${BLAS_LIBRARIES})
target_link_libraries ("benchmark" ${CMAKE_THREAD_LIBS_INIT})








