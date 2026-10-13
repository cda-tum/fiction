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
 * @brief Python bindings for `fiction/layouts/io/write_fgl_layout.hpp`.
 * @author Simon Hofmann (simon1hofmann)
 * @author Marcel Walter (marcelwa)
 */

#include "pyfiction/documentation.hpp"
#include "pyfiction/types.hpp"

#include <fiction/layouts/io/write_fgl_layout.hpp>
#include <fiction/utils/progress.hpp>

#include <string_view>

#include <nanobind/nanobind.h>
#include <nanobind/stl/function.h>     // NOLINT(misc-include-cleaner): enables callback conversion
#include <nanobind/stl/string_view.h>  // NOLINT(misc-include-cleaner): converts the filename argument.

/**
 * @brief Namespace alias for nanobind.
 */
namespace nb = nanobind;

namespace pyfiction
{

namespace detail
{

/** @brief Register a layout type's FGL writer. @tparam Lyt Layout type. @param m Layout I/O module. */
template <typename Lyt>
void register_write_fgl_layout(nb::module_& m)
{
    m.def(
        "write_fgl_layout",
        [](const Lyt& lyt, const std::string_view& filename, const fiction::utils::progress_callback& on_progress)
        { fiction::layouts::io::write_fgl_layout(lyt, filename, on_progress); }, nb::arg("layout"), nb::arg("filename"),
        nb::arg("on_progress").none() = nb::none(), DOC(fiction_layouts_io_write_fgl_layout_2),
        nb::call_guard<nb::gil_scoped_release>());
}

}  // namespace detail

/** @brief Register FGL writers for supported layout topologies. @param m Layout I/O module. */
void register_write_fgl_layout(nb::module_& m)
{
    detail::register_write_fgl_layout<py_cartesian_gate_layout>(m);
    detail::register_write_fgl_layout<py_shifted_cartesian_gate_layout>(m);
    detail::register_write_fgl_layout<py_hexagonal_gate_layout>(m);
}

}  // namespace pyfiction
