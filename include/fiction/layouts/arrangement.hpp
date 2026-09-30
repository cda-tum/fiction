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
 * @brief Arrangement of the shifted rows or columns of shifted Cartesian and hexagonal layouts.
 * @author Marcel Walter (marcelwa)
 */

#pragma once

#include <cstdint>

namespace fiction::layouts
{

/**
 * Arrangement of the shifted rows or columns of a shifted Cartesian or hexagonal layout.
 */
enum class arrangement : uint8_t
{
    /**
     * Odd rows are shifted.
     */
    ODD_ROW,
    /**
     * Even rows are shifted.
     */
    EVEN_ROW,
    /**
     * Odd columns are shifted.
     */
    ODD_COLUMN,
    /**
     * Even columns are shifted.
     */
    EVEN_COLUMN
};

}  // namespace fiction::layouts
