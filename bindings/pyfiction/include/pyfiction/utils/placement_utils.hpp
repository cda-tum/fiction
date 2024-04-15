//
// Created by marcel on 13.02.23.
//

#ifndef PYFICTION_PLACEMENT_UTILS_HPP
#define PYFICTION_PLACEMENT_UTILS_HPP

#include "pyfiction/documentation.hpp"
#include "pyfiction/types.hpp"

#include <fiction/traits.hpp>
#include <fiction/utils/placement_utils.hpp>

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include <optional>

namespace pyfiction
{

namespace detail
{

template <typename Lyt, typename Ntk>
void reserve_input_nodes(pybind11::module& m)
{
    using namespace pybind11::literals;

    m.def("reserve_input_nodes", &fiction::reserve_input_nodes<Lyt, Ntk>, "lyt"_a, "ntk"_a);
}

template <typename Lyt, typename Ntk>
void place(pybind11::module& m)
{
    using namespace pybind11::literals;

    m.def(
        "place",
        [](Lyt& lyt, const fiction::tile<Lyt>& t, const Ntk& ntk, const mockturtle::node<Ntk>& n)
        { return fiction::place(lyt, t, ntk, n); },
        "lyt"_a, "t"_a, "ntk"_a, "n"_a);  // TODO, DOC(fiction_place));

    m.def(
        "place",
        [](Lyt& lyt, const fiction::tile<Lyt>& t, const Ntk& ntk, const mockturtle::node<Ntk>& n,
           const mockturtle::signal<Lyt>& a) { return fiction::place(lyt, t, ntk, n, a); },
        "lyt"_a, "t"_a, "ntk"_a, "n"_a, "a"_a);

    m.def(
        "place",
        [](Lyt& lyt, const fiction::tile<Lyt>& t, const Ntk& ntk, const mockturtle::node<Ntk>& n,
           const mockturtle::signal<Lyt>& a, const mockturtle::signal<Lyt>& b,
           const std::optional<bool>& c = std::nullopt) { return fiction::place(lyt, t, ntk, n, a, b, c); },
        "lyt"_a, "t"_a, "ntk"_a, "n"_a, "a"_a, "b"_a, "c"_a);

    m.def(
        "place",
        [](Lyt& lyt, const fiction::tile<Lyt>& t, const Ntk& ntk, const mockturtle::node<Ntk>& n,
           const mockturtle::signal<Lyt>& a, const mockturtle::signal<Lyt>& b, const mockturtle::signal<Lyt>& c)
        { return fiction::place(lyt, t, ntk, n, a, b, c); },
        "lyt"_a, "t"_a, "ntk"_a, "n"_a, "a"_a, "b"_a, "c"_a);
}

}  // namespace detail

inline void placement_utils(pybind11::module& m)
{
    // NOTE be careful with the order of the following calls! Python will resolve the first matching overload!

    detail::reserve_input_nodes<py_cartesian_obstruction_layout, py_logic_network>(m);
    detail::reserve_input_nodes<py_cartesian_gate_layout, py_logic_network>(m);
    detail::reserve_input_nodes<py_shifted_cartesian_obstruction_layout, py_logic_network>(m);
    detail::reserve_input_nodes<py_shifted_cartesian_gate_layout, py_logic_network>(m);
    detail::reserve_input_nodes<py_hexagonal_obstruction_layout, py_logic_network>(m);
    detail::reserve_input_nodes<py_hexagonal_gate_layout, py_logic_network>(m);

    detail::place<py_cartesian_obstruction_layout, py_logic_network>(m);
    detail::place<py_cartesian_gate_layout, py_logic_network>(m);
    detail::place<py_shifted_cartesian_obstruction_layout, py_logic_network>(m);
    detail::place<py_shifted_cartesian_gate_layout, py_logic_network>(m);
    detail::place<py_hexagonal_obstruction_layout, py_logic_network>(m);
    detail::place<py_hexagonal_gate_layout, py_logic_network>(m);
}

}  // namespace pyfiction

#endif  // PYFICTION_PLACEMENT_UTILS_HPP
