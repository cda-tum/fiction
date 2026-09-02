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

#include <fiction/types.hpp>

#include <alice/alice.hpp>

namespace alice
{

/**
 * Performs logic synthesis to generate a network from a truth table.
 */
class akers_command final : public command
{
  public:
    /**
     * Standard constructor. Adds descriptive information, options, and flags.
     *
     * @param e alice::environment that specifies stores etc.
     */
    explicit akers_command(const environment::ptr& e);

  protected:
    /**
     * Function to perform the synth call. Generates a logic network from a truth table.
     */
    void execute() override;

  private:
    template <typename Ntk>
    void synthesize_and_store(const fiction::truth_table_t& tt);
};

}  // namespace alice
