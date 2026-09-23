# SiDB Simulation

```{toctree}
:maxdepth: 1

analysis/analysis
defects/defects
engines/engines
io/io
logic/logic
```

(sidb_timeouts)=

## Timeouts

SiDB timeouts use milliseconds: `2**64 - 1` (default) is unlimited; `0` expires immediately.
Nested calls and workers share the enclosing deadline without extending it.

| Application | Budget in its parameters |
| --- | --- |
| Gate design | `operational_params.timeout` |
| Circuit design, including defect-aware placement and routing | `timeout` |
| Operational checks and input-pattern analysis | `timeout` |
| Operational and critical-temperature domains, physical-validity sweeps | `operational_params.timeout` |
| Critical temperature, defect influence, displacement robustness and fabrication probability | `operational_params.timeout` |
| Operational-domain ratio | `op_domain_params.operational_params.timeout` |
| Population stability and time-to-solution | `timeout` |
| Band-bending resilience | `assess_population_stability_params.timeout` |

Expiration throws `fiction::utils::timeout_error` in C++ or `TimeoutError` in Python,
without publishing partial results or statistics. A completed result is returned even if the
last operation or callback overruns the deadline. Circuit design aborts on a gate timeout;
only a completed, unsuccessful gate search triggers defect-aware placement-and-routing retries.

Finite budgets support QuickExact, ExGS, and QuickSim. ClusterComplete rejects them with
`std::invalid_argument` (`ValueError` in Python). QuickSim's `quicksim_params.timeout`
remains a separate per-attempt limit in time-to-solution measurements.

Cancellation is cooperative: individual operations and callbacks can overrun the budget before
the next check, and cleanup callbacks can run afterward. Use a separate process for a hard deadline.

::::{tab-set}
:sync-group: language

:::{tab-item} C++
:sync: cpp

```cpp
fiction::sidb::simulation::logic::operational_domain_params params{};
params.operational_params.timeout = 5'000;
const auto domain = fiction::sidb::simulation::logic::operational_domain_grid_search(layout, spec, params);
```

:::

:::{tab-item} Python
:sync: python

```python
from mnt import pyfiction as pf

params = pf.operational_domain_params()
params.operational_params.timeout = 5_000
domain = pf.operational_domain_grid_search(layout, spec, params)
```

:::

::::

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
