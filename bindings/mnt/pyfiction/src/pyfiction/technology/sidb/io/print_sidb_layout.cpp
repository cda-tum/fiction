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
 * @brief Python binding for the textual SiDB layout printer.
 * @author Marcel Walter (marcelwa)
 */

#include "pyfiction/documentation.hpp"

#include <fiction/technology/sidb/charge_distribution.hpp>
#include <fiction/technology/sidb/io/print_sidb_layout.hpp>
#include <fiction/technology/sidb/layout.hpp>

#include <optional>
#include <sstream>

#include <nanobind/nanobind.h>
#include <nanobind/stl/optional.h>  // NOLINT(misc-include-cleaner)
#include <nanobind/stl/string.h>    // NOLINT(misc-include-cleaner)

namespace pyfiction
{

void print_sidb_layout(nanobind::module_& m)
{
    namespace py = nanobind;

    m.def(
        "print_sidb_layout",
        [](const fiction::sidb::layout& lyt, const std::optional<fiction::sidb::charge_distribution>& cd,
           const bool draw_lattice)
        {
            std::ostringstream os{};
            fiction::sidb::io::print_sidb_layout(os, lyt, cd.has_value() ? &*cd : nullptr, false, false, draw_lattice);
            return os.str();
        },
        py::arg("layout"), py::arg("charge_distribution") = py::none(), py::arg("draw_lattice") = false,
        DOC(fiction_sidb_io_print_sidb_layout));
}

}  // namespace pyfiction
