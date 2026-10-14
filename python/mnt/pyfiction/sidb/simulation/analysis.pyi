# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""Physical analyses of SiDB layouts built on simulation."""

import enum
from collections.abc import Callable, Mapping, Sequence
from typing import overload

import mnt.pyfiction.sidb
import mnt.pyfiction.sidb.simulation
import mnt.pyfiction.sidb.simulation.logic
import mnt.pyfiction.synthesis

class state_type(enum.Enum):
    """Label to categorize ground and excited states of an SiDB layout."""

    ACCEPTED = 0
    """
    A state is accepted if the charge distribution encodes the desired
    logic.
    """

    REJECTED = 1
    """
    A state is rejected if the charge distribution does not encode the
    desired logic. Moreover, if kinks are rejected, a charge distribution
    that encodes the logic, but does show kinks, is rejected.
    """

def calculate_energy_and_state_type_with_kinks_accepted(
    energy_distribution: energy_distribution,
    valid_charge_distributions: Sequence[mnt.pyfiction.sidb.charge_distribution],
    output_bdl_pairs: Sequence[mnt.pyfiction.sidb.simulation.logic.bdl_pair],
    spec: Sequence[mnt.pyfiction.synthesis.dynamic_truth_table],
    input_index: int,
) -> list[tuple[float, state_type]]:
    """
    Labels every energy level of an energy distribution by whether the
    physically valid charge distributions at that level encode the
    expected output for the given input pattern (`ACCEPTED`) or not
    (`REJECTED`). Kinks in the wires are tolerated: only the output BDL
    pairs are inspected.

    Args:
        energy_dist: The energy distribution of the charge distributions.
        valid_charge_distributions: The physically valid charge
                                    distributions.
        output_bdl_pairs: The output BDL pairs of the layout.
        spec: The Boolean function(s) to implement.
        input_index: The input pattern the charge distributions were
                     simulated for.

    Returns:
        The energies with their state types, ascending by energy.
    """

def calculate_energy_and_state_type_with_kinks_rejected(
    lyt: mnt.pyfiction.sidb.sidb_layout,
    energy_distribution: energy_distribution,
    valid_charge_distributions: Sequence[mnt.pyfiction.sidb.charge_distribution],
    spec: Sequence[mnt.pyfiction.synthesis.dynamic_truth_table],
    input_index: int,
    input_bdl_wires: Sequence[mnt.pyfiction.sidb.simulation.logic.bdl_wire],
    output_bdl_wires: Sequence[mnt.pyfiction.sidb.simulation.logic.bdl_wire],
) -> list[tuple[float, state_type]]:
    """
    Like `calculate_energy_and_state_type_with_kinks_accepted`, but a
    charge distribution with kinks in its wires is `REJECTED` as well:
    every energy level is `ACCEPTED`, and additionally `REJECTED` if any
    of its charge distributions fails the logic match with kinks rejected.

    Args:
        lyt: The layout the charge distributions belong to.
        energy_dist: The energy distribution of the charge distributions.
        valid_charge_distributions: The physically valid charge
                                    distributions.
        spec: The Boolean function(s) to implement.
        input_index: The input pattern the charge distributions were
                     simulated for.
        input_bdl_wires: The input BDL wires of `lyt`.
        output_bdl_wires: The output BDL wires of `lyt`.

    Returns:
        The energies with their state types.

    Raises:
        std::out_of_range: if logic validation encounters an invalid
                           lattice basis index.
    """

def can_positive_charges_occur(
    lyt: mnt.pyfiction.sidb.sidb_layout,
    *,
    base: int = 3,
    mu_minus: float = -0.32,
    epsilon_r: float = 5.6,
    lambda_tf: float = 5.0,
) -> bool:
    """
    Check whether positively charged SiDBs can occur under the scalar physical model.

    Args:
        base: Number of charge states: 2 for negative and neutral, or 3 to include positive charges.
        mu_minus: Negative-charge transition energy in eV.
        epsilon_r: Relative permittivity (dimensionless).
        lambda_tf: Thomas-Fermi screening distance in nm.
    """

class critical_temperature_stats:
    """This struct stores the result of the temperature simulation."""

    def __init__(self) -> None:
        """Default constructor."""

    @property
    def algorithm_name(self) -> str:
        """
        Name of the algorithm used to compute the physically valid charge
        distributions.
        """

    @property
    def num_valid_lyt(self) -> int:
        """Number of physically valid charge configurations."""

    @property
    def energy_between_ground_state_and_first_erroneous(self) -> float:
        """
        Energy difference between the ground state and the first (erroneous)
        excited state (unit: meV).
        """

