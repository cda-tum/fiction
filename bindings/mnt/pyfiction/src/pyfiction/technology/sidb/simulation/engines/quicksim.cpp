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
 * @brief Python bindings for `fiction/technology/sidb/simulation/engines/quicksim.hpp`.
 * @author Marcel Walter (marcelwa)
 * @author Jan Drewniok (Drewniok)
 */

#include "pyfiction/documentation.hpp"
#include "pyfiction/types.hpp"

#include <fiction/technology/sidb/layout.hpp>
#include <fiction/technology/sidb/simulation/engines/quicksim.hpp>
#include <fiction/technology/sidb/simulation/result.hpp>

#include <optional>

#include <nanobind/nanobind.h>
#include <nanobind/stl/optional.h>       // NOLINT(misc-include-cleaner)
#include <nanobind/stl/unordered_map.h>  // NOLINT(misc-include-cleaner)
#include <nanobind/stl/vector.h>         // NOLINT(misc-include-cleaner)

namespace pyfiction
{

void quicksim(nanobind::module_& m)
{
    namespace py = nanobind;

    using fiction::sidb::simulation::engines::quicksim_params;

    py::class_<quicksim_params>(m, "quicksim_params", DOC(fiction_sidb_simulation_engines_quicksim_params))
        .def(py::init<>(), "Default constructor.")
        .def_rw("simulation_parameters", &quicksim_params::sim_params,
                DOC(fiction_sidb_simulation_engines_quicksim_params_sim_params))
        .def_rw("iteration_steps", &quicksim_params::iteration_steps,
                DOC(fiction_sidb_simulation_engines_quicksim_params_iteration_steps))
        .def_rw("alpha", &quicksim_params::alpha, DOC(fiction_sidb_simulation_engines_quicksim_params_alpha))
        .def_rw("number_threads", &quicksim_params::number_threads,
                DOC(fiction_sidb_simulation_engines_quicksim_params_number_threads))
        .def_rw("timeout", &quicksim_params::timeout, DOC(fiction_sidb_simulation_engines_quicksim_params_timeout))

        ;

    // NOLINTNEXTLINE(misc-const-correctness)
    std::optional<fiction::sidb::simulation::result> (*const quicksim_pointer)(
        const fiction::sidb::layout&, const quicksim_params&) = &fiction::sidb::simulation::engines::quicksim;

    m.def("quicksim", quicksim_pointer, py::arg("lyt"), py::arg("params") = quicksim_params{},
          DOC(fiction_sidb_simulation_engines_quicksim));

    // transitional overloads over the Cartesian SiDB cell-level layouts that return `legacy_result`; they go away
    // once every consumer takes `sidb_layout`. NOTE: registered after the `sidb_layout` overload, since Python
    // resolves the first matching one.
    m.def("quicksim", &fiction::sidb::simulation::engines::quicksim<py_sidb_100_lattice>, py::arg("lyt"),
          py::arg("params") = quicksim_params{}, DOC(fiction_sidb_simulation_engines_quicksim_2));
    m.def("quicksim", &fiction::sidb::simulation::engines::quicksim<py_sidb_111_lattice>, py::arg("lyt"),
          py::arg("params") = quicksim_params{}, DOC(fiction_sidb_simulation_engines_quicksim_2));
}

}  // namespace pyfiction
