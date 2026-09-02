/*
 * Copyright (c) 2018 - 2023 Marcel Walter
 * Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
 * All rights reserved.
 *
 * SPDX-License-Identifier: MIT
 *
 * Licensed under the MIT License
 */

//
// Created by marcel on 24.10.19.
//

#include "cmd/verification/include/check.hpp"

#include "stores.hpp"  // NOLINT(misc-include-cleaner)

#include <fiction/types.hpp>
#include <fiction/verification/design_rule_violations.hpp>

#include <alice/alice.hpp>
#include <nlohmann/json.hpp>

#include <variant>

namespace alice
{

check_command::check_command(const environment::ptr& e) :
        command(e, "Performs various design rule checks on the current gate-level layout in store. "
                   "A full report can be logged and a summary is printed to standard output.")
{}

void check_command::execute()
{
    auto& s = store<fiction::gate_layout_t>();

    // error case: empty gate-level layout store
    if (s.empty())
    {
        env->out() << "[w] no gate layout in store\n";
        return;
    }

    ps.out = &env->out();
    pst    = {};

    const auto design_rule_check = [this](auto&& lyt_ptr)
    { fiction::verification::gate_level_drvs(*lyt_ptr, ps, &pst); };

    std::visit(design_rule_check, s.current());
}

nlohmann::json check_command::log() const
{
    return pst.report;
}

}  // namespace alice
