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
    Write an SVG representation of the molQCA layout to the supplied filename.
    simple selects a compact drawing; on_progress reports writing progress.
    """
