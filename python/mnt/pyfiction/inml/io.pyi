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

class write_qcc_layout_params:
    """Parameters for writing QCC layouts."""

    def __init__(self) -> None:
        """Default constructor."""

    @property
    def on_progress(self) -> Callable[[str, int, int], None] | None:
        """Receives serialization progress."""

    @on_progress.setter
    def on_progress(self, arg: Callable[[str, int, int], None], /) -> None: ...
    @property
    def use_filename_as_component_name(self) -> bool:
        """Use the given filename as the component name inside the QCC file."""

    @use_filename_as_component_name.setter
    def use_filename_as_component_name(self, arg: bool, /) -> None: ...

def write_qcc_layout(
    layout: mnt.pyfiction.inml.inml_layout, filename: str, params: write_qcc_layout_params = ...
) -> None:
    """
    Writes a cell-level iNML layout to a qcc file that is used by
    ToPoliNano & MagCAD (https://topolinano.polito.it/), an EDA tool and a
    physical simulator for the iNML technology platform.

    This overload uses an output stream to write into.

    Args:
        lyt: The layout to be written.
        os: The output stream to write into.
        ps: Parameters.
    """
