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

/**
 * \brief Input and output operations for PGM image format.
 *
 * The class allows to read and write ::Image
 * from stream that contains image in
 * <a href="http://netpbm.sourceforge.net/doc/pgm.html">"plain" PGM</a>
 * and write it to stream in this format.
 *
 * The structure is very simple.
 * All chunks of information are separated by whitespace
 * (space, tab or new line).
 * Let's call one chunk an \b instruction.
 * All instructions except comments and the first instruction
 * are regular ASCII numbers.
 * Comments start with `#` character
 * and go to the end of the line where the symbol occurs.
 * PGM file contains the following instructions in the following order:
 *
 * - Two magic characters: `P2`;
 * - Two numbers:
 *   width (number of columns) and height (number of rows);
 * - Maximum gray value;
 * - List of intensities in row-major order.
 */
class PGM_IO
{
private:
    /**
     * \brief Pointer to an ::Image the ::PGM_IO had read or needs to write.
     */
    std::shared_ptr<struct Image> image;
    /**
     * \brief Read the next chunk ignoring comments.
     *
     * \return String with instruction,
     * or blank string if input stream is finished.
     */
    static std::string read_pgm_instruction(std::istream& in);
public:
    /**
     * \brief Maximum value of maximum gray value.
     */
    static const unsigned MAX_VALUE_LIMIT = 1u << 16u;
    /**
     * \brief Maximal number of decimal digits in intensity integer.
     *
     * Exact formula is
     *
     * \f[
     *  MAX\_COLOR\_DIGITS =
     *  \left\lceil \log_{10}{MAX\_VALUE\_LIMIT} \right\rceil
     * \f]
     *
     * Though, `log10` function seems not to have `constexpr` overload,
     * so it's needed to hardcode the constant.
     */
    static const unsigned MAX_COLOR_DIGITS = 5;
    /**
     * \brief Maximal number of symbols per line.
     *
     * In description of the plain PGM format
     * it's said that no line should be longer than `70` characters.
     */
    static const unsigned MAX_COLUMNS = 70;
    /**
     * \brief Maximal number of instructions per line.
     *
     * It's easier to limit number of instruction per line
     * using the longest possible size,
     * than calculate length of each instruction in runtime.
     */
    static const unsigned MAX_NUMBERS_PER_ROW =
        MAX_COLUMNS / (1 + MAX_COLOR_DIGITS);
    /**
     * \brief Magic string that identifies the plain PGM format.
     */
    static constexpr const char* FORMAT_CODE = "P2";
    /**
     * \brief Default constructor.
     */
    PGM_IO() = default;
    /**
     * \brief Default copy constructor just makes a shallow copy.
     */
    PGM_IO(const PGM_IO&) = default;
    /**
     * \brief Default move constructor.
     */
    PGM_IO(PGM_IO&&) = default;
    /**
     * \brief Default copy assignment operator.
     */
    PGM_IO& operator=(const PGM_IO&) = default;
    /**
     * \brief Default move assignment operator.
     */
    PGM_IO& operator=(PGM_IO&&) = default;
    /**
     * \brief Constructor with specific image.
     *
     * If we want to write a file,
     * we may want to create a ::PGM_IO instance
     * with ::Image specified immediately.
     */
    explicit PGM_IO(std::shared_ptr<struct Image> image);
    /**
     * \brief Default destructor.
     */
    ~PGM_IO() = default;
    /**
     * \brief Image setter.
     */
    void set_image(const std::shared_ptr<struct Image>& image);
    /**
     * \brief Image getter.
     */
    std::shared_ptr<struct Image> get_image() const;
    /**
     * \brief Overload of `>>` operator to read an image from input stream.
     *
     * Sets
     * <a href="https://en.cppreference.com/w/cpp/io/ios_base/iostate">
     * failbit</a>
     * in the case of wrong file format (including blank file).
     */
    friend std::istream& operator>>(std::istream& in, PGM_IO& ppm_io);
};

std::ostream& operator<<(std::ostream& out, const PGM_IO& ppm_io);

#endif
