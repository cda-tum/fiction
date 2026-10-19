# Technology-Independent Gate-Level Layouts

Can be used to generate gate-level layout files (`.fgl`) as offered by [MNT Bench](https://www.cda.cit.tum.de/mntbench/).

The writer emits version 2 and stores every object, including complete dangling logic. The writer
validates all input slots, cycles, physical connections, clocking, and frame membership before
writing. Empty layouts are supported. FGL represents supported named clocking schemes with sparse
clock overrides, synchronization delays, and manual obstructions. Names may contain XML whitespace;
illegal XML control characters are rejected before writing. Editing checkpoints are outside this format's scope.

::::{tab-set}
:sync-group: language

:::{tab-item} C++
:sync: cpp

**Header:** `fiction/layouts/io/write_fgl_layout.hpp`

```{doxygenfunction} fiction::layouts::io::write_fgl_layout(const fiction::layouts::gate_layout<Geometry>& lyt, std::ostream& os, utils::progress_callback on_progress = {})

```

```{doxygenfunction} fiction::layouts::io::write_fgl_layout(const fiction::layouts::gate_layout<Geometry>& lyt, const std::string_view& filename, utils::progress_callback on_progress = {})

```

:::

:::{tab-item} Python
:sync: python

```{eval-rst}
.. autofunction:: mnt.pyfiction.layouts.io.write_fgl_layout
```

:::

::::
