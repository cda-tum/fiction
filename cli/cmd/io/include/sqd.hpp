/*
 * Copyright (c) 2018 - 2023 Marcel Walter
 * Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
 * All rights reserved.
 *
 * SPDX-License-Identifier: MIT
 *
 * Licensed under the MIT License
 */

//
// Created by marcel on 14.07.21.
//

#pragma once

#include <alice/alice.hpp>

#include <string>

namespace alice
{

/**
 * Generates a SiQAD file for the current cell layout in store and writes it to the given path.
 *
 * SiQAD is available at: https://waluslab.ece.ubc.ca/siqad/
 */
class sqd_command final : public command
{
  public:
    /**
     * Standard constructor. Adds descriptive information, options, and flags.
     *
     * @param e alice::environment that specifies stores etc.
     */
    explicit sqd_command(const environment::ptr& e);

  protected:
    /**
     * Function to perform the output call. Generates a SiQAD file.
     */
    void execute() override;

  private:
    /**
     * File name to write the SQD file into.
     */
    std::string filename;
};

}  // namespace alice
