//
// Created by marcel on 10.10.2025.
//

#ifndef FICTION_CMD_GENERAL_HPP
#define FICTION_CMD_GENERAL_HPP

// NOLINTBEGIN(misc-include-cleaner)
#include "include/clear.hpp"
#include "include/version.hpp"
// NOLINTEND(misc-include-cleaner)

namespace alice
{

constexpr inline auto FICTION_CLI_CATEGORY_GENERAL = "General";

// general commands
ALICE_ADD_COMMAND(clear, FICTION_CLI_CATEGORY_GENERAL)
ALICE_ADD_COMMAND(version, FICTION_CLI_CATEGORY_GENERAL)

}  // namespace alice

#endif  // FICTION_CMD_GENERAL_HPP
