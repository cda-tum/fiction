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
#include "pyfiction/types.hpp"

#include <fiction/technology/sidb/charge_distribution.hpp>
#include <fiction/technology/sidb/io/write_sidb_layout_svg.hpp>
#include <fiction/technology/sidb/layout.hpp>

#include <sstream>
#include <string>
#include <string_view>

#include <nanobind/nanobind.h>
#include <nanobind/stl/array.h>          // NOLINT(misc-include-cleaner)
#include <nanobind/stl/optional.h>       // NOLINT(misc-include-cleaner)
#include <nanobind/stl/pair.h>           // NOLINT(misc-include-cleaner)
#include <nanobind/stl/string.h>         // NOLINT(misc-include-cleaner)
#include <nanobind/stl/string_view.h>    // NOLINT(misc-include-cleaner)
#include <nanobind/stl/tuple.h>          // NOLINT(misc-include-cleaner)
#include <nanobind/stl/unordered_map.h>  // NOLINT(misc-include-cleaner)
#include <nanobind/stl/vector.h>         // NOLINT(misc-include-cleaner)

namespace pyfiction
{

/**
 * @brief Registers SVG export and its rendering parameters.
 *
 * @param m Python module.
 */
void write_sidb_layout_svg(nanobind::module_& m)
{
    namespace py = nanobind;

    py::enum_<fiction::sidb::io::write_sidb_layout_svg_params::color_mode>(
        m, "color_mode", DOC(fiction_sidb_io_write_sidb_layout_svg_params_color_mode))
        .value("LIGHT", fiction::sidb::io::write_sidb_layout_svg_params::color_mode::LIGHT,
               DOC(fiction_sidb_io_write_sidb_layout_svg_params_color_mode_LIGHT))
        .value("DARK", fiction::sidb::io::write_sidb_layout_svg_params::color_mode::DARK,
               DOC(fiction_sidb_io_write_sidb_layout_svg_params_color_mode_DARK));

    py::enum_<fiction::sidb::io::write_sidb_layout_svg_params::sidb_lattice_mode>(
        m, "sidb_lattice_mode", DOC(fiction_sidb_io_write_sidb_layout_svg_params_sidb_lattice_mode))
        .value("SHOW_LATTICE", fiction::sidb::io::write_sidb_layout_svg_params::sidb_lattice_mode::SHOW_LATTICE,
               DOC(fiction_sidb_io_write_sidb_layout_svg_params_sidb_lattice_mode_SHOW_LATTICE))
        .value("HIDE_LATTICE", fiction::sidb::io::write_sidb_layout_svg_params::sidb_lattice_mode::HIDE_LATTICE,
               DOC(fiction_sidb_io_write_sidb_layout_svg_params_sidb_lattice_mode_HIDE_LATTICE));

    py::class_<fiction::sidb::io::write_sidb_layout_svg_params>(m, "write_sidb_layout_svg_params",
                                                                DOC(fiction_sidb_io_write_sidb_layout_svg_params))
        .def(py::init<>(), "Default constructor.")
        .def_rw("lattice_point_size", &fiction::sidb::io::write_sidb_layout_svg_params::lattice_point_size,
                DOC(fiction_sidb_io_write_sidb_layout_svg_params_lattice_point_size))
        .def_rw("sidb_size", &fiction::sidb::io::write_sidb_layout_svg_params::sidb_size,
                DOC(fiction_sidb_io_write_sidb_layout_svg_params_sidb_size))
        .def_rw("sidb_border_width", &fiction::sidb::io::write_sidb_layout_svg_params::sidb_border_width,
                DOC(fiction_sidb_io_write_sidb_layout_svg_params_sidb_border_width))
        .def_rw("color_background", &fiction::sidb::io::write_sidb_layout_svg_params::color_background,
                DOC(fiction_sidb_io_write_sidb_layout_svg_params_color_background))
        .def_rw("lattice_mode", &fiction::sidb::io::write_sidb_layout_svg_params::lattice_mode,
                DOC(fiction_sidb_io_write_sidb_layout_svg_params_lattice_mode))

        ;

    m.def("write_sidb_layout_svg",
          static_cast<void (*)(const fiction::sidb::layout&, const std::string_view&,
                               const fiction::sidb::io::write_sidb_layout_svg_params&)>(
              &fiction::sidb::io::write_sidb_layout_svg),
          py::arg("layout"), py::arg("filename"),
          py::arg("ps") = fiction::sidb::io::write_sidb_layout_svg_params{},
          DOC(fiction_sidb_io_write_sidb_layout_svg_2));

    m.def(
        "write_sidb_layout_svg_to_string",
        [](const fiction::sidb::layout& layout, const fiction::sidb::io::write_sidb_layout_svg_params& params)
        {
            std::ostringstream oss;
            fiction::sidb::io::write_sidb_layout_svg(layout, oss, params);
            return oss.str();
        },
        py::arg("layout"), py::arg("ps") = fiction::sidb::io::write_sidb_layout_svg_params{},
        DOC(fiction_sidb_io_write_sidb_layout_svg));

    m.def("write_sidb_layout_svg",
          static_cast<void (*)(const fiction::sidb::layout&, const fiction::sidb::charge_distribution&,
                               const std::string_view&, const fiction::sidb::io::write_sidb_layout_svg_params&)>(
              &fiction::sidb::io::write_sidb_layout_svg),
          py::arg("layout"), py::arg("charge_distribution"),
          py::arg("filename"), py::arg("ps") = fiction::sidb::io::write_sidb_layout_svg_params{},
          DOC(fiction_sidb_io_write_sidb_layout_svg_4));

    m.def(
        "write_sidb_layout_svg_to_string",
        [](const fiction::sidb::layout& layout, const fiction::sidb::charge_distribution& cd,
           const fiction::sidb::io::write_sidb_layout_svg_params& params)
        {
            std::ostringstream oss;
            fiction::sidb::io::write_sidb_layout_svg(layout, cd, oss, params);
            return oss.str();
        },
        py::arg("layout"), py::arg("charge_distribution"),
        py::arg("ps") = fiction::sidb::io::write_sidb_layout_svg_params{},
        DOC(fiction_sidb_io_write_sidb_layout_svg_3));
}

}  // namespace pyfiction
