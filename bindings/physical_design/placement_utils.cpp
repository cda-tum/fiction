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

#include "fiction/layouts/layout_base.hpp"
#include "pyfiction/types.hpp"  // NOLINT(misc-include-cleaner): preserves coordinate and extent tuple signatures.

#include <fiction/layouts/gate_layout.hpp>
#include <fiction/physical_design/placement_utils.hpp>
#include <fiction/types.hpp>

#include <optional>

#include <nanobind/nanobind.h>
#include <nanobind/stl/optional.h>  // NOLINT(misc-include-cleaner): Accepts None for the optional constant input.

/**
 * @brief Namespace alias for nanobind.
 */
namespace nb = nanobind;

namespace pyfiction
{

namespace detail
{

/** @brief Bind placement at a known coordinate. @tparam Lyt Gate layout. @param m Python module.
 */
template <typename Lyt>
void register_place(nb::module_& m)
{
    // Keep the GIL: each overload places one gate with at most three inputs.
    m.def(
        "place",
        [](Lyt& lyt, const fiction::layouts::layout_base::coordinate& t,
           const fiction::networks::named_technology_network&       ntk,
           const fiction::networks::named_technology_network::node& n)
        { return fiction::physical_design::place(lyt, t, ntk, n); },
        nb::arg("lyt"), nb::arg("t"), nb::arg("ntk"), nb::arg("n"),
        "Places a primary input at the given coordinate and returns its output port.");

    m.def(
        "place",
        [](Lyt& lyt, const fiction::layouts::layout_base::coordinate& t,
           const fiction::networks::named_technology_network&       ntk,
           const fiction::networks::named_technology_network::node& n, const typename Lyt::object_id& a)
        { return fiction::physical_design::place(lyt, t, ntk, n, a); },
        nb::arg("lyt"), nb::arg("t"), nb::arg("ntk"), nb::arg("n"), nb::arg("a"));

    m.def(
        "place",
        [](Lyt& lyt, const fiction::layouts::layout_base::coordinate& t,
           const fiction::networks::named_technology_network&       ntk,
           const fiction::networks::named_technology_network::node& n, const typename Lyt::object_id& a,
           const typename Lyt::object_id& b, const std::optional<bool>& c = std::nullopt)
        { return fiction::physical_design::place(lyt, t, ntk, n, a, b, c); },
        nb::arg("lyt"), nb::arg("t"), nb::arg("ntk"), nb::arg("n"), nb::arg("a"), nb::arg("b"), nb::arg("c"));

    m.def(
        "place",
        [](Lyt& lyt, const fiction::layouts::layout_base::coordinate& t,
           const fiction::networks::named_technology_network&       ntk,
           const fiction::networks::named_technology_network::node& n, const typename Lyt::object_id& a,
           const typename Lyt::object_id& b, const typename Lyt::object_id& c)
        { return fiction::physical_design::place(lyt, t, ntk, n, a, b, c); },
        nb::arg("lyt"), nb::arg("t"), nb::arg("ntk"), nb::arg("n"), nb::arg("a"), nb::arg("b"), nb::arg("c"));
}

}  // namespace detail

/** @brief Register placed gate creation. @param m Python module. */
void register_placement_utils(nb::module_& m)
{
    // NOTE be careful with the order of the following calls! Python will resolve the first matching overload!

    detail::register_place<fiction::layouts::cartesian_gate_layout>(m);
    detail::register_place<fiction::layouts::shifted_cartesian_gate_layout>(m);
    detail::register_place<fiction::layouts::hexagonal_gate_layout>(m);
}

}  // namespace pyfiction
