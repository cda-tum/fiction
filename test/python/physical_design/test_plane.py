# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

from __future__ import annotations

from datetime import timedelta
from textwrap import dedent
from typing import TYPE_CHECKING

import pytest

from mnt.pyfiction.networks import high_degree_fanin_exception, simulate_outputs
from mnt.pyfiction.networks.io import read_network
from mnt.pyfiction.physical_design import apply_qca_one_library, num_clks, plane, plane_stats
from mnt.pyfiction.synthesis import decision_criterion, network_balancing, planarization, planarization_stats
from mnt.pyfiction.verification import eq_type, equivalence_checking

if TYPE_CHECKING:
    from collections.abc import Callable
    from pathlib import Path

    from mnt.pyfiction.layouts import cartesian_gate_layout
    from mnt.pyfiction.networks import technology_network


@pytest.fixture
def blif_network(tmp_path: Path) -> Callable[[str], technology_network]:
    """Read small networks with explicit fanins and output order.

    Returns:
        A reader for BLIF bodies.
    """

    def read(body: str) -> technology_network:
        path = tmp_path / "plane.blif"
        path.write_text(f".model plane\n{dedent(body).strip()}\n.end\n", encoding="utf-8")
        return read_network(str(path))

    return read


def _planarize(network: technology_network) -> tuple[technology_network, list[int]]:
    """Balance with unified outputs and planarize the network.

    Returns:
        The planar network and the original input index for each planar input.
    """
    return planarization(network_balancing(network, unify_outputs=True, buffer_constant_outputs=False))


def _check_layout(network: technology_network, layout: cartesian_gate_layout) -> None:
    """Check the layout interface and absence of crossings."""
    assert (layout.num_pis(), layout.num_pos()) == (network.num_pis(), network.num_pos())
    assert layout.num_crossings() == 0


@pytest.mark.parametrize(
    "spec",
    [
        pytest.param(".inputs a b\n.outputs f\n.names b n\n0 1\n.names a n f\n11 1", id="unbalanced-and-inverter"),
        pytest.param(
            ".inputs a b\n.outputs carry sum\n.names a b carry\n11 1\n.names a b sum\n01 1\n10 1", id="half-adder"
        ),
        pytest.param(
            """
            .inputs a b c d e
            .outputs f g
            .names a b n1
            11 1
            .names n1 c f
            1- 1
            -1 1
            .names f d n2
            11 1
            .names n2 e g
            1- 1
            -1 1
            """,
            id="alternating-and-or",
        ),
        pytest.param(
            """
            .inputs a
            .outputs f g h i
            .names a root
            1 1
            .names root left
            1 1
            .names root right
            1 1
            .names left f
            0 1
            .names left g
            0 1
            .names right h
            0 1
            .names right i
            0 1
            """,
            id="fanout-corner-case",
        ),
        pytest.param(
            """
            .inputs a b c
            .outputs f
            .names a b both
            11 1
            .names a b either
            1- 1
            -1 1
            .names c either two
            11 1
            .names both two f
            1- 1
            -1 1
            """,
            id="decomposed-majority",
        ),
        pytest.param(
            """
            .inputs a b c d e
            .outputs f g h
            .names a b ab
            11 1
            .names ab c f
            11 1
            .names b c bc
            1- 1
            -1 1
            .names bc d g
            1- 1
            -1 1
            .names c d cd
            01 1
            10 1
            .names cd e h
            01 1
            10 1
            """,
            id="overlapping-and-or-xor-chains",
        ),
    ],
)
def test_plane_places_planarized_networks(blif_network: Callable[[str], technology_network], spec: str) -> None:
    """PLANE preserves the interface and logic of each planar embedding."""
    planar, _ = _planarize(blif_network(spec))
    layout = plane(planar)
    _check_layout(planar, layout)
    assert equivalence_checking(planar, layout) != eq_type.NO


