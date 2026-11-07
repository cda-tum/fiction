# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""Network observations, names, and random generation through the bound API."""

from __future__ import annotations

from typing import TYPE_CHECKING, Any, cast

import pytest

from mnt.pyfiction import inml, layouts, mol_qca, networks, qca, sidb
from mnt.pyfiction.networks.io import read_network

if TYPE_CHECKING:
    from collections.abc import Callable
    from pathlib import Path

Network = networks.technology_network | networks.aig_network | networks.xag_network | networks.mig_network
NETWORK_TYPES = [networks.technology_network, networks.aig_network, networks.xag_network, networks.mig_network]


@pytest.mark.parametrize("cls", NETWORK_TYPES)
def test_empty_network(cls: type[Network]) -> None:
    """Empty networks contain constants but no terminals, gates, or output functions."""
    network = cls()
    constants = [0, 1] if cls is networks.technology_network else [0]
    assert network.nodes() == constants
    assert network.size() == len(constants)
    assert network.gates() == network.pis() == network.pos() == []
    assert (network.num_gates(), network.num_pis(), network.num_pos(), network.depth()) == (0, 0, 0, 0)
    assert all(network.is_constant(node) and not network.is_pi(node) for node in constants)
    assert [network.fanins(node) for node in constants] == [[] for _ in constants]
    assert not networks.get_name(network)
    assert networks.simulate_outputs(network) == []
    assert networks.simulate(network) == {}
    if isinstance(network, networks.technology_network):
        assert not network.is_pi(network.size())


@pytest.mark.parametrize("cls", NETWORK_TYPES)
def test_network_observations_and_names(tmp_path: Path, cls: type[Network]) -> None:
    """Observe a known one-gate circuit without relying on storage-specific gate IDs."""
    source = tmp_path / "conjunction.v"
    source.write_text(
        "module top(a,b,unused,y,z,t); input a,b,unused; output y,z,t; "
        "assign y = a & b; assign z = y; assign t = a; endmodule\n",
        encoding="utf-8",
    )
    network = read_network(str(source), network_type=cls)
    a, b, unused = network.pis()
    (gate,) = network.gates()
    constants = [node for node in network.nodes() if network.is_constant(node)]
    assert sorted(network.nodes()) == sorted([*constants, a, b, unused, gate])
    assert len(network.nodes()) == network.size()
    assert (network.num_pis(), network.num_pos(), network.num_gates(), network.depth()) == (3, 3, 1, 1)
    assert network.pos() == [gate, gate, a]
    expected_fanins = [*constants, a, b] if cls is networks.mig_network else [a, b]
    assert sorted(network.fanins(gate)) == sorted(expected_fanins)
    assert [network.fanins(node) for node in [*constants, a, b, unused]] == [[] for _ in [*constants, a, b, unused]]
    assert [node for node in network.nodes() if network.is_pi(node)] == [a, b, unused]
    assert not network.is_constant(gate)
    if cls is networks.mig_network:
        assert network.is_maj(gate)
    else:
        assert network.is_and(gate)
    assert networks.get_name(network) == "conjunction"
    assert [network.has_name(node) for node in network.pis()] == [True, True, True]
    assert [network.get_name(node) for node in network.pis()] == ["a", "b", "unused"]
    assert [network.has_output_name(index) for index in range(3)] == [True, True, True]
    assert [network.get_output_name(index) for index in range(3)] == ["y", "z", "t"]
    expected = [
        ("y", [True, False, False, False] * 2),
        ("z", [True, False, False, False] * 2),
        ("t", [True, False] * 4),
    ]
    assert networks.simulate_outputs(network) == expected
    if isinstance(network, networks.technology_network):
        assert [network.is_po(node) for node in [*constants, a, b, unused, gate]] == [
            False,
            False,
            True,
            False,
            False,
            True,
        ]
        assert [network.po_at(index) for index in range(3)] == [gate, gate, a]
        assert network.po_index(gate) == 0
        assert network.po_index(a) == 2
    # Native collection returns and simulation results are independent Python values.
    observations = [network.nodes, network.gates, network.pis, network.pos]
    for observation in observations:
        expected_nodes = observation()
        observation().clear()
        assert observation() == expected_nodes
    network.fanins(gate).clear()
    assert sorted(network.fanins(gate)) == sorted(expected_fanins)
    network.set_name(n=a, name="renamed\0input")
    network.set_name(n=gate, name="internal")
    network.set_output_name(index=1, name="renamed\0output")
    networks.set_name(ntk_or_lyt=network, name="renamed\0network")
    assert network.get_name(a) == "renamed\0input"
    assert network.get_name(b) == "b"
    assert network.has_name(gate)
    assert network.get_name(gate) == "internal"
    assert network.get_output_name(0) == "y"
    assert network.get_output_name(1) == "renamed\0output"
    assert networks.get_name(network) == "renamed\0network"
    expected[1] = ("renamed\0output", expected[1][1])
    assert networks.simulate_outputs(network) == expected
    networks.set_name(network, "")
    assert not networks.get_name(network)


