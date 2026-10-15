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
from mnt.pyfiction.sidb.simulation.logic import detect_bdl_pairs
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


@pytest.mark.parametrize("name", ["bdl_input_iterator", "generate_bdl_input_pattern_layouts"])
def test_explicit_wires_reject_detection_threshold(name: str) -> None:
    """Supplying wires omits the threshold used only by automatic wire detection."""
    algorithm = getattr(import_module("mnt.pyfiction.sidb.simulation.logic"), name)
    with pytest.raises(TypeError):
        algorithm(sidb_layout(), [], threshold_bdl_interdistance=1.0)


@pytest.mark.parametrize(
    "option",
    [
        "threshold_bdl_interdistance",
        "minimum_distance",
        "maximum_distance",
        "input_bdl_config",
        "op_condition",
        "strategy_to_analyze_operational_status",
    ],
)
def test_non_gate_temperature_rejects_gate_options(option: str) -> None:
    """Non-gate temperature analysis accepts physical options rather than gate checks."""
    algorithm = import_module("mnt.pyfiction.sidb.simulation.analysis").critical_temperature_non_gate_based
    options: dict[str, Any] = {option: 1}
    with pytest.raises(TypeError):
        algorithm(sidb_layout(), **options)


def test_supplied_operational_wires_reject_detection_threshold() -> None:
    """Operational checks use the supplied wires rather than a detection threshold."""
    _assert_native_rejection(
        "is_operational(sidb_layout(), [], [], [], threshold_bdl_interdistance=1.0)",
        "TypeError",
    )


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


