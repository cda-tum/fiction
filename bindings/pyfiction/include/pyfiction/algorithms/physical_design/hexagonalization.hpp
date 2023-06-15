//
// Created by marcel on 15.12.22.
//

#ifndef PYFICTION_HEXAGONALIZATION_HPP
#define PYFICTION_HEXAGONALIZATION_HPP

#include "pyfiction/documentation.hpp"

#include <fiction/types.hpp>
#include <fiction/algorithms/physical_design/hexagonalization.hpp>

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
        "hexagonalization",
        [](const Lyt& lyt) -> fiction::hex_even_row_gate_clk_lyt
        {
            return fiction::hexagonalization<Lyt>(lyt);
        },
        "layout"_a, DOC(fiction_hexagonalization));
}

}  // namespace detail

inline void hexagonalization(pybind11::module& m)
{
    detail::hexagonalization<py_cartesian_gate_layout>(m);
}

}  // namespace pyfiction

#endif  // PYFICTION_HEXAGONALIZATION_HPP
