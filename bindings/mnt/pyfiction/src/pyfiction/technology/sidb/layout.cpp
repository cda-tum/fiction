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
 * @brief Registers SiDB layouts and their cell and defect operations.
 *
 * @param m Python module.
 */
void layout(nanobind::module_& m)
{
    namespace py = nanobind;

    using fiction::sidb::layout;

    /**
     * SiDB cell technology.
     */
    const py::class_<fiction::sidb::sidb_technology> tech(m, "sidb_technology", DOC(fiction_sidb_sidb_technology));

    py::enum_<fiction::sidb::sidb_technology::cell_type>(tech, "cell_type", DOC(fiction_sidb_sidb_technology_cell_type))
        .value("EMPTY", fiction::sidb::sidb_technology::cell_type::EMPTY,
               DOC(fiction_sidb_sidb_technology_cell_type_EMPTY))
        .value("NORMAL", fiction::sidb::sidb_technology::cell_type::NORMAL,
               DOC(fiction_sidb_sidb_technology_cell_type_NORMAL))
        .value("INPUT", fiction::sidb::sidb_technology::cell_type::INPUT,
               DOC(fiction_sidb_sidb_technology_cell_type_INPUT))
        .value("OUTPUT", fiction::sidb::sidb_technology::cell_type::OUTPUT,
               DOC(fiction_sidb_sidb_technology_cell_type_OUTPUT))
        .value("LOGIC", fiction::sidb::sidb_technology::cell_type::LOGIC,
               DOC(fiction_sidb_sidb_technology_cell_type_LOGIC));

    py::class_<layout>(m, "sidb_layout", DOC(fiction_sidb_layout))
        .def(py::init<>(), DOC(fiction_sidb_layout_layout))
        .def(py::init<const fiction::sidb::lattice&, std::string>(), py::arg("lattice"), py::arg("name") = "",
             DOC(fiction_sidb_layout_layout_2))
        .def("get_lattice", &layout::get_lattice, DOC(fiction_sidb_layout_get_lattice))
        .def("set_lattice", &layout::set_lattice, py::arg("lattice"), DOC(fiction_sidb_layout_set_lattice))
        .def("get_layout_name", &layout::get_layout_name, DOC(fiction_sidb_layout_get_layout_name))
        .def("set_layout_name", &layout::set_layout_name, py::arg("name"), DOC(fiction_sidb_layout_set_layout_name))

        .def("assign_cell_type", &layout::assign_cell_type, py::arg("site"), py::arg("cell_type"),
             DOC(fiction_sidb_layout_assign_cell_type))
        .def("get_cell_type", &layout::get_cell_type, py::arg("site"), DOC(fiction_sidb_layout_get_cell_type))
        .def("is_empty_cell", &layout::is_empty_cell, py::arg("site"), DOC(fiction_sidb_layout_is_empty_cell))
        .def("is_empty", &layout::is_empty, DOC(fiction_sidb_layout_is_empty))
        .def("num_cells", &layout::num_cells, DOC(fiction_sidb_layout_num_cells))
        .def("num_cells_of_type", &layout::num_cells_of_type, py::arg("cell_type"),
             DOC(fiction_sidb_layout_num_cells_of_type))
        .def("cells_of_type", &layout::cells_of_type, py::arg("cell_type"), DOC(fiction_sidb_layout_cells_of_type))
        .def("sidbs", &layout::sidbs, DOC(fiction_sidb_layout_sidbs))
        .def("index_of", &layout::index_of, py::arg("site"), DOC(fiction_sidb_layout_index_of))
        .def("num_pis", &layout::num_pis, DOC(fiction_sidb_layout_num_pis))
        .def("num_pos", &layout::num_pos, DOC(fiction_sidb_layout_num_pos))
        .def("is_pi", &layout::is_pi, py::arg("site"), DOC(fiction_sidb_layout_is_pi))
        .def("is_po", &layout::is_po, py::arg("site"), DOC(fiction_sidb_layout_is_po))
        .def(
            "pis",
            [](const layout& lyt) { return lyt.cells_of_type(fiction::sidb::sidb_technology::cell_type::INPUT); },
            DOC(fiction_sidb_layout_foreach_pi))
        .def(
            "pos",
            [](const layout& lyt) { return lyt.cells_of_type(fiction::sidb::sidb_technology::cell_type::OUTPUT); },
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
