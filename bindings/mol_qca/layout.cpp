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
 * @brief Python bindings for `fiction/technology/mol_qca/layout.hpp`.
 * @author Marcel Walter (marcelwa)
 */

#include "pyfiction/cell_grid.hpp"
#include "pyfiction/documentation.hpp"
#include "pyfiction/types.hpp"

#include <fiction/technology/mol_qca/layout.hpp>

#include <string>

#include <nanobind/nanobind.h>
#include <nanobind/stl/string.h>  // NOLINT(misc-include-cleaner)

namespace pyfiction
{

/**
 * @brief Registers molQCA cell types and layouts.
 *
 * @param m Python module.
 */
void mol_qca_layout(nanobind::module_& m)
{
    namespace py = nanobind;

    using fiction::mol_qca::cell_type;
    using fiction::mol_qca::layout;

    py::enum_<cell_type>(m, "mol_qca_cell_type", DOC(fiction_mol_qca_cell_type))
        .value("EMPTY", cell_type::EMPTY, DOC(fiction_mol_qca_cell_type_EMPTY))
        .value("NORMAL1", cell_type::NORMAL1, DOC(fiction_mol_qca_cell_type_NORMAL1))
        .value("NORMAL2", cell_type::NORMAL2, DOC(fiction_mol_qca_cell_type_NORMAL2))
        .value("NORMAL3", cell_type::NORMAL3, DOC(fiction_mol_qca_cell_type_NORMAL3))
        .value("NORMAL4", cell_type::NORMAL4, DOC(fiction_mol_qca_cell_type_NORMAL4))
        .value("INPUT", cell_type::INPUT, DOC(fiction_mol_qca_cell_type_INPUT))
        .value("OUTPUT", cell_type::OUTPUT, DOC(fiction_mol_qca_cell_type_OUTPUT))
        .value("CONST_0", cell_type::CONST_0, DOC(fiction_mol_qca_cell_type_CONST_0))
        .value("CONST_1", cell_type::CONST_1, DOC(fiction_mol_qca_cell_type_CONST_1));

    m.def("mol_qca_clock_number", &fiction::mol_qca::clock_number, py::arg("ct"), DOC(fiction_mol_qca_clock_number));

    py::class_<layout, py_cartesian_layout> cls(m, "mol_qca_layout", DOC(fiction_mol_qca_layout));

    cls.def(py::init<>())
        .def(py::init<const layout::aspect_ratio&, std::string>(), py::arg("dimension"), py::arg("layout_name") = "",
             DOC(fiction_mol_qca_layout_layout));

    detail::bind_cell_grid(cls);

    cls.def("assign_cell_type", &layout::assign_cell_type, py::arg("c"), py::arg("ct"),
            DOC(fiction_layouts_cell_grid_assign_cell_type));
}

}  // namespace pyfiction
