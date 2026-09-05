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
 * @brief Python bindings for `fiction/technology/sidb/simulation/io/write_sqd_sim_result.hpp`.
 * @author Marcel Walter (marcelwa)
 */

#include "pyfiction/documentation.hpp"
#include "pyfiction/types.hpp"

#include <fiction/technology/sidb/simulation/io/write_sqd_sim_result.hpp>
#include <fiction/technology/sidb/simulation/result.hpp>

#include <string_view>

#include <nanobind/nanobind.h>
#include <nanobind/stl/string_view.h>  // NOLINT(misc-include-cleaner)

namespace pyfiction
{

void write_sqd_sim_result(nanobind::module_& m)
{
    namespace py = nanobind;

    m.def("write_sqd_sim_result",
          static_cast<void (*)(const fiction::sidb::simulation::result&, const std::string_view&)>(
              &fiction::sidb::simulation::io::write_sqd_sim_result),
          py::arg("sim_result"), py::arg("filename"), DOC(fiction_sidb_simulation_io_write_sqd_sim_result_4));
    m.def("write_sqd_sim_result",
          static_cast<void (*)(const fiction::sidb::simulation::legacy_result<py_sidb_100_lattice>&,
                               const std::string_view&)>(
              &fiction::sidb::simulation::io::write_sqd_sim_result<py_sidb_100_lattice>),
          py::arg("sim_result"), py::arg("filename"), DOC(fiction_sidb_simulation_io_write_sqd_sim_result_2));
    m.def("write_sqd_sim_result",
          static_cast<void (*)(const fiction::sidb::simulation::legacy_result<py_sidb_111_lattice>&,
                               const std::string_view&)>(
              &fiction::sidb::simulation::io::write_sqd_sim_result<py_sidb_111_lattice>),
          py::arg("sim_result"), py::arg("filename"), DOC(fiction_sidb_simulation_io_write_sqd_sim_result_2));
}

}  // namespace pyfiction
