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
 * @brief Declares the `clear` command, which empties every store.
 * @author Marcel Walter (marcelwa)
 */

#pragma once

#include <alice/alice.hpp>

namespace alice
{

/**
 * Clears all stores by removing all their entries.
 */
class clear_command final : public command
{
  public:
    /**
     * Standard constructor. Adds descriptive information, options, and flags.
     *
     * @param e alice::environment that specifies stores etc.
     */
    explicit clear_command(const environment::ptr& e);

  protected:
    /**
     * Function to perform the clear call. Calls alice's clear function for all stores.
     */
    void execute() override;

  private:
    /**
     * Stores to clear.
     */
    bool cell = false, gate = false, network = false, table = false;
};

}  // namespace alice
