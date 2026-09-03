(hexagonalization)=

# Mapping Cartesian to Hexagonal Layouts

This algorithm maps Cartesian 2DDWave-clocked layouts used for Quantum-dot Cellular Automata (QCA) to hexagonal row-clocked layouts,
which are suitable for Silicon Dangling Bonds (SiDBs).

The respective coordinates on the hexagonal grid are calculated as follows:

$$
y_{\mathit{hex}} = x_{\mathit{Cart}} + y_{\mathit{Cart}}
$$

$$
x_{\mathit{hex}} = x_{\mathit{Cart}} + \left \lceil{\left \lfloor{\frac{h}{2}} \right \rfloor} - \frac{y_{\mathit{hex}}}{2}\right \rceil
$$

:::{figure} /_static/hexagonalization.svg
:alt: Mapping a Cartesian 2DDWave-clocked layout onto a hexagonal row-clocked layout
:width: 600
:::

::::{tab-set}
:sync-group: language

:::{tab-item} C++
:sync: cpp

**Header:** `fiction/physical_design/hexagonalization.hpp`

```{doxygenstruct} fiction::physical_design::hexagonalization_params
:members:
```

```{doxygenstruct} fiction::physical_design::hexagonalization_stats
:members:
```

```{doxygenfunction} fiction::physical_design::hexagonalization

```

:::

:::{tab-item} Python
:sync: python

```{eval-rst}
.. autoclass:: mnt.pyfiction.hexagonalization_params
   :members:

.. autoclass:: mnt.pyfiction.hexagonalization_stats
   :members:

.. autofunction:: mnt.pyfiction.hexagonalization
```

:::

::::
