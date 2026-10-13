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
 * @brief Python bindings for `fiction/technology/sidb/simulation/analysis/energy_distribution.hpp`.
 * @author Marcel Walter (marcelwa)
 * @author Jan Drewniok (Drewniok)
 */

#include "pyfiction/documentation.hpp"

#include <fiction/technology/sidb/charge_distribution.hpp>
#include <fiction/technology/sidb/simulation/analysis/energy_distribution.hpp>

#include <cstdint>
#include <vector>

#include <nanobind/nanobind.h>
#include <nanobind/stl/array.h>          // NOLINT(misc-include-cleaner)
#include <nanobind/stl/map.h>            // NOLINT(misc-include-cleaner)
#include <nanobind/stl/optional.h>       // NOLINT(misc-include-cleaner)
#include <nanobind/stl/pair.h>           // NOLINT(misc-include-cleaner)
#include <nanobind/stl/set.h>            // NOLINT(misc-include-cleaner)
#include <nanobind/stl/shared_ptr.h>     // NOLINT(misc-include-cleaner)
#include <nanobind/stl/unordered_map.h>  // NOLINT(misc-include-cleaner)
#include <nanobind/stl/vector.h>         // NOLINT(misc-include-cleaner)

/**
 * @brief Namespace alias for nanobind.
 */
namespace nb = nanobind;

namespace pyfiction
{

/**
 * @brief Binds `energy_distribution`.
 * @param m Python module.
 */
void bind_energy_distribution(nb::module_& m)
{
    nb::class_<fiction::sidb::simulation::analysis::energy_state>(m, "energy_state")
        .def(nb::init<double, uint64_t>(), nb::arg("electrostatic_potential_energy"), nb::arg("degeneracy"),
             DOC(fiction_sidb_simulation_analysis_energy_state))
        .def_rw("electrostatic_potential_energy",
                &fiction::sidb::simulation::analysis::energy_state::electrostatic_potential_energy,
                DOC(fiction_sidb_simulation_analysis_energy_state_electrostatic_potential_energy))
        .def_rw("degeneracy", &fiction::sidb::simulation::analysis::energy_state::degeneracy,
                DOC(fiction_sidb_simulation_analysis_energy_state_degeneracy));

    nb::class_<fiction::sidb::simulation::analysis::energy_distribution>(m, "energy_distribution")
        .def(nb::init<>(), "Default constructor.")
        .def("get_nth_state", &fiction::sidb::simulation::analysis::energy_distribution::get_nth_state,
             nb::arg("state_index"), DOC(fiction_sidb_simulation_analysis_energy_distribution_get_nth_state))
        .def("degeneracy", &fiction::sidb::simulation::analysis::energy_distribution::degeneracy, nb::arg("energy"),
             DOC(fiction_sidb_simulation_analysis_energy_distribution_degeneracy))
        .def("add_energy_state", &fiction::sidb::simulation::analysis::energy_distribution::add_energy_state,
             nb::arg("state"), DOC(fiction_sidb_simulation_analysis_energy_distribution_add_energy_state))
        .def("size", &fiction::sidb::simulation::analysis::energy_distribution::size,
             DOC(fiction_sidb_simulation_analysis_energy_distribution_size))
        .def("empty", &fiction::sidb::simulation::analysis::energy_distribution::empty,
             DOC(fiction_sidb_simulation_analysis_energy_distribution_empty))
        .def("max_energy", &fiction::sidb::simulation::analysis::energy_distribution::max_energy,
             DOC(fiction_sidb_simulation_analysis_energy_distribution_max_energy))
        .def("min_energy", &fiction::sidb::simulation::analysis::energy_distribution::min_energy,
             DOC(fiction_sidb_simulation_analysis_energy_distribution_min_energy))

        ;

    // NOLINTNEXTLINE(misc-const-correctness)
    fiction::sidb::simulation::analysis::energy_distribution (*const energy_distribution_pointer)(
        const std::vector<fiction::sidb::charge_distribution>&) =
        &fiction::sidb::simulation::analysis::calculate_energy_distribution;

    m.def("calculate_energy_distribution", energy_distribution_pointer, nb::arg("charge_distributions"),
          DOC(fiction_sidb_simulation_analysis_calculate_energy_distribution));
}

}  // namespace pyfiction
