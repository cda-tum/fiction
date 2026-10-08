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
 * @brief Reader for gate-level layouts stored in the FGL file format.
 * @author Simon Hofmann (simon1hofmann)
 * @author Marcel Walter (marcelwa)
 */

#pragma once

// clang-format off
// NOLINTBEGIN(misc-include-cleaner): no symbol from these headers is named directly, but the
// clocking::get_scheme free function is looked up via two-phase name lookup at template instantiation time, so
// removing any of these breaks the build despite the tool's "not used directly" heuristic
#include "fiction/layouts/arrangement.hpp"
#include "fiction/layouts/cartesian_layout.hpp"
#include "fiction/layouts/clocking_scheme.hpp"
#include "fiction/layouts/gate_level_layout.hpp"
// NOLINTEND(misc-include-cleaner)
// clang-format on

#include "fiction/layouts/io/detail/fgl_layout_validation.hpp"
#include "fiction/traits.hpp"

#include <fmt/format.h>
#include <kitty/constructors.hpp>
#include <kitty/dynamic_truth_table.hpp>
#include <tinyxml2.h>

#include <algorithm>
#include <array>
#include <cctype>
#include <charconv>
#include <cstdint>
#include <fstream>
#include <istream>
#include <limits>
#include <memory>
#include <optional>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <system_error>
#include <tuple>
#include <unordered_map>
#include <utility>
#include <vector>

namespace fiction::layouts::io
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
    explicit fgl_parsing_error(const std::string_view& msg) : std::runtime_error(std::string{msg}) {}
};

namespace detail
{

/** @brief Parse a layout atomically. @tparam Lyt Gate-level layout type. */
template <typename Lyt>
class read_fgl_layout_impl
{
  public:
    /**
     * @brief Create a reader that constructs a layout from the stream.
     * @param s Input stream.
     * @param name Name of the new layout.
     */
    read_fgl_layout_impl(std::istream& s, const std::string_view& name) : layout_name{name}, is{s} {}

    /**
     * @brief Create a reader for an existing layout.
     * @param tgt Target layout.
     * @param s Input stream.
     */
    read_fgl_layout_impl(const Lyt& tgt, std::istream& s) : layout_name{tgt.get_layout_name()}, is{s}
    {
        if constexpr (!is_cartesian_layout_v<Lyt>)
        {
            expected_arrangement = tgt.get_arrangement();
        }
    }

    /** @brief Parse legacy maximum-index extents or version-2 extent counts. @return Parsed layout. */
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

        const auto* version     = fgl_root->Attribute("version");
        const bool  version_two = version != nullptr;
        if (version_two && std::string_view{version} != "2")
        {
            throw fgl_parsing_error("Error parsing FGL file: unsupported version");
        }

        auto* const layout = fgl_root->FirstChildElement("layout");
        if (layout == nullptr)
        {
            throw fgl_parsing_error("Error parsing FGL file: no element 'layout'");
        }

        // the topology selects the arrangement of the layout to create
        auto* const topology = layout->FirstChildElement("topology");
        if ((topology == nullptr || topology->GetText() == nullptr) && !is_cartesian_layout_v<Lyt>)
        {
            throw fgl_parsing_error("Error parsing FGL file: no element 'topology' in 'layout'");
        }

        const std::string topology_name =
            topology != nullptr && topology->GetText() != nullptr ? topology->GetText() : "cartesian";
        std::optional<fiction::layouts::arrangement> file_arrangement{};
        const auto                                   find_arrangement = [&topology_name](const std::string_view family)
        {
            for (const auto a : {fiction::layouts::arrangement::ODD_ROW, fiction::layouts::arrangement::EVEN_ROW,
                                 fiction::layouts::arrangement::ODD_COLUMN, fiction::layouts::arrangement::EVEN_COLUMN})
            {
                if (topology_name == fmt::format("{}_{}", fiction::layouts::to_string(a), family))
                {
                    return std::optional{a};
                }
            }

            return std::optional<fiction::layouts::arrangement>{};
        };

