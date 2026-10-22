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
 * @brief Canonical truth-table type for synthesis and simulation.
 * @author Marcel Walter (marcelwa)
 */

#pragma once

#include <kitty/dynamic_truth_table.hpp>

namespace fiction::synthesis
{

/**
 * @brief Dynamic truth table for Boolean functions.
 */
using truth_table = kitty::dynamic_truth_table;

}  // namespace fiction::synthesis
