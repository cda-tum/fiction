#ifndef FICTION_DOCSTRINGS_HPP
#define FICTION_DOCSTRINGS_HPP

#include "pyfiction/pybind11_mkdoc_docstrings.hpp"

static const char *__doc_fiction_cartesian_layout_overridden =
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

Template parameter ``OffsetCoordinateType``:
    The coordinate implementation to be used. Offset coordinates are
    required.)doc";

#endif  // FICTION_DOCSTRINGS_HPP
