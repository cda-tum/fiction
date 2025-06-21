//
// Created by marcel on 04.10.21.
//

#ifndef FICTION_READ_FQCA_LAYOUT_HPP
#define FICTION_READ_FQCA_LAYOUT_HPP

#include "fiction/technology/cell_technologies.hpp"
#include "fiction/traits.hpp"

#include <cctype>
#include <cstdint>
#include <exception>
#include <fstream>
#include <istream>
#include <regex>
#include <sstream>
#include <string>
#include <string_view>
#include <unordered_map>

namespace fiction
{

class unsupported_character_exception : public std::exception
{
  public:
    explicit unsupported_character_exception(const char c) noexcept : unsupported_char{c} {}

    [[nodiscard]] char which() const noexcept
    {
        return unsupported_char;
    }

  private:
    char unsupported_char;
};

class undefined_cell_label_exception : public std::exception
{
  public:
    explicit undefined_cell_label_exception(const char l) noexcept : undefined_label{l} {}

    [[nodiscard]] char which() const noexcept
    {
        return undefined_label;
    }

  private:
    char undefined_label;
};

class unrecognized_cell_definition_exception : public std::exception
{
  public:
    explicit unrecognized_cell_definition_exception(const uint64_t n) noexcept : line{n} {}

    [[nodiscard]] uint64_t where() const noexcept
    {
        return line;
    }

  private:
    uint64_t line;
};

namespace detail
{

namespace qca_stack
{

/* Regex */

static const std::regex RE_WHITE_SPACE{R"(\s)"};
static const std::regex RE_COMMENT{R"(\[.*\]$)"};
static const std::regex RE_LAYER_SEPARATOR{R"(^=+$)"};
static const std::regex RE_CELL_DEFINITION_ID{R"(^(\w)\:$)"};              // group 1 is the id
static const std::regex RE_CELL_DEFINITION_LABEL{R"(^-label=\"(.*)\"$)"};  // group 1 is the label
static const std::regex RE_CELL_DEFINITION_CLOCK{R"(^-clock=(\d)$)"};      // group 1 is the clock number
static const std::regex RE_CELL_DEFINITION_NUMBER{R"(^-number=(\d+)$)"};   // group 1 is the number
static const std::regex RE_CELL_DEFINITION_OFFSET{
    R"(^-offset=\((-?\d*(?:\.\d+)?),(-?\d*(?:\.\d+)?),(-?\d*(?:\.\d+)?)\)$)"};  // group 1, 2, and 3 are the x, y, and z
                                                                                // offset respectively

/* Strings */

inline constexpr const char* CELL_DEFINITION_INPUT     = "-input";
inline constexpr const char* CELL_DEFINITION_OUTPUT    = "-output";
inline constexpr const char* CELL_DEFINITION_PROPAGATE = "-propagate";

}  // namespace qca_stack

template <typename Lyt>
class read_fqca_layout_impl
{
  public:
    explicit read_fqca_layout_impl(std::istream& s, const std::string_view& name) : lyt{{}, name.data()}, is{s} {}

