# iNML

```{toctree}
:maxdepth: 1

io/io
```

## in-plane Nanomagnet Logic (iNML) Layout

An iNML layout places in-plane nanomagnets on a planar Cartesian grid. Each position holds a magnet type and, for
inputs and outputs, a name. Clock zones are tiles of magnets. Signal crossings are coplanar cross-wire magnets, so the
layout has no crossing layer. See {doc}`../../layouts/cell_grid` for the storage and the clock zones that iNML shares
with other technologies.

::::{tab-set}
:sync-group: language

:::{tab-item} C++
:sync: cpp

**Header:** `fiction/technology/inml/layout.hpp`

```{doxygenenum} fiction::inml::magnet_type
```

```{doxygenclass} fiction::inml::layout
:members:
```

:::

:::{tab-item} Python
:sync: python

```{eval-rst}
.. autoclass:: mnt.pyfiction.inml_magnet_type
   :members:

.. autoclass:: mnt.pyfiction.inml_layout
   :members:
```

:::

::::

## iNML ToPoliNano Library

**Header:** `fiction/technology/inml/topolinano_library.hpp`

```{doxygenclass} fiction::inml::topolinano_library
:members:
```

## MagCAD Magnet Count

**Header:** `fiction/technology/inml/magcad_magnet_count.hpp`

```{doxygenfunction} fiction::inml::magcad_magnet_count

```
