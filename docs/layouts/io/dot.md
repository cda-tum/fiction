# Graphviz (DOT) Drawers

::::{tab-set}
:sync-group: language

:::{tab-item} C++
:sync: cpp

**Header:** `fiction/layouts/io/layout_drawers.hpp`

```{doxygenclass} fiction::layouts::io::simple_gate_layout_tile_drawer
:members:
```

```{doxygenclass} fiction::layouts::io::gate_layout_cartesian_drawer
:members:
```

```{doxygenclass} fiction::layouts::io::gate_layout_shifted_cartesian_drawer
:members:
```

```{doxygenclass} fiction::layouts::io::gate_layout_hexagonal_drawer
:members:
```

```{doxygenfunction} fiction::layouts::io::write_dot_layout(const layouts::gate_layout<Geometry>& lyt, std::ostream& os, const Drawer& drawer = {}, utils::progress_callback on_progress = {})

```

```{doxygenfunction} fiction::layouts::io::write_dot_layout(const layouts::gate_layout<Geometry>& lyt, const std::string_view& filename, const Drawer& drawer = {}, utils::progress_callback on_progress = {})

```

:::

:::{tab-item} Python
:sync: python

```{eval-rst}
.. autofunction:: mnt.pyfiction.layouts.io.write_dot_layout
```

:::

::::
