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
 * @brief Python bindings for `fiction/technology/sidb/io/write_sidb_layout_svg.hpp`.
 * @author Marcel Walter (marcelwa)
 */

#include "pyfiction/documentation.hpp"

#include <fiction/technology/sidb/charge_distribution.hpp>
#include <fiction/technology/sidb/io/write_sidb_layout_svg.hpp>
#include <fiction/technology/sidb/layout.hpp>

#include <sstream>
#include <string_view>

#include <nanobind/nanobind.h>
#include <nanobind/stl/array.h>          // NOLINT(misc-include-cleaner)
#include <nanobind/stl/function.h>       // NOLINT(misc-include-cleaner): enables callback conversion
#include <nanobind/stl/optional.h>       // NOLINT(misc-include-cleaner)
#include <nanobind/stl/pair.h>           // NOLINT(misc-include-cleaner)
#include <nanobind/stl/string.h>         // NOLINT(misc-include-cleaner)
#include <nanobind/stl/string_view.h>    // NOLINT(misc-include-cleaner)
#include <nanobind/stl/tuple.h>          // NOLINT(misc-include-cleaner)
#include <nanobind/stl/unordered_map.h>  // NOLINT(misc-include-cleaner)
#include <nanobind/stl/vector.h>         // NOLINT(misc-include-cleaner)

/**
 * @brief Namespace alias for nanobind.
 */
namespace nb = nanobind;

