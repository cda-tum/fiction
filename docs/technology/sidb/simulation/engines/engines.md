# Simulation Engines

(quicksim)=

## Heuristic Ground State Simulation

::::{tab-set}
:sync-group: language

:::{tab-item} C++
:sync: cpp

**Header:** `fiction/technology/sidb/simulation/engines/quicksim.hpp`

```{doxygenstruct} fiction::sidb::simulation::engines::quicksim_params
:members:
```

```{doxygenfunction} fiction::sidb::simulation::engines::quicksim(const layout& lyt, const quicksim_params& ps = quicksim_params{})

```

:::

:::{tab-item} Python
:sync: python

```{eval-rst}
.. autoclass:: mnt.pyfiction.quicksim_params
   :members:

.. autofunction:: mnt.pyfiction.quicksim
```

:::

::::

## Exhaustive Ground State Simulation

::::{tab-set}
:sync-group: language

:::{tab-item} C++
:sync: cpp

(quickexact)=

**Header:** `fiction/technology/sidb/simulation/engines/quickexact.hpp`

```{doxygenstruct} fiction::sidb::simulation::engines::quickexact_params
:members:
```

```{doxygenfunction} fiction::sidb::simulation::engines::quickexact(const layout& lyt, const quickexact_params& params = {})

```

(clustercomplete)=

**Header:** `fiction/technology/sidb/simulation/engines/clustercomplete.hpp`

```{doxygenstruct} fiction::sidb::simulation::engines::clustercomplete_params
:members:
```

```{doxygenfunction} fiction::sidb::simulation::engines::clustercomplete(const layout& lyt, const clustercomplete_params& params = {})

```

**Header:** `fiction/technology/sidb/simulation/engines/exhaustive_ground_state_simulation.hpp`

```{doxygenfunction} fiction::sidb::simulation::engines::exhaustive_ground_state_simulation(const layout& lyt, const model::simulation_parameters& params = model::simulation_parameters{})

```

:::

:::{tab-item} Python
:sync: python

```{eval-rst}
.. autoclass:: mnt.pyfiction.quickexact_params
   :members:

.. autofunction:: mnt.pyfiction.quickexact

.. autoclass:: mnt.pyfiction.clustercomplete_params
   :members:

.. autofunction:: mnt.pyfiction.clustercomplete

.. autofunction:: mnt.pyfiction.exhaustive_ground_state_simulation
```

:::

::::

(ground_state_space)=

## Ground State Space Construction

**Header:** `fiction/technology/sidb/simulation/engines/ground_state_space.hpp`

```{doxygenstruct} fiction::sidb::simulation::engines::ground_state_space_results
:members:
```

```{doxygenfunction} fiction::sidb::simulation::engines::ground_state_space(const potential_landscape& land, const ground_state_space_params& params = {})
```

```{doxygenfunction} fiction::sidb::simulation::engines::ground_state_space(const layout& lyt, const ground_state_space_params& params = {})
```
