(wiring_reduction)=

# Wiring Reduction in 2DDWave-Clocked Cartesian Layouts

Reduces wiring in a 2DDWave-clocked Cartesian gate-level layout in place. The algorithm
uses A\* to find horizontal and vertical cuts, removes selected wire tiles, and shifts
and reconnects the remaining objects upward or leftward.

All placed objects must lie inside the layout frame. The search repeats until it finds no further cut
or its timeout expires; it does not guarantee minimum wiring or area.

::::{tab-set}
:sync-group: language

:::{tab-item} C++
:sync: cpp

**Header:** `fiction/physical_design/wiring_reduction.hpp`

```{doxygenstruct} fiction::physical_design::wiring_reduction_params
:members:
```

```{doxygenstruct} fiction::physical_design::wiring_reduction_stats
:members:
```

```{doxygenfunction} fiction::physical_design::wiring_reduction

```

:::

:::{tab-item} Python
:sync: python

```{eval-rst}
.. autoclass:: mnt.pyfiction.physical_design.wiring_reduction_stats
   :members:

.. autofunction:: mnt.pyfiction.physical_design.wiring_reduction
```

:::

::::
