# Simulation Analysis

## Energy Calculation

::::{tab-set}
:sync-group: language

:::{tab-item} C++
:sync: cpp

**Header:** `fiction/technology/sidb/simulation/analysis/energy_distribution.hpp`

```{doxygenstruct} fiction::sidb::simulation::analysis::energy_state
:members:
```

```{doxygenclass} fiction::sidb::simulation::analysis::energy_distribution
:members:
```

```{doxygenfunction} fiction::sidb::simulation::analysis::calculate_energy_distribution(const std::vector<charge_distribution>& charge_distributions)

```

**Header:** `fiction/technology/sidb/simulation/analysis/minimum_energy.hpp`

```{doxygenfunction} fiction::sidb::simulation::analysis::minimum_energy

```

```{doxygenfunction} fiction::sidb::simulation::analysis::minimum_energy_distribution

```

:::

:::{tab-item} Python
:sync: python

```{eval-rst}
.. autofunction:: mnt.pyfiction.calculate_energy_distribution

.. autofunction:: mnt.pyfiction.minimum_energy
```

:::

::::

(critical_temperature)=

## Temperature Behavior

::::{tab-set}
:sync-group: language

:::{tab-item} C++
:sync: cpp

**Header:** `fiction/technology/sidb/simulation/analysis/critical_temperature.hpp`

```{doxygenstruct} fiction::sidb::simulation::analysis::critical_temperature_params
:members:
```

```{doxygenfunction} fiction::sidb::simulation::analysis::critical_temperature_gate_based(const Lyt& lyt, const std::vector<TT>& spec, const critical_temperature_params& params = {}, critical_temperature_stats* pst = nullptr)

```

```{doxygenfunction} fiction::sidb::simulation::analysis::critical_temperature_gate_based(const std::vector<Lyt>& input_pattern_layouts, const std::vector<TT>& spec, const critical_temperature_params& params, const std::vector<sidb::simulation::logic::bdl_pair<cell<Lyt>>>& output_bdl_pairs, const std::vector<sidb::simulation::logic::bdl_wire<Lyt>>& input_bdl_wires, const std::vector<sidb::simulation::logic::bdl_wire<Lyt>>& output_bdl_wires, critical_temperature_stats* pst = nullptr)

```

```{doxygenfunction} fiction::sidb::simulation::analysis::critical_temperature_non_gate_based

```

**Header:** `fiction/technology/sidb/simulation/analysis/occupation_probability_of_excited_states.hpp`

```{doxygenfunction} fiction::sidb::simulation::analysis::calculate_boltzmann_factor

```

```{doxygenfunction} fiction::sidb::simulation::analysis::occupation_probability_gate_based

```

```{doxygenfunction} fiction::sidb::simulation::analysis::occupation_probability_non_gate_based

```

**Header:** `fiction/technology/sidb/simulation/analysis/calculate_energy_and_state_type.hpp`

```{doxygenenum} fiction::sidb::simulation::analysis::state_type

```

```{doxygentypedef} fiction::sidb::simulation::analysis::energy_and_state_type

```

```{doxygenfunction} fiction::sidb::simulation::analysis::calculate_energy_and_state_type_with_kinks_accepted

```

```{doxygenfunction} fiction::sidb::simulation::analysis::calculate_energy_and_state_type_with_kinks_rejected

```

:::

:::{tab-item} Python
:sync: python

```{eval-rst}
.. autoclass:: mnt.pyfiction.critical_temperature_stats
   :members:

.. autoclass:: mnt.pyfiction.critical_temperature_params
   :members:

.. autofunction:: mnt.pyfiction.critical_temperature_gate_based

.. autofunction:: mnt.pyfiction.critical_temperature_non_gate_based

.. autofunction:: mnt.pyfiction.occupation_probability_gate_based

.. autofunction:: mnt.pyfiction.occupation_probability_non_gate_based

.. autoclass:: mnt.pyfiction.state_type
   :members:

.. autofunction:: mnt.pyfiction.calculate_energy_and_state_type_with_kinks_accepted

.. autofunction:: mnt.pyfiction.calculate_energy_and_state_type_with_kinks_rejected
```

