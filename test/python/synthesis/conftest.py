# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""Small specifications shared by synthesis tests."""

from __future__ import annotations

from typing import TYPE_CHECKING

import pytest

from mnt.pyfiction.networks.io import read_network

if TYPE_CHECKING:
    from collections.abc import Callable
    from pathlib import Path

    from mnt.pyfiction.networks import technology_network


@pytest.fixture
def blif_network(tmp_path: Path) -> Callable[[str], technology_network]:
    """Read a fresh technology network from each small BLIF specification.

    Returns:
        A reader for small BLIF specifications.
    """

    def read(contents: str) -> technology_network:
        path = tmp_path / "synthesis.blif"
        path.write_text(contents, encoding="utf-8")
        return read_network(str(path))

    return read


@pytest.fixture
def interface_outputs() -> list[tuple[str, list[bool]]]:
    """Expected functions of ``network_interfaces.blif``, including its unused input.

    Returns:
        Ordered output names and Boolean values.
    """
    assignments = range(15, -1, -1)
    return [
        ("zero", [False] * 16),
        ("one", [True] * 16),
        ("inverted", [not (i & 1) for i in assignments]),
        ("xor", [bool(i & 1) != bool(i & 2) for i in assignments]),
        ("majority", [(i & 7).bit_count() >= 2 for i in assignments]),
        ("lut", [(i & 7) in (2, 4, 7) for i in assignments]),
        ("copy", [bool(i & 1) for i in assignments]),
    ]
