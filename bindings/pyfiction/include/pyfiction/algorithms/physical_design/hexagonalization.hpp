//
// Created by marcel on 19.09.23.
//

#ifndef PYFICTION_HEXAGONALIZATION_HPP
#define PYFICTION_HEXAGONALIZATION_HPP

#include "pyfiction/documentation.hpp"
#include "pyfiction/types.hpp"

#include <fiction/algorithms/physical_design/hexagonalization.hpp>

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace pyfiction
{

/**
 * Cartesian to hexagonal gate-level layout transformation.
 */
inline void hexagonalization(pybind11::module& m)
{
    using namespace pybind11::literals;

    m.def("hexagonalization", &fiction::hexagonalization<py_hexagonal_gate_layout, py_cartesian_gate_layout>, "lyt"_a);
    //, DOC(fiction_hexagonalization));
}

}  // namespace pyfiction

#endif  // PYFICTION_HEXAGONALIZATION_HPP
