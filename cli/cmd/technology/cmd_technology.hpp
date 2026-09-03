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
 * @brief Collects the technology CLI commands for inclusion into the shell.
 * @author Marcel Walter (marcelwa)
 */

#pragma once

#ifdef FICTION_PHYSICAL_DESIGN_FLOW

// NOLINTBEGIN(misc-include-cleaner)
#include "include/area.hpp"
#include "include/cell.hpp"
// NOLINTEND(misc-include-cleaner)

namespace alice
{

constexpr inline auto FICTION_CLI_CATEGORY_TECHNOLOGY = "Technology";

// technology commands
ALICE_ADD_COMMAND(area, FICTION_CLI_CATEGORY_TECHNOLOGY)
ALICE_ADD_COMMAND(cell, FICTION_CLI_CATEGORY_TECHNOLOGY)

}  // namespace alice

#endif  // FICTION_PHYSICAL_DESIGN_FLOW
