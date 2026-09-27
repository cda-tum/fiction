# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""Progress callbacks preserve results and report real completed work."""

from __future__ import annotations

import re
import sys
import threading
from typing import TYPE_CHECKING, Any

import pytest

from mnt.pyfiction import layouts, physical_design
from mnt.pyfiction.inml.io import WriteQccLayoutParams, write_qcc_layout
from mnt.pyfiction.layouts import CartesianGateLayout, ShiftedCartesianGateLayout
from mnt.pyfiction.layouts.io import write_dot_layout, write_fgl_layout
from mnt.pyfiction.mol_qca.io import write_qll_layout
from mnt.pyfiction.physical_design import (
    ExactParams,
    GraphOrientedLayoutDesignParams,
    TechnologyConstraints,
    apply_qca_one_library,
    apply_sim7_mol_library,
    apply_topolinano_library,
    exact,
    graph_oriented_layout_design,
    orthogonal,
)
from mnt.pyfiction.qca import io as qca_io
from mnt.pyfiction.sidb import DotTag, LatticeSite, SiDBLayout
from mnt.pyfiction.sidb.analysis import CriticalTemperatureParams, critical_temperature_gate_based
from mnt.pyfiction.sidb.io import SvgParams, read_sqd_layout, write_sidb_layout_svg, write_sqd_layout
from mnt.pyfiction.sidb.simulation import ClusterCompleteParams, SimulationEngine, clustercomplete
from mnt.pyfiction.synthesis import (
    FanoutSubstitutionParams,
    NetworkBalancingParams,
    fanout_substitution,
    network_balancing,
    standard_functions,
)
from mnt.pyfiction.verification import (
    DesignRuleParams,
    EquivalenceType,
    critical_path_length_and_throughput,
    equivalence_checking,
    gate_level_drvs,
)

if TYPE_CHECKING:
    from pathlib import Path

    from mnt.pyfiction.networks import TechnologyNetwork


def test_long_route_statistics_release_the_gil() -> None:
    """Deep layout summaries finish while Python display threads can run."""
    length = 200_000
    layout = CartesianGateLayout((length, 0), "2DDWave")
    signal = layout.create_pi("in", (0, 0))
    for x in range(1, length):
        signal = layout.create_buf(signal, (x, 0))
    layout.create_po(signal, "out", (length, 0))
    start = threading.Event()
    refreshed = threading.Event()

    def refresh() -> None:
        """Run one Python update after the native analysis starts."""
        start.wait()
        refreshed.set()

    worker = threading.Thread(target=refresh)
    interval = sys.getswitchinterval()
    worker.start()
    try:
        # The native call must release the GIL before Python's next scheduled handoff.
        sys.setswitchinterval(10)
        start.set()
        result = critical_path_length_and_throughput(layout)
        refreshed_during_analysis = refreshed.is_set()
    finally:
        sys.setswitchinterval(interval)
        start.set()
        worker.join()
    assert result == (length + 1, 1)
    assert refreshed_during_analysis


@pytest.mark.parametrize("threads", [1, 4])
def test_exact_candidate_lifecycle(mux21: TechnologyNetwork, threads: int) -> None:
    """Solver candidates use tile dimensions and leave no active worker after success."""
    params = ExactParams()
    params.num_threads = threads
    reports: list[tuple[int, int, str, int, int, bool]] = []
    params.on_worker_progress = lambda *report: reports.append(report)
    layout = exact(mux21, params=params, layout_type=CartesianGateLayout).layout
    assert layout is not None
    assert equivalence_checking(mux21, layout).eq == EquivalenceType.STRONG
    assert reports
    active = {}
    for worker, count, description, done, total, running in reports:
        assert 0 <= worker < count == threads
        assert done == total == 0
        dimensions = description.rsplit(": ", 1)[1].split(" \N{MULTIPLICATION SIGN} ")
        assert len(dimensions) == 2
        assert all(int(dimension) > 0 for dimension in dimensions)
        active[worker] = running
    assert not any(active.values())


@pytest.mark.parametrize("parallel", [False, True])
def test_gold_graph_expansions(mux21: TechnologyNetwork, *, parallel: bool) -> None:
    """Stable graphs report expansions, accepted solutions, and their final inactive state."""
    params = GraphOrientedLayoutDesignParams()
    params.enable_multithreading = parallel
    params.return_first = True
    params.seed = 7
    reports: list[tuple[int, int, str, int, int, bool]] = []
    counts: list[tuple[str, int, int]] = []
    params.on_worker_progress = lambda *report: reports.append(report)
    params.on_progress = lambda *report: counts.append(report)
    layout = graph_oriented_layout_design(mux21, params=params).layout
    assert layout is not None
    assert equivalence_checking(mux21, layout).eq != EquivalenceType.NO
    assert any("; best " in description for _, _, description, _, _, _ in reports)
    previous: dict[int, int] = {}
    active: dict[int, bool] = {}
    for worker, count, description, done, total, running in reports:
        assert 0 <= worker < count
        assert description.startswith(f"graph {worker + 1}:")
        assert "placed " in description
        assert total == 0
        assert done >= previous.get(worker, 0)
        previous[worker] = done
        active[worker] = running
    assert not any(active.values())
    assert sum(previous.values()) == next(done for task, done, _ in reversed(counts) if task == "expansions")
    assert max(previous.values()) > 1


