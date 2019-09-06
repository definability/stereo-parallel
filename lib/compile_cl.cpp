#include <compile_cl.hpp>

#define BOOST_COMPUTE_DEBUG_KERNEL_COMPILATION
#include <boost/compute.hpp>

#include <fstream>
#include <iterator>
#include <string>
#include <vector>

namespace gpu
{

using std::ifstream;
using std::istreambuf_iterator;
using std::string;
using std::vector;

using boost::compute::program;
using boost::compute::context;

program create_program(const string& filename, const context& context)
{
    return create_program(vector<string>{filename}, context);
}

program create_program(const vector<string>& filenames, const context& context)
{
    program program = program::build_with_source(
        concatenate_files(filenames),
        context,
        "-cl-std=CL1.2 -I ./include"
    );
    return program;
}

string concatenate_files(const vector<string>& filenames)
{
    string result;
    for (const string& filename : filenames)
    {
        ifstream file{filename};
        result += string{
            istreambuf_iterator<char>{file},
            istreambuf_iterator<char>{}
        };
    }
    return result;
}

}
