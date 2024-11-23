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
    using namespace pybind11::literals;

    m.def(
        "hexagonalization", [](const Lyt& lyt) -> py_hexagonal_gate_layout
        { return fiction::hexagonalization<py_hexagonal_gate_layout, Lyt>(lyt); }, "layout"_a,
        DOC(fiction_hexagonalization));
}

}  // namespace detail

inline void hexagonalization(pybind11::module& m)
{
    detail::hexagonalization<py_cartesian_gate_layout>(m);
}

}  // namespace pyfiction

#endif  // PYFICTION_HEXAGONALIZATION_HPP