@overload
def critical_temperature_gate_based(
    lyt: mnt.pyfiction.sidb.sidb_layout,
    spec: Sequence[mnt.pyfiction.synthesis.dynamic_truth_table],
    *,
    timeout: int = ...,
    base: int = 3,
    mu_minus: float = -0.32,
    epsilon_r: float = 5.6,
    lambda_tf: float = 5.0,
    sim_engine: mnt.pyfiction.sidb.simulation.sidb_simulation_engine = ...,
    threshold_bdl_interdistance: float = 2.0,
    minimum_distance: float = 0.75,
    maximum_distance: float = 1.5,
    input_bdl_config: mnt.pyfiction.sidb.simulation.logic.input_bdl_configuration = ...,
    op_condition: mnt.pyfiction.sidb.simulation.logic.operational_condition = ...,
    strategy_to_analyze_operational_status: mnt.pyfiction.sidb.simulation.logic.operational_analysis_strategy = ...,
    confidence_level: float = 0.99,
    max_temperature: float = 400.0,
    on_progress: Callable[[str, int, int], None] | None = None,
    on_worker_progress: Callable[[int, int, str, int, int, bool], None] | None = None,
    stats: critical_temperature_stats | None = None,
) -> float:
    """
    Determine the critical temperature of an SiDB gate implementing the truth tables.
    The keyword options select physical values, operational analysis, confidence, and temperature bounds.
    The overload with input and output wires bypasses automatic wire detection.

    Args:
        timeout: Execution budget in milliseconds.
        base: Number of charge states: 2 for negative and neutral, or 3 to include positive charges.
        mu_minus: Negative-charge transition energy in eV.
        epsilon_r: Relative permittivity (dimensionless).
        lambda_tf: Thomas-Fermi screening distance in nm.
        threshold_bdl_interdistance: Maximum separation between BDL pairs in one wire in nm.
        minimum_distance: Minimum SiDB separation for a BDL pair in nm.
        maximum_distance: Maximum SiDB separation for a BDL pair in nm.
        input_bdl_config: Input encoding: perturber absence or perturber distance.
        confidence_level: Required probability of a correct gate output, between 0 and 1.
        max_temperature: Maximum temperature examined in K.
        on_progress: Callback receiving (task, completed, total); None disables progress reporting.
        on_worker_progress: Callback receiving (worker, count, description, completed, total, active); None disables worker reports.
        stats: Statistics object filled by the algorithm; None discards statistics.
        sim_engine: SiDB simulation engine used for operational analysis.
        op_condition: Require the selected operational condition for every input pattern.
        strategy_to_analyze_operational_status: Operational-status analysis strategy.
    """

@overload
def critical_temperature_gate_based(
    input_pattern_layouts: Sequence[mnt.pyfiction.sidb.sidb_layout],
    spec: Sequence[mnt.pyfiction.synthesis.dynamic_truth_table],
    output_bdl_pairs: Sequence[mnt.pyfiction.sidb.simulation.logic.bdl_pair],
    input_bdl_wires: Sequence[mnt.pyfiction.sidb.simulation.logic.bdl_wire],
    output_bdl_wires: Sequence[mnt.pyfiction.sidb.simulation.logic.bdl_wire],
    *,
    timeout: int = ...,
    base: int = 3,
    mu_minus: float = -0.32,
    epsilon_r: float = 5.6,
    lambda_tf: float = 5.0,
    sim_engine: mnt.pyfiction.sidb.simulation.sidb_simulation_engine = ...,
    minimum_distance: float = 0.75,
    maximum_distance: float = 1.5,
    input_bdl_config: mnt.pyfiction.sidb.simulation.logic.input_bdl_configuration = ...,
    op_condition: mnt.pyfiction.sidb.simulation.logic.operational_condition = ...,
    strategy_to_analyze_operational_status: mnt.pyfiction.sidb.simulation.logic.operational_analysis_strategy = ...,
    confidence_level: float = 0.99,
    max_temperature: float = 400.0,
    on_progress: Callable[[str, int, int], None] | None = None,
    on_worker_progress: Callable[[int, int, str, int, int, bool], None] | None = None,
    stats: critical_temperature_stats | None = None,
) -> float:
    """
    Determine the critical temperature of an SiDB gate implementing the truth tables.
    The keyword options select physical values, operational analysis, confidence, and temperature bounds.
    The overload with input and output wires bypasses automatic wire detection.

    Args:
        timeout: Execution budget in milliseconds.
        base: Number of charge states: 2 for negative and neutral, or 3 to include positive charges.
        mu_minus: Negative-charge transition energy in eV.
        epsilon_r: Relative permittivity (dimensionless).
        lambda_tf: Thomas-Fermi screening distance in nm.
        minimum_distance: Minimum SiDB separation for a BDL pair in nm.
        maximum_distance: Maximum SiDB separation for a BDL pair in nm.
        input_bdl_config: Input encoding: perturber absence or perturber distance.
        confidence_level: Required probability of a correct gate output, between 0 and 1.
        max_temperature: Maximum temperature examined in K.
        on_progress: Callback receiving (task, completed, total); None disables progress reporting.
        on_worker_progress: Callback receiving (worker, count, description, completed, total, active); None disables worker reports.
        stats: Statistics object filled by the algorithm; None discards statistics.
        sim_engine: SiDB simulation engine used for operational analysis.
        op_condition: Require the selected operational condition for every input pattern.
        strategy_to_analyze_operational_status: Operational-status analysis strategy.
    """

