# Obstructions

Gate and cell layouts expose persistent obstruction methods directly. Occupied tiles or
cells remain obstructed regardless of manual assignments. Connections are directed: blocking
`a` to `b` does not block `b` to `a`.

Use separate obstruction data for search constraints. A*, path enumeration, and Yen's
algorithm combine this data with the layout's obstructions without changing either input.
The same data works with bare coordinate grids. Search data contains no layout or occupancy.

::::{tab-set}
:sync-group: language

:::{tab-item} C++
:sync: cpp

**Header:** `fiction/layouts/obstructions.hpp`

```cpp
using grid = fiction::layouts::cartesian_layout<>;
grid lyt{{3, 3}};
fiction::layouts::obstructions<grid::coordinate> blocked;
blocked.obstruct_coordinate({1, 1});
using path = fiction::physical_design::layout_coordinate_path<grid>;
auto paths = fiction::physical_design::path_finding::yen_k_shortest_paths<path>(
    lyt, {{0, 0}, {3, 3}}, 3, {}, blocked);
```

```{doxygenclass} fiction::layouts::obstructions
:members:
```

:::

:::{tab-item} Python
:sync: python

```python
from mnt.pyfiction import RoutingObstructions, cartesian_layout, yen_k_shortest_paths

lyt = cartesian_layout((3, 3))
blocked = RoutingObstructions()
blocked.obstruct_coordinate((1, 1))
paths = yen_k_shortest_paths(lyt, (0, 0), (3, 3), 3, obstructions=blocked)
```

```{eval-rst}
.. autoclass:: mnt.pyfiction.RoutingObstructions
   :members:

.. autoclass:: mnt.pyfiction.CubeRoutingObstructions
   :members:
```

:::

::::
