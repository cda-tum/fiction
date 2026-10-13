# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""Public contracts for keyword-only algorithm options."""

from __future__ import annotations

import gc
import subprocess  # ruff: ignore[suspicious-subprocess-import] -- bounded native guard regressions
import sys
import weakref
from importlib import import_module
from typing import TYPE_CHECKING, Any, cast

import pytest

from mnt.pyfiction.sidb import lattice_site, sidb_dot_tag, sidb_layout
from mnt.pyfiction.sidb.model import potential_to_distance_conversion, sidb_simulation_parameters
from mnt.pyfiction.sidb.simulation import potential_landscape
from mnt.pyfiction.sidb.simulation.engines import automatic_base_number_detection, quickexact, quicksim
from mnt.pyfiction.synthesis import (
    and_or_not,
    is_balanced,
    missing_required_gates_exception,
    network_balancing,
    technology_mapping,
)

if TYPE_CHECKING:
    from collections.abc import Callable

    from typing_extensions import assert_type

    from mnt.pyfiction.networks import technology_network


def test_balancing_keyword_options(mux21: technology_network) -> None:
    """Algorithms accept keywords and reject positional options."""
    balanced = network_balancing(mux21, unify_outputs=True)
    assert is_balanced(balanced, unify_outputs=True)
    with pytest.raises(TypeError):
        cast("Callable[..., Any]", network_balancing)(mux21, *(True,))


def test_gate_factory_keyword_options(mux21: technology_network) -> None:
    """Gate factories produce mapping keywords."""
    options = and_or_not()
    if TYPE_CHECKING:
        assert_type(options, dict[str, bool])
    assert isinstance(options, dict)
    assert technology_mapping(mux21, **cast("dict[str, Any]", options)).num_gates() > 0


@pytest.mark.parametrize(
    ("module_name", "class_name"),
    [
        ("mnt.pyfiction.physical_design", "color_routing_params"),
        ("mnt.pyfiction.physical_design", "exact_params"),
        ("mnt.pyfiction.physical_design", "graph_oriented_layout_design_params"),
        ("mnt.pyfiction.physical_design", "hexagonalization_params"),
        ("mnt.pyfiction.physical_design", "orthogonal_params"),
        ("mnt.pyfiction.physical_design", "post_layout_optimization_params"),
        ("mnt.pyfiction.physical_design", "wiring_reduction_params"),
        ("mnt.pyfiction.verification", "gate_level_drv_params"),
        ("mnt.pyfiction.sidb.generators", "design_sidb_gates_params"),
        ("mnt.pyfiction.sidb.generators", "sidb_on_the_fly_gate_library_params"),
        ("mnt.pyfiction.sidb.generators", "on_the_fly_sidb_circuit_design_params"),
        ("mnt.pyfiction.sidb.generators", "generate_random_sidb_layout_params"),
        ("mnt.pyfiction.sidb.io", "write_sidb_layout_svg_params"),
        ("mnt.pyfiction.sidb.simulation.analysis", "critical_temperature_params"),
        ("mnt.pyfiction.sidb.simulation.analysis", "physical_population_stability_params"),
        ("mnt.pyfiction.sidb.simulation.analysis", "time_to_solution_params"),
        ("mnt.pyfiction.sidb.simulation.defects", "displacement_robustness_domain_params"),
        ("mnt.pyfiction.sidb.simulation.engines", "clustercomplete_params"),
        ("mnt.pyfiction.sidb.simulation.engines", "quickexact_params"),
        ("mnt.pyfiction.sidb.simulation.engines", "quicksim_params"),
        ("mnt.pyfiction.sidb.simulation.io", "write_operational_domain_params"),
        ("mnt.pyfiction.sidb.simulation.logic", "bdl_input_iterator_params"),
        ("mnt.pyfiction.sidb.simulation.logic", "detect_bdl_pairs_params"),
        ("mnt.pyfiction.sidb.simulation.logic", "detect_bdl_wires_params"),
        ("mnt.pyfiction.sidb.simulation.logic", "is_operational_params"),
        ("mnt.pyfiction.sidb.simulation.logic", "operational_domain_params"),
        ("mnt.pyfiction.sidb.simulation.logic", "operational_domain_ratio_params"),
        ("mnt.pyfiction.qca.io", "write_qca_layout_params"),
        ("mnt.pyfiction.qca.io", "write_qca_layout_svg_params"),
        ("mnt.pyfiction.physical_design.path_finding", "a_star_params"),
        ("mnt.pyfiction.physical_design.path_finding", "enumerate_all_paths_params"),
        ("mnt.pyfiction.physical_design.path_finding", "yen_k_shortest_paths_params"),
        ("mnt.pyfiction.mol_qca.io", "write_mol_qca_layout_svg_params"),
        ("mnt.pyfiction.inml.io", "write_qcc_layout_params"),
        ("mnt.pyfiction.synthesis", "network_balancing_params"),
        ("mnt.pyfiction.synthesis", "fanout_substitution_params"),
        ("mnt.pyfiction.synthesis", "technology_mapping_params"),
    ],
)
def test_algorithm_options_classes_are_removed(module_name: str, class_name: str) -> None:
    """Algorithm configuration uses keyword arguments across every public module."""
    assert not hasattr(import_module(module_name), class_name)