def critical_temperature_non_gate_based(
    lyt: mnt.pyfiction.sidb.sidb_layout,
    *,
    timeout: int = ...,
    base: int = 3,
    mu_minus: float = -0.32,
    epsilon_r: float = 5.6,
    lambda_tf: float = 5.0,
    sim_engine: mnt.pyfiction.sidb.simulation.sidb_simulation_engine = ...,
    confidence_level: float = 0.99,
    max_temperature: float = 400.0,
    on_progress: Callable[[str, int, int], None] | None = None,
    on_worker_progress: Callable[[int, int, str, int, int, bool], None] | None = None,
    stats: critical_temperature_stats | None = None,
) -> float:
    """
    Determine the critical temperature from the population of excited charge distributions.
    The keyword options select physical values, confidence, and temperature bounds.

    Args:
        timeout: Execution budget in milliseconds.
        base: Number of charge states: 2 for negative and neutral, or 3 to include positive charges.
        mu_minus: Negative-charge transition energy in eV.
        epsilon_r: Relative permittivity (dimensionless).
        lambda_tf: Thomas-Fermi screening distance in nm.
        confidence_level: Required ground-state population probability between 0 and 1.
        max_temperature: Maximum temperature examined in K.
        on_progress: Callback receiving (task, completed, total); None disables progress reporting.
        on_worker_progress: Callback receiving (worker, count, description, completed, total, active); None disables worker reports.
        stats: Statistics object filled by the algorithm; None discards statistics.
        sim_engine: SiDB simulation engine used for operational analysis.
    """

class energy_state:
    def __init__(self, electrostatic_potential_energy: float, degeneracy: int) -> None:
        """
        This struct stores the energy state of an SiDB layout. The energy
        state consists of the electrostatic potential energy and the
        degeneracy of the state.
        """

    @property
    def electrostatic_potential_energy(self) -> float:
        """The electrostatic potential energy of the charge distribution (eV)."""

    @electrostatic_potential_energy.setter
    def electrostatic_potential_energy(self, arg: float, /) -> None: ...
    @property
    def degeneracy(self) -> int:
        """The degeneracy of the state."""

    @degeneracy.setter
    def degeneracy(self, arg: int, /) -> None: ...

