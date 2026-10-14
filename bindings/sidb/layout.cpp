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
 * @brief Python bindings for `fiction/technology/sidb/layout.hpp`.
 * @author Marcel Walter (marcelwa)
 */

#include "pyfiction/documentation.hpp"

#include <fiction/technology/sidb/io/print_sidb_layout.hpp>
#include <fiction/technology/sidb/lattice.hpp>
#include <fiction/technology/sidb/layout.hpp>

#include <functional>
#include <optional>
#include <sstream>
#include <string>

#include <nanobind/nanobind.h>
#include <nanobind/operators.h>
#include <nanobind/stl/optional.h>       // NOLINT(misc-include-cleaner)
#include <nanobind/stl/pair.h>           // NOLINT(misc-include-cleaner)
#include <nanobind/stl/string.h>         // NOLINT(misc-include-cleaner)
#include <nanobind/stl/unordered_set.h>  // NOLINT(misc-include-cleaner)
#include <nanobind/stl/vector.h>         // NOLINT(misc-include-cleaner)

/**
 * @brief Namespace alias for nanobind.
 */
namespace nb = nanobind;

namespace pyfiction
{

/**
 * @brief Registers SiDB layouts and their dot and defect operations.
 *
 * @param m Python module.
 */
void register_layout(nb::module_& m)
{
    // Keep the GIL for scalar queries, binary searches, and views converted through the Python API.
    using fiction::sidb::layout;

    nb::enum_<fiction::sidb::dot_tag>(m, "sidb_dot_tag", DOC(fiction_sidb_dot_tag))
        .value("EMPTY", fiction::sidb::dot_tag::EMPTY, DOC(fiction_sidb_dot_tag_EMPTY))
        .value("NORMAL", fiction::sidb::dot_tag::NORMAL, DOC(fiction_sidb_dot_tag_NORMAL))
        .value("INPUT", fiction::sidb::dot_tag::INPUT, DOC(fiction_sidb_dot_tag_INPUT))
        .value("OUTPUT", fiction::sidb::dot_tag::OUTPUT, DOC(fiction_sidb_dot_tag_OUTPUT))
        .value("LOGIC", fiction::sidb::dot_tag::LOGIC, DOC(fiction_sidb_dot_tag_LOGIC));

    nb::class_<layout>(m, "sidb_layout", DOC(fiction_sidb_layout))
        .def(nb::init<>(), DOC(fiction_sidb_layout_layout))
        .def(nb::init<const fiction::sidb::lattice&, std::string>(), nb::arg("lattice"), nb::arg("name") = "",
             DOC(fiction_sidb_layout_layout_2))
        .def("get_lattice", &layout::get_lattice, DOC(fiction_sidb_layout_get_lattice))
        .def("set_lattice", &layout::set_lattice, nb::arg("lattice"), DOC(fiction_sidb_layout_set_lattice))
        .def("get_layout_name", &layout::get_layout_name, DOC(fiction_sidb_layout_get_layout_name))
        .def("set_layout_name", &layout::set_layout_name, nb::arg("name"), DOC(fiction_sidb_layout_set_layout_name))

        // Keep the GIL for individual site updates; a release adds overhead to these common mutations.
        .def("assign_sidb", &layout::assign_sidb, nb::arg("site"), nb::arg("dot_tag") = fiction::sidb::dot_tag::NORMAL,
             DOC(fiction_sidb_layout_assign_sidb))
        .def("get_dot_tag", &layout::get_dot_tag, nb::arg("site"), DOC(fiction_sidb_layout_get_dot_tag))
        .def("is_empty_site", &layout::is_empty_site, nb::arg("site"), DOC(fiction_sidb_layout_is_empty_site))
        .def("is_empty", &layout::is_empty, DOC(fiction_sidb_layout_is_empty))
        .def("num_dots", &layout::num_dots, DOC(fiction_sidb_layout_num_dots))
        .def("num_dots_with_tag", &layout::num_dots_with_tag, nb::call_guard<nb::gil_scoped_release>(),
             nb::arg("dot_tag"), DOC(fiction_sidb_layout_num_dots_with_tag))
        .def("dots_with_tag", &layout::dots_with_tag, nb::call_guard<nb::gil_scoped_release>(), nb::arg("dot_tag"),
             DOC(fiction_sidb_layout_dots_with_tag))
        .def("sidbs", &layout::sidbs, DOC(fiction_sidb_layout_sidbs))
        .def("index_of", &layout::index_of, nb::arg("site"), DOC(fiction_sidb_layout_index_of))
        .def("num_pis", &layout::num_pis, nb::call_guard<nb::gil_scoped_release>(), DOC(fiction_sidb_layout_num_pis))
        .def("num_pos", &layout::num_pos, nb::call_guard<nb::gil_scoped_release>(), DOC(fiction_sidb_layout_num_pos))
        .def("is_pi", &layout::is_pi, nb::arg("site"), DOC(fiction_sidb_layout_is_pi))
        .def("is_po", &layout::is_po, nb::arg("site"), DOC(fiction_sidb_layout_is_po))
        .def(
            "pis", [](const layout& lyt) { return lyt.dots_with_tag(fiction::sidb::dot_tag::INPUT); },
            nb::call_guard<nb::gil_scoped_release>(), "Returns the sites of all input SiDBs.")
        .def(
            "pos", [](const layout& lyt) { return lyt.dots_with_tag(fiction::sidb::dot_tag::OUTPUT); },
            nb::call_guard<nb::gil_scoped_release>(), "Returns the sites of all output SiDBs.")

        // Keep individual defect updates under the GIL, as with individual dot updates.
        .def("assign_defect", &layout::assign_defect, nb::arg("site"), nb::arg("defect"),
             DOC(fiction_sidb_layout_assign_defect))
        .def("move_defect", &layout::move_defect, nb::arg("source"), nb::arg("target"),
             DOC(fiction_sidb_layout_move_defect))
        .def("get_defect", &layout::get_defect, nb::arg("site"), DOC(fiction_sidb_layout_get_defect))
        .def("num_defects", &layout::num_defects, DOC(fiction_sidb_layout_num_defects))
        .def("num_positively_charged_defects", &layout::num_positively_charged_defects,
             nb::call_guard<nb::gil_scoped_release>(), DOC(fiction_sidb_layout_num_positively_charged_defects))
        .def("num_negatively_charged_defects", &layout::num_negatively_charged_defects,
             nb::call_guard<nb::gil_scoped_release>(), DOC(fiction_sidb_layout_num_negatively_charged_defects))
        .def("num_charged_defects", &layout::num_charged_defects, nb::call_guard<nb::gil_scoped_release>(),
             DOC(fiction_sidb_layout_num_charged_defects))
        .def("num_neutral_defects", &layout::num_neutral_defects, nb::call_guard<nb::gil_scoped_release>(),
             DOC(fiction_sidb_layout_num_neutral_defects))
        .def("defects", &layout::defects, DOC(fiction_sidb_layout_defects))
        .def("affected_sidbs", &layout::affected_sidbs, nb::call_guard<nb::gil_scoped_release>(), nb::arg("site"),
             nb::arg("charged_defect_spacing_overwrite") = std::nullopt,
             nb::arg("neutral_defect_spacing_overwrite") = std::nullopt, DOC(fiction_sidb_layout_affected_sidbs))
        .def("all_affected_sidbs", &layout::all_affected_sidbs, nb::call_guard<nb::gil_scoped_release>(),
             nb::arg("charged_defect_spacing_overwrite") = std::nullopt,
             nb::arg("neutral_defect_spacing_overwrite") = std::nullopt, DOC(fiction_sidb_layout_all_affected_sidbs))

        .def("bounding_box", &layout::bounding_box, nb::call_guard<nb::gil_scoped_release>(),
             DOC(fiction_sidb_layout_bounding_box))
        // NOLINTBEGIN(misc-redundant-expression): nanobind operator bindings intentionally compare placeholder objects.
        .def(nb::self == nb::self, nb::call_guard<nb::gil_scoped_release>(), DOC(fiction_sidb_layout_operator_eq))
        .def(nb::self != nb::self, nb::call_guard<nb::gil_scoped_release>())
        // NOLINTEND(misc-redundant-expression)
        .def(
            "__hash__", [](const layout& lyt) { return std::hash<layout>{}(lyt); },
            nb::call_guard<nb::gil_scoped_release>())
        .def(
            "__repr__",
            [](const layout& lyt)
            {
                std::stringstream stream{};
                fiction::sidb::io::print_sidb_layout(stream, lyt, false);
                return stream.str();
            },
            nb::call_guard<nb::gil_scoped_release>())

        ;
}

}  // namespace pyfiction