@pytest.mark.parametrize("base", [0, 1, 4, 255])
def test_physics_rejects_invalid_base(base: int) -> None:
    """Invalid bases raise Python errors before entering the native model constructor."""

    with pytest.raises(ValueError, match="base must be 2 or 3"):
        quickexact(sidb_layout(), base=base)
    with pytest.raises(ValueError, match="base must be 2 or 3"):
        sidb_simulation_parameters(base_number=base)


def test_physics_keywords_and_fresh_defaults() -> None:
    """Custom physical values reach the simulation result and do not alter later defaults."""

    layout = sidb_layout()
    layout.assign_sidb(lattice_site(0, 0, 0), sidb_dot_tag.NORMAL)
    result = quickexact(
        layout,
        base=2,
        mu_minus=-0.25,
        epsilon_r=6.0,
        lambda_tf=4.0,
        base_number_detection=automatic_base_number_detection.OFF,
    )
    model = result.simulation_parameters
    assert (model.base, model.mu_minus, model.epsilon_r, model.lambda_tf) == (2, -0.25, 6.0, 4.0)
    assert result.groundstates()
    model = quickexact(layout, base_number_detection=automatic_base_number_detection.OFF).simulation_parameters
    assert (model.base, model.mu_minus, model.epsilon_r, model.lambda_tf) == (3, -0.32, 5.6, 5.0)


def test_potential_maps_are_per_call() -> None:
    """Each constructor copies supplied maps and recreates omitted potentials."""

    layout = sidb_layout()
    site = lattice_site(0, 0, 0)
    layout.assign_sidb(site, sidb_dot_tag.NORMAL)
    potentials = {site: 0.25}
    landscape = potential_landscape(layout, local_external_potential=potentials)
    potentials.clear()
    assert landscape.local_external_potential(0) == 0.25
    assert potential_landscape(layout).local_external_potential(0) == 0.0


@pytest.mark.parametrize("raises", [False, True])
def test_per_call_callback_lifetime(mux21: technology_network, *, raises: bool) -> None:
    """Completed and failed calls release their native callback references."""

    class Callback:
        """Record native progress, optionally failing during the callback."""

        def __init__(self) -> None:
            self.called = False

        def __call__(self, _task: str, _completed: int, _total: int) -> None:
            self.called = True
            if raises:
                msg = "callback failed"
                raise RuntimeError(msg)

    callback = Callback()
    reference = weakref.ref(callback)
    if raises:
        with pytest.raises(RuntimeError, match="callback failed"):
            network_balancing(mux21, on_progress=callback)
    else:
        network_balancing(mux21, on_progress=callback)
    assert callback.called
    del callback
    gc.collect()
    assert reference() is None