:::

::::

## Time-to-Solution (TTS) Statistics

::::{tab-set}
:sync-group: language

:::{tab-item} C++
:sync: cpp

**Header:** `fiction/technology/sidb/simulation/analysis/time_to_solution.hpp`

```{doxygenstruct} fiction::sidb::simulation::analysis::time_to_solution_params
:members:
```

```{doxygenstruct} fiction::sidb::simulation::analysis::time_to_solution_stats
:members:
```

```{doxygenfunction} fiction::sidb::simulation::analysis::time_to_solution

```

```{doxygenfunction} fiction::sidb::simulation::analysis::time_to_solution_for_given_simulation_results

```

:::

:::{tab-item} Python
:sync: python

```{eval-rst}
.. autoclass:: mnt.pyfiction.time_to_solution_params
   :members:

.. autoclass:: mnt.pyfiction.time_to_solution_stats
   :members:

.. autofunction:: mnt.pyfiction.time_to_solution

.. autofunction:: mnt.pyfiction.time_to_solution_for_given_simulation_results
```

:::

::::

## Physically Valid Parameters

::::{tab-set}
:sync-group: language

:::{tab-item} C++
:sync: cpp

**Header:** `fiction/technology/sidb/simulation/analysis/physically_valid_parameters.hpp`

```{doxygenfunction} fiction::sidb::simulation::analysis::physically_valid_parameters

```

:::

:::{tab-item} Python
:sync: python

```{eval-rst}
.. autoclass:: mnt.pyfiction.physically_valid_parameters_domain
   :members:

.. autoclass:: mnt.pyfiction.physically_valid_parameters
```

:::

::::

## Charge Detection

::::{tab-set}
:sync-group: language

:::{tab-item} C++
:sync: cpp

**Header:** `fiction/technology/sidb/simulation/analysis/can_positive_charges_occur.hpp`

```{doxygenfunction} fiction::sidb::simulation::analysis::can_positive_charges_occur

```

:::

:::{tab-item} Python
:sync: python

```{eval-rst}
.. autofunction:: mnt.pyfiction.can_positive_charges_occur
```

:::

::::

## Assess Population Stability

::::{tab-set}
:sync-group: language

:::{tab-item} C++
:sync: cpp

**Header:** `fiction/technology/sidb/simulation/analysis/physical_population_stability.hpp`

```{doxygenenum} fiction::sidb::simulation::analysis::transition_type

```

```{doxygenstruct} fiction::sidb::simulation::analysis::population_stability_information
:members:
```

```{doxygenstruct} fiction::sidb::simulation::analysis::physical_population_stability_params
:members:
```

```{doxygenfunction} fiction::sidb::simulation::analysis::physical_population_stability

```

:::

:::{tab-item} Python
:sync: python

```{eval-rst}
.. autoclass:: mnt.pyfiction.transition_type
   :members:

.. autoclass:: mnt.pyfiction.population_stability_information_100
   :members:

.. autoclass:: mnt.pyfiction.population_stability_information_111
   :members:

.. autoclass:: mnt.pyfiction.physical_population_stability_params
   :members:

.. autofunction:: mnt.pyfiction.physical_population_stability_100

.. autofunction:: mnt.pyfiction.physical_population_stability_111
```

:::

::::

(band_bending_resilience)=
(band-bending-resilience-1)=

## Band-Bending Resilience

**Header:** `fiction/technology/sidb/simulation/analysis/band_bending_resilience.hpp`

```{doxygenstruct} fiction::sidb::simulation::analysis::band_bending_resilience_params
:members:
```

```{doxygenfunction} fiction::sidb::simulation::analysis::band_bending_resilience

```