        if (topology_name == "cartesian")
        {
            if constexpr (!is_cartesian_layout_v<Lyt>)
            {
                throw fgl_parsing_error("Error parsing FGL file: Lyt is not a cartesian layout");
            }
        }
        else if (const auto shifted = find_arrangement("cartesian"); shifted.has_value())
        {
            file_arrangement = shifted;

            if constexpr (!is_shifted_cartesian_layout_v<Lyt>)
            {
                throw fgl_parsing_error("Error parsing FGL file: Lyt is not a shifted_cartesian layout");
            }
        }
        else if (const auto hexagonal = find_arrangement("hex"); hexagonal.has_value())
        {
            file_arrangement = hexagonal;

            if constexpr (!is_hexagonal_layout_v<Lyt>)
            {
                throw fgl_parsing_error("Error parsing FGL file: Lyt is not a hexagonal layout");
            }
        }
        else
        {
            throw fgl_parsing_error(fmt::format("Error parsing FGL file: unknown topology: {}", topology_name));
        }

        if (expected_arrangement && expected_arrangement != file_arrangement)
        {
            throw fgl_parsing_error(
                fmt::format("Error parsing FGL file: target arrangement differs from {}", topology_name));
        }

        if constexpr (is_cartesian_layout_v<Lyt>)
        {
            target.emplace(typename Lyt::extent{}, layout_name);
        }
        else
        {
            target.emplace(*file_arrangement, typename Lyt::extent{}, layout_name);
        }

        auto& lyt = *target;

        // set layout name
        if (auto* const name = layout->FirstChildElement("name"); name != nullptr)
        {
            std::string name_text = name->GetText() == nullptr ? "" : name->GetText();
            lyt.set_layout_name(name_text);
        }

        // set layout size
        if (auto* const size = layout->FirstChildElement("size"); size != nullptr)
        {
            const auto delta = version_two ? uint64_t{0} : uint64_t{1};
            const auto x     = read_number(size, "x");
            const auto y     = read_number(size, "y");
            const auto z     = read_number(size, "z");
            if (x > std::numeric_limits<uint64_t>::max() - delta || y > std::numeric_limits<uint64_t>::max() - delta ||
                z > std::numeric_limits<uint64_t>::max() - delta)
            {
                throw fgl_parsing_error("Error parsing FGL file: extent exceeds the target range");
            }
            try
            {
                lyt.resize(typename Lyt::extent{x + delta, y + delta, z + delta});
            }
            catch (const std::invalid_argument&)
            {
                throw fgl_parsing_error("Error parsing FGL file: extent exceeds the target range");
            }
        }
        else
        {
            throw fgl_parsing_error("Error parsing FGL file: no element 'size' in 'layout'");
        }

