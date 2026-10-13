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

#include "pyfiction/documentation.hpp"

#include <fiction/technology/mol_qca/io/write_mol_qca_layout_svg.hpp>
#include <fiction/technology/mol_qca/layout.hpp>

#include <optional>
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
 * @brief Registers the molQCA SVG writer and its parameters.
 *
 * @param m Python module.
 */
void register_write_mol_qca_layout_svg(nb::module_& m)
{
    m.def(
        "write_mol_qca_layout_svg",
        [](const fiction::mol_qca::layout& layout, const std::string_view& filename,
           const decltype(write_mol_qca_layout_svg_options{}.on_progress) on_progress,
           const decltype(write_mol_qca_layout_svg_options{}.simple)      simple)
        {
            write_mol_qca_layout_svg_options params{};
            params.on_progress = on_progress;
            params.simple      = simple;
            const nb::gil_scoped_release release{};
            return fiction::mol_qca::io::write_mol_qca_layout_svg(layout, filename, params);
        },
        nb::arg("layout"), nb::arg("filename"), nb::kw_only(), nb::arg("on_progress").none() = nb::none(),
        nb::arg("simple") = write_mol_qca_layout_svg_options{}.simple,
        R"doc(Writes an SVG representation of a molQCA layout to an output stream.
Each regular cell is colored by its clock phase.

May throw an `unsupported_cell_type_exception` if it encounters
unsupported cell types in the layout.

Args:
    lyt: The layout to be written.
    os: The output stream to write into.

Algorithm options are keyword-only; the signature lists their defaults.
)doc");
}

}  // namespace pyfiction
