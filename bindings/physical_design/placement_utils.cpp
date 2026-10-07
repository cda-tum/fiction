/*
 * Copyright (c) 2018 - 2023 Marcel Walter
 * Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
 * All rights reserved.
 *
 * SPDX-License-Identifier: MIT
 *
 * Licensed under the MIT License
 */

/**
 * @file
 * @brief Python bindings for `fiction/physical_design/placement_utils.hpp`.
 * @author Marcel Walter (marcelwa)
 */

#include "pyfiction/types.hpp"

#include <fiction/physical_design/placement_utils.hpp>
#include <fiction/traits.hpp>

#include <mockturtle/traits.hpp>

#include <optional>

#include <nanobind/nanobind.h>
#include <nanobind/stl/optional.h>  // NOLINT(misc-include-cleaner)

namespace pyfiction
{

namespace detail
{

/** @brief Bind placement at a known coordinate. @tparam Lyt Gate layout. @tparam Ntk Network. @param m Python module.
 */
template <typename Lyt, typename Ntk>
void place(nanobind::module_& m)
{
    namespace py = nanobind;  // NOLINT(misc-unused-alias-decls)

    m.def(
        "place", [](Lyt& lyt, const fiction::tile<Lyt>& t, const Ntk& ntk, const mockturtle::node<Ntk>& n)
        { return fiction::physical_design::place(lyt, t, ntk, n); }, py::arg("lyt"), py::arg("t"), py::arg("ntk"),
        py::arg("n"), "Places a primary input at the given coordinate and returns its output port.");

    m.def(
        "place",
        [](Lyt& lyt, const fiction::tile<Lyt>& t, const Ntk& ntk, const mockturtle::node<Ntk>& n,
           const typename Lyt::output_port& a) { return fiction::physical_design::place(lyt, t, ntk, n, a); },
        py::arg("lyt"), py::arg("t"), py::arg("ntk"), py::arg("n"), py::arg("a"));

    m.def(
        "place",
        [](Lyt& lyt, const fiction::tile<Lyt>& t, const Ntk& ntk, const mockturtle::node<Ntk>& n,
           const typename Lyt::output_port& a, const typename Lyt::output_port& b,
           const std::optional<bool>& c = std::nullopt)
        { return fiction::physical_design::place(lyt, t, ntk, n, a, b, c); },
        py::arg("lyt"), py::arg("t"), py::arg("ntk"), py::arg("n"), py::arg("a"), py::arg("b"), py::arg("c"));

    m.def(
        "place",
        [](Lyt& lyt, const fiction::tile<Lyt>& t, const Ntk& ntk, const mockturtle::node<Ntk>& n,
           const typename Lyt::output_port& a, const typename Lyt::output_port& b, const typename Lyt::output_port& c)
        { return fiction::physical_design::place(lyt, t, ntk, n, a, b, c); },
        py::arg("lyt"), py::arg("t"), py::arg("ntk"), py::arg("n"), py::arg("a"), py::arg("b"), py::arg("c"));
}

}  // namespace detail

/** @brief Register placed gate creation. @param m Python module. */
void placement_utils(nanobind::module_& m)
{
    // NOTE be careful with the order of the following calls! Python will resolve the first matching overload!

    detail::place<py_cartesian_gate_layout, py_tec_network>(m);
    detail::place<py_shifted_cartesian_gate_layout, py_tec_network>(m);
    detail::place<py_hexagonal_gate_layout, py_tec_network>(m);
}

}  // namespace pyfiction
