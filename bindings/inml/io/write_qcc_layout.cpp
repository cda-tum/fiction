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

#include <fiction/technology/inml/io/write_qcc_layout.hpp>
#include <fiction/technology/inml/layout.hpp>

#include <string_view>

#include <nanobind/nanobind.h>
#include <nanobind/stl/array.h>          // NOLINT(misc-include-cleaner)
#include <nanobind/stl/function.h>       // NOLINT(misc-include-cleaner): enables callback conversion
#include <nanobind/stl/optional.h>       // NOLINT(misc-include-cleaner): enables keyword options
#include <nanobind/stl/pair.h>           // NOLINT(misc-include-cleaner)
#include <nanobind/stl/shared_ptr.h>     // NOLINT(misc-include-cleaner)
#include <nanobind/stl/string_view.h>    // NOLINT(misc-include-cleaner)
#include <nanobind/stl/unordered_map.h>  // NOLINT(misc-include-cleaner)
#include <nanobind/stl/unordered_set.h>  // NOLINT(misc-include-cleaner)
#include <nanobind/stl/vector.h>         // NOLINT(misc-include-cleaner)

/**
 * @brief Namespace alias for nanobind.
 */
namespace nb = nanobind;

namespace pyfiction
{

/** @brief Native options constructed for each Python call. */
using write_qcc_layout_options = fiction::inml::io::write_qcc_layout_params;

/**
 * @brief Binds write qcc layout helpers.
 * @param m Python module.
 */
void register_write_qcc_layout(nb::module_& m)
{

    m.def(
        "write_qcc_layout",
        [](const fiction::inml::layout& lyt, const std::string_view& filename,
           const decltype(write_qcc_layout_options{}.on_progress)&                   on_progress,
           const decltype(write_qcc_layout_options{}.use_filename_as_component_name) use_filename_as_component_name)
        {
            write_qcc_layout_options params{};
            params.on_progress                    = on_progress;
            params.use_filename_as_component_name = use_filename_as_component_name;
            const nb::gil_scoped_release release{};
            fiction::inml::io::write_qcc_layout(lyt, filename, params);
        },
        nb::arg("layout"), nb::arg("filename"), nb::kw_only(), nb::arg("on_progress").none() = nb::none(),
        nb::arg("use_filename_as_component_name") = write_qcc_layout_options{}.use_filename_as_component_name,
        R"doc(Write the iNML layout to the supplied ToPoliNano and MagCAD filename.
use_filename_as_component_name selects the component name; on_progress reports writing progress.
)doc");
}

}  // namespace pyfiction
