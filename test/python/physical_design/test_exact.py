# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

from __future__ import annotations

from typing import TYPE_CHECKING, Any

import pytest

from mnt.pyfiction import physical_design
from mnt.pyfiction.layouts import arrangement, hexagonal_gate_layout, shifted_cartesian_gate_layout
from mnt.pyfiction.networks.io import read_network
from mnt.pyfiction.verification import (
    critical_path_length_and_throughput,
    eq_type,
    equivalence_checking,
    gate_level_drvs,
)

if TYPE_CHECKING:
    from pathlib import Path

    from mnt.pyfiction.networks import technology_network


@pytest.mark.skipif(not hasattr(physical_design, "exact"), reason="pyfiction was built without Z3")
def test_exact_cartesian_default(mux21):
    layout = physical_design.exact(mux21)
    assert layout is not None
    assert equivalence_checking(mux21, layout) == eq_type.STRONG


@pytest.mark.skipif(not hasattr(physical_design, "exact"), reason="pyfiction was built without Z3")
def test_exact_cartesian_with_parameters(mux21):

    layout = physical_design.exact(mux21, border_io=True, crossings=True, scheme="ESR")
    assert layout is not None

    assert equivalence_checking(mux21, layout) == eq_type.STRONG


@pytest.mark.skipif(not hasattr(physical_design, "exact"), reason="pyfiction was built without Z3")
def test_exact_cartesian_with_stats(mux21):
    stats = physical_design.exact_stats()

    layout = physical_design.exact(mux21, statistics=stats)
    assert layout is not None

    assert equivalence_checking(mux21, layout) == eq_type.STRONG


@pytest.mark.skipif(not hasattr(physical_design, "exact"), reason="pyfiction was built without Z3")
def test_exact_hexagonal_default(mux21):

    layout = physical_design.exact(mux21, layout_type=hexagonal_gate_layout, layout_arrangement=arrangement.EVEN_ROW)
    assert layout is not None
    assert equivalence_checking(mux21, layout) == eq_type.STRONG


@pytest.mark.skipif(not hasattr(physical_design, "exact"), reason="pyfiction was built without Z3")
def test_exact_hexagonal_with_parameters(mux21):

    layout = physical_design.exact(
        mux21,
        layout_type=hexagonal_gate_layout,
        border_io=True,
        crossings=True,
        scheme="ESR",
        layout_arrangement=arrangement.EVEN_ROW,
    )
    assert layout is not None

    assert equivalence_checking(mux21, layout) == eq_type.STRONG


@pytest.mark.skipif(not hasattr(physical_design, "exact"), reason="pyfiction was built without Z3")
def test_exact_hexagonal_with_stats(mux21):
    stats = physical_design.exact_stats()

    layout = physical_design.exact(
        mux21, layout_type=hexagonal_gate_layout, statistics=stats, layout_arrangement=arrangement.EVEN_ROW
    )
    assert layout is not None

    assert equivalence_checking(mux21, layout) == eq_type.STRONG


@pytest.mark.parametrize("cls", [shifted_cartesian_gate_layout, hexagonal_gate_layout])
@pytest.mark.skipif(not hasattr(physical_design, "exact"), reason="pyfiction was built without Z3")
def test_exact_requires_an_arrangement_for_shifted_and_hexagonal_layouts(mux21, cls):
    with pytest.raises(ValueError, match="arrangement"):
        physical_design.exact(mux21, layout_type=cls)


@pytest.mark.parametrize("cls", [shifted_cartesian_gate_layout, hexagonal_gate_layout])
@pytest.mark.skipif(not hasattr(physical_design, "exact"), reason="pyfiction was built without Z3")
def test_exact_selects_layout_class(
    mux21: technology_network, cls: type[shifted_cartesian_gate_layout | hexagonal_gate_layout]
) -> None:
    """Preserve the selected representation and explicit arrangement."""
    layout = physical_design.exact(mux21, layout_type=cls, layout_arrangement=arrangement.EVEN_ROW, timeout=10000)
    assert layout is not None
    assert type(layout) is cls
    assert isinstance(layout, (shifted_cartesian_gate_layout, hexagonal_gate_layout))
    assert layout.get_arrangement() == arrangement.EVEN_ROW
    assert equivalence_checking(mux21, layout) == eq_type.STRONG


@pytest.mark.parametrize("selector", [None, "cartesian", int])
@pytest.mark.skipif(not hasattr(physical_design, "exact"), reason="pyfiction was built without Z3")
def test_exact_rejects_invalid_selectors(mux21: technology_network, selector: Any) -> None:
    """Reject unrelated output selectors before solver work."""
    with pytest.raises(TypeError):
        physical_design.exact(mux21, layout_type=selector)


