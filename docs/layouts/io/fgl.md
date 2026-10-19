# Gate-Level Layouts

Can be used to read gate-level layout files (`.fgl`) as offered by [MNT Bench](https://www.cda.cit.tum.de/mntbench/).

The reader accepts legacy files with maximum-coordinate sizes and version 2 files with size counts.
Version 2 preserves declared PI/PO order, truth-table input indices, manual obstructions, and
whitespace in names. Version 2 accepts complete, acyclic, physically valid layouts. Objects are placed before
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
:members:
```

:::

:::{tab-item} Python
:sync: python

```{eval-rst}
.. autofunction:: mnt.pyfiction.layouts.io.read_fgl_layout

.. autoclass:: mnt.pyfiction.layouts.io.fgl_parsing_error
   :members:
```

:::

::::

Python selects the FGL output representation with a keyword-only registered class. The file
supplies the layout arrangement. Pass the exact registered class; subclasses are not supported.

```python
from mnt.pyfiction.layouts import hexagonal_gate_layout
from mnt.pyfiction.layouts.io import read_fgl_layout

layout = read_fgl_layout("layout.fgl", layout_type=hexagonal_gate_layout)
```
