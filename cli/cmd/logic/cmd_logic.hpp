//
// Created by marcel on 10.10.2025.
//

#ifndef FICTION_CMD_LOGIC_HPP
#define FICTION_CMD_LOGIC_HPP

#ifdef FICTION_LOGIC_SYNTHESIS_FLOW

// NOLINTBEGIN(misc-include-cleaner)
#if (FICTION_ABC)
#include "include/abc.hpp"
#endif
#include "include/akers.hpp"
#include "include/balance.hpp"
#include "include/fanouts.hpp"
#include "include/gates.hpp"
#include "include/map.hpp"
#include "include/miginvopt.hpp"
#include "include/miginvprop.hpp"
#include "include/random.hpp"
#include "include/simulate.hpp"
// NOLINTEND(misc-include-cleaner)

namespace alice
{

constexpr inline auto FICTION_CLI_CATEGORY_LOGIC = "Logic";

// logic synthesis commands
#if (FICTION_ABC)
ALICE_ADD_COMMAND(abc, FICTION_CLI_CATEGORY_LOGIC)
#endif
ALICE_ADD_COMMAND(akers, FICTION_CLI_CATEGORY_LOGIC)
ALICE_ADD_COMMAND(balance, FICTION_CLI_CATEGORY_LOGIC)
ALICE_ADD_COMMAND(fanouts, FICTION_CLI_CATEGORY_LOGIC)
ALICE_ADD_COMMAND(gates, FICTION_CLI_CATEGORY_LOGIC)
ALICE_ADD_COMMAND(map, FICTION_CLI_CATEGORY_LOGIC)
ALICE_ADD_COMMAND(miginvopt, FICTION_CLI_CATEGORY_LOGIC)
ALICE_ADD_COMMAND(miginvprop, FICTION_CLI_CATEGORY_LOGIC)
ALICE_ADD_COMMAND(random, FICTION_CLI_CATEGORY_LOGIC)
ALICE_ADD_COMMAND(simulate, FICTION_CLI_CATEGORY_LOGIC)

}  // namespace alice

#endif

#endif  // FICTION_CMD_LOGIC_HPP
