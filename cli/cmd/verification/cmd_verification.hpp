//
// Created by marcel on 10.10.2025.
//

#ifndef FICTION_CMD_VERIFICATION_HPP
#define FICTION_CMD_VERIFICATION_HPP

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

#endif  // FICTION_CMD_VERIFICATION_HPP
