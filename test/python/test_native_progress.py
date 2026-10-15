# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""Progress callbacks preserve results and report real completed work."""

from __future__ import annotations

import gc
import re
import sys
import threading
import weakref
from typing import TYPE_CHECKING, Any

import pytest

from mnt.pyfiction import layouts, physical_design
from mnt.pyfiction.fcn.io import write_qll_layout
from mnt.pyfiction.inml.io import write_qcc_layout
from mnt.pyfiction.layouts import arrangement, cartesian_gate_layout, shifted_cartesian_gate_layout
from mnt.pyfiction.layouts.io import write_dot_layout, write_fgl_layout
from mnt.pyfiction.physical_design import (
    apply_qca_one_library,
    apply_sim7_mol_library,
    apply_topolinano_library,
    exact,
    graph_oriented_layout_design,
    orthogonal,
    technology_constraints,
)
from mnt.pyfiction.qca import io as qca_io
from mnt.pyfiction.sidb import lattice_site, sidb_dot_tag, sidb_layout
from mnt.pyfiction.sidb.io import read_sqd_layout, write_sidb_layout_svg, write_sqd_layout
from mnt.pyfiction.sidb.simulation import sidb_simulation_engine
from mnt.pyfiction.sidb.simulation.analysis import critical_temperature_gate_based
from mnt.pyfiction.sidb.simulation.engines import clustercomplete
from mnt.pyfiction.synthesis import create_not_tt, fanout_substitution, network_balancing
from mnt.pyfiction.verification import (
    critical_path_length_and_throughput,
    eq_type,
    equivalence_checking,
    gate_level_drvs,
)

if TYPE_CHECKING:
    from pathlib import Path

    from mnt.pyfiction.networks import technology_network


@pytest.mark.parametrize("threads", [1, 4])
def test_exact_candidate_lifecycle(mux21: technology_network, threads: int) -> None:
    """Solver candidates use tile dimensions and leave no active worker after success."""
    options: dict[str, Any] = {
        "num_threads": threads,
    }
    reports: list[tuple[int, int, str, int, int, bool]] = []
    options["on_worker_progress"] = lambda *report: reports.append(report)
    layout = exact(mux21, **options)
    assert layout is not None
    assert equivalence_checking(mux21, layout) == eq_type.STRONG
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
def test_gold_graph_expansions(mux21: technology_network, *, parallel: bool) -> None:
    """Stable graphs report expansions; reusable callbacks leave no native owner after return."""
    options: dict[str, Any] = {
        "enable_multithreading": parallel,
        "return_first": True,
        "seed": 7,
    }
    reports: list[tuple[int, int, str, int, int, bool]] = []
    counts: list[tuple[str, int, int]] = []
    options["on_worker_progress"] = lambda *report: reports.append(report)
    options["on_progress"] = lambda *report: counts.append(report)
    callback_refs = [weakref.ref(options[name]) for name in ("on_progress", "on_worker_progress")]
    assert graph_oriented_layout_design(mux21, **options) is not None
    reports.clear()
    counts.clear()
    layout = graph_oriented_layout_design(mux21, **options)
    assert layout is not None
    assert equivalence_checking(mux21, layout) != eq_type.NO
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
    del options
    gc.collect()
    assert all(reference() is None for reference in callback_refs)


@pytest.mark.parametrize("command", ["balance", "fanouts", "check"])
def test_counted_native_phases(mux21: technology_network, command: str) -> None:
    """Source-gate passes and enabled checks finish at their declared totals."""
    reports: list[tuple[str, int, int]] = []

    def callback(*report):
        return reports.append(report)

    if command == "balance":
        balancing_options: dict[str, Any] = {
            "on_progress": callback,
        }
        result = network_balancing(mux21, **balancing_options)
        assert equivalence_checking(mux21, result) != eq_type.NO
    elif command == "fanouts":
        substitution_options: dict[str, Any] = {
            "on_progress": callback,
        }
        result = fanout_substitution(mux21, **substitution_options)
        assert equivalence_checking(mux21, result) != eq_type.NO
    else:
        drv_options: dict[str, Any] = {
            "on_progress": callback,
        }
        gate_level_drvs(orthogonal(mux21), **drv_options)
    final = {task: (done, total) for task, done, total in reports}
    assert final
    assert all(done == total for done, total in final.values())


