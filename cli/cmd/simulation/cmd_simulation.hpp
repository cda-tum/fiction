//
// Created by marcel on 10.10.2025.
//

#ifndef FICTION_CMD_SIMULATION_HPP
#define FICTION_CMD_SIMULATION_HPP

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

#endif  // FICTION_CMD_SIMULATION_HPP