def _assert_native_rejection(call: str, exception_type: str, message: str = "") -> None:
    """Check native input rejection in a child that cannot hang the test suite."""
    script = f"""from mnt.pyfiction.sidb import sidb_layout
from mnt.pyfiction.sidb.model import potential_to_distance_conversion
from mnt.pyfiction.sidb.simulation.analysis import (
    physical_population_stability, critical_temperature_non_gate_based,
    time_to_solution_for_given_simulation_results,
    critical_temperature_gate_based, time_to_solution,
)
from mnt.pyfiction.sidb.simulation.defects import determine_displacement_robustness_domain
from mnt.pyfiction.sidb.simulation import sidb_simulation_result
from mnt.pyfiction.sidb.simulation.logic import (
    bdl_input_iterator, detect_bdl_pairs, detect_bdl_wires,
    generate_bdl_input_pattern_layouts, is_operational,
)
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
    _assert_native_rejection(f"potential_to_distance_conversion(0.5, {precision!r})", "TypeError")


@pytest.mark.parametrize("potential", [0.0, -0.1, float("nan"), -float("inf")])
def test_distance_conversion_rejects_invalid_potential(potential: float) -> None:
    """Invalid potentials fail before entering the native distance scan."""
    _assert_native_rejection(
        f"potential_to_distance_conversion(float({str(potential)!r}), 2)",
        "ValueError",
        "potential must be positive",
    )


def test_distance_conversion_precision_and_infinite_potential() -> None:
    """Integer precision controls the scan step, including positive infinity."""
    assert potential_to_distance_conversion(0.01, 2, epsilon_r=2.0, lambda_tf=1.0) == pytest.approx(314 / 100)
    assert potential_to_distance_conversion(float("inf"), 3) == pytest.approx(0.001)
    _assert_native_rejection(
        "potential_to_distance_conversion(0.5, 1000)",
        "ValueError",
        "precision must yield a positive finite step",
    )


def test_distance_conversion_high_precision_completes() -> None:
    """Fine distance precision completes without a linear scan of the distance grid."""
    subprocess.run(
        [
            sys.executable,
            "-c",
            (
                "from mnt.pyfiction.sidb.model import potential_to_distance_conversion; "
                "assert 0.4 < potential_to_distance_conversion(0.5, 20) < 0.5"
            ),
        ],
        check=True,
        timeout=10,
        shell=False,
    )


@pytest.mark.parametrize("option", ["epsilon_r", "lambda_tf"])
@pytest.mark.parametrize("value", [0.0, -1.0, float("nan"), float("inf")])
def test_physics_rejects_invalid_positive_values(option: str, value: float) -> None:
    """The physical model rejects nonpositive and non-finite screening values."""
    _assert_native_rejection(
        f"potential_to_distance_conversion(0.5, 2, {option}=float({str(value)!r}))",
        "ValueError",
        option,
    )


def test_population_stability_rejects_zero_distance_step() -> None:
    """Both native distance-conversion callers reject unusable precision."""
    _assert_native_rejection(
        "physical_population_stability(sidb_layout(), precision_for_distance_corresponding_to_potential=1000)",
        "ValueError",
        "precision must yield a positive finite step",
    )


@pytest.mark.parametrize(
    "bounds",
    ["minimum_distance=2.0, maximum_distance=1.0", "minimum_distance=float('nan')", "maximum_distance=float('nan')"],
)
@pytest.mark.parametrize(
    "call",
    [
        "detect_bdl_pairs(sidb_layout(), {bounds})",
        "detect_bdl_wires(sidb_layout(), {bounds})",
        "bdl_input_iterator(sidb_layout(), {bounds})",
        "bdl_input_iterator(sidb_layout(), [], {bounds})",
        "generate_bdl_input_pattern_layouts(sidb_layout(), {bounds})",
        "generate_bdl_input_pattern_layouts(sidb_layout(), [], {bounds})",
        "is_operational(sidb_layout(), [], {bounds})",
    ],
)
def test_bdl_distance_bounds_reject_invalid_intervals(call: str, bounds: str) -> None:
    """BDL detection and shared algorithm options reject invalid intervals before native assertions."""
    _assert_native_rejection(
        call.format(bounds=bounds),
        "ValueError",
        "minimum_distance must not exceed maximum_distance and neither may be NaN",
    )


@pytest.mark.parametrize(
    ("minimum", "maximum"), [(0.0, 0.0), (-2.0, -1.0), (float("-inf"), float("inf")), (float("inf"), float("inf"))]
)
def test_bdl_distance_bounds_preserve_ordered_intervals(minimum: float, maximum: float) -> None:
    """Zero, negative, and infinite bounds preserve the native ordered interval contract."""
    assert detect_bdl_pairs(sidb_layout(), minimum_distance=minimum, maximum_distance=maximum) == []


@pytest.mark.parametrize("confidence", [-0.1, 0.0, 1.0, 1.1, float("nan"), float("inf")])
@pytest.mark.parametrize(
    "call",
    [
        "critical_temperature_non_gate_based(sidb_layout(), {option})",
        "critical_temperature_gate_based(sidb_layout(), [], {option})",
        "time_to_solution(sidb_layout(), {option})",
        "time_to_solution_for_given_simulation_results(sidb_simulation_result(), [], {option})",
    ],
)
def test_invalid_analysis_confidence(call: str, confidence: float) -> None:
    """Analysis rejects confidence outside the open unit interval before simulation."""
    _assert_native_rejection(
        call.format(option=f"confidence_level=float({str(confidence)!r})"), "ValueError", "confidence_level"
    )


@pytest.mark.parametrize("temperature", [-1.0, float("nan"), float("inf"), 1e20])
def test_invalid_temperature_limit(temperature: float) -> None:
    """Temperature limits must fit the native unsigned iteration count."""
    _assert_native_rejection(
        f"critical_temperature_non_gate_based(sidb_layout(), max_temperature=float({str(temperature)!r}))",
        "ValueError",
        "max_temperature",
    )


@pytest.mark.parametrize("fraction", [-0.1, 1.1, float("nan"), float("inf")])
def test_invalid_displacement_fraction(fraction: float) -> None:
    """Displacement analysis rejects fractions outside the closed unit interval."""
    _assert_native_rejection(
        "determine_displacement_robustness_domain(sidb_layout(), [], "
        f"percentage_of_analyzed_displaced_layouts=float({str(fraction)!r}))",
        "ValueError",
        "percentage_of_analyzed_displaced_layouts",
    )
