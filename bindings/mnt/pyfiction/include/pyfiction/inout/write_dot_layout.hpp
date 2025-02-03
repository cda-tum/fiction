//
// Created by marcel on 16.12.22.
//

#ifndef PYFICTION_WRITE_DOT_LAYOUT_HPP
#define PYFICTION_WRITE_DOT_LAYOUT_HPP

#include "pyfiction/documentation.hpp"
#include "pyfiction/types.hpp"

#include <fiction/io/dot_drawers.hpp>
#include <fiction/traits.hpp>

#include <mockturtle/io/write_dot.hpp>
#include <pybind11/pybind11.h>

#include <string_view>

namespace pyfiction
{

namespace detail
{

template <typename Lyt>
void write_dot_layout(pybind11::module& m)
{
    namespace py = pybind11;

    m.def(
        "write_dot_layout",
        [](const Lyt& lyt, const std::string_view& filename)
        {
            if constexpr (fiction::is_cartesian_layout_v<Lyt>)
            {
                fiction::write_dot_layout<Lyt, fiction::gate_layout_cartesian_drawer<Lyt>>(lyt, filename);
            }
            else if constexpr (fiction::is_hexagonal_layout_v<Lyt>)
            {
                fiction::write_dot_layout<Lyt, fiction::gate_layout_hexagonal_drawer<Lyt>>(lyt, filename);
            }
        },
        py::arg("layout"), py::arg("filename"), DOC(fiction_write_dot_layout));
}

template <typename Ntk>
void write_dot_network(pybind11::module& m)
{
    namespace py = pybind11;

    m.def(
        "write_dot_network", [](const Ntk& ntk, const std::string_view& filename)
        { mockturtle::write_dot(ntk, filename.data(), fiction::technology_dot_drawer<Ntk>{}); }, py::arg("network"),
        py::arg("filename"));
}

}  // namespace detail

inline void write_dot_layout(pybind11::module& m)
{
    detail::write_dot_layout<py_cartesian_gate_layout>(m);
    detail::write_dot_layout<py_shifted_cartesian_gate_layout>(m);
    detail::write_dot_layout<py_hexagonal_gate_layout>(m);

    detail::write_dot_network<py_logic_network>(m);
}

}  // namespace pyfiction

#endif  // PYFICTION_WRITE_DOT_LAYOUT_HPP
