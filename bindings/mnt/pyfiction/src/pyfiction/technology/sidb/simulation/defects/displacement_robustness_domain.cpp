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
 * @author Jan Drewniok (Drewniok)
 * @author Marcel Walter (marcelwa)
 */

#include "pyfiction/types.hpp"

#include <fiction/layouts/coordinates.hpp>
#include <fiction/technology/sidb/simulation/defects/displacement_robustness_domain.hpp>

#include <fmt/format.h>

#include <string>

#include <nanobind/nanobind.h>
#include <nanobind/stl/optional.h>  // NOLINT(misc-include-cleaner)
#include <nanobind/stl/pair.h>      // NOLINT(misc-include-cleaner)
#include <nanobind/stl/set.h>       // NOLINT(misc-include-cleaner)
#include <nanobind/stl/string.h>    // NOLINT(misc-include-cleaner)
#include <nanobind/stl/vector.h>    // NOLINT(misc-include-cleaner)

namespace pyfiction
{

namespace detail
{

template <typename Lyt>
void determine_displacement_robustness_domain_impl(nanobind::module_& m, const std::string& lattice = "")
{
    namespace py = nanobind;  // NOLINT(misc-unused-alias-decls)

    py::class_<fiction::sidb::simulation::defects::displacement_robustness_domain<Lyt>>(
        m, fmt::format("displacement_robustness_domain_{}", lattice).c_str())
        .def(py::init<>(), "Default constructor.")
        .def_rw("influence_information",
                &fiction::sidb::simulation::defects::displacement_robustness_domain<Lyt>::operational_values);

    m.def(fmt::format("determine_displacement_robustness_domain_{}", lattice).c_str(),
          &fiction::sidb::simulation::defects::determine_displacement_robustness_domain<Lyt, py_tt>, py::arg("layout"),
          py::arg("spec"), py::arg("params"), py::arg("stats") = nullptr);
}

}  // namespace detail

void displacement_robustness_domain(nanobind::module_& m)
{
    namespace py = nanobind;  // NOLINT(misc-unused-alias-decls)

    py::enum_<fiction::sidb::simulation::defects::displacement_robustness_domain_params<
        fiction::layouts::coords::offset>::dimer_displacement_policy>(m, "dimer_displacement_policy")
        .value("STAY_ON_ORIGINAL_DIMER",
               fiction::sidb::simulation::defects::displacement_robustness_domain_params<
                   fiction::layouts::coords::offset>::dimer_displacement_policy::STAY_ON_ORIGINAL_DIMER)
        .value("ALLOW_OTHER_DIMER",
               fiction::sidb::simulation::defects::displacement_robustness_domain_params<
                   fiction::layouts::coords::offset>::dimer_displacement_policy::ALLOW_OTHER_DIMER);

    py::enum_<fiction::sidb::simulation::defects::displacement_robustness_domain_params<
        fiction::layouts::coords::offset>::displacement_analysis_mode>(m, "displacement_analysis_mode")
        .value("EXHAUSTIVE", fiction::sidb::simulation::defects::displacement_robustness_domain_params<
                                 fiction::layouts::coords::offset>::displacement_analysis_mode::EXHAUSTIVE)
        .value("RANDOM", fiction::sidb::simulation::defects::displacement_robustness_domain_params<
                             fiction::layouts::coords::offset>::displacement_analysis_mode::RANDOM);

    py::class_<
        fiction::sidb::simulation::defects::displacement_robustness_domain_params<fiction::layouts::coords::offset>>(
        m, "displacement_robustness_domain_params")
        .def(py::init<>(), "Default constructor.")
        .def_rw("analysis_mode", &fiction::sidb::simulation::defects::displacement_robustness_domain_params<
                                     fiction::layouts::coords::offset>::analysis_mode)
        .def_rw("percentage_of_analyzed_displaced_layouts",
                &fiction::sidb::simulation::defects::displacement_robustness_domain_params<
                    fiction::layouts::coords::offset>::percentage_of_analyzed_displaced_layouts)
        .def_rw("displacement_variations", &fiction::sidb::simulation::defects::displacement_robustness_domain_params<
                                               fiction::layouts::coords::offset>::displacement_variations)
        .def_rw("operational_params", &fiction::sidb::simulation::defects::displacement_robustness_domain_params<
                                          fiction::layouts::coords::offset>::operational_params)
        .def_rw("fixed_sidbs", &fiction::sidb::simulation::defects::displacement_robustness_domain_params<
                                   fiction::layouts::coords::offset>::fixed_sidbs)
        .def_rw("dimer_policy", &fiction::sidb::simulation::defects::displacement_robustness_domain_params<
                                    fiction::layouts::coords::offset>::dimer_policy)
        .def_rw("number_of_threads", &fiction::sidb::simulation::defects::displacement_robustness_domain_params<
                                         fiction::layouts::coords::offset>::number_of_threads);

    py::class_<fiction::sidb::simulation::defects::displacement_robustness_domain_stats>(
        m, "displacement_robustness_domain_stats")
        .def(py::init<>(), "Default constructor.")
        .def_rw("time_total", &fiction::sidb::simulation::defects::displacement_robustness_domain_stats::time_total)
        .def_rw("num_operational_sidb_displacements",
                &fiction::sidb::simulation::defects::displacement_robustness_domain_stats::
                    num_operational_sidb_displacements)
        .def_rw("num_non_operational_sidb_displacements",
                &fiction::sidb::simulation::defects::displacement_robustness_domain_stats::
                    num_non_operational_sidb_displacements);

    // NOTE: be careful with the order of the following calls! Python will resolve the first matching overload!
    detail::determine_displacement_robustness_domain_impl<py_sidb_100_lattice>(m, "100");
    detail::determine_displacement_robustness_domain_impl<py_sidb_111_lattice>(m, "111");
}

}  // namespace pyfiction
