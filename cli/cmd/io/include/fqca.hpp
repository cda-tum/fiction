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
// Created by marcel on 24.09.21.
//

#pragma once

#include <fiction/technology/qca/io/write_fqca_layout.hpp>

#include <alice/alice.hpp>

#include <string>

namespace alice
{

/**
 * Generates a QCA-STACK file for the current cell layout in store and writes it to the given path.
 *
 * QCA-STACK is available at: https://github.com/wlambooy/QCA-STACK
 */
class fqca_command final : public command
{
  public:
    /**
     * Standard constructor. Adds descriptive information, options, and flags.
     *
     * @param e alice::environment that specifies stores etc.
     */
    explicit fqca_command(const environment::ptr& e);

  protected:
    /**
     * Function to perform the output call. Generates a QCA-STACK file.
     */
    void execute() override;

  private:
    /**
     * File name to write the FQCA file into.
     */
    std::string filename;

    fiction::qca::io::write_fqca_layout_params ps{};
};

}  // namespace alice
