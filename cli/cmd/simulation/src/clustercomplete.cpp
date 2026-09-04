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
 * @brief Implements the `clustercomplete` command.
 * @author Willem Lambooy (wlambooy)
 * @author Marcel Walter (marcelwa)
 */

#if (FICTION_ALGLIB_ENABLED)

#include "cmd/simulation/include/clustercomplete.hpp"

#include "stores.hpp"  // NOLINT(misc-include-cleaner)

#include <fiction/technology/sidb/layout.hpp>
#include <fiction/technology/sidb/model/simulation_parameters.hpp>
#include <fiction/technology/sidb/simulation/analysis/minimum_energy.hpp>
#include <fiction/technology/sidb/simulation/engines/clustercomplete.hpp>
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

clustercomplete_command::clustercomplete_command(const environment::ptr& e) :
        command(e,
                "ClusterComplete is a proof of concept of the more general idea of state space pruning in a cluster "
                "hierarchy. In the application on SiDB layouts, it is able to simulate SiDB logic in base 3 for 50 DBs "
                "and more, depending on the layout that determines base of the exponential growth in simulation "
                "complexity with added SiDBs.")
{
    add_option("--base,-b", physical_params.base, "The simulation base, can be 2 or 3", true);
    add_option("--epsilon_r,-e", physical_params.epsilon_r, "Electric permittivity of the substrate (unit-less)", true);
    add_option("--lambda_tf,-l", physical_params.lambda_tf, "Thomas-Fermi screening distance (unit: nm)", true);
    add_option("--mu_minus,-m", physical_params.mu_minus, "Energy transition level (0/-) (unit: eV)", true);
    add_option("--global_potential,-g", cc_params.global_potential,
               "Global potential applied to the entire layout (unit: V)", true);
    add_option("--witness_partitioning_limit,-w", cc_params.validity_witness_partitioning_max_cluster_size_gss,
               "The limit on the cluster size before Ground State Space omits the check for which it solves the "
               "validity witness partitioning NP-complete sub-problem",
               true);
    add_option("--overlapping_witnesses_limit,-o", cc_params.num_overlapping_witnesses_limit_gss,
               "The limit on the number of overlapping witnesses (that determines the factorial scaling of the "
               "sub-procedure) before Ground State Space skips validity witness partitioning",
               true);
    add_flag("--report_gss_stats,-r",
             "When set, Ground State Space statistics are shown, which give an estimate for the ClusterComplete "
             "runtimes, and thus allow the user to configure the validity witness partitioning parameters that "
             "could benefit large simulation problems through more intensive pruning before starting a lengthy "
             "unfolding process");
}

void clustercomplete_command::execute()
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
    if (physical_params.base != 2 && physical_params.base != 3)
    {
        env->out() << "[e] the simulation base has to be 2 or 3\n";
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

    const auto clustercomplete = [this](auto&& lyt_ptr)
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
            cc_params.sim_params = physical_params;
            cc_params.report_gss_stats =
                is_set("report_gss_stats") ?
                    fiction::sidb::simulation::engines::clustercomplete_params::ground_state_space_reporting::ON :
                    fiction::sidb::simulation::engines::clustercomplete_params::ground_state_space_reporting::OFF;

            sim_result = fiction::sidb::simulation::engines::clustercomplete(*lyt_ptr, cc_params);

            if (sim_result.charge_distributions.empty())
            {
                env->out() << fmt::format("[e] ground state of '{}' could not be determined\n",
                                          fiction::cli::name_of(*lyt_ptr));
                return;
            }

            min_energy = fiction::sidb::simulation::analysis::minimum_energy(sim_result.charge_distributions.cbegin(),
                                                                             sim_result.charge_distributions.cend());

            store<fiction::cell_layout_t>().extend() = std::make_shared<fiction::sidb::simulation::result>(sim_result);
        }
    };

    std::visit(clustercomplete, s.current());

    reset_params();
}

nlohmann::json clustercomplete_command::log() const
{
    try
    {
        return nlohmann::json{{"Algorithm name", sim_result.algorithm_name},
                              {"Simulation runtime", sim_result.simulation_runtime.count()},
                              {"Physical parameters",
                               {{"epsilon_r", sim_result.sim_params.epsilon_r},
                                {"lambda_tf", sim_result.sim_params.lambda_tf},
                                {"mu_minus", sim_result.sim_params.mu_minus}}},
                              {"Lowest state energy (eV)", min_energy},
                              {"Number of stable states", sim_result.charge_distributions.size()},
                              {"Validity witness partitioning limit",
                               std::any_cast<uint64_t>(sim_result.additional_simulation_parameters.at(
                                   "validity_witness_partitioning_limit"))},
                              {"Number of overlapping witnesses limit",
                               std::any_cast<uint64_t>(
                                   sim_result.additional_simulation_parameters.at("num_overlapping_witnesses_limit"))}};
    }
    catch (...)
    {
        return nlohmann::json{};
    }
}

void clustercomplete_command::reset_params()
{
    physical_params = fiction::sidb::model::simulation_parameters{3, -0.32, 5.6, 5.0};
    cc_params       = {};
}

}  // namespace alice

#endif  // FICTION_ALGLIB_ENABLED
