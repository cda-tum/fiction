(opdom)=

# Operational Domain Computation

The plots below show the {download}`SiQAD OR gate <../../../../../test/python/resources/siqad_or_gate.sqd>`
with each reconstruction method. QuickExact evaluates the OR truth table with two charge states,
distance-encoded inputs, tolerated kinks, and $\mu^{-} = -0.32\,\mathrm{eV}$. Both $\epsilon_r$ and
$\lambda_{\mathrm{TF}}$ range from 1 to 10 in steps of 0.1; the latter is measured in nm.
Purple marks operational samples, gray marks non-operational samples, and blank areas contain no samples.
Random sampling uses 500 draws; flood fill and contour tracing start from 500 random draws.
These three methods can produce different samples on each run.

Install `mnt-pyfiction[plot]`, download the gate, and run these commands in Bash in the download directory:

```bash
fiction opdom siqad_or_gate.sqd --gate or --algorithm grid_search \
    --bdl-wire-distance 1.5 --threads 4 --width 7 --height 5 --dpi 180 \
    --title "SiQAD OR gate: grid search" --plot opdom_siqad_or_grid_search.png
for algorithm in random_sampling flood_fill contour_tracing; do
    fiction opdom siqad_or_gate.sqd --gate or --algorithm "$algorithm" --samples 500 \
        --bdl-wire-distance 1.5 --threads 4 --width 7 --height 5 --dpi 180 \
        --title "SiQAD OR gate: ${algorithm//_/ }" --plot "opdom_siqad_or_${algorithm}.png"
done
```

The 1.5 nm BDL wire threshold keeps the gate's two inputs separate. The other simulation,
sweep, color, and legend options use the CLI defaults.

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

```{image} /_static/opdom_siqad_or_grid_search.png
:alt: SiQAD OR gate operational domain from grid search, with purple operational and gray non-operational samples.
:width: 100%
```

Grid search evaluates every point in the parameter grid.

```{doxygenfunction} fiction::sidb::simulation::logic::operational_domain_random_sampling(const layout& lyt, const std::vector<kitty::dynamic_truth_table>& spec, const std::size_t samples, const operational_domain_params& params = {}, operational_domain_stats* stats = nullptr)

```

```{image} /_static/opdom_siqad_or_random_sampling.png
:alt: SiQAD OR gate operational domain from random sampling, with purple operational and gray non-operational samples.
:width: 100%
```

Random sampling evaluates isolated points across the parameter grid.

```{doxygenfunction} fiction::sidb::simulation::logic::operational_domain_flood_fill(const layout& lyt, const std::vector<kitty::dynamic_truth_table>& spec, const std::size_t samples, const operational_domain_params& params = {}, operational_domain_stats* stats = nullptr)

```

```{image} /_static/opdom_siqad_or_flood_fill.png
:alt: SiQAD OR gate operational domain from flood fill, with purple operational and gray non-operational samples.
:width: 100%
```

Flood fill expands from operational seeds, evaluates their neighbors, and retains its initial random samples.

```{doxygenfunction} fiction::sidb::simulation::logic::operational_domain_contour_tracing(const layout& lyt, const std::vector<kitty::dynamic_truth_table>& spec, const std::size_t samples, const operational_domain_params& params = {}, operational_domain_stats* stats = nullptr)

```

```{image} /_static/opdom_siqad_or_contour_tracing.png
:alt: SiQAD OR gate operational domain from contour tracing, with purple operational and gray non-operational samples.
:width: 100%
```

Contour tracing samples the operational boundary and retains its initial random samples.

```{doxygenfunction} fiction::sidb::simulation::logic::critical_temperature_domain_grid_search(const layout& lyt, const std::vector<kitty::dynamic_truth_table>& spec, const operational_domain_params& params = {}, operational_domain_stats* stats = nullptr)

```

```{doxygenfunction} fiction::sidb::simulation::logic::critical_temperature_domain_random_sampling(const layout& lyt, const std::vector<kitty::dynamic_truth_table>& spec, const std::size_t samples, const operational_domain_params& params = {}, operational_domain_stats* stats = nullptr)

```

```{doxygenfunction} fiction::sidb::simulation::logic::critical_temperature_domain_flood_fill(const layout& lyt, const std::vector<kitty::dynamic_truth_table>& spec, const std::size_t samples, const operational_domain_params& params = {}, operational_domain_stats* stats = nullptr)

```

```{doxygenfunction} fiction::sidb::simulation::logic::critical_temperature_domain_contour_tracing(const layout& lyt, const std::vector<kitty::dynamic_truth_table>& spec, const std::size_t samples, const operational_domain_params& params = {}, operational_domain_stats* stats = nullptr)

```

**Header:** `fiction/technology/sidb/simulation/logic/operational_domain_ratio.hpp`

```{doxygenstruct} fiction::sidb::simulation::logic::operational_domain_ratio_params
:members:
```

```{doxygenfunction} fiction::sidb::simulation::logic::operational_domain_ratio(const layout& lyt, const std::vector<kitty::dynamic_truth_table>& spec, const parameter_point& pp, const operational_domain_ratio_params& params = {})

```

**Header:** `fiction/technology/sidb/simulation/logic/verify_logic_match.hpp`

```{doxygenfunction} fiction::sidb::simulation::logic::verify_logic_match

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
```

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
.. autofunction:: mnt.pyfiction.sidb.simulation.logic.operational_domain_grid_search

.. image:: /_static/opdom_siqad_or_grid_search.png
   :alt: SiQAD OR gate operational domain from grid search, with purple operational and gray non-operational samples.
   :width: 100%

Grid search evaluates every point in the parameter grid.

.. autofunction:: mnt.pyfiction.sidb.simulation.logic.operational_domain_random_sampling

.. image:: /_static/opdom_siqad_or_random_sampling.png
   :alt: SiQAD OR gate operational domain from random sampling, with purple operational and gray non-operational samples.
   :width: 100%

Random sampling evaluates isolated points across the parameter grid.

.. autofunction:: mnt.pyfiction.sidb.simulation.logic.operational_domain_flood_fill

.. image:: /_static/opdom_siqad_or_flood_fill.png
   :alt: SiQAD OR gate operational domain from flood fill, with purple operational and gray non-operational samples.
   :width: 100%

Flood fill expands from operational seeds, evaluates their neighbors, and retains its initial random samples.

.. autofunction:: mnt.pyfiction.sidb.simulation.logic.operational_domain_contour_tracing

.. image:: /_static/opdom_siqad_or_contour_tracing.png
   :alt: SiQAD OR gate operational domain from contour tracing, with purple operational and gray non-operational samples.
   :width: 100%

Contour tracing samples the operational boundary and retains its initial random samples.

.. autofunction:: mnt.pyfiction.sidb.simulation.logic.critical_temperature_domain_grid_search

.. autofunction:: mnt.pyfiction.sidb.simulation.logic.critical_temperature_domain_random_sampling

.. autofunction:: mnt.pyfiction.sidb.simulation.logic.critical_temperature_domain_flood_fill

.. autofunction:: mnt.pyfiction.sidb.simulation.logic.critical_temperature_domain_contour_tracing

.. autofunction:: mnt.pyfiction.sidb.simulation.logic.operational_domain_ratio
```

:::

::::
