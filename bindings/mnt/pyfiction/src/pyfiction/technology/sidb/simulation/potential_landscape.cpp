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
 * @brief Python bindings for `fiction/technology/sidb/simulation/potential_landscape.hpp`.
 * @author Marcel Walter (marcelwa)
 */

#include "pyfiction/documentation.hpp"

#include <fiction/technology/sidb/charge_distribution.hpp>
#include <fiction/technology/sidb/lattice.hpp>
#include <fiction/technology/sidb/layout.hpp>
#include <fiction/technology/sidb/model/simulation_parameters.hpp>
#include <fiction/technology/sidb/simulation/potential_landscape.hpp>

#include <unordered_map>
#include <vector>

#include <nanobind/nanobind.h>
#include <nanobind/stl/array.h>          // NOLINT(misc-include-cleaner)
#include <nanobind/stl/pair.h>           // NOLINT(misc-include-cleaner)
#include <nanobind/stl/unordered_map.h>  // NOLINT(misc-include-cleaner)
#include <nanobind/stl/vector.h>         // NOLINT(misc-include-cleaner)

namespace pyfiction
{

void potential_landscape(nanobind::module_& m)
{
    namespace py = nanobind;

    using fiction::sidb::simulation::potential_landscape;

    py::enum_<fiction::sidb::simulation::charge_transition_threshold_bounds>(
        m, "charge_transition_threshold_bounds", DOC(fiction_sidb_simulation_charge_transition_threshold_bounds))
        .value("NEGATIVE_UPPER_BOUND",
               fiction::sidb::simulation::charge_transition_threshold_bounds::NEGATIVE_UPPER_BOUND,
               DOC(fiction_sidb_simulation_charge_transition_threshold_bounds_NEGATIVE_UPPER_BOUND))
        .value("POSITIVE_LOWER_BOUND",
               fiction::sidb::simulation::charge_transition_threshold_bounds::POSITIVE_LOWER_BOUND,
               DOC(fiction_sidb_simulation_charge_transition_threshold_bounds_POSITIVE_LOWER_BOUND))
        .value("NEUTRAL_LOWER_BOUND",
               fiction::sidb::simulation::charge_transition_threshold_bounds::NEUTRAL_LOWER_BOUND,
               DOC(fiction_sidb_simulation_charge_transition_threshold_bounds_NEUTRAL_LOWER_BOUND))
        .value("NEUTRAL_UPPER_BOUND",
               fiction::sidb::simulation::charge_transition_threshold_bounds::NEUTRAL_UPPER_BOUND,
               DOC(fiction_sidb_simulation_charge_transition_threshold_bounds_NEUTRAL_UPPER_BOUND));

    py::class_<potential_landscape>(m, "potential_landscape", DOC(fiction_sidb_simulation_potential_landscape))
        .def(py::init<const fiction::sidb::layout&, const fiction::sidb::model::simulation_parameters&,
                      const std::unordered_map<fiction::sidb::lattice_site, double>&, double>(),
             py::arg("layout"), py::arg("params") = fiction::sidb::model::simulation_parameters{},
             py::arg("local_external_potential")  = std::unordered_map<fiction::sidb::lattice_site, double>{},
             py::arg("global_external_potential") = 0.0,
             DOC(fiction_sidb_simulation_potential_landscape_potential_landscape))
        .def("get_layout", &potential_landscape::get_layout,
             DOC(fiction_sidb_simulation_potential_landscape_get_layout))
        .def("params", &potential_landscape::params, DOC(fiction_sidb_simulation_potential_landscape_params))
        .def("num_sidbs", &potential_landscape::num_sidbs, DOC(fiction_sidb_simulation_potential_landscape_num_sidbs))
        .def(
            "sites", [](const potential_landscape& land) { return *land.sites(); },
            DOC(fiction_sidb_simulation_potential_landscape_sites))
        .def("defects", &potential_landscape::defects, DOC(fiction_sidb_simulation_potential_landscape_defects))
        .def("nm_distance", &potential_landscape::nm_distance, py::arg("i"), py::arg("j"),
             DOC(fiction_sidb_simulation_potential_landscape_nm_distance))
        .def("chargeless_potential", &potential_landscape::chargeless_potential, py::arg("i"), py::arg("j"),
             DOC(fiction_sidb_simulation_potential_landscape_chargeless_potential))
        .def("local_external_potential", &potential_landscape::local_external_potential, py::arg("i"),
             DOC(fiction_sidb_simulation_potential_landscape_local_external_potential))
        .def("local_potential_caused_by_defects", &potential_landscape::local_potential_caused_by_defects, py::arg("i"),
             DOC(fiction_sidb_simulation_potential_landscape_local_potential_caused_by_defects))
        .def("effective_charge_transition_thresholds", &potential_landscape::effective_charge_transition_thresholds,
             py::arg("i"), DOC(fiction_sidb_simulation_potential_landscape_effective_charge_transition_thresholds))
        .def("chargeless_potential_at_distance", &potential_landscape::chargeless_potential_at_distance,
             py::arg("distance"), DOC(fiction_sidb_simulation_potential_landscape_chargeless_potential_at_distance))
        .def("local_internal_potentials", &potential_landscape::local_internal_potentials, py::arg("cd"),
             DOC(fiction_sidb_simulation_potential_landscape_local_internal_potentials))
        .def("local_potentials", &potential_landscape::local_potentials, py::arg("cd"),
             DOC(fiction_sidb_simulation_potential_landscape_local_potentials))
        .def(
            "energy", [](const potential_landscape& land, const fiction::sidb::charge_distribution& cd)
            { return land.energy(cd); }, py::arg("cd"), DOC(fiction_sidb_simulation_potential_landscape_energy_2))
        .def(
            "is_physically_valid", [](const potential_landscape& land, const fiction::sidb::charge_distribution& cd)
            { return land.is_physically_valid(cd); }, py::arg("cd"),
            DOC(fiction_sidb_simulation_potential_landscape_is_physically_valid_2))
        .def("evaluate", &potential_landscape::evaluate, py::arg("cd"),
             DOC(fiction_sidb_simulation_potential_landscape_evaluate))

        ;
}

}  // namespace pyfiction
