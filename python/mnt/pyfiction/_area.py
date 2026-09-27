# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""Physical area adapters for the supported cell technologies."""

from __future__ import annotations

from math import isfinite
from typing import TYPE_CHECKING

from mnt.pyfiction._native.fcn import area as _native_area

if TYPE_CHECKING:
    from mnt.pyfiction._native.inml import INMLLayout
    from mnt.pyfiction._native.mol_qca import MolecularQCALayout
    from mnt.pyfiction._native.qca import QCALayout
    from mnt.pyfiction._native.sidb import SiDBLayout


def _area(
    layout: QCALayout | MolecularQCALayout | INMLLayout | SiDBLayout,
    width: float,
    height: float,
    hspace: float,
    vspace: float,
) -> float:
    """Validate cell dimensions and compute physical area in nm².

    Args:
        layout: The cell-level layout.
        width: Cell width in nm.
        height: Cell height in nm.
        hspace: Horizontal spacing in nm.
        vspace: Vertical spacing in nm.

    Returns:
        The physical area in nm².

    Raises:
        ValueError: A dimension is negative or nonfinite.
    """
    if any(not isfinite(value) or value < 0 for value in (width, height, hspace, vspace)):
        msg = "cell dimensions and spacing must be finite and nonnegative"
        raise ValueError(msg)
    return _native_area(layout, width, height, hspace, vspace)


def qca_area(
    layout: QCALayout,
    *,
    width: float = 18.0,
    height: float = 18.0,
    hspace: float = 2.0,
    vspace: float = 2.0,
) -> float:
    """Compute the physical area in nm² from the layout extent.

    Args:
        layout: The cell-level layout.
        width: Cell width in nm.
        height: Cell height in nm.
        hspace: Horizontal spacing in nm.
        vspace: Vertical spacing in nm.

    Cell dimensions and spacing must be finite and nonnegative.

    Returns:
        The physical area in nm².
    """
    return _area(layout, width, height, hspace, vspace)


def mol_qca_area(
    layout: MolecularQCALayout,
    *,
    width: float = 2.0,
    height: float = 2.0,
    hspace: float = 0.0,
    vspace: float = 0.0,
) -> float:
    """Compute the physical area in nm² from the layout extent.

    Args:
        layout: The cell-level layout.
        width: Cell width in nm.
        height: Cell height in nm.
        hspace: Horizontal spacing in nm.
        vspace: Vertical spacing in nm.

    Cell dimensions and spacing must be finite and nonnegative.

    Returns:
        The physical area in nm².
    """
    return _area(layout, width, height, hspace, vspace)


def inml_area(
    layout: INMLLayout,
    *,
    width: float = 50.0,
    height: float = 100.0,
    hspace: float = 10.0,
    vspace: float = 25.0,
) -> float:
    """Compute the physical area in nm² from the layout extent.

    Args:
        layout: The cell-level layout.
        width: Cell width in nm.
        height: Cell height in nm.
        hspace: Horizontal spacing in nm.
        vspace: Vertical spacing in nm.

    Cell dimensions and spacing must be finite and nonnegative.

    Returns:
        The physical area in nm².
    """
    return _area(layout, width, height, hspace, vspace)


def sidb_area(
    layout: SiDBLayout,
    *,
    width: float = 0.0,
    height: float = 0.0,
    hspace: float = 0.384,
    vspace: float = 0.384,
) -> float:
    """Compute the physical area in nm² from the bounding box of SiDBs and defects.

    Args:
        layout: The cell-level layout.
        width: Cell width in nm.
        height: Cell height in nm.
        hspace: Horizontal spacing in nm.
        vspace: Vertical spacing in nm.

    Cell dimensions and spacing must be finite and nonnegative.

    Returns:
        The physical area in nm².
    """
    return _area(layout, width, height, hspace, vspace)