@pytest.mark.parametrize("xor_gates", [False, True], ids=["and-or-not", "xor"])
def test_plane_places_both_crossing_gate_forms(parity_network: technology_network, *, xor_gates: bool) -> None:
    """Place and route the parity blueprint with either crossing gate implementation."""
    balanced = network_balancing(parity_network, unify_outputs=True, buffer_constant_outputs=False)
    stats = planarization_stats()
    # Make duplication expensive enough for the hybrid strategy to keep crossings.
    planar, _ = planarization(
        balanced,
        xor_gates=xor_gates,
        node_weight=1e6,
        buffer_weight=1e6,
        criterion=decision_criterion.WEIGHTED_CONE,
        statistics=stats,
    )
    assert stats.num_crossings > 0
    assert any(planar.is_xor(gate) for gate in planar.gates()) == xor_gates
    layout = plane(planar)
    _check_layout(planar, layout)
    assert equivalence_checking(planar, layout) != eq_type.NO


@pytest.mark.parametrize(("phases", "clocks"), [(None, 4), (num_clks.THREE, 3)], ids=["default-four", "three"])
def test_plane_layout_and_stats(mux21: technology_network, phases: num_clks | None, clocks: int) -> None:
    """The selected clocking preserves the logic and the statistics describe the result."""
    planar, _ = _planarize(mux21)
    stats = plane_stats()
    layout = (
        plane(planar, statistics=stats)
        if phases is None
        else plane(planar, statistics=stats, number_of_clock_phases=phases)
    )

    _check_layout(planar, layout)
    assert layout.num_clocks() == clocks
    assert equivalence_checking(planar, layout) == eq_type.STRONG
    assert (stats.x_size, stats.y_size) == (layout.width(), layout.height())
    assert (stats.num_gates, stats.num_wires, stats.num_crossings) == (layout.num_gates(), layout.num_wires(), 0)
    assert stats.time_total >= timedelta()


def test_plane_input_mapping(full_adder: technology_network) -> None:
    """The input mapping ties duplicated inputs to their source."""
    planar, mapping = _planarize(full_adder)
    layout = plane(planar)

    assert layout.num_pis() > full_adder.num_pis()
    assert equivalence_checking(full_adder, layout) == eq_type.NO
    assert equivalence_checking(full_adder, layout, input_mapping=mapping) != eq_type.NO


def test_plane_layout_maps_to_qca(mux21: technology_network) -> None:
    """A PLANE layout can be mapped with the QCA gate library."""
    planar, _ = _planarize(mux21)
    assert apply_qca_one_library(plane(planar)).num_cells() > 0


def test_plane_places_direct_input_outputs(blif_network: Callable[[str], technology_network]) -> None:
    """Direct input outputs survive balancing and placement in declaration order."""
    planar, _ = _planarize(blif_network(".inputs a b c\n.outputs a f c\n.names b c f\n11 1"))
    layout = plane(planar)
    _check_layout(planar, layout)
    outputs = simulate_outputs(layout)
    assert [name for name, _ in outputs] == ["a", "f", "c"]
    assert outputs == simulate_outputs(planar)


@pytest.mark.parametrize(
    "spec",
    [
        pytest.param(
            ".inputs a b c d\n.outputs f f g h h\n.names a b f\n11 1\n.names b c g\n1- 1\n-1 1\n.names c d h\n11 1",
            id="shared-output-drivers",
        ),
        pytest.param(
            ".inputs a b c d\n.outputs f one g zero\n.names a b f\n11 1\n"
            ".names one\n1\n.names c d g\n1- 1\n-1 1\n.names zero",
            id="interleaved-constants",
        ),
        pytest.param(".inputs unused\n.outputs one zero\n.names one\n1\n.names zero", id="constant-only-outputs"),
        pytest.param(".inputs a b\n.outputs lt\n.names a b lt\n01 1", id="asymmetric-fanins"),
        pytest.param(".inputs a b\n.outputs lt\n.names b a lt\n01 1", id="swapped-asymmetric-fanins"),
    ],
)
def test_plane_preserves_ordered_outputs(blif_network: Callable[[str], technology_network], spec: str) -> None:
    """Compare every output in declaration order, including duplicate labels and constants."""
    network = blif_network(spec)
    layout = plane(network)

    _check_layout(network, layout)
    assert simulate_outputs(layout) == simulate_outputs(network)