def test_technology_gate_predicates(tmp_path: Path) -> None:
    """Each bound gate predicate distinguishes its function, including fanout buffers."""
    source = tmp_path / "gates.blif"
    source.write_text(
        ".model gates\n.inputs a b c unused\n.outputs buf inv and nand or nor xor xnor maj\n"
        ".names a buf\n1 1\n.names a inv\n0 1\n.names buf b and\n11 1\n"
        ".names buf b nand\n0- 1\n-0 1\n.names a b or\n1- 1\n-1 1\n"
        ".names a b nor\n00 1\n.names a b xor\n01 1\n10 1\n"
        ".names a b xnor\n00 1\n11 1\n.names a b c maj\n11- 1\n1-1 1\n-11 1\n.end\n",
        encoding="utf-8",
    )
    network = read_network(str(source))
    names = [network.get_output_name(index) for index in range(network.num_pos())]
    assert names == ["buf", "inv", "and", "nand", "or", "nor", "xor", "xnor", "maj"]
    predicates = [*names, "fanout"]
    expected_by_node = {
        node: [name, "fanout"] if name == "buf" else [name] for name, node in zip(names, network.pos(), strict=True)
    }
    for node in network.nodes():
        actual = [name for name in predicates if getattr(network, f"is_{name}")(node)]
        assert actual == expected_by_node.get(node, []), node
    expected_tables = [[], [], [], [], [], [], [], [], []]
    for assignment in range(15, -1, -1):
        a, b, c = (bool(assignment & (1 << bit)) for bit in range(3))
        row = [a, not a, a and b, not (a and b), a or b, not (a or b), a != b, a == b, a + b + c >= 2]
        for table, value in zip(expected_tables, row, strict=True):
            table.append(value)
    assert networks.simulate_outputs(network) == list(zip(names, expected_tables, strict=True))
    assert networks.has_high_degree_fanin_nodes(network)
    assert not networks.has_high_degree_fanin_nodes(ntk=network, threshold=3)
    assert networks.has_high_degree_fanin_nodes(network, threshold=1)
    assert not networks.has_high_degree_fanin_nodes(networks.technology_network(), threshold=0)
    assert issubclass(networks.high_degree_fanin_exception, ValueError)


@pytest.mark.parametrize("cls", [networks.aig_network, networks.xag_network, networks.mig_network])
def test_specialized_gate_predicates(tmp_path: Path, cls: type[Network]) -> None:
    """Predicates classify stored primitives, while complemented-edge functions remain observable by simulation."""
    source = tmp_path / "gates.v"
    source.write_text(
        "module top(a,b,y,z); input a,b; output y,z; assign y = a & b; assign z = a ^ b; endmodule\n",
        encoding="utf-8",
    )
    network = read_network(str(source), network_type=cls)
    for node in network.nodes():
        predicates = [name for name in ["and", "or", "xor", "maj"] if getattr(network, f"is_{name}")(node)]
        if node not in network.gates():
            assert predicates == []
        elif cls is networks.aig_network:
            assert predicates == ["and"]
        elif cls is networks.mig_network:
            assert predicates == ["maj"]
        else:
            assert predicates == (["and"] if node == network.pos()[0] else ["xor"])
    assert networks.simulate_outputs(network) == [
        ("y", [True, False, False, False]),
        ("z", [False, True, True, False]),
    ]


