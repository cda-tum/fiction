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
 * @brief Python bindings for `fiction/synthesis/network_balancing.hpp`.
 * @author Marcel Walter (marcelwa)
 */

#include "pyfiction/types.hpp"

#include <fiction/synthesis/network_balancing.hpp>

#include <nanobind/nanobind.h>
#include <nanobind/stl/function.h>     // NOLINT(misc-include-cleaner): enables callback conversion
#include <nanobind/stl/pair.h>         // NOLINT(misc-include-cleaner)
#include <nanobind/stl/string_view.h>  // NOLINT(misc-include-cleaner): converts callback task names
#include <nanobind/stl/vector.h>       // NOLINT(misc-include-cleaner)

/**
 * @brief Namespace alias for nanobind.
 */
namespace nb = nanobind;

namespace pyfiction
{

namespace detail
{

/**
 * @brief Binds network balancing helpers.
 * @param m Python module.
 * @tparam Ntk Network type.
 */
template <typename Ntk>
void register_network_balancing_impl(nb::module_& m)
{
    m.def(
        "network_balancing",
        [](const Ntk& network, const decltype(fiction::synthesis::network_balancing_params{}.on_progress)& on_progress,
           const decltype(fiction::synthesis::network_balancing_params{}.unify_outputs) unify_outputs)
        {
            fiction::synthesis::network_balancing_params params{};
            params.on_progress   = on_progress;
            params.unify_outputs = unify_outputs;
            const nb::gil_scoped_release release{};
            return fiction::synthesis::network_balancing<py_tec_network, Ntk>(network, params);
        },
        nb::arg("network"), nb::kw_only(), nb::arg("on_progress").none() = nb::none(),
        nb::arg("unify_outputs") = fiction::synthesis::network_balancing_params{}.unify_outputs,
        R"doc(Insert identity nodes to balance network paths.
Returns a fresh technology network that is logically equivalent to the input.
Set unify_outputs to place every output at the same logic depth.

Args:
    on_progress: Callback receiving (task, completed, total); None disables progress reporting.
    unify_outputs: Flag to indicate that all output nodes should be in the same rank.
)doc");

    m.def(
        "is_balanced",
        [](const Ntk& network, const decltype(fiction::synthesis::network_balancing_params{}.on_progress)& on_progress,
           const decltype(fiction::synthesis::network_balancing_params{}.unify_outputs) unify_outputs)
        {
            fiction::synthesis::network_balancing_params params{};
            params.on_progress   = on_progress;
            params.unify_outputs = unify_outputs;
            const nb::gil_scoped_release release{};
            return fiction::synthesis::is_balanced<Ntk>(network, params);
        },
        nb::arg("network"), nb::kw_only(), nb::arg("on_progress").none() = nb::none(),
        nb::arg("unify_outputs") = fiction::synthesis::network_balancing_params{}.unify_outputs,
        R"doc(Check whether network paths are balanced.
Set unify_outputs to require every output at the same logic depth.

Args:
    on_progress: Callback receiving (task, completed, total); None disables progress reporting.
    unify_outputs: Flag to indicate that all output nodes should be in the same rank.
)doc");
}

}  // namespace detail

/**
 * @brief Binds network balancing helpers.
 * @param m Python module.
 */
void register_network_balancing(nb::module_& m)
{

    detail::register_network_balancing_impl<py_tec_network>(m);
}

}  // namespace pyfiction
