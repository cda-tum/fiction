/*
 * Copyright (c) 2018 - 2023 Marcel Walter
 * Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
 * All rights reserved.
 *
 * SPDX-License-Identifier: MIT
 *
 * Licensed under the MIT License
 */

//
// Created by marcel on 21.11.23.
//

#include "pyfiction/documentation.hpp"
#include "pyfiction/types.hpp"

#include <fiction/technology/sidb/simulation/io/write_sqd_sim_result.hpp>
#include <fiction/technology/sidb/simulation/result.hpp>

#include <string_view>

#include <nanobind/nanobind.h>
#include <nanobind/stl/array.h>          // NOLINT(misc-include-cleaner)
#include <nanobind/stl/function.h>       // NOLINT(misc-include-cleaner)
#include <nanobind/stl/optional.h>       // NOLINT(misc-include-cleaner)
#include <nanobind/stl/pair.h>           // NOLINT(misc-include-cleaner)
#include <nanobind/stl/set.h>            // NOLINT(misc-include-cleaner)
#include <nanobind/stl/shared_ptr.h>     // NOLINT(misc-include-cleaner)
#include <nanobind/stl/string_view.h>    // NOLINT(misc-include-cleaner)
#include <nanobind/stl/unordered_map.h>  // NOLINT(misc-include-cleaner)
#include <nanobind/stl/vector.h>         // NOLINT(misc-include-cleaner)

namespace pyfiction
{

namespace detail
{

template <typename Lyt>
void write_sqd_sim_result(nanobind::module_& m)
{
    namespace py = nanobind;  // NOLINT(misc-unused-alias-decls)

    m.def(
        "write_sqd_sim_result", [](const fiction::sidb::simulation::result<Lyt>& lyt, const std::string_view& filename)
        { fiction::sidb::simulation::io::write_sqd_sim_result(lyt, filename); }, py::arg("layout"), py::arg("filename"),
        DOC(fiction_sidb_simulation_io_write_sqd_sim_result));
}

}  // namespace detail

void write_sqd_sim_result(nanobind::module_& m)
{
    detail::write_sqd_sim_result<py_sidb_100_lattice>(m);
    detail::write_sqd_sim_result<py_sidb_111_lattice>(m);
}

}  // namespace pyfiction
