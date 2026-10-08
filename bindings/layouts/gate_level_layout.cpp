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

#include "pyfiction/documentation.hpp"
#include "pyfiction/types.hpp"

#include <fiction/layouts/arrangement.hpp>
#include <fiction/layouts/bounding_box.hpp>
#include <fiction/layouts/clocking_scheme.hpp>
#include <fiction/layouts/gate_level_layout.hpp>
#include <fiction/layouts/io/print_layout.hpp>
#include <fiction/traits.hpp>

#include <fmt/format.h>

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
void gate_level_layout(nanobind::module_& m, const std::string& topology)
{
    namespace py = nanobind;

    py::class_<GateLyt, LytBase> cls(m, fmt::format("{}_gate_layout", topology).c_str(),
                                     DOC(fiction_layouts_gate_level_layout));

    if constexpr (fiction::is_cartesian_layout_v<GateLyt>)
    {
        cls.def(py::init<>(), DOC(fiction_layouts_gate_level_layout_gate_level_layout))
            .def(py::init<const fiction::extent<GateLyt>&>(), py::arg("extent"),
                 DOC(fiction_layouts_gate_level_layout_gate_level_layout))
            .def(
                "__init__",
                [](py::pointer_and_handle<GateLyt> self, const fiction::extent<GateLyt>& extent,
                   const std::string& scheme_name, const std::string& layout_name)
                {
                    if (const auto scheme = fiction::layouts::clocking::get_scheme(scheme_name); scheme.has_value())
                    {
                        std::construct_at(self.p, extent, *scheme, layout_name);
                        return;
                    }

                    throw std::invalid_argument("Unknown clocking scheme");
                },
                py::arg("extent"), py::arg("clocking_scheme") = "2DDWave", py::arg("layout_name") = "",
                fmt::format("{}\n\nRaises:\n    ValueError: The clocking scheme name is unknown.",
                            DOC(fiction_layouts_gate_level_layout_gate_level_layout_2))
                    .c_str());
    }
    else
    {
        cls.def(py::init<fiction::layouts::arrangement>(), py::arg("arrangement"),
                DOC(fiction_layouts_gate_level_layout_gate_level_layout_3))
            .def(py::init<fiction::layouts::arrangement, const fiction::extent<GateLyt>&>(), py::arg("arrangement"),
                 py::arg("extent"), DOC(fiction_layouts_gate_level_layout_gate_level_layout_3))
            .def(
                "__init__",
                [](py::pointer_and_handle<GateLyt> self, const fiction::layouts::arrangement a,
                   const fiction::extent<GateLyt>& extent, const std::string& scheme_name,
                   const std::string& layout_name)
                {
                    // only hexagonal layouts take their arrangement into account for clocking scheme lookup
                    if (const auto scheme = fiction::layouts::clocking::get_scheme(
                            scheme_name, fiction::is_hexagonal_layout_v<GateLyt> ? std::optional{a} : std::nullopt);
                        scheme.has_value())
                    {
                        std::construct_at(self.p, a, extent, *scheme, layout_name);
                        return;
                    }

                    throw std::invalid_argument("Unknown clocking scheme");
                },
                py::arg("arrangement"), py::arg("extent"), py::arg("clocking_scheme") = "2DDWave",
                py::arg("layout_name") = "",
                fmt::format("{}\n\nRaises:\n    ValueError: The clocking scheme name is unknown.",
                            DOC(fiction_layouts_gate_level_layout_gate_level_layout_4))
                    .c_str());
    }

    cls.def("assign_clock_number", &GateLyt::assign_clock_number, py::arg("cz"), py::arg("cn"),
            DOC(fiction_layouts_gate_level_layout_assign_clock_number))
        .def("get_clock_number", &GateLyt::get_clock_number, py::arg("cz"),
             DOC(fiction_layouts_gate_level_layout_get_clock_number))
        .def("num_clocks", &GateLyt::num_clocks, DOC(fiction_layouts_gate_level_layout_num_clocks))
        .def("is_regularly_clocked", &GateLyt::is_regularly_clocked,
             DOC(fiction_layouts_gate_level_layout_is_regularly_clocked))

        .def("is_clocking_scheme", &GateLyt::is_clocking_scheme, py::arg("name"),
             DOC(fiction_layouts_gate_level_layout_is_clocking_scheme))
        .def(
            "get_clocking_scheme_name", [](const GateLyt& lyt) { return lyt.get_clocking_scheme().name(); },
            "Returns the name of the layout's clocking scheme, e.g., `2DDWave` or `USE`.")

        .def("is_incoming_clocked", &GateLyt::is_incoming_clocked, py::arg("cz1"), py::arg("cz2"),
             DOC(fiction_layouts_gate_level_layout_is_incoming_clocked))
        .def("is_outgoing_clocked", &GateLyt::is_outgoing_clocked, py::arg("cz1"), py::arg("cz2"),
             DOC(fiction_layouts_gate_level_layout_is_outgoing_clocked))

        .def("incoming_clocked_zones", &GateLyt::incoming_clocked_zones, py::arg("cz"),
             DOC(fiction_layouts_gate_level_layout_incoming_clocked_zones))
        .def("outgoing_clocked_zones", &GateLyt::outgoing_clocked_zones, py::arg("cz"),
             DOC(fiction_layouts_gate_level_layout_outgoing_clocked_zones))

        .def("in_degree", &GateLyt::in_degree, py::arg("cz"), DOC(fiction_layouts_gate_level_layout_in_degree))
        .def("out_degree", &GateLyt::out_degree, py::arg("cz"), DOC(fiction_layouts_gate_level_layout_out_degree))
        .def("degree", &GateLyt::degree, py::arg("cz"), DOC(fiction_layouts_gate_level_layout_degree))

        .def(
            "replace_clocking_scheme",
            [](GateLyt& lyt, const std::string& name)
            {
                if (const auto scheme = fiction::layouts::clocking::get_scheme(
                        name,
                        [](const GateLyt& layout) -> std::optional<fiction::layouts::arrangement>
                        {
                            if constexpr (fiction::is_hexagonal_layout_v<GateLyt>)
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
            py::arg("name"),
            "Replaces the clocking scheme by the predefined scheme of the given name. Clock-number overrides are "
            "discarded; synchronization elements are kept. Raises ValueError for an unknown name.")
        .def("obstruct_coordinate", &GateLyt::obstruct_coordinate, py::arg("c"),
             DOC(fiction_layouts_gate_level_layout_obstruct_coordinate))
        .def("obstruct_connection", &GateLyt::obstruct_connection, py::arg("src"), py::arg("tgt"),
             DOC(fiction_layouts_gate_level_layout_obstruct_connection))
        .def("clear_obstructed_coordinate", &GateLyt::clear_obstructed_coordinate, py::arg("c"),
             DOC(fiction_layouts_gate_level_layout_clear_obstructed_coordinate))
        .def("clear_obstructed_connection", &GateLyt::clear_obstructed_connection, py::arg("src"), py::arg("tgt"),
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
        .def("is_obstructed_coordinate", &GateLyt::is_obstructed_coordinate, py::arg("c"),
             DOC(fiction_layouts_gate_level_layout_is_obstructed_coordinate))
        .def("is_obstructed_connection", &GateLyt::is_obstructed_connection, py::arg("src"), py::arg("tgt"),
             DOC(fiction_layouts_gate_level_layout_is_obstructed_connection))

        .def("create_pi", &GateLyt::create_pi, py::arg("name"), py::arg("t"),
             DOC(fiction_layouts_gate_level_layout_create_pi))
        .def("create_po",
             py::overload_cast<typename GateLyt::object_id, const std::string&, const fiction::tile<GateLyt>&>(
                 &GateLyt::create_po),
             py::arg("s"), py::arg("name"), py::arg("t"), DOC(fiction_layouts_gate_level_layout_create_po))
        .def("is_pi", &GateLyt::is_pi, py::arg("n"), DOC(fiction_layouts_gate_level_layout_is_pi))
        .def("is_po", &GateLyt::is_po, py::arg("n"), DOC(fiction_layouts_gate_level_layout_is_po))
        .def("is_pi_tile", &GateLyt::is_pi_tile, py::arg("t"), DOC(fiction_layouts_gate_level_layout_is_pi_tile))
        .def("is_po_tile", &GateLyt::is_po_tile, py::arg("t"), DOC(fiction_layouts_gate_level_layout_is_po_tile))

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
        .def("is_wire", &GateLyt::is_wire, DOC(fiction_layouts_gate_level_layout_is_wire))

        .def("set_layout_name", &GateLyt::set_layout_name, py::arg("name"),
             DOC(fiction_layouts_gate_level_layout_set_layout_name))
        .def("get_layout_name", &GateLyt::get_layout_name, DOC(fiction_layouts_gate_level_layout_get_layout_name))
        .def("clone", &GateLyt::clone, DOC(fiction_layouts_gate_level_layout_clone))
        .def("__copy__", &GateLyt::clone, "Returns an independent layout copy, including placed objects and metadata.")
        .def(
            "__deepcopy__", [](const GateLyt& lyt, const py::dict&) { return lyt.clone(); }, py::arg("memo"),
            "Returns an independent layout copy, including placed objects and metadata.")
        .def("set_input_name", &GateLyt::set_input_name, py::arg("index"), py::arg("name"),
             DOC(fiction_layouts_gate_level_layout_set_input_name))
        .def("get_input_name", &GateLyt::get_input_name, py::arg("index"),
             DOC(fiction_layouts_gate_level_layout_get_input_name))
        .def("set_output_name", &GateLyt::set_output_name, py::arg("index"), py::arg("name"),
             DOC(fiction_layouts_gate_level_layout_set_output_name))
        .def("get_output_name", &GateLyt::get_output_name, py::arg("index"),
             DOC(fiction_layouts_gate_level_layout_get_output_name))
        .def("get_name", py::overload_cast<typename GateLyt::object_id>(&GateLyt::get_name, py::const_),
             py::arg("object"), DOC(fiction_layouts_gate_level_layout_get_name))

        .def("create_buf",
             py::overload_cast<typename GateLyt::object_id, const fiction::tile<GateLyt>&>(&GateLyt::create_buf),
             py::arg("a"), py::arg("t"), DOC(fiction_layouts_gate_level_layout_create_buf))
        .def("create_not", &GateLyt::create_not, py::arg("a"), py::arg("t"),
             DOC(fiction_layouts_gate_level_layout_create_not))
        .def("create_and", &GateLyt::create_and, py::arg("a"), py::arg("b"), py::arg("t"),
             DOC(fiction_layouts_gate_level_layout_create_and))
        .def("create_nand", &GateLyt::create_nand, py::arg("a"), py::arg("b"), py::arg("t"),
             DOC(fiction_layouts_gate_level_layout_create_nand))
        .def("create_or", &GateLyt::create_or, py::arg("a"), py::arg("b"), py::arg("t"),
             DOC(fiction_layouts_gate_level_layout_create_or))
        .def("create_nor", &GateLyt::create_nor, py::arg("a"), py::arg("b"), py::arg("t"),
             DOC(fiction_layouts_gate_level_layout_create_nor))
        .def("create_xor", &GateLyt::create_xor, py::arg("a"), py::arg("b"), py::arg("t"),
             DOC(fiction_layouts_gate_level_layout_create_xor))
        .def("create_xnor", &GateLyt::create_xnor, py::arg("a"), py::arg("b"), py::arg("t"),
             DOC(fiction_layouts_gate_level_layout_create_xnor))
        .def("create_lt", &GateLyt::create_lt, py::arg("a"), py::arg("b"), py::arg("t"),
             DOC(fiction_layouts_gate_level_layout_create_lt))
        .def("create_le", &GateLyt::create_le, py::arg("a"), py::arg("b"), py::arg("t"),
             DOC(fiction_layouts_gate_level_layout_create_le))
        .def("create_gt", &GateLyt::create_gt, py::arg("a"), py::arg("b"), py::arg("t"),
             DOC(fiction_layouts_gate_level_layout_create_gt))
        .def("create_ge", &GateLyt::create_ge, py::arg("a"), py::arg("b"), py::arg("t"),
             DOC(fiction_layouts_gate_level_layout_create_ge))
        .def("create_maj", &GateLyt::create_maj, py::arg("a"), py::arg("b"), py::arg("c"), py::arg("t"),
             DOC(fiction_layouts_gate_level_layout_create_maj))

        .def("num_pis", &GateLyt::num_pis, DOC(fiction_layouts_gate_level_layout_num_pis))
        .def("num_pos", &GateLyt::num_pos, DOC(fiction_layouts_gate_level_layout_num_pos))
        .def("num_gates", &GateLyt::num_gates, DOC(fiction_layouts_gate_level_layout_num_gates))
        .def("num_wires", &GateLyt::num_wires, DOC(fiction_layouts_gate_level_layout_num_wires))
        .def("num_crossings", &GateLyt::num_crossings, DOC(fiction_layouts_gate_level_layout_num_crossings))
        .def("is_empty", &GateLyt::is_empty, DOC(fiction_layouts_gate_level_layout_is_empty))

        // A truth-table argument implies that synthesis is loaded. Import synthesis only when returning a table;
        // importing it during layouts registration would create a cycle through networks.
        .def("create_gate", &GateLyt::create_gate, py::arg("inputs"), py::arg("function"), py::arg("t"),
             py::sig(
                 "def create_gate(self, inputs: collections.abc.Sequence[LayoutObjectId], function: "
                 "mnt.pyfiction.synthesis.dynamic_truth_table, t: mnt.pyfiction.layouts.coordinate | tuple[int, int] | "
                 "tuple[int, int, int]) -> "
                 "LayoutObjectId"),
             "Creates a placed gate. Input indices follow truth-table variable order; trailing inputs may be "
             "disconnected.")
        .def(
            "object_function",
            [](const GateLyt& lyt, const typename GateLyt::object_id id)
            {
                py::module_::import_("mnt.pyfiction.synthesis");
                return lyt.object_function(id);
            },
            py::arg("object"),
            py::sig("def object_function(self, object: LayoutObjectId) -> mnt.pyfiction.synthesis.dynamic_truth_table"),
            "Returns the object's truth table.")
        .def("size", &GateLyt::size, DOC(fiction_layouts_gate_level_layout_size))
        .def("fanin_size", &GateLyt::fanin_size, py::arg("object"), DOC(fiction_layouts_gate_level_layout_fanin_size))
        .def("fanout_size", &GateLyt::fanout_size, py::arg("object"),
             DOC(fiction_layouts_gate_level_layout_fanout_size))
        .def("input_count", &GateLyt::input_count, py::arg("object"),
             DOC(fiction_layouts_gate_level_layout_input_count))
        .def("find_object", &GateLyt::find_object, py::arg("t"), DOC(fiction_layouts_gate_level_layout_find_object))
        .def("contains", &GateLyt::contains, py::arg("object"), DOC(fiction_layouts_gate_level_layout_contains))
        .def("get_tile", &GateLyt::get_tile, py::arg("object"), DOC(fiction_layouts_gate_level_layout_get_tile))
        .def("source", &GateLyt::source, py::arg("input"), DOC(fiction_layouts_gate_level_layout_source))
        .def("connect", &GateLyt::connect, py::arg("source"), py::arg("input"),
             DOC(fiction_layouts_gate_level_layout_connect))
        .def("disconnect", &GateLyt::disconnect, py::arg("input"), DOC(fiction_layouts_gate_level_layout_disconnect))
        .def("remove", &GateLyt::remove, py::arg("object"), DOC(fiction_layouts_gate_level_layout_remove))
        .def("move_object", &GateLyt::move_object, py::arg("object"), py::arg("t"),
             DOC(fiction_layouts_gate_level_layout_move_object))
        .def("pi_at", &GateLyt::pi_at, py::arg("index"), DOC(fiction_layouts_gate_level_layout_pi_at))
        .def("po_at", &GateLyt::po_at, py::arg("index"), DOC(fiction_layouts_gate_level_layout_po_at))
        .def(
            "set_input_order", [](GateLyt& lyt, const std::vector<typename GateLyt::object_id>& order)
            { lyt.set_input_order(order); }, py::arg("order"), DOC(fiction_layouts_gate_level_layout_set_input_order))
        .def(
            "set_output_order", [](GateLyt& lyt, const std::vector<typename GateLyt::object_id>& order)
            { lyt.set_output_order(order); }, py::arg("order"), DOC(fiction_layouts_gate_level_layout_set_output_order))
        .def("create_po", py::overload_cast<const std::string&, const fiction::tile<GateLyt>&>(&GateLyt::create_po),
             py::arg("name"), py::arg("t"), DOC(fiction_layouts_gate_level_layout_create_po_2))
        .def("create_buf", py::overload_cast<const fiction::tile<GateLyt>&>(&GateLyt::create_buf), py::arg("t"),
             DOC(fiction_layouts_gate_level_layout_create_buf_2))
        .def("set_name", py::overload_cast<typename GateLyt::object_id, const std::string&>(&GateLyt::set_name),
             py::arg("object"), py::arg("name"), DOC(fiction_layouts_gate_level_layout_set_name))
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
            py::arg("object"), "Returns input sources in port-index order, including None for disconnected ports.")
        .def(
            "sinks",
            [](const GateLyt& lyt, const typename GateLyt::object_id object)
            {
                std::vector<typename GateLyt::input_port> sinks{};
                sinks.reserve(lyt.fanout_size(object));
                lyt.foreach_sink(object, [&](const auto& input) { sinks.push_back(input); });
                return sinks;
            },
            py::arg("object"),
            "Returns the connected sink input ports, including repeated inputs of one object. Sink order is "
            "unspecified.")
        .def("clear_tile", &GateLyt::clear_tile, py::arg("t"), DOC(fiction_layouts_gate_level_layout_clear_tile))

        .def("is_gate_tile", &GateLyt::is_gate_tile, py::arg("t"), DOC(fiction_layouts_gate_level_layout_is_gate_tile))
        .def("is_wire_tile", &GateLyt::is_wire_tile, py::arg("t"), DOC(fiction_layouts_gate_level_layout_is_wire_tile))
        .def("is_empty_tile", &GateLyt::is_empty_tile, py::arg("t"),
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
            "incoming_data_flow", [](const GateLyt& layout, const fiction::tile<GateLyt>& tile)
            { return layout.template incoming_data_flow<true>(tile); }, py::arg("t"))

        .def(
            "outgoing_data_flow", [](const GateLyt& layout, const fiction::tile<GateLyt>& tile)
            { return layout.template outgoing_data_flow<true>(tile); }, py::arg("t"))

        .def(
            "is_incoming_signal",
            [](const GateLyt& layout, const fiction::tile<GateLyt>& tile,
               const std::optional<fiction::tile<GateLyt>>& source)
            { return layout.template is_incoming_signal<true>(tile, source); },
            py::arg("t"), py::arg("s"), DOC(fiction_layouts_gate_level_layout_is_incoming_signal))

        .def(
            "has_no_incoming_signal", [](const GateLyt& layout, const fiction::tile<GateLyt>& tile)
            { return layout.template has_no_incoming_signal<true>(tile); }, py::arg("t"),
            DOC(fiction_layouts_gate_level_layout_has_no_incoming_signal))

        .def(
            "has_northern_incoming_signal", [](const GateLyt& layout, const fiction::tile<GateLyt>& tile)
            { return layout.template has_northern_incoming_signal<true>(tile); }, py::arg("t"),
            DOC(fiction_layouts_gate_level_layout_has_northern_incoming_signal))

        .def(
            "has_north_eastern_incoming_signal", [](const GateLyt& layout, const fiction::tile<GateLyt>& tile)
            { return layout.template has_north_eastern_incoming_signal<true>(tile); }, py::arg("t"),
            DOC(fiction_layouts_gate_level_layout_has_north_eastern_incoming_signal))

        .def(
            "has_eastern_incoming_signal", [](const GateLyt& layout, const fiction::tile<GateLyt>& tile)
            { return layout.template has_eastern_incoming_signal<true>(tile); }, py::arg("t"),
            DOC(fiction_layouts_gate_level_layout_has_eastern_incoming_signal))

        .def(
            "has_south_eastern_incoming_signal", [](const GateLyt& layout, const fiction::tile<GateLyt>& tile)
            { return layout.template has_south_eastern_incoming_signal<true>(tile); }, py::arg("t"),
            DOC(fiction_layouts_gate_level_layout_has_south_eastern_incoming_signal))

        .def(
            "has_southern_incoming_signal", [](const GateLyt& layout, const fiction::tile<GateLyt>& tile)
            { return layout.template has_southern_incoming_signal<true>(tile); }, py::arg("t"),
            DOC(fiction_layouts_gate_level_layout_has_southern_incoming_signal))

        .def(
            "has_south_western_incoming_signal", [](const GateLyt& layout, const fiction::tile<GateLyt>& tile)
            { return layout.template has_south_western_incoming_signal<true>(tile); }, py::arg("t"),
            DOC(fiction_layouts_gate_level_layout_has_south_western_incoming_signal))

        .def(
            "has_western_incoming_signal", [](const GateLyt& layout, const fiction::tile<GateLyt>& tile)
            { return layout.template has_western_incoming_signal<true>(tile); }, py::arg("t"),
            DOC(fiction_layouts_gate_level_layout_has_western_incoming_signal))

        .def(
            "has_north_western_incoming_signal", [](const GateLyt& layout, const fiction::tile<GateLyt>& tile)
            { return layout.template has_north_western_incoming_signal<true>(tile); }, py::arg("t"),
            DOC(fiction_layouts_gate_level_layout_has_north_western_incoming_signal))

        .def(
            "is_outgoing_signal",
            [](const GateLyt& layout, const fiction::tile<GateLyt>& tile,
               const std::optional<fiction::tile<GateLyt>>& destination)
            { return layout.template is_outgoing_signal<true>(tile, destination); },
            py::arg("t"), py::arg("s"), DOC(fiction_layouts_gate_level_layout_is_outgoing_signal))

        .def(
            "has_no_outgoing_signal", [](const GateLyt& layout, const fiction::tile<GateLyt>& tile)
            { return layout.template has_no_outgoing_signal<true>(tile); }, py::arg("t"),
            DOC(fiction_layouts_gate_level_layout_has_no_outgoing_signal))

        .def(
            "has_northern_outgoing_signal", [](const GateLyt& layout, const fiction::tile<GateLyt>& tile)
            { return layout.template has_northern_outgoing_signal<true>(tile); }, py::arg("t"),
            DOC(fiction_layouts_gate_level_layout_has_northern_outgoing_signal))

        .def(
            "has_north_eastern_outgoing_signal", [](const GateLyt& layout, const fiction::tile<GateLyt>& tile)
            { return layout.template has_north_eastern_outgoing_signal<true>(tile); }, py::arg("t"),
            DOC(fiction_layouts_gate_level_layout_has_north_eastern_outgoing_signal))

        .def(
            "has_eastern_outgoing_signal", [](const GateLyt& layout, const fiction::tile<GateLyt>& tile)
            { return layout.template has_eastern_outgoing_signal<true>(tile); }, py::arg("t"),
            DOC(fiction_layouts_gate_level_layout_has_eastern_outgoing_signal))

        .def(
            "has_south_eastern_outgoing_signal", [](const GateLyt& layout, const fiction::tile<GateLyt>& tile)
            { return layout.template has_south_eastern_outgoing_signal<true>(tile); }, py::arg("t"),
            DOC(fiction_layouts_gate_level_layout_has_south_eastern_outgoing_signal))

        .def(
            "has_southern_outgoing_signal", [](const GateLyt& layout, const fiction::tile<GateLyt>& tile)
            { return layout.template has_southern_outgoing_signal<true>(tile); }, py::arg("t"),
            DOC(fiction_layouts_gate_level_layout_has_southern_outgoing_signal))

        .def(
            "has_south_western_outgoing_signal", [](const GateLyt& layout, const fiction::tile<GateLyt>& tile)
            { return layout.template has_south_western_outgoing_signal<true>(tile); }, py::arg("t"),
            DOC(fiction_layouts_gate_level_layout_has_south_western_outgoing_signal))

        .def(
            "has_western_outgoing_signal", [](const GateLyt& layout, const fiction::tile<GateLyt>& tile)
            { return layout.template has_western_outgoing_signal<true>(tile); }, py::arg("t"),
            DOC(fiction_layouts_gate_level_layout_has_western_outgoing_signal))

        .def(
            "has_north_western_outgoing_signal", [](const GateLyt& layout, const fiction::tile<GateLyt>& tile)
            { return layout.template has_north_western_outgoing_signal<true>(tile); }, py::arg("t"),
            DOC(fiction_layouts_gate_level_layout_has_north_western_outgoing_signal))

        .def(
            "bounding_box_2d",
            [](const GateLyt& layout)
            {
                const auto bb = fiction::layouts::bounding_box_2d<GateLyt>(layout);
                return std::make_pair(bb.get_min(), bb.get_max());
            },
            BOUNDING_BOX_2D_DOC)
        .def(
            "__repr__",
            [](const GateLyt& lyt) -> std::string
            {
                std::stringstream stream{};
                fiction::layouts::io::print_layout(lyt, stream);
                return stream.str();
            },
            "Returns a string representation of the layout.")

        .def("assign_synchronization_element", &GateLyt::assign_synchronization_element, py::arg("coordinate"),
             py::arg("delay"), DOC(fiction_layouts_gate_level_layout_assign_synchronization_element))
        .def("is_synchronization_element", &GateLyt::is_synchronization_element, py::arg("coordinate"),
             DOC(fiction_layouts_gate_level_layout_is_synchronization_element))
        .def("get_synchronization_element", &GateLyt::get_synchronization_element, py::arg("coordinate"),
             DOC(fiction_layouts_gate_level_layout_get_synchronization_element))
        .def("num_se", &GateLyt::num_se, DOC(fiction_layouts_gate_level_layout_num_se));
}

}  // namespace detail

/** @brief Register object IDs, ports, and gate layouts. @param m Python module. */
void gate_level_layout(nanobind::module_& m)
{
    namespace py = nanobind;
    /** @brief Layout-local object identity. */
    using object_id = fiction::layouts::layout_object_id;
    /** @brief Ordered input endpoint. */
    using input_port = fiction::layouts::layout_input_port;
    py::class_<object_id>(m, "LayoutObjectId", "Layout-local generation-checked object identity.")
        .def(py::init<uint32_t, uint32_t>(), py::arg("index"), py::arg("generation"))
        .def_ro("index", &object_id::index)
        .def_ro("generation", &object_id::generation)
        .def(
            "__eq__", [](const object_id a, const object_id b) { return a == b; }, py::is_operator())
        .def("__hash__", [](const object_id id) { return std::hash<object_id>{}(id); });
    py::class_<input_port>(m, "LayoutInputPort", "An object's ordered input port.")
        .def(py::init<object_id, uint32_t>(), py::arg("object"), py::arg("index"))
        .def_ro("object", &input_port::object)
        .def_ro("index", &input_port::index)
        .def(
            "__eq__", [](const input_port a, const input_port b) { return a == b; }, py::is_operator())
        .def("__hash__", [](const input_port input) { return py::hash(py::make_tuple(input.object, input.index)); });

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
