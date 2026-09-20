# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""Progress callbacks preserve results and report real completed work."""

from __future__ import annotations

import sys
import threading
from typing import TYPE_CHECKING

import pytest

from mnt import pyfiction as fiction

if TYPE_CHECKING:
    from pathlib import Path


def test_long_route_statistics_release_the_gil() -> None:
    """Deep layout summaries finish while Python display threads can run."""
    length = 200_000
    layout = fiction.cartesian_gate_layout((length, 0), "2DDWave")
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
        result = fiction.critical_path_length_and_throughput(layout)
        refreshed_during_analysis = refreshed.is_set()
    finally:
        sys.setswitchinterval(interval)
        start.set()
        worker.join()
    assert result == (length + 1, 1)
    assert refreshed_during_analysis


@pytest.mark.parametrize("threads", [1, 4])
def test_exact_candidate_lifecycle(mux21: fiction.technology_network, threads: int) -> None:
    """Solver candidates use tile dimensions and leave no active worker after success."""
    params = fiction.exact_params()
    params.num_threads = threads
    reports: list[tuple[int, int, str, int, int, bool]] = []
    params.on_worker_progress = lambda *report: reports.append(report)
    layout = fiction.exact_cartesian(mux21, params)
    assert fiction.equivalence_checking(mux21, layout) == fiction.eq_type.STRONG
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
def test_gold_graph_expansions(mux21: fiction.technology_network, *, parallel: bool) -> None:
    """Stable graphs report expansions, accepted solutions, and their final inactive state."""
    params = fiction.graph_oriented_layout_design_params()
    params.enable_multithreading = parallel
    params.return_first = True
    params.seed = 7
    reports: list[tuple[int, int, str, int, int, bool]] = []
    counts: list[tuple[str, int, int]] = []
    params.on_worker_progress = lambda *report: reports.append(report)
    params.on_progress = lambda *report: counts.append(report)
    layout = fiction.graph_oriented_layout_design(mux21, params)
    assert fiction.equivalence_checking(mux21, layout) != fiction.eq_type.NO
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
def test_counted_native_phases(mux21: fiction.technology_network, command: str) -> None:
    """Source-gate passes and enabled checks finish at their declared totals."""
    reports: list[tuple[str, int, int]] = []

    def callback(*report):
        return reports.append(report)

    if command == "balance":
        params = fiction.network_balancing_params()
        params.on_progress = callback
        result = fiction.network_balancing(mux21, params)
        assert fiction.equivalence_checking(mux21, result) != fiction.eq_type.NO
    elif command == "fanouts":
        params = fiction.fanout_substitution_params()
        params.on_progress = callback
        result = fiction.fanout_substitution(mux21, params)
        assert fiction.equivalence_checking(mux21, result) != fiction.eq_type.NO
    else:
        params = fiction.gate_level_drv_params()
        params.on_progress = callback
        fiction.gate_level_drvs(fiction.orthogonal(mux21), params)
    final = {task: (done, total) for task, done, total in reports}
    assert final
    assert all(done == total for done, total in final.values())


@pytest.mark.parametrize("kind", ["fgl", "dot", "qll", "fqca", "qca", "svg", "sqd", "sidb_svg"])
def test_writer_counts_and_output(mux21: fiction.technology_network, tmp_path: Path, kind: str) -> None:
    """Callbacks leave writer output unchanged and count every completed phase."""
    gate_layout = fiction.orthogonal(mux21)
    reports: list[tuple[str, int, int]] = []
    paths = [tmp_path / f"{index}.{kind}" for index in range(2)]
    for index, path in enumerate(paths):
        callback = (lambda *report: reports.append(report)) if index else None
        kwargs = {"on_progress": callback} if callback else {}
        if kind == "fgl":
            fiction.write_fgl_layout(gate_layout, str(path), **kwargs)
        elif kind == "dot":
            fiction.write_dot_layout(gate_layout, str(path), **kwargs)
        elif kind == "qll":
            fiction.write_qll_layout(fiction.apply_sim7_mol_library(gate_layout), str(path), **kwargs)
        elif kind in {"sqd", "sidb_svg"}:
            layout = fiction.sidb_layout()
            layout.assign_sidb(fiction.lattice_site(0, 0, 0), fiction.sidb_dot_tag.NORMAL)
            layout.assign_sidb(fiction.lattice_site(2, 1, 1), fiction.sidb_dot_tag.NORMAL)
            if kind == "sqd":
                fiction.write_sqd_layout(layout, str(path), **kwargs)
            else:
                params = fiction.write_sidb_layout_svg_params()
                if callback:
                    params.on_progress = callback
                fiction.write_sidb_layout_svg(layout, str(path), params)
        else:
            layout = fiction.apply_qca_one_library(gate_layout)
            params = getattr(fiction, f"write_{'qca_layout_svg' if kind == 'svg' else kind + '_layout'}_params")()
            if callback:
                params.on_progress = callback
            getattr(fiction, f"write_{'qca_layout_svg' if kind == 'svg' else kind + '_layout'}")(
                layout, str(path), params
            )
    if kind == "sqd":
        assert fiction.read_sqd_layout(str(paths[0])) == fiction.read_sqd_layout(str(paths[1]))
    else:
        assert paths[0].read_bytes() == paths[1].read_bytes()
    final = {task: (done, total) for task, done, total in reports}
    assert final
    assert all(done == total for done, total in final.values())
    assert any(total > 0 for done, total in final.values())


