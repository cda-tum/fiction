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
 * @brief Python bindings for `fiction/layouts/gate_level_layout.hpp`.
 * @author Marcel Walter (marcelwa)
 * @author Simon Hofmann (simon1hofmann)
 */

#include "fiction/layouts/cartesian_layout.hpp"
#include "fiction/layouts/hexagonal_layout.hpp"
#include "fiction/layouts/layout_base.hpp"
#include "fiction/layouts/shifted_cartesian_layout.hpp"
#include "pyfiction/documentation.hpp"
#include "pyfiction/types.hpp"

#include <fiction/layouts/arrangement.hpp>
#include <fiction/layouts/bounding_box.hpp>
#include <fiction/layouts/clocking_scheme.hpp>
#include <fiction/layouts/gate_level_layout.hpp>
#include <fiction/layouts/io/print_layout.hpp>

#include <fmt/format.h>

#include <concepts>
#include <cstdint>
#include <memory>
#include <optional>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include <nanobind/nanobind.h>
#include <nanobind/stl/optional.h>  // NOLINT(misc-include-cleaner): Converts absent objects, input sources, and bounds to None.
#include <nanobind/stl/pair.h>  // NOLINT(misc-include-cleaner): Converts obstruction connections and bounding-box coordinate pairs.
#include <nanobind/stl/string.h>  // NOLINT(misc-include-cleaner): Converts layout names and clocking scheme arguments.
#include <nanobind/stl/string_view.h>  // NOLINT(misc-include-cleaner): Converts Python scheme names to string views.
#include <nanobind/stl/vector.h>  // NOLINT(misc-include-cleaner): Converts ordered ports, interfaces, and coordinate collections.

/**
 * @brief Namespace alias for nanobind.
 */
namespace nb = nanobind;

