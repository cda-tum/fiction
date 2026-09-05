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
 * @brief Reader for defective SiDB surfaces in a plain-text index format.
 * @author Marcel Walter (marcelwa)
 * @author Jan Drewniok (Drewniok)
 */

#pragma once

#include "fiction/technology/sidb/lattice.hpp"
#include "fiction/technology/sidb/layout.hpp"
#include "fiction/technology/sidb/model/defect.hpp"
#include "fiction/traits.hpp"

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <exception>
#include <fstream>
#include <istream>
#include <iterator>
#include <regex>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

namespace fiction::sidb::io
{

/**
 * Exception thrown when an unsupported defect index is encountered in the parsed file.
 */
class unsupported_defect_index_exception : public std::exception
{
  public:
    /**
     * @param i Unsupported index, or -1 if the input exceeds the int range.
     */
    explicit unsupported_defect_index_exception(const int i) noexcept : unsupported_index{i} {}

    /**
     * @return Unsupported index, or -1 if the input exceeds the int range.
     */
    [[nodiscard]] int which() const noexcept
    {
        return unsupported_index;
    }

  private:
    /**
     * Unsupported index, or -1 for an index outside the int range.
     */
    const int unsupported_index;
};
/**
 * Exception thrown when a missing SiDB position is encountered in the parsed file.
 */
class missing_position_exception : public std::exception
{
  public:
    explicit missing_position_exception(const uint32_t n) noexcept : line{n} {}

    [[nodiscard]] uint32_t where() const noexcept
    {
        return line;
    }

  private:
    const uint32_t line;
};

namespace detail
{

namespace defects
{

/* Regex */

inline const std::regex RE_DEFECT_MATRIX{R"(\[(?:\s*\d+\s*)+\])"};  // each match is one row
inline const std::regex RE_ROW_INDICES{R"((\d+))"};                 // each match is one index

/**
 * Maps indices in the data format to defect types.
 */
inline constexpr std::array<sidb::model::defect_type, 11> INDEX_TO_DEFECT{
    {sidb::model::defect_type::NONE, sidb::model::defect_type::DB, sidb::model::defect_type::SI_VACANCY,
     sidb::model::defect_type::DIHYDRIDE_PAIR, sidb::model::defect_type::SINGLE_DIHYDRIDE,
     sidb::model::defect_type::ONE_BY_ONE, sidb::model::defect_type::THREE_BY_ONE, sidb::model::defect_type::SILOXANE,
     sidb::model::defect_type::RAISED_SI, sidb::model::defect_type::ETCH_PIT, sidb::model::defect_type::MISSING_DIMER}};

}  // namespace defects

// data types cannot properly be converted to bit field types
#pragma GCC diagnostic push
#ifndef __clang__
#pragma GCC diagnostic ignored "-Wuseless-cast"
#endif
#pragma GCC diagnostic ignored "-Wconversion"

#pragma GCC diagnostic pop

/**
 * Reads a defect matrix into a defects-only `sidb::layout`: entry `x` of row `y` becomes the defect at column `x` of
 * single-SiDB row `y`.
 */
class surface_defects_reader
{
  public:
    /**
     * Reads the defect matrix text and sets the layout name.
     *
     * @param s Input stream.
     * @param name Layout name.
     */
    surface_defects_reader(std::istream& s, const std::string_view& name) :
            defect_matrix{std::istreambuf_iterator<char>(s), {}}  // read the stream into a string to perform regex
    {
        lyt.set_layout_name(std::string{name});
    }

    /**
     * Assigns each matrix entry to its site on the H-Si(100)-2x1 lattice.
     *
     * @return Layout containing the parsed defects.
     * @throws unsupported_defect_index_exception if an index is unsupported or exceeds the integer range.
     * @throws missing_position_exception if a row is shorter than a preceding row.
     */
    layout run()
    {
        // each match is one row
        const std::vector<std::smatch> matrix_matches{
            std::sregex_iterator(std::cbegin(defect_matrix), std::cend(defect_matrix), defects::RE_DEFECT_MATRIX),
            std::sregex_iterator()};

        std::size_t row_length = 0;

        for (std::size_t y = 0; y < matrix_matches.size(); ++y)
        {
            const auto row_str = matrix_matches[y].str();

            // each match is a defect index
            const std::vector<std::smatch> row_matches{
                std::sregex_iterator(std::cbegin(row_str), std::cend(row_str), defects::RE_ROW_INDICES),
                std::sregex_iterator()};

            if (row_matches.size() < row_length)
            {
                // row y has fewer SiDBs than previous rows
                throw missing_position_exception(static_cast<uint32_t>(y));
            }

            row_length = std::max(row_length, row_matches.size());

            for (std::size_t x = 0; x < row_matches.size(); ++x)
            {
                int defect_index{-1};

                try
                {
                    defect_index = std::stoi(row_matches[x].str());
                    lyt.assign_defect(
                        site_at_row(static_cast<int32_t>(x), static_cast<int32_t>(y)),
                        sidb::model::defect{defects::INDEX_TO_DEFECT.at(static_cast<std::size_t>(defect_index))});
                }
                catch (const std::out_of_range&)
                {
                    // defect index does not match any supported defects
                    throw unsupported_defect_index_exception(defect_index);
                }
            }
        }

        return lyt;
    }

  private:
    /**
     * Layout being populated.
     */
    layout lyt{};
    /**
     * Matrix text read from the input stream.
     */
    const std::string defect_matrix;
};

}  // namespace detail

/**
 * Reads a defect matrix from a stream into a defects-only `sidb::layout` on the H-Si(100)-2x1 lattice. Each `[...]`
 * row lists one defect index per column; entry `x` of row `y` becomes the defect at column `x` of single-SiDB row `y`.
 *
 * @param is The input stream to read from.
 * @param name The name to give to the layout.
 * @return The layout holding the defects.
 * @throws unsupported_defect_index_exception if an index names no known defect type.
 * @throws missing_position_exception if a row is shorter than a row before it.
 */
[[nodiscard]] inline layout read_surface_defects(std::istream& is, const std::string_view& name = "")
{
    detail::surface_defects_reader p{is, name};

    return p.run();
}
/**
 * Reads a defect matrix from a file into a defects-only `sidb::layout`. See the stream overload for the format.
 *
 * @param filename The file to read.
 * @param name The name to give to the layout.
 * @return The layout holding the defects.
 * @throws std::ifstream::failure if the file cannot be opened.
 */
[[nodiscard]] inline layout read_surface_defects(const std::string_view& filename, const std::string_view& name = "")
{
    std::ifstream is{std::string{filename}, std::ifstream::in};

    if (!is.is_open())
    {
        throw std::ifstream::failure("could not open file");
    }

    auto lyt = read_surface_defects(is, name);
    is.close();

    return lyt;
}

}  // namespace fiction::sidb::io
