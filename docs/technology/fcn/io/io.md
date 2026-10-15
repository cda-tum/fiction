# FCN I/O

## MagCAD & SCERPA

::::{tab-set}
:sync-group: language

:::{tab-item} C++
:sync: cpp

**Header:** `fiction/technology/fcn/io/write_qll_layout.hpp`

```{doxygenfunction} fiction::fcn::io::write_qll_layout(const Lyt& lyt, std::ostream& os, utils::progress_callback on_progress = {})

```

```{doxygenfunction} fiction::fcn::io::write_qll_layout(const Lyt& lyt, const std::string_view& filename, utils::progress_callback on_progress = {})

```

:::

:::{tab-item} Python
:sync: python

```{eval-rst}
.. autofunction:: mnt.pyfiction.fcn.io.write_qll_layout
```

:::

::::

## Unsupported Cell Types

**Header:** `fiction/technology/fcn/io/cell_layout_svg.hpp`

```{doxygenclass} fiction::fcn::io::unsupported_cell_type_exception
:members:
```
