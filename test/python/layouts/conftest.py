# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

from __future__ import annotations

from functools import partial
from typing import TYPE_CHECKING

import pytest

from mnt.pyfiction.layouts import (
    arrangement,
    cartesian_gate_layout,
    cartesian_layout,
    hexagonal_gate_layout,
    hexagonal_layout,
    shifted_cartesian_gate_layout,
    shifted_cartesian_layout,
)

if TYPE_CHECKING:
    from collections.abc import Callable


@pytest.fixture(
    params=[cartesian_layout]
    + [partial(cls, a) for cls in (hexagonal_layout, shifted_cartesian_layout) for a in arrangement],
    ids=["cartesian"]
    + [f"{cls.__name__}-{a.name}" for cls in (hexagonal_layout, shifted_cartesian_layout) for a in arrangement],
)
def geometry_factory(request: pytest.FixtureRequest) -> Callable:
    """Return a constructor for independent geometry of one supported arrangement."""
    return request.param


@pytest.fixture(
    params=[cartesian_gate_layout]
    + [partial(cls, a) for cls in (hexagonal_gate_layout, shifted_cartesian_gate_layout) for a in arrangement],
    ids=["cartesian"]
    + [
        f"{cls.__name__}-{a.name}"
        for cls in (hexagonal_gate_layout, shifted_cartesian_gate_layout)
        for a in arrangement
    ],
)
def gate_factory(request: pytest.FixtureRequest) -> Callable:
    """Return a constructor for independent gate layouts of one supported arrangement."""
    return request.param
