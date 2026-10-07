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

namespace pyfiction
{

void write_location_and_ground_state(nanobind::module_& m)
{
    namespace py = nanobind;

    // NOLINTNEXTLINE(misc-const-correctness)
    void (*const write_pointer)(const fiction::sidb::simulation::result&, const std::string_view&) =
        &fiction::sidb::simulation::io::write_location_and_ground_state;

    m.def("write_location_and_ground_state", write_pointer, py::arg("sim_result"), py::arg("filename"),
          DOC(fiction_sidb_simulation_io_write_location_and_ground_state_2));
}

}  // namespace pyfiction
