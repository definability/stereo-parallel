# MIT License
#
# Copyright (c) 2018 char-lie
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
find_package(Boost 1.54 REQUIRED COMPONENTS unit_test_framework)

add_executable(test_executable api.cpp image.cpp)
target_include_directories(test_executable PRIVATE ${BOOST_INCLUDE_DIRS})
target_link_libraries(
    test_executable
    image
    ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY}
)
target_compile_definitions(
    test_executable
    PRIVATE
    "BOOST_TEST_DYN_LINK=1"
)

add_test(NAME stereo-parallel COMMAND test_executable)
