(opdom)=

# Operational Domain Computation

::::{tab-set}
:sync-group: language

:::{tab-item} C++
:sync: cpp

**Header:** `fiction/technology/sidb/simulation/logic/operational_domain.hpp`

```{doxygenstruct} fiction::sidb::simulation::logic::parameter_point
:members:
```

```{doxygenenum} fiction::sidb::simulation::logic::sweep_parameter

```

```{doxygenclass} fiction::sidb::simulation::logic::operational_domain
:members:
```

```{doxygenclass} fiction::sidb::simulation::logic::critical_temperature_domain
:members:
```

```{doxygenstruct} fiction::sidb::simulation::logic::operational_domain_value_range
:members:
```

```{doxygenstruct} fiction::sidb::simulation::logic::operational_domain_params
:members:
```

```{doxygenstruct} fiction::sidb::simulation::logic::operational_domain_stats
:members:
```

Four strategies sample the parameter space. Grid search and random sampling place their samples
independently and accept any number of sweep dimensions. Flood fill and contour tracing follow the
shape of the operational region and therefore need a neighborhood to expand over, which requires at
least two dimensions. In two dimensions, contour tracing walks the boundary as a closed curve; in
three or more, where the boundary is a surface, it collects the boundary instead.

Setting `strategy_to_analyze_operational_status` to `FILTER_ONLY` computes the _operational
domain sketch_: each parameter point is classified by filtering alone, without physical simulation.
This is dramatically faster and never rejects a point that is operational, but it does report some
non-operational points as operational. The sketch requires `REJECT_KINKS` and a layout with
`LOGIC` dots, since the filtering steps enumerate the charge configurations of the canvas those
dots define; without either, it is rejected with `std::invalid_argument`.

The sketch combines with any of the four strategies, but pairs best with the exhaustive ones. Grid
search and random sampling place their samples independently of the result, so the sketch simply makes
each of them cheaper. Flood fill and contour tracing instead follow the boundary of the _sketch_
region rather than of the operational region. That is sound, since the former contains the latter, but
it is only useful when the initial random sampling lands inside the region: over a three-dimensional
range, a sample count that suffices in two dimensions frequently misses the region altogether and
returns almost nothing. Where the sketch region does fill the swept range, the frontier never stops
and the flood fill visits every point anyway, at a higher per-point cost than a grid search. Prefer
grid search with the sketch, and raise the sample count substantially before combining the sketch with
flood fill or contour tracing in three dimensions.

```{doxygenfunction} fiction::sidb::simulation::logic::operational_domain_grid_search(const layout& lyt, const std::vector<fiction::synthesis::truth_table>& spec, const operational_domain_params& params = {}, operational_domain_stats* stats = nullptr)

```

```{doxygenfunction} fiction::sidb::simulation::logic::operational_domain_random_sampling(const layout& lyt, const std::vector<fiction::synthesis::truth_table>& spec, const std::size_t samples, const operational_domain_params& params = {}, operational_domain_stats* stats = nullptr)

```

```{doxygenfunction} fiction::sidb::simulation::logic::operational_domain_flood_fill(const layout& lyt, const std::vector<fiction::synthesis::truth_table>& spec, const std::size_t samples, const operational_domain_params& params = {}, operational_domain_stats* stats = nullptr)

```

```{doxygenfunction} fiction::sidb::simulation::logic::operational_domain_contour_tracing(const layout& lyt, const std::vector<fiction::synthesis::truth_table>& spec, const std::size_t samples, const operational_domain_params& params = {}, operational_domain_stats* stats = nullptr)

```

```{doxygenfunction} fiction::sidb::simulation::logic::critical_temperature_domain_grid_search(const layout& lyt, const std::vector<fiction::synthesis::truth_table>& spec, const operational_domain_params& params = {}, operational_domain_stats* stats = nullptr)

```

```{doxygenfunction} fiction::sidb::simulation::logic::critical_temperature_domain_random_sampling(const layout& lyt, const std::vector<fiction::synthesis::truth_table>& spec, const std::size_t samples, const operational_domain_params& params = {}, operational_domain_stats* stats = nullptr)

```

```{doxygenfunction} fiction::sidb::simulation::logic::critical_temperature_domain_flood_fill(const layout& lyt, const std::vector<fiction::synthesis::truth_table>& spec, const std::size_t samples, const operational_domain_params& params = {}, operational_domain_stats* stats = nullptr)

```

```{doxygenfunction} fiction::sidb::simulation::logic::critical_temperature_domain_contour_tracing(const layout& lyt, const std::vector<fiction::synthesis::truth_table>& spec, const std::size_t samples, const operational_domain_params& params = {}, operational_domain_stats* stats = nullptr)

```

:::

:::{tab-item} Python
:sync: python

The same four strategies are available here. Grid search and random sampling accept any number of
sweep dimensions; flood fill and contour tracing follow the shape of the operational region and
therefore need at least two. In two dimensions, contour tracing walks the boundary as a closed
curve; in three or more, where the boundary is a surface, it collects the boundary instead.

Setting `strategy_to_analyze_operational_status` to `FILTER_ONLY` computes the _operational
domain sketch_, which classifies each parameter point by filtering alone instead of by physical
simulation. It requires `REJECT_KINKS` and a layout with `LOGIC` dots; without either, the
call raises `ValueError`. The sketch pairs best with grid search and random sampling — see the
C++ tab for why combining it with flood fill or contour tracing needs a much higher sample count.

```{eval-rst}
.. autoclass:: mnt.pyfiction.sidb.simulation.logic.sweep_parameter
   :members:

.. autoclass:: mnt.pyfiction.sidb.simulation.logic.parameter_point
   :members:

.. autoclass:: mnt.pyfiction.sidb.simulation.logic.operational_domain
   :members:

.. autoclass:: mnt.pyfiction.sidb.simulation.logic.critical_temperature_domain
   :members:

.. autoclass:: mnt.pyfiction.sidb.simulation.logic.operational_domain_value_range
   :members:

.. autoclass:: mnt.pyfiction.sidb.simulation.logic.operational_domain_stats
   :members:

.. autofunction:: mnt.pyfiction.sidb.simulation.logic.operational_domain_grid_search

.. autofunction:: mnt.pyfiction.sidb.simulation.logic.operational_domain_random_sampling

.. autofunction:: mnt.pyfiction.sidb.simulation.logic.operational_domain_flood_fill

.. autofunction:: mnt.pyfiction.sidb.simulation.logic.operational_domain_contour_tracing

.. autofunction:: mnt.pyfiction.sidb.simulation.logic.critical_temperature_domain_grid_search

.. autofunction:: mnt.pyfiction.sidb.simulation.logic.critical_temperature_domain_random_sampling

.. autofunction:: mnt.pyfiction.sidb.simulation.logic.critical_temperature_domain_flood_fill

.. autofunction:: mnt.pyfiction.sidb.simulation.logic.critical_temperature_domain_contour_tracing
```

:::

::::