def test_plane_keeps_unused_inputs_inside_the_extent(blif_network: Callable[[str], technology_network]) -> None:
    """Unused primary inputs stay disconnected at the border, within the layout extent."""
    network = blif_network(".inputs a unused1 b unused2\n.outputs f\n.names a b f\n11 1")
    layout = plane(network)

    _check_layout(network, layout)
    unused = [layout.get_tile(pi) for pi in layout.pis() if layout.fanout_size(pi) == 0]
    assert len(unused) == 2
    assert all(tile.x == 0 or tile.y == 0 for tile in unused)
    for obj in (*layout.pis(), *layout.pos(), *layout.gates(), *layout.wires()):
        assert layout.is_within_bounds(layout.get_tile(obj))
    assert simulate_outputs(layout) == simulate_outputs(network)


def test_plane_reports_progress(full_adder: technology_network) -> None:
    """Placement reports a monotone count for the levels and output routing."""
    planar, _ = _planarize(full_adder)
    reports: list[tuple[str, int, int]] = []
    layout = plane(planar, on_progress=lambda *report: reports.append(report))

    assert equivalence_checking(planar, layout) != eq_type.NO
    placements = [(done, total) for task, done, total in reports if task == "placing levels"]
    total = planar.depth() + 2
    assert placements
    assert placements[0] == (0, total)
    assert placements[-1] == (total, total)
    assert all(expected == total and 0 <= done <= total for done, expected in placements)
    assert placements == sorted(placements)


@pytest.mark.parametrize(
    ("spec", "error", "message"),
    [
        pytest.param(
            ".inputs a b c d\n.outputs f g\n.names a c f\n11 1\n.names b d g\n11 1",
            ValueError,
            "crossings",
            id="crossing-rank-order",
        ),
        pytest.param(
            ".inputs a b c\n.outputs f\n.names a b c f\n11- 1\n1-1 1\n-11 1",
            high_degree_fanin_exception,
            None,
            id="three-fanins",
        ),
        pytest.param(
            ".inputs a b\n.outputs f f f\n.names a b f\n11 1",
            ValueError,
            "more than two fanouts",
            id="three-fanouts",
        ),
        pytest.param(".outputs one\n.names one\n1", ValueError, "no primary inputs", id="no-inputs"),
        pytest.param(
            ".inputs a b c\n.outputs f\n.names a b n\n11 1\n.names n c f\n11 1",
            ValueError,
            "not balanced",
            id="unbalanced",
        ),
        pytest.param(
            ".inputs a\n.outputs f\n.names one\n1\n.names a one f\n11 1",
            ValueError,
            "constant fanins",
            id="one-constant-fanin",
        ),
        pytest.param(
            ".inputs a\n.outputs f\n.names zero\n.names one\n1\n.names a zero one f\n11- 1\n1-1 1\n-11 1",
            ValueError,
            "constant fanins",
            id="two-constant-fanins",
        ),
        pytest.param(
            ".inputs a\n.outputs a f\n.names one\n1\n.names one f\n1 1",
            ValueError,
            "constant fanins",
            id="constant-only-fanins",
        ),
        pytest.param(
            ".inputs a\n.outputs f\n.names a buf\n1 1\n.names buf buf f\n11 1",
            ValueError,
            "repeated non-constant fanins",
            id="repeated-fanins",
        ),
        pytest.param(
            ".inputs a b c\n.outputs a f\n.names b c f\n11 1",
            ValueError,
            "last level",
            id="early-output",
        ),
    ],
)
def test_plane_rejects_invalid_networks(
    blif_network: Callable[[str], technology_network], spec: str, error: type[Exception], message: str | None
) -> None:
    """Reject each unsupported network shape at the placement boundary."""
    network = blif_network(spec)
    with pytest.raises(error, match=message):
        plane(network)
