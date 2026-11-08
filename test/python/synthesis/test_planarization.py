# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""Tests for the planarization pipeline."""

from __future__ import annotations

from collections import Counter
from datetime import timedelta
from itertools import combinations, starmap
from typing import TYPE_CHECKING, Any

import pytest

from mnt.pyfiction.networks import simulate_outputs, technology_network
from mnt.pyfiction.networks.io import read_network
from mnt.pyfiction.synthesis import (
    decision_criterion,
    fanout_substitution,
    is_balanced,
    is_fanout_substituted,
    network_balancing,
    output_order,
    planarization,
    planarization_stats,
    planarization_strategy,
)

if TYPE_CHECKING:
    from collections.abc import Callable
    from pathlib import Path


UNIFIED: dict[str, Any] = {
    "unify_outputs": True,
    "buffer_constant_outputs": False,
}


def _balanced(network: technology_network) -> technology_network:
    return network_balancing(network, **UNIFIED)


def _equivalent_with_tied_inputs(
    original: technology_network, planar: technology_network, original_input: list[int]
) -> bool:
    """Compare the truth tables with every input of ``planar`` tied to the original input it stands for.

    ``simulate_outputs`` lists Boolean values from the highest assignment down, and input ``j`` of a
    network is bit ``j`` of the table index.

    Returns:
        Whether every output agrees for every assignment of the original inputs.
    """
    # These examples have at most 12 flattened inputs; larger examples need tied-input SAT checking.
    assert planar.num_pis() <= 12
    assert len(original_input) == planar.num_pis()
    assert sorted(set(original_input)) == list(range(original.num_pis()))
    num_original = original.num_pis()
    original_tables = [bits for _, bits in simulate_outputs(original)]
    planar_tables = [bits for _, bits in simulate_outputs(planar)]

    if len(original_tables) != len(planar_tables):
        return False

    for assignment in range(1 << num_original):
        planar_index = sum(((assignment >> real) & 1) << position for position, real in enumerate(original_input))
        for original_bits, planar_bits in zip(original_tables, planar_tables, strict=True):
            if original_bits[-1 - assignment] != planar_bits[-1 - planar_index]:
                return False

    return True


def _check_structure(network: technology_network, degree: int = 2) -> None:
    """Check balance, rank-order planarity, fanout limits, and absence of dangling gates from public edges."""
    levels = dict.fromkeys(network.pis(), 0)
    levels.update((n, 0) for n in network.nodes() if network.is_constant(n))
    uses = Counter(network.pos())
    for gate in network.gates():
        fanins = network.fanins(gate)
        uses.update(fanins)
        depths = {levels[n] for n in fanins if not network.is_constant(n)}
        assert len(depths) == 1, (gate, depths)
        levels[gate] = depths.pop() + 1
    assert len({levels[n] for n in network.pos() if not network.is_constant(n)}) <= 1
    positions = {node: index for index, node in enumerate(network.nodes())}
    edges: dict[int, list[tuple[int, int]]] = {}
    for gate in network.gates():
        assert uses[gate] > 0, gate
        for fanin in network.fanins(gate):
            if not network.is_constant(fanin):
                edges.setdefault(levels[gate], []).append((positions[fanin], positions[gate]))
    for level_edges in edges.values():
        # Pairwise comparison stays small here; wide ranks should use an inversion-count algorithm.
        for (a, b), (c, d) in combinations(level_edges, 2):
            assert (a - c) * (b - d) >= 0, ((a, b), (c, d))
    for node in network.nodes():
        if not network.is_constant(node):
            assert uses[node] <= (degree if network.is_buf(node) else 1), (node, uses[node])


