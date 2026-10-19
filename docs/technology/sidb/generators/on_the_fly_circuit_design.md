(on_the_fly_design)=

# On-the-Fly SiDB Circuit Design

(placed-and-routed-layouts)=

## Placed and Routed Layouts

The circuit designer implements each tile of a hexagonal gate-level layout with SiDBs.
The input must be placed and routed with port orientations supported by the Bestagon skeletons.
Convert Cartesian layouts with {ref}`hexagonalization` before designing the SiDB circuit.

The default search uses exhaustive gate design with one canvas SiDB and stops at the first
solution. Crossings and double wires use predefined implementations when possible.
The required number of canvas SiDBs depends on the gates; the example uses three with QuickCell.
The returned layout can be exported as SVG or SiQAD SQD.

The circuit's `timeout` covers all gate designs. Set `design_gate_params.operational_params.timeout` in C++
or `design_gate_timeout` in Python for a shorter per-gate limit. Both use milliseconds; see {ref}`sidb_timeouts` for defaults and limits.

::::{tab-set}
:sync-group: language

:::{tab-item} C++
:sync: cpp

**Header:** `fiction/technology/sidb/generators/on_the_fly_circuit_design.hpp`

```{doxygenclass} fiction::sidb::generators::unsuccessful_gate_design_error
:members:
```

```{doxygenstruct} fiction::sidb::generators::on_the_fly_circuit_design_params
:members:
```

```{doxygenfunction} fiction::sidb::generators::on_the_fly_circuit_design

```

:::

:::{tab-item} Python
:sync: python

The Python call accepts `hexagonal_gate_layout` and returns `sidb_layout`.
The call leaves its input unchanged and releases the GIL during circuit design.
An unsupported gate type or orientation raises `ValueError`; an unsuccessful gate search
raises `RuntimeError`. This interface does not accept defective surfaces or logic networks.

Choose the gate-search algorithm with the `design_mode` keyword, as shown below.
The `PRUNING_ONLY` mode skips operational simulation, so it does not verify the designed gates' functionality.

```python
from mnt.pyfiction.layouts import hexagonal_gate_layout
from mnt.pyfiction.sidb.generators import (
    design_sidb_gates_mode,
    on_the_fly_sidb_circuit_design,
)
from mnt.pyfiction.sidb.io import write_sqd_layout

layout = hexagonal_gate_layout((2, 2, 0), "ROW", "AND")
a = layout.create_pi("a", (0, 0, 0))
b = layout.create_pi("b", (1, 0, 0))
gate = layout.create_and(a, b, (1, 1, 0))
layout.create_po(gate, "f", (0, 2, 0))

circuit = on_the_fly_sidb_circuit_design(
    layout, timeout=60_000, design_mode=design_sidb_gates_mode.QUICKCELL, number_of_canvas_sidbs=3
)
write_sqd_layout(circuit, "and.sqd")
```

```{eval-rst}
.. autoclass:: mnt.pyfiction.sidb.generators.sidb_complex_gate_design_policy
   :members:

.. autofunction:: mnt.pyfiction.sidb.generators.on_the_fly_sidb_circuit_design
```

:::

::::

(defective-surfaces)=

## Defective Surfaces

The C++ interface for defective surfaces requires Z3 and a named technology network (`fiction::tec_nt`).
Convert other network types with `fiction::synthesis::convert_network<fiction::tec_nt>` before circuit design.
The circuit designer excludes gate-tile combinations whose skeletons overlap neutral defects,
then uses `exact` for placement and routing. The gate designer
implements the placed tiles while accounting for the surface's defects.

If gate design fails, the algorithm blacklists the failing gate-tile-port combination and retries
placement and routing. It raises `unsuccessful_pr_error` when no placement is possible.

**Header:** `fiction/technology/sidb/generators/on_the_fly_circuit_design.hpp`

```{doxygenclass} fiction::sidb::generators::unsuccessful_pr_error
:members:
```

```{doxygenstruct} fiction::sidb::generators::on_the_fly_circuit_design_on_defective_surface_params
:members:
```

```{doxygenstruct} fiction::sidb::generators::on_the_fly_circuit_design_on_defective_surface_stats
:members:
```

```{doxygenfunction} fiction::sidb::generators::on_the_fly_circuit_design_on_defective_surface

```
