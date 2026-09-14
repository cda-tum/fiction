# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""Shared command helpers."""

from __future__ import annotations

from typing import TYPE_CHECKING

if TYPE_CHECKING:
    from mnt.pyfiction.cli.session import Session
    from mnt.pyfiction.cli.stores import Store


def stores_of(session: Session) -> dict[str, Store]:  # type: ignore[type-arg]
    """Map the store flag names onto the session's stores.

    Args:
        session: The session.

    Returns:
        The stores by their :data:`~mnt.pyfiction.cli.registry.STORE_FLAGS` key.
    """
    return {
        "truth_table": session.truth_tables,
        "network": session.networks,
        "gate_layout": session.gate_layouts,
        "cell_layout": session.cell_layouts,
    }
