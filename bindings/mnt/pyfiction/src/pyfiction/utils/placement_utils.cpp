//
// Created by marcel on 13.02.23.
//

#include "pyfiction/types.hpp"

#include <fiction/traits.hpp>
#include <fiction/utils/placement_utils.hpp>

#include <mockturtle/traits.hpp>

#include <optional>

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

namespace detail
{

template <typename Lyt, typename Ntk>
void reserve_input_nodes(nanobind::module_& m)
{
    namespace py = nanobind;  // NOLINT(misc-unused-alias-decls)

    m.def("reserve_input_nodes", &fiction::reserve_input_nodes<Lyt, Ntk>, py::arg("lyt"), py::arg("ntk"));
}

template <typename Lyt, typename Ntk>
void place(nanobind::module_& m)
{
    namespace py = nanobind;  // NOLINT(misc-unused-alias-decls)

    m.def(
        "place", [](Lyt& lyt, const fiction::tile<Lyt>& t, const Ntk& ntk, const mockturtle::node<Ntk>& n)
        { return fiction::place(lyt, t, ntk, n); }, py::arg("lyt"), py::arg("t"), py::arg("ntk"),
        py::arg("n"));  // TODO, DOC(fiction_place));

    m.def(
        "place",
        [](Lyt& lyt, const fiction::tile<Lyt>& t, const Ntk& ntk, const mockturtle::node<Ntk>& n,
           const mockturtle::signal<Lyt>& a) { return fiction::place(lyt, t, ntk, n, a); },
        py::arg("lyt"), py::arg("t"), py::arg("ntk"), py::arg("n"), py::arg("a"));

    m.def(
        "place",
        [](Lyt& lyt, const fiction::tile<Lyt>& t, const Ntk& ntk, const mockturtle::node<Ntk>& n,
           const mockturtle::signal<Lyt>& a, const mockturtle::signal<Lyt>& b,
           const std::optional<bool>& c = std::nullopt) { return fiction::place(lyt, t, ntk, n, a, b, c); },
        py::arg("lyt"), py::arg("t"), py::arg("ntk"), py::arg("n"), py::arg("a"), py::arg("b"), py::arg("c"));

    m.def(
        "place",
        [](Lyt& lyt, const fiction::tile<Lyt>& t, const Ntk& ntk, const mockturtle::node<Ntk>& n,
           const mockturtle::signal<Lyt>& a, const mockturtle::signal<Lyt>& b, const mockturtle::signal<Lyt>& c)
        { return fiction::place(lyt, t, ntk, n, a, b, c); },
        py::arg("lyt"), py::arg("t"), py::arg("ntk"), py::arg("n"), py::arg("a"), py::arg("b"), py::arg("c"));
}

}  // namespace detail

void placement_utils(nanobind::module_& m)
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
