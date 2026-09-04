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
 * @brief Implements the `quicksim` command.
 * @author Marcel Walter (marcelwa)
 */

#include "cmd/simulation/include/quicksim.hpp"

#include "stores.hpp"  // NOLINT(misc-include-cleaner)

#include <fiction/technology/sidb/layout.hpp>
#include <fiction/technology/sidb/simulation/analysis/minimum_energy.hpp>
#include <fiction/technology/sidb/simulation/engines/quicksim.hpp>
#include <fiction/technology/sidb/simulation/result.hpp>
#include <fiction/types.hpp>

#include <alice/alice.hpp>
#include <nlohmann/json.hpp>

#include <any>
#include <cstdint>
#include <limits>
#include <memory>
#include <string>
#include <type_traits>
#include <variant>

namespace alice
{

quicksim_command::quicksim_command(const environment::ptr& e) :
        command(e, "The QuickSim algorithm is a heuristic electrostatic ground state simulation algorithm for SiDB "
                   "layouts. It determines physically valid charge configurations (with minimal energy). Depending on "
                   "the simulation parameters, the ground state is found with a certain probability after one run.")
{
    add_option("--epsilon_r,-e", physical_params.epsilon_r, "Electric permittivity of the substrate (unit-less)", true);
    add_option("--lambda_tf,-l", physical_params.lambda_tf, "Thomas-Fermi screening distance (unit: nm)", true);
    add_option("--mu_minus,-m", physical_params.mu_minus, "Energy transition level (0/-) (unit: eV)", true);
    add_option("--iterations,-i", qs_params.iteration_steps, "Number of iterations to run the simulation for", true);
    add_option("--alpha,-a", qs_params.alpha,
               "alpha parameter (should be reduced if no charge distribution can be determined)", true);
}

void quicksim_command::execute()
{
    sim_result = {};
    min_energy = std::numeric_limits<double>::infinity();

    if (physical_params.epsilon_r <= 0)
    {
        env->out() << "[e] epsilon_r must be positive\n";
        reset_params();
        return;
    }
    if (physical_params.lambda_tf <= 0)
    {
        env->out() << "[e] lambda_tf must be positive\n";
        reset_params();
        return;
    }
    if (qs_params.alpha <= 0)
    {
        env->out() << "[e] alpha must be positive\n";
        reset_params();
        return;
    }
    if (qs_params.iteration_steps == 0)
    {
        env->out() << "[e] iterations must be > 0\n";
        reset_params();
        return;
    }

    auto& s = store<fiction::cell_layout_t>();

    if (s.empty())
    {
        env->out() << "[w] no cell layout in store\n";
        reset_params();
        return;
    }

    const auto quicksim = [this](auto&& lyt_ptr)
    {
        using Lyt = typename std::decay_t<decltype(lyt_ptr)>::element_type;

        if constexpr (std::is_same_v<Lyt, fiction::sidb::simulation::result>)
        {
            env->out() << fmt::format("[w] '{}' has already been simulated; no simulation is conducted\n",
                                      fiction::cli::name_of(*lyt_ptr));
        }
        else if constexpr (!std::is_same_v<Lyt, fiction::sidb::layout>)
        {
            env->out() << fmt::format("[e] '{}' is not an SiDB layout\n", fiction::cli::name_of(*lyt_ptr));
        }
        else
        {
            qs_params.sim_params = physical_params;

            if (const auto result = fiction::sidb::simulation::engines::quicksim(*lyt_ptr, qs_params);
                result.has_value())
            {
                sim_result = *result;
            }

            if (sim_result.charge_distributions.empty())
            {
                env->out() << fmt::format("[e] no stable charge distribution could be determined for '{}'\n",
                                          fiction::cli::name_of(*lyt_ptr));
                return;
            }

            min_energy = fiction::sidb::simulation::analysis::minimum_energy(sim_result.charge_distributions.cbegin(),
                                                                             sim_result.charge_distributions.cend());

            store<fiction::cell_layout_t>().extend() = std::make_shared<fiction::sidb::simulation::result>(sim_result);
        }
    };

    std::visit(quicksim, s.current());

    reset_params();
}

nlohmann::json quicksim_command::log() const
{
    try
    {
        return nlohmann::json{
            {"Algorithm name", sim_result.algorithm_name},
            {"Simulation runtime", sim_result.simulation_runtime.count()},
            {"Physical parameters",
             {{"epsilon_r", sim_result.sim_params.epsilon_r},
              {"lambda_tf", sim_result.sim_params.lambda_tf},
              {"mu_minus", sim_result.sim_params.mu_minus}}},
            {"Lowest state energy (eV)", min_energy},
            {"Number of stable states", sim_result.charge_distributions.size()},
            {"Iteration steps",
             std::any_cast<uint64_t>(sim_result.additional_simulation_parameters.at("iteration_steps"))},
            {"alpha", std::any_cast<double>(sim_result.additional_simulation_parameters.at("alpha"))}};
    }
    catch (...)
    {
        return nlohmann::json{};
    }
}

void quicksim_command::reset_params()
{
    physical_params = fiction::sidb::model::simulation_parameters{2, -0.32, 5.6, 5.0};
    qs_params       = {};
}

}  // namespace alice
