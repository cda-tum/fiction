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
 * @brief Python bindings for `fiction/technology/sidb/simulation/analysis/physically_valid_parameters.hpp`.
 * @author Jan Drewniok (Drewniok)
 * @author Marcel Walter (marcelwa)
 */

#include "pyfiction/simulation_parameters.hpp"

#include <fiction/technology/sidb/charge_distribution.hpp>
#include <fiction/technology/sidb/layout.hpp>
#include <fiction/technology/sidb/simulation/analysis/physically_valid_parameters.hpp>
#include <fiction/technology/sidb/simulation/domain.hpp>
#include <fiction/technology/sidb/simulation/logic/operational_domain.hpp>

#include <cstdint>
#include <optional>

#include <nanobind/nanobind.h>
#include <nanobind/stl/function.h>     // NOLINT(misc-include-cleaner): converts progress callbacks
#include <nanobind/stl/optional.h>     // NOLINT(misc-include-cleaner)
#include <nanobind/stl/string_view.h>  // NOLINT(misc-include-cleaner): converts callback task names
#include <nanobind/stl/vector.h>       // NOLINT(misc-include-cleaner)

/**
 * @brief Namespace alias for nanobind.
 */
namespace nb = nanobind;

namespace pyfiction
{

/** @brief Native options constructed for each Python call. */
using operational_domain_options = fiction::sidb::simulation::logic::operational_domain_params;

/**
 * @brief Binds physically valid parameters helpers.
 * @param m Python module.
 */
void register_physically_valid_parameters(nb::module_& m)
{
    // Keep the GIL for domain lookups and Python exception handling; the native sweep releases it.
    nb::class_<fiction::sidb::simulation::domain<fiction::sidb::simulation::logic::parameter_point, uint64_t>>(
        m, "physically_valid_parameters_domain")
        .def(nb::init<>(), "Default constructor.")
        .def(
            "get_excited_state_number_for_parameter",
            [](const fiction::sidb::simulation::domain<fiction::sidb::simulation::logic::parameter_point, uint64_t>&
                                                                        domain,
               const fiction::sidb::simulation::logic::parameter_point& pp)
            {
                if (const auto result = domain.contains(pp); result.has_value())
                {
                    return std::get<0>(result.value());
                }
                // If no value is present, raise a Python ValueError
                throw nb::value_error(
                    "Invalid parameter point: no excited state number available for the provided parameter.");
            },
            nb::arg("pp"));

    // NOTE be careful with the order of the following calls! Python will resolve the first matching overload!
    m.def(
        "physically_valid_parameters",
        [](const fiction::sidb::layout& lyt, const fiction::sidb::charge_distribution& cd,
           const decltype(operational_domain_options{}.operational_params.timeout)              timeout,
           const decltype(operational_domain_options{}.operational_params.sim_params.base)      base,
           const decltype(operational_domain_options{}.operational_params.sim_params.mu_minus)  mu_minus,
           const decltype(operational_domain_options{}.operational_params.sim_params.epsilon_r) epsilon_r,
           const decltype(operational_domain_options{}.operational_params.sim_params.lambda_tf) lambda_tf,
           const decltype(operational_domain_options{}.operational_params.sim_engine)           sim_engine,
           const std::optional<decltype(operational_domain_options{}.sweep_dimensions)>&        sweep_dimensions,
           const std::optional<decltype(operational_domain_options{}.number_of_threads)>        number_of_threads,
           const decltype(operational_domain_options{}.on_progress)&                            on_progress,
           const decltype(operational_domain_options{}.on_worker_progress)&                     on_worker_progress)
        {
            operational_domain_options params{
                .operational_params = {.sim_params = make_simulation_parameters(base, mu_minus, epsilon_r, lambda_tf),
                                       .sim_engine = sim_engine,
                                       .timeout    = timeout},
                .on_progress        = on_progress,
                .on_worker_progress = on_worker_progress};
            if (sweep_dimensions)
            {
                params.sweep_dimensions = *sweep_dimensions;
            }
            if (number_of_threads)
            {
                params.number_of_threads = *number_of_threads;
            }
            const nb::gil_scoped_release release{};
            return fiction::sidb::simulation::analysis::physically_valid_parameters(lyt, cd, params);
        },
        nb::arg("lyt"), nb::arg("cd"), nb::kw_only(),
        nb::arg("timeout")          = operational_domain_options{}.operational_params.timeout,
        nb::arg("base")             = operational_domain_options{}.operational_params.sim_params.base,
        nb::arg("mu_minus")         = operational_domain_options{}.operational_params.sim_params.mu_minus,
        nb::arg("epsilon_r")        = operational_domain_options{}.operational_params.sim_params.epsilon_r,
        nb::arg("lambda_tf")        = operational_domain_options{}.operational_params.sim_params.lambda_tf,
        nb::arg("sim_engine")       = operational_domain_options{}.operational_params.sim_engine,
        nb::arg("sweep_dimensions") = nb::none(), nb::arg("number_of_threads") = nb::none(),
        nb::arg("on_progress").none() = nb::none(), nb::arg("on_worker_progress").none() = nb::none(),
        R"doc(Sweep physical parameter values for which the supplied charge distribution is physically valid.
The keyword options define the sweep, physical model, worker count, and progress callbacks.
Raises ValueError for engines other than QuickExact, EXGS, and QuickSim.

Args:
    timeout: Execution budget in milliseconds.
    base: Number of charge states: 2 for negative and neutral, or 3 to include positive charges.
    mu_minus: Negative-charge transition energy in eV.
    epsilon_r: Relative permittivity (dimensionless).
    lambda_tf: Thomas-Fermi screening distance in nm.
    sweep_dimensions: Ordered parameter ranges; None sweeps relative permittivity and screening distance with the native ranges.
    number_of_threads: Number of workers; None uses the native default.
    on_progress: Callback receiving (task, completed, total); None disables progress reporting.
    on_worker_progress: Callback receiving (worker, count, description, completed, total, active); None disables worker reports.
    sim_engine: SiDB simulation engine used to count lower-energy charge distributions.
)doc");
}

}  // namespace pyfiction
