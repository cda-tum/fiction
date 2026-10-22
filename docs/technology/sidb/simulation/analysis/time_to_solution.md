# Time-to-Solution (TTS) Statistics

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

```{doxygenfunction} fiction::sidb::simulation::analysis::time_to_solution(const layout& lyt, const engines::quicksim_params& qs_params, const time_to_solution_params& tts_params = {}, time_to_solution_stats* ps = nullptr)

```

```{doxygenfunction} fiction::sidb::simulation::analysis::time_to_solution_for_given_simulation_results

```

:::

:::{tab-item} Python
:sync: python

```{eval-rst}
.. autoclass:: mnt.pyfiction.sidb.simulation.analysis.time_to_solution_stats
   :members:

.. autofunction:: mnt.pyfiction.sidb.simulation.analysis.time_to_solution

.. autofunction:: mnt.pyfiction.sidb.simulation.analysis.time_to_solution_for_given_simulation_results
```

:::

::::