namespace pyfiction
{

/** @brief Native options constructed for each Python call. */
using write_sidb_layout_svg_options = fiction::sidb::io::write_sidb_layout_svg_params;

/**
 * @brief Registers SVG export and its rendering parameters.
 *
 * @param m Python module.
 */
void register_write_sidb_layout_svg(nb::module_& m)
{
    nb::enum_<write_sidb_layout_svg_options::color_mode>(m, "color_mode",
                                                         DOC(fiction_sidb_io_write_sidb_layout_svg_params_color_mode))
        .value("LIGHT", write_sidb_layout_svg_options::color_mode::LIGHT,
               DOC(fiction_sidb_io_write_sidb_layout_svg_params_color_mode_LIGHT))
        .value("DARK", write_sidb_layout_svg_options::color_mode::DARK,
               DOC(fiction_sidb_io_write_sidb_layout_svg_params_color_mode_DARK));

    nb::enum_<write_sidb_layout_svg_options::sidb_lattice_mode>(
        m, "sidb_lattice_mode", DOC(fiction_sidb_io_write_sidb_layout_svg_params_sidb_lattice_mode))
        .value("SHOW_LATTICE", write_sidb_layout_svg_options::sidb_lattice_mode::SHOW_LATTICE,
               DOC(fiction_sidb_io_write_sidb_layout_svg_params_sidb_lattice_mode_SHOW_LATTICE))
        .value("HIDE_LATTICE", write_sidb_layout_svg_options::sidb_lattice_mode::HIDE_LATTICE,
               DOC(fiction_sidb_io_write_sidb_layout_svg_params_sidb_lattice_mode_HIDE_LATTICE));

    m.def(
        "write_sidb_layout_svg",
        [](const fiction::sidb::layout& layout, const std::string_view& filename,
           const decltype(fiction::sidb::io::write_sidb_layout_svg_params{}.on_progress)&       on_progress,
           const decltype(fiction::sidb::io::write_sidb_layout_svg_params{}.lattice_point_size) lattice_point_size,
           const decltype(fiction::sidb::io::write_sidb_layout_svg_params{}.sidb_size)          sidb_size,
           const decltype(fiction::sidb::io::write_sidb_layout_svg_params{}.sidb_border_width)  sidb_border_width,
           const decltype(fiction::sidb::io::write_sidb_layout_svg_params{}.color_background)   color_background,
           const decltype(fiction::sidb::io::write_sidb_layout_svg_params{}.lattice_mode)       lattice_mode)
        {
            fiction::sidb::io::write_sidb_layout_svg_params ps{};
            ps.on_progress        = on_progress;
            ps.lattice_point_size = lattice_point_size;
            ps.sidb_size          = sidb_size;
            ps.sidb_border_width  = sidb_border_width;
            ps.color_background   = color_background;
            ps.lattice_mode       = lattice_mode;
            const nb::gil_scoped_release release{};
            (static_cast<void (*)(const fiction::sidb::layout&, const std::string_view&,
                                  const write_sidb_layout_svg_options&)>(&fiction::sidb::io::write_sidb_layout_svg))(
                layout, filename, ps);
        },
        nb::arg("layout"), nb::arg("filename"), nb::kw_only(), nb::arg("on_progress").none() = nb::none(),
        nb::arg("lattice_point_size") = fiction::sidb::io::write_sidb_layout_svg_params{}.lattice_point_size,
        nb::arg("sidb_size")          = fiction::sidb::io::write_sidb_layout_svg_params{}.sidb_size,
        nb::arg("sidb_border_width")  = fiction::sidb::io::write_sidb_layout_svg_params{}.sidb_border_width,
        nb::arg("color_background")   = fiction::sidb::io::write_sidb_layout_svg_params{}.color_background,
        nb::arg("lattice_mode")       = fiction::sidb::io::write_sidb_layout_svg_params{}.lattice_mode,
        R"doc(Write an SVG representation of the SiDB layout to the supplied filename.
The keyword options select dot sizes, background coloring, lattice rendering, and progress reporting.
)doc");

    m.def(
        "write_sidb_layout_svg_to_string",
        [](const fiction::sidb::layout&                                       layout,
           const decltype(write_sidb_layout_svg_options{}.on_progress)&       on_progress,
           const decltype(write_sidb_layout_svg_options{}.lattice_point_size) lattice_point_size,
           const decltype(write_sidb_layout_svg_options{}.sidb_size)          sidb_size,
           const decltype(write_sidb_layout_svg_options{}.sidb_border_width)  sidb_border_width,
           const decltype(write_sidb_layout_svg_options{}.color_background)   color_background,
           const decltype(write_sidb_layout_svg_options{}.lattice_mode)       lattice_mode)
        {
            write_sidb_layout_svg_options params{};
            params.on_progress        = on_progress;
            params.lattice_point_size = lattice_point_size;
            params.sidb_size          = sidb_size;
            params.sidb_border_width  = sidb_border_width;
            params.color_background   = color_background;
            params.lattice_mode       = lattice_mode;
            const nb::gil_scoped_release release{};

            std::ostringstream oss;
            fiction::sidb::io::write_sidb_layout_svg(layout, oss, params);
            return oss.str();
        },
        nb::arg("layout"), nb::kw_only(), nb::arg("on_progress").none() = nb::none(),
        nb::arg("lattice_point_size") = write_sidb_layout_svg_options{}.lattice_point_size,
        nb::arg("sidb_size")          = write_sidb_layout_svg_options{}.sidb_size,
        nb::arg("sidb_border_width")  = write_sidb_layout_svg_options{}.sidb_border_width,
        nb::arg("color_background")   = write_sidb_layout_svg_options{}.color_background,
        nb::arg("lattice_mode")       = write_sidb_layout_svg_options{}.lattice_mode,
        R"doc(Return an SVG representation of the SiDB layout.
The keyword options select dot sizes, background coloring, lattice rendering, and progress reporting.
)doc");

    m.def(
        "write_sidb_layout_svg",
        [](const fiction::sidb::layout& layout, const fiction::sidb::charge_distribution& charge_distribution,
           const std::string_view&                                                              filename,
           const decltype(fiction::sidb::io::write_sidb_layout_svg_params{}.on_progress)&       on_progress,
           const decltype(fiction::sidb::io::write_sidb_layout_svg_params{}.lattice_point_size) lattice_point_size,
           const decltype(fiction::sidb::io::write_sidb_layout_svg_params{}.sidb_size)          sidb_size,
           const decltype(fiction::sidb::io::write_sidb_layout_svg_params{}.sidb_border_width)  sidb_border_width,
           const decltype(fiction::sidb::io::write_sidb_layout_svg_params{}.color_background)   color_background,
           const decltype(fiction::sidb::io::write_sidb_layout_svg_params{}.lattice_mode)       lattice_mode)
        {
            fiction::sidb::io::write_sidb_layout_svg_params ps{};
            ps.on_progress        = on_progress;
            ps.lattice_point_size = lattice_point_size;
            ps.sidb_size          = sidb_size;
            ps.sidb_border_width  = sidb_border_width;
            ps.color_background   = color_background;
            ps.lattice_mode       = lattice_mode;
            const nb::gil_scoped_release release{};
            (static_cast<void (*)(const fiction::sidb::layout&, const fiction::sidb::charge_distribution&,
                                  const std::string_view&, const write_sidb_layout_svg_options&)>(
                &fiction::sidb::io::write_sidb_layout_svg))(layout, charge_distribution, filename, ps);
        },
        nb::arg("layout"), nb::arg("charge_distribution"), nb::arg("filename"), nb::kw_only(),
        nb::arg("on_progress").none() = nb::none(),
        nb::arg("lattice_point_size") = fiction::sidb::io::write_sidb_layout_svg_params{}.lattice_point_size,
        nb::arg("sidb_size")          = fiction::sidb::io::write_sidb_layout_svg_params{}.sidb_size,
        nb::arg("sidb_border_width")  = fiction::sidb::io::write_sidb_layout_svg_params{}.sidb_border_width,
        nb::arg("color_background")   = fiction::sidb::io::write_sidb_layout_svg_params{}.color_background,
        nb::arg("lattice_mode")       = fiction::sidb::io::write_sidb_layout_svg_params{}.lattice_mode,
        R"doc(Write an SVG representation of the SiDB layout to the supplied filename.
The keyword options select dot sizes, background coloring, lattice rendering, and progress reporting.
)doc");

    m.def(
        "write_sidb_layout_svg_to_string",
        [](const fiction::sidb::layout& layout, const fiction::sidb::charge_distribution& cd,
           const decltype(write_sidb_layout_svg_options{}.on_progress)&       on_progress,
           const decltype(write_sidb_layout_svg_options{}.lattice_point_size) lattice_point_size,
           const decltype(write_sidb_layout_svg_options{}.sidb_size)          sidb_size,
           const decltype(write_sidb_layout_svg_options{}.sidb_border_width)  sidb_border_width,
           const decltype(write_sidb_layout_svg_options{}.color_background)   color_background,
           const decltype(write_sidb_layout_svg_options{}.lattice_mode)       lattice_mode)
        {
            write_sidb_layout_svg_options params{};
            params.on_progress        = on_progress;
            params.lattice_point_size = lattice_point_size;
            params.sidb_size          = sidb_size;
            params.sidb_border_width  = sidb_border_width;
            params.color_background   = color_background;
            params.lattice_mode       = lattice_mode;
            const nb::gil_scoped_release release{};

            std::ostringstream oss;
            fiction::sidb::io::write_sidb_layout_svg(layout, cd, oss, params);
            return oss.str();
        },
        nb::arg("layout"), nb::arg("charge_distribution"), nb::kw_only(), nb::arg("on_progress").none() = nb::none(),
        nb::arg("lattice_point_size") = write_sidb_layout_svg_options{}.lattice_point_size,
        nb::arg("sidb_size")          = write_sidb_layout_svg_options{}.sidb_size,
        nb::arg("sidb_border_width")  = write_sidb_layout_svg_options{}.sidb_border_width,
        nb::arg("color_background")   = write_sidb_layout_svg_options{}.color_background,
        nb::arg("lattice_mode")       = write_sidb_layout_svg_options{}.lattice_mode,
        R"doc(Return an SVG representation of the SiDB layout.
The keyword options select dot sizes, background coloring, lattice rendering, and progress reporting.
)doc");
}

}  // namespace pyfiction
