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
 * @brief Declares the `fgl` command, which writes the current gate-level layout to FGL.
 * @author Simon Hofmann (simon1hofmann)
 * @author Marcel Walter (marcelwa)
 */

#pragma once

#include <alice/alice.hpp>

#include <string>

namespace alice
{

/**
 * Generates a FGL file for the current gate-level layout in store and writes it to the given path.
 */
class fgl_command final : public command
{
  public:
    /**
     * Standard constructor. Adds descriptive information, options, and flags.
     *
     * @param e alice::environment that specifies stores etc.
     */
    explicit fgl_command(const environment::ptr& e);

  protected:
    /**
     * Function to perform the output call. Generates a FGL file.
     */
    void execute() override;

  private:
    /**
     * File name to write the FGL file into.
     */
    std::string filename;
};

}  // namespace alice
