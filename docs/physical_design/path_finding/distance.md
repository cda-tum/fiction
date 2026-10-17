(distance-functions)=

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
