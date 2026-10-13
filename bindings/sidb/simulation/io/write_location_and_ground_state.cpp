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
 * @brief Python bindings for `fiction/technology/sidb/simulation/io/write_location_and_ground_state.hpp`.
 * @author Marcel Walter (marcelwa)
 */

#include "pyfiction/documentation.hpp"

#include <fiction/technology/sidb/simulation/io/write_location_and_ground_state.hpp>
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
 * @brief Binds `write_location_and_ground_state`.
 * @param m Python module.
 */
void bind_write_location_and_ground_state(nb::module_& m)
{
    // NOLINTNEXTLINE(misc-const-correctness)
    void (*const write_pointer)(const fiction::sidb::simulation::result&, const std::string_view&) =
        &fiction::sidb::simulation::io::write_location_and_ground_state;

    m.def("write_location_and_ground_state", write_pointer, nb::arg("sim_result"), nb::arg("filename"),
          DOC(fiction_sidb_simulation_io_write_location_and_ground_state_2));
}

}  // namespace pyfiction
