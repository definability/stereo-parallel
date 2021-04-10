#include <boost/program_options.hpp>

#include <constraint_graph.hpp>
#include <disparity_graph.hpp>
#ifdef USE_OPENCL
#include <gpu_csp.hpp>
#endif
#include <image.hpp>
#include <labeling_finder.hpp>
#include <lowest_penalties.hpp>
#include <pgm_io.hpp>

#include <algorithm>
#include <fstream>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>

struct sp::image::Image read_image(const std::string& image_path);

enum Parallelism
{
    CPU,
#ifdef USE_OPENCL
    OpenCL,
#endif
#ifdef USE_CUDA
    CUDA,
#endif
};

int main(int argc, char* argv[]) try
{
    boost::program_options::options_description desc("Allowed options");
    desc.add_options()
        ("help,h", "Help message")
        ("parallel,p",
         boost::program_options::value<std::string>(),
         "Choose the paralleling technology: OMP, CL")
        ("left-image,l",
         boost::program_options::value<std::string>(),
         "Left image")
        ("right-image,r",
         boost::program_options::value<std::string>(),
         "Right image")
        ("output-image,o",
         boost::program_options::value<std::string>(),
         "Output image with disparity map")
        ("disparity-levels,d",
         boost::program_options::value<std::string>(),
         "Number of disparity levels")
        ("smoothness,s",
         boost::program_options::value<std::string>(),
         "Smoothness weight")
        ("cleanness,c",
         boost::program_options::value<std::string>(),
         "Cleanness weight")
    ;

    boost::program_options::variables_map vm;
    try
    {
        boost::program_options::store(
            boost::program_options::parse_command_line(argc, argv, desc),
            vm
        );
    }
    catch (boost::program_options::multiple_occurrences& e)
    {
        std::cerr
            << "You should specify each parameter only once: "
            << e.what() << std::endl;
        return 1;
    }

    boost::program_options::notify(vm);

    if (
        vm.count("output-image") == 1
        && vm.count("left-image") == 1
        && vm.count("right-image") == 1
    )
    {
        Parallelism parallelism;
        if (vm.count("parallel") == 0)
        {
            parallelism = Parallelism::CPU;
        }
        else
        {
            std::string parallelism_input = vm["parallel"].as<std::string>();
            std::transform(
                parallelism_input.begin(),
                parallelism_input.end(),
                parallelism_input.begin(),
                [](unsigned char c){ return std::tolower(c); }
            );
#ifdef USE_OPENCL
            if (parallelism_input == "cl" || parallelism_input == "opencl")
            {
                parallelism = Parallelism::OpenCL;
                    std::cout << "OpenCL parallelism" << std::endl;
            }
            else
#endif
#ifdef USE_CUDA
            if (parallelism_input == "cuda")
            {
                parallelism = Parallelism::CUDA;
                std::cout << "CUDA parallelism" << std::endl;
            }
            else
#endif
            if (
                parallelism_input == "cpu"
                || parallelism_input == "openmp"
                || parallelism_input == "omp"
            )
            {
                    parallelism = Parallelism::CPU;
                    std::cout << "OpenMP parallelism" << std::endl;
            }
            else
            {
                throw std::invalid_argument(
                    "`parallel` cannot be " + vm["parallel"].as<std::string>() + "."
                );
            }
        }
        try
        {
            struct sp::image::Image left_image{
                read_image(vm["left-image"].as<std::string>())
            };
            struct sp::image::Image right_image{
                read_image(vm["right-image"].as<std::string>())
            };
            sp::types::ULONG disparity_levels = 0;
            if (vm.count("disparity-levels") == 0)
            {
                disparity_levels = left_image.width;
            }
            else
            {
                disparity_levels = std::stoul(
                    vm["disparity-levels"].as<std::string>()
                );
            }
            sp::types::FLOAT cleanness = 1;
            if (vm.count("cleanness") == 1)
            {
                cleanness = std::stof(
                    vm["cleanness"].as<std::string>()
                );
            }
            sp::types::FLOAT smoothness = 1;
            if (vm.count("smoothness") == 1)
            {
                smoothness = std::stof(
                    vm["smoothness"].as<std::string>()
                );
            }
            struct sp::graph::disparity::DisparityGraph disparity_graph{
                left_image,
                right_image,
                disparity_levels,
                cleanness,
                smoothness
            };
            struct sp::graph::lowest_penalties::LowestPenalties
                lowest_penalties{&disparity_graph};
            auto available_penalties
                = sp::labeling::finder::fetch_available_penalties(
                    &lowest_penalties
                );
            sp::types::FLOAT threshold
                = sp::labeling::finder::calculate_minimal_consistent_threshold(
                    &lowest_penalties,
                    &disparity_graph,
                    available_penalties
                );
            struct sp::graph::constraint::ConstraintGraph constraint_graph{
                &disparity_graph,
                &lowest_penalties,
                threshold
            };
            if (!solve_csp(&constraint_graph))
            {
                throw std::logic_error(
                    "Cannot solve CSP problem. "
                    "This should not ever happen. "
                    "Refer to the developers."
                );
            }

            struct sp::graph::constraint::ConstraintGraph* labeled_graph = nullptr;
            switch (parallelism)
            {
                case Parallelism::CPU:
                    labeled_graph = sp::labeling::finder::find_labeling(
                        &constraint_graph
                    );
                    break;
#ifdef USE_OPENCL
                case Parallelism::OpenCL:
                    labeled_graph = sp::labeling::finder::find_labeling_cl(
                        &constraint_graph
                    );
                    break;
#endif
#ifdef USE_CUDA
                case Parallelism::CUDA:
                    labeled_graph = sp::labeling::finder::find_labeling_cuda(
                        &constraint_graph
                    );
                    break;
#endif
                default:
                    throw std::logic_error(
                        "Unknown parallelism schema. "
                        "This should not ever happen. "
                        "Refer to the developers."
                    );
            }
            if (labeled_graph == nullptr)
            {
                throw std::logic_error(
                    "Cannot find labeling. "
                    "This should not ever happen. "
                    "Refer to the developers."
                );
            }

            std::shared_ptr<struct sp::image::Image> result
                = std::make_shared<struct sp::image::Image>(
                    sp::labeling::finder::build_disparity_map(
                        &constraint_graph
                    ));
            std::ofstream image_file(vm["output-image"].as<std::string>());
            sp::image::PGM_IO pgm_io{result};
            image_file << pgm_io;
        }
        catch (std::invalid_argument& e)
        {
            std::cerr << "Invalid argument: " << e.what() << std::endl;
        }
        catch (std::logic_error& e)
        {
            std::cerr << "Logic error: " << e.what() << std::endl;
        }
    }
    else if (vm.count("left-image") == 0 || vm.count("right-image") == 0)
    {
        std::cerr
            << "You should specify both left and right image."
            << std::endl;
    }
    else if (vm.count("output-image") == 0)
    {
        std::cerr
            << "You should specify the output image path."
            << std::endl;
    }
    else
    {
        std::cout << desc;
        return 1;
    }
    return 0;
}
catch(std::exception& e) {
    std::cerr << "Unexpected exception: " << e.what() << std::endl;
    return 1;
}
catch(...) {
    std::cerr << "Exception of unknown type!" << std::endl;
    return 1;
}

struct sp::image::Image read_image(const std::string& image_path)
{
    std::ifstream image_file(image_path);
    if (!image_file)
    {
        throw std::invalid_argument(
            "Unable to open file `" + image_path + "`."
        );
    }

    sp::image::PGM_IO pgm_io;
    image_file >> pgm_io;
    if (!image_file)
    {
        throw std::invalid_argument(
            "File `" + image_path +
            "` is not a correct plain PGM image."
        );
    }
    if (!pgm_io.get_image())
    {
        throw std::logic_error(
            "Image `" + image_path + "` is valid, "
            "but it wasn't read for some reason. "
            "Please, report the issue to developers."
        );
    }
    return *pgm_io.get_image();
}
