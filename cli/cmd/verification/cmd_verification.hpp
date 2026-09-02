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

namespace alice
{

constexpr inline auto FICTION_CLI_CATEGORY_VERIFICATION = "Verification";

}

// equiv is useful for both the logic synthesis and physical design flow
#if defined(FICTION_LOGIC_SYNTHESIS_FLOW) || defined(FICTION_PHYSICAL_DESIGN_FLOW)
// NOLINTBEGIN(misc-include-cleaner)
#include "include/equiv.hpp"
// NOLINTEND(misc-include-cleaner)

namespace alice
{

// formal verification commands
ALICE_ADD_COMMAND(equiv, FICTION_CLI_CATEGORY_VERIFICATION)

}  // namespace alice

#endif

// NOLINTBEGIN(misc-include-cleaner)
#ifdef FICTION_PHYSICAL_DESIGN_FLOW
#include "include/check.hpp"
// NOLINTEND(misc-include-cleaner)

namespace alice
{

// layout validation commands
ALICE_ADD_COMMAND(check, FICTION_CLI_CATEGORY_VERIFICATION)

}  // namespace alice

#endif
