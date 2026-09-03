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

#include <fiction/technology/sidb/simulation/engines/quicksim.hpp>

#include <nanobind/nanobind.h>
#include <nanobind/stl/array.h>          // NOLINT(misc-include-cleaner)
#include <nanobind/stl/optional.h>       // NOLINT(misc-include-cleaner)
#include <nanobind/stl/pair.h>           // NOLINT(misc-include-cleaner)
#include <nanobind/stl/set.h>            // NOLINT(misc-include-cleaner)
#include <nanobind/stl/shared_ptr.h>     // NOLINT(misc-include-cleaner)
#include <nanobind/stl/unordered_map.h>  // NOLINT(misc-include-cleaner)
#include <nanobind/stl/vector.h>         // NOLINT(misc-include-cleaner)

namespace pyfiction
{

namespace detail
{

template <typename Lyt>
void quicksim_impl(nanobind::module_& m)
{
    namespace py = nanobind;  // NOLINT(misc-unused-alias-decls)

    m.def("quicksim", &fiction::sidb::simulation::engines::quicksim<Lyt>, py::arg("lyt"),
          py::arg("params") = fiction::sidb::simulation::engines::quicksim_params{},
          DOC(fiction_sidb_simulation_engines_quicksim));
}

}  // namespace detail

void quicksim(nanobind::module_& m)
{
    namespace py = nanobind;  // NOLINT(misc-unused-alias-decls)

    /**
     * QuickSim parameters.
     */
    py::class_<fiction::sidb::simulation::engines::quicksim_params>(
        m, "quicksim_params", DOC(fiction_sidb_simulation_engines_quicksim_params))
        .def(py::init<>(), "Default constructor.")
        .def_rw("simulation_parameters", &fiction::sidb::simulation::engines::quicksim_params::sim_params,
                DOC(fiction_sidb_simulation_engines_quicksim_params_sim_params))
        .def_rw("iteration_steps", &fiction::sidb::simulation::engines::quicksim_params::iteration_steps,
                DOC(fiction_sidb_simulation_engines_quicksim_params_iteration_steps))
        .def_rw("alpha", &fiction::sidb::simulation::engines::quicksim_params::alpha,
                DOC(fiction_sidb_simulation_engines_quicksim_params_alpha))
        .def_rw("number_threads", &fiction::sidb::simulation::engines::quicksim_params::number_threads,
                DOC(fiction_sidb_simulation_engines_quicksim_params_number_threads))
        .def_rw("timeout", &fiction::sidb::simulation::engines::quicksim_params::timeout,
                DOC(fiction_sidb_simulation_engines_quicksim_params_timeout))

        ;

    // NOTE be careful with the order of the following calls! Python will resolve the first matching overload!
    detail::quicksim_impl<py_sidb_100_lattice>(m);
    detail::quicksim_impl<py_sidb_111_lattice>(m);
}

}  // namespace pyfiction
