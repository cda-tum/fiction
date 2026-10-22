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
    Write the QCA layout to the supplied QCADesigner filename.
    create_inter_layer_via_cells controls via cells; on_progress reports writing progress.

    Args:
        layout: Layout to write.
        filename: Output file path.
        on_progress: Callback receiving (task, completed, total); None disables progress reporting.
        create_inter_layer_via_cells: Create via cells in between each layer.

    Raises:
        RuntimeError: The file cannot be created, written, closed, or replaced.
    """

def write_qca_layout_svg(
    layout: mnt.pyfiction.qca.qca_layout,
    filename: str,
    *,
    on_progress: Callable[[str, int, int], None] | None = None,
    simple: bool = False,
) -> None:
    """
    Write an SVG representation of the QCA layout to the supplied filename.
    simple selects a compact drawing; on_progress reports writing progress.

    Args:
        layout: Layout to write.
        filename: Output file path.
        on_progress: Callback receiving (task, completed, total); None disables progress reporting.
        simple: Use a compact SVG drawing.

    Raises:
        RuntimeError: The file cannot be created, written, closed, or replaced, or the layout contains
            an unsupported cell type.
    """
