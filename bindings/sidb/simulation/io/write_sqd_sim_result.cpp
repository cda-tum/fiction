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

#include <fiction/technology/sidb/simulation/io/write_sqd_sim_result.hpp>
#include <fiction/technology/sidb/simulation/result.hpp>

#include <string_view>

#include <nanobind/nanobind.h>
#include <nanobind/stl/string_view.h>  // NOLINT(misc-include-cleaner)

/**
 * @brief Namespace alias for nanobind.
 */
namespace nb = nanobind;

namespace pyfiction
{

/**
 * @brief Binds write sqd sim result helpers.
 * @param m Python module.
 */
void register_write_sqd_sim_result(nb::module_& m)
{
    m.def("write_sqd_sim_result",
          nb::overload_cast<const fiction::sidb::simulation::result&, const std::string_view&>(
              &fiction::sidb::simulation::io::write_sqd_sim_result),
          nb::arg("sim_result"), nb::arg("filename"), DOC(fiction_sidb_simulation_io_write_sqd_sim_result_2));
}

}  // namespace pyfiction
