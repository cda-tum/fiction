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
 * @brief Declares the `balance` command, which path-balances the current logic network.
 * @author Marcel Walter (marcelwa)
 */

#pragma once

#include <fiction/synthesis/network_balancing.hpp>

#include <alice/alice.hpp>

namespace alice
{

/**
 * Creates a new technology network of the current logic network in store and performs a path balancing.
 */
class balance_command final : public command
{
  public:
    /**
     * Standard constructor. Adds descriptive information, options, and flags.
     *
     * @param e alice::environment that specifies stores etc.
     */
    explicit balance_command(const environment::ptr& e);

  protected:
    /**
     * Function to perform the balancing call. Inserts buffer nodes to equalize path lengths.
     */
    void execute() override;

  private:
    /**
     * Parameters.
     */
    fiction::synthesis::network_balancing_params ps{};
};

}  // namespace alice
