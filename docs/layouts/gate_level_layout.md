# Gate-level Layout

`gate_level_layout<CoordinateLayout>` places Boolean gates and wires on a coordinate layout.
The layout owns its clocking scheme, synchronization delays, and persistent obstructions.
Tile operations are available directly. Geometry remains a template argument; clocking
schemes and obstruction assignments do not change the layout type.

The layout represents logic placement and routing independently of a cell technology.
A gate library supplies physical cell implementations and tile dimensions.

The gate-level layout is implemented in accordance with `mockturtle`'s
[network interface API](https://mockturtle.readthedocs.io/en/latest/network.html). Thus, it is itself a mockturtle
network that can be passed to various of mockturtle's algorithms. However, since each logic node (gate) in the layout
has to have a concrete position assigned, mockturtle cannot be used to generate valid layouts.

::::{tab-set}
:sync-group: language

:::{tab-item} C++
:sync: cpp

**Header:** `fiction/layouts/gate_level_layout.hpp`

```{doxygenclass} fiction::layouts::gate_level_layout
:members:
```

:::

:::{tab-item} Python
:sync: python

```{eval-rst}
.. autoclass:: mnt.pyfiction.cartesian_gate_layout
   :members:

.. autoclass:: mnt.pyfiction.hexagonal_gate_layout
   :members:
```

:::

::::

## Construction and capabilities

::::{tab-set}
:sync-group: language

:::{tab-item} C++
:sync: cpp

```cpp
using layout = fiction::layouts::gate_level_layout<fiction::layouts::cartesian_layout<>>;
layout lyt{{4, 4}, fiction::layouts::clocking::twoddwave()};
lyt.assign_synchronization_element({1, 1}, 2);
lyt.obstruct_coordinate({2, 2});
auto independent = lyt.clone();
```

:::

:::{tab-item} Python
:sync: python

```python
from mnt.pyfiction import cartesian_gate_layout

lyt = cartesian_gate_layout((4, 4), "2DDWave")
lyt.assign_synchronization_element((1, 1), 2)
lyt.obstruct_coordinate((2, 2))
independent = lyt.clone()
```

:::

::::

Ordinary C++ copies share layout storage. `clone()` copies geometry, gates, clocking,
synchronization, and obstructions independently. Explicit obstructions survive clearing
or moving gates; removing an explicit obstruction does not remove a gate or wire.

An empty layout uses the four-phase OPEN scheme. Assigning a synchronization delay of zero
removes the synchronization element. Replacing the clocking scheme preserves synchronization
delays. Synchronization changes clocked traversal; it does not add support for sequential circuits.
