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

#include <fiction/synthesis/technology_mapping.hpp>

#include <alice/alice.hpp>

namespace alice
{

/**
 * Performs technology mapping to transform a network into another one using specific gates.
 */
class map_command final : public command
{
  public:
    /**
     * Standard constructor. Adds descriptive information, options, and flags.
     *
     * @param e alice::environment that specifies stores etc.
     */
    explicit map_command(const environment::ptr& e);

  protected:
    /**
     * Function to perform the mockturtle::emap call. Generates a logic network from another one.
     */
    void execute() override;

  private:
    /**
     * Technology mapping parameters.
     */
    fiction::synthesis::technology_mapping_params ps{};
};

}  // namespace alice
