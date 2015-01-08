cmake_minimum_required(VERSION 2.6)
project(TDP1)

# Write binaries to bin/
set(EXECUTABLE_OUTPUT_PATH "${PROJECT_SOURCE_DIR}/bin")

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O2 -Wall")

if(DEFINED USE_MKL)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mkl:${USE_MKL} -DUSE_MKL")
else()
  find_package(BLAS REQUIRED)
  message("Using BLAS libs: ${BLAS_LIBRARIES}")
  find_package(LAPACK REQUIRED)
  message("Using Lapack libs: ${LAPACK_LIBRARIES}")
endif()

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

add_library(myblas SHARED ${myblas_files})

add_library(mylapack SHARED ${mylapack_files})


add_executable("test-util"
			src/test/test-util.c
			${util_files})

add_executable("test-myblas"
			src/test/test-myblas.c
			${util_files})
target_link_libraries("test-myblas" myblas ${BLAS_LIBRARIES})
target_link_libraries ("test-myblas" ${CMAKE_THREAD_LIBS_INIT})

add_executable("test-mylapack"
			src/test/test-mylapack.c
			${util_files})
target_link_libraries("test-mylapack" ${BLAS_LIBRARIES} ${LAPACK_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries("test-mylapack" myblas mylapack)


add_executable("benchmark"
			${util_files}
			${benchmark_files})
target_link_libraries("benchmark" ${BLAS_LIBRARIES} ${LAPACK_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries("benchmark" myblas mylapack)








