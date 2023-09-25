//
// Created by simon on 25.09.23.
//

#ifndef FICTION_READ_GATE_LEVEL_LAYOUT_HPP
#define FICTION_READ_GATE_LEVEL_LAYOUT_HPP

#include "fiction/traits.hpp"
#include <fiction/layouts/cartesian_layout.hpp>
#include <fiction/layouts/clocked_layout.hpp>
#include <fiction/layouts/gate_level_layout.hpp>
#include "fiction/utils/name_utils.hpp"

#include <tinyxml2.h>

#include <algorithm>
#include <cctype>
#include <exception>
#include <fstream>
#include <istream>
#include <sstream>
#include <string>
#include <string_view>
#include <unordered_map>

namespace fiction
{

/**
 * Exception thrown when an error occurs during parsing of an FCN file.
 */
class fcn_parsing_error : public std::runtime_error
{
  public:
    explicit fcn_parsing_error(const std::string_view& msg) noexcept : std::runtime_error(msg.data()) {}
};

namespace detail
{

template <typename Lyt>
class read_gate_level_layout_impl
{
  public:
    read_gate_level_layout_impl(std::istream& s, const std::string_view& name) : lyt{}, is{s}
    {
        set_name(lyt, name);
    }

    read_gate_level_layout_impl(Lyt& tgt, std::istream& s) : lyt{tgt}, is{s} {}

    Lyt run()
    {
        // tinyXML2 does not support std::istream, so we have to read the whole file into a string first
        std::stringstream buffer{};
        buffer << is.rdbuf();
        const std::string sqd_content{buffer.str()};

        tinyxml2::XMLDocument xml_document{};
        xml_document.Parse(sqd_content.c_str());

        if (xml_document.ErrorID() != 0)
        {
            throw fcn_parsing_error("Error parsing FCN file: " + std::string(xml_document.ErrorName()));
        }

        auto* const fcn_root = xml_document.FirstChildElement("fcn");
        if (fcn_root == nullptr)
        {
            throw fcn_parsing_error("Error parsing FCN file: no root element 'fcn'");
        }

        auto* const layout = fcn_root->FirstChildElement("layout");
        if (layout == nullptr)
        {
            throw fcn_parsing_error("Error parsing FCN file: no element 'layout'");
        }

        auto* const name = layout->FirstChildElement("name");
        if (name != nullptr)
        {
            set_name(lyt, name->GetText());
        }

        auto* const clocking = layout->FirstChildElement("clocking");
        if (clocking != nullptr)
        {
            lyt.replace_clocking_scheme(*get_clocking_scheme<Lyt>(clocking->GetText()));
        }

        return lyt;
    }

  private:
    /**
     * The layout to which the parsed cells are added.
     */
    Lyt lyt;
    /**
     * The input stream from which the FCN file is read.
     */
    std::istream& is;
};

}  // namespace detail

/**
 * Reads a gate-level layout from an fcn file provided as an input stream. The format is used by fiction
 * (https://github.com/cda-tum/fiction).
 *
 * May throw an `fcn_parsing_exception` if the fcn file is malformed.
 *
 * @tparam Lyt The layout type to be created from an input.
 * @param is The input stream to read from.
 * @param name The name to give to the generated layout.
 */
template <typename Lyt>
Lyt read_gate_level_layout(std::istream& is, const std::string_view& name = "")
{
    detail::read_gate_level_layout_impl<Lyt> p{is, name};

    const auto lyt = p.run();

    return lyt;
}
/**
 * Reads a gate-level layout from an fcn file provided as an input stream. The format is used by fiction
 * (https://github.com/cda-tum/fiction).
 *
 * May throw an `fcn_parsing_exception` if the fcn file is malformed.
 *
 * This is an in-place version of read_gate_level_layout that utilizes the given layout as a target to write to.
 *
 * @tparam Lyt The layout type to be used as input.
 * @param lyt The layout to write to.
 * @param is The input stream to read from.
 */
template <typename Lyt>
void read_gate_level_layout(Lyt& lyt, std::istream& is)
{
    detail::read_gate_level_layout_impl<Lyt> p{lyt, is};

    lyt = p.run();
}
/**
 * Reads a gate-level layout from an fcn file provided as an input stream. The format is used by fiction
 * (https://github.com/cda-tum/fiction).
 *
 * May throw an `fcn_parsing_exception` if the fcn file is malformed.
 *
 * @tparam Lyt The layout type to be created from an input.
 * @param filename The file name to open and read from.
 * @param name The name to give to the generated layout.
 */
template <typename Lyt>
Lyt read_gate_level_layout(const std::string_view& filename, const std::string_view& name = "")
{
    std::ifstream is{filename.data(), std::ifstream::in};

    if (!is.is_open())
    {
        throw std::ifstream::failure("could not open file");
    }

    const auto lyt = read_gate_level_layout<Lyt>(is, name);
    is.close();

    return lyt;
}
/**
 * Reads a gate-level layout from an fcn file provided as an input stream. The format is used by fiction
 * (https://github.com/cda-tum/fiction).

 * May throw an `fcn_parsing_exception` if the fcn file is malformed.
 *
 * This is an in-place version of `read_gate_level_layout` that utilizes the given layout as a target to write to.
 *
 * @tparam Lyt The layout type to be used as input. Must be a cell-level SiDB layout.
 * @param lyt The layout to write to.
 * @param filename The file name to open and read from.
 */
template <typename Lyt>
void read_gate_level_layout(Lyt& lyt, const std::string_view& filename)
{
    std::ifstream is{filename.data(), std::ifstream::in};

    if (!is.is_open())
    {
        throw std::ifstream::failure("could not open file");
    }

    read_gate_level_layout<Lyt>(lyt, is);
    is.close();
}

}  // namespace fiction

#endif  // FICTION_READ_GATE_LEVEL_LAYOUT_HPP
