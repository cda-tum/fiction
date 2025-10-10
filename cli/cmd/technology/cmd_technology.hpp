//
// Created by marcel on 10.10.2025.
//

#ifndef FICTION_CMD_TECHNOLOGY_HPP
#define FICTION_CMD_TECHNOLOGY_HPP

#ifdef FICTION_PHYSICAL_DESIGN_FLOW

#include "include/area.hpp"
#include "include/cell.hpp"
#include "include/energy.hpp"

namespace alice
{

constexpr inline auto FICTION_CLI_CATEGORY_TECHNOLOGY = "Technology";

// technlogy commands
ALICE_ADD_COMMAND(area, FICTION_CLI_CATEGORY_TECHNOLOGY)
ALICE_ADD_COMMAND(cell, FICTION_CLI_CATEGORY_TECHNOLOGY)
ALICE_ADD_COMMAND(energy, FICTION_CLI_CATEGORY_TECHNOLOGY)

}  // namespace alice

#endif  // FICTION_PHYSICAL_DESIGN_FLOW

#endif  // FICTION_CMD_TECHNOLOGY_HPP
