# Hexagonal Layout

Coordinate system that represents a hexagonal grid of compile-time coordinate types. In contrast to Cartesian layouts,
the faces of hexagonal layouts can be arranged in different ways. The most obvious are the distinction between the
orientation of the hexagons that can either be pointy-top up or flat-top up.

::::{tab-set}
:sync-group: language

:::{tab-item} C++
:sync: cpp

**Header:** `fiction/layouts/hexagonal_layout.hpp`

```{doxygenstruct} fiction::layouts::pointy_top_hex

```

```{doxygenstruct} fiction::layouts::flat_top_hex

```

Furthermore, when assuming rectangular-ish layouts, either odd or even rows or columns can be shifted inwards,
yielding a total of four different orientations.

```{doxygenstruct} fiction::layouts::odd_row_hex

```

```{doxygenstruct} fiction::layouts::even_row_hex

```

```{doxygenstruct} fiction::layouts::odd_column_hex

```

```{doxygenstruct} fiction::layouts::even_column_hex

```

The hexagonal layout supports all of them via template parameters.

```{doxygenclass} fiction::layouts::hexagonal_layout
:members:
```

:::

:::{tab-item} Python
:sync: python

```{eval-rst}
.. autoclass:: mnt.pyfiction.hexagonal_layout
   :members:
```

:::

::::
