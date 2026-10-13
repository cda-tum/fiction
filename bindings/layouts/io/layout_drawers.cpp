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

#include "fiction/layouts/cartesian_layout.hpp"
#include "fiction/layouts/hexagonal_layout.hpp"
#include "fiction/layouts/shifted_cartesian_layout.hpp"
#include "pyfiction/documentation.hpp"
#include "pyfiction/types.hpp"

#include <fiction/layouts/io/layout_drawers.hpp>
#include <fiction/utils/progress.hpp>

#include <concepts>
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

/**
 * @brief Namespace alias for nanobind.
 */
namespace nb = nanobind;

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
 * @param on_progress Receives completed tile rendering.
 */
template <typename Lyt, bool ClockColors, bool DrawIndexes>
void draw(const Lyt& lyt, const std::string_view& filename, const fiction::utils::progress_callback& on_progress)
{
    if constexpr (std::derived_from<Lyt, fiction::layouts::cartesian_layout>)
    {
        fiction::layouts::io::write_dot_layout<
            Lyt, fiction::layouts::io::gate_layout_cartesian_drawer<Lyt, ClockColors, DrawIndexes>>(lyt, filename, {},
                                                                                                    on_progress);
    }
    else if constexpr (std::derived_from<Lyt, fiction::layouts::shifted_cartesian_layout>)
    {
        fiction::layouts::io::write_dot_layout<
            Lyt, fiction::layouts::io::gate_layout_shifted_cartesian_drawer<Lyt, ClockColors, DrawIndexes>>(
            lyt, filename, {}, on_progress);
    }
    else if constexpr ((std::derived_from<Lyt, fiction::layouts::hexagonal_layout> &&
                        !std::derived_from<Lyt, fiction::layouts::shifted_cartesian_layout>))
    {
        fiction::layouts::io::write_dot_layout<
            Lyt, fiction::layouts::io::gate_layout_hexagonal_drawer<Lyt, ClockColors, DrawIndexes>>(lyt, filename, {},
                                                                                                    on_progress);
    }
}

template <typename Lyt>
void write_dot_layout(nb::module_& m)
{
    m.def(
        "write_dot_layout",
        [](const Lyt& lyt, const std::string_view& filename, const bool clock_colors, const bool indexes,
           const fiction::utils::progress_callback& on_progress)
        {
            if (clock_colors && indexes)
            {
                draw<Lyt, true, true>(lyt, filename, on_progress);
            }
            else if (clock_colors)
            {
                draw<Lyt, true, false>(lyt, filename, on_progress);
            }
            else if (indexes)
            {
                draw<Lyt, false, true>(lyt, filename, on_progress);
            }
            else
            {
                draw<Lyt, false, false>(lyt, filename, on_progress);
            }
        },
        nb::arg("layout"), nb::arg("filename"), nb::arg("clock_colors") = false, nb::arg("indexes") = false,
        nb::arg("on_progress").none() = nb::none(), DOC(fiction_layouts_io_write_dot_layout),
        nb::call_guard<nb::gil_scoped_release>());
}

}  // namespace detail

void layout_drawers(nb::module_& m)
{
    detail::write_dot_layout<py_cartesian_gate_layout>(m);
    detail::write_dot_layout<py_shifted_cartesian_gate_layout>(m);
    detail::write_dot_layout<py_hexagonal_gate_layout>(m);
}

}  // namespace pyfiction