@pytest.mark.parametrize(
    ("factory_name", "enabled"),
    [
        ("and_or_not", {"inv", "and2", "or2"}),
        ("and_or_not_maj", {"inv", "and2", "or2", "maj3"}),
        ("all_standard_2_input_functions", {"inv", "and2", "nand2", "or2", "nor2", "xor2", "xnor2"}),
        (
            "all_standard_3_input_functions",
            {"inv", "and3", "xor_and", "or_and", "onehot", "maj3", "gamble", "dot", "mux", "and_xor"},
        ),
        (
            "all_supported_standard_functions",
            {
                "inv",
                "and2",
                "nand2",
                "or2",
                "nor2",
                "xor2",
                "xnor2",
                "and3",
                "xor_and",
                "or_and",
                "onehot",
                "maj3",
                "gamble",
                "dot",
                "mux",
                "and_xor",
            },
        ),
    ],
)
def test_factory_gate_sets(factory_name: str, enabled: set[str], mux21: technology_network) -> None:
    """Each factory enables its documented gates and returns usable mapping keywords."""
    factory = getattr(import_module("mnt.pyfiction.synthesis"), factory_name)
    options = factory()
    assert {name for name, selected in options.items() if selected} == enabled
    assert all(isinstance(selected, bool) for selected in options.values())
    assert technology_mapping(mux21, **cast("dict[str, Any]", options)).num_gates() > 0


@pytest.mark.parametrize("base", [-1, 256])
def test_physics_rejects_out_of_range_base(base: int) -> None:
    """Values outside the native unsigned-byte range fail conversion."""
    with pytest.raises(TypeError):
        quickexact(sidb_layout(), base=base)


def test_explicit_none_progress(mux21: technology_network) -> None:
    """Omitted and explicit None callbacks have the same behavior."""
    assert network_balancing(mux21, on_progress=None).num_gates() == network_balancing(mux21).num_gates()


@pytest.mark.parametrize("engine", [quickexact, quicksim])
def test_engine_keyword_boundary(engine: Callable[..., Any]) -> None:
    """Every engine validates the physical model and rejects positional options."""
    with pytest.raises(ValueError, match="base must be 2 or 3"):
        engine(sidb_layout(), base=1)
    with pytest.raises(TypeError):
        engine(sidb_layout(), 2)


def test_mapping_default_gate_set(mux21: technology_network) -> None:
    """No gates are enabled by default, so mapping requires an explicit gate set."""
    with pytest.raises(missing_required_gates_exception, match="missing required gates"):
        technology_mapping(mux21)
    assert technology_mapping(mux21, **cast("dict[str, Any]", and_or_not())).num_gates() > 0


def _assert_distance_rejection(call: str, exception_type: str, message: str = "") -> None:
    """Check native input rejection in a child that cannot hang the test suite."""
    script = f"""from mnt.pyfiction.sidb import sidb_layout
from mnt.pyfiction.sidb.model import potential_to_distance_conversion
from mnt.pyfiction.sidb.simulation.analysis import physical_population_stability
try:
    {call}
except {exception_type} as error:
    assert {message!r} in str(error)
else:
    raise AssertionError("input was accepted")
"""
    subprocess.run([sys.executable, "-c", script], check=True, timeout=10, shell=False)  # ruff: ignore[subprocess-without-shell-equals-true] -- fixed interpreter and constant regression calls


@pytest.mark.parametrize("precision", [-1, 2.7])
def test_distance_conversion_rejects_non_unsigned_precision(precision: float) -> None:
    """Precision preserves the native unsigned integer contract."""
    _assert_distance_rejection(f"potential_to_distance_conversion(0.5, {precision!r})", "TypeError")


@pytest.mark.parametrize("potential", [0.0, -0.1, float("nan"), -float("inf")])
def test_distance_conversion_rejects_invalid_potential(potential: float) -> None:
    """Invalid potentials fail before entering the native distance scan."""
    _assert_distance_rejection(
        f"potential_to_distance_conversion(float({str(potential)!r}), 2)",
        "ValueError",
        "potential must be positive",
    )


def test_distance_conversion_precision_and_infinite_potential() -> None:
    """Integer precision controls the scan step, including positive infinity."""
    assert potential_to_distance_conversion(0.01, 2, epsilon_r=2.0, lambda_tf=1.0) == pytest.approx(314 / 100)
    assert potential_to_distance_conversion(float("inf"), 3) == pytest.approx(0.001)
    _assert_distance_rejection(
        "potential_to_distance_conversion(0.5, 1000)",
        "ValueError",
        "precision must yield a positive finite step",
    )


def test_population_stability_rejects_zero_distance_step() -> None:
    """Both native distance-conversion callers reject unusable precision."""
    _assert_distance_rejection(
        "physical_population_stability(sidb_layout(), precision_for_distance_corresponding_to_potential=1000)",
        "ValueError",
        "precision must yield a positive finite step",
    )
