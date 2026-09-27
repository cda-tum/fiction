# Python FCN workflows

`mnt.pyfiction` groups tools by their FCN domain. Import the public modules; the
`_native` package is an implementation detail. Python types use PascalCase names.

## Layouts and geometry

Construct a gate or cell layout directly. Gate layouts provide geometry, clocking,
gates, wires, synchronization, and obstructions on the same object. QCA, molecular
QCA, and iNML layouts provide their technology's cells and geometry.

```python
from mnt.pyfiction.layouts import CartesianGateLayout
from mnt.pyfiction.layouts.coords import OffsetCoordinate

layout = CartesianGateLayout((4, 4), "2DDWave")
layout.name = "example"
origin = OffsetCoordinate(0, 0)
source = layout.create_pi("a", origin)
output = layout.create_po(source, "y", (1, 0))
assert output == (1, 0)
assert layout.is_within_bounds(origin)
assert layout.name == "example"
```

Gate creation takes and returns coordinates. Use tuples for coordinate inputs, or immutable `OffsetCoordinate` and
`CubeCoordinate` values as dictionary keys. Geometry-only grids, packed integer
coordinate constructors, and coordinate area and volume helpers are not public
Python APIs. A layout's `area()` measures its grid area; physical FCN area is a
separate technology-dependent quantity.

Choose the concrete topology explicitly:

| Geometry | Python type |
| --- | --- |
| Cartesian | `CartesianGateLayout` |
| Odd-column Cartesian | `ShiftedCartesianGateLayout` |
| Even-column Cartesian | `EvenColumnCartesianGateLayout` |
| Odd-row Cartesian | `OddRowCartesianGateLayout` |
| Even-row Cartesian | `EvenRowCartesianGateLayout` |
| Even-row hexagonal | `HexagonalGateLayout` |
| Odd-row hexagonal | `OddRowHexGateLayout` |
| Odd-column hexagonal | `OddColumnHexGateLayout` |
| Even-column hexagonal | `EvenColumnHexGateLayout` |

An algorithm's signature lists the topologies it supports.

## Boolean specifications

`TruthTable` provides validated constructors for binary text, hexadecimal text,
and kitty Boolean expressions. Binary text determines the variable count;
hexadecimal text and expressions require an explicit count.

```python
from mnt.pyfiction.synthesis import TruthTable, standard_functions

majority = TruthTable.from_expression("<abc>", num_vars=3)
assert majority.to_binary() == "11101000"
assert TruthTable.from_hex("e8", num_vars=3).to_binary() == majority.to_binary()
sum_output, carry_output = standard_functions("half_adder")
assert [sum_output.to_hex(), carry_output.to_hex()] == ["6", "8"]
```

`standard_functions()` returns the named catalog. Each value is a list of fresh
truth tables in specification output order. Use `standard_functions("and")[0]`
for a single output and pass the whole list to algorithms that consume a
multi-output specification. Unknown names raise `ValueError`.

## Network inspection

`TechnologyNetwork`, `AigNetwork`, `XagNetwork`, and `MigNetwork` expose node
inspection. `nodes()`, `gates()`, and `pis()` return node identifiers; `pos()` and
`fanins(node)` return immutable `Signal` values. A signal's `node` identifies its
source, and `complemented` records whether the edge inverts the Boolean value.
Use both fields when interpreting an AIG, XAG, or MIG. `len(network)` returns the
node count, and `network.name` reads or sets the network name.

## Migrating data objects

Replace snake_case class imports with the corresponding PascalCase type. The
technology layout names are `QCALayout`, `MolecularQCALayout`, and `INMLLayout`.
Replace a geometry-only layout with the corresponding gate layout. Replace
`create_<name>_tt()` with `standard_functions("<name>")`; select element zero
only for a single-output specification. Network connectivity consumers must read
`Signal.node` and preserve `Signal.complemented`.

## Placement and routing

