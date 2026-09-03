# Layouts

```{toctree}
:maxdepth: 1

bounding_box
cartesian_layout
cell_level_layout
clocked_layout
clocking_scheme
coordinates
gate_level_layout
hexagonal_layout
obstruction_layout
shifted_cartesian_layout
synchronization_element_layout
tile_based_layout
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

```{doxygenfunction} fiction::layouts::convert_layout_to_siqad_coordinates

```

```{doxygenfunction} fiction::layouts::convert_layout_to_fiction_coordinates

```

```{doxygenfunction} fiction::layouts::random_coordinate

```

```{doxygenfunction} fiction::layouts::all_coordinates_in_spanned_area

```

:::

:::{tab-item} Python
:sync: python

```{autofunction} mnt.pyfiction.num_adjacent_coordinates

```

```{autofunction} mnt.pyfiction.normalize_layout_coordinates

```

```{autofunction} mnt.pyfiction.convert_layout_to_siqad_coordinates

```

```{autofunction} mnt.pyfiction.random_coordinate

```

:::

::::
