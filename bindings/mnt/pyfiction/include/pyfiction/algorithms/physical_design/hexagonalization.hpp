//
// Created by simon on 22.09.23.
//

#ifndef PYFICTION_HEXAGONALIZATION_HPP
#define PYFICTION_HEXAGONALIZATION_HPP

#include "pyfiction/documentation.hpp"

#include <fiction/algorithms/physical_design/hexagonalization.hpp>
#include <fiction/types.hpp>

#include <pybind11/pybind11.h>

namespace pyfiction
{

namespace detail
{

template <typename Lyt>
void hexagonalization(pybind11::module& m)
{
    namespace py = pybind11;

    m.def(
        "hexagonalization", [](const Lyt& lyt) -> py_hexagonal_gate_layout
        { return fiction::hexagonalization<py_hexagonal_gate_layout, Lyt>(lyt); }, py::arg("layout"),
        DOC(fiction_hexagonalization));
}

}  // namespace detail

inline void hexagonalization(pybind11::module& m)
{
    detail::hexagonalization<py_cartesian_gate_layout<py_offset_coordinate>>(m);
}

}  // namespace pyfiction

#endif  // PYFICTION_HEXAGONALIZATION_HPP
