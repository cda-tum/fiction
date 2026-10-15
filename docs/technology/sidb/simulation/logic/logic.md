(logic-simulation)=

# Logic

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

```{doxygenfunction} fiction::sidb::simulation::logic::generate_bdl_input_pattern_layouts(const layout& lyt, const bdl_input_iterator_params& ps = {})

```

```{doxygenfunction} fiction::sidb::simulation::logic::generate_bdl_input_pattern_layouts(const layout& lyt, const bdl_input_iterator_params& ps, const std::vector<bdl_wire>& input_wires)

```

:::

:::{tab-item} Python
:sync: python

```{eval-rst}
.. autoclass:: mnt.pyfiction.sidb.simulation.logic.input_bdl_configuration
   :members:

.. autoclass:: mnt.pyfiction.sidb.simulation.logic.bdl_input_iterator
   :members:

.. autofunction:: mnt.pyfiction.sidb.simulation.logic.generate_bdl_input_pattern_layouts
```

:::

::::

## Operational Checks

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

```{doxygenfunction} fiction::sidb::simulation::logic::is_operational(const layout& lyt, const std::vector<kitty::dynamic_truth_table>& spec, const is_operational_params& params = {})

```

```{doxygenfunction} fiction::sidb::simulation::logic::is_operational(const layout& lyt, const std::vector<kitty::dynamic_truth_table>& spec, const is_operational_params& params, const std::vector<bdl_wire>& input_bdl_wire, const std::vector<bdl_wire>& output_bdl_wire, const std::optional<layout>& canvas_lyt = std::nullopt)

```

```{doxygenfunction} fiction::sidb::simulation::logic::is_operational(const std::vector<layout>& input_pattern_layouts, const std::vector<kitty::dynamic_truth_table>& spec, const is_operational_params& params, const std::vector<bdl_wire>& input_bdl_wire, const std::vector<bdl_wire>& output_bdl_wire, const std::optional<layout>& canvas_lyt = std::nullopt)

```

```{doxygenfunction} fiction::sidb::simulation::logic::operational_input_patterns(const layout& lyt, const std::vector<kitty::dynamic_truth_table>& spec, const is_operational_params& params = {})

```

```{doxygenfunction} fiction::sidb::simulation::logic::operational_input_patterns(const layout& lyt, const std::vector<kitty::dynamic_truth_table>& spec, const is_operational_params& params, const std::vector<bdl_wire>& input_bdl_wire, const std::vector<bdl_wire>& output_bdl_wire, const std::optional<layout>& canvas_lyt = std::nullopt)

```

```{doxygenfunction} fiction::sidb::simulation::logic::is_kink_induced_non_operational(const layout& lyt, const std::vector<kitty::dynamic_truth_table>& spec, const is_operational_params& params = {})

```

```{doxygenfunction} fiction::sidb::simulation::logic::is_kink_induced_non_operational(const layout& lyt, const std::vector<kitty::dynamic_truth_table>& spec, const is_operational_params& params, const std::vector<bdl_wire>& input_bdl_wire, const std::vector<bdl_wire>& output_bdl_wire, const std::optional<layout>& canvas_lyt = std::nullopt)

```

```{doxygenfunction} fiction::sidb::simulation::logic::kink_induced_non_operational_input_patterns(const layout& lyt, const std::vector<kitty::dynamic_truth_table>& spec, const is_operational_params& params = {})

```

```{doxygenfunction} fiction::sidb::simulation::logic::kink_induced_non_operational_input_patterns(const layout& lyt, const std::vector<kitty::dynamic_truth_table>& spec, const is_operational_params& params, const std::vector<bdl_wire>& input_bdl_wire, const std::vector<bdl_wire>& output_bdl_wire, const std::optional<layout>& canvas_lyt = std::nullopt)

```

:::

:::{tab-item} Python
:sync: python

