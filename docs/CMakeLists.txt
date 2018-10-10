option(BUILD_DOC "Build documentation" OFF)

if (BUILD_DOC)
    find_package(Doxygen REQUIRED)
endif (BUILD_DOC)