@pytest.mark.parametrize("command", ["balance", "fanouts", "check"])
def test_counted_native_phases(mux21: TechnologyNetwork, command: str) -> None:
    """Source-gate passes and enabled checks finish at their declared totals."""
    reports: list[tuple[str, int, int]] = []

    def callback(*report):
        return reports.append(report)

    if command == "balance":
        balancing_params = NetworkBalancingParams()
        balancing_params.on_progress = callback
        result = network_balancing(mux21, params=balancing_params)
        assert equivalence_checking(mux21, result).eq != EquivalenceType.NO
    elif command == "fanouts":
        substitution_params = FanoutSubstitutionParams()
        substitution_params.on_progress = callback
        result = fanout_substitution(mux21, params=substitution_params)
        assert equivalence_checking(mux21, result).eq != EquivalenceType.NO
    else:
        drv_params = DesignRuleParams()
        drv_params.on_progress = callback
        gate_level_drvs(orthogonal(mux21).layout, params=drv_params)
    final = {task: (done, total) for task, done, total in reports}
    assert final
    assert all(done == total for done, total in final.values())


@pytest.mark.parametrize("kind", ["fgl", "dot", "qll", "qca", "svg", "sqd", "sidb_svg"])
def test_writer_counts_and_output(mux21: TechnologyNetwork, tmp_path: Path, kind: str) -> None:
    """Callbacks leave writer output unchanged and count every completed phase."""
    gate_layout = orthogonal(mux21).layout
    reports: list[tuple[str, int, int]] = []
    paths = [tmp_path / f"{index}.{kind}" for index in range(2)]
    for index, path in enumerate(paths):
        callback = (lambda *report: reports.append(report)) if index else None
        kwargs: dict[str, Any] = {"on_progress": callback} if callback else {}
        if kind == "fgl":
            write_fgl_layout(gate_layout, str(path), **kwargs)
        elif kind == "dot":
            write_dot_layout(gate_layout, str(path), **kwargs)
        elif kind == "qll":
            write_qll_layout(apply_sim7_mol_library(gate_layout), str(path), **kwargs)
        elif kind in {"sqd", "sidb_svg"}:
            layout = SiDBLayout()
            layout.assign_sidb(LatticeSite(0, 0, 0), DotTag.NORMAL)
            layout.assign_sidb(LatticeSite(2, 1, 1), DotTag.NORMAL)
            if kind == "sqd":
                write_sqd_layout(layout, str(path), **kwargs)
            else:
                params = SvgParams()
                if callback:
                    params.on_progress = callback
                write_sidb_layout_svg(layout, str(path), params=params)
        else:
            cell_layout = apply_qca_one_library(gate_layout)
            writer_params = qca_io.SvgParams() if kind == "svg" else qca_io.QcaWriterParams()
            if callback:
                writer_params.on_progress = callback
            getattr(qca_io, f"write_{'qca_layout_svg' if kind == 'svg' else kind + '_layout'}")(
                cell_layout, path, params=writer_params
            )
    if kind == "sqd":
        assert read_sqd_layout(str(paths[0])) == read_sqd_layout(str(paths[1]))
    else:
        # FGL metadata records the wall-clock time of each write.
        contents = [re.sub(rb"<date>[^<]*</date>", b"<date/>", path.read_bytes(), count=1) for path in paths]
        assert contents[0] == contents[1]
    final = {task: (done, total) for task, done, total in reports}
    assert final
    assert all(done == total for done, total in final.values())
    assert any(total > 0 for done, total in final.values())


@pytest.mark.parametrize("threads", [1, 4])
def test_clustercomplete_worker_counts(resources_dir: Path, threads: int) -> None:
    """Serial and stolen compositions contribute to the same aggregate count."""
    layout = read_sqd_layout(str(resources_dir / "21_hex_inputsdbp_and_v19.sqd"))
    params = ClusterCompleteParams()
    params.available_threads = threads
    params.simulation_parameters.base = 2
    reports: list[tuple[int, int, str, int, int, bool]] = []
    counts: list[tuple[str, int, int]] = []
    params.on_worker_progress = lambda *report: reports.append(report)
    params.on_progress = lambda *report: counts.append(report)
    result = clustercomplete(layout, params=params)
    assert result.charge_distributions
    assert reports
    final = {worker: done for worker, count, description, done, total, active in reports if not active}
    assert all(total == 0 and count == threads for worker, count, description, done, total, active in reports)
    assert sum(final.values()) == next(done for task, done, _ in reversed(counts) if task == "compositions")


