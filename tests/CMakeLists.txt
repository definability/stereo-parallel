find_package(Boost 1.54 REQUIRED COMPONENTS unit_test_framework)

add_executable(test_executable api.cpp)
target_include_directories(test_executable PRIVATE ${BOOST_INCLUDE_DIRS})
target_link_libraries(test_executable ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})
target_compile_definitions(
    test_executable
    PRIVATE
    "BOOST_TEST_DYN_LINK=1"
    "BOOST_TEST_MODULE=Stereo Parallel tests"
)

add_test(NAME stereo-parallel COMMAND test_executable)