Placement returns a `LayoutResult` with `.layout` and `.stats`. Select a concrete
output topology with `layout_type` and pass options with `params`. `orthogonal`
supports Cartesian and all four hexagonal layouts. `exact` supports all nine gate
topologies when `exact_available()` reports Z3 support. Exact and graph-oriented
searches return `layout=None` when their bounds or timeout yield no solution;
the result still carries statistics. Native progress callbacks and their exception
behavior apply to the public workflows.

```python
from mnt.pyfiction.layouts import HexagonalGateLayout
from mnt.pyfiction.physical_design import orthogonal

# network is a TechnologyNetwork loaded from a logic specification.
result = orthogonal(network, layout_type=HexagonalGateLayout)
layout = result.layout
print(result.stats)
```

`hexagonalization` also returns a layout and statistics. `post_layout_optimization`
and `wiring_reduction` optimize an independent copy; use the result's `.layout`.

`physical_design.routing` provides A*, Yen's algorithm, path enumeration, distance
metrics, graph-coloring routing, `place`, `reserve_input_nodes`, `route_path`, and
routing extraction and clearing. These primitives support Cartesian, odd-column
Cartesian, and even-row hexagonal gate layouts. `place` accepts incoming coordinates
and returns the placed gate's coordinate. `route_path` requires at least two in-bounds
coordinates with gates or wires at both endpoints.

## Layout files

`layouts.io.read_fgl_layout(path, layout_type=HexagonalGateLayout)` replaces the
separate topology-specific readers. The file must match the selected topology.
Readers and writers accept strings and `pathlib.Path` values. `write_fgl_layout`
and `write_dot_layout` retain progress callbacks; DOT output also accepts
`clock_colors` and `indexes` as keyword options.

## SiDB simulation

`sidb` provides `SiDBLayout`, immutable `LatticeSite` coordinates, `Lattice`,
`DotTag`, `Defect`, `DefectType`, `ChargeState`, and `SimulationParams`.
`sidb.simulation` exposes `quickexact`, `quicksim`, and
`exhaustive_ground_state_simulation` directly, with `clustercomplete` available
in builds with ALGLIB. Engine options use keyword-only `params`; the named engine
parameter types retain physical settings, external potentials, and progress callbacks.
QuickSim can return `None` when it finds no valid state.

```python
from mnt.pyfiction.sidb import ChargeState, DotTag, LatticeSite, SiDBLayout
from mnt.pyfiction.sidb.simulation import PotentialLandscape, quickexact

layout = SiDBLayout()
layout.assign_sidb(LatticeSite(0, 0), DotTag.NORMAL)
layout.assign_sidb(LatticeSite(3, 0), DotTag.NORMAL)
result = quickexact(layout)
for charges in result.ground_states():
    print(charges.energy, charges[LatticeSite(0, 0)])

landscape = PotentialLandscape(layout)
custom = landscape.evaluate([ChargeState.NEGATIVE, ChargeState.NEUTRAL])
print(custom.energy, landscape.is_physically_valid(custom))
```

`SimulationResult` and `ChargeDistribution` are read-only. Iterate a result to
read its distributions and a distribution to read its charge states in raster
order. `.sites` and `.charge_states` return list snapshots; result layout and
parameter properties also return independent copies. Use `PotentialLandscape`
to evaluate custom states with consistent energy, including defects and external
potentials. Supply exactly one negative, neutral, or positive state per site in
`landscape.sites` order. Evaluation does not require physical validity; call
`is_physically_valid` to check that condition. Distances, local potentials, and
charge-transition thresholds remain available for physical-model inspection.

Replace imports from `sidb.model` with `sidb`, and imports from
`sidb.simulation.engines` with `sidb.simulation`. Replace direct distribution
construction and mutation with `PotentialLandscape.evaluate(states)`, `energy()`
with `.energy`, and `groundstates()` with `ground_states()`.
