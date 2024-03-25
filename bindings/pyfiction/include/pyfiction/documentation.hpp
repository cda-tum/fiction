#ifndef FICTION_DOCSTRINGS_HPP
#define FICTION_DOCSTRINGS_HPP

#include "pyfiction/pybind11_mkdoc_docstrings.hpp"

static const char* __doc_fiction_cartesian_layout_overridden =
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

static const char* __doc_fiction_shifted_cartesian_layout_overridden =
    R"doc(A layout type that utilizes offset coordinates to represent a
Cartesian layout with shifted coordinates. In this implementation, odd
columns are vertically shifted. Its faces are organized in the following
way:

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

static const char* __doc_fiction_hexagonal_layout_overridden =
    R"doc(A layout type that utilizes offset coordinates to represent a
hexagonal grid. In this implementation, the hexagons are in the pointy-top
orientation with even rows horizontally shifted. Its faces are organized
in the following way:

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

static const char* __doc_fiction_bounding_box_2d_overridden =
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

#endif  // FICTION_DOCSTRINGS_HPP
