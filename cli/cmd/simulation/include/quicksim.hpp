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
 * @brief Declares the `quicksim` command, which runs *QuickSim* on the current layout.
 * @author Marcel Walter (marcelwa)
 */

#pragma once

#include <fiction/technology/sidb/model/simulation_parameters.hpp>
#include <fiction/technology/sidb/simulation/engines/quicksim.hpp>
#include <fiction/technology/sidb/simulation/result.hpp>
#include <fiction/types.hpp>

#include <alice/alice.hpp>
#include <nlohmann/json.hpp>

#include <limits>

namespace alice
{

/**
 * Executes *QuickSim* heuristic simulation for the current SiDB cell-level layout in store.
 */
class quicksim_command final : public command
{
  public:
    /**
     * Standard constructor. Adds descriptive information, options, and flags.
     *
     * @param e alice::environment that specifies stores etc.
     */
    explicit quicksim_command(const environment::ptr& e);

  protected:
    /**
     * Function to perform the simulation call.
     */
    void execute() override;

  private:
    /**
     * Physical parameters for the simulation.
     */
    fiction::sidb::model::simulation_parameters physical_params{2, -0.32, 5.6, 5.0};
    /**
     * QuickSim parameters.
     */
    fiction::sidb::simulation::engines::quicksim_params qs_params{};
    /**
     * Result of the last simulation.
     */
    fiction::sidb::simulation::result sim_result{};
    /**
     * Minimum energy.
     */
    double min_energy{std::numeric_limits<double>::infinity()};
    /**
     * Logs the resulting information in a log file.
     *
     * @return JSON object containing details about the simulation.
     */
    [[nodiscard]] nlohmann::json log() const override;

    /**
     * Resets the parameters to their default values.
     */
    void reset_params();
};

}  // namespace alice
