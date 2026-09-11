# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""The four stores of the shell and the one description of each element type.

The dictionaries the ``describe_*`` functions return feed ``store``, ``ps``, and the JSON log alike,
so the three never disagree.
"""

from __future__ import annotations

from dataclasses import dataclass
from typing import TYPE_CHECKING, Any, Generic, TypeVar

from mnt.pyfiction import (
    aig_network,
    cartesian_gate_layout,
    charge_distribution,
    critical_path_length_and_throughput,
    dynamic_truth_table,
    get_name,
    hexagonal_gate_layout,
    inml_layout,
    mig_network,
    mol_qca_layout,
    qca_layout,
    row_of,
    shifted_cartesian_gate_layout,
    sidb_layout,
    sidb_simulation_result,
    technology_network,
    xag_network,
)

from .errors import CommandError

if TYPE_CHECKING:
    from collections.abc import Iterator

# The binding classes reach mypy as ``Any``, so the unions below are documentation for readers;
# ``isinstance`` checks use the ``*_TYPES`` tables.
Network = Any
"""``aig_network | xag_network | mig_network | technology_network``."""
GateLayout = Any
"""``cartesian_gate_layout | shifted_cartesian_gate_layout | hexagonal_gate_layout``."""
CellLayout = Any
"""``qca_layout | inml_layout | mol_qca_layout | sidb_layout``."""

NETWORK_TYPES: dict[type, str] = {
    aig_network: "AIG",
    xag_network: "XAG",
    mig_network: "MIG",
    technology_network: "TEC",
}
"""The network classes and the type names ``read --type`` accepts, lowercased."""

TOPOLOGIES: dict[type, str] = {
    cartesian_gate_layout: "cartesian",
    shifted_cartesian_gate_layout: "shifted_cartesian",
    hexagonal_gate_layout: "hexagonal",
}
"""The gate-level layout classes and the names ``--topology`` accepts."""

TECHNOLOGIES: dict[type, str] = {
    qca_layout: "QCA",
    inml_layout: "iNML",
    mol_qca_layout: "molQCA",
    sidb_layout: "SiDB",
}
"""The cell-level layout classes and their technology names."""

MAX_PRINTED_TRUTH_TABLE_VARS = 8
"""Truth tables of more variables are described by their hex string alone."""

T = TypeVar("T")


@dataclass
class CellEntry:
    """An element of the cell-level layout store: a layout and, once simulated, its simulation result.

    Attributes:
        layout: The cell-level layout.
        result: The physical simulation result of an SiDB layout; ``None`` until an engine ran.
        engine: The name of the engine that produced ``result``.
    """

    layout: CellLayout
    result: sidb_simulation_result | None = None
    engine: str | None = None


class Store(Generic[T]):
    """A list of elements with one active element, the one commands work on.

    Attributes:
        kind: What the store holds, for messages.
        items: The elements in insertion order.
        active: The index of the active element; ``None`` while the store is empty.
    """

    def __init__(self, kind: str) -> None:
        """Create an empty store.

        Args:
            kind: What the store holds, for messages.
        """
        self.kind = kind
        self.items: list[T] = []
        self.active: int | None = None

    def __len__(self) -> int:
        """Return the number of elements.

        Returns:
            The element count.
        """
        return len(self.items)

    def __iter__(self) -> Iterator[T]:
        """Iterate over the elements in insertion order.

        Returns:
            An iterator over the elements.
        """
        return iter(self.items)

    def add(self, item: T) -> None:
        """Append an element and make it the active one.

        Args:
            item: The element.
        """
        self.items.append(item)
        self.active = len(self.items) - 1

    def current(self) -> T:
        """Return the active element.

        Returns:
            The active element.

        Raises:
            CommandError: When the store is empty.
        """
        if self.active is None:
            msg = f"no {self.kind} in store"
            raise CommandError(msg)
        return self.items[self.active]

    def select(self, index: int) -> None:
        """Make an element the active one.

        Args:
            index: The element's position, as ``store`` lists it.

        Raises:
            CommandError: When the index is out of range.
        """
        if not 0 <= index < len(self.items):
            msg = f"{self.kind} index {index} is out of range; the store holds {len(self.items)}"
            raise CommandError(msg)
        self.active = index

    def pop(self) -> T:
        """Remove the active element and make its predecessor, or the new last element, active.

        Returns:
            The removed element.

        Raises:
            CommandError: When the store is empty.
        """
        if self.active is None:
            msg = f"no {self.kind} in store"
            raise CommandError(msg)
        item = self.items.pop(self.active)
        self.active = min(self.active, len(self.items) - 1) if self.items else None
        return item

    def clear(self) -> None:
        """Remove every element."""
        self.items.clear()
        self.active = None

    def summary(self) -> str:
        """Return the element count and the active element's name, for the status line.

        Returns:
            For example ``networks: 2 (mux21)``.
        """
        count = f"{self.kind}s: {len(self.items)}"
        if self.active is None:
            return count
        name = element_name(self.items[self.active])
        return f"{count} ({name})" if name else count


def element_name(element: object) -> str:
    """Return the name of a store element, or an empty string for elements without one.

    This is where every ``name`` in a description comes from, so the status line, ``store``, and the
    log never disagree. It stays cheap, because the status line asks for it on every keystroke.

    Args:
        element: A truth table, network, layout, or cell entry.

    Returns:
        The name.
    """
    if isinstance(element, CellEntry):
        element = element.layout
    if isinstance(element, NETWORK_TYPES_TUPLE):
        return str(get_name(element))
    if isinstance(element, GATE_LAYOUT_TYPES_TUPLE + CELL_LAYOUT_TYPES_TUPLE):
        layout: Any = element
        return str(layout.get_layout_name())
    return ""


NETWORK_TYPES_TUPLE = tuple(NETWORK_TYPES)
GATE_LAYOUT_TYPES_TUPLE = tuple(TOPOLOGIES)
CELL_LAYOUT_TYPES_TUPLE = tuple(TECHNOLOGIES)


def describe_truth_table(tt: dynamic_truth_table) -> dict[str, object]:
    """Describe a truth table.

    Args:
        tt: The truth table.

    Returns:
        ``vars`` and ``hex``, plus ``binary`` for up to :data:`MAX_PRINTED_TRUTH_TABLE_VARS` variables.
    """
    description: dict[str, object] = {"vars": tt.num_vars(), "hex": tt.to_hex()}
    if tt.num_vars() <= MAX_PRINTED_TRUTH_TABLE_VARS:
        description["binary"] = tt.to_binary()
    return description


def describe_network(network: Network) -> dict[str, object]:
    """Describe a logic network.

    Args:
        network: The network.

    Returns:
        ``name``, ``type``, ``inputs``, ``outputs``, ``gates``, and ``depth``.
    """
    return {
        "name": element_name(network),
        "type": NETWORK_TYPES[type(network)],
        "inputs": network.num_pis(),
        "outputs": network.num_pos(),
        "gates": network.num_gates(),
        "depth": network.depth(),
    }


def describe_gate_layout(layout: GateLayout) -> dict[str, object]:
    """Describe a gate-level layout.

    Args:
        layout: The layout.

    Returns:
        Name, topology, clocking scheme, size, I/O and gate counts, critical path, and throughput.
        ``throughput`` is the denominator ``x`` of the throughput ``1/x``.
    """
    critical_path, throughput = critical_path_length_and_throughput(layout)
    return {
        "name": element_name(layout),
        "topology": TOPOLOGIES[type(layout)],
        "clocking": layout.get_clocking_scheme_name(),
        "size": {"x": layout.x() + 1, "y": layout.y() + 1, "area": layout.area()},
        "inputs": layout.num_pis(),
        "outputs": layout.num_pos(),
        "gates": layout.num_gates(),
        "wires": layout.num_wires(),
        "crossings": layout.num_crossings(),
        "critical_path": critical_path,
        "throughput": throughput,
    }


def describe_cell_layout(entry: CellEntry) -> dict[str, object]:
    """Describe a cell-level layout and, when present, its simulation result.

    Args:
        entry: The store element.

    Returns:
        Name, technology, size, I/O and dot or cell counts; for SiDB layouts the lattice and defect count;
        and a ``simulation`` section once simulated.
    """
    layout = entry.layout
    description: dict[str, object] = {
        "name": element_name(entry),
        "technology": TECHNOLOGIES[type(layout)],
    }
    if isinstance(layout, sidb_layout):
        description["lattice"] = layout.get_lattice().name
        if layout.is_empty():
            description["size"] = {"x": 0, "y": 0}
        else:
            north_west, south_east = layout.bounding_box()
            description["size"] = {
                "x": south_east.x - north_west.x + 1,
                "y": row_of(south_east) - row_of(north_west) + 1,
            }
    else:
        description["size"] = {"x": layout.x() + 1, "y": layout.y() + 1, "z": layout.z() + 1, "area": layout.area()}
    description["inputs"] = layout.num_pis()
    description["outputs"] = layout.num_pos()
    if isinstance(layout, sidb_layout):
        description["dots"] = layout.num_dots()
        description["defects"] = layout.num_defects()
    else:
        description["cells"] = layout.num_cells()
    if entry.result is not None:
        description["simulation"] = describe_simulation(entry)
    return description


def describe_simulation(entry: CellEntry) -> dict[str, object]:
    """Describe the simulation result attached to a cell entry.

    Args:
        entry: A store element with a result.

    Returns:
        The engine, the number of physically valid charge distributions, the ground state energy in
        eV, and the runtime in seconds.
    """
    result = entry.result
    if result is None:
        return {}
    ground_states = result.groundstates()
    return {
        "engine": entry.engine,
        "stable_states": len(result.charge_distributions),
        "ground_state_energy_ev": ground_states[0].energy() if ground_states else None,
        "runtime_s": result.simulation_runtime.total_seconds(),
    }


def ground_state(entry: CellEntry) -> charge_distribution | None:
    """Return the ground state of a simulated cell entry.

    Args:
        entry: The store element.

    Returns:
        The lowest-energy charge distribution, or ``None`` when the entry is not simulated or the
        simulation found no physically valid state.
    """
    if entry.result is None:
        return None
    states = entry.result.groundstates()
    return states[0] if states else None


def describe(element: object) -> dict[str, object]:
    """Describe any store element.

    Args:
        element: A truth table, network, gate-level layout, or cell entry.

    Returns:
        The element's description.

    Raises:
        TypeError: When the element is of no store type.
    """
    if isinstance(element, dynamic_truth_table):
        return describe_truth_table(element)
    if isinstance(element, NETWORK_TYPES_TUPLE):
        return describe_network(element)
    if isinstance(element, GATE_LAYOUT_TYPES_TUPLE):
        return describe_gate_layout(element)
    if isinstance(element, CellEntry):
        return describe_cell_layout(element)
    msg = f"cannot describe {type(element).__name__}"
    raise TypeError(msg)


def one_line(description: dict[str, object]) -> str:
    """Render a description as the one line ``store`` prints.

    Args:
        description: A dictionary from :func:`describe`.

    Returns:
        The line, e.g. ``mux21 (TEC) - I/O: 3/1, gates: 3, depth: 2``.
    """
    parts: list[str] = []
    name = description.get("name")
    kind = description.get("type") or description.get("technology") or description.get("clocking")
    head = f"{name} ({kind})" if name else str(kind or "")
    size = description.get("size")
    if isinstance(size, dict):
        dims = [str(size[axis]) for axis in ("x", "y", "z") if axis in size and (axis != "z" or size[axis] != 1)]
        parts.append(" x ".join(dims))
    if "inputs" in description:
        parts.append(f"I/O: {description['inputs']}/{description['outputs']}")
    parts.extend(
        f"{key}: {description[key]}"
        for key in ("gates", "wires", "crossings", "depth", "dots", "cells", "vars", "hex")
        if key in description
    )
    if "critical_path" in description:
        parts.append(f"CP: {description['critical_path']}, TP: 1/{description['throughput']}")
    simulation = description.get("simulation")
    if isinstance(simulation, dict):
        parts.append(f"simulated with {simulation['engine']}: {simulation['stable_states']} stable states")
    body = ", ".join(parts)
    return f"{head} - {body}" if head and body else head or body
