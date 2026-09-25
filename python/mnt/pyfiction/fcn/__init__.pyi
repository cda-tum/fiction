# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""Technology-independent FCN layout properties."""

from typing import overload

import mnt.pyfiction.inml
import mnt.pyfiction.mol_qca
import mnt.pyfiction.qca
import mnt.pyfiction.sidb
from mnt.pyfiction.fcn import io as io

@overload
def area(
    layout: mnt.pyfiction.qca.qca_layout,
    width: float = 18.0,
    height: float = 18.0,
    hspace: float = 2.0,
    vspace: float = 2.0,
) -> float: ...
@overload
def area(
    layout: mnt.pyfiction.mol_qca.mol_qca_layout,
    width: float = 2.0,
    height: float = 2.0,
    hspace: float = 0.0,
    vspace: float = 0.0,
) -> float: ...
@overload
def area(
    layout: mnt.pyfiction.inml.inml_layout,
    width: float = 50.0,
    height: float = 100.0,
    hspace: float = 10.0,
    vspace: float = 25.0,
) -> float:
    """
    Computes real-world area requirements in nm² of a given cell-level
    layout. For this purpose, each cell position in the layout is assigned
    a vertical and horizontal size. Additionally, a spacing between cell
    positions in horizontal and vertical direction is taken into account.

    Args:
        lyt: The layout whose area is desired.
        ps: Area parameters.
        pst: Area statistics.

    Template Args:
        Lyt: Layout type with a Cartesian extent, e.g., `qca::layout`,
             `mol_qca::layout`, or `inml::layout`.
        Dims: Layout type whose cell dimensions `ps` holds; `Lyt` by
              default.

    Returns:
        Area requirements in nm².
    """

@overload
def area(
    layout: mnt.pyfiction.sidb.sidb_layout,
    width: float = 0.0,
    height: float = 0.0,
    hspace: float = 0.384,
    vspace: float = 0.384,
) -> float:
    """
    Computes real-world area requirements in nm² of the bounding box of a
    given SiDB layout. The bounding box covers the layout's SiDBs and
    defects; every column and every single-SiDB row inside it is assigned
    a horizontal and vertical size, and a spacing between neighboring
    columns and rows is taken into account. An empty layout has no area.

    Args:
        lyt: The SiDB layout whose area is desired.
        ps: Area parameters.
        pst: Area statistics.

    Returns:
        Area requirements in nm².
    """
