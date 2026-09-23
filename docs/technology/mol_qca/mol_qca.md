(molQCA)=

# molQCA

```{toctree}
:maxdepth: 1

io/io
```

## Molecular Quantum-dot Cellular Automata (molQCA) Layout

Molecular QCA implements the QCA paradigm with molecules. A molQCA layout places molecular cells on a planar Cartesian
grid. Each cell has a type and, for inputs and outputs, a name. A regular cell names its own clock phase through its
type, `NORMAL1` to `NORMAL4`, which is how the SCERPA simulator addresses clock regions and how SIM(7) gates arrange
several clock regions in one tile. The layout therefore has no tile-based clock zones, no cell modes, and no crossing
layer. See {doc}`../../layouts/cell_grid` for the cell storage that molQCA shares with other technologies.

::::{tab-set}
:sync-group: language

:::{tab-item} C++
:sync: cpp

**Header:** `fiction/technology/mol_qca/layout.hpp`

```{doxygenenum} fiction::mol_qca::cell_type
```

```{doxygenfunction} fiction::mol_qca::clock_number
```

```{doxygenfunction} fiction::mol_qca::is_normal
```

```{doxygenfunction} fiction::mol_qca::is_constant
```

```{doxygenclass} fiction::mol_qca::layout
:members:
```

:::

:::{tab-item} Python
:sync: python

```{eval-rst}
.. autoclass:: mnt.pyfiction.mol_qca_cell_type
   :members:

.. autofunction:: mnt.pyfiction.mol_qca_clock_number

.. autoclass:: mnt.pyfiction.mol_qca_layout
   :members:
```

:::

::::

## SIM(7) Library

**Header:** `fiction/technology/mol_qca/sim7_mol_library.hpp`

```{doxygenclass} fiction::mol_qca::sim7_mol_library
:members:
```
