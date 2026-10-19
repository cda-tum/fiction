(defect_influence)=

# Defect Influence

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
:members:
```

```{doxygenfunction} fiction::sidb::simulation::defects::defect_influence_grid_search(const layout& lyt, const std::vector<fiction::synthesis::truth_table>& spec, const defect_influence_params& params = {}, const std::size_t step_size = 1, defect_influence_stats* stats = nullptr)

```

```{doxygenfunction} fiction::sidb::simulation::defects::defect_influence_grid_search(const layout& lyt, const defect_influence_params& params = {}, const std::size_t step_size = 1, defect_influence_stats* stats = nullptr)

```

```{doxygenfunction} fiction::sidb::simulation::defects::defect_influence_random_sampling(const layout& lyt, const std::vector<fiction::synthesis::truth_table>& spec, const std::size_t samples, const defect_influence_params& params = {}, defect_influence_stats* stats = nullptr)

```

```{doxygenfunction} fiction::sidb::simulation::defects::defect_influence_random_sampling(const layout& lyt, const std::size_t samples, const defect_influence_params& params = {}, defect_influence_stats* stats = nullptr)

```

```{doxygenfunction} fiction::sidb::simulation::defects::defect_influence_quicktrace(const layout& lyt, const std::vector<fiction::synthesis::truth_table>& spec, const std::size_t samples, const defect_influence_params& params = {}, defect_influence_stats* stats = nullptr)

```

```{doxygenfunction} fiction::sidb::simulation::defects::defect_influence_quicktrace(const layout& lyt, const std::size_t samples, const defect_influence_params& params = {}, defect_influence_stats* stats = nullptr)

```

**Header:** `fiction/technology/sidb/simulation/defects/defect_clearance.hpp`

```{doxygenstruct} fiction::sidb::simulation::defects::defect_clearance
:members:
```

```{doxygenfunction} fiction::sidb::simulation::defects::calculate_defect_clearance

```
