# MIT License
#
# Copyright (c) 2018-2020 char-lie
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.
option(WITH_OPENMP "Use OpenMP for parallel computations on CPU" OFF)
option(WITH_OPENCL "Use OpenCL for parallel computations on GPU" OFF)
option(WITH_CUDA "Use CUDA for parallel computations on GPU" OFF)

find_package(Boost 1.54 REQUIRED COMPONENTS program_options)
if (WITH_OPENMP)
    find_package(OpenMP REQUIRED)
endif(WITH_OPENMP)
if (WITH_OPENCL)
    find_package(Boost 1.61 REQUIRED)
    find_package(OpenCL REQUIRED)
endif(WITH_OPENCL)
if (WITH_CUDA)
    cmake_minimum_required(VERSION 3.11 FATAL_ERROR)
    enable_language(CUDA)
endif(WITH_CUDA)

add_library(image image.cpp)
add_library(pgm_io pgm_io.cpp)
add_library(disparity_graph disparity_graph.cpp)
add_library(lowest_penalties lowest_penalties.cpp)
add_library(constraint_graph constraint_graph.cpp)
add_library(labeling_finder labeling_finder.cpp)
add_library(indexing indexing.cpp)
add_library(indexing_checks indexing_checks.cpp)
if (OpenCL_FOUND)
add_library(compile_cl compile_cl.cpp)
add_library(gpu_csp gpu_csp.cpp)
endif (OpenCL_FOUND)

if (WITH_CUDA)
    add_library(
        cuda_csp
        cuda_csp.cu
        solve_csp.cu
        constraint_graph.cu
        lowest_penalties.cu
        labeling_finder.cu
        disparity_graph.cu
        image.cu
        indexing_checks.cu
        indexing.cu
    )
endif (WITH_CUDA)

target_include_directories(
    indexing PUBLIC
    ${STEREO_PARALLEL_MAIN_INCLUDE_DIR}
)
target_include_directories(
    indexing_checks PUBLIC
    ${STEREO_PARALLEL_MAIN_INCLUDE_DIR}
)
target_include_directories(
    image PUBLIC
    ${STEREO_PARALLEL_MAIN_INCLUDE_DIR}
)
target_include_directories(
    pgm_io PUBLIC
    ${STEREO_PARALLEL_MAIN_INCLUDE_DIR}
)
target_include_directories(
    disparity_graph PUBLIC
    ${STEREO_PARALLEL_MAIN_INCLUDE_DIR}
)
target_include_directories(
    lowest_penalties PUBLIC
    ${STEREO_PARALLEL_MAIN_INCLUDE_DIR}
)
target_include_directories(
    constraint_graph PUBLIC
    ${STEREO_PARALLEL_MAIN_INCLUDE_DIR}
)
target_include_directories(
    labeling_finder PUBLIC
    ${STEREO_PARALLEL_MAIN_INCLUDE_DIR}
)

if (OpenCL_FOUND)
    target_include_directories(
        compile_cl PUBLIC
        ${STEREO_PARALLEL_MAIN_INCLUDE_DIR}
    )
    target_include_directories(
        gpu_csp PUBLIC
        ${STEREO_PARALLEL_MAIN_INCLUDE_DIR}
    )
endif (OpenCL_FOUND)

if (WITH_CUDA)
    target_include_directories(
        cuda_csp PUBLIC
        ${STEREO_PARALLEL_MAIN_INCLUDE_DIR}
    )
endif (WITH_CUDA)

target_link_libraries(
    pgm_io
    image
    indexing
)
target_link_libraries(
    disparity_graph
    image
    indexing
)
target_link_libraries(
    lowest_penalties
    disparity_graph
    image
)
target_link_libraries(
    constraint_graph
    lowest_penalties
    disparity_graph
    image
    indexing_checks
    indexing
)
if (OpenMP_CXX_FOUND)
    target_link_libraries(
        constraint_graph
        OpenMP::OpenMP_CXX
    )
endif (OpenMP_CXX_FOUND)

target_link_libraries(
    labeling_finder
    constraint_graph
    lowest_penalties
    disparity_graph
    image
)

if (WITH_CUDA)
    set(CMAKE_CUDA_FLAGS "${CMAKE_CUDA_FLAGS} -arch=sm_60")
    set_target_properties(cuda_csp PROPERTIES CUDA_SEPARABLE_COMPILATION ON)

    target_link_libraries(
        labeling_finder
        cuda_csp
    )
    target_compile_definitions(
        labeling_finder
        PRIVATE
        "USE_CUDA"
    )
endif (WITH_CUDA)

if (OpenCL_FOUND)
    target_link_libraries(
        labeling_finder
        gpu_csp
    )
    target_compile_definitions(
        labeling_finder
        PRIVATE
        "USE_OPENCL"
    )
endif (OpenCL_FOUND)

if (OpenCL_FOUND)
    target_link_libraries(
        compile_cl
        OpenCL::OpenCL
    )
    target_link_libraries(
        gpu_csp
        constraint_graph
        compile_cl
        OpenCL::OpenCL
    )
endif (OpenCL_FOUND)

add_executable(stereo_parallel main.cpp)
target_include_directories(
    stereo_parallel
    PRIVATE
    ${BOOST_INCLUDE_DIRS}
)
target_link_libraries(
    stereo_parallel
    image
    pgm_io
    disparity_graph
    constraint_graph
    labeling_finder
    ${Boost_PROGRAM_OPTIONS_LIBRARY}
)

if (OpenCL_FOUND)
    target_link_libraries(
        stereo_parallel
        gpu_csp
    )
    target_compile_definitions(
        stereo_parallel
        PRIVATE
        "USE_OPENCL"
    )
endif (OpenCL_FOUND)

if (WITH_CUDA)
    target_compile_definitions(
        stereo_parallel
        PRIVATE
        "USE_CUDA"
    )
endif (WITH_CUDA)
