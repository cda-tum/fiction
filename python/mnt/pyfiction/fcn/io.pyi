# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""Technology-independent readers and writers of cell-level layouts."""

from collections.abc import Callable
from typing import overload

import mnt.pyfiction.inml
import mnt.pyfiction.mol_qca
import mnt.pyfiction.qca

@overload
def write_qll_layout(
    layout: mnt.pyfiction.qca.qca_layout, filename: str, on_progress: Callable[[str, int, int], None] | None = None
) -> None: ...
@overload
def write_qll_layout(
    layout: mnt.pyfiction.inml.inml_layout, filename: str, on_progress: Callable[[str, int, int], None] | None = None
) -> None: ...
@overload
def write_qll_layout(
    layout: mnt.pyfiction.mol_qca.mol_qca_layout,
    filename: str,
    on_progress: Callable[[str, int, int], None] | None = None,
) -> None:
    """
    Writes a QCA, molQCA, or iNML layout to a qll file that is used by
    ToPoliNano & MagCAD (https://topolinano.polito.it/), an EDA tool and a
    physical simulator for the iNML technology platform as well as SCERPA
    (https://ieeexplore.ieee.org/document/8935211), a physical simulator
    for the molQCA (mQCA) technology platform.

    This overload uses a file name to create and write into.

    Args:
        layout: The layout to be written.
        filename: The file name to create and write into. Should
                  preferably use the `.qll` extension.
        on_progress: Receives completed serialization work.

    Template Args:
        Lyt: `qca::layout`, `mol_qca::layout`, or `inml::layout`.

    Raises:
        ValueError: If an iNML input or output cell is not at its
                    designated horizontal border.
        RuntimeError: If creating, writing, closing, or replacing the
                      output file fails.
    """
