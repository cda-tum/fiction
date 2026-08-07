//
// Created by simon on 25.09.23.
//

#ifndef FICTION_READ_FGL_LAYOUT_HPP
#define FICTION_READ_FGL_LAYOUT_HPP

// clang-format off
// NOLINTBEGIN(misc-include-cleaner): no symbol from these headers is named directly, but clocked_layout.hpp's
// get_clocking_scheme free function is looked up via two-phase name lookup at template instantiation time, so
// removing any of these breaks the build despite the tool's "not used directly" heuristic
#include "fiction/layouts/cartesian_layout.hpp"
#include "fiction/layouts/clocked_layout.hpp"
#include "fiction/layouts/gate_level_layout.hpp"
#include "fiction/layouts/tile_based_layout.hpp"
// NOLINTEND(misc-include-cleaner)
// clang-format on
#include "fiction/traits.hpp"
#include "fiction/utils/name_utils.hpp"

#include <kitty/constructors.hpp>
#include <kitty/dynamic_truth_table.hpp>
#include <mockturtle/traits.hpp>
#include <tinyxml2.h>

#include <algorithm>
#include <array>
#include <cctype>
#include <cstdint>
#include <cstdlib>
#include <fstream>
#include <istream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

namespace fiction
{

/**
 * Exception thrown when an error occurs during parsing of a .fgl file containing a gate-level layout.
 */
class fgl_parsing_error : public std::runtime_error
{
  public:
    /**
     * Constructs a `fgl_parsing_error` object with the given error message.
     *
     * @param msg The error message describing the parsing error.
     */
    explicit fgl_parsing_error(const std::string_view& msg) noexcept : std::runtime_error(std::string{msg}) {}
};

namespace detail
{

template <typename Lyt>
class read_fgl_layout_impl
{
  public:
    read_fgl_layout_impl(std::istream& s, const std::string_view& name) : lyt{}, is{s}
    {
        set_name(lyt, name);
    }

    read_fgl_layout_impl(Lyt& tgt, std::istream& s) : lyt{tgt}, is{s} {}

