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

def write_mol_qca_layout_svg(
    layout: mnt.pyfiction.mol_qca.mol_qca_layout,
    filename: str,
    *,
    on_progress: Callable[[str, int, int], None] | None = None,
    simple: bool = False,
) -> None:
    """
    Writes an SVG representation of a molQCA layout to an output stream.
    Each regular cell is colored by its clock phase.

    May throw an `unsupported_cell_type_exception` if it encounters
    unsupported cell types in the layout.

    Args:
        lyt: The layout to be written.
        os: The output stream to write into.

    Algorithm options are keyword-only; the signature lists their defaults.
    """