class energy_distribution:
    def __init__(self) -> None:
        """Default constructor."""

    def get_nth_state(self, state_index: int) -> energy_state | None:
        """
        Returns the nth state (energy + degeneracy) in the energy
        distribution.

        Args:
            state_index: The index of the state to be retrieved.

        Returns:
            Energy state. If the index is out of range, `std::nullopt` is
            returned instead.
        """

    def degeneracy(self, energy: float) -> int | None:
        """
        Returns the degeneracy value (number of states) with the given energy
        value.

        Args:
            energy: The energy value whose degeneracy is requested.

        Returns:
            The degeneracy of the given energy. If the energy value is not
            found, `std::nullopt` is returned instead.
        """

    def add_energy_state(self, state: energy_state) -> None:
        """
        Adds a state to the energy distribution.

        Args:
            state: The energy state to be added.
        """

    def size(self) -> int:
        """
        Returns the number of energy states in the energy distribution.

        Returns:
            The number of energy states in the energy distribution.
        """

    def empty(self) -> bool:
        """
        Checks if the energy distribution is empty.

        Returns:
            `true` if the energy distribution is empty, `false` otherwise.
        """

    def max_energy(self) -> float:
        """
        Returns the maximum energy value in the energy distribution.

        Returns:
            The maximum energy value in the energy distribution.
        """

    def min_energy(self) -> float:
        """
        Returns the minimum energy value in the energy distribution.

        Returns:
            The minimum energy value in the energy distribution.
        """

def calculate_energy_distribution(
    charge_distributions: Sequence[mnt.pyfiction.sidb.charge_distribution],
) -> energy_distribution:
    """
    The energy distribution of a set of charge distributions: every
    distinct energy with the number of distinct charge distributions that
    have it. Distributions with identical charge states count once.

    Args:
        charge_distributions: Charge distributions of one layout.

    Returns:
        The energy distribution.
    """

def minimum_energy(charge_distributions: Sequence[mnt.pyfiction.sidb.charge_distribution]) -> float:
    """
    Computes the minimum energy of a range of charge distributions. If the
    range is empty, infinity is returned to indicate no valid energy value
    exists.

    Args:
        first: Begin of the range to examine.
        last: End of the range to examine.

    Template Args:
        InputIt: Must meet the requirements of `LegacyInputIterator`.

    Returns:
        Value of the minimum energy found in the input range (unit: eV),
        or `std::numeric_limits<double>::infinity()` if the range is
        empty.
    """

def occupation_probability_gate_based(
    energy_and_state_type: Sequence[tuple[float, state_type]], temperature: float
) -> float:
    """
    This function computes the occupation probability of erroneous charge
    distributions (output charge does not match the expected output
    according the truth table) at a given temperature.

    Args:
        est: This contains the energies of all possible charge
             distributions together with the information if the charge
             distribution (state) is transparent or erroneous.
        temperature: System temperature to assume (unit: K).

    Returns:
        The occupation probability of all erroneous states is returned.
    """

def occupation_probability_non_gate_based(
    calculate_energy_distribution: energy_distribution, temperature: float
) -> float: ...

class transition_type(enum.Enum):
    """
    Possible types of charge transitions that can occur in an SiDB layout.
    These transitions represent changes in the charge state of SiDBs,
    including transitions from neutral to negative, negative to neutral,
    neutral to positive, and positive to neutral.
    """

    NEUTRAL_TO_NEGATIVE = 0
    """SiDB is neutrally charged, but is closest to being negatively charged."""

    NEGATIVE_TO_NEUTRAL = 1
    """SiDB is negatively charged, but is closest to being neutrally charged."""

    NEUTRAL_TO_POSITIVE = 2
    """SiDB is neutrally charged, but is closest to being positively charged."""

    POSITIVE_TO_NEUTRAL = 3
    """SiDB is positively charged, but is closest to being neutrally charged."""

class population_stability_information:
    """
    Population stability of one physically valid charge distribution: for
    every transition type, the SiDB that is closest to that transition and
    the potential change it would take, plus the distance an SiDB would
    have to be placed at to induce that potential change.
    """

    def __init__(self) -> None:
        """Default constructor."""

    @property
    def critical_dot(self) -> mnt.pyfiction.sidb.lattice_site:
        """
        SiDB with the minimum potential difference to any charge transition
        threshold.
        """

    @critical_dot.setter
    def critical_dot(self, arg: mnt.pyfiction.sidb.lattice_site, /) -> None: ...
    @property
    def transition_potentials(self) -> dict[transition_type, tuple[mnt.pyfiction.sidb.lattice_site, float]]:
        """
        For each transition type, the SiDB closest to it and the required
        potential difference (unit: V).
        """

    @transition_potentials.setter
    def transition_potentials(
        self, arg: Mapping[transition_type, tuple[mnt.pyfiction.sidb.lattice_site, float]], /
    ) -> None: ...
    @property
    def distance_corresponding_to_potential(self) -> dict[transition_type, float]:
        """
        For each transition type, the distance (unit: nm) an additional SiDB
        would have to be placed at to induce the required potential
        difference.
        """

    @distance_corresponding_to_potential.setter
    def distance_corresponding_to_potential(self, arg: Mapping[transition_type, float], /) -> None: ...
    @property
    def system_energy(self) -> float:
        """Electrostatic potential energy of the charge distribution (unit: eV)."""

    @system_energy.setter
    def system_energy(self, arg: float, /) -> None: ...

