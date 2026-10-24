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
 * @author Benjamin Hien (hibenj)
 */

#include <fiction/synthesis/network_balancing.hpp>
#include <fiction/types.hpp>

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

/**
 * @brief Binds network balancing helpers.
 * @param m Python module.
 */
void register_network_balancing(nb::module_& m)
{
    m.def(
        "network_balancing",
        [](const fiction::networks::named_technology_network&                           network,
           const decltype(fiction::synthesis::network_balancing_params{}.on_progress)&  on_progress,
           const decltype(fiction::synthesis::network_balancing_params{}.unify_outputs) unify_outputs,
           const decltype(fiction::synthesis::network_balancing_params{}
                              .buffer_constant_outputs)                                 buffer_constant_outputs)
        {
            fiction::synthesis::network_balancing_params params{};
            params.on_progress             = on_progress;
            params.unify_outputs           = unify_outputs;
            params.buffer_constant_outputs = buffer_constant_outputs;
            const nb::gil_scoped_release release{};
            return fiction::synthesis::network_balancing<fiction::networks::named_technology_network,
                                                         fiction::networks::named_technology_network>(network, params);
        },
        nb::arg("network"), nb::kw_only(), nb::arg("on_progress").none() = nb::none(),
        nb::arg("unify_outputs")           = fiction::synthesis::network_balancing_params{}.unify_outputs,
        nb::arg("buffer_constant_outputs") = fiction::synthesis::network_balancing_params{}.buffer_constant_outputs,
        R"doc(Insert identity nodes to balance network paths.
Returns a fresh technology network that is logically equivalent to the input.
Set unify_outputs to place every output at the same logic depth.

Args:
    on_progress: Callback receiving (task, completed, total); None disables progress reporting.
    unify_outputs: Flag to indicate that all output nodes should be in the same rank.
    buffer_constant_outputs: Whether outputs driven by a constant receive a buffer chain when unifying outputs;
        set to False for flows that do not place constants, which then also ignore them in the check.
)doc");

    m.def(
        "is_balanced",
        [](const fiction::networks::named_technology_network&                           network,
           const decltype(fiction::synthesis::network_balancing_params{}.on_progress)&  on_progress,
           const decltype(fiction::synthesis::network_balancing_params{}.unify_outputs) unify_outputs,
           const decltype(fiction::synthesis::network_balancing_params{}
                              .buffer_constant_outputs)                                 buffer_constant_outputs)
        {
            fiction::synthesis::network_balancing_params params{};
            params.on_progress             = on_progress;
            params.unify_outputs           = unify_outputs;
            params.buffer_constant_outputs = buffer_constant_outputs;
            const nb::gil_scoped_release release{};
            return fiction::synthesis::is_balanced<fiction::networks::named_technology_network>(network, params);
        },
        nb::arg("network"), nb::kw_only(), nb::arg("on_progress").none() = nb::none(),
        nb::arg("unify_outputs")           = fiction::synthesis::network_balancing_params{}.unify_outputs,
        nb::arg("buffer_constant_outputs") = fiction::synthesis::network_balancing_params{}.buffer_constant_outputs,
        R"doc(Check whether network paths are balanced.
Set unify_outputs to require every output at the same logic depth.

Args:
    on_progress: Callback receiving (task, completed, total); None disables progress reporting.
    unify_outputs: Flag to indicate that all output nodes should be in the same rank.
    buffer_constant_outputs: Whether outputs driven by a constant receive a buffer chain when unifying outputs;
        set to False for flows that do not place constants, which then also ignore them in the check.
)doc");
}

}  // namespace pyfiction
