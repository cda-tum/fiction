//
// Created by marcel on 29.03.22.
//

#ifndef FICTION_READ_SIDB_SURFACE_DEFECTS_HPP
#define FICTION_READ_SIDB_SURFACE_DEFECTS_HPP

#include "fiction/technology/sidb_defect_surface.hpp"
#include "fiction/technology/sidb_defects.hpp"
#include "fiction/traits.hpp"

#include <array>
#include <cstddef>
#include <cstdint>
#include <exception>
#include <fstream>
#include <istream>
#include <regex>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

namespace fiction
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
class missing_sidb_position_exception : public std::exception
{
  public:
    explicit missing_sidb_position_exception(const uint32_t n) noexcept : line{n} {}

    [[nodiscard]] uint32_t where() const noexcept
    {
        return line;
    }

  private:
    const uint32_t line;
};

namespace detail
{

namespace sidb_defects
{

/* Regex */

static const std::regex RE_DEFECT_MATRIX{R"(\[(?:\s*\d+\s*)+\])"};  // each match is one row
static const std::regex RE_ROW_INDICES{R"((\d+))"};                 // each match is one index

/**
 * Maps indices in the data format to defect types.
 */
inline constexpr std::array<sidb_defect_type, 11> INDEX_TO_DEFECT{
    {sidb_defect_type::NONE, sidb_defect_type::DB, sidb_defect_type::SI_VACANCY, sidb_defect_type::DIHYDRIDE_PAIR,
     sidb_defect_type::SINGLE_DIHYDRIDE, sidb_defect_type::ONE_BY_ONE, sidb_defect_type::THREE_BY_ONE,
     sidb_defect_type::SILOXANE, sidb_defect_type::RAISED_SI, sidb_defect_type::ETCH_PIT,
     sidb_defect_type::MISSING_DIMER}};

}  // namespace sidb_defects

// data types cannot properly be converted to bit field types
#pragma GCC diagnostic push
#ifndef __clang__
#pragma GCC diagnostic ignored "-Wuseless-cast"
#endif
#pragma GCC diagnostic ignored "-Wconversion"

template <typename Lyt>
class read_sidb_surface_defects_impl
{
  public:
    explicit read_sidb_surface_defects_impl(std::istream& s, const std::string_view& name) :
            lyt{sidb_defect_surface{Lyt{{}, name.data()}}},
            defect_matrix{std::istreambuf_iterator<char>(s), {}}  // read the stream into a string to perform regex
    {}

    sidb_defect_surface<Lyt> run()
    {
        // each match is one row
        const std::vector<std::smatch> matrix_matches{
            std::sregex_iterator(std::cbegin(defect_matrix), std::cend(defect_matrix), sidb_defects::RE_DEFECT_MATRIX),
            std::sregex_iterator()};

        // iterate over the row matches
        for (auto y = 0u; y < matrix_matches.size(); ++y)
        {
            // get the match as a string
            const auto row_str = matrix_matches[y].str();

            // each match is a defect index
            const std::vector<std::smatch> row_matches{
                std::sregex_iterator(std::cbegin(row_str), std::cend(row_str), sidb_defects::RE_ROW_INDICES),
                std::sregex_iterator()};

            // track x-dimension of the surface
            if (row_matches.size() - 1 > static_cast<std::size_t>(max_cell_pos.x))
            {
                max_cell_pos.x = static_cast<decltype(max_cell_pos.x)>(row_matches.size() - 1);
            }
            else if (static_cast<decltype(max_cell_pos.x)>(row_matches.size() - 1) < max_cell_pos.x)
            {
                // row y has fewer SiDBs than previous rows
                throw missing_sidb_position_exception(y);
            }

            // iterate over the index matches
            for (auto x = 0u; x < row_matches.size(); ++x)
            {
                const auto defect_index = std::stoi(row_matches[x].str());

                try
                {
                    // assign the defect
                    lyt.assign_sidb_defect(
                        {x, y}, sidb_defect{sidb_defects::INDEX_TO_DEFECT.at(static_cast<std::size_t>(defect_index))});
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
    sidb_defect_surface<Lyt> lyt;

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
sidb_defect_surface<Lyt> read_sidb_surface_defects(std::istream& is, const std::string_view& name = "")
{
    static_assert(is_cell_level_layout_v<Lyt>, "Lyt is not a cell-level layout");
    static_assert(has_sidb_technology_v<Lyt>, "Lyt must be an SiDB layout");

    detail::read_sidb_surface_defects_impl<Lyt> p{is, name};

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
sidb_defect_surface<Lyt> read_sidb_surface_defects(const std::string_view& filename, const std::string_view& name = "")
{
    std::ifstream is{filename.data(), std::ifstream::in};

    if (!is.is_open())
    {
        throw std::ifstream::failure("could not open file");
    }

    const auto lyt = read_sidb_surface_defects<Lyt>(is, name);
    is.close();

    return lyt;
}

}  // namespace fiction

#endif  // FICTION_READ_SIDB_SURFACE_DEFECTS_HPP
