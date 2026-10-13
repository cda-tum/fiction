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
    """
