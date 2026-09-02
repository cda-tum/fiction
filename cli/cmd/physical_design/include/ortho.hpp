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

#include <fiction/physical_design/orthogonal.hpp>

#include <alice/alice.hpp>
#include <nlohmann/json.hpp>

#include <cstdint>
#include <string>

namespace alice
{

/**
 * Executes a heuristic physical approach based on "Improved Orthogonal Drawing of 3-graphs" by Therese Biedl, 1996.
 * See algo/orthogonal.hpp for more details.
 */
class ortho_command final : public command
{
  public:
    /**
     * Standard constructor. Adds descriptive information, options, and flags.
     *
     * @param e alice::environment that specifies stores etc.
     */
    explicit ortho_command(const environment::ptr& e);

  protected:
    /**
     * Function to perform the physical design call. Generates a placed and routed FCN gate layout.
     */
    void execute() override;

    /**
     * Logs the resulting information in a log file.
     *
     * @return JSON object containing information about the physical design process.
     */
    nlohmann::json log() const override;

  private:
    /**
     * Tile shift for hexagonal layouts.
     */
    std::string hexagonal_tile_shift{};
    /**
     * Number of clock phases.
     */
    uint8_t num_clock_phases = 4u;
    /**
     * Parameters.
     */
    fiction::physical_design::orthogonal_physical_design_params ps{};
    /**
     * Statistics.
     */
    fiction::physical_design::orthogonal_physical_design_stats st{};

    template <typename Lyt>
    void orthogonal_physical_design();
};

}  // namespace alice
