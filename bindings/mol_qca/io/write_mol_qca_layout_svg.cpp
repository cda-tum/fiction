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
 * @brief Python bindings for `fiction/technology/mol_qca/io/write_mol_qca_layout_svg.hpp`.
 * @author Marcel Walter (marcelwa)
 */

#include <fiction/technology/mol_qca/io/write_mol_qca_layout_svg.hpp>
#include <fiction/technology/mol_qca/layout.hpp>

#include <string_view>

#include <nanobind/nanobind.h>
#include <nanobind/stl/function.h>     // NOLINT(misc-include-cleaner): enables callback conversion
#include <nanobind/stl/optional.h>     // NOLINT(misc-include-cleaner): enables keyword options
#include <nanobind/stl/string.h>       // NOLINT(misc-include-cleaner)
#include <nanobind/stl/string_view.h>  // NOLINT(misc-include-cleaner)

/**
 * @brief Namespace alias for nanobind.
 */
namespace nb = nanobind;

namespace pyfiction
{

/** @brief Native options constructed for each Python call. */
using write_mol_qca_layout_svg_options = fiction::mol_qca::io::write_mol_qca_layout_svg_params;

/**
 * @brief Registers the molQCA SVG writer.
 *
 * @param m Python module.
 */
void register_write_mol_qca_layout_svg(nb::module_& m)
{
    m.def(
        "write_mol_qca_layout_svg",
        [](const fiction::mol_qca::layout& layout, const std::string_view& filename,
           const decltype(write_mol_qca_layout_svg_options{}.on_progress)& on_progress,
           const decltype(write_mol_qca_layout_svg_options{}.simple)       simple)
        {
            write_mol_qca_layout_svg_options params{};
            params.on_progress = on_progress;
            params.simple      = simple;
            const nb::gil_scoped_release release{};
            fiction::mol_qca::io::write_mol_qca_layout_svg(layout, filename, params);
        },
        nb::arg("layout"), nb::arg("filename"), nb::kw_only(), nb::arg("on_progress").none() = nb::none(),
        nb::arg("simple") = write_mol_qca_layout_svg_options{}.simple,
        R"doc(Write an SVG representation of the molQCA layout to the supplied filename.
simple selects a compact drawing; on_progress reports writing progress.

Args:
    layout: Layout to write.
    filename: Output file path.
    on_progress: Callback receiving (task, completed, total); None disables progress reporting.
    simple: Limit details to create smaller file sizes.

Raises:
    RuntimeError: The file cannot be created, written, closed, or replaced, or the layout contains
        an unsupported cell type.
)doc");
}

}  // namespace pyfiction
