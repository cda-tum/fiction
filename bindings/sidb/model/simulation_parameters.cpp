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
 * @brief Python bindings for `fiction/technology/sidb/model/simulation_parameters.hpp`.
 * @author Marcel Walter (marcelwa)
 */

#include "pyfiction/simulation_parameters.hpp"

#include "pyfiction/documentation.hpp"

#include <fiction/technology/sidb/model/simulation_parameters.hpp>

#include <fmt/format.h>

#include <cstdint>
#include <memory>

#include <nanobind/nanobind.h>

/**
 * @brief Namespace alias for nanobind.
 */
namespace nb = nanobind;

namespace pyfiction
{

/**
 * @brief Binds simulation parameters helpers.
 * @param m Python module.
 */
void register_simulation_parameters(nb::module_& m)
{
    // Keep the GIL for scalar physical parameters; releasing it costs more than these operations.
    nb::class_<fiction::sidb::model::simulation_parameters>(m, "sidb_simulation_parameters",
                                                            DOC(fiction_sidb_model_simulation_parameters))
        .def(
            "__init__",
            [](fiction::sidb::model::simulation_parameters* self, const uint8_t base_number, const double mu_minus,
               const double relative_permittivity, const double screening_distance)
            {
                std::construct_at(
                    self, make_simulation_parameters(base_number, mu_minus, relative_permittivity, screening_distance));
            },
            nb::kw_only(), nb::arg("base_number") = 3, nb::arg("mu_minus") = -0.32,
            nb::arg("relative_permittivity") = 5.6, nb::arg("screening_distance") = 5.0,
            "Store physical model values. All arguments are keyword-only; base_number must be 2 or 3.")
        .def_rw("epsilon_r", &fiction::sidb::model::simulation_parameters::epsilon_r,
                DOC(fiction_sidb_model_simulation_parameters_epsilon_r))
        .def_rw("lambda_tf", &fiction::sidb::model::simulation_parameters::lambda_tf,
                DOC(fiction_sidb_model_simulation_parameters_lambda_tf))
        .def_rw("mu_minus", &fiction::sidb::model::simulation_parameters::mu_minus,
                DOC(fiction_sidb_model_simulation_parameters_mu_minus))
        .def_rw("base", &fiction::sidb::model::simulation_parameters::base,
                DOC(fiction_sidb_model_simulation_parameters_base))
        .def("k", &fiction::sidb::model::simulation_parameters::k, DOC(fiction_sidb_model_simulation_parameters_k))
        .def("mu_plus", &fiction::sidb::model::simulation_parameters::mu_plus,
             DOC(fiction_sidb_model_simulation_parameters_mu_plus));
}

}  // namespace pyfiction