        // set clocking scheme
        if (auto* const clocking = layout->FirstChildElement("clocking"); clocking != nullptr)
        {
            if (const auto* elements = clocking->FirstChildElement("synchronization_elements"); elements != nullptr)
            {
                for (const auto* element = elements->FirstChildElement("element"); element != nullptr;
                     element             = element->NextSiblingElement("element"))
                {
                    const auto delay = read_number(element, "delay");
                    if (delay > std::numeric_limits<typename Lyt::sync_elem_t>::max())
                    {
                        throw fgl_parsing_error(
                            "Error parsing FGL file: synchronization delay exceeds the target range");
                    }
                    lyt.assign_synchronization_element(read_position(element),
                                                       static_cast<typename Lyt::sync_elem_t>(delay));
                }
            }
            if (auto* const clocking_scheme_name = clocking->FirstChildElement("name");
                clocking_scheme_name != nullptr && (clocking_scheme_name->GetText() != nullptr))
            {
                const auto clocking_scheme = layouts::clocking::get_scheme(
                    clocking_scheme_name->GetText(), is_hexagonal_layout_v<Lyt> ? file_arrangement : std::nullopt);
                if (clocking_scheme.has_value())
                {
                    lyt.replace_clocking_scheme(*clocking_scheme);
                    static constexpr std::array<const char*, 3> open_clocking_schemes{"OPEN", "OPEN3", "OPEN4"};

                    if (auto* const clock_zones = clocking->FirstChildElement("zones"); clock_zones != nullptr)
                    {
                        for (const auto* clock_zone = clock_zones->FirstChildElement("zone"); clock_zone != nullptr;
                             clock_zone             = clock_zone->NextSiblingElement("zone"))
                        {
                            const auto position = read_position(clock_zone, false);
                            const auto clock    = read_number(clock_zone, "clock");
                            if (clock >= lyt.num_clocks())
                            {
                                throw fgl_parsing_error("Error parsing FGL file: clock exceeds the phase count");
                            }
                            lyt.assign_clock_number(position, static_cast<uint8_t>(clock));
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

        if (version_two)
        {
            if (const auto* manual = layout->FirstChildElement("obstructions"); manual != nullptr)
            {
                if (const auto* coordinates = manual->FirstChildElement("coordinates"); coordinates != nullptr)
                {
                    for (const auto* coordinate = coordinates->FirstChildElement("coordinate"); coordinate != nullptr;
                         coordinate             = coordinate->NextSiblingElement("coordinate"))
                    {
                        lyt.obstruct_coordinate(read_position<int64_t>(coordinate));
                    }
                }
                if (const auto* connections = manual->FirstChildElement("connections"); connections != nullptr)
                {
                    for (const auto* connection = connections->FirstChildElement("connection"); connection != nullptr;
                         connection             = connection->NextSiblingElement("connection"))
                    {
                        lyt.obstruct_connection(read_position<int64_t>(connection->FirstChildElement("source")),
                                                read_position<int64_t>(connection->FirstChildElement("target")));
                    }
                }
            }
        }

        if (version_two &&
            (layout->FirstChildElement("inputs") == nullptr || layout->FirstChildElement("outputs") == nullptr))
        {
            throw fgl_parsing_error("Error parsing FGL file: missing interface order");
        }

        // parse layout gates
        std::vector<gate_storage> gates{};
        if (auto* const gates_xml = fgl_root->FirstChildElement("gates"); gates_xml != nullptr)
        {
            for (const auto* gate_xml = gates_xml->FirstChildElement("gate"); gate_xml != nullptr;
                 gate_xml             = gate_xml->NextSiblingElement("gate"))
            {
                gate_storage gate{};

                const auto id = read_number(gate_xml, "id");
                if (id > (version_two ? std::numeric_limits<uint32_t>::max() :
                                        static_cast<uint32_t>(std::numeric_limits<int>::max())))
                {
                    throw fgl_parsing_error("Error parsing FGL file: gate ID exceeds the target range");
                }
                gate.id = static_cast<decltype(gate.id)>(id);

                if (const auto* const gate_type = gate_xml->FirstChildElement("type");
                    gate_type != nullptr && (gate_type->GetText() != nullptr))
                {
                    gate.type = gate_type->GetText();
                }
                else
                {
                    throw fgl_parsing_error("Error parsing FGL file: no element 'type' in 'gate'");
                }

                if (const auto* name = gate_xml->FirstChildElement("name"); name != nullptr)
                {
                    gate.name = name->GetText() == nullptr ? "" : name->GetText();
                    if (!version_two && (gate.type == "PI" || gate.type == "PO") && name->GetText() == nullptr)
                    {
                        throw fgl_parsing_error("Error parsing FGL file: missing interface name");
                    }
                }
                else if (gate.type == "PI" || gate.type == "PO")
                {
                    throw fgl_parsing_error("Error parsing FGL file: no element 'name' in interface gate");
                }
                if (version_two)
                {
                    const auto arity = read_number(gate_xml, "arity");
                    if (arity > std::numeric_limits<uint32_t>::max())
                    {
                        throw fgl_parsing_error("Error parsing FGL file: arity exceeds the target range");
                    }
                    gate.arity = static_cast<uint32_t>(arity);
                }

                const auto* const loc = gate_xml->FirstChildElement("loc");
                if (loc == nullptr)
                {
                    throw fgl_parsing_error("Error parsing FGL file: no element 'loc'");
                }

                gate.loc = read_position(loc);

                if (const auto* const incoming_signals = gate_xml->FirstChildElement("incoming");
                    incoming_signals != nullptr)
                {
                    for (const auto* incoming_signal                 = incoming_signals->FirstChildElement("signal");
                         incoming_signal != nullptr; incoming_signal = incoming_signal->NextSiblingElement("signal"))
                    {
                        if (version_two)
                        {
                            const auto source = read_number(incoming_signal, "source");
                            const auto index  = read_number(incoming_signal, "index");
                            const auto input  = read_number(incoming_signal, "input");
                            if (source > std::numeric_limits<uint32_t>::max() || index != 0 || input >= gate.arity)
                            {
                                throw fgl_parsing_error("Error parsing FGL file: invalid connection port");
                            }
                            gate.connections.emplace_back(static_cast<uint32_t>(source), static_cast<uint32_t>(input));
                        }
                        else
                        {
                            gate.incoming.push_back(read_position(incoming_signal));
                        }
                    }
                }

                gates.push_back(gate);
            }

            // sort gates ascending based on id
            std::ranges::sort(gates, gate_storage::compare_by_id);

            std::unordered_map<uint32_t, typename Lyt::object_id> objects{};
            try
            {
                for (const auto& gate : gates)
                {
                    if (objects.contains(gate.id))
                    {
                        throw fgl_parsing_error("Error parsing FGL file: duplicate gate ID");
                    }
                    typename Lyt::object_id port{};
                    const auto arity = version_two ? gate.arity : static_cast<uint32_t>(gate.incoming.size());
                    if (gate.type == "PI" && arity == 0)
                    {
                        port = lyt.create_pi(gate.name, gate.loc);
                    }
                    else if (gate.type == "PO" && arity == 1)
                    {
                        port = lyt.create_po(gate.name, gate.loc);
                    }
                    else if (gate.type == "BUF" && arity == 1)
                    {
                        port = lyt.create_buf(gate.loc);
                    }
                    else
                    {
                        std::string hex      = gate.type;
                        uint32_t    required = arity;
                        const auto  alias    = std::ranges::find_if(LEGACY_GATE_FUNCTIONS, [&](const auto& function)
                                                                    { return std::get<0>(function) == gate.type; });
                        if (alias != LEGACY_GATE_FUNCTIONS.end())
                        {
                            hex      = std::get<1>(*alias);
                            required = std::get<2>(*alias);
                        }
                        if ((!version_two && arity == 0) || arity != required || arity >= 64 || hex.empty() ||
                            !std::ranges::all_of(hex, [](const unsigned char c) { return std::isxdigit(c) != 0; }) ||
                            hex.size() != (arity < 2 ? 1 : uint64_t{1} << (arity - 2)))
                        {
                            throw fgl_parsing_error("Error parsing FGL file: invalid gate type or function arity");
                        }
                        if (arity < 2)
                        {
                            uint32_t bits{};
                            std::from_chars(hex.data(), std::to_address(hex.end()), bits, 16);
                            if (bits >= (uint32_t{1} << (uint32_t{1} << arity)))
                            {
                                throw fgl_parsing_error("Error parsing FGL file: truth table exceeds its arity");
                            }
                        }
                        kitty::dynamic_truth_table function{arity};
                        kitty::create_from_hex_string(function, hex);
                        port = lyt.create_gate({}, function, gate.loc);
                    }
                    lyt.set_name(port, gate.name);
                    objects.emplace(gate.id, port);
                }
                for (const auto& gate : gates)
                {
                    const auto id = objects.at(gate.id);
                    if (version_two)
                    {
                        for (const auto& [source, input] : gate.connections)
                        {
                            if (!objects.contains(source) || lyt.source({id, input}))
                            {
                                throw fgl_parsing_error("Error parsing FGL file: missing source or duplicate input");
                            }
                            lyt.connect(objects.at(source), {id, input});
                        }
                    }
                    else
                    {
                        for (uint32_t input = 0; input < gate.incoming.size(); ++input)
                        {
                            const auto source = lyt.find_object(gate.incoming[input]);
                            if (!source)
                            {
                                throw fgl_parsing_error("Error parsing FGL file: missing source object");
                            }
                            lyt.connect(*source, {id, input});
                        }
                    }
                    for (uint32_t input = 0; input < lyt.input_count(id); ++input)
                    {
                        if (!lyt.source({id, input}))
                        {
                            throw fgl_parsing_error("Error parsing FGL file: missing input");
                        }
                    }
                }
                if (version_two)
                {
                    const auto read_order = [&](const char* tag)
                    {
                        const auto* order = layout->FirstChildElement(tag);
                        if (order == nullptr)
                        {
                            throw fgl_parsing_error("Error parsing FGL file: missing interface order");
                        }
                        std::vector<typename Lyt::object_id> ids{};
                        for (const auto* entry = order->FirstChildElement("id"); entry != nullptr;
                             entry             = entry->NextSiblingElement("id"))
                        {
                            const auto serialized = read_value(entry);
                            if (serialized > std::numeric_limits<uint32_t>::max() ||
                                !objects.contains(static_cast<uint32_t>(serialized)))
                            {
                                throw fgl_parsing_error("Error parsing FGL file: unknown interface object");
                            }
                            ids.push_back(objects.at(static_cast<uint32_t>(serialized)));
                        }
                        return ids;
                    };
                    lyt.set_input_order(read_order("inputs"));
                    lyt.set_output_order(read_order("outputs"));
                }
            }
            catch (const std::invalid_argument& error)
            {
                throw fgl_parsing_error(error.what());
            }
        }

        else if (version_two)
        {
            throw fgl_parsing_error("Error parsing FGL file: no element 'gates'");
        }
        if (version_two)
        {
            try
            {
                fgl::validate_layout(lyt);
            }
            catch (const std::invalid_argument& error)
            {
                throw fgl_parsing_error(error.what());
            }
        }
        return std::move(lyt);
    }

  private:
    /** @brief Legacy gate names, truth tables, and declared input counts. */
    static constexpr std::array<std::tuple<std::string_view, std::string_view, uint32_t>, 12> LEGACY_GATE_FUNCTIONS{
        {{"INV", "1", 1},
         {"AND", "8", 2},
         {"NAND", "7", 2},
         {"OR", "e", 2},
         {"NOR", "1", 2},
         {"XOR", "6", 2},
         {"XNOR", "9", 2},
         {"LT", "2", 2},
         {"GT", "4", 2},
         {"LE", "b", 2},
         {"GE", "d", 2},
         {"MAJ", "e8", 3}}};
    /**
     * Scratch layout created from the file.
     */
    std::optional<Lyt> target{};
    /** @brief Required arrangement when reading into an existing layout. */
    std::optional<fiction::layouts::arrangement> expected_arrangement{};
    /**
     * The name of a newly created layout.
     */
    std::string layout_name{};
    /**
     * The input stream from which the gate-level layout is read.
     */
    std::istream& is;
    /**
     * @brief Read an integer without truncation or trailing characters.
     * @tparam Integer Checked integer storage type; unsigned by default.
     * @param parent XML element containing the number.
     * @param name Child element name.
     * @return Parsed integer.
     * @throws fgl_parsing_error If the element is missing or the number is invalid.
     */
    template <typename Integer = uint64_t>
    static Integer read_number(const tinyxml2::XMLElement* parent, const char* name)
    {
        const auto* child = parent->FirstChildElement(name);
        if (child == nullptr || child->GetText() == nullptr)
        {
            throw fgl_parsing_error(
                fmt::format("Error parsing FGL file: no element '{}' in '{}'", name, parent->Name()));
        }
        return read_value<Integer>(child);
    }
    /**
     * @brief Parse an XML element's integer text.
     * @tparam Integer Checked integer storage type; unsigned by default.
     * @param child Numeric element.
     * @return Parsed value.
     */
    template <typename Integer = uint64_t>
    static Integer read_value(const tinyxml2::XMLElement* child)
    {
        if (child->GetText() == nullptr)
        {
            throw fgl_parsing_error("Error parsing FGL file: empty integer");
        }
        const std::string_view text{child->GetText()};
        const auto             first = text.find_first_not_of(" \t\r\n");
        const auto             last  = text.find_last_not_of(" \t\r\n");
        if (first == std::string_view::npos)
        {
            throw fgl_parsing_error("Error parsing FGL file: empty coordinate");
        }
        const auto        trimmed = text.substr(first, last - first + 1);
        const auto* const end     = std::to_address(trimmed.end());
        Integer           value{};
        const auto        result = std::from_chars(std::to_address(trimmed.begin()), end, value);
        if (result.ec != std::errc{} || result.ptr != end)
        {
            throw fgl_parsing_error(fmt::format("Error parsing FGL file: invalid integer '{}'", text));
        }
        return value;
    }
    /**
     * @brief Read a position and reject values the layout's coordinate type cannot represent.
     * @tparam Integer Axis storage type; unsigned for placed objects, signed for manual obstructions.
     * @param element XML element containing x, y, and optionally z.
     * @param with_z Whether the z child is required.
     * @return Losslessly represented coordinate.
     * @throws fgl_parsing_error If an axis is invalid or overflows.
     */
    template <typename Integer = uint64_t>
    static tile<Lyt> read_position(const tinyxml2::XMLElement* element, const bool with_z = true)
    {
        if (element == nullptr)
        {
            throw fgl_parsing_error("Error parsing FGL file: missing position");
        }
        const auto x = read_number<Integer>(element, "x");
        const auto y = read_number<Integer>(element, "y");
        const auto z = with_z ? read_number<Integer>(element, "z") : Integer{0};
        try
        {
            const tile<Lyt> position{x, y, z};
            return position;
        }
        catch (const std::overflow_error&)
        {
            throw fgl_parsing_error("Error parsing FGL file: coordinate exceeds the target layout's range");
        }
    }
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
        uint32_t id{};
        /**
         * Type of the gate, can be an alias (AND, OR, PI, ..) or the implemented function in a binary or hexadecimal
         * form.
         */
        std::string type;
        /**
         * Object label.
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
        /** @brief Declared function arity in version 2. */
        uint32_t arity{};
        /** @brief Serialized source IDs and destination input indices. */
        std::vector<std::pair<uint32_t, uint32_t>> connections{};

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
 * Reads legacy maximum-index extents or version-2 extent counts, declared interface order, and manual obstructions.
 * Version 2 requires a complete, acyclic, physically valid layout. Validation finishes before assigning a target.
 * The target layout changes only after a successful read.
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

    return p.run();
}
/**
 * Reads legacy maximum-index extents or version-2 extent counts, declared interface order, and manual obstructions.
 * Version 2 requires a complete, acyclic, physically valid layout. Validation finishes before assigning a target.
 * The target layout changes only after a successful read.
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

    return read_fgl_layout<Lyt>(is, name);
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
}

}  // namespace fiction::layouts::io