@pytest.mark.parametrize("threads", [1, 4])
def test_clustercomplete_worker_counts(resources_dir: Path, threads: int) -> None:
    """Serial and stolen compositions contribute to the same aggregate count."""
    layout = fiction.read_sqd_layout(str(resources_dir / "21_hex_inputsdbp_and_v19.sqd"))
    params = fiction.clustercomplete_params()
    params.available_threads = threads
    params.simulation_parameters.base = 2
    reports: list[tuple[int, int, str, int, int, bool]] = []
    counts: list[tuple[str, int, int]] = []
    params.on_worker_progress = lambda *report: reports.append(report)
    params.on_progress = lambda *report: counts.append(report)
    result = fiction.clustercomplete(layout, params)
    assert result.charge_distributions
    assert reports
    final = {worker: done for worker, count, description, done, total, active in reports if not active}
    assert all(total == 0 and count == threads for worker, count, description, done, total, active in reports)
    assert sum(final.values()) == next(done for task, done, _ in reversed(counts) if task == "compositions")


def test_exact_skips_candidates_outside_area_bound(mux21: fiction.technology_network) -> None:
    """Candidates rejected by the area bound never appear as active solvers."""
    params = fiction.exact_params()
    params.upper_bound_area = 1
    reports: list[tuple[int, int, str, int, int, bool]] = []
    params.on_worker_progress = lambda *report: reports.append(report)
    assert fiction.exact_cartesian(mux21, params) is None
    assert not reports


def test_exact_clears_candidates_on_timeout(mux21: fiction.technology_network) -> None:
    """A timeout leaves no active candidate, whether it interrupts a solver or prevents its start."""
    params = fiction.exact_params()
    params.num_threads = 4
    params.timeout = 1
    reports: list[tuple[int, int, str, int, int, bool]] = []
    params.on_worker_progress = lambda *report: reports.append(report)
    fiction.exact_cartesian(mux21, params)
    active = {worker: running for worker, count, description, done, total, running in reports}
    assert not any(active.values())


def test_temperature_forwards_simulation_workers(resources_dir: Path) -> None:
    """Temperature analysis keeps outer phases and forwards the active simulation's workers."""
    layout = fiction.read_sqd_layout(str(resources_dir / "hex_11_inputsdbp_inv_straight_v0_manual.sqd"))
    params = fiction.critical_temperature_params()
    params.operational_params.sim_engine = fiction.sidb_simulation_engine.CLUSTERCOMPLETE
    params.operational_params.simulation_parameters.base = 2
    workers: list[tuple[int, int, str, int, int, bool]] = []
    phases: list[tuple[str, int, int]] = []
    params.on_worker_progress = lambda *report: workers.append(report)
    params.on_progress = lambda *report: phases.append(report)
    fiction.critical_temperature_gate_based(layout, [fiction.create_not_tt()], params)
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
    layout = getattr(fiction, f"{topology}_gate_layout")()
    reports: list[tuple[str, int, int]] = []
    result = getattr(fiction, f"apply_{library}_library")(
        layout, lambda task, done, total: reports.append((task, done, total))
    )
    assert result.is_empty()
    assert reports
    assert all(done == total == 0 for _, done, total in reports)


def test_failed_mapping_retains_completed_count() -> None:
    """Unsupported routing leaves the last completed count below the mapping total."""
    layout = fiction.shifted_cartesian_gate_layout((1, 1), "2DDWave", "unsupported routing")
    source = layout.create_pi("a", (0, 0))
    layout.create_po(source, "f", (0, 1))
    reports: list[tuple[str, int, int]] = []
    with pytest.raises(ValueError, match="unsupported gate orientation"):
        fiction.apply_topolinano_library(layout, lambda task, done, total: reports.append((task, done, total)))
    assert reports
    assert reports[-1][1] < reports[-1][2]


def test_qcc_writer_counts_and_output(mux21: fiction.technology_network, tmp_path: Path) -> None:
    """QCC callbacks count rows without changing the serialized layout."""
    placement = fiction.exact_params()
    placement.scheme = "COLUMNAR3"
    placement.crossings = True
    placement.border_io = True
    placement.technology_specifics = fiction.technology_constraints.TOPOLINANO
    layout = fiction.apply_topolinano_library(fiction.exact_shifted_cartesian(mux21, placement))
    before = tmp_path / "before.qcc"
    after = tmp_path / "after.qcc"
    fiction.write_qcc_layout(layout, str(before))
    params = fiction.write_qcc_layout_params()
    reports: list[tuple[str, int, int]] = []
    params.on_progress = lambda *report: reports.append(report)
    fiction.write_qcc_layout(layout, str(after), params)
    assert before.read_bytes() == after.read_bytes()
    assert reports[-1][1] == reports[-1][2] > 0
