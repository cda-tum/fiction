# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""Python bindings for fiction, a framework for Design Automation for Field-coupled Nanotechnologies.

Domain modules load on first access and expose the supported FCN tools.
"""

from __future__ import annotations

from importlib import import_module
from typing import TYPE_CHECKING

from ._native.utils import __version__

if TYPE_CHECKING:
    from . import (
        inml,
        layouts,
        mol_qca,
        networks,
        physical_design,
        qca,
        sidb,
        synthesis,
        verification,
    )

_LAZY_SUBMODULES = {
    "inml",
    "layouts",
    "mol_qca",
    "networks",
    "physical_design",
    "qca",
    "sidb",
    "synthesis",
    "verification",
}

__all__ = [
    "__version__",
    "inml",
    "layouts",
    "mol_qca",
    "networks",
    "physical_design",
    "qca",
    "sidb",
    "synthesis",
    "verification",
]


def __getattr__(name: str) -> object:
    if name in _LAZY_SUBMODULES:
        module = import_module(f"{__name__}.{name}")
        globals()[name] = module
        return module
    msg = f"module {__name__!r} has no attribute {name!r}"
    raise AttributeError(msg)


def __dir__() -> list[str]:
    return sorted(set(globals()) | _LAZY_SUBMODULES)