    Lyt run()
    {
        uint64_t line_number{1ull};
        for (std::string line{}; std::getline(is, line); ++line_number)
        {
            // remove all white space from the line to make regex matching easier and more robust
            const auto substituted_line = std::regex_replace(line, qca_stack::RE_WHITE_SPACE, "");

            // skip empty lines
            if (!line.empty())
            {
                // are we currently parsing the layout definition...
                if (parsing_status == fqca_section::LAYOUT_DEFINITION)
                {
                    // if line is a comment
                    if (std::regex_match(substituted_line, qca_stack::RE_COMMENT))
                    {
                        continue;
                    }
                    // if line is a layer separator
                    if (std::regex_match(substituted_line, qca_stack::RE_LAYER_SEPARATOR))
                    {
                        // reset cell row
                        current_cell_row = 0ull;
                        // increment layer
                        ++next_cell_layer;
                    }
                    // parse line
                    else
                    {
                        // section delimiter
                        if (substituted_line == "$")
                        {
                            parsing_status = fqca_section::CELL_DEFINITION;
                        }
                        // line must represent a row of cells
                        else
                        {
                            for (auto i = 0ull, current_cell_column = 0ull; i < line.size();
                                 i += 2, ++current_cell_column)
                            {
                                parse_cell(line[i], {current_cell_column, current_cell_row, next_cell_layer - 1});
                            }

                            // another row done
                            ++current_cell_row;
                        }
                    }
                }
                // ... or the cell definition?
                else if (parsing_status == fqca_section::CELL_DEFINITION)
                {
                    // skip empty substituted lines
                    if (!substituted_line.empty())
                    {
                        // if line indicates a new cell id
                        if (std::smatch sm; std::regex_match(substituted_line, sm, qca_stack::RE_CELL_DEFINITION_ID))
                        {
                            // the cell id is captured in the first regex group, whose result is a single character
                            const auto cell_id = sm.str(1)[0];

                            if (auto it = cell_label_map.find(cell_id); it != cell_label_map.cend())
                            {
                                current_labeled_cell = it->second;
                            }
                            else
                            {
                                throw undefined_cell_label_exception(cell_id);
                            }
                        }
                        // if line indicates a primary input flag
                        else if (substituted_line == qca_stack::CELL_DEFINITION_INPUT)
                        {
                            lyt.assign_cell_type(current_labeled_cell, technology<Lyt>::cell_type::INPUT);
                        }
                        // if line indicates a primary output flag
                        else if (substituted_line == qca_stack::CELL_DEFINITION_OUTPUT)
                        {
                            lyt.assign_cell_type(current_labeled_cell, technology<Lyt>::cell_type::OUTPUT);
                        }
                        // if line indicates a cell label
                        else if (std::regex_match(substituted_line, sm, qca_stack::RE_CELL_DEFINITION_LABEL))
                        {
                            // the cell label is captured in the first regex group
                            const auto cell_label = sm.str(1);

                            lyt.assign_cell_name(current_labeled_cell, cell_label);
                        }
                        // if line indicates a cell label
                        else if (std::regex_match(substituted_line, sm, qca_stack::RE_CELL_DEFINITION_CLOCK))
                        {
                            // the clock number is captured in the first regex group, whose result is a single number
                            const auto clock_number_char = sm.str(1)[0];

                            lyt.assign_clock_number(current_labeled_cell, to_clock_number(clock_number_char));
                        }

                        // NOLINTBEGIN(*-branch-clone)

                        // if line indicates a propagate flag
                        else if (substituted_line == qca_stack::CELL_DEFINITION_PROPAGATE)
                        {
                            // 'propagate' is not supported
                        }
                        // if line indicates a number definition
                        else if (std::regex_match(substituted_line, sm, qca_stack::RE_CELL_DEFINITION_NUMBER))
                        {
                            // 'number' is not supported
                        }
                        // if line indicates an offset definition
                        else if (std::regex_match(substituted_line, sm, qca_stack::RE_CELL_DEFINITION_OFFSET))
                        {
                            // 'offset' is not supported
                        }

                        // NOLINTEND(*-branch-clone)

                        else
                        {
                            throw unrecognized_cell_definition_exception(line_number);
                        }
                    }
                }
            }
        }

        // resize the layout to fit all cells
        lyt.resize(aspect_ratio_t<Lyt>{max_cell_pos});

        return lyt;
    }

  private:
    Lyt lyt;

    std::istream& is;

    enum class fqca_section
    {
        LAYOUT_DEFINITION,
        CELL_DEFINITION
    };

    fqca_section parsing_status = fqca_section::LAYOUT_DEFINITION;

    uint64_t next_cell_layer{
        0ull},  // refers to the next layer to parse due to the initial layer separator in the file format
        current_cell_row{0ull};  // refers to the row that is currently being parsed

    cell<Lyt> max_cell_pos{}, current_labeled_cell{};

    std::unordered_map<char, cell<Lyt>> cell_label_map{};

