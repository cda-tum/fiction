//
// Created by marcel on 10.10.2025.
//

#ifndef FICTION_CMD_PHYSICAL_DESIGN_HPP
#define FICTION_CMD_PHYSICAL_DESIGN_HPP

#ifdef FICTION_PHYSICAL_DESIGN_FLOW

// NOLINTBEGIN(misc-include-cleaner)
#if (FICTION_Z3_SOLVER)
#include "include/exact.hpp"
#endif
#include "include/gold.hpp"
#include "include/hex.hpp"
#if (MUGEN)
#include "include/onepass.hpp"
#endif
#include "include/optimize.hpp"
#include "include/ortho.hpp"
// NOLINTEND(misc-include-cleaner)

namespace alice
{

constexpr inline auto FICTION_CLI_CATEGORY_PHYSICAL_DESIGN = "Physical Design";

// physical design commands
#if (FICTION_Z3_SOLVER)
ALICE_ADD_COMMAND(exact, FICTION_CLI_CATEGORY_PHYSICAL_DESIGN)
#endif
ALICE_ADD_COMMAND(gold, FICTION_CLI_CATEGORY_PHYSICAL_DESIGN)
ALICE_ADD_COMMAND(hex, FICTION_CLI_CATEGORY_PHYSICAL_DESIGN)
#if (MUGEN)
ALICE_ADD_COMMAND(onepass, FICTION_CLI_CATEGORY_PHYSICAL_DESIGN)
#endif
ALICE_ADD_COMMAND(optimize, FICTION_CLI_CATEGORY_PHYSICAL_DESIGN)
ALICE_ADD_COMMAND(ortho, FICTION_CLI_CATEGORY_PHYSICAL_DESIGN)

}  // namespace alice

#endif  // FICTION_PHYSICAL_DESIGN_FLOW

#endif  // FICTION_CMD_PHYSICAL_DESIGN_HPP
