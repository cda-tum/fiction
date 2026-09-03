# Logic Simulation

## BDL Input Iterator

::::{tab-set}
:sync-group: language

:::{tab-item} C++
:sync: cpp

**Header:** `fiction/technology/sidb/simulation/logic/bdl_input_iterator.hpp`

```{doxygenstruct} fiction::sidb::simulation::logic::bdl_input_iterator_params
:members:
```

```{doxygenclass} fiction::sidb::simulation::logic::bdl_input_iterator
:members:
```

```{doxygenfunction} fiction::sidb::simulation::logic::generate_bdl_input_pattern_layouts(const Lyt& lyt, const bdl_input_iterator_params& ps = {})

```

```{doxygenfunction} fiction::sidb::simulation::logic::generate_bdl_input_pattern_layouts(const Lyt& lyt, const bdl_input_iterator_params& ps, const std::vector<bdl_wire<Lyt>>& input_wires)

```

:::

:::{tab-item} Python
:sync: python

```{autoclass} mnt.pyfiction.input_bdl_configuration
:members:
```

```{autoclass} mnt.pyfiction.bdl_input_iterator_params
:members:
```

```{autoclass} mnt.pyfiction.bdl_input_iterator_100
:members:
```

```{autoclass} mnt.pyfiction.bdl_input_iterator_111
:members:
```

```{autofunction} mnt.pyfiction.generate_bdl_input_pattern_layouts

```

:::

::::

## Operational Domain Computation

(opdom)=

::::{tab-set}
:sync-group: language

:::{tab-item} C++
:sync: cpp

**Header:** `fiction/technology/sidb/simulation/logic/is_operational.hpp`

```{doxygenenum} fiction::sidb::simulation::logic::operational_status

```

```{doxygenstruct} fiction::sidb::simulation::logic::is_operational_params
:members:
```

```{doxygenfunction} fiction::sidb::simulation::logic::is_operational(const Lyt& lyt, const std::vector<TT>& spec, const is_operational_params& params = {})

```

```{doxygenfunction} fiction::sidb::simulation::logic::is_operational(const Lyt& lyt, const std::vector<TT>& spec, const is_operational_params& params, const std::vector<bdl_wire<Lyt>>& input_bdl_wire, const std::vector<bdl_wire<Lyt>>& output_bdl_wire, const std::optional<Lyt>& canvas_lyt = std::nullopt)

```

```{doxygenfunction} fiction::sidb::simulation::logic::is_operational(const std::vector<Lyt>& input_pattern_layouts, const std::vector<TT>& spec, const is_operational_params& params, const std::vector<bdl_wire<Lyt>>& input_bdl_wire, const std::vector<bdl_wire<Lyt>>& output_bdl_wire, const std::optional<Lyt>& canvas_lyt = std::nullopt)

```

```{doxygenfunction} fiction::sidb::simulation::logic::operational_input_patterns(const Lyt& lyt, const std::vector<TT>& spec, const is_operational_params& params = {})

```

```{doxygenfunction} fiction::sidb::simulation::logic::operational_input_patterns(const Lyt& lyt, const std::vector<TT>& spec, const is_operational_params& params, const std::vector<bdl_wire<Lyt>>& input_bdl_wire, const std::vector<bdl_wire<Lyt>>& output_bdl_wire, const std::optional<Lyt>& canvas_lyt = std::nullopt)

```

```{doxygenfunction} fiction::sidb::simulation::logic::is_kink_induced_non_operational(const Lyt& lyt, const std::vector<TT>& spec, const is_operational_params& params = {})

```

```{doxygenfunction} fiction::sidb::simulation::logic::is_kink_induced_non_operational(const Lyt& lyt, const std::vector<TT>& spec, const is_operational_params& params, const std::vector<bdl_wire<Lyt>>& input_bdl_wire, const std::vector<bdl_wire<Lyt>>& output_bdl_wire, const std::optional<Lyt>& canvas_lyt = std::nullopt)

```

```{doxygenfunction} fiction::sidb::simulation::logic::kink_induced_non_operational_input_patterns(const Lyt& lyt, const std::vector<TT>& spec, const is_operational_params& params = {})

```

```{doxygenfunction} fiction::sidb::simulation::logic::kink_induced_non_operational_input_patterns(const Lyt& lyt, const std::vector<TT>& spec, const is_operational_params& params, const std::vector<bdl_wire<Lyt>>& input_bdl_wire, const std::vector<bdl_wire<Lyt>>& output_bdl_wire, const std::optional<Lyt>& canvas_lyt = std::nullopt)

```

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
`LOGIC` cells, since the filtering steps enumerate the charge configurations of the canvas those
cells define; without either, it is rejected with `std::invalid_argument`.

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

```{doxygenfunction} fiction::sidb::simulation::logic::operational_domain_grid_search

```

```{doxygenfunction} fiction::sidb::simulation::logic::operational_domain_random_sampling

```

```{doxygenfunction} fiction::sidb::simulation::logic::operational_domain_flood_fill

```

```{doxygenfunction} fiction::sidb::simulation::logic::operational_domain_contour_tracing

```

```{doxygenfunction} fiction::sidb::simulation::logic::critical_temperature_domain_grid_search

```

```{doxygenfunction} fiction::sidb::simulation::logic::critical_temperature_domain_random_sampling

```

```{doxygenfunction} fiction::sidb::simulation::logic::critical_temperature_domain_flood_fill

```