    typename Lyt::clock_number_t to_clock_number(const char c) const noexcept
    {
        return static_cast<typename Lyt::clock_number_t>(c) % lyt.num_clocks();
    }

    void parse_cell(const char c, cell<Lyt> cell)
    {
        // if c is a space
        if (std::isspace(c) != 0)
        {
            // do nothing
            return;
        }
        // if c is a number
        if (std::isdigit(c) != 0)
        {
            lyt.assign_cell_type(cell, technology<Lyt>::cell_type::NORMAL);

            // assign the clock number modulo the number of clocks in its regime
            lyt.assign_clock_number(cell, to_clock_number(c));
        }
        // if c is a letter
        else if (std::isalpha(c) != 0)
        {
            lyt.assign_cell_type(cell, technology<Lyt>::cell_type::NORMAL);

            // store the cell for later lookup
            cell_label_map[c] = cell;
        }
        // fixed 0 cell
        else if (c == '-')
        {
            lyt.assign_cell_type(cell, technology<Lyt>::cell_type::CONST_0);
        }
        // fixed + cell
        else if (c == '+')
        {
            lyt.assign_cell_type(cell, technology<Lyt>::cell_type::CONST_1);
        }
        else
        {
            throw unsupported_character_exception(c);
        }

        // if cell is not in ground layer
        if (cell.z != 0)
        {
            // if cell is in an even crossing layer
            if (cell.z % 2 == 0)
            {
                lyt.assign_cell_mode(cell, technology<Lyt>::cell_mode::CROSSOVER);
            }
            // odd crossing layer (assuming it's a via)
            else
            {
                lyt.assign_cell_mode(cell, technology<Lyt>::cell_mode::VERTICAL);
            }
        }

        // store latest cell position via bounding box
        if (cell.x > max_cell_pos.x)
        {
            max_cell_pos.x = cell.x;
        }
        if (cell.y > max_cell_pos.y)
        {
            max_cell_pos.y = cell.y;
        }
        if (cell.z > max_cell_pos.z)
        {
            max_cell_pos.z = cell.z;
        }
    }
};

}  // namespace detail

/**
 * Reads a cell-level QCA layout from an fqca file provided as an input stream. The format is used by QCA-STACK by
 * Willem Lambooy (https://github.com/wlambooy/QCA-STACK).
 *
 * May throw an unsupported_character_exception, undefined_cell_label_exception, or
 * unrecognized_cell_definition_exception.
 *
 * @tparam Lyt The layout type to be created from an input. Must be a clocked cell-level QCA layout.
 * @param is The input stream to read from.
 * @param layout_name The name to give to the generated layout.
 */
template <typename Lyt>
Lyt read_fqca_layout(std::istream& is, const std::string_view& layout_name = "")
{
    static_assert(is_cell_level_layout_v<Lyt>, "Lyt is not a cell-level layout");
    static_assert(has_qca_technology_v<Lyt>, "Lyt must be a QCA layout");

    detail::read_fqca_layout_impl<Lyt> p{is, layout_name};

    const auto lyt = p.run();

    return lyt;
}
/**
 * Reads a cell-level QCA layout from an fqca file provided as a file name. The format is used by QCA-STACK by
 * Willem Lambooy (https://github.com/wlambooy/QCA-STACK).
 *
 * May throw an unsupported_character_exception, undefined_cell_label_exception, or
 * unrecognized_cell_definition_exception.
 *
 * @tparam Lyt The layout type to be created from an input. Must be a clocked cell-level QCA layout.
 * @param filename The file name to open and read from.
 * @param layout_name The name to give to the generated layout.
 */
template <typename Lyt>
Lyt read_fqca_layout(const std::string_view& filename, const std::string_view& layout_name = "")
{
    std::ifstream is{filename.data(), std::ifstream::in};

    if (!is.is_open())
    {
        throw std::ifstream::failure("could not open file");
    }

    const auto lyt = read_fqca_layout<Lyt>(is, layout_name);
    is.close();

    return lyt;
}

}  // namespace fiction

#endif  // FICTION_READ_FQCA_LAYOUT_HPP
