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
 * @brief Declares the `qll` command, which writes a ToPoliNano/MagCAD/SCERPA layout file.
 * @author Marcel Walter (marcelwa)
 * @author Benjamin Hien (hibenj)
 */

#pragma once

#include <alice/alice.hpp>

#include <string>

namespace alice
{

/**
 * Generates a ToPoliNano/MagCAD/SCERPA layout file for the current iNML, QCA, or molQCA cell layout in store and
 * writes it to the given path.
 *
 * ToPoliNano and MagCAD are available at: https://topolinano.polito.it/. SCERPA is available at:
 * https://ieeexplore.ieee.org/document/8935211
 */
class qll_command final : public command
{
  public:
    /**
     * Standard constructor. Adds descriptive information, options, and flags.
     *
     * @param e alice::environment that specifies stores etc.
     */
    explicit qll_command(const environment::ptr& e);

  protected:
    /**
     * Function to perform the output call. Generates a ToPoliNano/MagCAD/SCERPA layout file.
     */
    void execute() override;

  private:
    /**
     * File name to write the QLL file into.
     */
    std::string filename;
};

}  // namespace alice
