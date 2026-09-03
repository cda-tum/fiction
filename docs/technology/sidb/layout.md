# SiDB Layout

An SiDB layout places SiDBs and surface defects on the lattice sites of one hydrogen-passivated silicon surface. The
surface is a Bravais lattice with a two-site basis: two lattice vectors `a1` and `a2` plus the two basis sites
of the unit cell, exactly as SiQAD stores it. Site `(x, y, z)` lies at `x * a1 + y * a2 + basis[z]`. The
H-Si(100)-2x1 and H-Si(111)-1x1 reconstructions are predefined; any other reconstruction with a two-site basis can be
described by setting the vectors directly.

## Lattice

::::{tab-set}
:sync-group: language

:::{tab-item} C++
:sync: cpp

**Header:** `fiction/technology/sidb/lattice.hpp`

```{doxygenstruct} fiction::sidb::lattice_site
:members:
```

```{doxygenfunction} fiction::sidb::row_of
```

```{doxygenfunction} fiction::sidb::site_at_row
```

```{doxygenfunction} fiction::sidb::sites_in_area
```

```{doxygenfunction} fiction::sidb::random_site_in_area
```

```{doxygenstruct} fiction::sidb::lattice
:members:
```

:::

:::{tab-item} Python
:sync: python

```{eval-rst}
.. autoclass:: mnt.pyfiction.lattice_site
    :members:
.. autofunction:: mnt.pyfiction.row_of
.. autofunction:: mnt.pyfiction.site_at_row
.. autofunction:: mnt.pyfiction.sites_in_area
.. autoclass:: mnt.pyfiction.lattice
    :members:

```

:::

::::

## Layout

::::{tab-set}
:sync-group: language

:::{tab-item} C++
:sync: cpp

**Header:** `fiction/technology/sidb/layout.hpp`

```{doxygenclass} fiction::sidb::layout
:members:
```

:::

:::{tab-item} Python
:sync: python

```{eval-rst}
.. autoclass:: mnt.pyfiction.sidb_layout
    :members:

```

:::

::::

## Conversion from Cell-level Layouts

Placement and routing produce Cartesian SiDB cell-level layouts whose rows count single SiDB rows. These functions
map such layouts and their coordinates onto lattice sites.

**Header:** `fiction/technology/sidb/cell_level_layout_conversion.hpp`

```{doxygenfunction} fiction::sidb::to_lattice_site
```

```{doxygenfunction} fiction::sidb::to_cube
```

```{doxygenfunction} fiction::sidb::lattice_of
```

```{doxygenfunction} fiction::sidb::to_sidb_layout(const CellLyt& lyt, const lattice& lat)
```

```{doxygenfunction} fiction::sidb::to_sidb_layout(const CellLyt& lyt)
```
