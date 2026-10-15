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

#include "pyfiction/bdl_parameters.hpp"
#include "pyfiction/documentation.hpp"

#include <fiction/technology/sidb/lattice.hpp>
#include <fiction/technology/sidb/layout.hpp>
#include <fiction/technology/sidb/simulation/logic/detect_bdl_pairs.hpp>

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

/**
 * @brief Namespace alias for nanobind.
 */
namespace nb = nanobind;

namespace pyfiction
{

/** @brief Native options constructed for each Python call. */
using detect_bdl_pairs_options = fiction::sidb::simulation::logic::detect_bdl_pairs_params;

/**
 * @brief Binds detect bdl pairs helpers.
 * @param m Python module.
 */
void register_detect_bdl_pairs(nb::module_& m)
{
    using fiction::sidb::lattice_site;
    using fiction::sidb::layout;
    using bdl_pair_t = fiction::sidb::simulation::logic::bdl_pair;

    nb::class_<bdl_pair_t>(m, "bdl_pair", DOC(fiction_sidb_simulation_logic_bdl_pair))
        .def(nb::init<>(), DOC(fiction_sidb_simulation_logic_bdl_pair_bdl_pair))
        .def(nb::init<fiction::sidb::dot_tag, lattice_site, lattice_site>(), nb::arg("t"), nb::arg("u"), nb::arg("l"),
             DOC(fiction_sidb_simulation_logic_bdl_pair_bdl_pair_2))
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

    m.def(
        "detect_bdl_pairs",
        [](const layout& lyt, const std::optional<fiction::sidb::dot_tag>& type,
           const decltype(detect_bdl_pairs_options{}.minimum_distance) minimum_distance,
           const decltype(detect_bdl_pairs_options{}.maximum_distance) maximum_distance)
        {
            validate_bdl_pair_distances(minimum_distance, maximum_distance);
            detect_bdl_pairs_options params{};
            params.minimum_distance = minimum_distance;
            params.maximum_distance = maximum_distance;
            return fiction::sidb::simulation::logic::detect_bdl_pairs(lyt, type, params);
        },
        nb::arg("lyt"), nb::arg("type") = std::nullopt, nb::kw_only(),
        nb::arg("minimum_distance") = detect_bdl_pairs_options{}.minimum_distance,
        nb::arg("maximum_distance") = detect_bdl_pairs_options{}.maximum_distance,
        R"doc(Detect uniquely paired SiDBs with the selected tag in increasing distance.
The keyword minimum_distance and maximum_distance bound the pair separation in nm.

Args:
    minimum_distance: Minimum SiDB separation for a BDL pair in nm.
    maximum_distance: Maximum SiDB separation for a BDL pair in nm.
)doc");
}

}  // namespace pyfiction
