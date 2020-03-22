/*
 * MIT License
 *
 * Copyright (c) 2018-2020 char-lie
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
#include <indexing.hpp>
#include <pgm_io.hpp>

#include <cctype>
#include <ios>
#include <iostream>
#include <limits>
#include <memory>
#include <string>

namespace sp::image
{

using sp::indexing::pixel_index;
using sp::types::ULONG;
using std::endl;
using std::invalid_argument;
using std::ios;
using std::ios_base;
using std::isspace;
using std::make_shared;
using std::move;
using std::numeric_limits;
using std::stoul;
using std::streamsize;
using std::string;

PGM_IO::PGM_IO(shared_ptr<struct Image> image) : image{move(image)}
{
}

void PGM_IO::set_image(const shared_ptr<struct Image>& image)
{
    this->image = image;
}

shared_ptr<struct Image> PGM_IO::get_image() const
{
    return this->image;
}

string PGM_IO::read_pgm_instruction(istream& in)
{
    if (in.fail())
    {
        return "";
    }
    string current_input;
    in >> current_input;
    while (in.good() && current_input[0] == '#')
    {
        in.ignore(numeric_limits<streamsize>::max(), '\n');
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

bool PGM_IO::check_file_end(istream& in)
{
    char current_input;
    while (in.good() && !in.eof())
    {
        in.get(current_input);
        if (current_input == '#')
        {
            in.ignore(numeric_limits<streamsize>::max(), '\n');
        }
        else if (isspace(current_input) == 0)
        {
            in.setstate(ios_base::failbit);
        };
    }
    if (in.eof())
    {
        in.clear(ios::goodbit | ios::eofbit);
    }
    return in.good();
}

ostream& operator<<(ostream& out, const PGM_IO& ppm_io)
{
    if (!ppm_io.get_image() || !image_valid(ppm_io.get_image().get()))
    {
        return out;
    }
    shared_ptr<struct Image> image = ppm_io.get_image();
    out << PGM_IO::FORMAT_CODE << endl;
    out << image->width << " " << image->height << endl;
    out << image->max_value << endl;
    for (ULONG y = 0; y < image->height; ++y)
    {
        for (ULONG x = 0; x < image->width; ++x)
        {
            out << image->data[pixel_index(image.get(), {x, y})];
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
                    out << endl;
                }
            }
        }
        out << endl;
    }
    return out;
}

istream& operator>>(istream& in, PGM_IO& ppm_io)
{
    string format_code = PGM_IO::read_pgm_instruction(in);
    if (format_code != PGM_IO::FORMAT_CODE)
    {
        in.setstate(ios_base::failbit);
        return in;
    }

    shared_ptr<struct Image> image =
        make_shared<struct Image>(Image{0, 0, 0, {}});

    try
    {
        image->width = stoul(PGM_IO::read_pgm_instruction(in));
        image->height = stoul(PGM_IO::read_pgm_instruction(in));
        image->max_value = stoul(PGM_IO::read_pgm_instruction(in));
    }
    catch (invalid_argument&)
    {
        in.setstate(ios_base::failbit);
        return in;
    }

    if (image->max_value > PGM_IO::MAX_VALUE_LIMIT)
    {
        in.setstate(ios_base::failbit);
        return in;
    }
    image->data.resize(image->width * image->height);

    for (ULONG y = 0; y < image->height; ++y)
    {
        for (ULONG x = 0; x < image->width; ++x)
        {
            try
            {
                image->data[pixel_index(image.get(), {x, y})] =
                    stoul(PGM_IO::read_pgm_instruction(in));
            }
            catch (invalid_argument&)
            {
                in.setstate(ios_base::failbit);
                return in;
            }
        }
    }

    PGM_IO::check_file_end(in);

    ppm_io.set_image(image);

    return in;
}

}