@pytest.mark.parametrize("cls", NETWORK_TYPES)
@pytest.mark.parametrize(("inputs", "gates", "seed"), [(2, 1, 0), (3, 5, 42)])
def test_random_network_contract(cls: type[Network], inputs: int, gates: int, seed: int) -> None:
    """Generators retain representation, requested size, seed determinism, and names."""
    network = networks.random_network(inputs=inputs, gates=gates, seed=seed, network_type=cls)
    repeated = networks.random_network(inputs=inputs, gates=gates, seed=seed, network_type=cls)
    assert type(network) is cls
    assert network.num_pis() == inputs
    assert network.num_gates() >= gates if cls is networks.technology_network else network.num_gates() == gates
    assert networks.get_name(network) == str(seed)
    assert network.nodes() == repeated.nodes()
    assert network.pis() == repeated.pis()
    assert network.pos() == repeated.pos()
    assert [network.fanins(node) for node in network.nodes()] == [repeated.fanins(node) for node in repeated.nodes()]
    outputs = networks.simulate_outputs(network)
    assert outputs
    assert len(outputs) == network.num_pos()
    assert [name for name, _ in outputs] == [f"po{index}" for index in range(network.num_pos())]
    assert all(len(bits) == 2**inputs and all(type(bit) is bool for bit in bits) for _, bits in outputs)
    assert outputs == networks.simulate_outputs(repeated)
    network.set_name(network.pis()[0], "first")
    assert not repeated.has_name(repeated.pis()[0])
    with pytest.raises(IndexError):
        repeated.get_name(repeated.pis()[0])


@pytest.mark.parametrize("cls", NETWORK_TYPES)
@pytest.mark.parametrize(("inputs", "gates"), [(0, 5), (1, 5), (3, 0)])
def test_random_network_rejects_invalid_sizes(cls: type[Network], inputs: int, gates: int) -> None:
    """Every representation rejects input counts and gate counts outside the generator domain."""
    with pytest.raises(ValueError, match="at least two inputs and one gate"):
        networks.random_network(inputs=inputs, gates=gates, seed=42, network_type=cls)


@pytest.mark.parametrize("selector", [None, "aig", int, networks.aig_network()])
def test_random_network_rejects_invalid_selectors(selector: Any) -> None:
    """Reject selectors that are not a registered network class."""
    with pytest.raises(TypeError):
        networks.random_network(3, 4, 42, network_type=selector)


def test_random_network_default_and_keyword_only_selector() -> None:
    """The default generator returns a technology network and selects types by keyword."""
    assert type(networks.random_network(3, 4, 42)) is networks.technology_network
    with pytest.raises(TypeError):
        cast("Any", networks.random_network)(3, 4, 42, networks.aig_network)


@pytest.mark.parametrize(
    "name", ["random_aig_network", "random_xag_network", "random_mig_network", "random_tec_network"]
)
def test_random_specialization_names_are_removed(name: str) -> None:
    """Expose one random generator name for every representation."""
    assert not hasattr(networks, name)


@pytest.mark.parametrize(
    "make_layout",
    [
        layouts.cartesian_gate_layout,
        pytest.param(lambda: layouts.shifted_cartesian_gate_layout(layouts.arrangement.EVEN_ROW), id="shifted"),
        pytest.param(lambda: layouts.hexagonal_gate_layout(layouts.arrangement.EVEN_ROW), id="hexagonal"),
        qca.qca_layout,
        mol_qca.mol_qca_layout,
        inml.inml_layout,
        sidb.sidb_layout,
    ],
)
def test_name_helpers_accept_all_bound_layout_types(make_layout: Callable[[], Any]) -> None:
    """Name helpers retain complete names and clear names on every supported layout type."""
    layout = make_layout()
    assert not networks.get_name(layout)
    networks.set_name(layout, "layout\0name")
    assert networks.get_name(layout) == "layout\0name"
    assert layout.get_layout_name() == "layout\0name"
    networks.set_name(layout, "")
    assert not networks.get_name(layout)
