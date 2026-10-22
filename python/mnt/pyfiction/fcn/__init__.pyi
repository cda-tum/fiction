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
    Return the area of the layout's Cartesian extent in square nanometers.
    Every cell position contributes its cell dimensions, including empty positions.
    Neighboring positions contribute their horizontal and vertical spacing.

    Args:
        layout: QCA, molQCA, or iNML layout to measure.
        width: Cell width in nm; defaults to the layout type's CELL_WIDTH.
        height: Cell height in nm; defaults to the layout type's CELL_HEIGHT.
        hspace: Horizontal spacing in nm; defaults to the layout type's CELL_HSPACE.
        vspace: Vertical spacing in nm; defaults to the layout type's CELL_VSPACE.

    Returns:
        The area as a float in square nanometers.
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
    Return the SiDB layout's bounding-box area in square nanometers.
    The bounding box covers SiDBs and defects. Every column and single-SiDB row
    contributes its dimensions and the spacing between neighboring columns and rows.
    An empty layout has zero area.

    Args:
        layout: SiDB layout to measure.
        width: Column width in nm; defaults to the layout type's CELL_WIDTH.
        height: Row height in nm; defaults to the layout type's CELL_HEIGHT.
        hspace: Horizontal spacing in nm; defaults to the layout type's CELL_HSPACE.
        vspace: Vertical spacing in nm; defaults to the layout type's CELL_VSPACE.

    Returns:
        The area as a float in square nanometers.
    """
