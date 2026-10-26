/*
 * Copyright (c) 2018 - 2023 Marcel Walter
 * Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
 * All rights reserved.
 *
 * SPDX-License-Identifier: MIT
 *
 * Licensed under the MIT License
 */

/**
 * @file
 * @brief Reader for files pairing a hexadecimal truth table with its Boolean expression.
 * @author Marcel Walter (marcelwa)
 */

#pragma once

#include <kitty/constructors.hpp>
#include <kitty/dynamic_truth_table.hpp>

#include <algorithm>
#include <bit>
#include <cstddef>
#include <cstdint>
#include <fstream>
#include <ios>
#include <istream>
#include <iterator>
#include <optional>
#include <stdexcept>
#include <string>
#include <vector>

namespace fiction::synthesis::io
{

/**
 * Reads whitespace-separated pairs of a hexadecimal truth table and its corresponding Boolean expression.
 * Truth tables use a `0x` or `0X` prefix and a power-of-two number of hexadecimal digits. A single digit represents
 * two variables. Expressions must be single tokens; the reader does not parse them.
 *
 * This format is used by, e.g., Alan Mishchenko for his DSD functions: https://people.eecs.berkeley.edu/~alanmi/temp5/
 */
template <typename TT = kitty::dynamic_truth_table>
class tt_reader
{
  public:
    /**
     * Standard constructor. Reads the given stream into a vector of tokens.
     *
     * @param stream Stream to parse.
     */
    explicit tt_reader(std::istream& stream)
    {
        read(stream);
    }
    /**
     * Standard constructor. Reads the given file into a vector of tokens.
     *
     * @param filename File to parse.
     * @throws std::ifstream::failure If the file cannot be opened.
     */
    explicit tt_reader(const std::string& filename)
    {
        std::ifstream file{filename, std::ios::in};
        if (!file.is_open())
        {
            throw std::ifstream::failure("could not open file");
        }
        read(file);
    }
    /**
     * Returns the next truth table, or `std::nullopt` when all records have been read.
     * A parsing error leaves the current record unchanged.
     *
     * @return Next truth table from the file.
     * @throws std::invalid_argument If a record is incomplete, has an invalid prefix, digit count, or character, or
     * requires 38 or more variables.
     */
    std::optional<TT> next()
    {
        if (pos >= elements.size())
        {
            return std::nullopt;
        }

        if (elements.size() - pos < 2u || !(elements[pos].starts_with("0x") || elements[pos].starts_with("0X")))
        {
            throw std::invalid_argument("truth-table records require a prefixed hexadecimal table and an expression");
        }

        /** @brief Hexadecimal digits without the prefix. */
        const auto table = elements[pos].substr(2u);
        if (!std::has_single_bit(table.size()) ||
            !std::ranges::all_of(
                table, [](const char c)
                { return (c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F'); }))
        {
            throw std::invalid_argument("truth tables require a power-of-two number of hexadecimal digits");
        }
        /** @brief Number of variables represented by the hexadecimal digits. */
        const auto num_vars = static_cast<uint32_t>(std::bit_width(table.size())) + 1u;
        // kitty's dynamic truth table uses a 32-bit shift to allocate blocks above six variables.
        if (num_vars >= 38u)
        {
            throw std::invalid_argument("truth tables require fewer than 38 variables");
        }
        /** @brief Truth table represented by the current record. */
        TT tt{num_vars};
        kitty::create_from_hex_string(tt, table);

        pos += 2;

        return tt;
    }

  private:
    /**
     * Reads hexadecimal truth table representations as well as Boolean expressions from the given stream.
     *
     * @param in_stream Stream of truth table representations.
     */
    void read(std::istream& in_stream)
    {
        elements = std::vector<std::string>{std::istream_iterator<std::string>{in_stream},
                                            std::istream_iterator<std::string>{}};
    }
    /**
     * Contains the hexadecimal truth tables and their Boolean expressions in string representation. The two are
     * alternating.
     */
    std::vector<std::string> elements;
    /**
     * Current position in the vector.
     */
    std::size_t pos = 0ul;
};

}  // namespace fiction::synthesis::io
