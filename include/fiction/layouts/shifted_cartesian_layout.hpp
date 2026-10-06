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
 * @brief Cartesian layout with shifted rows or columns in four offset arrangements.
 * @author Marcel Walter (marcelwa)
 * @author Simon Hofmann (simon1hofmann)
 */

#pragma once

#include "fiction/layouts/arrangement.hpp"
#include "fiction/layouts/hexagonal_layout.hpp"

#include <type_traits>

namespace fiction::layouts
{

/**
 * A layout type that utilizes offset coordinates to represent a Cartesian layout with shifted coordinates. Its faces
 * are organized in an offset coordinate system as provided. The arrangement fixed at construction selects which rows or
 * columns are shifted. Row arrangements shift horizontally, column arrangements vertically. The four arrangements look
 * as follows.
 *
 * `arrangement::ODD_ROW`:
 * \verbatim
  +-------+-------+-------+
  |       |       |       |
  | (0,0) | (1,0) | (2,0) |
  |       |       |       |
  +---+---+---+---+---+---+---+
      |       |       |       |
      | (0,1) | (1,1) | (2,1) |
      |       |       |       |
  +---+---+---+---+---+---+---+
  |       |       |       |
  | (0,2) | (1,2) | (2,2) |
  |       |       |       |
  +-------+-------+-------+
  \endverbatim
 *
 * `arrangement::EVEN_ROW`:
 * \verbatim
      +-------+-------+-------+
      |       |       |       |
      | (0,0) | (1,0) | (2,0) |
      |       |       |       |
  +---+---+---+---+---+---+---+
  |       |       |       |
  | (0,1) | (1,1) | (2,1) |
  |       |       |       |
  +---+---+---+---+---+---+---+
      |       |       |       |
      | (0,2) | (1,2) | (2,2) |
      |       |       |       |
      +-------+-------+-------+
  \endverbatim
 *
 * `arrangement::ODD_COLUMN`:
 * \verbatim
   +-------+       +-------+
   |       |       |       |
   | (0,0) +-------+ (2,0) +-------+
   |       |       |       |       |
   +-------+ (1,0) +-------+ (3,0) |
   |       |       |       |       |
   | (0,1) +-------+ (2,1) +-------+
   |       |       |       |       |
   +-------+ (1,1) +-------+ (3,1) |
   |       |       |       |       |
   | (0,2) +-------+ (2,2) +-------+
   |       |       |       |
   +-------+       +-------+
  \endverbatim
 *
 * `arrangement::EVEN_COLUMN`:
 * \verbatim
          +-------+       +-------+
          |       |       |       |
  +-------+ (1,0) +-------+ (3,0) |
  |       |       |       |       |
  | (0,0) +-------+ (2,0) +-------+
  |       |       |       |       |
  +-------+ (1,1) +-------+ (3,1) |
  |       |       |       |       |
  | (0,1) +-------+ (2,1) +-------+
  |       |       |       |       |
  +-------+ (1,2) +-------+ (3,2) |
          |       |       |       |
          +-------+       +-------+
  \endverbatim
 *
 */
class shifted_cartesian_layout : public hexagonal_layout
{
  private:
    /** Geometry shared with hexagonal layouts. */
    using HexagonalLayout = hexagonal_layout;

  public:
    /**
     * Marks the layout as shifted Cartesian, which distinguishes it from the hexagonal layout it shares its neighbor
     * geometry with.
     */
    using is_shifted_cartesian = std::true_type;

    /**
     * Creates geometry with half-open, zero-origin bounds. The default extent is empty.
     * @param a Arrangement of shifted rows or columns.
     * @param size Axis sizes.
     * @throws std::invalid_argument If a size exceeds the coordinate domain.
     */
    explicit shifted_cartesian_layout(const arrangement a, const HexagonalLayout::extent& size = {}) :
            HexagonalLayout(a, size)
    {}

    /** @param lyt Hexagonal geometry to copy. */
    // NOLINTNEXTLINE(*-explicit-constructor, *-explicit-conversions): implicit geometry conversion preserves clone
    // usage
    shifted_cartesian_layout(const HexagonalLayout& lyt) : HexagonalLayout(lyt) {}

  private:
    // intentionally hide members of HexagonalLayout
    using HexagonalLayout::to_cube_coordinate;
    using HexagonalLayout::to_offset_coordinate;
};

}  // namespace fiction::layouts
