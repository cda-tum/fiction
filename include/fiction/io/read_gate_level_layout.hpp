//
// Created by simon on 25.09.23.
//

#ifndef FICTION_READ_GATE_LEVEL_LAYOUT_HPP
#define FICTION_READ_GATE_LEVEL_LAYOUT_HPP

#include "fiction/traits.hpp"
#include "fiction/utils/name_utils.hpp"

#include <fiction/layouts/cartesian_layout.hpp>
#include <fiction/layouts/clocked_layout.hpp>
#include <fiction/layouts/gate_level_layout.hpp>
#include <fiction/layouts/tile_based_layout.hpp>

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
 * Exception thrown when an error occurs during parsing of an gate_level file.
 */
class gate_level_parsing_error : public std::runtime_error
{
  public:
    explicit gate_level_parsing_error(const std::string_view& msg) noexcept : std::runtime_error(msg.data()) {}
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
            throw gate_level_parsing_error("Error parsing gate_level file: " + std::string(xml_document.ErrorName()));
        }

        auto* const fcn_root = xml_document.FirstChildElement("fcn");
        if (fcn_root == nullptr)
        {
            throw gate_level_parsing_error("Error parsing gate_level file: no root element 'fcn'");
        }

        auto* const layout = fcn_root->FirstChildElement("layout");
        if (layout == nullptr)
        {
            throw gate_level_parsing_error("Error parsing gate_level file: no element 'layout'");
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

        auto* const size = layout->FirstChildElement("size");
        if (size != nullptr)
        {
            int       x = std::stoi(size->FirstChildElement("x")->GetText());
            int       y = std::stoi(size->FirstChildElement("y")->GetText());
            int       z = std::stoi(size->FirstChildElement("z")->GetText());
            tile<Lyt> max_pos{x, y, z};
            lyt.resize(max_pos);
        }

        std::vector<Gate> gates{};
        auto* const       gates_xml = fcn_root->FirstChildElement("gates");
        if (gates_xml != nullptr)
        {
            for (const auto* gate_xml = gates_xml->FirstChildElement("gate"); gate_xml != nullptr;
                 gate_xml             = gate_xml->NextSiblingElement("gate"))
            {
                Gate gate;
                gate.id   = std::stoi(gate_xml->FirstChildElement("id")->GetText());
                gate.type = gate_xml->FirstChildElement("type")->GetText();

                auto* const pi_name = gate_xml->FirstChildElement("name");
                if (pi_name != nullptr)
                {
                    gate.name = pi_name->GetText();
                }

                auto* const loc = gate_xml->FirstChildElement("loc");
                gate.loc.x      = std::stoull(loc->FirstChildElement("x")->GetText());
                gate.loc.y      = std::stoull(loc->FirstChildElement("y")->GetText());
                gate.loc.z      = std::stoull(loc->FirstChildElement("z")->GetText());

                auto* const incoming_signals = gate_xml->FirstChildElement("incoming");
                if (incoming_signals != nullptr)
                {
                    for (const auto* signal = incoming_signals->FirstChildElement("signal"); signal != nullptr;
                         signal             = signal->NextSiblingElement("signal"))
                    {
                        tile<Lyt> incoming_signal{};
                        incoming_signal.x = std::stoull(signal->FirstChildElement("x")->GetText());
                        incoming_signal.y = std::stoull(signal->FirstChildElement("y")->GetText());
                        incoming_signal.z = std::stoull(signal->FirstChildElement("z")->GetText());
                        gate.incoming.push_back(incoming_signal);
                    }
                }

                gates.push_back(gate);
            }
        }
        std::sort(gates.begin(), gates.end(), Gate::compareById);

        for (const Gate& gate : gates)
        {
            tile<Lyt> location{gate.loc.x, gate.loc.y, gate.loc.z};
            for (tile<Lyt> i : gate.incoming) std::cout << i << std::endl;
            if (gate.type == "PI")
            {
                lyt.create_pi(gate.name, location);
            }

            else if (gate.incoming.size() == 1)
            {
                auto incoming_tile   = gate.incoming.front();
                auto incoming_signal = lyt.make_signal(lyt.get_node(incoming_tile));

                if (gate.type == "PO")
                {
                    lyt.create_po(lyt.make_signal(lyt.get_node(incoming_tile)), gate.name, location);
                }

                else if (gate.type == "BUF")
                {
                    lyt.create_buf(incoming_signal, location);
                }

                else if (gate.type == "INV")
                {
                    lyt.create_not(incoming_signal, location);
                }
            }

            else if (gate.incoming.size() == 2)
            {
                auto incoming_tile_1 = gate.incoming.front();
                auto incoming_tile_2 = gate.incoming.back();

                auto incoming_signal_1 = lyt.make_signal(lyt.get_node(incoming_tile_1));
                auto incoming_signal_2 = lyt.make_signal(lyt.get_node(incoming_tile_2));

                if (gate.type == "AND")
                {
                    lyt.create_and(incoming_signal_1, incoming_signal_2, location);
                }

                else if (gate.type == "NAND")
                {
                    lyt.create_nand(incoming_signal_1, incoming_signal_2, location);
                }

                else if (gate.type == "OR")
                {
                    lyt.create_or(incoming_signal_1, incoming_signal_2, location);
                }

                else if (gate.type == "NOR")
                {
                    lyt.create_nor(incoming_signal_1, incoming_signal_2, location);
                }

                else if (gate.type == "XOR")
                {
                    lyt.create_xor(incoming_signal_1, incoming_signal_2, location);
                }

                else if (gate.type == "XNOR")
                {
                    lyt.create_xnor(incoming_signal_1, incoming_signal_2, location);
                }
            }
        }

        return lyt;
    }

  private:
    /**
     * The layout to which the parsed cells are added.
     */
    Lyt lyt;
    /**
     * The input stream from which the gate_level file is read.
     */
    std::istream& is;

    struct Gate
    {
        int                    id;
        std::string            type;
        std::string            name;
        tile<Lyt>              loc;
        std::vector<tile<Lyt>> incoming;

        static bool compareById(const Gate& gate1, const Gate& gate2)
        {
            return gate1.id < gate2.id;
        }
    };
};

}  // namespace detail

/**
 * Reads a gate-level layout from a gate_level file provided as an input stream. The format is used by fiction
 * (https://github.com/cda-tum/fiction).
 *
 * May throw an `gate_level_parsing_exception` if the gate_level file is malformed.
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
 * Reads a gate-level layout from an gate_level file provided as an input stream. The format is used by fiction
 * (https://github.com/cda-tum/fiction).
 *
 * May throw an `gate_level_parsing_exception` if the gate_level file is malformed.
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
 * Reads a gate-level layout from a gate_level file provided as an input stream. The format is used by fiction
 * (https://github.com/cda-tum/fiction).
 *
 * May throw an `gate_level_parsing_exception` if the gate_level file is malformed.
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
 * Reads a gate-level layout from an gate_level file provided as an input stream. The format is used by fiction
 * (https://github.com/cda-tum/fiction).

 * May throw an `gate_level_parsing_exception` if the gate_level file is malformed.
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
