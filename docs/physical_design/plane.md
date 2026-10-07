(plane)=

# Planar Placement and Routing

Planar Layout from Network Embedding (PLANE) places and routes a planar, balanced, fanout-substituted, ranked logic
network (see {ref}`planarization <planarization>`) as a 2DDWave-clocked gate-level layout without wire crossings
{cite:p}`nano_2025_3`. Every level of the network is placed on one anti-diagonal of the layout in rank order, so the
planar embedding of the network is preserved; lookup tables decide for every node whether it sits east or south of its
predecessor's wire and how many routing lines the level needs.

::::{tab-set}
:sync-group: language

:::{tab-item} C++
:sync: cpp

**Header:** `fiction/physical_design/plane.hpp`

```{doxygenstruct} fiction::physical_design::plane_params
:members:
```

```{doxygenstruct} fiction::physical_design::plane_stats
:members:
```

```{doxygenfunction} fiction::physical_design::plane

```

:::

:::{tab-item} Python
:sync: python

```{eval-rst}
.. autoclass:: mnt.pyfiction.physical_design.plane_params
   :members:

.. autoclass:: mnt.pyfiction.physical_design.plane_stats
   :members:

.. autofunction:: mnt.pyfiction.physical_design.plane
```

:::

::::
