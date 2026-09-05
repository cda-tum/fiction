# Defect Simulation

## Displacement Robustness Domain

::::{tab-set}
:sync-group: language

:::{tab-item} C++
:sync: cpp

**Header:** `fiction/technology/sidb/simulation/defects/displacement_robustness_domain.hpp`

```{doxygenstruct} fiction::sidb::simulation::defects::displacement_robustness_domain
:members:
```

```{doxygenstruct} fiction::sidb::simulation::defects::displacement_robustness_domain_params
:members:
```

```{doxygenstruct} fiction::sidb::simulation::defects::displacement_robustness_domain_stats
:members:
```

```{doxygenfunction} fiction::sidb::simulation::defects::determine_displacement_robustness_domain

```

```{doxygenfunction} fiction::sidb::simulation::defects::determine_probability_of_fabricating_operational_gate

```

:::

:::{tab-item} Python
:sync: python

```{eval-rst}
.. autoclass:: mnt.pyfiction.dimer_displacement_policy
   :members:

.. autoclass:: mnt.pyfiction.displacement_analysis_mode
   :members:

.. autoclass:: mnt.pyfiction.displacement_robustness_domain_params
   :members:

.. autoclass:: mnt.pyfiction.displacement_robustness_domain_stats
   :members:

.. autoclass:: mnt.pyfiction.displacement_robustness_domain
   :members:

.. autofunction:: mnt.pyfiction.determine_displacement_robustness_domain
```

:::

::::

(defect_influence)=

## Defect Influence

**Header:** `fiction/technology/sidb/simulation/defects/defect_influence.hpp`

```{doxygenstruct} fiction::sidb::simulation::defects::defect_influence_params
:members:
```

```{doxygenenum} fiction::sidb::simulation::defects::defect_influence_status

```

```{doxygenclass} fiction::sidb::simulation::defects::defect_influence_domain
:members:
```

```{doxygenstruct} fiction::sidb::simulation::defects::defect_influence_stats

```

```{doxygenfunction} fiction::sidb::simulation::defects::defect_influence_grid_search(const layout& lyt, const std::vector<TT>& spec, const defect_influence_params& params = {}, const std::size_t step_size = 1, defect_influence_stats* stats = nullptr)

```

```{doxygenfunction} fiction::sidb::simulation::defects::defect_influence_grid_search(const layout& lyt, const defect_influence_params& params = {}, const std::size_t step_size = 1, defect_influence_stats* stats = nullptr)

```

```{doxygenfunction} fiction::sidb::simulation::defects::defect_influence_random_sampling(const layout& lyt, const std::vector<TT>& spec, const std::size_t samples, const defect_influence_params& params = {}, defect_influence_stats* stats = nullptr)

```

```{doxygenfunction} fiction::sidb::simulation::defects::defect_influence_random_sampling(const layout& lyt, const std::size_t samples, const defect_influence_params& params = {}, defect_influence_stats* stats = nullptr)

```

```{doxygenfunction} fiction::sidb::simulation::defects::defect_influence_quicktrace(const layout& lyt, const std::vector<TT>& spec, const std::size_t samples, const defect_influence_params& params = {}, defect_influence_stats* stats = nullptr)

```

```{doxygenfunction} fiction::sidb::simulation::defects::defect_influence_quicktrace(const layout& lyt, const std::size_t samples, const defect_influence_params& params = {}, defect_influence_stats* stats = nullptr)

```

**Header:** `fiction/technology/sidb/simulation/defects/defect_clearance.hpp`

```{doxygenstruct} fiction::sidb::simulation::defects::defect_clearance
:members:
```

```{doxygenfunction} fiction::sidb::simulation::defects::calculate_defect_clearance

```