```{doxygenfunction} fiction::sidb::simulation::logic::critical_temperature_domain_contour_tracing

```

**Header:** `fiction/technology/sidb/simulation/logic/operational_domain_ratio.hpp`

```{doxygenstruct} fiction::sidb::simulation::logic::operational_domain_ratio_params
:members:
```

```{doxygenfunction} fiction::sidb::simulation::logic::operational_domain_ratio

```

**Header:** `fiction/technology/sidb/simulation/logic/verify_logic_match.hpp`

```{doxygenfunction} fiction::sidb::simulation::logic::verify_logic_match

```

:::

:::{tab-item} Python
:sync: python

```{autoclass} mnt.pyfiction.operational_status
:members:
```

```{autoclass} mnt.pyfiction.operational_condition
:members:
```

```{autoclass} mnt.pyfiction.operational_analysis_strategy
:members:
```

```{autoclass} mnt.pyfiction.is_operational_params
:members:
```

```{autofunction} mnt.pyfiction.is_operational

```

```{autofunction} mnt.pyfiction.operational_input_patterns

```

```{autofunction} mnt.pyfiction.is_kink_induced_non_operational

```

```{autofunction} mnt.pyfiction.kink_induced_non_operational_input_patterns

```

```{autoclass} mnt.pyfiction.sweep_parameter
:members:
```

```{autoclass} mnt.pyfiction.parameter_point
:members:
```

```{autoclass} mnt.pyfiction.operational_domain
:members:
```

```{autoclass} mnt.pyfiction.critical_temperature_domain
:members:
```

```{autoclass} mnt.pyfiction.operational_domain_value_range
:members:
```

```{autoclass} mnt.pyfiction.operational_domain_params
:members:
```

```{autoclass} mnt.pyfiction.operational_domain_stats
:members:
```

The same four strategies are available here. Grid search and random sampling accept any number of
sweep dimensions; flood fill and contour tracing follow the shape of the operational region and
therefore need at least two. In two dimensions, contour tracing walks the boundary as a closed
curve; in three or more, where the boundary is a surface, it collects the boundary instead.

Setting `strategy_to_analyze_operational_status` to `FILTER_ONLY` computes the _operational
domain sketch_, which classifies each parameter point by filtering alone instead of by physical
simulation. It requires `REJECT_KINKS` and a layout with `LOGIC` cells; without either, the
call raises `ValueError`. The sketch pairs best with grid search and random sampling — see the
C++ tab for why combining it with flood fill or contour tracing needs a much higher sample count.

```{autofunction} mnt.pyfiction.operational_domain_grid_search

```

```{autofunction} mnt.pyfiction.operational_domain_random_sampling

```

```{autofunction} mnt.pyfiction.operational_domain_flood_fill

```

```{autofunction} mnt.pyfiction.operational_domain_contour_tracing

```

```{autofunction} mnt.pyfiction.critical_temperature_domain_grid_search

```

```{autofunction} mnt.pyfiction.critical_temperature_domain_random_sampling

```

```{autofunction} mnt.pyfiction.critical_temperature_domain_flood_fill

```

```{autofunction} mnt.pyfiction.critical_temperature_domain_contour_tracing

```

```{autoclass} mnt.pyfiction.operational_domain_ratio_params
:members:
```

```{autofunction} mnt.pyfiction.operational_domain_ratio

```

:::

::::

## Binary-dot Logic (BDL) Pair Detection

::::{tab-set}
:sync-group: language

:::{tab-item} C++
:sync: cpp

**Header:** `fiction/technology/sidb/simulation/logic/detect_bdl_pairs.hpp`

```{doxygenstruct} fiction::sidb::simulation::logic::bdl_pair
:members:
```

```{doxygenstruct} fiction::sidb::simulation::logic::detect_bdl_pairs_params
:members:
```

```{doxygenfunction} fiction::sidb::simulation::logic::detect_bdl_pairs

```

:::

:::{tab-item} Python
:sync: python

```{autoclass} mnt.pyfiction.bdl_pair
:members:
```

```{autoclass} mnt.pyfiction.detect_bdl_pairs_params
:members:
```

```{autofunction} mnt.pyfiction.detect_bdl_pairs

```

:::

::::

## Binary-dot Logic (BDL) Wire Detection

::::{tab-set}
:sync-group: language

:::{tab-item} C++
:sync: cpp

**Header:** `fiction/technology/sidb/simulation/logic/detect_bdl_wires.hpp`

```{doxygenenum} fiction::sidb::simulation::logic::bdl_wire_selection

```

```{doxygenstruct} fiction::sidb::simulation::logic::detect_bdl_wires_params
:members:
```

```{doxygenstruct} fiction::sidb::simulation::logic::bdl_wire
:members:
```

```{doxygenfunction} fiction::sidb::simulation::logic::detect_bdl_wires

```

:::

:::{tab-item} Python
:sync: python

```{autoclass} mnt.pyfiction.bdl_wire_selection
:members:
```

```{autoclass} mnt.pyfiction.detect_bdl_wires_params
:members:
```

```{autoclass} mnt.pyfiction.bdl_wire_100
:members:
```

```{autoclass} mnt.pyfiction.bdl_wire_111
:members:
```

```{autofunction} mnt.pyfiction.detect_bdl_wires_100

```

```{autofunction} mnt.pyfiction.detect_bdl_wires_111

```

:::

::::
