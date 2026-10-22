# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""Readers and writers of iNML cell-level layouts."""

from collections.abc import Callable

import mnt.pyfiction.inml

def write_qcc_layout(
    layout: mnt.pyfiction.inml.inml_layout,
    filename: str,
    *,
    on_progress: Callable[[str, int, int], None] | None = None,
    use_filename_as_component_name: bool = False,
) -> None:
    """
    Write the iNML layout to the supplied ToPoliNano and MagCAD filename.
    use_filename_as_component_name selects the component name; on_progress reports writing progress.

    Args:
        layout: Layout to write.
        filename: Output file path.
        on_progress: Callback receiving (task, completed, total); None disables progress reporting.
        use_filename_as_component_name: Use the output filename as the component name inside the QCC file.

    Raises:
        ValueError: The layout has no designated I/O pins or the pins are not routed to its borders.
        RuntimeError: The file cannot be created, written, closed, or replaced.
    """
