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
 * @author Marcel Walter (marcelwa)
 */

#pragma once

#include <fiction/technology/sidb/model/simulation_parameters.hpp>
#include <fiction/technology/sidb/simulation/logic/operational_domain.hpp>

#include <alice/alice.hpp>
#include <nlohmann/json.hpp>

#include <cstdlib>
#include <string>
#include <vector>

namespace alice
{

/**
 * Executes operational domain computation for the current SiDB cell-level layout in store. The operational domain is a
 * set of simulation parameter values for which a given SiDB layout is logically operational. This means that a layout
 * is deemed operational if the layout's ground state corresponds with a given Boolean function at the layout's outputs
 * for all possible input combinations.
 *
 * The computation comes in different flavors:
 * - Grid search: Evaluates all possible parameter combinations within the specified ranges.
 * - Random sampling: Evaluates a specified number of random parameter combinations.
 * - Flood fill: Evaluates a specified number of random parameter combinations and then performs a flood fill to find
 *  the operational domain. Requires at least two sweep dimensions.
 *  - Contour tracing: Evaluates a specified number of random parameter combinations and then performs contour tracing
 *  to find the edges of the operational domain. Requires at least two sweep dimensions; in three, it collects the
 *  boundary surface instead of walking a closed curve.
 *
 * Each flavor can determine the operational status either by physical simulation or, with `--sketch`, by filtering
 * alone. The latter is the *operational domain sketch*: dramatically faster, never rejecting a point that is
 * operational, but reporting some non-operational points as operational.
 *
 * The operational domain is written to a CSV file, which can be used for further analysis or visualization.
 *
 * For more information, see algorithms/simulation/sidb/operational_domain.hpp.
 */
class opdom_command final : public command
{
  public:
    /**
     * Standard constructor. Adds descriptive information, options, and flags.
     *
     * @param e alice::environment that specifies stores etc.
     */
    explicit opdom_command(const environment::ptr& e);

  protected:
    /**
     * Function to perform the operational domain call.
     */
    void execute() override;

  private:
    /**
     * Default physical parameters for the simulation.
     */
    fiction::sidb::model::simulation_parameters sim_params{2, -0.32, 5.6, 5.0};
    /**
     * Default value ranges for sweeping.
     */
    std::vector<fiction::sidb::simulation::logic::operational_domain_value_range> sweep_dimensions{
        {fiction::sidb::simulation::logic::sweep_parameter::EPSILON_R, 1.0, 10.0, 0.1},
        {fiction::sidb::simulation::logic::sweep_parameter::LAMBDA_TF, 1.0, 10.0, 0.1},
        {fiction::sidb::simulation::logic::sweep_parameter::MU_MINUS, -0.50, -0.10, 0.025}};
    /**
     * Operational domain parameters.
     */
    fiction::sidb::simulation::logic::operational_domain_params params{};
    /**
     * Operational domain stats.
     */
    fiction::sidb::simulation::logic::operational_domain_stats stats{};
    /**
     * Number of random samples.
     */
    std::size_t num_random_samples{};
    /**
     * User input for the x dimension sweep parameter.
     */
    std::string x_sweep{"epsilon_r"};
    /**
     * User input for the y dimension sweep parameter.
     */
    std::string y_sweep{"lambda_tf"};
    /**
     * User input for the z dimension sweep parameter.
     */
    std::string z_sweep{};
    /**
     * The simulation engine to use.
     */
    std::string sim_engine_str{"QuickExact"};
    /**
     * Cached engine name for logging (preserves the engine name after params reset).
     */
    std::string last_engine_name{};
    /**
     * Cached sketch flag for logging (preserves the setting after params reset).
     */
    bool last_sketch{};
    /**
     * CSV filename to write the operational domain to.
     */
    std::string filename{};
    /**
     * Flag to omit non-operational samples.
     */
    bool omit_non_operational_samples{};
    /**
     * Flag to compute the operational domain sketch instead of simulating each parameter point.
     */
    bool sketch{};
    /**
     * The operational domain.
     */
    fiction::sidb::simulation::logic::operational_domain op_domain{};
    /**
     * Writes the operational domain to the specified CSV file.
     */
    void write_op_domain();
    /**
     * Logs the resulting information in a log file.
     *
     * @return JSON object containing details about the operational domain.
     */
    [[nodiscard]] nlohmann::json log() const override;
    /**
     * Resets the parameters to their default values.
     */
    void reset_params();
};

}  // namespace alice
