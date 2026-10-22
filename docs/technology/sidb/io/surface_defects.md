# SiDB Surface Defects

::::{tab-set}
:sync-group: language

:::{tab-item} C++
:sync: cpp

**Header:** `fiction/technology/sidb/io/read_surface_defects.hpp`

```{doxygenfunction} fiction::sidb::io::read_surface_defects(std::istream& is, const std::string_view& name)
```

```{doxygenfunction} fiction::sidb::io::read_surface_defects(const std::string_view& filename, const std::string_view& name)
```

```{doxygenclass} fiction::sidb::io::unsupported_defect_index_exception
:members:
```

```{doxygenclass} fiction::sidb::io::missing_position_exception
:members:
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
