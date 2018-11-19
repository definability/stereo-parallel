#include <boost/program_options.hpp>

#include <fstream>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>

#include <image.hpp>
#include <pgm_io.hpp>

struct Image read_image(std::string image_path);

int main(int argc, char* argv[]) try
{
    boost::program_options::options_description desc("Allowed options");
    desc.add_options()
        ("help,h", "Help message")
        ("left-image,l",
         boost::program_options::value<std::string>(),
         "Left image")
        ("right-image,r",
         boost::program_options::value<std::string>(),
         "Right image");

    boost::program_options::variables_map vm;
    boost::program_options::store(
        boost::program_options::parse_command_line(argc, argv, desc),
        vm
    );
    boost::program_options::notify(vm);    

    if (vm.count("left-image") && vm.count("right-image"))
    {
        struct Image left_image;
        struct Image right_image;
        try {
            left_image = read_image(vm["left-image"].as<std::string>());
            right_image = read_image(vm["right-image"].as<std::string>());
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
    else if (vm.count("left-image") || vm.count("right-image"))
    {
        std::cerr
            << "You should specify both left and right image."
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

struct Image read_image(std::string image_path)
{
    std::ifstream image_file(image_path);
    if (!image_file)
    {
        throw std::invalid_argument(
            "Unable to open file `" + image_path + "`."
        );
    }

    PGM_IO pgm_io;
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
