(exact)=

# SMT-based Exact Physical Design

Utilizes the SMT solver [Z3](https://github.com/Z3Prover/z3) to generate minimal FCN gate-level layouts from logic
network specifications under constraints. This approach finds exact results but has a large runtime overhead.

::::{tab-set}
:sync-group: language

:::{tab-item} C++
:sync: cpp

**Header:** `fiction/physical_design/exact.hpp`

```{doxygenstruct} fiction::physical_design::exact_physical_design_params
:members:
```

```{doxygenstruct} fiction::physical_design::exact_physical_design_stats
:members:
```

```{doxygenfunction} fiction::physical_design::exact(const Ntk& ntk, const exact_physical_design_params& ps = {}, exact_physical_design_stats *pst = nullptr)

```

```{doxygenfunction} fiction::physical_design::exact_with_blacklist

```

:::

:::{tab-item} Python
:sync: python

```{eval-rst}
.. autofunction:: mnt.pyfiction.physical_design.exact
```

:::

::::

Python `exact` and `orthogonal` select their output with the keyword-only `layout_type` class,
which defaults to `cartesian_gate_layout`. `exact` also supports shifted Cartesian and hexagonal
layouts; `orthogonal` also supports hexagonal layouts. Non-Cartesian layouts require an explicit
`layout_arrangement`. Pass the exact registered class; subclasses are not supported.

```python
from mnt.pyfiction.layouts import arrangement, hexagonal_gate_layout
from mnt.pyfiction.networks.io import read_network
from mnt.pyfiction.physical_design import exact, exact_params

network = read_network("network.v")
params = exact_params()
params.layout_arrangement = arrangement.EVEN_ROW
layout = exact(network, params, layout_type=hexagonal_gate_layout)
```
