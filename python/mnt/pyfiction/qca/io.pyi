# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""Readers and writers of QCA cell-level layouts."""

from collections.abc import Callable

import mnt.pyfiction.qca

def write_qca_layout(
    layout: mnt.pyfiction.qca.qca_layout,
    filename: str,
    *,
    on_progress: Callable[[str, int, int], None] | None = None,
    create_inter_layer_via_cells: bool = True,
) -> None:
    """
    Writes a QCA layout to a qca file that is used by QCADesigner
    (https://waluslab.ece.ubc.ca/qcadesigner/), a physical simulator for
    the QCA technology platform.

    This overload uses an output stream to write into.

    Args:
        lyt: The layout to be written.
        os: The output stream to write into.

    Algorithm options are keyword-only; the signature lists their defaults.
    """

def write_qca_layout_svg(
    layout: mnt.pyfiction.qca.qca_layout,
    filename: str,
    *,
    on_progress: Callable[[str, int, int], None] | None = None,
    simple: bool = False,
) -> None:
    """
    Writes an SVG representation of a QCA layout into an output stream.
    Both tile- and cell-based layouts are supported. For tile-based
    layouts, QCA layouts of tile size :math:`5 \\times 5` are supported
    exclusively so far.

    The utilized color scheme is based on the standard scheme used in
    QCADesigner (https://waluslab.ece.ubc.ca/qcadesigner/).

    May throw an `unsupported_cell_type_exception` if it encounters
    unsupported cell types in the layout.

    Args:
        lyt: The layout to be written.
        os: The output stream to write into.

    Algorithm options are keyword-only; the signature lists their defaults.
    """
