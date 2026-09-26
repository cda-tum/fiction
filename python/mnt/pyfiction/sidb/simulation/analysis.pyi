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
import mnt.pyfiction.sidb.model
import mnt.pyfiction.sidb.simulation
import mnt.pyfiction.sidb.simulation.engines
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
    lyt: mnt.pyfiction.sidb.sidb_layout, sim_params: mnt.pyfiction.sidb.model.sidb_simulation_parameters
) -> bool:
    """
    Checks whether positively charged SiDBs can occur in a layout under
    the given physical parameters. See the `potential_landscape` overload.

    Args:
        lyt: The layout to check.
        sim_params: The physical parameters.

    Returns:
        `true` if at least one SiDB can be positively charged, `false`
        otherwise.

    Raises:
        std::out_of_range: if a SiDB or defect site has an invalid lattice
                           basis index.
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

class critical_temperature_params:
    """
    This struct stores the parameters for the *Critical Temperature*
    algorithm.
    """

    def __init__(self) -> None:
        """Default constructor."""

    @property
    def operational_params(self) -> mnt.pyfiction.sidb.simulation.logic.is_operational_params:
        """
        This struct stores the parameters for the *Critical Temperature*
        algorithm.
        """

    @operational_params.setter
    def operational_params(self, arg: mnt.pyfiction.sidb.simulation.logic.is_operational_params, /) -> None: ...
    @property
    def confidence_level(self) -> float:
        """
        Probability threshold for ground state population. The temperature at
        which the simulation finds the ground state to be populated with a
        probability of less than the given percentage, is determined to be the
        critical temperature. For gate-based simulation, this is the
        probability of erroneous calculations of the gate.
        """

    @confidence_level.setter
    def confidence_level(self, arg: float, /) -> None: ...
    @property
    def max_temperature(self) -> float:
        """
        Maximum simulation temperature beyond which no simulation will be
        conducted (~ 126 °C by default) (unit: K).
        """

    @max_temperature.setter
    def max_temperature(self, arg: float, /) -> None: ...
    @property
    def on_progress(self, /) -> Callable[[str, int, int], None] | None:
        """
        Callback that receives the number of simulated input patterns (gate-
        based) or the progress of the physical simulation (non-gate-based).
        """

    @on_progress.setter
    def on_progress(self, value: Callable[[str, int, int], None] | None) -> None: ...
    @property
    def on_worker_progress(self, /) -> Callable[[int, int, str, int, int, bool], None] | None:
        """
        Reports logical worker activity with a fixed worker count for each
        invocation.
        """

    @on_worker_progress.setter
    def on_worker_progress(self, value: Callable[[int, int, str, int, int, bool], None] | None) -> None: ...

@overload
def critical_temperature_gate_based(
    lyt: mnt.pyfiction.sidb.sidb_layout,
    spec: Sequence[mnt.pyfiction.synthesis.dynamic_truth_table],
    params: critical_temperature_params = ...,
    stats: critical_temperature_stats | None = None,
) -> float:
    """
    This algorithm performs temperature-aware SiDB simulation as proposed
    in \\"Temperature Behavior of Silicon Dangling Bond Logic\\" by J.
    Drewniok, M. Walter, and R. Wille in IEEE NANO 2023
    (https://ieeexplore.ieee.org/document/10231259). It comes in two
    flavors: gate-based and non-gate based.

    For *Gate-based Critical Temperature* Simulation, the Critical
    Temperature is defined as follows: The temperature at which the
    erroneous charge distributions are populated by more than :math:`1 -
    \\eta`, where :math:`\\eta \\in [0,1]`.

    Args:
        lyt: The layout to simulate.
        spec: Expected Boolean function of the layout given as a multi-
              output truth table.
        params: Simulation and physical parameters.
        pst: Statistics.

    Returns:
        The critical temperature (unit: K).
    """

@overload
def critical_temperature_gate_based(
    input_pattern_layouts: Sequence[mnt.pyfiction.sidb.sidb_layout],
    spec: Sequence[mnt.pyfiction.synthesis.dynamic_truth_table],
    params: critical_temperature_params,
    output_bdl_pairs: Sequence[mnt.pyfiction.sidb.simulation.logic.bdl_pair],
    input_bdl_wires: Sequence[mnt.pyfiction.sidb.simulation.logic.bdl_wire],
    output_bdl_wires: Sequence[mnt.pyfiction.sidb.simulation.logic.bdl_wire],
    stats: critical_temperature_stats | None = None,
) -> float:
    """
    *Gate-based Critical Temperature* simulation of an SiDB layout from
    its pre-generated input pattern layouts.

    This overload takes one layout per input pattern together with the BDL
    detection results, instead of deriving all of them from the layout.
    Since neither the input configuration nor the BDL pairs and wires
    depend on the simulation parameters, a caller that simulates the same
    layout under many parameter settings can determine them once with
    `generate_bdl_input_pattern_layouts`, `detect_bdl_pairs` and
    `detect_bdl_wires` and pass them to every call, instead of re-deriving
    them each time.

    The layouts and the detection results are only read and may be shared
    by concurrent calls.

    Args:
        input_pattern_layouts: One layout per input pattern, indexed by
                               input pattern, as generated by
                               `generate_bdl_input_pattern_layouts`.
        spec: Expected Boolean function of the layout given as a multi-
              output truth table.
        params: Simulation and physical parameters.
        output_bdl_pairs: Output BDL pairs of the layout.
        input_bdl_wires: BDL input wires of the layout.
        output_bdl_wires: BDL output wires of the layout.
        pst: Statistics.

    Returns:
        The critical temperature (unit: K).

    Raises:
        std::invalid_argument: if `spec` is empty, if the number of input
                               pattern layouts does not match the number
                               of input combinations of `spec`, or if the
                               number of output BDL pairs does not match
                               the number of truth tables.
    """

def critical_temperature_non_gate_based(
    lyt: mnt.pyfiction.sidb.sidb_layout,
    params: critical_temperature_params = ...,
    stats: critical_temperature_stats | None = None,
) -> float:
    """
    For *Non-gate-based Critical Temperature* simulation, the Critical
    Temperature is defined as follows: The temperature at which the
    excited charge distributions are populated by more than :math:`1 -
    \\eta`, where :math:`\\eta \\in [0,1]` is the confidence level for the
    presence of a working gate.

    Args:
        lyt: The layout to simulate.
        params: Simulation and physical parameters.
        pst: Statistics.

    Returns:
        The critical temperature (unit: K)
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
            energy: The energy value for which the excited state number is to
                    be determined.

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

class physical_population_stability_params:
    """Parameters of the population stability analysis."""

    def __init__(self) -> None:
        """Default constructor."""

    @property
    def timeout(self) -> int:
        """
        Millisecond budget for simulation and population analysis. The maximum
        value means unlimited; zero expires immediately. Expiration throws
        `utils::timeout_error` without returning a partial result.
        """

    @timeout.setter
    def timeout(self, arg: int, /) -> None: ...
    @property
    def simulation_parameters(self) -> mnt.pyfiction.sidb.model.sidb_simulation_parameters:
        """Parameters of the population stability analysis."""

    @simulation_parameters.setter
    def simulation_parameters(self, arg: mnt.pyfiction.sidb.model.sidb_simulation_parameters, /) -> None: ...
    @property
    def precision_for_distance_corresponding_to_potential(self) -> int:
        """
        Number of decimal places of the distance corresponding to a potential
        difference.
        """

    @precision_for_distance_corresponding_to_potential.setter
    def precision_for_distance_corresponding_to_potential(self, arg: int, /) -> None: ...

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
    lyt: mnt.pyfiction.sidb.sidb_layout, params: physical_population_stability_params = ...
) -> list[population_stability_information]:
    """
    Assesses the population stability of an SiDB layout: for every
    physically valid charge distribution (found with *QuickExact*), it
    determines the SiDB closest to each charge transition and the
    potential change required for it, in ascending order of the
    distributions' energies. The distance an SiDB would have to be placed
    at to cause that potential change is reported as well.

    Args:
        lyt: The layout to analyze.
        params: Parameters.

    Returns:
        The population stability information, one entry per distinct
        charge distribution.
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
    params: mnt.pyfiction.sidb.simulation.logic.operational_domain_params = ...,
) -> physically_valid_parameters_domain:
    """
    Determines the physical parameters under which a given charge
    distribution of a layout is physically valid: every parameter point of
    the sweep dimensions in `params` is checked, and for each one where
    the charge distribution is valid, its excited-state number (the number
    of valid configurations with lower energy under those parameters) is
    recorded.

    Args:
        lyt: The layout.
        cd: The charge distribution whose validity is checked.
        params: Parameters; the sweep dimensions and the engine that
                determines the excited-state number.

    Returns:
        The parameter points where `cd` is physically valid, each with its
        excited-state number; empty if `cd` does not cover the layout's
        SiDBs in raster order.

    Raises:
        std::invalid_argument: if a sweep range has non-finite or reversed
                               bounds, a non-positive or non-finite step,
                               or more points than the storage range
                               permits.
    """

class time_to_solution_params:
    """Parameters for measuring heuristic accuracy and time-to-solution."""

    def __init__(self) -> None:
        """Default constructor."""

    @property
    def timeout(self) -> int:
        """
        Millisecond budget for the reference simulation and all heuristic
        repetitions together. The maximum value means unlimited; zero expires
        immediately. Expiration throws `utils::timeout_error` without
        publishing statistics. Finite budgets reject ClusterComplete;
        QuickSim's separate timeout still applies to each heuristic attempt.
        """

    @timeout.setter
    def timeout(self, arg: int, /) -> None: ...
    @property
    def engine(self) -> mnt.pyfiction.sidb.simulation.exact_sidb_simulation_engine:
        """
        Exhaustive simulation algorithm used to simulate the ground state as
        reference.
        """

    @engine.setter
    def engine(self, arg: mnt.pyfiction.sidb.simulation.exact_sidb_simulation_engine, /) -> None: ...
    @property
    def repetitions(self) -> int:
        """
        Number of iterations of the heuristic algorithm used to determine the
        simulation accuracy (`repetitions = 100` means that accuracy is
        precise to 1 %).
        """

    @repetitions.setter
    def repetitions(self, arg: int, /) -> None: ...
    @property
    def confidence_level(self) -> float:
        """
        The confidence level represents the probability that the confidence
        interval calculated from the simulation contains the true value. For
        example, a 99.7 % (0.997) confidence level means that if the
        simulation were repeated many times, approximately 997 out of 1000 of
        the calculated confidence intervals would contain the true value.
        """

    @confidence_level.setter
    def confidence_level(self, arg: float, /) -> None: ...
    @property
    def on_progress(self, /) -> Callable[[str, int, int], None] | None:
        """Callback that receives the number of completed heuristic repetitions."""

    @on_progress.setter
    def on_progress(self, value: Callable[[str, int, int], None] | None) -> None: ...

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
    quicksim_params: mnt.pyfiction.sidb.simulation.engines.quicksim_params,
    tts_params: time_to_solution_params = ...,
    ps: time_to_solution_stats | None = None,
) -> None:
    """
    Computes the time-to-solution (TTS) of *QuickSim* for a layout: the
    layout is simulated once with the exact engine of the parameters and
    `tts_params.repetitions` times with *QuickSim*, and the runs are
    combined with `time_to_solution_for_given_simulation_results`.

    Failed attempts contribute elapsed wall time; successful attempts
    retain the engine runtime.

    Args:
        lyt: The layout to simulate.
        qs_params: *QuickSim* parameters.
        tts_params: TTS parameters.
        ps: Statistics.
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
