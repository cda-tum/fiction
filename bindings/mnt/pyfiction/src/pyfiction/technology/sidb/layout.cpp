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
#include <fiction/technology/sidb/technology.hpp>

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

namespace pyfiction
{

/**
 * @brief Registers SiDB layouts and their dot and defect operations.
 *
 * @param m Python module.
 */
void layout(nanobind::module_& m)
{
    namespace py = nanobind;

    using fiction::sidb::layout;

    /**
     * Cartesian SiDB technology bridge.
     */
    const py::class_<fiction::sidb::sidb_technology> tech(m, "sidb_technology", DOC(fiction_sidb_sidb_technology));

    const auto tags = py::enum_<fiction::sidb::dot_tag>(m, "sidb_dot_tag", DOC(fiction_sidb_dot_tag))
                          .value("EMPTY", fiction::sidb::dot_tag::EMPTY, DOC(fiction_sidb_dot_tag_EMPTY))
                          .value("NORMAL", fiction::sidb::dot_tag::NORMAL, DOC(fiction_sidb_dot_tag_NORMAL))
                          .value("INPUT", fiction::sidb::dot_tag::INPUT, DOC(fiction_sidb_dot_tag_INPUT))
                          .value("OUTPUT", fiction::sidb::dot_tag::OUTPUT, DOC(fiction_sidb_dot_tag_OUTPUT))
                          .value("LOGIC", fiction::sidb::dot_tag::LOGIC, DOC(fiction_sidb_dot_tag_LOGIC));

    tech.attr("cell_type") = tags;

    py::class_<layout>(m, "sidb_layout", DOC(fiction_sidb_layout))
        .def(py::init<>(), DOC(fiction_sidb_layout_layout))
        .def(py::init<const fiction::sidb::lattice&, std::string>(), py::arg("lattice"), py::arg("name") = "",
             DOC(fiction_sidb_layout_layout_2))
        .def("get_lattice", &layout::get_lattice, DOC(fiction_sidb_layout_get_lattice))
        .def("set_lattice", &layout::set_lattice, py::arg("lattice"), DOC(fiction_sidb_layout_set_lattice))
        .def("get_layout_name", &layout::get_layout_name, DOC(fiction_sidb_layout_get_layout_name))
        .def("set_layout_name", &layout::set_layout_name, py::arg("name"), DOC(fiction_sidb_layout_set_layout_name))

        .def("assign_sidb", &layout::assign_sidb, py::arg("site"), py::arg("dot_tag") = fiction::sidb::dot_tag::NORMAL,
             DOC(fiction_sidb_layout_assign_sidb))
        .def("get_dot_tag", &layout::get_dot_tag, py::arg("site"), DOC(fiction_sidb_layout_get_dot_tag))
        .def("is_empty_site", &layout::is_empty_site, py::arg("site"), DOC(fiction_sidb_layout_is_empty_site))
        .def("is_empty", &layout::is_empty, DOC(fiction_sidb_layout_is_empty))
        .def("num_dots", &layout::num_dots, DOC(fiction_sidb_layout_num_dots))
        .def("num_dots_with_tag", &layout::num_dots_with_tag, py::arg("dot_tag"),
             DOC(fiction_sidb_layout_num_dots_with_tag))
        .def("dots_with_tag", &layout::dots_with_tag, py::arg("dot_tag"), DOC(fiction_sidb_layout_dots_with_tag))
        .def("sidbs", &layout::sidbs, DOC(fiction_sidb_layout_sidbs))
        .def("index_of", &layout::index_of, py::arg("site"), DOC(fiction_sidb_layout_index_of))
        .def("num_pis", &layout::num_pis, DOC(fiction_sidb_layout_num_pis))
        .def("num_pos", &layout::num_pos, DOC(fiction_sidb_layout_num_pos))
        .def("is_pi", &layout::is_pi, py::arg("site"), DOC(fiction_sidb_layout_is_pi))
        .def("is_po", &layout::is_po, py::arg("site"), DOC(fiction_sidb_layout_is_po))
        .def(
            "pis", [](const layout& lyt) { return lyt.dots_with_tag(fiction::sidb::dot_tag::INPUT); },
            DOC(fiction_sidb_layout_foreach_pi))
        .def(
            "pos", [](const layout& lyt) { return lyt.dots_with_tag(fiction::sidb::dot_tag::OUTPUT); },
            DOC(fiction_sidb_layout_foreach_po))

        .def("assign_defect", &layout::assign_defect, py::arg("site"), py::arg("defect"),
             DOC(fiction_sidb_layout_assign_defect))
        .def("move_defect", &layout::move_defect, py::arg("source"), py::arg("target"),
             DOC(fiction_sidb_layout_move_defect))
        .def("get_defect", &layout::get_defect, py::arg("site"), DOC(fiction_sidb_layout_get_defect))
        .def("num_defects", &layout::num_defects, DOC(fiction_sidb_layout_num_defects))
        .def("num_positively_charged_defects", &layout::num_positively_charged_defects,
             DOC(fiction_sidb_layout_num_positively_charged_defects))
        .def("num_negatively_charged_defects", &layout::num_negatively_charged_defects,
             DOC(fiction_sidb_layout_num_negatively_charged_defects))
        .def("num_charged_defects", &layout::num_charged_defects, DOC(fiction_sidb_layout_num_charged_defects))
        .def("num_neutral_defects", &layout::num_neutral_defects, DOC(fiction_sidb_layout_num_neutral_defects))
        .def("defects", &layout::defects, DOC(fiction_sidb_layout_defects))
        .def("affected_sidbs", &layout::affected_sidbs, py::arg("site"),
             py::arg("charged_defect_spacing_overwrite") = std::nullopt,
             py::arg("neutral_defect_spacing_overwrite") = std::nullopt, DOC(fiction_sidb_layout_affected_sidbs))
        .def("all_affected_sidbs", &layout::all_affected_sidbs,
             py::arg("charged_defect_spacing_overwrite") = std::nullopt,
             py::arg("neutral_defect_spacing_overwrite") = std::nullopt, DOC(fiction_sidb_layout_all_affected_sidbs))

        .def("bounding_box", &layout::bounding_box, DOC(fiction_sidb_layout_bounding_box))
        // NOLINTBEGIN(misc-redundant-expression): nanobind operator bindings intentionally compare placeholder objects.
        .def(py::self == py::self, DOC(fiction_sidb_layout_operator_eq))
        .def(py::self != py::self)
        // NOLINTEND(misc-redundant-expression)
        .def("__hash__", [](const layout& lyt) { return std::hash<layout>{}(lyt); })
        .def("__repr__",
             [](const layout& lyt)
             {
                 std::stringstream stream{};
                 fiction::sidb::io::print_sidb_layout(stream, lyt, false);
                 return stream.str();
             })

        ;
}

}  // namespace pyfiction
