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
 * @brief Python bindings for `fiction/technology/inml/io/write_qcc_layout.hpp`.
 * @author Marcel Walter (marcelwa)
 */

#include "pyfiction/documentation.hpp"
#include "pyfiction/types.hpp"

#include <fiction/technology/inml/io/write_qcc_layout.hpp>

#include <string_view>

#include <nanobind/nanobind.h>
#include <nanobind/stl/array.h>          // NOLINT(misc-include-cleaner)
#include <nanobind/stl/pair.h>           // NOLINT(misc-include-cleaner)
#include <nanobind/stl/shared_ptr.h>     // NOLINT(misc-include-cleaner)
#include <nanobind/stl/string_view.h>    // NOLINT(misc-include-cleaner)
#include <nanobind/stl/unordered_map.h>  // NOLINT(misc-include-cleaner)
#include <nanobind/stl/unordered_set.h>  // NOLINT(misc-include-cleaner)
#include <nanobind/stl/vector.h>         // NOLINT(misc-include-cleaner)

namespace pyfiction
{

void write_qcc_layout(nanobind::module_& m)
{
    namespace py = nanobind;

    py::class_<fiction::inml::io::write_qcc_layout_params>(m, "write_qcc_layout_params",
                                                           DOC(fiction_inml_io_write_qcc_layout_params))
        .def(py::init<>(), "Default constructor.")
        .def_rw("use_filename_as_component_name",
                &fiction::inml::io::write_qcc_layout_params::use_filename_as_component_name,
                DOC(fiction_inml_io_write_qcc_layout_params_use_filename_as_component_name));

    m.def(
        "write_qcc_layout",
        [](const py_inml_layout& lyt, const std::string_view& filename,
           const fiction::inml::io::write_qcc_layout_params& params)
        { fiction::inml::io::write_qcc_layout(lyt, filename, params); },
        py::arg("layout"), py::arg("filename"), py::arg("params") = fiction::inml::io::write_qcc_layout_params{},
        DOC(fiction_inml_io_write_qcc_layout));
}

}  // namespace pyfiction
