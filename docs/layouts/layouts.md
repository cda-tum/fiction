# Layouts

```{toctree}
:maxdepth: 1

bounding_box
cartesian_layout
cell_level_layout
clocking_scheme
clocking_state
coordinates
gate_level_layout
hexagonal_layout
obstructions
shifted_cartesian_layout
io/io
```

## Layout Utils

::::{tab-set}
:sync-group: language

:::{tab-item} C++
:sync: cpp

**Header:** `fiction/layouts/layout_utils.hpp`

```{doxygenfunction} fiction::layouts::num_adjacent_coordinates

```

```{doxygenfunction} fiction::layouts::relative_to_absolute_cell_position

```

```{doxygenfunction} fiction::layouts::port_direction_to_coordinate

```

```{doxygenfunction} fiction::layouts::normalize_layout_coordinates

```

```{doxygenfunction} fiction::layouts::random_coordinate

```

:::

:::{tab-item} Python
:sync: python

```{eval-rst}
.. autofunction:: mnt.pyfiction.num_adjacent_coordinates

.. autofunction:: mnt.pyfiction.normalize_layout_coordinates

.. autofunction:: mnt.pyfiction.random_coordinate
```

:::

::::
