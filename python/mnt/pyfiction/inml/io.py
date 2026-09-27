# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""Readers and writers of iNML cell-level layouts."""

from __future__ import annotations

from os import PathLike, fspath
from typing import TYPE_CHECKING

from mnt.pyfiction._native.fcn.io import write_qll_layout as _write_qll
from mnt.pyfiction._native.inml import io as _native

if TYPE_CHECKING:
    from collections.abc import Callable

    from mnt.pyfiction.inml import INMLLayout

from mnt.pyfiction._native.inml.io import (
    WriteQccLayoutParams,
)

__all__ = [
    "WriteQccLayoutParams",
    "write_qcc_layout",
    "write_qll_layout",
]


def write_qcc_layout(
    layout: INMLLayout, path: str | PathLike[str], *, params: WriteQccLayoutParams | None = None
) -> None:
    """Write an iNML layout as QCA-One.

    Args:
        layout: Cell-level layout to serialize.
        path: Output file.
        params: Writer options. None uses the native defaults.
    """
    _native.write_qcc_layout(layout, fspath(path), params if params is not None else WriteQccLayoutParams())


def write_qll_layout(
    layout: INMLLayout, path: str | PathLike[str], *, on_progress: Callable[[str, int, int], None] | None = None
) -> None:
    """Write an iNML layout for ToPoliNano and MagCAD.

    Args:
        layout: Cell-level layout to serialize.
        path: QLL output file.
        on_progress: Optional progress callback; exceptions propagate to the caller.
    """
    _write_qll(layout, fspath(path), on_progress)
