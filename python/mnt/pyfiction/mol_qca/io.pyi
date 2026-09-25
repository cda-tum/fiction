# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""Writers of molecular QCA cell-level layouts."""

from collections.abc import Callable

import mnt.pyfiction.mol_qca

class write_mol_qca_layout_svg_params:
    """Parameters for writing SVG molQCA layouts."""

    def __init__(self) -> None:
        """Default constructor."""

    @property
    def on_progress(self) -> Callable[[str, int, int], None] | None:
        """Receives serialization progress."""

    @on_progress.setter
    def on_progress(self, arg: Callable[[str, int, int], None], /) -> None: ...
    @property
    def simple(self) -> bool:
        """Limit details to create smaller file sizes."""

    @simple.setter
    def simple(self, arg: bool, /) -> None: ...

def write_mol_qca_layout_svg(
    layout: mnt.pyfiction.mol_qca.mol_qca_layout, filename: str, params: write_mol_qca_layout_svg_params = ...
) -> None:
    """
    Writes an SVG representation of a molQCA layout to an output stream.
    Each regular cell is colored by its clock phase.

    May throw an `unsupported_cell_type_exception` if it encounters
    unsupported cell types in the layout.

    Args:
        lyt: The layout to be written.
        os: The output stream to write into.
        ps: Parameters.
    """
