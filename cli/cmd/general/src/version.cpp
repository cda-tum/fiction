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
 * @brief Implements the `version` command.
 * @author Marcel Walter (marcelwa)
 */

#include "cmd/general/include/version.hpp"

#include "fiction/utils/version_info.hpp"

#include <alice/alice.hpp>
#include <fmt/format.h>

namespace alice
{

version_command::version_command(const environment::ptr& e) :
        command(e, "Outputs the version string as well as build time and date.")
{}

void version_command::execute()
{
    env->out() << fmt::format("[i] {} - compiled on {} at {}\n", fiction::FICTION_VERSION, fiction::COMPILED_DATE,
                              fiction::COMPILED_TIME)
               << fmt::format("[i] {}\n", fiction::FICTION_REPO);
}

}  // namespace alice
