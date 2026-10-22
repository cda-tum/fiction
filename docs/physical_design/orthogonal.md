(ortho)=

# Scalable Orthogonal Physical Design

Utilizes approximations to the graph-theoretical problem of _Orthogonal Graph Drawing_ to generate FCN gate-level
layouts from named technology networks. Gates may have at most two nonconstant fanins; fanout normalization produces
a 3-graph. The output layout is 2DDWave-clocked and has a large area overhead.

::::{tab-set}
:sync-group: language

:::{tab-item} C++
:sync: cpp

**Header:** `fiction/physical_design/orthogonal.hpp`

```{doxygenstruct} fiction::physical_design::orthogonal_physical_design_params
:members:
```

```{doxygenstruct} fiction::physical_design::orthogonal_physical_design_stats
:members:
```

```{doxygenfunction} fiction::physical_design::orthogonal(const tec_nt& ntk, orthogonal_physical_design_params ps = {}, orthogonal_physical_design_stats* pst = nullptr)

```

:::

:::{tab-item} Python
:sync: python

Use keyword-only `layout_type` to select `cartesian_gate_layout` (the default) or
`hexagonal_gate_layout`. Pass the exact registered class; subclasses and shifted Cartesian
layouts raise `TypeError`. Hexagonal layouts require `layout_arrangement`.

```{eval-rst}
.. autoclass:: mnt.pyfiction.physical_design.num_clks
   :members:

.. autoclass:: mnt.pyfiction.physical_design.orthogonal_stats
   :members:
   :undoc-members:

.. autofunction:: mnt.pyfiction.physical_design.orthogonal
```

:::

::::
