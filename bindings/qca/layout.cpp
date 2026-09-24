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
 * @brief Python bindings for `fiction/technology/qca/layout.hpp`.
 * @author Marcel Walter (marcelwa)
 */

#include "pyfiction/cell_grid.hpp"
#include "pyfiction/documentation.hpp"
#include "pyfiction/types.hpp"

#include <fiction/technology/qca/layout.hpp>

#include <nanobind/nanobind.h>

namespace pyfiction
{

/**
 * @brief Registers QCA cell types, cell modes, and layouts.
 *
 * @param m Python module.
 */
void qca_layout(nanobind::module_& m)
{
    namespace py = nanobind;

    using fiction::qca::cell_mode;
    using fiction::qca::cell_type;
    using fiction::qca::layout;

    py::enum_<cell_type>(m, "qca_cell_type", DOC(fiction_qca_cell_type))
        .value("EMPTY", cell_type::EMPTY, DOC(fiction_qca_cell_type_EMPTY))
        .value("NORMAL", cell_type::NORMAL, DOC(fiction_qca_cell_type_NORMAL))
        .value("INPUT", cell_type::INPUT, DOC(fiction_qca_cell_type_INPUT))
        .value("OUTPUT", cell_type::OUTPUT, DOC(fiction_qca_cell_type_OUTPUT))
        .value("CONST_0", cell_type::CONST_0, DOC(fiction_qca_cell_type_CONST_0))
        .value("CONST_1", cell_type::CONST_1, DOC(fiction_qca_cell_type_CONST_1));

    py::enum_<cell_mode>(m, "qca_cell_mode", DOC(fiction_qca_cell_mode))
        .value("NORMAL", cell_mode::NORMAL, DOC(fiction_qca_cell_mode_NORMAL))
        .value("ROTATED", cell_mode::ROTATED, DOC(fiction_qca_cell_mode_ROTATED))
        .value("VERTICAL", cell_mode::VERTICAL, DOC(fiction_qca_cell_mode_VERTICAL))
        .value("CROSSOVER", cell_mode::CROSSOVER, DOC(fiction_qca_cell_mode_CROSSOVER));

    py::class_<layout, py_cartesian_layout> cls(m, "qca_layout", DOC(fiction_qca_layout));

    detail::bind_clocked_constructors(cls);
    detail::bind_cell_grid(cls);
    detail::bind_tile_clocking(cls);

    cls.def("assign_cell_type", &layout::assign_cell_type, py::arg("c"), py::arg("ct"),
            DOC(fiction_qca_layout_assign_cell_type))
        .def("assign_cell_mode", &layout::assign_cell_mode, py::arg("c"), py::arg("m"),
             DOC(fiction_qca_layout_assign_cell_mode))
        .def("get_cell_mode", &layout::get_cell_mode, py::arg("c"), DOC(fiction_qca_layout_get_cell_mode))
        .def("assign_synchronization_element", &layout::assign_synchronization_element, py::arg("cz"), py::arg("se"),
             DOC(fiction_qca_layout_assign_synchronization_element))
        .def("is_synchronization_element", &layout::is_synchronization_element, py::arg("c"),
             DOC(fiction_qca_layout_is_synchronization_element))
        .def("get_synchronization_element", &layout::get_synchronization_element, py::arg("c"),
             DOC(fiction_qca_layout_get_synchronization_element))
        .def("num_se", &layout::num_se, DOC(fiction_qca_layout_num_se));
}

}  // namespace pyfiction
