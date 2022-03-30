//
// Created by marcel on 29.03.22.
//

#ifndef FICTION_READ_SIDB_SURFACE_DEFECTS_HPP
#define FICTION_READ_SIDB_SURFACE_DEFECTS_HPP

#include "fiction/technology/cell_technologies.hpp"
#include "fiction/technology/sidb_surface.hpp"
#include "fiction/traits.hpp"

#include <exception>
#include <fstream>
#include <istream>
#include <map>
#include <regex>
#include <string>
#include <type_traits>

namespace fiction
{

class unsupported_defect_index_exception : public std::exception
{
  public:
    explicit unsupported_defect_index_exception(const int i) noexcept : std::exception(), unsupported_index{i} {}

    [[nodiscard]] int which() const noexcept
    {
        return unsupported_index;
    }

  private:
    const int unsupported_index;
};

class missing_sidb_position_exception : public std::exception
{
  public:
    explicit missing_sidb_position_exception(const uint32_t n) noexcept : std::exception(), line{n} {}

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

static const std::regex re_defect_matrix{R"(\[(?:\s*\d+\s*)+\])"};  // each match is one row
static const std::regex re_row_indices{R"((\d+))"};                 // each match is one index

/**
 * Maps indices in the data format to defect types.
 */
static const std::map<int, sidb_defect_type> index_to_defect{{{0, sidb_defect_type::NONE},
                                                              {1, sidb_defect_type::DB},
                                                              {2, sidb_defect_type::SI_VACANCY},
                                                              {3, sidb_defect_type::DIHYDRIDE_PAIR},
                                                              {4, sidb_defect_type::SINGLE_DIHYDRIDE},
                                                              {5, sidb_defect_type::ONE_BY_ONE},
                                                              {6, sidb_defect_type::THREE_BY_ONE},
                                                              {7, sidb_defect_type::SILOXANE},
                                                              {8, sidb_defect_type::RAISED_SI},
                                                              {9, sidb_defect_type::ETCH_PIT},
                                                              {10, sidb_defect_type::MISSING_DIMER}}};

}  // namespace sidb_defects

// data types cannot properly be converted to bit field types
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wuseless-cast"
#pragma GCC diagnostic ignored "-Wconversion"

template <typename Lyt>
class read_sidb_surface_defects_impl
{
  public:
    explicit read_sidb_surface_defects_impl(std::istream& s, const std::string& name) :
            lyt{sidb_surface{Lyt{{}, name}}},
            defect_matrix{std::istreambuf_iterator<char>(s), {}}  // read the stream into a string to perform regex
    {}

    sidb_surface<Lyt> run()
    {
        // each match is one row
        const std::vector<std::smatch> matrix_matches{
            std::sregex_iterator(std::cbegin(defect_matrix), std::cend(defect_matrix), sidb_defects::re_defect_matrix),
            std::sregex_iterator()};

        // iterate over the row matches
        for (auto y = 0u; y < matrix_matches.size(); ++y)
        {
            // get the match as a string
            const auto row_str = matrix_matches[y].str();

            // each match is a defect index
            const std::vector<std::smatch> row_matches{
                std::sregex_iterator(std::cbegin(row_str), std::cend(row_str), sidb_defects::re_row_indices),
                std::sregex_iterator()};

            // track x-dimension of the surface
            if (row_matches.size() - 1 > max_cell_pos.x)
            {
                max_cell_pos.x = static_cast<decltype(max_cell_pos.x)>(row_matches.size() - 1);
            }
            else if (row_matches.size() - 1 < max_cell_pos.x)
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
                    lyt.assign_sidb_defect({x, y}, sidb_defects::index_to_defect.at(defect_index));
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
    sidb_surface<Lyt> lyt;

    const std::string defect_matrix;

    cell<Lyt> max_cell_pos{};
};

#pragma GCC diagnostic pop

}  // namespace detail

/**
 * Reads a defective SiDB surface from a text file provided as an input stream. The format is rudimentary and consists
 * of a simple 2D array of integers representing defect indices printed by Python.
 *
 * May throw a 'missing_sidb_position_exception' or an 'unsupported_defect_index_exception'.
 *
 * @tparam Lyt The layout type underlying the SiDB surface. Must be a cell-level SiDB layout.
 * @param is The input stream to read from.
 * @param name The name to give to the generated layout.
 */
template <typename Lyt>
sidb_surface<Lyt> read_sidb_surface_defects(std::istream& is, const std::string& name = "")
{
    static_assert(is_cell_level_layout_v<Lyt>, "Lyt is not a cell-level layout");
    static_assert(std::is_same_v<technology<Lyt>, sidb_technology>, "Lyt must be an SiDB layout");

    detail::read_sidb_surface_defects_impl<Lyt> p{is, name};

    const auto lyt = p.run();

    return lyt;
}
/**
 * Reads a defective SiDB surface from a text file provided as a file name. The format is rudimentary and consists
 * of a simple 2D array of integers representing defect indices printed by Python.
 *
 * May throw a 'missing_sidb_position_exception' or an 'unsupported_defect_index_exception'.
 *
 * @tparam Lyt The layout type underlying the SiDB surface. Must be a cell-level SiDB layout.
 * @param filename The file name to open and read from.
 * @param name The name to give to the generated layout.
 */
template <typename Lyt>
sidb_surface<Lyt> read_sidb_surface_defects(const std::string& filename, const std::string& name = "")
{
    std::ifstream is{filename.c_str(), std::ifstream::in};

    if (!is.is_open())
        throw std::ifstream::failure("could not open file");

    const auto lyt = read_sidb_surface_defects<Lyt>(is, name);
    is.close();

    return lyt;
}

}  // namespace fiction

#endif  // FICTION_READ_SIDB_SURFACE_DEFECTS_HPP
