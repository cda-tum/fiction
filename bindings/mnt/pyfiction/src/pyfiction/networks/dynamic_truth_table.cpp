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
 * @brief Python bindings for `kitty`'s dynamic truth tables.
 * @author Marcel Walter (marcelwa)
 */

#include "pyfiction/types.hpp"

#include <cstdint>

#include <nanobind/nanobind.h>

namespace pyfiction
{

void dynamic_truth_table(nanobind::module_& m)
{
    namespace py = nanobind;

    py::class_<py_tt>(m, "dynamic_truth_table")
        .def(py::init<>(), "Default constructor. Constructs a truth table of 0 variables.")
        .def(py::init<uint32_t>(), py::arg("num_vars"), "Constructs a truth table of the given number of variables.")

        .def("num_vars", &py_tt::num_vars)
        .def("num_blocks", &py_tt::num_blocks)
        .def("num_bits", &py_tt::num_bits)

        ;
}

}  // namespace pyfiction
