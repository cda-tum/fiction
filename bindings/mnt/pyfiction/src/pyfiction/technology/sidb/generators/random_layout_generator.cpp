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
 * @brief Python bindings for `fiction/technology/sidb/generators/random_layout_generator.hpp`.
 * @author Marcel Walter (marcelwa)
 * @author Jan Drewniok (Drewniok)
 */

#include "pyfiction/documentation.hpp"

#include <fiction/technology/sidb/generators/random_layout_generator.hpp>
#include <fiction/technology/sidb/layout.hpp>

#include <optional>

#include <nanobind/nanobind.h>
#include <nanobind/stl/optional.h>  // NOLINT(misc-include-cleaner)
#include <nanobind/stl/pair.h>      // NOLINT(misc-include-cleaner)
#include <nanobind/stl/vector.h>    // NOLINT(misc-include-cleaner)

namespace pyfiction
{

void random_layout_generator(nanobind::module_& m)
{
    namespace py = nanobind;

    using fiction::sidb::layout;
    using fiction::sidb::generators::generate_random_layout_params;

    py::enum_<generate_random_layout_params::positive_charges>(
        m, "positive_charges", DOC(fiction_sidb_generators_generate_random_layout_params_positive_charges))
        .value("ALLOWED", generate_random_layout_params::positive_charges::ALLOWED,
               DOC(fiction_sidb_generators_generate_random_layout_params_positive_charges_ALLOWED))
        .value("FORBIDDEN", generate_random_layout_params::positive_charges::FORBIDDEN,
               DOC(fiction_sidb_generators_generate_random_layout_params_positive_charges_FORBIDDEN))
        .value("MAY_OCCUR", generate_random_layout_params::positive_charges::MAY_OCCUR,
               DOC(fiction_sidb_generators_generate_random_layout_params_positive_charges_MAY_OCCUR));

    /**
     * Parameters.
     */
    py::class_<generate_random_layout_params>(m, "generate_random_sidb_layout_params",
                                              DOC(fiction_sidb_generators_generate_random_layout_params))
        .def(py::init<>(), "Default constructor.")
        .def_rw("coordinate_pair", &generate_random_layout_params::coordinate_pair,
                DOC(fiction_sidb_generators_generate_random_layout_params_coordinate_pair))
        .def_rw("number_of_sidbs", &generate_random_layout_params::number_of_sidbs,
                DOC(fiction_sidb_generators_generate_random_layout_params_number_of_sidbs))
        .def_rw("positive_sidbs", &generate_random_layout_params::positive_sidbs,
                DOC(fiction_sidb_generators_generate_random_layout_params_positive_sidbs))
        .def_rw("simulation_parameters", &generate_random_layout_params::sim_params,
                DOC(fiction_sidb_generators_generate_random_layout_params_sim_params))
        .def_rw("maximal_attempts", &generate_random_layout_params::maximal_attempts,
                DOC(fiction_sidb_generators_generate_random_layout_params_maximal_attempts))
        .def_rw("number_of_unique_generated_layouts",
                &generate_random_layout_params::number_of_unique_generated_layouts,
                DOC(fiction_sidb_generators_generate_random_layout_params_number_of_unique_generated_layouts))
        .def_rw("maximal_attempts_for_multiple_layouts",
                &generate_random_layout_params::maximal_attempts_for_multiple_layouts,
                DOC(fiction_sidb_generators_generate_random_layout_params_maximal_attempts_for_multiple_layouts));

    m.def(
        "generate_random_sidb_layout",
        [](const generate_random_layout_params& params, const std::optional<layout>& skeleton)
        { return fiction::sidb::generators::generate_random_layout(params, skeleton); }, py::arg("params"),
        py::arg("lyt_skeleton") = std::nullopt, DOC(fiction_sidb_generators_generate_random_layout));

    m.def(
        "generate_multiple_random_sidb_layouts",
        [](const generate_random_layout_params& params, const std::optional<layout>& skeleton)
        { return fiction::sidb::generators::generate_multiple_random_layouts(params, skeleton); }, py::arg("params"),
        py::arg("lyt_skeleton") = std::nullopt, DOC(fiction_sidb_generators_generate_multiple_random_layouts));
}

}  // namespace pyfiction