    Lyt run()
    {
        // tinyXML2 does not support std::istream, so we have to read the whole file into a string first
        std::stringstream buffer{};
        buffer << is.rdbuf();
        const std::string fgl_content{buffer.str()};

        // parse xml file
        tinyxml2::XMLDocument xml_document{};
        xml_document.Parse(fgl_content.c_str());

        if (xml_document.ErrorID() != 0)
        {
            throw fgl_parsing_error(fmt::format("Error parsing FGL file: {}", xml_document.ErrorName()));
        }

        auto* const fgl_root = xml_document.FirstChildElement("fgl");
        if (fgl_root == nullptr)
        {
            throw fgl_parsing_error("Error parsing FGL file: no root element 'fgl'");
        }

        auto* const layout = fgl_root->FirstChildElement("layout");
        if (layout == nullptr)
        {
            throw fgl_parsing_error("Error parsing FGL file: no element 'layout'");
        }

        // set layout name
        if (auto* const name = layout->FirstChildElement("name"); name != nullptr && (name->GetText() != nullptr))
        {
            std::string layout_name = name->GetText();
            set_name(lyt, layout_name);
        }

        // check topology
        if (auto* const topology = layout->FirstChildElement("topology");
            topology != nullptr && (topology->GetText() != nullptr))
        {
            const std::string                           topology_name = topology->GetText();
            static constexpr std::array<const char*, 4> shifted_cartesian{
                "odd_row_cartesian", "even_row_cartesian", "odd_column_cartesian", "even_column_cartesian"};
            static constexpr std::array<const char*, 4> hex{"odd_row_hex", "even_row_hex", "odd_column_hex",
                                                            "even_column_hex"};

            if (topology_name == "cartesian")
            {
                if constexpr (!is_cartesian_layout_v<Lyt>)
                {
                    throw fgl_parsing_error("Error parsing FGL file: Lyt is not a cartesian layout");
                }
            }
            else if (std::ranges::find(shifted_cartesian, topology_name) != shifted_cartesian.cend())
            {
                if constexpr (is_shifted_cartesian_layout_v<Lyt>)
                {
                    if (topology_name == "odd_row_cartesian")
                    {
                        if constexpr (!has_odd_row_cartesian_arrangement_v<Lyt>)
                        {
                            throw fgl_parsing_error("Error parsing FGL file: Lyt is not an odd_row_cartesian layout");
                        }
                    }
                    else if (topology_name == "even_row_cartesian")
                    {
                        if constexpr (!has_even_row_cartesian_arrangement_v<Lyt>)
                        {
                            throw fgl_parsing_error("Error parsing FGL file: Lyt is not an even_row_cartesian layout");
                        }
                    }
                    else if (topology_name == "odd_column_cartesian")
                    {
                        if constexpr (!has_odd_column_cartesian_arrangement_v<Lyt>)
                        {
                            throw fgl_parsing_error(
                                "Error parsing FGL file: Lyt is not an odd_column_cartesian layout");
                        }
                    }
                    else if (topology_name == "even_column_cartesian")
                    {
                        if constexpr (!has_even_column_cartesian_arrangement_v<Lyt>)
                        {
                            throw fgl_parsing_error(
                                "Error parsing FGL file: Lyt is not an even_column_cartesian layout");
                        }
                    }
                }
                else
                {
                    throw fgl_parsing_error("Error parsing FGL file: Lyt is not a shifted_cartesian layout");
                }
            }
            else if (std::ranges::find(hex, topology_name) != hex.cend())
            {
                if constexpr (is_hexagonal_layout_v<Lyt>)
                {
                    if (topology_name == "odd_row_hex")
                    {
                        if constexpr (!has_odd_row_hex_arrangement_v<Lyt>)
                        {
                            throw fgl_parsing_error("Error parsing FGL file: Lyt is not an odd_row_hex layout");
                        }
                    }
                    else if (topology_name == "even_row_hex")
                    {
                        if constexpr (!has_even_row_hex_arrangement_v<Lyt>)
                        {
                            throw fgl_parsing_error("Error parsing FGL file: Lyt is not an even_row_hex layout");
                        }
                    }
                    else if (topology_name == "odd_column_hex")
                    {
                        if constexpr (!has_odd_column_hex_arrangement_v<Lyt>)
                        {
                            throw fgl_parsing_error("Error parsing FGL file: Lyt is not an odd_column_hex layout");
                        }
                    }
                    else if (topology_name == "even_column_hex")
                    {
                        if constexpr (!has_even_column_hex_arrangement_v<Lyt>)
                        {
                            throw fgl_parsing_error("Error parsing FGL file: Lyt is not an even_column_hex layout");
                        }
                    }
                }
                else
                {
                    throw fgl_parsing_error("Error parsing FGL file: Lyt is not a hexagonal layout");
                }
            }
            else
            {
                throw fgl_parsing_error(fmt::format("Error parsing FGL file: unknown topology: {}", topology_name));
            }
        }
        else
        {
            throw fgl_parsing_error("Error parsing FGL file: no element 'topology' in 'layout'");
        }

        // set layout size
        if (auto* const size = layout->FirstChildElement("size"); size != nullptr)
        {
            int x = 0;
            if (auto* const size_x = size->FirstChildElement("x"); size_x != nullptr && (size_x->GetText() != nullptr))
            {
                x = std::stoi(size_x->GetText());
            }
            else
            {
                throw fgl_parsing_error("Error parsing FGL file: no element 'x' in 'size'");
            }

            int y = 0;
            if (auto* const size_y = size->FirstChildElement("y"); size_y != nullptr && (size_y->GetText() != nullptr))
            {
                y = std::stoi(size_y->GetText());
            }
            else
            {
                throw fgl_parsing_error("Error parsing FGL file: no element 'y' in 'size'");
            }

            int z = 0;
            if (auto* const size_z = size->FirstChildElement("z"); size_z != nullptr && (size_z->GetText() != nullptr))
            {
                z = std::stoi(size_z->GetText());
            }
            else
            {
                throw fgl_parsing_error("Error parsing FGL file: no element 'z' in 'size'");
            }

            const aspect_ratio<Lyt> ar{x, y, z};
            lyt.resize(ar);
        }
        else
        {
            throw fgl_parsing_error("Error parsing FGL file: no element 'size' in 'layout'");
        }

        // set clocking scheme
        if (auto* const clocking = layout->FirstChildElement("clocking"); clocking != nullptr)
        {
            if (auto* const clocking_scheme_name = clocking->FirstChildElement("name");
                clocking_scheme_name != nullptr && (clocking_scheme_name->GetText() != nullptr))
            {
                const auto clocking_scheme = get_clocking_scheme<Lyt>(clocking_scheme_name->GetText());
                if (clocking_scheme.has_value())
                {
                    lyt.replace_clocking_scheme(*clocking_scheme);
                    static constexpr std::array<const char*, 3> open_clocking_schemes{"OPEN", "OPEN3", "OPEN4"};

                    if (auto* const clock_zones = clocking->FirstChildElement("zones"); clock_zones != nullptr)
                    {
                        for (const auto* clock_zone = clock_zones->FirstChildElement("zone"); clock_zone != nullptr;
                             clock_zone             = clock_zone->NextSiblingElement("zone"))
                        {
                            int x_coord = 0;
                            if (const auto* const clocking_zone_x = clock_zone->FirstChildElement("x");
                                clocking_zone_x != nullptr && (clocking_zone_x->GetText() != nullptr))
                            {
                                x_coord = std::stoi(clocking_zone_x->GetText());
                            }
                            else
                            {
                                throw fgl_parsing_error("Error parsing FGL file: no element 'x' in 'zone'");
                            }

                            int y_coord = 0;
                            if (const auto* const clocking_zone_y = clock_zone->FirstChildElement("y");
                                clocking_zone_y != nullptr && (clocking_zone_y->GetText() != nullptr))
                            {
                                y_coord = std::stoi(clocking_zone_y->GetText());
                            }
                            else
                            {
                                throw fgl_parsing_error("Error parsing FGL file: no element 'y' in 'zone'");
                            }

                            uint8_t clock = 0;
                            if (const auto* const clocking_zone_clock = clock_zone->FirstChildElement("clock");
                                clocking_zone_clock != nullptr && (clocking_zone_clock->GetText() != nullptr))
                            {
                                clock = static_cast<uint8_t>(*clocking_zone_clock->GetText());
                            }
                            else
                            {
                                throw fgl_parsing_error("Error parsing FGL file: no element 'clock' in 'zone'");
                            }

                            lyt.assign_clock_number({x_coord, y_coord}, clock);
                        }
                    }
                    else if (std::ranges::find(open_clocking_schemes,
                                               static_cast<std::string>(clocking_scheme_name->GetText())) !=
                             open_clocking_schemes.cend())
                    {
                        throw fgl_parsing_error("Error parsing FGL file: no element 'zones' in 'clocking'");
                    }
                }
                else
                {
                    throw fgl_parsing_error(fmt::format("Error parsing FGL file: unknown clocking scheme: {}",
                                                        clocking_scheme_name->GetText()));
                }
            }
            else
            {
                throw fgl_parsing_error("Error parsing FGL file: no element 'name' in 'clocking'");
            }
        }
        else
        {
            throw fgl_parsing_error("Error parsing FGL file: no element 'clocking' in 'layout'");
        }

        // parse layout gates
        std::vector<gate_storage> gates{};
        if (auto* const gates_xml = fgl_root->FirstChildElement("gates"); gates_xml != nullptr)
        {
            for (const auto* gate_xml = gates_xml->FirstChildElement("gate"); gate_xml != nullptr;
                 gate_xml             = gate_xml->NextSiblingElement("gate"))
            {
                gate_storage gate{};

                if (const auto* const gate_id = gate_xml->FirstChildElement("id");
                    gate_id != nullptr && (gate_id->GetText() != nullptr))
                {
                    gate.id = std::stoi(gate_id->GetText());
                }
                else
                {
                    throw fgl_parsing_error("Error parsing FGL file: no element 'id' in 'gate'");
                }

                if (const auto* const gate_type = gate_xml->FirstChildElement("type");
                    gate_type != nullptr && (gate_type->GetText() != nullptr))
                {
                    gate.type = gate_type->GetText();
                }
                else
                {
                    throw fgl_parsing_error("Error parsing FGL file: no element 'type' in 'gate'");
                }

                if (gate.type == "PI" || gate.type == "PO")
                {
                    if (const auto* const pi_po_name = gate_xml->FirstChildElement("name");
                        pi_po_name != nullptr && (pi_po_name->GetText() != nullptr))
                    {
                        gate.name = pi_po_name->GetText();
                    }
                    else
                    {
                        throw fgl_parsing_error("Error parsing FGL file: no element 'name' in 'gate' for input/output");
                    }
                }

                const auto* const loc = gate_xml->FirstChildElement("loc");
                if (loc == nullptr)
                {
                    throw fgl_parsing_error("Error parsing FGL file: no element 'loc'");
                }

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconversion"
                // get x-coordinate
                if (const auto* const loc_x = loc->FirstChildElement("x");
                    loc_x != nullptr && (loc_x->GetText() != nullptr))
                {
                    gate.loc.x = static_cast<decltype(gate.loc.x)>(std::stoull(loc_x->GetText()));
                }
                else
                {
                    throw fgl_parsing_error("Error parsing FGL file: no element 'x' in 'loc'");
                }

                // get y-coordinate
                if (const auto* const loc_y = loc->FirstChildElement("y");
                    loc_y != nullptr && (loc_y->GetText() != nullptr))
                {
                    gate.loc.y = static_cast<decltype(gate.loc.y)>(std::stoull(loc_y->GetText()));
                }
                else
                {
                    throw fgl_parsing_error("Error parsing FGL file: no element 'y' in 'loc'");
                }

                // get z-coordinate
                if (const auto* const loc_z = loc->FirstChildElement("z");
                    loc_z != nullptr && (loc_z->GetText() != nullptr))
                {
                    gate.loc.z = static_cast<decltype(gate.loc.z)>(std::stoull(loc_z->GetText()));
                }
                else
                {
                    throw fgl_parsing_error("Error parsing FGL file: no element 'z' in 'loc'");
                }

                if (const auto* const incoming_signals = gate_xml->FirstChildElement("incoming");
                    incoming_signals != nullptr)
                {
                    for (const auto* incoming_signal                 = incoming_signals->FirstChildElement("signal");
                         incoming_signal != nullptr; incoming_signal = incoming_signal->NextSiblingElement("signal"))
                    {
                        tile<Lyt> incoming{};

                        // get x-coordinate of incoming signal
                        if (const auto* const incoming_signal_x = incoming_signal->FirstChildElement("x");
                            incoming_signal_x != nullptr && (incoming_signal_x->GetText() != nullptr))
                        {
                            incoming.x = static_cast<decltype(incoming.x)>(std::stoull(incoming_signal_x->GetText()));
                        }
                        else
                        {
                            throw fgl_parsing_error("Error parsing FGL file: no element 'x' in 'signal'");
                        }

                        // get y-coordinate of incoming signal
                        if (const auto* const incoming_signal_y = incoming_signal->FirstChildElement("y");
                            incoming_signal_y != nullptr && (incoming_signal_y->GetText() != nullptr))
                        {
                            incoming.y = static_cast<decltype(incoming.y)>(std::stoull(incoming_signal_y->GetText()));
                        }
                        else
                        {
                            throw fgl_parsing_error("Error parsing FGL file: no element 'y' in 'signal'");
                        }

                        // get z-coordinate of incoming signal
                        if (const auto* const incoming_signal_z = incoming_signal->FirstChildElement("z");
                            incoming_signal_z != nullptr && (incoming_signal_z->GetText() != nullptr))
                        {
                            incoming.z = static_cast<decltype(incoming.z)>(std::stoull(incoming_signal_z->GetText()));
                        }
                        else
                        {
                            throw fgl_parsing_error("Error parsing FGL file: no element 'z' in 'signal'");
                        }

                        gate.incoming.push_back(incoming);
                    }
                }
#pragma GCC diagnostic pop

                gates.push_back(gate);
            }

            // sort gates ascending based on id
            std::ranges::sort(gates, gate_storage::compare_by_id);

            for (const auto& gate : gates)
            {
                const tile<Lyt> location{gate.loc.x, gate.loc.y, gate.loc.z};

                if (gate.incoming.size() == 0)
                {
                    if (gate.type == "PI")
                    {
                        if constexpr (mockturtle::has_create_pi_v<Lyt>)
                        {
                            lyt.create_pi(gate.name, location);
                        }
                    }
                    else
                    {
                        throw fgl_parsing_error(fmt::format(
                            "Error parsing FGL file: unknown gate of type '{}' without input signals", gate.type));
                    }
                }

                else if (gate.incoming.size() == 1)
                {
                    const tile<Lyt> incoming_tile{gate.incoming.front().x, gate.incoming.front().y,
                                                  gate.incoming.front().z};
                    const auto      incoming_signal = lyt.make_signal(lyt.get_node(incoming_tile));

                    if (gate.type == "PO")
                    {
                        if constexpr (mockturtle::has_create_po_v<Lyt>)
                        {
                            lyt.create_po(incoming_signal, gate.name, location);
                        }
                    }
                    else if (gate.type == "BUF")
                    {
                        if constexpr (mockturtle::has_create_buf_v<Lyt>)
                        {
                            lyt.create_buf(incoming_signal, location);
                        }
                    }
                    else if (gate.type == "INV")
                    {
                        if constexpr (mockturtle::has_create_not_v<Lyt>)
                        {
                            lyt.create_not(incoming_signal, location);
                        }
                    }
                    else if (std::ranges::all_of(gate.type,
                                                 [](const unsigned char c) { return std::isxdigit(c) != 0; }))
                    {
                        if constexpr (mockturtle::has_create_node_v<Lyt>)
                        {
                            kitty::dynamic_truth_table tt_t(1u);
                            kitty::create_from_hex_string(tt_t, gate.type);
                            lyt.create_node({incoming_signal}, tt_t, location);
                        }
                    }
                    else
                    {
                        throw fgl_parsing_error(fmt::format(
                            "Error parsing FGL file: unknown gate of type '{}' with 1 input signal", gate.type));
                    }
                }

                else if (gate.incoming.size() == 2)
                {
                    const tile<Lyt> incoming_tile_1{gate.incoming.front().x, gate.incoming.front().y,
                                                    gate.incoming.front().z};
                    const tile<Lyt> incoming_tile_2{gate.incoming.back().x, gate.incoming.back().y,
                                                    gate.incoming.back().z};

                    const auto incoming_signal_1 = lyt.make_signal(lyt.get_node(incoming_tile_1));
                    const auto incoming_signal_2 = lyt.make_signal(lyt.get_node(incoming_tile_2));

                    if (gate.type == "AND")
                    {
                        if constexpr (mockturtle::has_create_and_v<Lyt>)
                        {
                            lyt.create_and(incoming_signal_1, incoming_signal_2, location);
                        }
                    }
                    else if (gate.type == "NAND")
                    {
                        if constexpr (mockturtle::has_create_nand_v<Lyt>)
                        {
                            lyt.create_nand(incoming_signal_1, incoming_signal_2, location);
                        }
                    }
                    else if (gate.type == "OR")
                    {
                        if constexpr (mockturtle::has_create_or_v<Lyt>)
                        {
                            lyt.create_or(incoming_signal_1, incoming_signal_2, location);
                        }
                    }
                    else if (gate.type == "NOR")
                    {
                        if constexpr (mockturtle::has_create_nor_v<Lyt>)
                        {
                            lyt.create_nor(incoming_signal_1, incoming_signal_2, location);
                        }
                    }
                    else if (gate.type == "XOR")
                    {
                        if constexpr (mockturtle::has_create_xor_v<Lyt>)
                        {
                            lyt.create_xor(incoming_signal_1, incoming_signal_2, location);
                        }
                    }
                    else if (gate.type == "XNOR")
                    {
                        if constexpr (mockturtle::has_create_xnor_v<Lyt>)
                        {
                            lyt.create_xnor(incoming_signal_1, incoming_signal_2, location);
                        }
                    }
                    else if (gate.type == "LT")
                    {
                        if constexpr (mockturtle::has_create_lt_v<Lyt>)
                        {
                            lyt.create_lt(incoming_signal_1, incoming_signal_2, location);
                        }
                    }
                    else if (gate.type == "LE")
                    {
                        if constexpr (mockturtle::has_create_le_v<Lyt>)
                        {
                            lyt.create_le(incoming_signal_1, incoming_signal_2, location);
                        }
                    }
                    else if (gate.type == "GT")
                    {
                        if constexpr (mockturtle::has_create_gt_v<Lyt>)
                        {
                            lyt.create_gt(incoming_signal_1, incoming_signal_2, location);
                        }
                    }
                    else if (gate.type == "GE")
                    {
                        if constexpr (mockturtle::has_create_ge_v<Lyt>)
                        {
                            lyt.create_ge(incoming_signal_1, incoming_signal_2, location);
                        }
                    }
                    else if (std::ranges::all_of(gate.type,
                                                 [](const unsigned char c) { return std::isxdigit(c) != 0; }))
                    {
                        if constexpr (mockturtle::has_create_node_v<Lyt>)
                        {
                            kitty::dynamic_truth_table tt_t(2u);
                            kitty::create_from_hex_string(tt_t, gate.type);
                            lyt.create_node({incoming_signal_1, incoming_signal_2}, tt_t, location);
                        }
                    }
                    else
                    {
                        throw fgl_parsing_error(fmt::format(
                            "Error parsing FGL file: unknown gate of type '{}' with 2 input signals", gate.type));
                    }
                }
                else if (gate.incoming.size() == 3)
                {
                    const tile<Lyt> incoming_tile_1{gate.incoming.front().x, gate.incoming.front().y,
                                                    gate.incoming.front().z};
                    const tile<Lyt> incoming_tile_2{gate.incoming[1].x, gate.incoming[1].y, gate.incoming[1].z};
                    const tile<Lyt> incoming_tile_3{gate.incoming.back().x, gate.incoming.back().y,
                                                    gate.incoming.back().z};

                    const auto incoming_signal_1 = lyt.make_signal(lyt.get_node(incoming_tile_1));
                    const auto incoming_signal_2 = lyt.make_signal(lyt.get_node(incoming_tile_2));
                    const auto incoming_signal_3 = lyt.make_signal(lyt.get_node(incoming_tile_3));

                    if (gate.type == "MAJ")
                    {
                        if constexpr (mockturtle::has_create_maj_v<Lyt>)
                        {
                            lyt.create_maj(incoming_signal_1, incoming_signal_2, incoming_signal_3, location);
                        }
                    }
                    else if (std::ranges::all_of(gate.type,
                                                 [](const unsigned char c) { return std::isxdigit(c) != 0; }))
                    {
                        if constexpr (mockturtle::has_create_node_v<Lyt>)
                        {
                            kitty::dynamic_truth_table tt_t(3u);
                            kitty::create_from_hex_string(tt_t, gate.type);
                            lyt.create_node({incoming_signal_1, incoming_signal_2, incoming_signal_3}, tt_t, location);
                        }
                    }
                    else
                    {
                        throw fgl_parsing_error(fmt::format(
                            "Error parsing FGL file: unknown gate of type '{}' with 3 input signals", gate.type));
                    }
                }
                else if (std::ranges::all_of(gate.type, [](const unsigned char c) { return std::isxdigit(c) != 0; }))
                {
                    if constexpr (mockturtle::has_create_node_v<Lyt>)
                    {
                        const auto                           num_incoming_signals = gate.incoming.size();
                        std::vector<mockturtle::signal<Lyt>> incoming_signals{};
                        for (const auto& in : gate.incoming)
                        {
                            const tile<Lyt> incoming_tile_i{in.x, in.y, in.z};
                            const auto      incoming_signal_i = lyt.make_signal(lyt.get_node(incoming_tile_i));
                            incoming_signals.push_back(incoming_signal_i);
                        }
                        kitty::dynamic_truth_table tt_t(static_cast<uint32_t>(num_incoming_signals));
                        kitty::create_from_hex_string(tt_t, gate.type);
                        lyt.create_node({incoming_signals}, tt_t, location);
                    }
                }
                else
                {
                    throw fgl_parsing_error(
                        fmt::format("Error parsing FGL file: unknown gate of type '{}' with {} input signals",
                                    gate.type, gate.incoming.size()));
                }
            }
        }

        return lyt;
    }