def test_exact_skips_candidates_outside_area_bound(mux21: TechnologyNetwork) -> None:
    """Candidates rejected by the area bound never appear as active solvers."""
    params = ExactParams()
    params.upper_bound_area = 1
    reports: list[tuple[int, int, str, int, int, bool]] = []
    params.on_worker_progress = lambda *report: reports.append(report)
    result = exact(mux21, params=params, layout_type=CartesianGateLayout)
    assert result.layout is None
    assert result.stats.time_total.total_seconds() >= 0
    assert not reports


def test_exact_clears_candidates_on_timeout(mux21: TechnologyNetwork) -> None:
    """A timeout leaves no active candidate, whether it interrupts a solver or prevents its start."""
    params = ExactParams()
    params.num_threads = 4
    params.timeout = 1
    reports: list[tuple[int, int, str, int, int, bool]] = []
    params.on_worker_progress = lambda *report: reports.append(report)
    exact(mux21, params=params, layout_type=CartesianGateLayout)
    active = {worker: running for worker, count, description, done, total, running in reports}
    assert not any(active.values())


def test_temperature_forwards_simulation_workers(resources_dir: Path) -> None:
    """Temperature analysis keeps outer phases and forwards the active simulation's workers."""
    layout = read_sqd_layout(str(resources_dir / "hex_11_inputsdbp_inv_straight_v0_manual.sqd"))
    params = CriticalTemperatureParams()
    params.operational_params.sim_engine = SimulationEngine.CLUSTERCOMPLETE
    params.operational_params.simulation_parameters.base = 2
    workers: list[tuple[int, int, str, int, int, bool]] = []
    phases: list[tuple[str, int, int]] = []
    params.on_worker_progress = lambda *report: workers.append(report)
    params.on_progress = lambda *report: phases.append(report)
    critical_temperature_gate_based(layout, [standard_functions("not")[0]], params=params)
    assert workers
    assert phases
    active = {worker: running for worker, count, description, done, total, running in workers}
    assert not any(active.values())


@pytest.mark.parametrize(
    ("library", "topology"),
    [
        ("qca_one", "cartesian"),
        ("sim7_mol", "cartesian"),
        ("bestagon", "hexagonal"),
        ("topolinano", "shifted_cartesian"),
    ],
)
def test_empty_mapping_has_no_completed_gates(library: str, topology: str) -> None:
    """Empty source layouts report no gate mappings for every cell library."""
    layout = getattr(layouts, "".join(part.title() for part in topology.split("_")) + "GateLayout")()
    reports: list[tuple[str, int, int]] = []
    result = getattr(physical_design, f"apply_{library}_library")(
        layout, lambda task, done, total: reports.append((task, done, total))
    )
    assert result.is_empty()
    assert reports
    assert all(done == total == 0 for _, done, total in reports)


def test_failed_mapping_retains_completed_count() -> None:
    """Unsupported routing leaves the last completed count below the mapping total."""
    layout = ShiftedCartesianGateLayout((1, 1), "2DDWave", "unsupported routing")
    source = layout.create_pi("a", (0, 0))
    layout.create_po(source, "f", (0, 1))
    reports: list[tuple[str, int, int]] = []
    with pytest.raises(ValueError, match="unsupported gate orientation"):
        apply_topolinano_library(layout, lambda task, done, total: reports.append((task, done, total)))
    assert reports
    assert reports[-1][1] < reports[-1][2]


def test_qcc_writer_counts_and_output(mux21: TechnologyNetwork, tmp_path: Path) -> None:
    """QCC callbacks count rows without changing the serialized layout."""
    placement = ExactParams()
    placement.scheme = "COLUMNAR3"
    placement.crossings = True
    placement.border_io = True
    placement.technology_specifics = TechnologyConstraints.TOPOLINANO
    gate_layout = exact(mux21, params=placement, layout_type=ShiftedCartesianGateLayout).layout
    assert gate_layout is not None
    layout = apply_topolinano_library(gate_layout)
    before = tmp_path / "before.qcc"
    after = tmp_path / "after.qcc"
    write_qcc_layout(layout, str(before))
    params = WriteQccLayoutParams()
    reports: list[tuple[str, int, int]] = []
    params.on_progress = lambda *report: reports.append(report)
    write_qcc_layout(layout, str(after), params=params)
    assert before.read_bytes() == after.read_bytes()
    assert reports[-1][1] == reports[-1][2] > 0
