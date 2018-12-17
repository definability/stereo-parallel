/*
 * MIT License
 *
 * Copyright (c) 2018 char-lie
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#include "pgm_io.hpp"

#include <cctype>
#include <ios>
#include <iostream>
#include <limits>
#include <memory>
#include <string>

PGM_IO::PGM_IO(std::shared_ptr<struct Image> image) : image{std::move(image)}
{
}

void PGM_IO::set_image(const std::shared_ptr<struct Image>& image)
{
    this->image = image;
}

std::shared_ptr<struct Image> PGM_IO::get_image() const
{
    return this->image;
}

std::string PGM_IO::read_pgm_instruction(std::istream& in)
{
    if (in.fail())
    {
        return "";
    }
    std::string current_input;
    in >> current_input;
    while (in.good() && current_input[0] == '#')
    {
        in.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        if (in.fail())
        {
            return "";
        }
        in >> current_input;
    }
    if (current_input[0] == '#')
    {
        return "";
    }
    return current_input;
}

bool PGM_IO::check_file_end(std::istream& in)
{
    char current_input;
    while (in.good() && !in.eof())
    {
        in.get(current_input);
        if (current_input == '#')
        {
            in.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
        else if (std::isspace(current_input) == 0)
        {
            in.setstate(std::ios_base::failbit);
        };
    }
    if (in.eof())
    {
        in.clear(std::ios::goodbit | std::ios::eofbit);
    }
    return in.good();
}

std::ostream& operator<<(std::ostream& out, const PGM_IO& ppm_io)
{
    if (!ppm_io.get_image() || !image_valid(*ppm_io.get_image()))
    {
        return out;
    }
    std::shared_ptr<struct Image> image = ppm_io.get_image();
    out << PGM_IO::FORMAT_CODE << std::endl;
    out << image->width << " " << image->height << std::endl;
    out << image->max_value << std::endl;
    for (ULONG y = 0; y < image->height; ++y)
    {
        for (ULONG x = 0; x < image->width; ++x)
        {
            out << image->data[get_pixel_index(*image, {x, y})];
            if (x == 0 || (x + 1) % PGM_IO::MAX_NUMBERS_PER_ROW != 0)
            {
                if (x + 1 < image->width)
                {
                    out << " ";
                }
            }
            else
            {
                if (x + 1 < image->width)
                {
                    out << std::endl;
                }
            }
        }
        out << std::endl;
    }
    return out;
}

std::istream& operator>>(std::istream& in, PGM_IO& ppm_io)
{
    std::string format_code = PGM_IO::read_pgm_instruction(in);
    if (format_code != PGM_IO::FORMAT_CODE)
    {
        in.setstate(std::ios_base::failbit);
        return in;
    }

    std::shared_ptr<struct Image> image =
        std::make_shared<struct Image>(Image{0, 0, 0, {}});

    try
    {
        image->width = std::stoul(PGM_IO::read_pgm_instruction(in));
        image->height = std::stoul(PGM_IO::read_pgm_instruction(in));
        image->max_value = std::stoul(PGM_IO::read_pgm_instruction(in));
    }
    catch (std::invalid_argument&)
    {
        in.setstate(std::ios_base::failbit);
        return in;
    }

    if (image->max_value > PGM_IO::MAX_VALUE_LIMIT)
    {
        in.setstate(std::ios_base::failbit);
        return in;
    }
    image->data.resize(image->width * image->height);

    for (ULONG y = 0; y < image->height; ++y)
    {
        for (ULONG x = 0; x < image->width; ++x)
        {
            try
            {
                image->data[get_pixel_index(*image, {x, y})] =
                    std::stoul(PGM_IO::read_pgm_instruction(in));
            }
            catch (std::invalid_argument&)
            {
                in.setstate(std::ios_base::failbit);
                return in;
            }
        }
    }

    PGM_IO::check_file_end(in);

    ppm_io.set_image(image);

    return in;
}
