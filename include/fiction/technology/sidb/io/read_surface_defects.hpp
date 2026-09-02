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

#include "fiction/technology/sidb/model/defect.hpp"
#include "fiction/technology/sidb/surfaces/defect_surface.hpp"
#include "fiction/traits.hpp"

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
    explicit unsupported_defect_index_exception(const int i) noexcept : unsupported_index{i} {}

    [[nodiscard]] int which() const noexcept
    {
        return unsupported_index;
    }

  private:
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

template <typename Lyt>
class read_surface_defects_impl
{
  public:
    explicit read_surface_defects_impl(std::istream& s, const std::string_view& name) :
            lyt{sidb::surfaces::defect_surface{Lyt{{}, name.data()}}},
            defect_matrix{std::istreambuf_iterator<char>(s), {}}  // read the stream into a string to perform regex
    {}

    sidb::surfaces::defect_surface<Lyt> run()
    {
        // each match is one row
        const std::vector<std::smatch> matrix_matches{
            std::sregex_iterator(std::cbegin(defect_matrix), std::cend(defect_matrix), defects::RE_DEFECT_MATRIX),
            std::sregex_iterator()};

        // iterate over the row matches
        for (auto y = 0u; y < matrix_matches.size(); ++y)
        {
            // get the match as a string
            const auto row_str = matrix_matches[y].str();

            // each match is a defect index
            const std::vector<std::smatch> row_matches{
                std::sregex_iterator(std::cbegin(row_str), std::cend(row_str), defects::RE_ROW_INDICES),
                std::sregex_iterator()};

            // track x-dimension of the surface
            if (row_matches.size() - 1 > static_cast<std::size_t>(max_cell_pos.x))
            {
                max_cell_pos.x = static_cast<decltype(max_cell_pos.x)>(row_matches.size() - 1);
            }
            else if (static_cast<decltype(max_cell_pos.x)>(row_matches.size() - 1) < max_cell_pos.x)
            {
                // row y has fewer SiDBs than previous rows
                throw missing_position_exception(y);
            }

            // iterate over the index matches
            for (auto x = 0u; x < row_matches.size(); ++x)
            {
                const auto defect_index = std::stoi(row_matches[x].str());

                try
                {
                    // assign the defect
                    lyt.assign_defect({x, y}, sidb::model::defect{
                                                  defects::INDEX_TO_DEFECT.at(static_cast<std::size_t>(defect_index))});
                }
                catch (const std::out_of_range&)
                {
                    // defect index does not match any supported defects
                    throw unsupported_defect_index_exception(defect_index);
                }
            }
        }

        // y-dimension of the surface
        max_cell_pos.y = static_cast<decltype(max_cell_pos.y)>(matrix_matches.empty() ? 0 : matrix_matches.size() - 1);

        // resize the layout to fit all surface defects
        lyt.resize(max_cell_pos);

        return lyt;
    }

  private:
    sidb::surfaces::defect_surface<Lyt> lyt;

    const std::string defect_matrix;

    cell<Lyt> max_cell_pos{};
};

#pragma GCC diagnostic pop

}  // namespace detail

/**
 * Reads a defective SiDB surface from a text file provided as an input stream. The format is rudimentary and consists
 * of a simple 2D array of integers representing defect indices printed by Python.
 *
 * May throw a `missing_sidb_position_exception` or an `unsupported_defect_index_exception`.
 *
 * @note For testing purposes, a Python script that generates defective surfaces is provided in the
 * `experiments/defect_aware_physical_design` directory.
 *
 * @tparam Lyt The layout type underlying the SiDB surface. Must be a cell-level SiDB layout.
 * @param is The input stream to read from.
 * @param name The name to give to the generated layout.
 */
template <typename Lyt>
sidb::surfaces::defect_surface<Lyt> read_surface_defects(std::istream& is, const std::string_view& name = "")
{
    static_assert(is_cell_level_layout_v<Lyt>, "Lyt is not a cell-level layout");
    static_assert(has_sidb_technology_v<Lyt>, "Lyt must be an SiDB layout");

    detail::read_surface_defects_impl<Lyt> p{is, name};

    const auto lyt = p.run();

    return lyt;
}
/**
 * Reads a defective SiDB surface from a text file provided as a file name. The format is rudimentary and consists
 * of a simple 2D array of integers representing defect indices printed by Python.
 *
 * May throw a `missing_sidb_position_exception` or an `unsupported_defect_index_exception`.
 *
 * @note For testing purposes, a Python script that generates defective surfaces is provided in the
 * `experiments/defect_aware_physical_design` directory.
 *
 * @tparam Lyt The layout type underlying the SiDB surface. Must be a cell-level SiDB layout.
 * @param filename The file name to open and read from.
 * @param name The name to give to the generated layout.
 */
template <typename Lyt>
sidb::surfaces::defect_surface<Lyt> read_surface_defects(const std::string_view& filename,
                                                         const std::string_view& name = "")
{
    std::ifstream is{std::string{filename}, std::ifstream::in};

    if (!is.is_open())
    {
        throw std::ifstream::failure("could not open file");
    }

    const auto lyt = read_surface_defects<Lyt>(is, name);
    is.close();

    return lyt;
}

}  // namespace fiction::sidb::io
