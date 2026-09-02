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
 * @brief Declares the `fanouts` command, which substitutes multi-outputs with fanout nodes.
 * @author Marcel Walter (marcelwa)
 */

#pragma once

#include <fiction/synthesis/fanout_substitution.hpp>

#include <alice/alice.hpp>

#include <cstdint>

namespace alice
{

/**
 * Substitutes multi-outputs of gate vertices and replaces them with fan-out ones.
 */
class fanouts_command final : public command
{
  public:
    /**
     * Standard constructor. Adds descriptive information, options, and flags.
     *
     * @param e alice::environment that specifies stores etc.
     */
    explicit fanouts_command(const environment::ptr& e);

  protected:
    /**
     * Function to perform the fanouts call. Substitutes high-degree outputs and replaces them by fan-out vertices.
     */
    void execute() override;

  private:
    /**
     * Random seed used for random fanout substitution
     */
    uint32_t seed{0u};
    /**
     * Strategy as integer for CLI11 compatibility
     */
    int strategy_int{0};
    /**
     * Parameters.
     */
    fiction::synthesis::fanout_substitution_params ps{};
};

}  // namespace alice
