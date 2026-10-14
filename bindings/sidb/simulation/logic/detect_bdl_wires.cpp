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
 * @brief Python bindings for `fiction/technology/sidb/simulation/logic/detect_bdl_wires.hpp`.
 * @author Jan Drewniok (Drewniok)
 * @author Marcel Walter (marcelwa)
 */

#include "pyfiction/bdl_parameters.hpp"
#include "pyfiction/documentation.hpp"

#include <fiction/technology/fcn/cell_ports.hpp>
#include <fiction/technology/sidb/layout.hpp>
#include <fiction/technology/sidb/simulation/logic/detect_bdl_pairs.hpp>
#include <fiction/technology/sidb/simulation/logic/detect_bdl_wires.hpp>

#include <vector>

#include <nanobind/nanobind.h>
#include <nanobind/stl/optional.h>       // NOLINT(misc-include-cleaner)
#include <nanobind/stl/pair.h>           // NOLINT(misc-include-cleaner)
#include <nanobind/stl/set.h>            // NOLINT(misc-include-cleaner)
#include <nanobind/stl/string.h>         // NOLINT(misc-include-cleaner)
#include <nanobind/stl/tuple.h>          // NOLINT(misc-include-cleaner)
#include <nanobind/stl/unordered_map.h>  // NOLINT(misc-include-cleaner)
#include <nanobind/stl/unordered_set.h>  // NOLINT(misc-include-cleaner)
#include <nanobind/stl/vector.h>         // NOLINT(misc-include-cleaner)

/**
 * @brief Namespace alias for nanobind.
 */
namespace nb = nanobind;

