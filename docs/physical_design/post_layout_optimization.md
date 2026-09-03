(post_layout_optimization)=

# Optimizing 2DDWave-clocked Cartesian Layouts

(post_layout_opt)=

This algorithm aims to decrease the overall layout area of a given 2DDWave-clocked Cartesian layout that has been
generated using either heuristic methods or machine learning techniques. It achieves this objective by strategically
repositioning gates within the layout, removing excess wiring using the `wiring_reduction` algorithm (cf. {ref}`wiring_reduction`), and effectively
relocating outputs to more favorable positions.

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

```{autoclass} mnt.pyfiction.post_layout_optimization_params
:members:
```

```{autoclass} mnt.pyfiction.post_layout_optimization_stats
:members:
```

```{autofunction} mnt.pyfiction.post_layout_optimization

```

:::

::::
