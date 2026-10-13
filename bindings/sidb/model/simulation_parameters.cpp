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

#include "pyfiction/documentation.hpp"

#include <fiction/technology/sidb/model/simulation_parameters.hpp>

#include <fmt/format.h>

#include <cstdint>

#include <nanobind/nanobind.h>

/**
 * @brief Namespace alias for nanobind.
 */
namespace nb = nanobind;

namespace pyfiction
{

/**
 * @brief Binds `simulation_parameters`.
 * @param m Python module.
 */
void bind_simulation_parameters(nb::module_& m)
{
    nb::class_<fiction::sidb::model::simulation_parameters>(m, "sidb_simulation_parameters",
                                                            DOC(fiction_sidb_model_simulation_parameters))
        .def(nb::init<const uint8_t, const double, const double, const double>(), nb::arg("base_number") = 3,
             nb::arg("mu_minus") = -0.32, nb::arg("relative_permittivity") = 5.6, nb::arg("screening_distance") = 5.0,
             DOC(fiction_sidb_model_simulation_parameters_simulation_parameters))
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
