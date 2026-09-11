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
 * @brief Python bindings for `fiction/layouts/io/layout_drawers.hpp`.
 * @author Marcel Walter (marcelwa)
 */

#include "pyfiction/documentation.hpp"
#include "pyfiction/types.hpp"

#include <fiction/layouts/io/layout_drawers.hpp>
#include <fiction/traits.hpp>

#include <string_view>

#include <nanobind/nanobind.h>
#include <nanobind/stl/array.h>        // NOLINT(misc-include-cleaner)
#include <nanobind/stl/function.h>     // NOLINT(misc-include-cleaner)
#include <nanobind/stl/optional.h>     // NOLINT(misc-include-cleaner)
#include <nanobind/stl/pair.h>         // NOLINT(misc-include-cleaner)
#include <nanobind/stl/set.h>          // NOLINT(misc-include-cleaner)
#include <nanobind/stl/shared_ptr.h>   // NOLINT(misc-include-cleaner)
#include <nanobind/stl/string.h>       // NOLINT(misc-include-cleaner)
#include <nanobind/stl/string_view.h>  // NOLINT(misc-include-cleaner)
#include <nanobind/stl/vector.h>       // NOLINT(misc-include-cleaner)

namespace pyfiction
{

namespace detail
{

/**
 * Writes a gate-level layout as DOT with the drawer its topology needs.
 *
 * @tparam Lyt Gate-level layout type.
 * @tparam ClockColors Color the tiles by clock number instead of by gate type.
 * @tparam DrawIndexes Label the tiles with their node indices.
 * @param lyt The layout to draw.
 * @param filename Path of the file to write.
 */
template <typename Lyt, bool ClockColors, bool DrawIndexes>
void draw(const Lyt& lyt, const std::string_view& filename)
{
    if constexpr (fiction::is_cartesian_layout_v<Lyt>)
    {
        fiction::layouts::io::write_dot_layout<
            Lyt, fiction::layouts::io::gate_layout_cartesian_drawer<Lyt, ClockColors, DrawIndexes>>(lyt, filename);
    }
    else if constexpr (fiction::is_shifted_cartesian_layout_v<Lyt>)
    {
        fiction::layouts::io::write_dot_layout<
            Lyt, fiction::layouts::io::gate_layout_shifted_cartesian_drawer<Lyt, ClockColors, DrawIndexes>>(lyt,
                                                                                                            filename);
    }
    else if constexpr (fiction::is_hexagonal_layout_v<Lyt>)
    {
        fiction::layouts::io::write_dot_layout<
            Lyt, fiction::layouts::io::gate_layout_hexagonal_drawer<Lyt, ClockColors, DrawIndexes>>(lyt, filename);
    }
}

template <typename Lyt>
void write_dot_layout(nanobind::module_& m)
{
    namespace py = nanobind;  // NOLINT(misc-unused-alias-decls)

    m.def(
        "write_dot_layout",
        [](const Lyt& lyt, const std::string_view& filename, const bool clock_colors, const bool indexes)
        {
            if (clock_colors && indexes)
            {
                draw<Lyt, true, true>(lyt, filename);
            }
            else if (clock_colors)
            {
                draw<Lyt, true, false>(lyt, filename);
            }
            else if (indexes)
            {
                draw<Lyt, false, true>(lyt, filename);
            }
            else
            {
                draw<Lyt, false, false>(lyt, filename);
            }
        },
        py::arg("layout"), py::arg("filename"), py::arg("clock_colors") = false, py::arg("indexes") = false,
        DOC(fiction_layouts_io_write_dot_layout));
}

}  // namespace detail

void layout_drawers(nanobind::module_& m)
{
    detail::write_dot_layout<py_cartesian_gate_layout>(m);
    detail::write_dot_layout<py_shifted_cartesian_gate_layout>(m);
    detail::write_dot_layout<py_hexagonal_gate_layout>(m);
}

}  // namespace pyfiction
