# MIT License
#
# Copyright (c) 2018-2019 char-lie
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
add_library(image image.cpp)
add_library(pgm_io pgm_io.cpp)
add_library(disparity_graph disparity_graph.cpp)
add_library(lowest_penalties lowest_penalties.cpp)
add_library(constraint_graph constraint_graph.cpp)
add_library(labeling_finder labeling_finder.cpp)
add_library(indexing indexing.cpp)
add_library(indexing_checks indexing_checks.cpp)
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
target_link_libraries(
    labeling_finder
    constraint_graph
    lowest_penalties
    disparity_graph
    image
)

find_package(Boost 1.54 REQUIRED COMPONENTS program_options)
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
