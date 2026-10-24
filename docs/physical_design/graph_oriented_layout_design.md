(graph_oriented_layout_design)=

# Graph-Oriented Layout Design

Generates FCN gate-level layouts from logic network specifications by spanning a search space graph where each placement
event can be represented as a search space vertex characterized by a partial layout at that instance. Edges between a
partial layout $a$ and $b$ exist iff $a$ can be transformed into $b$ via a single placement
event. Similar to navigating through a maze, A\*-search can be employed to discover a path from the starting vertex (the
empty layout) to the exit of the maze (a layout with all gates placed). This approach is scalable but requires that the
input network is restricted to a 3-graph. At the same time, the output layout will always be 2DDWave-clocked and is not
always optimal.

An unset `seed` selects a random seed. With a fixed `seed`, sequential searches that return their first solution or
exhaust their frontier before their timeout reproduce the same layout within the same build. Custom cost functions
must also be deterministic. A seed does not fix how many expansions fit into a wall-clock timeout, and parallel
searches depend on worker scheduling. Parallel custom cost callbacks must be thread-safe. `HIGH_EFFICIENCY` uses a
10-second timeout when no explicit timeout is supplied; this implicit timeout stops the search only after a solution
exists.

The `AREA` priority uses geometric tile counts, including when a partial layout is one tile wide or high. The area
estimate excludes the spare eastern and southern borders and includes the candidate position. Layouts may change
across library versions as the search and routing algorithms evolve.

::::{tab-set}
:sync-group: language

:::{tab-item} C++
:sync: cpp

**Header:** `fiction/physical_design/graph_oriented_layout_design.hpp`

```{doxygenstruct} fiction::physical_design::graph_oriented_layout_design_params
:members:
```

```{doxygenstruct} fiction::physical_design::graph_oriented_layout_design_stats
:members:
```

```{doxygenfunction} fiction::physical_design::to_string(const graph_oriented_layout_design_params::effort_mode mode) noexcept
```

```{doxygenfunction} fiction::physical_design::operator<<(std::ostream& os, const graph_oriented_layout_design_params::effort_mode mode) noexcept
```

```{doxygenfunction} fiction::physical_design::to_string(const graph_oriented_layout_design_params::cost_objective cost) noexcept
```

```{doxygenfunction} fiction::physical_design::operator<<(std::ostream& os, const graph_oriented_layout_design_params::cost_objective cost) noexcept
```

```{doxygenfunction} fiction::physical_design::graph_oriented_layout_design

```

:::

:::{tab-item} Python
:sync: python

```{eval-rst}
.. autoclass:: mnt.pyfiction.physical_design.gold_cost_objective
   :members:
   :undoc-members:

.. autoclass:: mnt.pyfiction.physical_design.gold_effort_mode
   :members:
   :undoc-members:

.. autoclass:: mnt.pyfiction.physical_design.graph_oriented_layout_design_stats
   :members:

.. autofunction:: mnt.pyfiction.physical_design.graph_oriented_layout_design
```

:::

::::
