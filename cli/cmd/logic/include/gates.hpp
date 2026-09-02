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

#include <alice/alice.hpp>

namespace alice
{

/**
 * Prints numbers of gates for a current logic network.
 */
class gates_command final : public command
{
  public:
    /**
     * Standard constructor. Adds descriptive information, options, and flags.
     *
     * @param e alice::environment that specifies stores etc.
     */
    explicit gates_command(const environment::ptr& e);

  protected:
    /**
     * Function to perform the gates call. Outputs gates counts for each node type.
     */
    void execute() override;
};

}  // namespace alice
