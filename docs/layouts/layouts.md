# Layouts

```{toctree}
:maxdepth: 1

bounding_box
cartesian_layout
cell_grid
clocking_scheme
clocking_state
coordinates
gate_layout
hexagonal_layout
obstructions
shifted_cartesian_layout
io/io
```

(layout-utils)=

## Layout Utilities

::::{tab-set}
:sync-group: language

:::{tab-item} C++
:sync: cpp

**Header:** `fiction/layouts/layout_utils.hpp`

```{doxygenfunction} fiction::layouts::require_arrangement
```

```{doxygenfunction} fiction::layouts::make_gate_level_layout
```

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
.. autofunction:: mnt.pyfiction.layouts.num_adjacent_coordinates

.. autofunction:: mnt.pyfiction.layouts.normalize_layout_coordinates

.. autofunction:: mnt.pyfiction.layouts.random_coordinate
```

:::

::::

## Type Traits

**Header:** `fiction/traits.hpp`

```{doxygenfile} traits.hpp
```

## Preset Types

**Header:** `fiction/types.hpp`

```{doxygentypedef} fiction::tt
```

```{doxygentypedef} fiction::tt_ptr
```

```{doxygentypedef} fiction::truth_table_t
```

```{doxygentypedef} fiction::aig_nt
```

```{doxygentypedef} fiction::aig_ptr
```

```{doxygentypedef} fiction::xag_nt
```

```{doxygentypedef} fiction::xag_ptr
```

```{doxygentypedef} fiction::mig_nt
```

```{doxygentypedef} fiction::mig_ptr
```

```{doxygentypedef} fiction::tec_nt
```

```{doxygentypedef} fiction::tec_ptr
```

```{doxygentypedef} fiction::logic_network_t
```

```{doxygentypedef} fiction::cart_gate_clk_lyt
```

```{doxygentypedef} fiction::cart_gate_clk_lyt_ptr
```

```{doxygentypedef} fiction::shifted_cart_gate_clk_lyt
```

```{doxygentypedef} fiction::shifted_cart_gate_clk_lyt_ptr
```

```{doxygentypedef} fiction::hex_gate_clk_lyt
```

```{doxygentypedef} fiction::hex_gate_clk_lyt_ptr
```

```{doxygentypedef} fiction::gate_layout_t
```

```{doxygenvariable} fiction::aig_name
```

```{doxygenvariable} fiction::xag_name
```

```{doxygenvariable} fiction::mig_name
```

```{doxygenvariable} fiction::tec_name
```

```{doxygenvariable} fiction::ntk_type_name
```

```{doxygenfunction} fiction::get_ntk_type_name
```
