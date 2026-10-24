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

#include "pyfiction/simulation_parameters.hpp"

#include <fiction/technology/sidb/layout.hpp>
#include <fiction/technology/sidb/simulation/engines/quicksim.hpp>

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
           const decltype(quicksim_options{}.on_progress)&                  on_progress,
           const decltype(quicksim_options{}.on_worker_progress)&           on_worker_progress)
        {
            quicksim_options params{};
            params.sim_params      = make_simulation_parameters(base, mu_minus, epsilon_r, lambda_tf);
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
        R"doc(Search for low-energy distributions of negative and neutral charges with QuickSim.
The keyword options select physical values, iterations, threads, runtime, and progress callbacks.
Returns None when the heuristic search finds no physically valid charge distribution.
Raises ValueError when base is neither 2 nor 3.

Args:
    base: Accepts 2 or 3; QuickSim searches negative and neutral charge states for either value.
    mu_minus: Negative-charge transition energy in eV.
    epsilon_r: Relative permittivity (dimensionless).
    lambda_tf: Thomas-Fermi screening distance in nm.
    iteration_steps: Number of iterations to run the simulation for.
    alpha: `alpha` parameter for the *QuickSim* algorithm.
    number_threads: Number of QuickSim workers; None uses the available hardware threads.
    timeout: Execution budget in milliseconds.
    on_progress: Callback receiving (task, completed, total); None disables progress reporting.
    on_worker_progress: Callback receiving (worker, count, description, completed, total, active); None disables worker reports.
)doc");
}

}  // namespace pyfiction
