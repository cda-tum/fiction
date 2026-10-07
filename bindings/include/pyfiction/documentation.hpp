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
 * @brief Hand-written docstrings that override the ones extracted from the C++ sources.
 * @author Marcel Walter (marcelwa)
 */

#pragma once

#include "pyfiction/pybind11_mkdoc_docstrings.hpp"  // IWYU pragma: export

/** @brief Python binding documentation. */
namespace pyfiction
{

/** @brief Documentation for the Cartesian layout class. */
inline constexpr auto CARTESIAN_LAYOUT_DOC =
    R"doc(A layout type that utilizes offset coordinates to represent a
Cartesian grid. Its faces are organized in the following way:

.. code-block:: text

    +-------+-------+-------+-------+
    |       |       |       |       |
    | (0,0) | (1,0) | (2,0) | (3,0) |
    |       |       |       |       |
    +-------+-------+-------+-------+
    |       |       |       |       |
    | (0,1) | (1,1) | (2,1) | (3,1) |
    |       |       |       |       |
    +-------+-------+-------+-------+
    |       |       |       |       |
    | (0,2) | (1,2) | (2,2) | (3,2) |
    |       |       |       |       |
    +-------+-------+-------+-------+

)doc";

/** @brief Documentation for the shifted Cartesian layout class. */
inline constexpr auto SHIFTED_CARTESIAN_LAYOUT_DOC =
    R"doc(A layout type that utilizes offset coordinates to represent a
Cartesian layout with shifted rows or columns selected by its arrangement.
This example uses arrangement.ODD_COLUMN:

.. code-block:: text

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

)doc";

/** @brief Documentation for the hexagonal layout class. */
inline constexpr auto HEXAGONAL_LAYOUT_DOC =
    R"doc(A layout type that utilizes offset coordinates to represent a
hexagonal grid. Its arrangement selects shifted rows or columns. Row
arrangements use pointy-top hexagons; column arrangements use flat-top
hexagons. This example uses arrangement.EVEN_ROW:

.. code-block:: text

           / \     / \     / \
         /     \ /     \ /     \
        | (0,0) | (1,0) | (2,0) |
        |       |       |       |
       / \     / \     / \     /
     /     \ /     \ /     \ /
    | (0,1) | (1,1) | (2,1) |
    |       |       |       |
     \     / \     / \     / \
       \ /     \ /     \ /     \
        | (0,2) | (1,2) | (2,2) |
        |       |       |       |
         \     / \     / \     /
           \ /     \ /     \ /

Other representations would be using cube or axial coordinates for
instance, but since we want the layouts to be rectangular-ish, offset
coordinates make the most sense here.

https://www.redblobgames.com/grids/hexagons/ is a wonderful resource
on the topic.)doc";

/** @brief Documentation for occupied layout bounds. */
inline constexpr auto BOUNDING_BOX_2D_DOC =
    R"doc(Returns the minimum and maximum corner of the bounding box.
A 2D bounding box object computes a minimum-sized box around all
non-empty coordinates in a given layout. Layouts can be of arbitrary
size and, thus, may be larger than their contained elements.
Sometimes, it might be necessary to know exactly which space the
associated layout internals occupy. A bounding box computes
coordinates that span a minimum-sized rectangle that encloses all non-
empty layout coordinates.

Returns:
    The minimum  and maximum enclosing coordinate in the associated layout.)doc";

}  // namespace pyfiction
