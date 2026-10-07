# Layout I/O

## Gate-level Layouts

Can be used to read gate-level layout files (`.fgl`) as offered by [MNT Bench](https://www.cda.cit.tum.de/mntbench/).

The reader accepts legacy files with maximum-coordinate sizes and version 2 files with size counts.
Version 2 preserves declared PI/PO order and truth-table input indices. Objects are placed before
their connections are assigned, so file order does not constrain dependency order. A failed read
leaves an existing destination layout unchanged.

::::{tab-set}
:sync-group: language

:::{tab-item} C++
:sync: cpp

**Header:** `fiction/layouts/io/read_fgl_layout.hpp`

```{doxygenfunction} fiction::layouts::io::read_fgl_layout(std::istream& is, const std::string_view& name = "")

```

```{doxygenfunction} fiction::layouts::io::read_fgl_layout(Lyt& lyt, std::istream& is)

```

```{doxygenfunction} fiction::layouts::io::read_fgl_layout(const std::string_view& filename, const std::string_view& name = "")

```

```{doxygenfunction} fiction::layouts::io::read_fgl_layout(Lyt& lyt, const std::string_view& filename)

```

```{doxygenclass} fiction::layouts::io::fgl_parsing_error

```

:::

:::{tab-item} Python
:sync: python

```{eval-rst}
.. autofunction:: mnt.pyfiction.layouts.io.read_cartesian_fgl_layout

.. autofunction:: mnt.pyfiction.layouts.io.read_shifted_cartesian_fgl_layout

.. autofunction:: mnt.pyfiction.layouts.io.read_hexagonal_fgl_layout

.. autoclass:: mnt.pyfiction.layouts.io.fgl_parsing_error
   :members:
```

:::

::::

## Technology-independent Gate-level Layouts

Can be used to generate gate-level layout files (`.fgl`) as offered by [MNT Bench](https://www.cda.cit.tum.de/mntbench/).

The writer emits version 2 and stores every object, including complete dangling logic. The writer
validates all input slots, cycles, physical connections, clocking, and frame membership before
writing. Empty layouts are supported. FGL represents supported named clocking schemes with sparse
clock overrides and synchronization delays. Editing checkpoints are outside this format's scope.

::::{tab-set}
:sync-group: language

:::{tab-item} C++
:sync: cpp

**Header:** `fiction/layouts/io/write_fgl_layout.hpp`

```{doxygenfunction} fiction::layouts::io::write_fgl_layout(const Lyt& lyt, std::ostream& os, utils::progress_callback on_progress = {})

```

```{doxygenfunction} fiction::layouts::io::write_fgl_layout(const Lyt& lyt, const std::string_view& filename, utils::progress_callback on_progress = {})

```

:::

:::{tab-item} Python
:sync: python

```{eval-rst}
.. autofunction:: mnt.pyfiction.layouts.io.write_fgl_layout
```

:::

::::

## Layout Printing

**Header:** `fiction/layouts/io/print_layout.hpp`

```{doxygenfunction} fiction::layouts::io::print_gate_level_layout

```

```{doxygenfunction} fiction::layouts::io::print_cell_level_layout

```

```{doxygenfunction} fiction::layouts::io::print_layout

```

## Graphviz (DOT) Drawers

::::{tab-set}
:sync-group: language

:::{tab-item} C++
:sync: cpp

**Header:** `fiction/layouts/io/layout_drawers.hpp`

```{doxygenclass} fiction::layouts::io::simple_gate_layout_tile_drawer

```

```{doxygenclass} fiction::layouts::io::gate_layout_cartesian_drawer

```

```{doxygenclass} fiction::layouts::io::gate_layout_shifted_cartesian_drawer

```

```{doxygenclass} fiction::layouts::io::gate_layout_hexagonal_drawer

```

```{doxygenfunction} fiction::layouts::io::write_dot_layout(const Lyt& lyt, std::ostream& os, const Drawer& drawer = {}, utils::progress_callback on_progress = {})

```

```{doxygenfunction} fiction::layouts::io::write_dot_layout(const Lyt& lyt, const std::string_view& filename, const Drawer& drawer = {}, utils::progress_callback on_progress = {})

```

:::

:::{tab-item} Python
:sync: python

```{eval-rst}
.. autofunction:: mnt.pyfiction.layouts.io.write_dot_layout
```

:::

::::
