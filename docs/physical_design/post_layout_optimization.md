(post_layout_optimization)=

(post_layout_opt)=

# Optimizing 2DDWave-Clocked Cartesian Layouts

Optimizes a 2DDWave-clocked Cartesian gate-level layout in place. By default, the algorithm moves
gates toward the upper left, reroutes connections, removes excess wiring with
{ref}`wiring_reduction`, and relocates outputs to the layout border.

All placed objects must lie inside the layout frame. The heuristic can stop at its
timeout and does not guarantee a minimum-area layout.

::::{tab-set}
:sync-group: language

:::{tab-item} C++
:sync: cpp

**Header:** `fiction/physical_design/post_layout_optimization.hpp`

```{doxygenstruct} fiction::physical_design::post_layout_optimization_params
:members:
```

```{doxygenstruct} fiction::physical_design::post_layout_optimization_stats
:members:
```

```{doxygenfunction} fiction::physical_design::post_layout_optimization

```

:::

:::{tab-item} Python
:sync: python

```{eval-rst}
.. autoclass:: mnt.pyfiction.physical_design.post_layout_optimization_stats
   :members:

.. autofunction:: mnt.pyfiction.physical_design.post_layout_optimization
```

:::

::::