```{eval-rst}
.. autoclass:: mnt.pyfiction.sidb.simulation.logic.operational_status
   :members:

.. autoclass:: mnt.pyfiction.sidb.simulation.logic.operational_condition
   :members:

.. autoclass:: mnt.pyfiction.sidb.simulation.logic.operational_analysis_strategy
   :members:

.. autofunction:: mnt.pyfiction.sidb.simulation.logic.is_operational

.. autofunction:: mnt.pyfiction.sidb.simulation.logic.operational_input_patterns

.. autofunction:: mnt.pyfiction.sidb.simulation.logic.is_kink_induced_non_operational

.. autofunction:: mnt.pyfiction.sidb.simulation.logic.kink_induced_non_operational_input_patterns
```

:::

::::

(opdom)=

## Operational Domain Computation

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

```{doxygenfunction} fiction::sidb::simulation::logic::operational_domain_grid_search(const layout& lyt, const std::vector<kitty::dynamic_truth_table>& spec, const operational_domain_params& params = {}, operational_domain_stats* stats = nullptr)

```

```{doxygenfunction} fiction::sidb::simulation::logic::operational_domain_random_sampling(const layout& lyt, const std::vector<kitty::dynamic_truth_table>& spec, const std::size_t samples, const operational_domain_params& params = {}, operational_domain_stats* stats = nullptr)

```

```{doxygenfunction} fiction::sidb::simulation::logic::operational_domain_flood_fill(const layout& lyt, const std::vector<kitty::dynamic_truth_table>& spec, const std::size_t samples, const operational_domain_params& params = {}, operational_domain_stats* stats = nullptr)

```

```{doxygenfunction} fiction::sidb::simulation::logic::operational_domain_contour_tracing(const layout& lyt, const std::vector<kitty::dynamic_truth_table>& spec, const std::size_t samples, const operational_domain_params& params = {}, operational_domain_stats* stats = nullptr)

```

```{doxygenfunction} fiction::sidb::simulation::logic::critical_temperature_domain_grid_search(const layout& lyt, const std::vector<kitty::dynamic_truth_table>& spec, const operational_domain_params& params = {}, operational_domain_stats* stats = nullptr)

```

```{doxygenfunction} fiction::sidb::simulation::logic::critical_temperature_domain_random_sampling(const layout& lyt, const std::vector<kitty::dynamic_truth_table>& spec, const std::size_t samples, const operational_domain_params& params = {}, operational_domain_stats* stats = nullptr)

```

```{doxygenfunction} fiction::sidb::simulation::logic::critical_temperature_domain_flood_fill(const layout& lyt, const std::vector<kitty::dynamic_truth_table>& spec, const std::size_t samples, const operational_domain_params& params = {}, operational_domain_stats* stats = nullptr)

```

```{doxygenfunction} fiction::sidb::simulation::logic::critical_temperature_domain_contour_tracing(const layout& lyt, const std::vector<kitty::dynamic_truth_table>& spec, const std::size_t samples, const operational_domain_params& params = {}, operational_domain_stats* stats = nullptr)

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

## Operational Domain Ratio

::::{tab-set}
:sync-group: language

:::{tab-item} C++
:sync: cpp

**Header:** `fiction/technology/sidb/simulation/logic/operational_domain_ratio.hpp`

```{doxygenstruct} fiction::sidb::simulation::logic::operational_domain_ratio_params
:members:
```

```{doxygenfunction} fiction::sidb::simulation::logic::operational_domain_ratio(const layout& lyt, const std::vector<kitty::dynamic_truth_table>& spec, const parameter_point& pp, const operational_domain_ratio_params& params = {})

```

:::

:::{tab-item} Python
:sync: python

```{eval-rst}
.. autofunction:: mnt.pyfiction.sidb.simulation.logic.operational_domain_ratio
```

:::

::::

## Logic Matching

**Header:** `fiction/technology/sidb/simulation/logic/verify_logic_match.hpp`

```{doxygenfunction} fiction::sidb::simulation::logic::verify_logic_match

