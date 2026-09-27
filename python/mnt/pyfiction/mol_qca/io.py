# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""Writers of molecular QCA cell-level layouts."""

from __future__ import annotations

from os import PathLike, fspath
from typing import TYPE_CHECKING

from mnt.pyfiction._native.fcn.io import write_qll_layout as _write_qll
from mnt.pyfiction._native.mol_qca import io as _native

if TYPE_CHECKING:
    from collections.abc import Callable

    from mnt.pyfiction.mol_qca import MolecularQCALayout

from mnt.pyfiction._native.mol_qca.io import (
    WriteMolQcaLayoutSvgParams,
)

__all__ = [
    "WriteMolQcaLayoutSvgParams",
    "write_mol_qca_layout_svg",
    "write_qll_layout",
]


def write_mol_qca_layout_svg(
    layout: MolecularQCALayout, path: str | PathLike[str], *, params: WriteMolQcaLayoutSvgParams | None = None
) -> None:
    """Write a molQCA layout as SVG.

    Args:
        layout: Cell-level layout to serialize.
        path: Output file.
        params: Writer options. None uses the native defaults.
    """
    _native.write_mol_qca_layout_svg(
        layout, fspath(path), params if params is not None else WriteMolQcaLayoutSvgParams()
    )


def write_qll_layout(
    layout: MolecularQCALayout, path: str | PathLike[str], *, on_progress: Callable[[str, int, int], None] | None = None
) -> None:
    """Write a molQCA layout for ToPoliNano and MagCAD.

    Args:
        layout: Cell-level layout to serialize.
        path: QLL output file.
        on_progress: Optional progress callback; exceptions propagate to the caller.
    """
    _write_qll(layout, fspath(path), on_progress)
