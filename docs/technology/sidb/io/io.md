# SiDB I/O

## Dot Layouts

::::{tab-set}
:sync-group: language

:::{tab-item} C++
:sync: cpp

**Header:** `fiction/technology/sidb/io/read_sqd_layout.hpp`

```{doxygenfile} fiction/technology/sidb/io/read_sqd_layout.hpp
:sections: func
```

```{doxygenclass} fiction::sidb::io::sqd_parsing_error
```

:::

:::{tab-item} Python
:sync: python

```{eval-rst}

.. autofunction:: mnt.pyfiction.sidb.io.read_sqd_layout

.. autoclass:: mnt.pyfiction.sidb.io.sqd_parsing_error
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

```{doxygenfile} fiction/technology/sidb/io/read_surface_defects.hpp
:sections: func
```

```{doxygenclass} fiction::sidb::io::unsupported_defect_index_exception
```

```{doxygenclass} fiction::sidb::io::missing_position_exception
```

:::

:::{tab-item} Python
:sync: python

```{eval-rst}

.. autofunction:: mnt.pyfiction.sidb.io.read_surface_defects

.. autoclass:: mnt.pyfiction.sidb.io.unsupported_defect_index_exception
    :members:

.. autoclass:: mnt.pyfiction.sidb.io.missing_position_exception
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

```{doxygenfunction} fiction::sidb::io::write_sqd_layout(const layout& lyt, std::ostream& os, utils::progress_callback on_progress = {})
```

```{doxygenfunction} fiction::sidb::io::write_sqd_layout(const layout& lyt, const std::string_view& filename, utils::progress_callback on_progress = {})
```

:::

:::{tab-item} Python
:sync: python

```{eval-rst}

.. autofunction:: mnt.pyfiction.sidb.io.write_sqd_layout

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

.. autoclass:: mnt.pyfiction.sidb.io.write_sidb_layout_svg_params
   :members:

.. autoclass:: mnt.pyfiction.sidb.io.sidb_lattice_mode
   :members:

.. autoclass:: mnt.pyfiction.sidb.io.color_mode
   :members:

.. autofunction:: mnt.pyfiction.sidb.io.write_sidb_layout_svg

.. autofunction:: mnt.pyfiction.sidb.io.write_sidb_layout_svg_to_string

```

:::

::::

## Layout Printing

::::{tab-set}
:sync-group: language

:::{tab-item} C++
:sync: cpp

**Header:** `fiction/technology/sidb/io/print_sidb_layout.hpp`

```{doxygenfunction} fiction::sidb::io::print_sidb_layout(std::ostream& os, const layout& lyt, const bool lat_color = true, const bool crop_layout = false, const bool draw_lattice = true)
```

```{doxygenfunction} fiction::sidb::io::print_sidb_layout(std::ostream& os, const layout& lyt, const charge_distribution& cd, const bool lat_color = true, const bool crop_layout = false, const bool draw_lattice = true)
```

:::

:::{tab-item} Python
:sync: python

```{eval-rst}
.. autofunction:: mnt.pyfiction.sidb.io.print_sidb_layout
```

:::

::::
