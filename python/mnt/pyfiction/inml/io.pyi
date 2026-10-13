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
    Writes a cell-level iNML layout to a qcc file that is used by
    ToPoliNano & MagCAD (https://topolinano.polito.it/), an EDA tool and a
    physical simulator for the iNML technology platform.

    This overload uses an output stream to write into.

    Args:
        lyt: The layout to be written.
        os: The output stream to write into.

    Algorithm options are keyword-only; the signature lists their defaults.
    """
