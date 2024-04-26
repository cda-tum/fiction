//
// Created by marcel on 21.11.23.
//

#ifndef PYFICTION_TRUTH_TABLES_HPP
#define PYFICTION_TRUTH_TABLES_HPP

#include "pyfiction/types.hpp"

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace pyfiction
{

inline void truth_tables(pybind11::module& m)
{
    namespace py = pybind11;

    py::class_<py_tt>(m, "dynamic_truth_table")
        .def(py::init<>())
        .def(py::init<uint32_t>())

        .def("num_vars", &py_tt::num_vars)
        .def("num_blocks", &py_tt::num_blocks)
        .def("num_bits", &py_tt::num_bits)

        ;
}

}  // namespace pyfiction

#endif  // PYFICTION_TRUTH_TABLES_HPP
