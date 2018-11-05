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
#ifndef PGM_IO_HPP
#define PGM_IO_HPP

#include <memory>
#include <string>

#include <image.hpp>

class PGM_IO
{
private:
    std::shared_ptr<struct Image> image;
    static std::string read_ppm_instruction(std::istream& in);
public:
    static const unsigned MAX_VALUE_LIMIT = 1u << 16u;
    static const unsigned MAX_COLOR_DIGITS = 5;
    static const unsigned MAX_COLUMNS = 70;
    static const unsigned MAX_NUMBERS_PER_ROW =
        MAX_COLUMNS / (1 + MAX_COLOR_DIGITS);
    static constexpr const char* FORMAT_CODE = "P2";
    PGM_IO() = default;
    PGM_IO(const PGM_IO&) = default;
    PGM_IO(PGM_IO&&) = default;
    PGM_IO& operator=(const PGM_IO&) = default;
    PGM_IO& operator=(PGM_IO&&) = default;
    explicit PGM_IO(std::shared_ptr<struct Image> image);
    ~PGM_IO() = default;
    void set_image(const std::shared_ptr<struct Image>& image);
    std::shared_ptr<struct Image> get_image() const;

    friend std::istream& operator>>(std::istream& in, PGM_IO& ppm_io);
};

std::ostream& operator<<(std::ostream& out, const PGM_IO& ppm_io);

#endif
