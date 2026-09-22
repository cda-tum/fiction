# SiDB Simulation

```{toctree}
:maxdepth: 1

analysis/analysis
defects/defects
engines/engines
io/io
logic/logic
```

## Execution Budgets

Simulation applications accept cooperative millisecond budgets. The default, `2**64 - 1`,
is unlimited; `0` expires immediately. Each calculation starts one deadline and shares it
across its parameter points, input patterns, temperature steps, and worker threads.
Nested simulations cannot restart or extend that deadline.

| Application | Budget in its parameters |
| --- | --- |
| Operational checks and input-pattern analysis | `timeout` |
| Operational and critical-temperature domains, physical-validity sweeps | `operational_params.timeout` |
| Critical temperature, defect influence, displacement robustness and fabrication probability | `operational_params.timeout` |
| Operational-domain ratio | `op_domain_params.operational_params.timeout` |
| Population stability and time-to-solution | `timeout` |
| Band-bending resilience | `assess_population_stability_params.timeout` |

Expiration throws `fiction::utils::timeout_error` in C++ and the built-in `TimeoutError`
from existing Python bindings. No partial result or completed statistics are published.
Progress callbacks may have reported work before cancellation. Algorithms that deliberately
sample only part of a domain retain their normal sampling behavior when they finish.

Finite budgets support QuickExact, ExGS, and QuickSim wherever the application supports
those engines. Selecting ClusterComplete with a finite budget raises `std::invalid_argument`
(`ValueError` in Python). Time-to-solution also retains QuickSim's separate per-attempt
`quicksim_params.timeout`; that setting is not the budget for the whole measurement.

Cancellation is cooperative. Allocations, layout preprocessing, lattice and Cartesian-product
enumeration, sorting, individual post-processing operations, and callbacks are not interruptible.
They may exceed the budget before the next check raises an exception. Final progress callbacks
can also run during cleanup after the last check. Use a separate process when a hard wall-clock
cutoff is required. Small geometry helpers do not take timeout parameters.

::::{tab-set}
:sync-group: language

:::{tab-item} C++
:sync: cpp

```cpp
fiction::sidb::simulation::logic::operational_domain_params params{};
params.operational_params.timeout = 5'000;
try
{
    const auto domain = fiction::sidb::simulation::logic::operational_domain_grid_search(layout, spec, params);
}
catch (const fiction::utils::timeout_error&)
{
    // No domain was returned.
}
```

:::

:::{tab-item} Python
:sync: python

```python
from mnt import pyfiction as pf

params = pf.operational_domain_params()
params.operational_params.timeout = 5_000
try:
    domain = pf.operational_domain_grid_search(layout, spec, params)
except TimeoutError:
    print("The domain calculation exceeded its budget.")
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