def _parity_verilog() -> str:
    """The 16-input parity tree of the C++ blueprints as Verilog.

    Pairs of signals are combined by an XNOR built from AND and NOT gates, the root by an XOR built from AND, NOT
    and OR gates; the hybrid strategy keeps crossings on this network where pure duplication copies whole levels.

    Returns:
        Verilog source of the network with inputs ``pi00`` to ``pi15`` and output ``po0``.
    """
    inputs = [f"pi{i:02d}" for i in range(16)]
    wires: list[str] = []
    assigns: list[str] = []

    def xnor(x: str, y: str) -> str:
        a, b, c = (f"n{len(wires) + k}" for k in range(3))
        wires.extend((a, b, c))
        assigns.extend((f"assign {a} = {x} & ~{y};", f"assign {b} = ~{x} & {y};", f"assign {c} = ~{a} & ~{b};"))
        return c

    def tree(pairs: list[tuple[str, str]]) -> str:
        # the blueprint pairs inputs from the highest index down and combines neighbouring pairs
        signals = list(starmap(xnor, pairs))
        while len(signals) > 1:
            signals = [xnor(signals[i], signals[i + 1]) for i in range(0, len(signals), 2)]
        return signals[0]

    pairs = [(inputs[i], inputs[i + 1]) for i in range(14, -1, -2)]
    left = tree(pairs[:4])
    right = tree(pairs[4:])
    wires.extend(("n_left", "n_right"))
    assigns.extend((f"assign n_left = {left} & ~{right};", f"assign n_right = ~{left} & {right};"))

    return (
        f"module top({', '.join(inputs)}, po0);\n"
        f"input {', '.join(inputs)};\n"
        "output po0;\n"
        f"wire {', '.join(wires)};\n" + "\n".join(assigns) + "\nassign po0 = n_left | n_right;\nendmodule\n"
    )


@pytest.fixture
def full_adder(resources_dir: Path) -> technology_network:
    """A full adder, whose planarization duplicates inputs.

    Returns:
        The ``FA.v`` network as a ``technology_network``.
    """
    return read_network(str(resources_dir / "FA.v"))


def test_unbalanced_networks_are_rejected(mux21: technology_network) -> None:
    """The pipeline requires a balanced network with unified outputs."""
    with pytest.raises(ValueError, match="balanced"):
        planarization(mux21)


@pytest.mark.parametrize(
    ("strategy", "criterion"),
    [
        (planarization_strategy.DUPLICATION, decision_criterion.LOOKAHEAD),
        (planarization_strategy.HYBRID, decision_criterion.WEIGHTED_CONE),
        (planarization_strategy.HYBRID, decision_criterion.LOOKAHEAD),
    ],
)
def test_planarization_is_equivalent(
    full_adder: technology_network, strategy: planarization_strategy, criterion: decision_criterion
) -> None:
    """Every strategy returns a balanced, fanout-substituted network that computes the same functions."""
    balanced = _balanced(full_adder)
    assert simulate_outputs(full_adder) == [
        ("s", [True, False, False, True, False, True, True, False]),
        ("cout", [True, True, True, False, True, False, False, False]),
    ]

    stats = planarization_stats()
    planar, original_input = planarization(balanced, strategy=strategy, criterion=criterion, statistics=stats)

    assert planar.num_pis() == len(original_input)
    assert planar.num_pis() > balanced.num_pis()  # the full adder needs virtual inputs
    assert sorted(set(original_input)) == list(range(balanced.num_pis()))
    assert planar.num_pos() == balanced.num_pos()
    assert is_balanced(planar, **UNIFIED)
    assert is_fanout_substituted(planar)
    assert stats.num_nodes == planar.size()
    assert stats.num_duplications > 0
    assert "num. nodes" in repr(stats)
    assert _equivalent_with_tied_inputs(balanced, planar, original_input)
    _check_structure(planar)


def test_networks_without_duplicates_keep_their_inputs(mux21: technology_network) -> None:
    """A network that needs no duplicate keeps its inputs, in the order of the planar embedding."""
    balanced = _balanced(mux21)

    planar, original_input = planarization(balanced)

    assert planar.num_pis() == balanced.num_pis()
    assert sorted(original_input) == list(range(balanced.num_pis()))
    assert _equivalent_with_tied_inputs(balanced, planar, original_input)
    _check_structure(planar)


@pytest.mark.parametrize("degree", [3, 1 << 31, (1 << 32) - 1])
def test_fanout_degree(full_adder: technology_network, degree: int) -> None:
    """Fanout nodes respect the requested degree."""
    balanced = _balanced(full_adder)

    planar, original_input = planarization(balanced, fanout_degree=degree)

    assert is_fanout_substituted(planar, degree=degree)
    assert _equivalent_with_tied_inputs(balanced, planar, original_input)
    _check_structure(planar, degree=degree)


