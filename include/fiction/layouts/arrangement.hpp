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
#include <string_view>

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

/**
 * Checks whether an arrangement shifts rows, i.e., whether the layout is pointy-top (hexagonal) or horizontally shifted
 * (Cartesian).
 *
 * @param a Arrangement to check.
 * @return `true` iff `a` is `ODD_ROW` or `EVEN_ROW`.
 */
[[nodiscard]] constexpr bool is_row_arrangement(const arrangement a) noexcept
{
    return a == arrangement::ODD_ROW || a == arrangement::EVEN_ROW;
}

/**
 * Checks whether an arrangement shifts odd rows or columns.
 *
 * @param a Arrangement to check.
 * @return `true` iff `a` is `ODD_ROW` or `ODD_COLUMN`.
 */
[[nodiscard]] constexpr bool is_odd_arrangement(const arrangement a) noexcept
{
    return a == arrangement::ODD_ROW || a == arrangement::ODD_COLUMN;
}

/**
 * Returns the name of an arrangement in lower case with underscores, e.g., `"odd_row"`.
 *
 * @param a Arrangement to name.
 * @return Name of `a`.
 */
[[nodiscard]] constexpr std::string_view to_string(const arrangement a) noexcept
{
    switch (a)
    {
        case arrangement::ODD_ROW: return "odd_row";
        case arrangement::EVEN_ROW: return "even_row";
        case arrangement::ODD_COLUMN: return "odd_column";
        case arrangement::EVEN_COLUMN: return "even_column";
    }

    return {};
}

}  // namespace fiction::layouts
