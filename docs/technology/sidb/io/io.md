# SiDB I/O

## Cell-level Layouts

::::{tab-set}
:sync-group: language

:::{tab-item} C++
:sync: cpp

**Header:** `fiction/technology/sidb/io/read_sqd_layout.hpp`

```{doxygenfunction} fiction::sidb::io::read_sqd_layout(std::istream& is, const std::string_view& name = "")
```

```{doxygenfunction} fiction::sidb::io::read_sqd_layout(const std::string_view& filename, const std::string_view& name = "")
```

```{doxygenclass} fiction::sidb::io::sqd_parsing_error
```

:::

:::{tab-item} Python
:sync: python

```{eval-rst}

.. autofunction:: mnt.pyfiction.read_sqd_layout

.. autoclass:: mnt.pyfiction.sqd_parsing_error
    :members:

```

:::

::::

## SiDB Surface Defects

::::{tab-set}
:sync-group: language

:::{tab-item} C++
:sync: cpp

**Header:** `fiction/technology/sidb/io/read_surface_defects.hpp`

```{doxygenfunction} fiction::sidb::io::read_surface_defects(std::istream& is, const std::string_view& name = "")
```

```{doxygenfunction} fiction::sidb::io::read_surface_defects(const std::string_view& filename, const std::string_view& name = "")
```

```{doxygenclass} fiction::sidb::io::unsupported_defect_index_exception
```

```{doxygenclass} fiction::sidb::io::missing_position_exception
```

:::

:::{tab-item} Python
:sync: python

```{eval-rst}

.. autofunction:: mnt.pyfiction.read_surface_defects

.. autoclass:: mnt.pyfiction.unsupported_defect_index_exception
    :members:

.. autoclass:: mnt.pyfiction.missing_position_exception
    :members:

```

:::

::::

## SiQAD

::::{tab-set}
:sync-group: language

:::{tab-item} C++
:sync: cpp

**Header:** `fiction/technology/sidb/io/write_sqd_layout.hpp`

```{doxygenfunction} fiction::sidb::io::write_sqd_layout(const layout& lyt, std::ostream& os)
```

```{doxygenfunction} fiction::sidb::io::write_sqd_layout(const layout& lyt, const std::string_view& filename)
```

:::

:::{tab-item} Python
:sync: python

```{eval-rst}

.. autofunction:: mnt.pyfiction.write_sqd_layout

```

:::

::::

## SVG Images

::::{tab-set}
:sync-group: language

:::{tab-item} C++
:sync: cpp

**Header:** `fiction/technology/sidb/io/write_sidb_layout_svg.hpp`

```{doxygenstruct} fiction::sidb::io::write_sidb_layout_svg_params
:members:
```

```{doxygenfunction} fiction::sidb::io::write_sidb_layout_svg(const layout& lyt, std::ostream& os, const write_sidb_layout_svg_params& ps = {})
```

```{doxygenfunction} fiction::sidb::io::write_sidb_layout_svg(const layout& lyt, const std::string_view& filename, const write_sidb_layout_svg_params& ps = {})
```

```{doxygenfunction} fiction::sidb::io::write_sidb_layout_svg(const layout& lyt, const charge_distribution& cd, std::ostream& os, const write_sidb_layout_svg_params& ps = {})
```

```{doxygenfunction} fiction::sidb::io::write_sidb_layout_svg(const layout& lyt, const charge_distribution& cd, const std::string_view& filename, const write_sidb_layout_svg_params& ps = {})
```

:::

:::{tab-item} Python
:sync: python

```{eval-rst}

.. autoclass:: mnt.pyfiction.write_sidb_layout_svg_params

.. autofunction:: mnt.pyfiction.write_sidb_layout_svg

.. autofunction:: mnt.pyfiction.write_sidb_layout_svg_to_string

```

:::

::::

## Layout Printing

**Header:** `fiction/technology/sidb/io/print_sidb_layout.hpp`

```{doxygenfunction} fiction::sidb::io::print_sidb_layout(std::ostream& os, const layout& lyt, const bool lat_color = true, const bool crop_layout = false, const bool draw_lattice = true)
```

```{doxygenfunction} fiction::sidb::io::print_sidb_layout(std::ostream& os, const layout& lyt, const charge_distribution& cd, const bool lat_color = true, const bool crop_layout = false, const bool draw_lattice = true)
```