namespace pyfiction
{

namespace detail
{

/**
 * @brief Registers gate layouts with their owned clocking and obstruction capabilities.
 * @tparam LytBase Coordinate geometry.
 * @tparam GateLyt Gate layout type.
 * @param m Python module.
 * @param topology Name identifying the coordinate geometry.
 */
template <typename LytBase, typename GateLyt>
void gate_level_layout(nb::module_& m, const std::string& topology)
{
    nb::class_<GateLyt, LytBase> cls(m, fmt::format("{}_gate_layout", topology).c_str(),
                                     DOC(fiction_layouts_gate_level_layout));

    if constexpr (std::derived_from<GateLyt, fiction::layouts::cartesian_layout>)
    {
        cls.def(nb::init<>(), DOC(fiction_layouts_gate_level_layout_gate_level_layout))
            .def(nb::init<const fiction::layouts::layout_base::extent&>(), nb::arg("extent"),
                 DOC(fiction_layouts_gate_level_layout_gate_level_layout))
            .def(
                "__init__",
                [](nb::pointer_and_handle<GateLyt> self, const fiction::layouts::layout_base::extent& extent,
                   const std::string& scheme_name, const std::string& layout_name)
                {
                    if (const auto scheme = fiction::layouts::clocking::get_scheme(scheme_name); scheme.has_value())
                    {
                        std::construct_at(self.p, extent, *scheme, layout_name);
                        return;
                    }

                    throw std::invalid_argument("Unknown clocking scheme");
                },
                nb::arg("extent"), nb::arg("clocking_scheme") = "2DDWave", nb::arg("layout_name") = "",
                fmt::format("{}\n\nRaises:\n    ValueError: The clocking scheme name is unknown.",
                            DOC(fiction_layouts_gate_level_layout_gate_level_layout_2))
                    .c_str());
    }
    else
    {
        cls.def(nb::init<fiction::layouts::arrangement>(), nb::arg("arrangement"),
                DOC(fiction_layouts_gate_level_layout_gate_level_layout_3))
            .def(nb::init<fiction::layouts::arrangement, const fiction::layouts::layout_base::extent&>(),
                 nb::arg("arrangement"), nb::arg("extent"), DOC(fiction_layouts_gate_level_layout_gate_level_layout_3))
            .def(
                "__init__",
                [](nb::pointer_and_handle<GateLyt> self, const fiction::layouts::arrangement a,
                   const fiction::layouts::layout_base::extent& extent, const std::string& scheme_name,
                   const std::string& layout_name)
                {
                    // only hexagonal layouts take their arrangement into account for clocking scheme lookup
                    if (const auto scheme = fiction::layouts::clocking::get_scheme(
                            scheme_name, (std::derived_from<GateLyt, fiction::layouts::hexagonal_layout> &&
                                          !std::derived_from<GateLyt, fiction::layouts::shifted_cartesian_layout>) ?
                                             std::optional{a} :
                                             std::nullopt);
                        scheme.has_value())
                    {
                        std::construct_at(self.p, a, extent, *scheme, layout_name);
                        return;
                    }

                    throw std::invalid_argument("Unknown clocking scheme");
                },
                nb::arg("arrangement"), nb::arg("extent"), nb::arg("clocking_scheme") = "2DDWave",
                nb::arg("layout_name") = "",
                fmt::format("{}\n\nRaises:\n    ValueError: The clocking scheme name is unknown.",
                            DOC(fiction_layouts_gate_level_layout_gate_level_layout_4))
                    .c_str());
    }

    cls.def("assign_clock_number", &GateLyt::assign_clock_number, nb::arg("cz"), nb::arg("cn"),
            DOC(fiction_layouts_gate_level_layout_assign_clock_number))
        .def("get_clock_number", &GateLyt::get_clock_number, nb::arg("cz"),
             DOC(fiction_layouts_gate_level_layout_get_clock_number))
        .def("num_clocks", &GateLyt::num_clocks, DOC(fiction_layouts_gate_level_layout_num_clocks))
        .def("is_regularly_clocked", &GateLyt::is_regularly_clocked,
             DOC(fiction_layouts_gate_level_layout_is_regularly_clocked))

        .def("is_clocking_scheme", &GateLyt::is_clocking_scheme, nb::arg("name"),
             DOC(fiction_layouts_gate_level_layout_is_clocking_scheme))
        .def(
            "get_clocking_scheme_name", [](const GateLyt& lyt) { return lyt.get_clocking_scheme().name(); },
            "Returns the name of the layout's clocking scheme, e.g., `2DDWave` or `USE`.")

        .def("is_incoming_clocked", &GateLyt::is_incoming_clocked, nb::arg("cz1"), nb::arg("cz2"),
             DOC(fiction_layouts_gate_level_layout_is_incoming_clocked))
        .def("is_outgoing_clocked", &GateLyt::is_outgoing_clocked, nb::arg("cz1"), nb::arg("cz2"),
             DOC(fiction_layouts_gate_level_layout_is_outgoing_clocked))

        .def("incoming_clocked_zones", &GateLyt::incoming_clocked_zones, nb::arg("cz"),
             DOC(fiction_layouts_gate_level_layout_incoming_clocked_zones))
        .def("outgoing_clocked_zones", &GateLyt::outgoing_clocked_zones, nb::arg("cz"),
             DOC(fiction_layouts_gate_level_layout_outgoing_clocked_zones))

        .def("in_degree", &GateLyt::in_degree, nb::arg("cz"), DOC(fiction_layouts_gate_level_layout_in_degree))
        .def("out_degree", &GateLyt::out_degree, nb::arg("cz"), DOC(fiction_layouts_gate_level_layout_out_degree))
        .def("degree", &GateLyt::degree, nb::arg("cz"), DOC(fiction_layouts_gate_level_layout_degree))

        .def(
            "replace_clocking_scheme",
            [](GateLyt& lyt, const std::string& name)
            {
                if (const auto scheme = fiction::layouts::clocking::get_scheme(
                        name,
                        [](const GateLyt& layout) -> std::optional<fiction::layouts::arrangement>
                        {
                            if constexpr ((std::derived_from<GateLyt, fiction::layouts::hexagonal_layout> &&
                                           !std::derived_from<GateLyt, fiction::layouts::shifted_cartesian_layout>))
                            {
                                return layout.get_arrangement();
                            }
                            else
                            {
                                return std::nullopt;
                            }
                        }(lyt));
                    scheme)
                {
                    lyt.replace_clocking_scheme(*scheme);
                }
                else
                {
                    throw std::invalid_argument("Unknown clocking scheme");
                }
            },
            nb::arg("name"),
            "Replaces the clocking scheme by the predefined scheme of the given name. Clock-number overrides are "
            "discarded; synchronization elements are kept. Raises ValueError for an unknown name.")
        .def("obstruct_coordinate", &GateLyt::obstruct_coordinate, nb::arg("c"),
             DOC(fiction_layouts_gate_level_layout_obstruct_coordinate))
        .def("obstruct_connection", &GateLyt::obstruct_connection, nb::arg("src"), nb::arg("tgt"),
             DOC(fiction_layouts_gate_level_layout_obstruct_connection))
        .def("clear_obstructed_coordinate", &GateLyt::clear_obstructed_coordinate, nb::arg("c"),
             DOC(fiction_layouts_gate_level_layout_clear_obstructed_coordinate))
        .def("clear_obstructed_connection", &GateLyt::clear_obstructed_connection, nb::arg("src"), nb::arg("tgt"),
             DOC(fiction_layouts_gate_level_layout_clear_obstructed_connection))
        .def("clear_obstructed_coordinates", &GateLyt::clear_obstructed_coordinates,
             DOC(fiction_layouts_gate_level_layout_clear_obstructed_coordinates))
        .def("clear_obstructed_connections", &GateLyt::clear_obstructed_connections,
             DOC(fiction_layouts_gate_level_layout_clear_obstructed_connections))
        .def(
            "obstructed_coordinates",
            [](const GateLyt& lyt)
            {
                std::vector<typename GateLyt::tile> coordinates{};
                lyt.foreach_obstructed_coordinate([&coordinates](const auto& c) { coordinates.push_back(c); });
                return coordinates;
            },
            "Returns manual coordinate obstructions in unspecified order, without implicit occupancy.")
        .def(
            "obstructed_connections",
            [](const GateLyt& lyt)
            {
                std::vector<std::pair<typename GateLyt::tile, typename GateLyt::tile>> connections{};
                lyt.foreach_obstructed_connection([&connections](const auto& source, const auto& target)
                                                  { connections.emplace_back(source, target); });
                return connections;
            },
            "Returns manual directed-connection obstructions in unspecified order, without physical connections.")
        .def("is_obstructed_coordinate", &GateLyt::is_obstructed_coordinate, nb::arg("c"),
             DOC(fiction_layouts_gate_level_layout_is_obstructed_coordinate))
        .def("is_obstructed_connection", &GateLyt::is_obstructed_connection, nb::arg("src"), nb::arg("tgt"),
             DOC(fiction_layouts_gate_level_layout_is_obstructed_connection))

        .def("create_pi", &GateLyt::create_pi, nb::arg("name"), nb::arg("t"),
             DOC(fiction_layouts_gate_level_layout_create_pi))
        .def("create_po",
             nb::overload_cast<typename GateLyt::object_id, const std::string&,
                               const fiction::layouts::layout_base::coordinate&>(&GateLyt::create_po),
             nb::arg("s"), nb::arg("name"), nb::arg("t"), DOC(fiction_layouts_gate_level_layout_create_po))
        .def("is_pi", &GateLyt::is_pi, nb::arg("n"), DOC(fiction_layouts_gate_level_layout_is_pi))
        .def("is_po", &GateLyt::is_po, nb::arg("n"), DOC(fiction_layouts_gate_level_layout_is_po))
        .def("is_pi_tile", &GateLyt::is_pi_tile, nb::arg("t"), DOC(fiction_layouts_gate_level_layout_is_pi_tile))
        .def("is_po_tile", &GateLyt::is_po_tile, nb::arg("t"), DOC(fiction_layouts_gate_level_layout_is_po_tile))

        .def("is_inv", &GateLyt::is_inv, DOC(fiction_layouts_gate_level_layout_is_inv))
        .def("is_and", &GateLyt::is_and, DOC(fiction_layouts_gate_level_layout_is_and))
        .def("is_nand", &GateLyt::is_nand, DOC(fiction_layouts_gate_level_layout_is_nand))
        .def("is_or", &GateLyt::is_or, DOC(fiction_layouts_gate_level_layout_is_or))
        .def("is_nor", &GateLyt::is_nor, DOC(fiction_layouts_gate_level_layout_is_nor))
        .def("is_xor", &GateLyt::is_xor, DOC(fiction_layouts_gate_level_layout_is_xor))
        .def("is_xnor", &GateLyt::is_xnor, DOC(fiction_layouts_gate_level_layout_is_xnor))
        .def("is_lt", &GateLyt::is_lt, DOC(fiction_layouts_gate_level_layout_is_lt))
        .def("is_le", &GateLyt::is_le, DOC(fiction_layouts_gate_level_layout_is_le))
        .def("is_gt", &GateLyt::is_gt, DOC(fiction_layouts_gate_level_layout_is_gt))
        .def("is_ge", &GateLyt::is_ge, DOC(fiction_layouts_gate_level_layout_is_ge))
        .def("is_maj", &GateLyt::is_maj, DOC(fiction_layouts_gate_level_layout_is_maj))
        .def("is_fanout", &GateLyt::is_fanout, DOC(fiction_layouts_gate_level_layout_is_fanout))
        .def("is_buf", &GateLyt::is_buf, DOC(fiction_layouts_gate_level_layout_is_buf))
        .def("is_wire", &GateLyt::is_wire, DOC(fiction_layouts_gate_level_layout_is_wire))

        .def("set_layout_name", &GateLyt::set_layout_name, nb::arg("name"),
             DOC(fiction_layouts_gate_level_layout_set_layout_name))
        .def("get_layout_name", &GateLyt::get_layout_name, DOC(fiction_layouts_gate_level_layout_get_layout_name))
        .def("clone", &GateLyt::clone, DOC(fiction_layouts_gate_level_layout_clone))
        .def("__copy__", &GateLyt::clone, "Returns an independent layout copy, including placed objects and metadata.")
        .def(
            "__deepcopy__", [](const GateLyt& lyt, const nb::dict&) { return lyt.clone(); }, nb::arg("memo"),
            "Returns an independent layout copy, including placed objects and metadata.")
        .def("set_input_name", &GateLyt::set_input_name, nb::arg("index"), nb::arg("name"),
             DOC(fiction_layouts_gate_level_layout_set_input_name))
        .def("get_input_name", &GateLyt::get_input_name, nb::arg("index"),
             DOC(fiction_layouts_gate_level_layout_get_input_name))
        .def("set_output_name", &GateLyt::set_output_name, nb::arg("index"), nb::arg("name"),
             DOC(fiction_layouts_gate_level_layout_set_output_name))
        .def("get_output_name", &GateLyt::get_output_name, nb::arg("index"),
             DOC(fiction_layouts_gate_level_layout_get_output_name))
        .def("get_name", nb::overload_cast<typename GateLyt::object_id>(&GateLyt::get_name, nb::const_),
             nb::arg("object"), DOC(fiction_layouts_gate_level_layout_get_name))

        .def("create_buf",
             nb::overload_cast<typename GateLyt::object_id, const fiction::layouts::layout_base::coordinate&>(
                 &GateLyt::create_buf),
             nb::arg("a"), nb::arg("t"), DOC(fiction_layouts_gate_level_layout_create_buf))
        .def("create_not", &GateLyt::create_not, nb::arg("a"), nb::arg("t"),
             DOC(fiction_layouts_gate_level_layout_create_not))
        .def("create_and", &GateLyt::create_and, nb::arg("a"), nb::arg("b"), nb::arg("t"),
             DOC(fiction_layouts_gate_level_layout_create_and))
        .def("create_nand", &GateLyt::create_nand, nb::arg("a"), nb::arg("b"), nb::arg("t"),
             DOC(fiction_layouts_gate_level_layout_create_nand))
        .def("create_or", &GateLyt::create_or, nb::arg("a"), nb::arg("b"), nb::arg("t"),
             DOC(fiction_layouts_gate_level_layout_create_or))
        .def("create_nor", &GateLyt::create_nor, nb::arg("a"), nb::arg("b"), nb::arg("t"),
             DOC(fiction_layouts_gate_level_layout_create_nor))
        .def("create_xor", &GateLyt::create_xor, nb::arg("a"), nb::arg("b"), nb::arg("t"),
             DOC(fiction_layouts_gate_level_layout_create_xor))
        .def("create_xnor", &GateLyt::create_xnor, nb::arg("a"), nb::arg("b"), nb::arg("t"),
             DOC(fiction_layouts_gate_level_layout_create_xnor))
        .def("create_lt", &GateLyt::create_lt, nb::arg("a"), nb::arg("b"), nb::arg("t"),
             DOC(fiction_layouts_gate_level_layout_create_lt))
        .def("create_le", &GateLyt::create_le, nb::arg("a"), nb::arg("b"), nb::arg("t"),
             DOC(fiction_layouts_gate_level_layout_create_le))
        .def("create_gt", &GateLyt::create_gt, nb::arg("a"), nb::arg("b"), nb::arg("t"),
             DOC(fiction_layouts_gate_level_layout_create_gt))
        .def("create_ge", &GateLyt::create_ge, nb::arg("a"), nb::arg("b"), nb::arg("t"),
             DOC(fiction_layouts_gate_level_layout_create_ge))
        .def("create_maj", &GateLyt::create_maj, nb::arg("a"), nb::arg("b"), nb::arg("c"), nb::arg("t"),
             DOC(fiction_layouts_gate_level_layout_create_maj))

        .def("num_pis", &GateLyt::num_pis, DOC(fiction_layouts_gate_level_layout_num_pis))
        .def("num_pos", &GateLyt::num_pos, DOC(fiction_layouts_gate_level_layout_num_pos))
        .def("num_gates", &GateLyt::num_gates, DOC(fiction_layouts_gate_level_layout_num_gates))
        .def("num_wires", &GateLyt::num_wires, DOC(fiction_layouts_gate_level_layout_num_wires))
        .def("num_crossings", &GateLyt::num_crossings, DOC(fiction_layouts_gate_level_layout_num_crossings))
        .def("is_empty", &GateLyt::is_empty, DOC(fiction_layouts_gate_level_layout_is_empty))

        // A truth-table argument implies that synthesis is loaded. Import synthesis only when returning a table;
        // importing it during layouts registration would create a cycle through networks.
        .def("create_gate", &GateLyt::create_gate, nb::arg("inputs"), nb::arg("function"), nb::arg("t"),
             nb::sig(
                 "def create_gate(self, inputs: collections.abc.Sequence[layout_object_id], function: "
                 "mnt.pyfiction.synthesis.dynamic_truth_table, t: mnt.pyfiction.layouts.coordinate | tuple[int, int] | "
                 "tuple[int, int, int]) -> "
                 "layout_object_id"),
             "Creates a placed gate. Input indices follow truth-table variable order; trailing inputs may be "
             "disconnected.")
        .def(
            "object_function",
            [](const GateLyt& lyt, const typename GateLyt::object_id id)
            {
                nb::module_::import_("mnt.pyfiction.synthesis");
                return lyt.object_function(id);
            },
            nb::arg("object"),
            nb::sig(
                "def object_function(self, object: layout_object_id) -> mnt.pyfiction.synthesis.dynamic_truth_table"),
            "Returns the object's truth table.")
        .def("size", &GateLyt::size, DOC(fiction_layouts_gate_level_layout_size))
        .def("fanin_size", &GateLyt::fanin_size, nb::arg("object"), DOC(fiction_layouts_gate_level_layout_fanin_size))
        .def("fanout_size", &GateLyt::fanout_size, nb::arg("object"),
             DOC(fiction_layouts_gate_level_layout_fanout_size))
        .def("input_count", &GateLyt::input_count, nb::arg("object"),
             DOC(fiction_layouts_gate_level_layout_input_count))
        .def("find_object", &GateLyt::find_object, nb::arg("t"), DOC(fiction_layouts_gate_level_layout_find_object))
        .def("contains", &GateLyt::contains, nb::arg("object"), DOC(fiction_layouts_gate_level_layout_contains))
        .def("get_tile", &GateLyt::get_tile, nb::arg("object"), DOC(fiction_layouts_gate_level_layout_get_tile))
        .def("source", &GateLyt::source, nb::arg("input"), DOC(fiction_layouts_gate_level_layout_source))
        .def("connect", &GateLyt::connect, nb::arg("source"), nb::arg("input"),
             DOC(fiction_layouts_gate_level_layout_connect))
        .def("disconnect", &GateLyt::disconnect, nb::arg("input"), DOC(fiction_layouts_gate_level_layout_disconnect))
        .def("remove", &GateLyt::remove, nb::arg("object"), DOC(fiction_layouts_gate_level_layout_remove))
        .def("move_object", &GateLyt::move_object, nb::arg("object"), nb::arg("t"),
             DOC(fiction_layouts_gate_level_layout_move_object))
        .def("pi_at", &GateLyt::pi_at, nb::arg("index"), DOC(fiction_layouts_gate_level_layout_pi_at))
        .def("po_at", &GateLyt::po_at, nb::arg("index"), DOC(fiction_layouts_gate_level_layout_po_at))
        .def(
            "set_input_order", [](GateLyt& lyt, const std::vector<typename GateLyt::object_id>& order)
            { lyt.set_input_order(order); }, nb::arg("order"), DOC(fiction_layouts_gate_level_layout_set_input_order))
        .def(
            "set_output_order", [](GateLyt& lyt, const std::vector<typename GateLyt::object_id>& order)
            { lyt.set_output_order(order); }, nb::arg("order"), DOC(fiction_layouts_gate_level_layout_set_output_order))
        .def("create_po",
             nb::overload_cast<const std::string&, const fiction::layouts::layout_base::coordinate&>(
                 &GateLyt::create_po),
             nb::arg("name"), nb::arg("t"), DOC(fiction_layouts_gate_level_layout_create_po_2))
        .def("create_buf", nb::overload_cast<const fiction::layouts::layout_base::coordinate&>(&GateLyt::create_buf),
             nb::arg("t"), DOC(fiction_layouts_gate_level_layout_create_buf_2))
        .def("set_name", nb::overload_cast<typename GateLyt::object_id, const std::string&>(&GateLyt::set_name),
             nb::arg("object"), nb::arg("name"), DOC(fiction_layouts_gate_level_layout_set_name))
        .def(
            "inputs",
            [](const GateLyt& lyt, const typename GateLyt::object_id object)
            {
                /** @brief Number of ordered input slots. */
                const auto                                              input_count = lyt.input_count(object);
                std::vector<std::optional<typename GateLyt::object_id>> inputs{};
                inputs.reserve(input_count);
                for (uint32_t i = 0; i < input_count; ++i)
                {
                    inputs.push_back(lyt.source({object, i}));
                }
                return inputs;
            },
            nb::arg("object"), "Returns input sources in port-index order, including None for disconnected ports.")
        .def(
            "sinks",
            [](const GateLyt& lyt, const typename GateLyt::object_id object)
            {
                std::vector<typename GateLyt::input_port> sinks{};
                sinks.reserve(lyt.fanout_size(object));
                lyt.foreach_sink(object, [&](const auto& input) { sinks.push_back(input); });
                return sinks;
            },
            nb::arg("object"),
            "Returns the destination input ports driven by this object's output. Each sink identifies the destination "
            "object and its input index. If the source drives several inputs of one destination, each input is a "
            "separate sink. Connections need not be physically legal; order is unspecified.")
        .def("clear_tile", &GateLyt::clear_tile, nb::arg("t"), DOC(fiction_layouts_gate_level_layout_clear_tile))

        .def("is_gate_tile", &GateLyt::is_gate_tile, nb::arg("t"), DOC(fiction_layouts_gate_level_layout_is_gate_tile))
        .def("is_wire_tile", &GateLyt::is_wire_tile, nb::arg("t"), DOC(fiction_layouts_gate_level_layout_is_wire_tile))
        .def("is_empty_tile", &GateLyt::is_empty_tile, nb::arg("t"),
             DOC(fiction_layouts_gate_level_layout_is_empty_tile))

        .def("pis",
             [](const GateLyt& lyt)
             {
                 std::vector<typename GateLyt::object_id> pis{};
                 pis.reserve(lyt.num_pis());
                 lyt.foreach_pi([&pis](const auto& pi) { pis.push_back(pi); });
                 return pis;
             })
        .def("pos",
             [](const GateLyt& lyt)
             {
                 std::vector<typename GateLyt::object_id> pos{};
                 pos.reserve(lyt.num_pos());
                 lyt.foreach_po([&pos](const auto& po) { pos.push_back(po); });
                 return pos;
             })
        .def("gates",
             [](const GateLyt& lyt)
             {
                 std::vector<typename GateLyt::object_id> gates{};
                 gates.reserve(lyt.num_gates());
                 lyt.foreach_gate([&gates](const auto& g) { gates.push_back(g); });
                 return gates;
             })
        .def("wires",
             [](const GateLyt& lyt)
             {
                 std::vector<typename GateLyt::object_id> wires{};
                 wires.reserve(lyt.num_wires());
                 lyt.foreach_wire([&wires](const auto& w) { wires.push_back(w); });
                 return wires;
             })

        .def(
            "incoming_data_flow", [](const GateLyt& layout, const fiction::layouts::layout_base::coordinate& tile)
            { return layout.template incoming_data_flow<true>(tile); }, nb::arg("t"))

        .def(
            "outgoing_data_flow", [](const GateLyt& layout, const fiction::layouts::layout_base::coordinate& tile)
            { return layout.template outgoing_data_flow<true>(tile); }, nb::arg("t"),
            "Returns adjacent, outgoing-clocked destination coordinates once per connected input port. "
            "Multiple connected inputs on one destination yield repeated coordinates; order is unspecified.")

        .def(
            "is_incoming_signal",
            [](const GateLyt& layout, const fiction::layouts::layout_base::coordinate& tile,
               const std::optional<fiction::layouts::layout_base::coordinate>& source)
            { return layout.template is_incoming_signal<true>(tile, source); },
            nb::arg("t"), nb::arg("s"), DOC(fiction_layouts_gate_level_layout_is_incoming_signal))

        .def(
            "has_no_incoming_signal", [](const GateLyt& layout, const fiction::layouts::layout_base::coordinate& tile)
            { return layout.template has_no_incoming_signal<true>(tile); }, nb::arg("t"),
            DOC(fiction_layouts_gate_level_layout_has_no_incoming_signal))

        .def(
            "has_northern_incoming_signal",
            [](const GateLyt& layout, const fiction::layouts::layout_base::coordinate& tile)
            { return layout.template has_northern_incoming_signal<true>(tile); }, nb::arg("t"),
            DOC(fiction_layouts_gate_level_layout_has_northern_incoming_signal))

        .def(
            "has_north_eastern_incoming_signal",
            [](const GateLyt& layout, const fiction::layouts::layout_base::coordinate& tile)
            { return layout.template has_north_eastern_incoming_signal<true>(tile); }, nb::arg("t"),
            DOC(fiction_layouts_gate_level_layout_has_north_eastern_incoming_signal))

        .def(
            "has_eastern_incoming_signal",
            [](const GateLyt& layout, const fiction::layouts::layout_base::coordinate& tile)
            { return layout.template has_eastern_incoming_signal<true>(tile); }, nb::arg("t"),
            DOC(fiction_layouts_gate_level_layout_has_eastern_incoming_signal))

        .def(
            "has_south_eastern_incoming_signal",
            [](const GateLyt& layout, const fiction::layouts::layout_base::coordinate& tile)
            { return layout.template has_south_eastern_incoming_signal<true>(tile); }, nb::arg("t"),
            DOC(fiction_layouts_gate_level_layout_has_south_eastern_incoming_signal))

        .def(
            "has_southern_incoming_signal",
            [](const GateLyt& layout, const fiction::layouts::layout_base::coordinate& tile)
            { return layout.template has_southern_incoming_signal<true>(tile); }, nb::arg("t"),
            DOC(fiction_layouts_gate_level_layout_has_southern_incoming_signal))

        .def(
            "has_south_western_incoming_signal",
            [](const GateLyt& layout, const fiction::layouts::layout_base::coordinate& tile)
            { return layout.template has_south_western_incoming_signal<true>(tile); }, nb::arg("t"),
            DOC(fiction_layouts_gate_level_layout_has_south_western_incoming_signal))

        .def(
            "has_western_incoming_signal",
            [](const GateLyt& layout, const fiction::layouts::layout_base::coordinate& tile)
            { return layout.template has_western_incoming_signal<true>(tile); }, nb::arg("t"),
            DOC(fiction_layouts_gate_level_layout_has_western_incoming_signal))

        .def(
            "has_north_western_incoming_signal",
            [](const GateLyt& layout, const fiction::layouts::layout_base::coordinate& tile)
            { return layout.template has_north_western_incoming_signal<true>(tile); }, nb::arg("t"),
            DOC(fiction_layouts_gate_level_layout_has_north_western_incoming_signal))

        .def(
            "is_outgoing_signal",
            [](const GateLyt& layout, const fiction::layouts::layout_base::coordinate& tile,
               const std::optional<fiction::layouts::layout_base::coordinate>& destination)
            { return layout.template is_outgoing_signal<true>(tile, destination); },
            nb::arg("t"), nb::arg("s"), DOC(fiction_layouts_gate_level_layout_is_outgoing_signal))

        .def(
            "has_no_outgoing_signal", [](const GateLyt& layout, const fiction::layouts::layout_base::coordinate& tile)
            { return layout.template has_no_outgoing_signal<true>(tile); }, nb::arg("t"),
            DOC(fiction_layouts_gate_level_layout_has_no_outgoing_signal))

        .def(
            "has_northern_outgoing_signal",
            [](const GateLyt& layout, const fiction::layouts::layout_base::coordinate& tile)
            { return layout.template has_northern_outgoing_signal<true>(tile); }, nb::arg("t"),
            DOC(fiction_layouts_gate_level_layout_has_northern_outgoing_signal))

        .def(
            "has_north_eastern_outgoing_signal",
            [](const GateLyt& layout, const fiction::layouts::layout_base::coordinate& tile)
            { return layout.template has_north_eastern_outgoing_signal<true>(tile); }, nb::arg("t"),
            DOC(fiction_layouts_gate_level_layout_has_north_eastern_outgoing_signal))

        .def(
            "has_eastern_outgoing_signal",
            [](const GateLyt& layout, const fiction::layouts::layout_base::coordinate& tile)
            { return layout.template has_eastern_outgoing_signal<true>(tile); }, nb::arg("t"),
            DOC(fiction_layouts_gate_level_layout_has_eastern_outgoing_signal))

        .def(
            "has_south_eastern_outgoing_signal",
            [](const GateLyt& layout, const fiction::layouts::layout_base::coordinate& tile)
            { return layout.template has_south_eastern_outgoing_signal<true>(tile); }, nb::arg("t"),
            DOC(fiction_layouts_gate_level_layout_has_south_eastern_outgoing_signal))

        .def(
            "has_southern_outgoing_signal",
            [](const GateLyt& layout, const fiction::layouts::layout_base::coordinate& tile)
            { return layout.template has_southern_outgoing_signal<true>(tile); }, nb::arg("t"),
            DOC(fiction_layouts_gate_level_layout_has_southern_outgoing_signal))

        .def(
            "has_south_western_outgoing_signal",
            [](const GateLyt& layout, const fiction::layouts::layout_base::coordinate& tile)
            { return layout.template has_south_western_outgoing_signal<true>(tile); }, nb::arg("t"),
            DOC(fiction_layouts_gate_level_layout_has_south_western_outgoing_signal))

        .def(
            "has_western_outgoing_signal",
            [](const GateLyt& layout, const fiction::layouts::layout_base::coordinate& tile)
            { return layout.template has_western_outgoing_signal<true>(tile); }, nb::arg("t"),
            DOC(fiction_layouts_gate_level_layout_has_western_outgoing_signal))

        .def(
            "has_north_western_outgoing_signal",
            [](const GateLyt& layout, const fiction::layouts::layout_base::coordinate& tile)
            { return layout.template has_north_western_outgoing_signal<true>(tile); }, nb::arg("t"),
            DOC(fiction_layouts_gate_level_layout_has_north_western_outgoing_signal))

        .def(
            "bounding_box",
            [](const GateLyt& layout)
            {
                const auto bb = fiction::layouts::bounding_box<GateLyt>(layout);
                return std::make_pair(bb.get_min(), bb.get_max());
            },
            BOUNDING_BOX_DOC)
        .def(
            "__repr__",
            [](const GateLyt& lyt) -> std::string
            {
                std::stringstream stream{};
                fiction::layouts::io::print_layout(lyt, stream);
                return stream.str();
            },
            "Returns a string representation of the layout.")

        .def("assign_synchronization_element", &GateLyt::assign_synchronization_element, nb::arg("coordinate"),
             nb::arg("delay"), DOC(fiction_layouts_gate_level_layout_assign_synchronization_element))
        .def("is_synchronization_element", &GateLyt::is_synchronization_element, nb::arg("coordinate"),
             DOC(fiction_layouts_gate_level_layout_is_synchronization_element))
        .def("get_synchronization_element", &GateLyt::get_synchronization_element, nb::arg("coordinate"),
             DOC(fiction_layouts_gate_level_layout_get_synchronization_element))
        .def("num_se", &GateLyt::num_se, DOC(fiction_layouts_gate_level_layout_num_se));
}

}  // namespace detail

/** @brief Register object IDs, ports, and gate layouts. @param m Python module. */
void gate_level_layout(nb::module_& m)
{
    /** @brief Layout-local object identity. */
    using object_id = fiction::layouts::layout_object_id;
    /** @brief Ordered input endpoint. */
    using input_port = fiction::layouts::layout_input_port;
    nb::class_<object_id>(m, "layout_object_id", "Layout-local generation-checked object identity.")
        .def(nb::init<uint32_t, uint32_t>(), nb::arg("index"), nb::arg("generation"))
        .def_ro("index", &object_id::index)
        .def_ro("generation", &object_id::generation)
        .def(
            "__eq__", [](const object_id a, const object_id b) { return a == b; }, nb::is_operator())
        .def("__hash__", [](const object_id id) { return std::hash<object_id>{}(id); });
    nb::class_<input_port>(m, "layout_input_port",
                           "A destination input port, identified by its object and truth-table argument index. "
                           "A connected input port is a sink of its source object's output.")
        .def(nb::init<object_id, uint32_t>(), nb::arg("object"), nb::arg("index"))
        .def_ro("object", &input_port::object)
        .def_ro("index", &input_port::index)
        .def(
            "__eq__", [](const input_port a, const input_port b) { return a == b; }, nb::is_operator())
        .def("__hash__", [](const input_port input) { return nb::hash(nb::make_tuple(input.object, input.index)); });

    /**
     * Gate-level clocked Cartesian layout.
     */
    detail::gate_level_layout<py_cartesian_layout, py_cartesian_gate_layout>(m, "cartesian");
    /**
     * Gate-level clocked shifted Cartesian layout.
     */
    detail::gate_level_layout<py_shifted_cartesian_layout, py_shifted_cartesian_gate_layout>(m, "shifted_cartesian");
    /**
     * Gate-level clocked hexagonal layout.
     */
    detail::gate_level_layout<py_hexagonal_layout, py_hexagonal_gate_layout>(m, "hexagonal");
}

}  // namespace pyfiction
