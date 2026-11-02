# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

from __future__ import annotations

from typing import TYPE_CHECKING

import pytest

from mnt.pyfiction.qca import qca_cell_type, qca_layout
from mnt.pyfiction.qca.io import write_qca_layout

if TYPE_CHECKING:
    from pathlib import Path


@pytest.mark.parametrize("name", ["a\nb", "a\rb", "a\0b"])
@pytest.mark.parametrize("cell_name", [False, True])
def test_reject_invalid_names_without_changing_file(tmp_path: Path, name: str, *, cell_name: bool) -> None:
    """Reject names that cannot fit one QCADesigner text record."""
    layout = qca_layout((1, 1), "OPEN", "valid" if cell_name else name)
    layout.assign_cell_type((0, 0), qca_cell_type.INPUT)
    layout.assign_cell_name((0, 0), name if cell_name else "valid")
    destination = tmp_path / "layout.qca"
    destination.write_text("sentinel", encoding="utf-8")

    with pytest.raises(ValueError, match="line breaks or null bytes"):
        write_qca_layout(layout, str(destination))

    assert destination.read_text(encoding="utf-8") == "sentinel"
