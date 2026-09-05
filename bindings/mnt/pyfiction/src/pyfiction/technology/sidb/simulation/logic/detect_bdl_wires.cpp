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

#include "pyfiction/documentation.hpp"

#include <fiction/technology/sidb/lattice.hpp>
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

namespace pyfiction
{

void detect_bdl_wires(nanobind::module_& m)
{
    namespace py = nanobind;

    using fiction::sidb::lattice_site;
    using fiction::sidb::layout;
    using fiction::sidb::simulation::logic::bdl_wire;

    py::enum_<fiction::sidb::simulation::logic::bdl_wire_selection>(
        m, "bdl_wire_selection", DOC(fiction_sidb_simulation_logic_bdl_wire_selection))
        .value("ALL", fiction::sidb::simulation::logic::bdl_wire_selection::ALL,
               DOC(fiction_sidb_simulation_logic_bdl_wire_selection_ALL))
        .value("INPUT", fiction::sidb::simulation::logic::bdl_wire_selection::INPUT,
               DOC(fiction_sidb_simulation_logic_bdl_wire_selection_INPUT))
        .value("OUTPUT", fiction::sidb::simulation::logic::bdl_wire_selection::OUTPUT,
               DOC(fiction_sidb_simulation_logic_bdl_wire_selection_OUTPUT))
        .export_values();

    py::class_<fiction::sidb::simulation::logic::detect_bdl_wires_params>(
        m, "detect_bdl_wires_params", DOC(fiction_sidb_simulation_logic_detect_bdl_wires_params))
        .def(py::init<>(), DOC(fiction_sidb_simulation_logic_detect_bdl_wires_params))
        .def_rw("threshold_bdl_interdistance",
                &fiction::sidb::simulation::logic::detect_bdl_wires_params::threshold_bdl_interdistance,
                DOC(fiction_sidb_simulation_logic_detect_bdl_wires_params_threshold_bdl_interdistance))
        .def_rw("bdl_pairs_params", &fiction::sidb::simulation::logic::detect_bdl_wires_params::bdl_pairs_params,
                DOC(fiction_sidb_simulation_logic_detect_bdl_wires_params_bdl_pairs_params));

    py::class_<bdl_wire>(m, "bdl_wire", DOC(fiction_sidb_simulation_logic_bdl_wire))
        .def(py::init<>(), DOC(fiction_sidb_simulation_logic_bdl_wire_bdl_wire))
        .def(py::init<std::vector<fiction::sidb::simulation::logic::bdl_pair<lattice_site>>>(), py::arg("p"),
             DOC(fiction_sidb_simulation_logic_bdl_wire_bdl_wire_2))
        .def_rw("pairs", &bdl_wire::pairs, DOC(fiction_sidb_simulation_logic_bdl_wire_pairs))
        .def_rw("direction", &bdl_wire::port, DOC(fiction_sidb_simulation_logic_bdl_wire_port))
        .def_rw("first_bdl_pair", &bdl_wire::first_bdl_pair, DOC(fiction_sidb_simulation_logic_bdl_wire_first_bdl_pair))
        .def_rw("last_bdl_pair", &bdl_wire::last_bdl_pair, DOC(fiction_sidb_simulation_logic_bdl_wire_last_bdl_pair))
        .def("add_bdl_pair", &bdl_wire::add_bdl_pair, py::arg("pair"),
             DOC(fiction_sidb_simulation_logic_bdl_wire_add_bdl_pair))
        .def("erase_bdl_pair", &bdl_wire::erase_bdl_pair, py::arg("pair"),
             DOC(fiction_sidb_simulation_logic_bdl_wire_erase_bdl_pair))
        .def("find_bdl_pair_by_type", &bdl_wire::find_bdl_pair_by_type, py::arg("t"),
             DOC(fiction_sidb_simulation_logic_bdl_wire_find_bdl_pair_by_type))
        .def(
            "__eq__", [](const bdl_wire& lhs, const bdl_wire& rhs) { return lhs == rhs; },
            DOC(fiction_sidb_simulation_logic_bdl_wire_operator_eq))
        .def("__ne__", [](const bdl_wire& lhs, const bdl_wire& rhs) { return lhs != rhs; });

    m.def(
        "detect_bdl_wires",
        [](const layout& lyt, const fiction::sidb::simulation::logic::detect_bdl_wires_params& params,
           const fiction::sidb::simulation::logic::bdl_wire_selection wire_selection)
        { return fiction::sidb::simulation::logic::detect_bdl_wires(lyt, params, wire_selection); },
        py::arg("lyt"), py::arg("params") = fiction::sidb::simulation::logic::detect_bdl_wires_params{},
        py::arg("wire_selection") = fiction::sidb::simulation::logic::bdl_wire_selection::ALL,
        DOC(fiction_sidb_simulation_logic_detect_bdl_wires));
}

}  // namespace pyfiction
