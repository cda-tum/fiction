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
 * @brief Tags describing the role of a silicon dangling bond.
 * @author Marcel Walter (marcelwa)
 */

#pragma once

#include <cstdint>

namespace fiction::sidb
{

/**
 * Tags describing the role of a silicon dangling bond. EMPTY denotes an unoccupied site.
 */
enum class dot_tag : uint8_t
{
    /**
     * Unoccupied lattice site.
     */
    EMPTY = ' ',
    /**
     * Untagged dot.
     */
    NORMAL = 'x',
    /**
     * Primary input dot.
     */
    INPUT = 'i',
    /**
     * Primary output dot.
     */
    OUTPUT = 'o',
    /**
     * Logic dot (e.g., a canvas SiDB).
     */
    LOGIC = 'l'
};

}  // namespace fiction::sidb
