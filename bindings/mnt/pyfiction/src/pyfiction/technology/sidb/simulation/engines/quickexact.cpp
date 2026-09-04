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
 * @brief Python bindings for `fiction/technology/sidb/simulation/engines/quickexact.hpp`.
 * @author Marcel Walter (marcelwa)
 * @author Willem Lambooy (wlambooy)
 */

#include "pyfiction/documentation.hpp"
#include "pyfiction/types.hpp"

#include <fiction/technology/sidb/layout.hpp>
#include <fiction/technology/sidb/simulation/engines/quickexact.hpp>
#include <fiction/technology/sidb/simulation/result.hpp>

#include <nanobind/nanobind.h>
// These headers register nanobind type casters without exposing directly referenced symbols.
#include <nanobind/stl/optional.h>       // NOLINT(misc-include-cleaner)
#include <nanobind/stl/unordered_map.h>  // NOLINT(misc-include-cleaner)
#include <nanobind/stl/vector.h>         // NOLINT(misc-include-cleaner)

namespace pyfiction
{

void quickexact(nanobind::module_& m)
{
    namespace py = nanobind;

    using fiction::sidb::simulation::engines::quickexact_params;

    py::enum_<quickexact_params::automatic_base_number_detection>(
        m, "automatic_base_number_detection",
        DOC(fiction_sidb_simulation_engines_quickexact_params_automatic_base_number_detection))
        .value("ON", quickexact_params::automatic_base_number_detection::ON,
               DOC(fiction_sidb_simulation_engines_quickexact_params_automatic_base_number_detection_ON))
        .value("OFF", quickexact_params::automatic_base_number_detection::OFF,
               DOC(fiction_sidb_simulation_engines_quickexact_params_automatic_base_number_detection_OFF));

    py::class_<quickexact_params>(m, "quickexact_params", DOC(fiction_sidb_simulation_engines_quickexact_params))
        .def(py::init<>(), "Default constructor.")
        .def_rw("simulation_parameters", &quickexact_params::sim_params,
                DOC(fiction_sidb_simulation_engines_quickexact_params_sim_params))
        .def_rw("base_number_detection", &quickexact_params::base_number_detection,
                DOC(fiction_sidb_simulation_engines_quickexact_params_base_number_detection))
        .def_rw("local_external_potential", &quickexact_params::local_external_potential,
                DOC(fiction_sidb_simulation_engines_quickexact_params_local_external_potential))
        .def_rw("global_potential", &quickexact_params::global_potential,
                DOC(fiction_sidb_simulation_engines_quickexact_params_global_potential));

    // NOLINTNEXTLINE(misc-const-correctness)
    fiction::sidb::simulation::result (*const quickexact_pointer)(
        const fiction::sidb::layout&, const quickexact_params&) = &fiction::sidb::simulation::engines::quickexact;

    m.def("quickexact", quickexact_pointer, py::arg("lyt"), py::arg("params") = quickexact_params{},
          DOC(fiction_sidb_simulation_engines_quickexact));

    // transitional overloads over the Cartesian SiDB cell-level layouts that return `legacy_result`; they go away
    // once every consumer takes `sidb_layout`. NOTE: registered after the `sidb_layout` overload, since Python
    // resolves the first matching one.
    m.def("quickexact", &fiction::sidb::simulation::engines::quickexact<py_sidb_100_lattice>, py::arg("lyt"),
          py::arg("params") = quickexact_params{}, DOC(fiction_sidb_simulation_engines_quickexact_2));
    m.def("quickexact", &fiction::sidb::simulation::engines::quickexact<py_sidb_111_lattice>, py::arg("lyt"),
          py::arg("params") = quickexact_params{}, DOC(fiction_sidb_simulation_engines_quickexact_2));
}

}  // namespace pyfiction
