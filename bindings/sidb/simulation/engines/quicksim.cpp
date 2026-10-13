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
 * @brief Python bindings for `fiction/technology/sidb/simulation/engines/quicksim.hpp`.
 * @author Marcel Walter (marcelwa)
 * @author Jan Drewniok (Drewniok)
 */

#include "pyfiction/documentation.hpp"

#include <fiction/technology/sidb/layout.hpp>
#include <fiction/technology/sidb/simulation/engines/quicksim.hpp>
#include <fiction/technology/sidb/simulation/result.hpp>

#include <optional>

#include <nanobind/nanobind.h>
#include <nanobind/stl/function.h>       // NOLINT(misc-include-cleaner)
#include <nanobind/stl/optional.h>       // NOLINT(misc-include-cleaner)
#include <nanobind/stl/string_view.h>    // NOLINT(misc-include-cleaner)
#include <nanobind/stl/unordered_map.h>  // NOLINT(misc-include-cleaner)
#include <nanobind/stl/vector.h>         // NOLINT(misc-include-cleaner)

/**
 * @brief Namespace alias for nanobind.
 */
namespace nb = nanobind;

namespace pyfiction
{

/** @brief Native options constructed for each Python call. */
using quicksim_options = fiction::sidb::simulation::engines::quicksim_params;

/**
 * @brief Binds quicksim helpers.
 * @param m Python module.
 */
void register_quicksim(nb::module_& m)
{
    m.def(
        "quicksim",
        [](const fiction::sidb::layout& lyt, const decltype(quicksim_options{}.sim_params.base) base,
           const decltype(quicksim_options{}.sim_params.mu_minus)           mu_minus,
           const decltype(quicksim_options{}.sim_params.epsilon_r)          epsilon_r,
           const decltype(quicksim_options{}.sim_params.lambda_tf)          lambda_tf,
           const decltype(quicksim_options{}.iteration_steps)               iteration_steps,
           const decltype(quicksim_options{}.alpha)                         alpha,
           const std::optional<decltype(quicksim_options{}.number_threads)> number_threads,
           const decltype(quicksim_options{}.timeout)                       timeout,
           const decltype(quicksim_options{}.on_progress)                   on_progress,
           const decltype(quicksim_options{}.on_worker_progress)            on_worker_progress)
        {
            quicksim_options params{};
            if (base != 2 && base != 3)
            {
                throw nb::value_error("base must be 2 or 3");
            }
            params.sim_params      = fiction::sidb::model::simulation_parameters{base, mu_minus, epsilon_r, lambda_tf};
            params.iteration_steps = iteration_steps;
            params.alpha           = alpha;
            if (number_threads)
            {
                params.number_threads = *number_threads;
            }
            params.timeout            = timeout;
            params.on_progress        = on_progress;
            params.on_worker_progress = on_worker_progress;
            const nb::gil_scoped_release release{};
            return fiction::sidb::simulation::engines::quicksim(lyt, params);
        },
        nb::arg("lyt"), nb::kw_only(), nb::arg("base") = quicksim_options{}.sim_params.base,
        nb::arg("mu_minus")        = quicksim_options{}.sim_params.mu_minus,
        nb::arg("epsilon_r")       = quicksim_options{}.sim_params.epsilon_r,
        nb::arg("lambda_tf")       = quicksim_options{}.sim_params.lambda_tf,
        nb::arg("iteration_steps") = quicksim_options{}.iteration_steps, nb::arg("alpha") = quicksim_options{}.alpha,
        nb::arg("number_threads") = nb::none(), nb::arg("timeout") = quicksim_options{}.timeout,
        nb::arg("on_progress").none() = nb::none(), nb::arg("on_worker_progress").none() = nb::none(),
        R"doc(*QuickSim* is a heuristic ground-state simulation of SiDB layouts
proposed in \"QuickSim: Efficient and Accurate Physical Simulation of
Silicon Dangling Bond Logic\" by J. Drewniok, M. Walter, S. S. H. Ng,
K. Walus, and R. Wille in IEEE NANO 2023
(https://ieeexplore.ieee.org/document/10231266). It grows candidate
charge distributions from the SiDBs that have to be negative by
repeatedly charging the neutral SiDB farthest from every negative one,
keeping the physically valid intermediate states. Every worker thread
owns one simulation state and shares the layout's potential landscape.
Charged surface defects are not supported: a layout that holds any
yields `std::nullopt`.

Args:
    lyt: Layout to simulate.

Returns:
    The physically valid charge distributions found, or `std::nullopt`
    if the layout is empty, holds charged defects, the iteration count
    is zero, the timeout was hit, or no valid distribution was found.

Raises:
    std::out_of_range: if a site has an invalid lattice basis index.
    utils::timeout_error: if the shared caller deadline expires. No
                          partial result is returned.

Algorithm options are keyword-only; the signature lists their defaults.
)doc");
}

}  // namespace pyfiction
