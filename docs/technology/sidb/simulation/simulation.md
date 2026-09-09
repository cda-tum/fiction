# SiDB Simulation

```{toctree}
:maxdepth: 1

analysis/analysis
defects/defects
engines/engines
io/io
logic/logic
```

## Simulation Result

A simulation result holds one copy of the simulated layout and one `charge_distribution` per physically valid charge
configuration, together with the algorithm name, runtime, and physical parameters.

::::{tab-set}
:sync-group: language

:::{tab-item} C++
:sync: cpp

**Header:** `fiction/technology/sidb/simulation/result.hpp`

```{doxygenstruct} fiction::sidb::simulation::result
:members:
```

:::

:::{tab-item} Python
:sync: python

```{eval-rst}
.. autoclass:: mnt.pyfiction.sidb_simulation_result
   :members:
```

:::

::::

## Potential Landscape

The potential landscape stores the static electrostatics of one layout and parameter set: SiDB pair distances and
potentials, defect and external potentials, and charge transition thresholds. Simulations share the immutable
landscape across worker threads; it evaluates local potentials, energy, and physical validity for a
`charge_distribution`.

::::{tab-set}
:sync-group: language

:::{tab-item} C++
:sync: cpp

**Header:** `fiction/technology/sidb/simulation/potential_landscape.hpp`

```{doxygenenum} fiction::sidb::simulation::charge_transition_threshold_bounds
```

```{doxygenclass} fiction::sidb::simulation::potential_landscape
:members:
```

:::

:::{tab-item} Python
:sync: python

```{eval-rst}
.. autoclass:: mnt.pyfiction.charge_transition_threshold_bounds
   :members:

.. autoclass:: mnt.pyfiction.potential_landscape
   :members:
```

:::

::::

## Engine Selectors

::::{tab-set}
:sync-group: language

:::{tab-item} C++
:sync: cpp

**Header:** `fiction/technology/sidb/simulation/engine.hpp`

```{doxygenenum} fiction::sidb::simulation::engine

```

```{doxygenenum} fiction::sidb::simulation::exact_engine

```

```{doxygenenum} fiction::sidb::simulation::heuristic_engine

```

```{doxygenfunction} fiction::sidb::simulation::engine_name

```

:::

:::{tab-item} Python
:sync: python

```{eval-rst}
.. autoclass:: mnt.pyfiction.sidb_simulation_engine
   :members:

.. autoclass:: mnt.pyfiction.exact_sidb_simulation_engine
   :members:

.. autoclass:: mnt.pyfiction.heuristic_sidb_simulation_engine
   :members:

.. autofunction:: mnt.pyfiction.sidb_simulation_engine_name
```

:::

::::

## Simulation Equivalence Checking

::::{tab-set}
:sync-group: language

:::{tab-item} C++
:sync: cpp

**Header:** `fiction/technology/sidb/simulation/check_simulation_results_for_equivalence.hpp`

```{doxygenfunction} fiction::sidb::simulation::check_simulation_results_for_equivalence(result result1, result result2)

```

:::

:::{tab-item} Python
:sync: python

```{eval-rst}
.. autofunction:: mnt.pyfiction.check_simulation_results_for_equivalence
```

:::

::::

## Ground State Check

::::{tab-set}
:sync-group: language

:::{tab-item} C++
:sync: cpp

**Header:** `fiction/technology/sidb/simulation/is_ground_state.hpp`

```{doxygenfunction} fiction::sidb::simulation::is_ground_state(const result& heuristic_results, const result& exact_results)

```

:::

:::{tab-item} Python
:sync: python

```{eval-rst}
.. autofunction:: mnt.pyfiction.is_ground_state
```

:::

::::

## Simulation Domain

**Header:** `fiction/technology/sidb/simulation/domain.hpp`

```{doxygenclass} fiction::sidb::simulation::domain
:members:
```