```

## Binary-Dot Logic (BDL) Pair Detection

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

```{doxygenfunction} fiction::sidb::simulation::logic::detect_bdl_pairs(const layout& lyt, const std::optional<dot_tag>& type = std::nullopt, const detect_bdl_pairs_params& params = {})

```

:::

:::{tab-item} Python
:sync: python

```{eval-rst}
.. autoclass:: mnt.pyfiction.sidb.simulation.logic.bdl_pair
   :members:

.. autofunction:: mnt.pyfiction.sidb.simulation.logic.detect_bdl_pairs
```

:::

::::

## Binary-Dot Logic (BDL) Wire Detection

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

```{doxygenfunction} fiction::sidb::simulation::logic::detect_bdl_wires(const layout& lyt, const detect_bdl_wires_params& params = {}, const bdl_wire_selection wire_selection = bdl_wire_selection::ALL)

```

:::

:::{tab-item} Python
:sync: python

```{eval-rst}
.. autoclass:: mnt.pyfiction.sidb.simulation.logic.bdl_wire_selection
   :members:

.. autoclass:: mnt.pyfiction.sidb.simulation.logic.bdl_wire
   :members:

.. autofunction:: mnt.pyfiction.sidb.simulation.logic.detect_bdl_wires
```

:::

::::

## Example

Sweep relative permittivity at two points and write the classified domain as CSV.
This layout produces two `NON_OPERATIONAL` entries: a completed sweep does not imply
that the layout implements the identity function.

::::{tab-set}
:sync-group: language

:::{tab-item} C++
:sync: cpp

```cpp
#include <fiction/technology/sidb/layout.hpp>
#include <fiction/technology/sidb/simulation/logic/operational_domain.hpp>
#include <fiction/technology/sidb/simulation/io/write_operational_domain.hpp>
#include <fiction/synthesis/truth_tables.hpp>
#include <cassert>
#include <initializer_list>
#include <iostream>

int main()
{
    namespace sidb = fiction::sidb;
    namespace logic = sidb::simulation::logic;
    sidb::layout wire{};
    const auto place = [&](sidb::dot_tag tag, std::initializer_list<int> xs)
    {
        for (const auto x : xs) { wire.assign_sidb({x, 0, 0}, tag); }
    };
    place(sidb::dot_tag::INPUT, {0, 2});
    place(sidb::dot_tag::NORMAL, {6, 8, 18});
    place(sidb::dot_tag::OUTPUT, {12, 14});
    logic::operational_domain_params params{};
    params.operational_params.sim_params.base = 2;
    params.operational_params.timeout = 5'000;
    params.sweep_dimensions = {{logic::sweep_parameter::EPSILON_R, 5.5, 5.6, 0.1}};
    const auto domain = logic::operational_domain_grid_search(wire, {fiction::synthesis::create_id_tt()}, params);
    assert(domain.size() == 2);
    sidb::simulation::io::write_operational_domain(domain, std::cout);
}
```

:::

:::{tab-item} Python
:sync: python

```python
from mnt.pyfiction.sidb import lattice_site, sidb_dot_tag, sidb_layout
from mnt.pyfiction.sidb.simulation.io import write_operational_domain_to_string
from mnt.pyfiction.sidb.simulation.logic import (
    operational_domain_grid_search,
    operational_domain_value_range,
    sweep_parameter,
)
from mnt.pyfiction.synthesis import create_id_tt

wire = sidb_layout()
for tag, xs in (
    (sidb_dot_tag.INPUT, (0, 2)),
    (sidb_dot_tag.NORMAL, (6, 8, 18)),
    (sidb_dot_tag.OUTPUT, (12, 14)),
):
    for x in xs:
        wire.assign_sidb(lattice_site(x, 0, 0), tag)
axis = operational_domain_value_range(sweep_parameter.EPSILON_R, 5.5, 5.6, 0.1)
domain = operational_domain_grid_search(wire, [create_id_tt()], base=2, sweep_dimensions=[axis], timeout=5_000)
assert len(domain) == 2
print(write_operational_domain_to_string(domain))
```

:::

::::
