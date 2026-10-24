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
 * @brief Python bindings for `fiction/technology/qca/io/write_qca_layout.hpp`.
 * @author Marcel Walter (marcelwa)
 */

#include <fiction/technology/qca/io/write_qca_layout.hpp>
#include <fiction/technology/qca/layout.hpp>

#include <string_view>

#include <nanobind/nanobind.h>
#include <nanobind/stl/function.h>     // NOLINT(misc-include-cleaner): enables callback conversion
#include <nanobind/stl/optional.h>     // NOLINT(misc-include-cleaner): enables keyword options
#include <nanobind/stl/pair.h>         // NOLINT(misc-include-cleaner)
#include <nanobind/stl/string_view.h>  // NOLINT(misc-include-cleaner)
#include <nanobind/stl/vector.h>       // NOLINT(misc-include-cleaner)

/**
 * @brief Namespace alias for nanobind.
 */
namespace nb = nanobind;

namespace pyfiction
{

/** @brief Native options constructed for each Python call. */
using write_qca_layout_options = fiction::qca::io::write_qca_layout_params;

/**
 * @brief Binds write qca layout helpers.
 * @param m Python module.
 */
void register_write_qca_layout(nb::module_& m)
{
    m.def(
        "write_qca_layout",
        [](const fiction::qca::layout& layout, const std::string_view& filename,
           const decltype(write_qca_layout_options{}.on_progress)&                 on_progress,
           const decltype(write_qca_layout_options{}.create_inter_layer_via_cells) create_inter_layer_via_cells)
        {
            const write_qca_layout_options params{.create_inter_layer_via_cells = create_inter_layer_via_cells,
                                                  .on_progress                  = on_progress};
            const nb::gil_scoped_release   release{};
            fiction::qca::io::write_qca_layout(layout, filename, params);
        },
        nb::arg("layout"), nb::arg("filename"), nb::kw_only(), nb::arg("on_progress").none() = nb::none(),
        nb::arg("create_inter_layer_via_cells") = write_qca_layout_options{}.create_inter_layer_via_cells,
        R"doc(Write the QCA layout to the supplied QCADesigner filename.
create_inter_layer_via_cells controls via cells; on_progress reports writing progress.

Args:
    layout: Layout to write.
    filename: Output file path.
    on_progress: Callback receiving (task, completed, total); None disables progress reporting.
    create_inter_layer_via_cells: Create via cells in between each layer.

Raises:
    RuntimeError: The file cannot be created, written, closed, or replaced.
)doc");
}

}  // namespace pyfiction
