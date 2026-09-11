(on_the_fly_design)=

# On-the-fly SiDB Circuit Design

## Placed and routed layouts

The circuit designer implements each tile of a hexagonal gate-level layout with SiDBs.
The input must be placed and routed with port orientations supported by the Bestagon skeletons.
Convert Cartesian layouts with {ref}`hexagonalization` before designing the SiDB circuit.

The default search uses exhaustive gate design with one canvas SiDB and stops at the first
solution. Crossings and double wires use predefined implementations when possible.
The required number of canvas SiDBs depends on the gates; the example uses three with QuickCell.
The search has no timeout or cancellation parameter; applications that require cancellation
must run the call in a separate process. The returned layout can be exported as SVG or SiQAD SQD.

::::{tab-set}
:sync-group: language

:::{tab-item} C++
:sync: cpp

**Header:** `fiction/technology/sidb/generators/on_the_fly_circuit_design.hpp`

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

Choose the gate-search algorithm through `design_gate_params.design_mode`, as shown below.
The `PRUNING_ONLY` mode skips operational simulation, so it does not verify the designed gates' functionality.

```python
from mnt.pyfiction import (
    design_sidb_gates_mode,
    hexagonal_gate_layout,
    on_the_fly_sidb_circuit_design,
    on_the_fly_sidb_circuit_design_params,
    write_sqd_layout,
)

layout = hexagonal_gate_layout((2, 2, 0), "ROW", "AND")
a = layout.create_pi("a", (0, 0, 0))
b = layout.create_pi("b", (1, 0, 0))
gate = layout.create_and(a, b, (1, 1, 0))
layout.create_po(gate, "f", (0, 2, 0))

params = on_the_fly_sidb_circuit_design_params()
gates = params.sidb_on_the_fly_gate_library_parameters.design_gate_params
gates.design_mode = design_sidb_gates_mode.QUICKCELL
gates.number_of_canvas_sidbs = 3
circuit = on_the_fly_sidb_circuit_design(layout, params)
write_sqd_layout(circuit, "and.sqd")
```

```{eval-rst}
.. autoclass:: mnt.pyfiction.sidb_complex_gate_design_policy
   :members:

.. autoclass:: mnt.pyfiction.sidb_on_the_fly_gate_library_params
   :members:

.. autoclass:: mnt.pyfiction.on_the_fly_sidb_circuit_design_params
   :members:

.. autofunction:: mnt.pyfiction.on_the_fly_sidb_circuit_design
```

:::

::::

## Defective surfaces

The C++ interface for defective surfaces requires Z3. It excludes gate-tile combinations whose
skeletons overlap neutral defects, then uses `exact` for placement and routing. The gate designer
implements the placed tiles while accounting for the surface's defects.

If gate design fails, the algorithm blacklists the failing gate-tile-port combination and retries
placement and routing. It raises `unsuccessful_pr_error` when no placement is possible.

**Header:** `fiction/technology/sidb/generators/on_the_fly_circuit_design.hpp`

```{doxygenstruct} fiction::sidb::generators::on_the_fly_circuit_design_on_defective_surface_params
:members:
```

```{doxygenstruct} fiction::sidb::generators::on_the_fly_circuit_design_on_defective_surface_stats
:members:
```

```{doxygenfunction} fiction::sidb::generators::on_the_fly_circuit_design_on_defective_surface

```