  private:
    /**
     * The layout which will be altered based on the parsed information.
     */
    Lyt lyt;
    /**
     * The input stream from which the gate-level layout is read.
     */
    std::istream& is;
    /**
     * @struct gate_storage
     *
     * Represents a gate in a fcn layout, storing its unique ID, type, name, location, and incoming connections.
     */
    struct gate_storage
    {
        /**
         * Unique identifier for the gate.
         */
        int id{};
        /**
         * Type of the gate, can be an alias (AND, OR, PI, ..) or the implemented function in a binary or hexadecimal
         * form.
         */
        std::string type;
        /**
         * Name of the gate (for inputs and outputs).
         */
        std::string name;
        /**
         * Location of the gate represented its x-, y- and z-coordinate.
         */
        tile<Lyt> loc{};
        /**
         * List of incoming connections to the gate.
         */
        std::vector<tile<Lyt>> incoming{};

        /**
         * Static member function to compare gate_storage objects by their IDs.
         *
         * @param gate1 First gate to be compared.
         * @param gate2 Second gate to be compared.
         * @return True if gate1's ID is less than gate2's ID, false otherwise.
         */
        static bool compare_by_id(const gate_storage& gate1, const gate_storage& gate2) noexcept
        {
            return gate1.id < gate2.id;
        }
    };
};

}  // namespace detail

/**
 * Reads a gate-level layout from an FGL file provided as an input stream.
 *
 * May throw an `fgl_parsing_error` if the FGL file is malformed.
 *
 * @tparam Lyt The layout type to be created from an input.
 * @param is The input stream to read from.
 * @param name The name to give to the generated layout.
 */
template <typename Lyt>
[[nodiscard]] Lyt read_fgl_layout(std::istream& is, const std::string_view& name = "")
{
    static_assert(is_gate_level_layout_v<Lyt>, "Lyt is not a gate-level layout");

    detail::read_fgl_layout_impl<Lyt> p{is, name};

    const auto lyt = p.run();

    return lyt;
}
/**
 * Reads a gate-level layout from an FGL file provided as an input stream.
 *
 * May throw an `fgl_parsing_error` if the FGL file is malformed.
 *
 * This is an in-place version of `read_fgl_layout` that utilizes the given layout as a target to write to.
 *
 * @tparam Lyt The layout type to be used as input.
 * @param lyt The layout to write to.
 * @param is The input stream to read from.
 */
template <typename Lyt>
void read_fgl_layout(Lyt& lyt, std::istream& is)
{
    static_assert(is_gate_level_layout_v<Lyt>, "Lyt is not a gate-level layout");

    detail::read_fgl_layout_impl<Lyt> p{lyt, is};

    lyt = p.run();
}
/**
 * Reads a gate-level layout from an FGL file provided as a file name.
 *
 * May throw an `fgl_parsing_error` if the FGL file is malformed.
 *
 * @tparam Lyt The layout type to be created from an input.
 * @param filename The file name to open and read from.
 * @param name The name to give to the generated layout.
 */
template <typename Lyt>
[[nodiscard]] Lyt read_fgl_layout(const std::string_view& filename, const std::string_view& name = "")
{
    std::ifstream is{std::string{filename}, std::ifstream::in};

    if (!is.is_open())
    {
        throw std::ifstream::failure("could not open file");
    }

    const auto lyt = read_fgl_layout<Lyt>(is, name);
    is.close();

    return lyt;
}
/**
 * Reads a gate-level layout from an FGL file provided as a file name.
 *
 * May throw an `fgl_parsing_error` if the FGL file is malformed.
 *
 * This is an in-place version of `read_fgl_layout` that utilizes the given layout as a target to write to.
 *
 * @tparam Lyt The layout type to be used as input.
 * @param lyt The layout to write to.
 * @param filename The file name to open and read from.
 */
template <typename Lyt>
void read_fgl_layout(Lyt& lyt, const std::string_view& filename)
{
    std::ifstream is{std::string{filename}, std::ifstream::in};

    if (!is.is_open())
    {
        throw std::ifstream::failure("could not open file");
    }

    read_fgl_layout<Lyt>(lyt, is);
    is.close();
}

}  // namespace fiction

#endif  // FICTION_READ_FGL_LAYOUT_HPP
