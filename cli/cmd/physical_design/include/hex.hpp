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
 * @brief Declares the `hex` command, which hexagonalizes a 2DDWave-clocked Cartesian layout.
 * @author Simon Hofmann (simon1hofmann)
 * @author Marcel Walter (marcelwa)
 */

#pragma once

#include <fiction/physical_design/hexagonalization.hpp>

#include <alice/alice.hpp>

namespace alice
{

/**
 * Transforms a 2DDWave-clocked Cartesian layout into a hexagonal one.
 */
class hex_command final : public command
{
  public:
    /**
     * Standard constructor. Adds descriptive information, options, and flags.
     *
     * @param e alice::environment that specifies stores etc.
     */
    explicit hex_command(const environment::ptr& e);

  protected:
    /**
     * Parameters.
     */
    fiction::physical_design::hexagonalization_params ps{};
    /**
     * Statistics.
     */
    fiction::physical_design::hexagonalization_stats st{};

    /**
     * Function to transform a 2DDWave-clocked Cartesian layout into a hexagonal one.
     */
    void execute() override;
};

}  // namespace alice