def test_asymmetric_gates_keep_fanin_order(blif_network: Callable[[str], technology_network]) -> None:
    """LT, GT and ITE retain their input order when shared asymmetric gates require duplication."""
    network = blif_network(
        ".model asymmetric\n.inputs a b\n.outputs h f g\n.names a b lt\n10 1\n.names a b gt\n01 1\n"
        ".names lt gt f\n10 1\n.names gt lt g\n10 1\n.names f g a h\n11- 1\n0-1 1\n.end\n"
    )
    assert simulate_outputs(network) == [
        ("h", [True, False, False, False]),
        ("f", [False, False, True, False]),
        ("g", [False, True, False, False]),
    ]
    balanced = _balanced(network)
    stats = planarization_stats()
    planar, original_input = planarization(balanced, strategy=planarization_strategy.DUPLICATION, statistics=stats)
    assert stats.num_duplications > 0
    assert _equivalent_with_tied_inputs(balanced, planar, original_input)
    _check_structure(planar)


@pytest.mark.parametrize("seed", [None, 7])
def test_random_output_order_is_equivalent(full_adder: technology_network, seed: int | None) -> None:
    """Random output orders preserve logic and structural contracts."""
    balanced = _balanced(full_adder)
    expected = simulate_outputs(balanced)

    options: dict[str, Any] = {
        "po_order": output_order.RANDOM_PO_ORDER,
        "seed": seed,
    }
    planar, original_input = planarization(balanced, **options)
    assert _equivalent_with_tied_inputs(balanced, planar, original_input)
    _check_structure(planar)
    if seed is not None:
        again, original_input_again = planarization(balanced, **options)
        assert _equivalent_with_tied_inputs(balanced, again, original_input_again)
        _check_structure(again)
    assert simulate_outputs(balanced) == expected


def test_statistics_and_progress(full_adder: technology_network) -> None:
    """The hybrid strategy reports its crossing decisions, and the progress callback is called."""
    balanced = _balanced(full_adder)

    reports: list[tuple[str, int, int]] = []

    stats = planarization_stats()
    planar, original_input = planarization(
        balanced,
        strategy=planarization_strategy.HYBRID,
        on_progress=lambda task, done, total: reports.append((task, done, total)),
        statistics=stats,
    )

    assert reports
    assert reports[0][1] == 0
    assert all(done <= total for _, done, total in reports if total > 0)
    assert stats.time_total >= timedelta()
    assert (stats.num_crossings > 0) == (stats.num_crossing_levels > 0)
    assert "crossing levels" in repr(stats)
    assert _equivalent_with_tied_inputs(balanced, planar, original_input)


def test_default_strategy_is_hybrid(tmp_path: Path) -> None:
    """The keyword defaults are the pipeline's: the default call plans like ``HYBRID``, not like ``DUPLICATION``."""
    path = tmp_path / "parity.v"
    path.write_text(_parity_verilog(), encoding="utf-8")
    balanced = _balanced(read_network(str(path)))

    by_default, _ = planarization(balanced)
    hybrid, _ = planarization(balanced, strategy=planarization_strategy.HYBRID)
    duplicated, _ = planarization(balanced, strategy=planarization_strategy.DUPLICATION)

    assert by_default.size() == hybrid.size()
    assert by_default.size() < duplicated.size()


@pytest.mark.parametrize("xor_gates", [False, True])
def test_kept_crossings_use_both_gate_libraries(
    blif_network: Callable[[str], technology_network], *, xor_gates: bool
) -> None:
    network = blif_network(
        ".model crossing\n.inputs a b\n.outputs f g\n.names a x\n0 1\n.names b y\n0 1\n.names x y f\n11 1\n"
        ".names x y g\n01 1\n10 1\n11 1\n.end\n"
    )
    stats = planarization_stats()
    planar, original_input = planarization(
        network,
        xor_gates=xor_gates,
        node_weight=1e6,
        buffer_weight=1e6,
        criterion=decision_criterion.WEIGHTED_CONE,
        statistics=stats,
    )
    assert stats.num_crossings > 0
    assert stats.num_crossing_levels > 0
    assert simulate_outputs(network) == [("f", [False, False, False, True]), ("g", [False, True, True, True])]
    assert _equivalent_with_tied_inputs(network, planar, original_input)
    _check_structure(planar)


@pytest.mark.parametrize("options", [{"lookahead_budget": 1}, {"max_crossings_per_level": 0}])
def test_budget_fallback_and_crossing_limit(full_adder: technology_network, options: dict[str, Any]) -> None:
    balanced = _balanced(full_adder)
    stats = planarization_stats()
    planar, original_input = planarization(balanced, statistics=stats, **options)
    if "max_crossings_per_level" in options:
        assert stats.num_crossing_levels == stats.num_crossings == 0
    assert _equivalent_with_tied_inputs(balanced, planar, original_input)
    _check_structure(planar)


