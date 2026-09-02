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

// NOLINTBEGIN(misc-include-cleaner)
#include "include/blif.hpp"
#include "include/fgl.hpp"
#include "include/fqca.hpp"
#include "include/qca.hpp"
#include "include/qcc.hpp"
#include "include/qll.hpp"
#include "include/read.hpp"
#include "include/sqd.hpp"
#include "include/tt.hpp"
#include "include/verilog.hpp"
// NOLINTEND(misc-include-cleaner)

namespace alice
{

constexpr inline auto FICTION_CLI_CATEGORY_IO = "I/O";

// input/output commands
ALICE_ADD_COMMAND(blif, FICTION_CLI_CATEGORY_IO)
ALICE_ADD_COMMAND(fgl, FICTION_CLI_CATEGORY_IO)
ALICE_ADD_COMMAND(fqca, FICTION_CLI_CATEGORY_IO)
ALICE_ADD_COMMAND(qca, FICTION_CLI_CATEGORY_IO)
ALICE_ADD_COMMAND(qcc, FICTION_CLI_CATEGORY_IO)
ALICE_ADD_COMMAND(qll, FICTION_CLI_CATEGORY_IO)
ALICE_ADD_COMMAND(read, FICTION_CLI_CATEGORY_IO)
ALICE_ADD_COMMAND(sqd, FICTION_CLI_CATEGORY_IO)
ALICE_ADD_COMMAND(tt, FICTION_CLI_CATEGORY_IO)
ALICE_ADD_COMMAND(verilog, FICTION_CLI_CATEGORY_IO)

}  // namespace alice
