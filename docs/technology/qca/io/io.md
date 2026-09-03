# QCA I/O

## QCADesigner

::::{tab-set}
:sync-group: language

:::{tab-item} C++
:sync: cpp

**Header:** `fiction/technology/qca/io/write_qca_layout.hpp`

```{doxygenstruct} fiction::qca::io::write_qca_layout_params
:members:
```

```{doxygenfunction} fiction::qca::io::write_qca_layout(const Lyt& lyt, std::ostream& os, write_qca_layout_params ps = {})

```

```{doxygenfunction} fiction::qca::io::write_qca_layout(const Lyt& lyt, const std::string_view& filename, write_qca_layout_params ps = {})

```

:::

:::{tab-item} Python
:sync: python

```{autoclass} mnt.pyfiction.write_qca_layout_params
:members:
```

```{autofunction} mnt.pyfiction.write_qca_layout

```

:::

::::

## QCA-STACK

::::{tab-set}
:sync-group: language

:::{tab-item} C++
:sync: cpp

**Header:** `fiction/technology/qca/io/write_fqca_layout.hpp`

```{doxygenstruct} fiction::qca::io::write_fqca_layout_params
:members:
```

```{doxygenfunction} fiction::qca::io::write_fqca_layout(const Lyt& lyt, std::ostream& os, write_fqca_layout_params ps = {})

```

```{doxygenfunction} fiction::qca::io::write_fqca_layout(const Lyt& lyt, const std::string_view& filename, write_fqca_layout_params ps = {})

```

```{doxygenclass} fiction::qca::io::out_of_cell_names_exception

```

:::

:::{tab-item} Python
:sync: python

```{autoclass} mnt.pyfiction.write_fqca_layout_params
:members:
```

```{autofunction} mnt.pyfiction.write_fqca_layout

```

```{autoclass} mnt.pyfiction.out_of_cell_names_exception
:members:
```

:::

::::

::::{tab-set}
:sync-group: language

:::{tab-item} C++
:sync: cpp

**Header:** `fiction/technology/qca/io/read_fqca_layout.hpp`

```{doxygenfunction} fiction::qca::io::read_fqca_layout(std::istream& is, const std::string_view& name = "")

```

```{doxygenfunction} fiction::qca::io::read_fqca_layout(const std::string_view& filename, const std::string_view& name = "")

```

```{doxygenclass} fiction::qca::io::unsupported_character_exception
:members:
```

```{doxygenclass} fiction::qca::io::undefined_cell_label_exception
:members:
```

```{doxygenclass} fiction::qca::io::unrecognized_cell_definition_exception
:members:
```

:::

:::{tab-item} Python
:sync: python

```{autofunction} mnt.pyfiction.read_fqca_layout

```

```{autoclass} mnt.pyfiction.unsupported_character_exception
:members:
```

```{autoclass} mnt.pyfiction.undefined_cell_label_exception
:members:
```

```{autoclass} mnt.pyfiction.unrecognized_cell_definition_exception
:members:
```

:::

::::

## SVG Images

::::{tab-set}
:sync-group: language

:::{tab-item} C++
:sync: cpp

**Header:** `fiction/technology/qca/io/write_qca_layout_svg.hpp`

```{doxygenstruct} fiction::qca::io::write_qca_layout_svg_params
:members:
```

```{doxygenfunction} fiction::qca::io::write_qca_layout_svg(const Lyt& lyt, std::ostream& os, const write_qca_layout_svg_params& ps = {})

```

```{doxygenfunction} fiction::qca::io::write_qca_layout_svg(const Lyt& lyt, const std::string_view& filename, const write_qca_layout_svg_params& ps = {})

```

```{doxygenclass} fiction::qca::io::unsupported_cell_type_exception

```

```{doxygenfunction} fiction::qca::io::write_mol_qca_layout_svg(const Lyt& lyt, std::ostream& os, const write_qca_layout_svg_params& ps = {})

```

```{doxygenfunction} fiction::qca::io::write_mol_qca_layout_svg(const Lyt& lyt, const std::string_view& filename, const write_qca_layout_svg_params& ps = {})

```

:::

:::{tab-item} Python
:sync: python

```{autoclass} mnt.pyfiction.write_qca_layout_svg_params
:members:
```

```{autofunction} mnt.pyfiction.write_qca_layout_svg

```

```{autofunction} mnt.pyfiction.write_mol_qca_layout_svg

```

:::

::::