def test_substituted_inputs_and_exact_duplication_limit(full_adder: technology_network) -> None:
    balanced = _balanced(fanout_substitution(full_adder))
    stats = planarization_stats()
    planar, original_input = planarization(balanced, max_duplications=0, statistics=stats)
    assert stats.num_duplications > 1
    with pytest.raises(RuntimeError, match="duplications"):
        planarization(balanced, max_duplications=stats.num_duplications - 1)
    limited, limited_inputs = planarization(balanced, max_duplications=stats.num_duplications)
    assert _equivalent_with_tied_inputs(balanced, planar, original_input)
    assert _equivalent_with_tied_inputs(balanced, limited, limited_inputs)
    _check_structure(planar)


@pytest.mark.parametrize(
    "options",
    [
        {"fanout_degree": 0},
        {"fanout_degree": 1},
        {"lookahead_budget": 0},
        {"node_weight": -1.0},
        {"node_weight": float("inf")},
        {"node_weight": float("nan")},
        {"buffer_weight": -1.0},
        {"buffer_weight": float("inf")},
        {"buffer_weight": float("nan")},
        {"depth_growth": 0.0},
        {"depth_growth": -1.0},
        {"depth_growth": float("inf")},
        {"depth_growth": float("nan")},
    ],
)
def test_invalid_options_do_not_start_stages(full_adder: technology_network, options: dict[str, Any]) -> None:
    reports = []
    with pytest.raises(ValueError, match=r"degree|budget|weight|growth"):
        planarization(_balanced(full_adder), on_progress=lambda *args: reports.append(args), **options)
    assert reports == []


def test_nonunified_outputs_are_rejected(blif_network: Callable[[str], technology_network]) -> None:
    network = blif_network(".model depths\n.inputs a b\n.outputs f g\n.names a b f\n11 1\n.names f g\n1 1\n.end\n")
    assert is_balanced(network)
    assert not is_balanced(network, **UNIFIED)
    with pytest.raises(ValueError, match="unified"):
        planarization(network)


@pytest.mark.parametrize(
    ("definitions", "expected"),
    [
        (
            ".names a f\n1 1\n.names b g\n0 1\n.names a h\n1 1\n",
            [("f", [True, False, True, False]), ("g", [False, False, True, True]), ("h", [True, False, True, False])],
        ),
        (
            ".names a b m\n11 1\n.names m m f\n11 1\n.names m g\n0 1\n.names g h\n1 1\n",
            [("f", [True, False, False, False]), ("g", [False, True, True, True]), ("h", [False, True, True, True])],
        ),
        (
            ".names h\n1\n.names a h f\n11 1\n.names g\n",
            [("f", [True, False, True, False]), ("g", [False] * 4), ("h", [True] * 4)],
        ),
    ],
    ids=["direct-inverted-repeated-output", "repeated-fanin-shared-output", "constant-fanins-and-outputs"],
)
def test_interface_and_edge_cases(
    blif_network: Callable[[str], technology_network], definitions: str, expected: list[tuple[str, list[bool]]]
) -> None:
    network = blif_network(f".model edges\n.inputs a b\n.outputs f g h\n{definitions}.end\n")
    assert simulate_outputs(network) == expected
    balanced = _balanced(network)
    planar, original_input = planarization(balanced)
    assert _equivalent_with_tied_inputs(balanced, planar, original_input)
    _check_structure(planar)


def test_unused_inputs_and_dangling_gates(blif_network: Callable[[str], technology_network]) -> None:
    network = blif_network(
        ".model dangling\n.inputs unused a b\n.outputs f\n.names a b f\n11 1\n.names a b dangling\n01 1\n10 1\n.end\n"
    )
    planar, original_input = planarization(network)
    assert planar.num_pis() == 3
    assert _equivalent_with_tied_inputs(network, planar, original_input)
    _check_structure(planar)


def test_constant_only_gate_is_rejected(blif_network: Callable[[str], technology_network]) -> None:
    network = blif_network(".model constants\n.outputs f\n.names one\n1\n.names one one f\n11 1\n.end\n")
    with pytest.raises(ValueError, match="constant"):
        planarization(network)


def test_empty_pipeline() -> None:
    planar, original_input = planarization(technology_network())
    assert planar.num_pis() == planar.num_pos() == planar.num_gates() == 0
    assert original_input == []