def physical_population_stability(
    lyt: mnt.pyfiction.sidb.sidb_layout,
    *,
    timeout: int = ...,
    base: int = 3,
    mu_minus: float = -0.32,
    epsilon_r: float = 5.6,
    lambda_tf: float = 5.0,
    precision_for_distance_corresponding_to_potential: int = 2,
) -> list[population_stability_information]:
    """
    Determine the stability of the SiDB population under the physical model.
    The keyword options select simulation, potential precision, and the execution budget.
    Raises ValueError for an invalid base or a precision yielding a zero or non-finite distance step.
    Large precisions can make the native distance scan impractical or stall floating-point increments.

    Args:
        timeout: Execution budget in milliseconds.
        base: Number of charge states: 2 for negative and neutral, or 3 to include positive charges.
        mu_minus: Negative-charge transition energy in eV.
        epsilon_r: Relative permittivity (dimensionless).
        lambda_tf: Thomas-Fermi screening distance in nm.
        precision_for_distance_corresponding_to_potential: Decimal places for the distance scan; runtime grows exponentially with precision.
    """

class physically_valid_parameters_domain:
    def __init__(self) -> None:
        """Default constructor."""

    def get_excited_state_number_for_parameter(
        self, pp: mnt.pyfiction.sidb.simulation.logic.parameter_point
    ) -> int: ...

def physically_valid_parameters(
    lyt: mnt.pyfiction.sidb.sidb_layout,
    cd: mnt.pyfiction.sidb.charge_distribution,
    *,
    timeout: int = ...,
    base: int = 3,
    mu_minus: float = -0.32,
    epsilon_r: float = 5.6,
    lambda_tf: float = 5.0,
    sim_engine: mnt.pyfiction.sidb.simulation.sidb_simulation_engine = ...,
    threshold_bdl_interdistance: float = 2.0,
    minimum_distance: float = 0.75,
    maximum_distance: float = 1.5,
    input_bdl_config: mnt.pyfiction.sidb.simulation.logic.input_bdl_configuration = ...,
    op_condition: mnt.pyfiction.sidb.simulation.logic.operational_condition = ...,
    strategy_to_analyze_operational_status: mnt.pyfiction.sidb.simulation.logic.operational_analysis_strategy = ...,
    sweep_dimensions: Sequence[mnt.pyfiction.sidb.simulation.logic.operational_domain_value_range] | None = None,
    number_of_threads: int | None = None,
    on_progress: Callable[[str, int, int], None] | None = None,
    on_worker_progress: Callable[[int, int, str, int, int, bool], None] | None = None,
) -> physically_valid_parameters_domain:
    """
    Sweep physical parameter values for which the supplied charge distribution is physically valid.
    The keyword options define the sweep, physical model, worker count, and progress callbacks.

    Args:
        timeout: Execution budget in milliseconds.
        base: Number of charge states: 2 for negative and neutral, or 3 to include positive charges.
        mu_minus: Negative-charge transition energy in eV.
        epsilon_r: Relative permittivity (dimensionless).
        lambda_tf: Thomas-Fermi screening distance in nm.
        threshold_bdl_interdistance: Maximum separation between BDL pairs in one wire in nm.
        minimum_distance: Minimum SiDB separation for a BDL pair in nm.
        maximum_distance: Maximum SiDB separation for a BDL pair in nm.
        input_bdl_config: Input encoding: perturber absence or perturber distance.
        sweep_dimensions: Ordered parameter ranges; None sweeps relative permittivity and screening distance with the native ranges.
        number_of_threads: Number of workers; None uses the native default.
        on_progress: Callback receiving (task, completed, total); None disables progress reporting.
        on_worker_progress: Callback receiving (worker, count, description, completed, total, active); None disables worker reports.
        sim_engine: SiDB simulation engine used for operational analysis.
        op_condition: Require the selected operational condition for every input pattern.
        strategy_to_analyze_operational_status: Operational-status analysis strategy.
    """

