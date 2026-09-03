(wiring_reduction)=

# Wiring Reduction in 2DDWave-clocked Cartesian Layouts

**Header:** `fiction/physical_design/wiring_reduction.hpp`

This algorithm aims to minimize the number of wire segments, the area, and the length of the critical path in
2DDWave-clocked Cartesian gate-level layouts.

Initially, it constructs an equivalent layout where non-wire tiles are obstructed, and wire-tiles are obstructed
selectively based on the search direction, either horizontal from left to right or vertical from top to bottom.
Subsequently, it employs A\* path-finding to identify cuts through the layout that are eligible for deletion.

The removal of these wire tiles creates gaps, which are then filled by shifting all gates located beneath the emptied
spaces upward and subsequently reconnecting them. This iterative process continues until convergence is achieved.

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

```{autoclass} mnt.pyfiction.wiring_reduction_params
:members:
```

```{autoclass} mnt.pyfiction.wiring_reduction_stats
:members:
```

```{autofunction} mnt.pyfiction.wiring_reduction

```

:::

::::
