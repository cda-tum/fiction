# Path Finding

## Example

Find a forward path through a 2DDWave-clocked Cartesian layout.
The returned path includes both endpoints.

::::{tab-set}
:sync-group: language

:::{tab-item} C++
:sync: cpp

```cpp
#include <fiction/layouts/cartesian_layout.hpp>
#include <fiction/layouts/gate_level_layout.hpp>
#include <fiction/layouts/clocking_scheme.hpp>
#include <fiction/physical_design/path_finding/a_star.hpp>
#include <cassert>

int main()
{
    using layout = fiction::layouts::gate_level_layout<fiction::layouts::cartesian_layout>;
    layout lyt{{5, 5}, fiction::layouts::clocking::twoddwave()};
    const auto path = fiction::physical_design::path_finding::a_star<
        fiction::physical_design::layout_coordinate_path>(lyt, {{0, 0}, {2, 2}});
    assert(path.size() == 5);
}
```

:::

:::{tab-item} Python
:sync: python

```python
from mnt.pyfiction.layouts import cartesian_gate_layout, coordinate
from mnt.pyfiction.physical_design.path_finding import a_star

layout = cartesian_gate_layout((5, 5), "2DDWave")
path = a_star(layout, coordinate(0, 0), coordinate(2, 2))
assert len(path) == 5
```

:::

::::

(distance-functions)=

## Distance Functions

Distance functions compute (an approximation for) the distance between two coordinates.

::::{tab-set}
:sync-group: language

:::{tab-item} C++
:sync: cpp

**Header:** `fiction/physical_design/path_finding/distance.hpp`

```{doxygenfunction} fiction::physical_design::path_finding::manhattan_distance

```

```{doxygenfunction} fiction::physical_design::path_finding::euclidean_distance

```

```{doxygenfunction} fiction::physical_design::path_finding::squared_euclidean_distance

```

```{doxygenfunction} fiction::physical_design::path_finding::twoddwave_distance

```

```{doxygenfunction} fiction::physical_design::path_finding::chebyshev_distance

```

```{doxygenclass} fiction::physical_design::path_finding::distance_functor
:members:
```

```{doxygenclass} fiction::physical_design::path_finding::manhattan_distance_functor
:members:
```

```{doxygenclass} fiction::physical_design::path_finding::euclidean_distance_functor
:members:
```

```{doxygenclass} fiction::physical_design::path_finding::squared_euclidean_distance_functor
:members:
```

```{doxygenclass} fiction::physical_design::path_finding::twoddwave_distance_functor
:members:
```

```{doxygenclass} fiction::physical_design::path_finding::chebyshev_distance_functor
:members:
```

:::

:::{tab-item} Python
:sync: python

```{eval-rst}
.. autofunction:: mnt.pyfiction.physical_design.path_finding.manhattan_distance

.. autofunction:: mnt.pyfiction.physical_design.path_finding.euclidean_distance

.. autofunction:: mnt.pyfiction.physical_design.path_finding.squared_euclidean_distance

.. autofunction:: mnt.pyfiction.physical_design.path_finding.twoddwave_distance

.. autofunction:: mnt.pyfiction.physical_design.path_finding.chebyshev_distance
```

:::

::::

## Distance Maps

Distance maps can store the distance from a coordinate to all other coordinates. They are particularly useful when
repeatedly calling complex distance functions that are expensive to evaluate. The distance maps can serve as a
lookup-table for these cases.

**Header:** `fiction/physical_design/path_finding/distance_map.hpp`

```{doxygentypedef} fiction::physical_design::path_finding::distance_map

```

```{doxygentypedef} fiction::physical_design::path_finding::sparse_distance_map

```

```{doxygenfunction} fiction::physical_design::path_finding::initialize_distance_map

```

```{doxygenfunction} fiction::physical_design::path_finding::initialize_sparse_distance_map

```

```{doxygenclass} fiction::physical_design::path_finding::distance_map_functor
:members:
```

```{doxygenclass} fiction::physical_design::path_finding::sparse_distance_map_functor
:members:
```

```{doxygenclass} fiction::physical_design::path_finding::smart_distance_cache_functor
:members:
```

## Cost Functions

Cost functions compute the cost to move from one coordinate to another (adjacent) one.

**Header:** `fiction/physical_design/path_finding/cost.hpp`

```{doxygenfunction} fiction::physical_design::path_finding::unit_cost

```

```{doxygenfunction} fiction::physical_design::path_finding::random_cost

```

```{doxygenclass} fiction::physical_design::path_finding::cost_functor
:members:
```

```{doxygenclass} fiction::physical_design::path_finding::unit_cost_functor
:members:
```

```{doxygenclass} fiction::physical_design::path_finding::random_cost_functor
:members:
```

## A\* Shortest Path

::::{tab-set}
:sync-group: language

:::{tab-item} C++
:sync: cpp

**Header:** `fiction/physical_design/path_finding/a_star.hpp`

```{doxygenstruct} fiction::physical_design::path_finding::a_star_params
:members:
```

```{doxygenfunction} fiction::physical_design::path_finding::a_star

```

```{doxygenfunction} fiction::physical_design::path_finding::a_star_distance

```

```{doxygenclass} fiction::physical_design::path_finding::a_star_distance_functor
:members:
```

:::

:::{tab-item} Python
:sync: python

```{eval-rst}
.. autofunction:: mnt.pyfiction.physical_design.path_finding.a_star

.. autofunction:: mnt.pyfiction.physical_design.path_finding.a_star_distance
```

:::

::::

## k Shortest Paths

::::{tab-set}
:sync-group: language

:::{tab-item} C++
:sync: cpp

**Header:** `fiction/physical_design/path_finding/k_shortest_paths.hpp`

```{doxygenstruct} fiction::physical_design::path_finding::yen_k_shortest_paths_params
:members:
```

```{doxygenfunction} fiction::physical_design::path_finding::yen_k_shortest_paths

```

:::

:::{tab-item} Python
:sync: python

```{eval-rst}
.. autofunction:: mnt.pyfiction.physical_design.path_finding.yen_k_shortest_paths
```

:::

::::

## Enumerate All Paths

::::{tab-set}
:sync-group: language

:::{tab-item} C++
:sync: cpp

**Header:** `fiction/physical_design/path_finding/enumerate_all_paths.hpp`

```{doxygenstruct} fiction::physical_design::path_finding::enumerate_all_paths_params
:members:
```

```{doxygenfunction} fiction::physical_design::path_finding::enumerate_all_paths

```

:::

:::{tab-item} Python
:sync: python

```{eval-rst}
.. autofunction:: mnt.pyfiction.physical_design.path_finding.enumerate_all_paths
```

:::

::::
