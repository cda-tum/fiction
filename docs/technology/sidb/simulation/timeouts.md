(sidb_timeouts)=

# Timeouts

SiDB timeouts use milliseconds: `2**64 - 1` (default) is unlimited; `0` expires immediately.
Nested calls and workers share the enclosing deadline without extending it.

The table names C++ parameter members. Python uses the keyword `timeout` for these bound applications;
circuit design also accepts `design_gate_timeout` for each gate search.

| Application | C++ budget member |
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
`std::invalid_argument` (`ValueError` in Python). QuickSim's `quicksim_params.timeout` in C++ and
`quicksim_timeout` in Python set a separate per-attempt limit in time-to-solution measurements.

Cancellation is cooperative: individual operations and callbacks can overrun the budget before
the next check, and cleanup callbacks can run afterward. Use a separate process for a hard deadline.

::::{tab-set}
:sync-group: language

:::{tab-item} C++
:sync: cpp

```cpp
const fiction::sidb::simulation::logic::operational_domain_params params{
    .operational_params = {.timeout = 5'000}};
const auto domain = fiction::sidb::simulation::logic::operational_domain_grid_search(layout, spec, params);
```

:::

:::{tab-item} Python
:sync: python

```python
from mnt.pyfiction.sidb.simulation.logic import operational_domain_grid_search

domain = operational_domain_grid_search(layout, spec, timeout=5_000)
```

:::

::::
