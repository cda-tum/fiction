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
 * @brief Python bindings for `fiction/technology/sidb/simulation/logic/detect_bdl_pairs.hpp`.
 * @author Marcel Walter (marcelwa)
 * @author Jan Drewniok (Drewniok)
 */

#include "pyfiction/documentation.hpp"

#include <fiction/technology/sidb/lattice.hpp>
#include <fiction/technology/sidb/layout.hpp>
#include <fiction/technology/sidb/simulation/logic/detect_bdl_pairs.hpp>
#include <fiction/technology/sidb/technology.hpp>

#include <functional>
#include <optional>

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

void detect_bdl_pairs(nanobind::module_& m)
{
    namespace py = nanobind;

    using fiction::sidb::lattice_site;
    using fiction::sidb::layout;
    using bdl_pair_t = fiction::sidb::simulation::logic::bdl_pair<lattice_site>;

    py::class_<bdl_pair_t>(m, "bdl_pair", DOC(fiction_sidb_simulation_logic_bdl_pair))
        .def(py::init<>(), DOC(fiction_sidb_simulation_logic_bdl_pair_bdl_pair))
        .def(py::init<fiction::sidb::sidb_technology::cell_type, lattice_site, lattice_site>(), py::arg("t"),
             py::arg("u"), py::arg("l"), DOC(fiction_sidb_simulation_logic_bdl_pair_bdl_pair_2))
        .def_ro("type", &bdl_pair_t::type, DOC(fiction_sidb_simulation_logic_bdl_pair_type))
        .def_ro("upper", &bdl_pair_t::upper, DOC(fiction_sidb_simulation_logic_bdl_pair_upper))
        .def_ro("lower", &bdl_pair_t::lower, DOC(fiction_sidb_simulation_logic_bdl_pair_lower))
        .def(
            "__eq__", [](const bdl_pair_t& lhs, const bdl_pair_t& rhs) { return lhs == rhs; },
            DOC(fiction_sidb_simulation_logic_bdl_pair_operator_eq))
        .def("__ne__", [](const bdl_pair_t& lhs, const bdl_pair_t& rhs) { return lhs != rhs; })
        .def(
            "__lt__", [](const bdl_pair_t& lhs, const bdl_pair_t& rhs) { return lhs < rhs; },
            DOC(fiction_sidb_simulation_logic_bdl_pair_operator_lt))
        .def("__hash__", [](const bdl_pair_t& p)
             { return std::hash<lattice_site>{}(p.upper) ^ (std::hash<lattice_site>{}(p.lower) << 1U); })
        .def("__repr__", [](const bdl_pair_t& p) { return "bdl_pair(" + p.upper.str() + ", " + p.lower.str() + ")"; });

    py::class_<fiction::sidb::simulation::logic::detect_bdl_pairs_params>(
        m, "detect_bdl_pairs_params", DOC(fiction_sidb_simulation_logic_detect_bdl_pairs_params))
        .def(py::init<>(), "Default constructor.")
        .def_rw("minimum_distance", &fiction::sidb::simulation::logic::detect_bdl_pairs_params::minimum_distance,
                DOC(fiction_sidb_simulation_logic_detect_bdl_pairs_params_minimum_distance))
        .def_rw("maximum_distance", &fiction::sidb::simulation::logic::detect_bdl_pairs_params::maximum_distance,
                DOC(fiction_sidb_simulation_logic_detect_bdl_pairs_params_maximum_distance));

    m.def(
        "detect_bdl_pairs",
        [](const layout& lyt, const std::optional<fiction::sidb::sidb_technology::cell_type>& type,
           const fiction::sidb::simulation::logic::detect_bdl_pairs_params& params)
        { return fiction::sidb::simulation::logic::detect_bdl_pairs(lyt, type, params); },
        py::arg("lyt"), py::arg("type") = std::nullopt,
        py::arg("params") = fiction::sidb::simulation::logic::detect_bdl_pairs_params{},
        DOC(fiction_sidb_simulation_logic_detect_bdl_pairs));
}

}  // namespace pyfiction
