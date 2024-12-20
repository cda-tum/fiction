//
// Created by marcel on 21.11.23.
//

#ifndef PYFICTION_CONVERT_POTENTIAL_TO_DISTANCE_HPP
#define PYFICTION_CONVERT_POTENTIAL_TO_DISTANCE_HPP

#include "pyfiction/documentation.hpp"
#include "pyfiction/types.hpp"

#include <fiction/algorithms/simulation/sidb/potential_to_distance_conversion.hpp>

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace pyfiction
{

inline void convert_potential_to_distance(pybind11::module& m)
{
    namespace py = pybind11;

    m.def("convert_potential_to_distance", &fiction::convert_potential_to_distance, py::arg("potential"),
          py::arg("params"), py::arg("precision"), DOC(fiction_convert_potential_to_distance));
}

}  // namespace pyfiction

#endif  // PYFICTION_CONVERT_POTENTIAL_TO_DISTANCE_HPP
