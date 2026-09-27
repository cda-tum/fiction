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
