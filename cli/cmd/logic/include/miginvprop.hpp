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
 * Propagates inverting edges in the current MIG to the primary inputs to reduce inverter cost (may duplicate logic).
 */
class miginvprop_command final : public command
{
  public:
    /**
     * Standard constructor. Adds descriptive information, options, and flags.
     *
     * @param e alice::environment that specifies stores etc.
     */
    explicit miginvprop_command(const environment::ptr& e);

  protected:
    /**
     * Executes inverter propagation on the current MIG and pushes the result to the logic-network store.
     */
    void execute() override;
};

}  // namespace alice