class time_to_solution_stats:
    """
    This struct stores the time-to-solution, the simulation accuracy and
    the average single simulation runtime of *QuickSim*, the single
    runtime of the exact simulator used, and the number of valid charge
    configurations found by the exact algorithm.
    """

    def __init__(self) -> None:
        """Default constructor."""

    @property
    def time_to_solution(self) -> float:
        """Time-to-solution in seconds."""

    @property
    def acc(self) -> float:
        """Accuracy of the simulation in %."""

    @property
    def mean_single_runtime(self) -> float:
        """
        Average runtime of all heuristic attempts in seconds, including failed
        attempts.
        """

    @property
    def single_runtime_exact(self) -> float:
        """
        Single simulation runtime of the exact ground state simulation
        algorithm.
        """

    @property
    def algorithm(self) -> str:
        """
        Exact simulation algorithm used to simulate the ground state as
        reference.
        """

def time_to_solution(
    lyt: mnt.pyfiction.sidb.sidb_layout,
    *,
    base: int = 3,
    mu_minus: float = -0.32,
    epsilon_r: float = 5.6,
    lambda_tf: float = 5.0,
    iteration_steps: int = 80,
    alpha: float = 0.7,
    number_threads: int | None = None,
    quicksim_timeout: int = ...,
    quicksim_on_progress: Callable[[str, int, int], None] | None = None,
    on_worker_progress: Callable[[int, int, str, int, int, bool], None] | None = None,
    timeout: int = ...,
    engine: mnt.pyfiction.sidb.simulation.exact_sidb_simulation_engine = ...,
    repetitions: int = 100,
    confidence_level: float = 0.997,
    on_progress: Callable[[str, int, int], None] | None = None,
    ps: time_to_solution_stats | None = None,
) -> None:
    """
    Compute QuickSim time-to-solution from one exact run and repetitions heuristic runs.
    quicksim_timeout bounds each heuristic run; timeout bounds the complete evaluation.
    Failed attempts contribute elapsed wall time, while successful attempts retain the engine runtime.
    ps receives completed statistics; expiration of the complete evaluation raises TimeoutError.
    Raises ValueError when ClusterComplete is selected with a finite timeout.

    Args:
        base: Number of charge states: 2 for negative and neutral, or 3 to include positive charges.
        mu_minus: Negative-charge transition energy in eV.
        epsilon_r: Relative permittivity (dimensionless).
        lambda_tf: Thomas-Fermi screening distance in nm.
        number_threads: Number of QuickSim workers; None uses the available hardware threads.
        quicksim_timeout: Execution budget for each heuristic run in milliseconds.
        quicksim_on_progress: Progress callback for heuristic iterations; None disables these reports.
        on_worker_progress: Callback receiving (worker, count, description, completed, total, active); None disables worker reports.
        timeout: Execution budget in milliseconds.
        engine: Exhaustive simulation algorithm used to simulate the ground state as reference.
        repetitions: Number of iterations of the heuristic algorithm used to determine the simulation accuracy (`repetitions = 100` means that accuracy is precise to 1 %).
        confidence_level: Probability of finding the ground state used to compute time-to-solution, between 0 and 1.
        on_progress: Callback receiving (task, completed, total); None disables progress reporting.
        ps: Statistics object filled by the algorithm; None discards statistics.
        iteration_steps: Number of QuickSim iterations.
        alpha: QuickSim heuristic alpha parameter.
    """

def time_to_solution_for_given_simulation_results(
    results_exact: mnt.pyfiction.sidb.simulation.sidb_simulation_result,
    results_heuristic: Sequence[mnt.pyfiction.sidb.simulation.sidb_simulation_result],
    confidence_level: float = 0.997,
    ps: time_to_solution_stats | None = None,
) -> None:
    """
    Computes the time-to-solution (TTS) of *QuickSim* from an exact result
    and a series of heuristic results: the accuracy is the share of
    heuristic runs that found the ground state, and TTS is the mean
    heuristic runtime scaled to reach the given confidence level, `t ·
    log(1 - c) / log(1 - acc)`. An empty sample yields zero accuracy, zero
    mean heuristic runtime, and infinite time-to-solution.

    Args:
        results_exact: The result of an exact engine, which provides the
                       ground state.
        results_heuristic: The results of the heuristic runs.
        confidence_level: Confidence level for the TTS computation.
        ps: Statistics.
    """
