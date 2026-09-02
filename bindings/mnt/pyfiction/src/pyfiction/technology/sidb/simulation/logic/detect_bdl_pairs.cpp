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
 * @author Marcel Walter (marcelwa)
 * @author Jan Drewniok (Drewniok)
 */

#include "pyfiction/documentation.hpp"
#include "pyfiction/types.hpp"

#include <fiction/layouts/coordinates.hpp>
#include <fiction/technology/sidb/simulation/logic/detect_bdl_pairs.hpp>
#include <fiction/technology/sidb/technology.hpp>

#include <fmt/format.h>

#include <optional>

#include <nanobind/nanobind.h>
#include <nanobind/stl/optional.h>       // NOLINT(misc-include-cleaner)
#include <nanobind/stl/pair.h>           // NOLINT(misc-include-cleaner)
#include <nanobind/stl/unordered_set.h>  // NOLINT(misc-include-cleaner)
#include <nanobind/stl/vector.h>         // NOLINT(misc-include-cleaner)

namespace pyfiction
{

namespace detail
{

template <typename Lyt>
void detect_bdl_pairs_impl(nanobind::module_& m)
{
    namespace py = nanobind;  // NOLINT(misc-unused-alias-decls)

    m.def("detect_bdl_pairs", &fiction::sidb::simulation::logic::detect_bdl_pairs<Lyt>, py::arg("lyt"),
          py::arg("type")   = std::nullopt,
          py::arg("params") = fiction::sidb::simulation::logic::detect_bdl_pairs_params{},
          DOC(fiction_sidb_simulation_logic_detect_bdl_pairs));
}

}  // namespace detail

void detect_bdl_pairs(nanobind::module_& m)
{
    namespace py = nanobind;

    py::class_<fiction::sidb::simulation::logic::bdl_pair<fiction::layouts::coords::offset>>(
        m, "bdl_pair", DOC(fiction_sidb_simulation_logic_bdl_pair))
        .def(py::init<>(), DOC(fiction_sidb_simulation_logic_bdl_pair_bdl_pair))
        .def(py::init<fiction::sidb::sidb_technology::cell_type, fiction::layouts::coords::offset,
                      fiction::layouts::coords::offset>(),
             py::arg("t"), py::arg("u"), py::arg("l"), DOC(fiction_sidb_simulation_logic_bdl_pair_bdl_pair_2))
        .def_ro("type", &fiction::sidb::simulation::logic::bdl_pair<fiction::layouts::coords::offset>::type,
                DOC(fiction_sidb_simulation_logic_bdl_pair_type))
        .def_ro("upper", &fiction::sidb::simulation::logic::bdl_pair<fiction::layouts::coords::offset>::upper,
                DOC(fiction_sidb_simulation_logic_bdl_pair_upper))
        .def_ro("lower", &fiction::sidb::simulation::logic::bdl_pair<fiction::layouts::coords::offset>::lower,
                DOC(fiction_sidb_simulation_logic_bdl_pair_lower));

    py::class_<fiction::sidb::simulation::logic::detect_bdl_pairs_params>(
        m, "detect_bdl_pairs_params", DOC(fiction_sidb_simulation_logic_detect_bdl_pairs_params))
        .def(py::init<>(), "Default constructor.")
        .def_rw("minimum_distance", &fiction::sidb::simulation::logic::detect_bdl_pairs_params::minimum_distance,
                DOC(fiction_sidb_simulation_logic_detect_bdl_pairs_params_minimum_distance))
        .def_rw("maximum_distance", &fiction::sidb::simulation::logic::detect_bdl_pairs_params::maximum_distance,
                DOC(fiction_sidb_simulation_logic_detect_bdl_pairs_params_maximum_distance))

        ;

    // NOTE be careful with the order of the following calls! Python will resolve the first matching overload!

    detail::detect_bdl_pairs_impl<py_sidb_100_lattice>(m);
    detail::detect_bdl_pairs_impl<py_sidb_111_lattice>(m);
}

}  // namespace pyfiction
