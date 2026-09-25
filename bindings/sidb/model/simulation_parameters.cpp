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

namespace pyfiction
{

void simulation_parameters(nanobind::module_& m)
{
    namespace py = nanobind;  // NOLINT(misc-unused-alias-decls)

    py::class_<fiction::sidb::model::simulation_parameters>(m, "sidb_simulation_parameters",
                                                            DOC(fiction_sidb_model_simulation_parameters))
        .def(py::init<const uint8_t, const double, const double, const double>(), py::arg("base_number") = 3,
             py::arg("mu_minus") = -0.32, py::arg("relative_permittivity") = 5.6, py::arg("screening_distance") = 5.0,
             DOC(fiction_sidb_model_simulation_parameters_simulation_parameters))
        .def(py::init<>(), DOC(fiction_sidb_model_simulation_parameters_simulation_parameters))
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