@pytest.mark.skipif(not hasattr(physical_design, "exact"), reason="pyfiction was built without Z3")
def test_exact_unsatisfiable_result(mux21: technology_network) -> None:
    """A fixed one-tile layout cannot contain a multiplexer."""
    assert physical_design.exact(mux21, fixed_size=True, upper_bound_x=1, upper_bound_y=1, timeout=1000) is None


@pytest.mark.parametrize(
    "name", ["exact_cartesian", "exact_shifted_cartesian", "exact_hexagonal", "orthogonal_hexagonal"]
)
def test_physical_design_specialization_names_are_removed(name: str) -> None:
    """Expose one name per physical design algorithm."""
    assert not hasattr(physical_design, name)


@pytest.mark.skipif(not hasattr(physical_design, "exact"), reason="pyfiction was built without Z3")
@pytest.mark.parametrize("threads", [1, 2])
def test_exact_statistics_names_and_progress(mux21: technology_network, threads: int) -> None:
    """Return consistent statistics and interface names through serial and parallel solving."""
    stats = physical_design.exact_stats()
    reports = []
    layout = physical_design.exact(
        mux21, statistics=stats, num_threads=threads, timeout=10000, on_progress=lambda *report: reports.append(report)
    )
    assert layout is not None
    assert equivalence_checking(mux21, layout) == eq_type.STRONG
    assert (stats.x_size, stats.y_size) == (layout.width(), layout.height())
    assert (stats.num_gates, stats.num_wires, stats.num_crossings) == (
        layout.num_gates(),
        layout.num_wires(),
        layout.num_crossings(),
    )
    assert stats.time_total.total_seconds() >= 0
    assert layout.get_layout_name() == "mux21"
    assert [layout.get_input_name(i) for i in range(layout.num_pis())] == ["in0", "in1", "in2"]
    assert layout.get_output_name(0) == "out"
    counts = [(done, total) for task, done, total in reports if task == "aspect ratios"]
    assert counts
    assert counts[-1][0] == stats.num_aspect_ratios
    assert all(total == 0 for _, total in counts)


@pytest.mark.skipif(not hasattr(physical_design, "exact"), reason="pyfiction was built without Z3")
@pytest.mark.parametrize("bound", ["upper_bound_area", "upper_bound_x", "upper_bound_y"])
def test_exact_unsatisfiable_upper_bounds(mux21: technology_network, bound: str) -> None:
    """Each independent bound can exclude every feasible placement."""
    bounds: dict[str, Any] = {"upper_bound_x": 2, "upper_bound_y": 2, bound: 1}
    assert physical_design.exact(mux21, **bounds, timeout=1000) is None


@pytest.mark.skipif(not hasattr(physical_design, "exact"), reason="pyfiction was built without Z3")
@pytest.mark.parametrize(
    "options",
    [
        {"scheme": "OPEN", "crossings": True},
        {"scheme": "USE", "synchronization_elements": True, "border_io": True},
        {"scheme": "RES", "minimize_wires": True},
        {"scheme": "2DDWave", "minimize_crossings": True, "crossings": True},
        {"scheme": "USE", "straight_inverters": True},
    ],
    ids=["open", "synchronization", "minimize-wires", "minimize-crossings", "straight-inverters"],
)
def test_exact_supported_clocking_and_constraints(tmp_path: Path, options: dict[str, Any]) -> None:
    """Exercise distinct solver constraints on one bounded AND-inverter circuit."""
    source = tmp_path / "solver.blif"
    source.write_text(".model solver\n.inputs a b\n.outputs f\n.names a b gate\n11 1\n.names gate f\n0 1\n.end\n")
    network = read_network(str(source))
    layout = physical_design.exact(network, timeout=10000, **options)
    assert layout is not None
    assert equivalence_checking(network, layout) != eq_type.NO
    assert layout.is_clocking_scheme(options["scheme"].upper())
    assert gate_level_drvs(layout)[1] == 0
    assert critical_path_length_and_throughput(layout)[1] >= 1
    if options.get("straight_inverters"):
        inverters = [gate for gate in layout.gates() if layout.is_inv(gate)]
        assert inverters
        for inverter in inverters:
            site = layout.get_tile(inverter)
            incoming = layout.incoming_data_flow(site)
            outgoing = layout.outgoing_data_flow(site)
            assert len(incoming) == len(outgoing) == 1
            assert incoming[0].x + outgoing[0].x == 2 * site.x
            assert incoming[0].y + outgoing[0].y == 2 * site.y
    if options.get("border_io"):
        assert all(
            layout.is_at_northern_border(site)
            or layout.is_at_western_border(site)
            or layout.is_at_southern_border(site)
            or layout.is_at_eastern_border(site)
            for site in [layout.get_tile(node) for node in [*layout.pis(), *layout.pos()]]
        )
