# Distance Functions

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

```

```{doxygenclass} fiction::physical_design::path_finding::euclidean_distance_functor

```

```{doxygenclass} fiction::physical_design::path_finding::squared_euclidean_distance_functor

```

```{doxygenclass} fiction::physical_design::path_finding::twoddwave_distance_functor

```

```{doxygenclass} fiction::physical_design::path_finding::chebyshev_distance_functor

```

:::

:::{tab-item} Python
:sync: python

```{autofunction} mnt.pyfiction.manhattan_distance

```

```{autofunction} mnt.pyfiction.euclidean_distance

```

```{autofunction} mnt.pyfiction.squared_euclidean_distance

```

```{autofunction} mnt.pyfiction.twoddwave_distance

```

```{autofunction} mnt.pyfiction.chebyshev_distance

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

```

```{doxygenclass} fiction::physical_design::path_finding::random_cost_functor

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

```

:::

:::{tab-item} Python
:sync: python

```{autoclass} mnt.pyfiction.a_star_params
:members:
```

```{autofunction} mnt.pyfiction.a_star

```

```{autofunction} mnt.pyfiction.a_star_distance

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

```{autoclass} mnt.pyfiction.yen_k_shortest_paths_params
:members:
```

```{autofunction} mnt.pyfiction.yen_k_shortest_paths

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

```{autoclass} mnt.pyfiction.enumerate_all_paths_params
:members:
```

```{autofunction} mnt.pyfiction.enumerate_all_paths

```

:::

::::
