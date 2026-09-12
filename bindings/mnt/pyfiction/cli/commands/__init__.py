# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""The commands of the ``fiction`` shell, one module per ``help`` category.

Importing this package registers every command.
"""

from __future__ import annotations

from . import files, general, logic, physical_design, simulation, technology, verification

__all__ = ["files", "general", "logic", "physical_design", "simulation", "technology", "verification"]
