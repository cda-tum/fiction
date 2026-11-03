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
 * @brief Comparison truth tables used by the published standard-cell experiments.
 */

#pragma once

#include <kitty/dynamic_truth_table.hpp>

#include <cstdint>

namespace fiction_experiments
{

/**
 * The historical LE word 0x11 has NOR in its four valid bits. The historical GE word 0x13 has !b,
 * where b is variable 1. Both words retain bit 4 to preserve the published experiments.
 * @param word Original truth-table word.
 * @return Two-variable truth table with the original stored word.
 */
[[nodiscard]] inline kitty::dynamic_truth_table historical_comparison_table(const uint64_t word) noexcept
{
    kitty::dynamic_truth_table table{2};
    *table.begin() = word;
    return table;
}

}  // namespace fiction_experiments
