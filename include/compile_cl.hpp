#ifndef COMPILE_CL_HPP
#define COMPILE_CL_HPP

#include <string>
#include <vector>

#define BOOST_COMPUTE_DEBUG_KERNEL_COMPILATION

#include <boost/compute.hpp>

namespace gpu
{

using std::string;
using std::vector;

using boost::compute::program;
using boost::compute::context;

/**
 * \brief Build an OpenCL program from a single file.
 *
 * The program is built using OpenCL 1.2.
 * This is done for the compatibility with nVidia devices.
 *
 * Headers are included from the `./include` folder of the project.
 * This is done to use one code for both CPU and GPU:
 * headers are located at the `./include` directory.
 */
program create_program(const string& filename, const context& context);
/**
 * \brief Build an OpenCL program from multiple files.
 * Needed for the case when the code is split into separate files.
 *
 * The program is built using OpenCL 1.2.
 * This is done for the compatibility with nVidia devices.
 *
 * Headers are included from the `./include` folder of the project.
 * This is done to use one code for both CPU and GPU:
 * headers are located at the `./include` directory.
 */
program create_program(const vector<string>& filenames, const context& context);
/**
 * \brief Concatenate contents of multiple files.
 *
 * As long as I don't know
 * how to build an OpenCL program directly from multiple sources,
 * I need to read several files and concatenate them into a string.
 */
string concatenate_files(const vector<string>& filenames);

}

#endif
