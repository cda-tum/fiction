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
#include "pyfiction/simulation_parameters.hpp"

#include <fiction/technology/sidb/charge_distribution.hpp>
#include <fiction/technology/sidb/lattice.hpp>
#include <fiction/technology/sidb/layout.hpp>
#include <fiction/technology/sidb/simulation/potential_landscape.hpp>

#include <memory>
#include <optional>
#include <unordered_map>
#include <vector>

#include <nanobind/nanobind.h>
#include <nanobind/stl/array.h>          // NOLINT(misc-include-cleaner)
#include <nanobind/stl/optional.h>       // NOLINT(misc-include-cleaner): enables optional potentials
#include <nanobind/stl/pair.h>           // NOLINT(misc-include-cleaner)
#include <nanobind/stl/unordered_map.h>  // NOLINT(misc-include-cleaner)
#include <nanobind/stl/vector.h>         // NOLINT(misc-include-cleaner)

/**
 * @brief Namespace alias for nanobind.
 */
namespace nb = nanobind;

namespace pyfiction
{

/**
 * @brief Registers SiDB potential landscapes and charge-transition bounds.
 *
 * @param m Python module.
 */
void register_potential_landscape(nb::module_& m)
{
    using fiction::sidb::simulation::potential_landscape;

    nb::enum_<fiction::sidb::simulation::charge_transition_threshold_bounds>(
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

    nb::class_<potential_landscape>(m, "potential_landscape", DOC(fiction_sidb_simulation_potential_landscape))
        .def(
            "__init__",
            [](potential_landscape* self, const fiction::sidb::layout& layout,
               const decltype(simulation_model{}.base) base, const decltype(simulation_model{}.mu_minus) mu_minus,
               const decltype(simulation_model{}.epsilon_r)                                  epsilon_r,
               const decltype(simulation_model{}.lambda_tf)                                  lambda_tf,
               const std::optional<std::unordered_map<fiction::sidb::lattice_site, double>>& local_external_potential,
               double                                                                        global_external_potential)
            {
                const auto params = make_simulation_parameters(base, mu_minus, epsilon_r, lambda_tf);
                std::construct_at(
                    self, layout, params,
                    local_external_potential.value_or(std::unordered_map<fiction::sidb::lattice_site, double>{}),
                    global_external_potential);
            },
            nb::arg("layout"), nb::kw_only(), nb::arg("base") = simulation_model{}.base,
            nb::arg("mu_minus") = simulation_model{}.mu_minus, nb::arg("epsilon_r") = simulation_model{}.epsilon_r,
            nb::arg("lambda_tf") = simulation_model{}.lambda_tf, nb::arg("local_external_potential") = nb::none(),
            nb::arg("global_external_potential") = 0.0,
            R"doc(Construct the electrostatic potential landscape for the SiDB layout.

Args:
    base: Number of charge states: 2 for negative and neutral, or 3 to include positive charges.
    mu_minus: Negative-charge transition energy in eV.
    epsilon_r: Relative permittivity (dimensionless).
    lambda_tf: Thomas-Fermi screening distance in nm.
    local_external_potential: External electrostatic potential per lattice site in V; None uses an empty map.
    global_external_potential: External electrostatic potential applied to every SiDB in V.
)doc")
        .def("get_layout", &potential_landscape::get_layout,
             DOC(fiction_sidb_simulation_potential_landscape_get_layout))
        .def("params", &potential_landscape::params, DOC(fiction_sidb_simulation_potential_landscape_params))
        .def("num_sidbs", &potential_landscape::num_sidbs, DOC(fiction_sidb_simulation_potential_landscape_num_sidbs))
        .def(
            "sites", [](const potential_landscape& land) { return *land.sites(); },
            DOC(fiction_sidb_simulation_potential_landscape_sites))
        .def("defects", &potential_landscape::defects, DOC(fiction_sidb_simulation_potential_landscape_defects))
        .def("nm_distance", &potential_landscape::nm_distance, nb::arg("i"), nb::arg("j"),
             DOC(fiction_sidb_simulation_potential_landscape_nm_distance))
        .def("chargeless_potential", &potential_landscape::chargeless_potential, nb::arg("i"), nb::arg("j"),
             DOC(fiction_sidb_simulation_potential_landscape_chargeless_potential))
        .def("local_external_potential", &potential_landscape::local_external_potential, nb::arg("i"),
             DOC(fiction_sidb_simulation_potential_landscape_local_external_potential))
        .def("local_potential_caused_by_defects", &potential_landscape::local_potential_caused_by_defects, nb::arg("i"),
             DOC(fiction_sidb_simulation_potential_landscape_local_potential_caused_by_defects))
        .def("effective_charge_transition_thresholds", &potential_landscape::effective_charge_transition_thresholds,
             nb::arg("i"), DOC(fiction_sidb_simulation_potential_landscape_effective_charge_transition_thresholds))
        .def("chargeless_potential_at_distance", &potential_landscape::chargeless_potential_at_distance,
             nb::arg("distance"), DOC(fiction_sidb_simulation_potential_landscape_chargeless_potential_at_distance))
        .def("local_internal_potentials", &potential_landscape::local_internal_potentials, nb::arg("cd"),
             DOC(fiction_sidb_simulation_potential_landscape_local_internal_potentials))
        .def("local_potentials", &potential_landscape::local_potentials, nb::arg("cd"),
             DOC(fiction_sidb_simulation_potential_landscape_local_potentials))
        .def(
            "energy", [](const potential_landscape& land, const fiction::sidb::charge_distribution& cd)
            { return land.energy(cd); }, nb::arg("cd"), DOC(fiction_sidb_simulation_potential_landscape_energy_2))
        .def(
            "is_physically_valid", [](const potential_landscape& land, const fiction::sidb::charge_distribution& cd)
            { return land.is_physically_valid(cd); }, nb::arg("cd"),
            DOC(fiction_sidb_simulation_potential_landscape_is_physically_valid_2))
        .def("evaluate", &potential_landscape::evaluate, nb::arg("cd"),
             DOC(fiction_sidb_simulation_potential_landscape_evaluate))

        ;
}

}  // namespace pyfiction
