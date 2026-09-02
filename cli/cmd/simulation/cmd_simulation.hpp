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

#ifdef FICTION_SIMULATION_FLOW

// NOLINTBEGIN(misc-include-cleaner)
#if (FICTION_ALGLIB_ENABLED)
#include "include/clustercomplete.hpp"
#endif
#include "include/opdom.hpp"
#include "include/quickexact.hpp"
#include "include/quicksim.hpp"
#include "include/temp.hpp"
// NOLINTEND(misc-include-cleaner)

namespace alice
{

constexpr inline auto FICTION_CLI_CATEGORY_SIMULATION = "Simulation";

// physical simulation commands
#if (FICTION_ALGLIB_ENABLED)
ALICE_ADD_COMMAND(clustercomplete, FICTION_CLI_CATEGORY_SIMULATION)
#endif
ALICE_ADD_COMMAND(opdom, FICTION_CLI_CATEGORY_SIMULATION)
ALICE_ADD_COMMAND(quickexact, FICTION_CLI_CATEGORY_SIMULATION)
ALICE_ADD_COMMAND(quicksim, FICTION_CLI_CATEGORY_SIMULATION)
ALICE_ADD_COMMAND(temp, FICTION_CLI_CATEGORY_SIMULATION)

}  // namespace alice

#endif  // FICTION_SIMULATION_FLOW