namespace pyfiction
{

/** @brief Native options constructed for each Python call. */
using detect_bdl_wires_options = fiction::sidb::simulation::logic::detect_bdl_wires_params;

/**
 * @brief Binds detect bdl wires helpers.
 * @param m Python module.
 */
void register_detect_bdl_wires(nb::module_& m)
{
    // Keep the GIL for wire properties and mutation so calls on one wire remain serialized.
    using fiction::sidb::layout;
    using fiction::sidb::simulation::logic::bdl_wire;

    nb::enum_<fiction::sidb::simulation::logic::bdl_wire_selection>(
        m, "bdl_wire_selection", DOC(fiction_sidb_simulation_logic_bdl_wire_selection))
        .value("ALL", fiction::sidb::simulation::logic::bdl_wire_selection::ALL,
               DOC(fiction_sidb_simulation_logic_bdl_wire_selection_ALL))
        .value("INPUT", fiction::sidb::simulation::logic::bdl_wire_selection::INPUT,
               DOC(fiction_sidb_simulation_logic_bdl_wire_selection_INPUT))
        .value("OUTPUT", fiction::sidb::simulation::logic::bdl_wire_selection::OUTPUT,
               DOC(fiction_sidb_simulation_logic_bdl_wire_selection_OUTPUT))
        .export_values();

    auto wire = nb::class_<bdl_wire>(m, "bdl_wire", DOC(fiction_sidb_simulation_logic_bdl_wire));
    // The port belongs to the wire's Python API; importing fcn here would cycle through sidb.
    auto port =
        nb::class_<fiction::fcn::port_direction>(wire, "port_direction", "Direction and I/O flags of a BDL wire port.");
    nb::enum_<fiction::fcn::port_direction::cardinal>(port, "cardinal", "Cardinal port direction.")
        .value("NORTH", fiction::fcn::port_direction::NORTH)
        .value("NORTH_EAST", fiction::fcn::port_direction::NORTH_EAST)
        .value("EAST", fiction::fcn::port_direction::EAST)
        .value("SOUTH_EAST", fiction::fcn::port_direction::SOUTH_EAST)
        .value("SOUTH", fiction::fcn::port_direction::SOUTH)
        .value("SOUTH_WEST", fiction::fcn::port_direction::SOUTH_WEST)
        .value("WEST", fiction::fcn::port_direction::WEST)
        .value("NORTH_WEST", fiction::fcn::port_direction::NORTH_WEST)
        .value("NONE", fiction::fcn::port_direction::NONE);
    port.def(nb::init<fiction::fcn::port_direction::cardinal, bool, bool>(),
             nb::arg("direction") = fiction::fcn::port_direction::NORTH, nb::arg("pi") = false, nb::arg("po") = false,
             "Constructs a port with a cardinal direction and input/output flags.")
        .def_rw("dir", &fiction::fcn::port_direction::dir, "Cardinal direction as an integer.")
        .def_rw("pi", &fiction::fcn::port_direction::pi, "Whether the port is a primary input.")
        .def_rw("po", &fiction::fcn::port_direction::po, "Whether the port is a primary output.");
    wire.def(nb::init<>(), DOC(fiction_sidb_simulation_logic_bdl_wire_bdl_wire))
        .def(nb::init<std::vector<fiction::sidb::simulation::logic::bdl_pair>>(), nb::arg("p"),
             DOC(fiction_sidb_simulation_logic_bdl_wire_bdl_wire_2))
        .def_rw("pairs", &bdl_wire::pairs, DOC(fiction_sidb_simulation_logic_bdl_wire_pairs))
        .def_rw("direction", &bdl_wire::port, DOC(fiction_sidb_simulation_logic_bdl_wire_port))
        .def_rw("first_bdl_pair", &bdl_wire::first_bdl_pair, DOC(fiction_sidb_simulation_logic_bdl_wire_first_bdl_pair))
        .def_rw("last_bdl_pair", &bdl_wire::last_bdl_pair, DOC(fiction_sidb_simulation_logic_bdl_wire_last_bdl_pair))
        .def("add_bdl_pair", &bdl_wire::add_bdl_pair, nb::arg("pair"),
             DOC(fiction_sidb_simulation_logic_bdl_wire_add_bdl_pair))
        .def("erase_bdl_pair", &bdl_wire::erase_bdl_pair, nb::arg("pair"),
             DOC(fiction_sidb_simulation_logic_bdl_wire_erase_bdl_pair))
        .def("find_bdl_pair_by_type", &bdl_wire::find_bdl_pair_by_type, nb::arg("t"),
             DOC(fiction_sidb_simulation_logic_bdl_wire_find_bdl_pair_by_type))
        .def(
            "__eq__", [](const bdl_wire& lhs, const bdl_wire& rhs) { return lhs == rhs; },
            DOC(fiction_sidb_simulation_logic_bdl_wire_operator_eq))
        .def("__ne__", [](const bdl_wire& lhs, const bdl_wire& rhs) { return lhs != rhs; });

    m.def(
        "detect_bdl_wires",
        [](const layout&                                                                lyt,
           const decltype(detect_bdl_wires_options{}.threshold_bdl_interdistance)       threshold_bdl_interdistance,
           const decltype(detect_bdl_wires_options{}.bdl_pairs_params.minimum_distance) minimum_distance,
           const decltype(detect_bdl_wires_options{}.bdl_pairs_params.maximum_distance) maximum_distance,
           const fiction::sidb::simulation::logic::bdl_wire_selection                   wire_selection)
        {
            validate_bdl_pair_distances(minimum_distance, maximum_distance);
            detect_bdl_wires_options params{};
            params.threshold_bdl_interdistance       = threshold_bdl_interdistance;
            params.bdl_pairs_params.minimum_distance = minimum_distance;
            params.bdl_pairs_params.maximum_distance = maximum_distance;
            const nb::gil_scoped_release release{};
            return fiction::sidb::simulation::logic::detect_bdl_wires(lyt, params, wire_selection);
        },
        nb::arg("lyt"), nb::kw_only(),
        nb::arg("threshold_bdl_interdistance") = detect_bdl_wires_options{}.threshold_bdl_interdistance,
        nb::arg("minimum_distance")            = detect_bdl_wires_options{}.bdl_pairs_params.minimum_distance,
        nb::arg("maximum_distance")            = detect_bdl_wires_options{}.bdl_pairs_params.maximum_distance,
        nb::arg("wire_selection")              = fiction::sidb::simulation::logic::bdl_wire_selection::ALL,
        R"doc(Chain BDL pairs into wires along the SiDB lattice.
The keyword options select pair-distance bounds, inter-pair distance, and wire types.

Args:
    threshold_bdl_interdistance: Maximum separation between BDL pairs in one wire in nm.
    minimum_distance: Minimum SiDB separation for a BDL pair in nm.
    maximum_distance: Maximum SiDB separation for a BDL pair in nm.
    wire_selection: Select all wires, input wires, or output wires.
)doc");
}

}  // namespace pyfiction
