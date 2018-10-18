add_library(image image.hpp)
set_target_properties(image PROPERTIES LINKER_LANGUAGE CXX)
target_include_directories(
    image PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)