def test_progress_callback_exception_propagates(mux21: technology_network) -> None:
    """A Python exception survives the native callback and split backend unchanged."""
    error = RuntimeError("callback failed")

    def fail(*_report: object) -> None:
        """Raise the original Python exception from inside a native algorithm."""
        raise error

    with pytest.raises(RuntimeError, match="callback failed") as caught:
        network_balancing(mux21, on_progress=fail)
    assert caught.value is error


@pytest.mark.parametrize("kind", ["fgl", "dot", "qll", "qca", "svg", "sqd", "sidb_svg"])
def test_writer_counts_and_output(mux21: technology_network, tmp_path: Path, kind: str) -> None:
    """Callbacks leave writer output unchanged and count every completed phase."""
    gate_layout = orthogonal(mux21)
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
            layout = sidb_layout()
            layout.assign_sidb(lattice_site(0, 0, 0), sidb_dot_tag.NORMAL)
            layout.assign_sidb(lattice_site(2, 1, 1), sidb_dot_tag.NORMAL)
            if kind == "sqd":
                write_sqd_layout(layout, str(path), **kwargs)
            else:
                write_sidb_layout_svg(layout, str(path), **kwargs)
        else:
            qca_layout = apply_qca_one_library(gate_layout)
            getattr(qca_io, f"write_{'qca_layout_svg' if kind == 'svg' else kind + '_layout'}")(
                qca_layout, str(path), **kwargs
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
    options: dict[str, Any] = {
        "available_threads": threads,
        "base": 2,
    }
    reports: list[tuple[int, int, str, int, int, bool]] = []
    counts: list[tuple[str, int, int]] = []
    options["on_worker_progress"] = lambda *report: reports.append(report)
    options["on_progress"] = lambda *report: counts.append(report)
    result = clustercomplete(layout, **options)
    assert result.charge_distributions
    assert reports
    final = {worker: done for worker, count, description, done, total, active in reports if not active}
    assert all(total == 0 and count == threads for worker, count, description, done, total, active in reports)
    assert sum(final.values()) == next(done for task, done, _ in reversed(counts) if task == "compositions")


def test_exact_skips_candidates_outside_area_bound(mux21: technology_network) -> None:
    """Candidates rejected by the area bound never appear as active solvers."""
    options: dict[str, Any] = {
        "upper_bound_area": 1,
    }
    reports: list[tuple[int, int, str, int, int, bool]] = []
    options["on_worker_progress"] = lambda *report: reports.append(report)
    assert exact(mux21, **options) is None
    assert not reports


def test_exact_clears_candidates_on_timeout(mux21: technology_network) -> None:
    """A timeout leaves no active candidate, whether it interrupts a solver or prevents its start."""
    options: dict[str, Any] = {
        "num_threads": 4,
        "timeout": 1,
    }
    reports: list[tuple[int, int, str, int, int, bool]] = []
    options["on_worker_progress"] = lambda *report: reports.append(report)
    exact(mux21, **options)
    active = {worker: running for worker, count, description, done, total, running in reports}
    assert not any(active.values())


def test_temperature_forwards_simulation_workers(resources_dir: Path) -> None:
    """Temperature analysis keeps outer phases and forwards the active simulation's workers."""
    layout = read_sqd_layout(str(resources_dir / "hex_11_inputsdbp_inv_straight_v0_manual.sqd"))
    options: dict[str, Any] = {
        "sim_engine": sidb_simulation_engine.CLUSTERCOMPLETE,
        "base": 2,
    }
    workers: list[tuple[int, int, str, int, int, bool]] = []
    phases: list[tuple[str, int, int]] = []
    options["on_worker_progress"] = lambda *report: workers.append(report)
    options["on_progress"] = lambda *report: phases.append(report)
    critical_temperature_gate_based(layout, [create_not_tt()], **options)
    assert workers
    assert phases
    active = {worker: running for worker, count, description, done, total, running in workers}
    assert not any(active.values())


ARRANGEMENTS = {"hexagonal": arrangement.EVEN_ROW, "shifted_cartesian": arrangement.ODD_COLUMN}
"""The arrangement under which each non-Cartesian topology is tested."""


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
    make_layout = getattr(layouts, f"{topology}_gate_layout")
    layout = make_layout() if topology == "cartesian" else make_layout(ARRANGEMENTS[topology])
    reports: list[tuple[str, int, int]] = []
    result = getattr(physical_design, f"apply_{library}_library")(
        layout, lambda task, done, total: reports.append((task, done, total))
    )
    assert result.is_empty()
    assert reports
    assert all(done == total == 0 for _, done, total in reports)


def test_failed_mapping_retains_completed_count() -> None:
    """Unsupported routing leaves the last completed count below the mapping total."""
    layout = shifted_cartesian_gate_layout(arrangement.ODD_COLUMN, (1, 1), "2DDWave", "unsupported routing")
    source = layout.create_pi("a", (0, 0))
    layout.create_po(source, "f", (0, 1))
    reports: list[tuple[str, int, int]] = []
    with pytest.raises(ValueError, match="unsupported gate orientation"):
        apply_topolinano_library(layout, lambda task, done, total: reports.append((task, done, total)))
    assert reports
    assert reports[-1][1] < reports[-1][2]


def test_qcc_writer_counts_and_output(mux21: technology_network, tmp_path: Path) -> None:
    """QCC callbacks count rows without changing the serialized layout."""
    placement: dict[str, Any] = {
        "scheme": "COLUMNAR3",
        "crossings": True,
        "border_io": True,
        "technology_specifics": technology_constraints.TOPOLINANO,
        "layout_arrangement": arrangement.ODD_COLUMN,
    }
    gate_layout = exact(mux21, layout_type=shifted_cartesian_gate_layout, **placement)
    assert gate_layout is not None
    layout = apply_topolinano_library(gate_layout)
    before = tmp_path / "before.qcc"
    after = tmp_path / "after.qcc"
    write_qcc_layout(layout, str(before))
    options: dict[str, Any] = {}
    reports: list[tuple[str, int, int]] = []
    options["on_progress"] = lambda *report: reports.append(report)
    write_qcc_layout(layout, str(after), **options)
    assert before.read_bytes() == after.read_bytes()
    assert reports[-1][1] == reports[-1][2] > 0


@pytest.mark.parametrize("kind", ["balance", "exact", "worker", "both"])
@pytest.mark.parametrize("raises", [False, True])
def test_progress_callbacks_are_released(mux21: technology_network, kind: str, *, raises: bool) -> None:
    """Native calls release callback objects after success and callback exceptions."""

    def invoke() -> weakref.ReferenceType[Any]:
        """Call an algorithm and release its callback.

        Returns:
            A weak reference to the completed call's callback.
        """

        class Callback:
            """Record invocation and optionally raise from a native callback."""

            calls = 0

            def __call__(self, *_report: object) -> None:
                """Count progress and exercise exception propagation.

                Raises:
                    RuntimeError: When the test requests a callback exception.
                """
                self.calls += 1
                if raises:
                    msg = "callback failed"
                    raise RuntimeError(msg)

        callback = Callback()
        callback_ref = weakref.ref(callback)
        options: dict[str, Any] = {}
        if kind in {"worker", "both"}:
            options["on_worker_progress"] = callback
        if kind != "worker":
            options["on_progress"] = callback
        algorithm: Any = network_balancing if kind == "balance" else exact
        if kind != "balance":
            options["num_threads"] = 2
        if raises:
            with pytest.raises(RuntimeError, match="callback failed"):
                algorithm(mux21, **options)
        else:
            assert algorithm(mux21, **options) is not None
        assert callback.calls > 0
        return callback_ref

    for _ in range(2):
        callback_ref = invoke()
        gc.collect()
        assert callback_ref() is None


def test_long_route_statistics_release_the_gil() -> None:
    """Deep layout summaries finish while Python display threads can run."""
    length = 200_000
    layout = cartesian_gate_layout((length, 0), "2DDWave")
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
