# Distance Maps

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
