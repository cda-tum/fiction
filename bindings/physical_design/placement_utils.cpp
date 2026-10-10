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
#include <nanobind/stl/optional.h>  // NOLINT(misc-include-cleaner): Accepts None for the optional constant input.

namespace pyfiction
{

namespace detail
{

/** @brief Bind placement at a known coordinate. @tparam Lyt Gate layout. @tparam Ntk Network. @param m Python module.
 */
template <typename Lyt, typename Ntk>
void place(nanobind::module_& m)
{
    m.def(
        "place", [](Lyt& lyt, const fiction::tile<Lyt>& t, const Ntk& ntk, const mockturtle::node<Ntk>& n)
        { return fiction::physical_design::place(lyt, t, ntk, n); }, nanobind::arg("lyt"), nanobind::arg("t"),
        nanobind::arg("ntk"), nanobind::arg("n"),
        "Places a primary input at the given coordinate and returns its output port.");

    m.def(
        "place",
        [](Lyt& lyt, const fiction::tile<Lyt>& t, const Ntk& ntk, const mockturtle::node<Ntk>& n,
           const typename Lyt::object_id& a) { return fiction::physical_design::place(lyt, t, ntk, n, a); },
        nanobind::arg("lyt"), nanobind::arg("t"), nanobind::arg("ntk"), nanobind::arg("n"), nanobind::arg("a"));

    m.def(
        "place",
        [](Lyt& lyt, const fiction::tile<Lyt>& t, const Ntk& ntk, const mockturtle::node<Ntk>& n,
           const typename Lyt::object_id& a, const typename Lyt::object_id& b,
           const std::optional<bool>& c = std::nullopt)
        { return fiction::physical_design::place(lyt, t, ntk, n, a, b, c); },
        nanobind::arg("lyt"), nanobind::arg("t"), nanobind::arg("ntk"), nanobind::arg("n"), nanobind::arg("a"),
        nanobind::arg("b"), nanobind::arg("c"));

    m.def(
        "place",
        [](Lyt& lyt, const fiction::tile<Lyt>& t, const Ntk& ntk, const mockturtle::node<Ntk>& n,
           const typename Lyt::object_id& a, const typename Lyt::object_id& b, const typename Lyt::object_id& c)
        { return fiction::physical_design::place(lyt, t, ntk, n, a, b, c); },
        nanobind::arg("lyt"), nanobind::arg("t"), nanobind::arg("ntk"), nanobind::arg("n"), nanobind::arg("a"),
        nanobind::arg("b"), nanobind::arg("c"));
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
