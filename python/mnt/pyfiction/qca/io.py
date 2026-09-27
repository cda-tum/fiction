# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""Readers and writers of QCA cell-level layouts."""

from __future__ import annotations

from os import PathLike, fspath
from typing import TYPE_CHECKING

from mnt.pyfiction._native.fcn.io import write_qll_layout as _write_qll
from mnt.pyfiction._native.qca import io as _native

if TYPE_CHECKING:
    from collections.abc import Callable

    from mnt.pyfiction.qca import QCALayout

from mnt.pyfiction._native.qca.io import (
    QcaWriterParams,
    SvgParams,
)

__all__ = [
    "QcaWriterParams",
    "SvgParams",
    "write_qca_layout",
    "write_qca_layout_svg",
    "write_qll_layout",
]


def write_qca_layout(layout: QCALayout, path: str | PathLike[str], *, params: QcaWriterParams | None = None) -> None:
    """Write a QCA layout as QCADesigner.

    Args:
        layout: Cell-level layout to serialize.
        path: Output file.
        params: Writer options. None uses the native defaults.
    """
    _native.write_qca_layout(layout, fspath(path), params if params is not None else QcaWriterParams())


def write_qca_layout_svg(layout: QCALayout, path: str | PathLike[str], *, params: SvgParams | None = None) -> None:
    """Write a QCA layout as SVG.

    Args:
        layout: Cell-level layout to serialize.
        path: Output file.
        params: Writer options. None uses the native defaults.
    """
    _native.write_qca_layout_svg(layout, fspath(path), params if params is not None else SvgParams())


def write_qll_layout(
    layout: QCALayout, path: str | PathLike[str], *, on_progress: Callable[[str, int, int], None] | None = None
) -> None:
    """Write a QCA layout for ToPoliNano and MagCAD.

    Args:
        layout: Cell-level layout to serialize.
        path: QLL output file.
        on_progress: Optional progress callback; exceptions propagate to the caller.
    """
    _write_qll(layout, fspath(path), on_progress)
