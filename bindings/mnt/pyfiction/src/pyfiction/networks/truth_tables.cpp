//
// Created by marcel on 21.11.23.
//

#include "pyfiction/types.hpp"

#include <cstdint>

#include <nanobind/nanobind.h>
#include <nanobind/stl/array.h>
#include <nanobind/stl/function.h>
#include <nanobind/stl/map.h>
#include <nanobind/stl/optional.h>
#include <nanobind/stl/pair.h>
#include <nanobind/stl/set.h>
#include <nanobind/stl/shared_ptr.h>
#include <nanobind/stl/tuple.h>
#include <nanobind/stl/unordered_map.h>
#include <nanobind/stl/unordered_set.h>
#include <nanobind/stl/vector.h>

namespace pyfiction
{

void truth_tables(nanobind::module_& m)
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
