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

class write_qca_layout_params:
    """Parameters for writing QCADesigner layouts."""

    def __init__(self) -> None:
        """Default constructor."""

    @property
    def on_progress(self, /) -> Callable[[str, int, int], None] | None:
        """Receives serialization progress."""

    @on_progress.setter
    def on_progress(self, value: Callable[[str, int, int], None] | None) -> None: ...
    @property
    def create_inter_layer_via_cells(self) -> bool:
        """Create via cells in between each layer."""

    @create_inter_layer_via_cells.setter
    def create_inter_layer_via_cells(self, arg: bool, /) -> None: ...

def write_qca_layout(
    layout: mnt.pyfiction.qca.qca_layout, filename: str, params: write_qca_layout_params = ...
) -> None:
    """
    Writes a QCA layout to a qca file that is used by QCADesigner
    (https://waluslab.ece.ubc.ca/qcadesigner/), a physical simulator for
    the QCA technology platform.

    This overload uses an output stream to write into.

    Args:
        lyt: The layout to be written.
        os: The output stream to write into.
        ps: Parameters.
    """

class write_qca_layout_svg_params:
    """Parameters for writing SVG QCA layouts."""

    def __init__(self) -> None:
        """Default constructor."""

    @property
    def on_progress(self, /) -> Callable[[str, int, int], None] | None:
        """Receives serialization progress."""

    @on_progress.setter
    def on_progress(self, value: Callable[[str, int, int], None] | None) -> None: ...
    @property
    def simple(self) -> bool:
        """Limit details to create smaller file sizes."""

    @simple.setter
    def simple(self, arg: bool, /) -> None: ...

def write_qca_layout_svg(
    layout: mnt.pyfiction.qca.qca_layout, filename: str, params: write_qca_layout_svg_params = ...
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
        ps: Parameters.
    """
