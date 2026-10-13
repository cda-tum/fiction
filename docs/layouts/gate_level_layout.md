# Gate-level Layout

`gate_level_layout<CoordinateLayout>` places Boolean gates and wires on a coordinate layout.
The layout owns its clocking scheme, synchronization delays, and persistent obstructions.
Tile operations are available directly. Geometry remains a template argument; clocking
schemes and obstruction assignments do not change the layout type.

The layout represents logic placement and routing independently of a cell technology.
A gate library supplies physical cell implementations and tile dimensions.

Each gate, wire, input, and output is a placed object with a generation-checked `object_id`.
Coordinates describe positions; IDs describe objects and their single output; an `input_port`
names the object and the truth-table argument where a connection enters. Wires and fanouts
are explicit objects. An empty layout contains no objects or implicit constants.

`is_wire`, `foreach_wire`, and `num_wires` identify physical wire objects and exclude PIs and POs.
Python's `wires()` returns the same objects. `is_buf` identifies the identity function, including terminals;
`is_fanout` identifies identity objects that drive more than one input port. `num_gates` excludes wires and terminals.

Use `find_object(tile)` to look up an occupied coordinate and `get_tile(id)` to read an object's
position. Moving an object preserves its ID and connections. Creation and movement reject an
occupied coordinate. Removing an object disconnects its destinations without renumbering their
input ports. IDs are local to a layout; removing and recreating an object invalidates its old ID.

Editing permits disconnected input ports, cycles, and placement outside the frame. Design-rule
checks validate adjacency, clocking, and frame membership separately. Algorithms that need a
`mockturtle` network use `fiction::networks::extract_layout_network`: extraction retains every PI
in declared order and visits only output dependency cones. A missing input or cycle in an output
cone raises an error. Unused PIs and disconnected logic outside those cones do not prevent extraction.

::::{tab-set}
:sync-group: language

:::{tab-item} C++
:sync: cpp

**Header:** `fiction/layouts/gate_level_layout.hpp`

```{doxygenclass} fiction::layouts::gate_level_layout
:members:
```

:::

:::{tab-item} Python
:sync: python

```{eval-rst}
.. autoclass:: mnt.pyfiction.layouts.cartesian_gate_layout
   :members:

.. autoclass:: mnt.pyfiction.layouts.hexagonal_gate_layout
   :members:
```

:::

::::

## Construction and capabilities

::::{tab-set}
:sync-group: language

:::{tab-item} C++
:sync: cpp

```cpp
using layout = fiction::layouts::gate_level_layout<fiction::layouts::cartesian_layout>;
layout lyt{{4, 4}, fiction::layouts::clocking::twoddwave()};
lyt.assign_synchronization_element({1, 1}, 2);
lyt.obstruct_coordinate({2, 2});
const auto input = lyt.create_pi("a", {0, 0});
const auto wire = lyt.create_buf(input, {1, 0});
lyt.create_po(wire, "f", {2, 0});
lyt.move_object(wire, {1, 1});
lyt.disconnect({wire, 0});
lyt.connect(input, {wire, 0});
auto independent = lyt.clone();
```

:::

:::{tab-item} Python
:sync: python

```python
from mnt.pyfiction.layouts import LayoutInputPort, cartesian_gate_layout

lyt = cartesian_gate_layout((4, 4), "2DDWave")
lyt.assign_synchronization_element((1, 1), 2)
lyt.obstruct_coordinate((2, 2))
input_port = lyt.create_pi("a", (0, 0))
wire = lyt.create_buf(input_port, (1, 0))
lyt.create_po(wire, "f", (2, 0))
lyt.move_object(wire, (1, 1))
lyt.disconnect(LayoutInputPort(wire, 0))
lyt.connect(input_port, LayoutInputPort(wire, 0))
independent = lyt.clone()
```

:::

::::

Ordinary C++ copies, `clone()`, Python `copy.copy()`, and Python `copy.deepcopy()` copy
geometry, objects, connections, clocking,
synchronization, and obstructions independently. Corresponding objects retain their numeric IDs;
an ID must still be used with the layout that owns it. Explicit obstructions survive clearing
or moving gates; removing an explicit obstruction does not remove a gate or wire.

Replacing a whole layout invalidates handles into the destination's old contents. Moving a whole
layout transfers its objects and numeric IDs to the destination; handles into the emptied source
cannot be used with that source after reuse.

PI and PO order is independent of coordinates and object iteration. `set_input_order` and
`set_output_order` accept a permutation of all live terminals of the corresponding role and
validate the full permutation before changing the order. Interface matching uses names that are
unique on both sides first, then pairs the remaining terminals in declared order.

An empty layout uses the four-phase OPEN scheme. Assigning a synchronization delay of zero
removes the synchronization element. Replacing the clocking scheme preserves synchronization
delays. Synchronization changes clocked traversal; it does not add support for sequential circuits.

Generations detect removed-slot reuse within one layout contents lifetime. IDs do not carry a layout identity;
`contains` cannot detect IDs from unrelated layouts or contents invalidated by whole-layout assignment.
The caller must use an ID only with its owning contents.

Object and terminal visitors permit coordinate, name, and capability edits. Callbacks must not create or remove
objects, change terminal order, or replace the layout. Connection visitors preserve the traversed object and its
input or sink connections. Connection callbacks may create or remove unrelated objects if those connections
remain unchanged. Collect IDs before changing traversed membership or connections. Object traversal scans
one bitmap word per 64 retained slots and visits each live object. Its cost is
$O(H / 64 + L)$ for $H$ historical slots and $L$ live objects.

The layout stores object records and mutable connections in contiguous reusable pools. Indexed sink lists
permit constant-time unlinking. Every built-in gate keeps its ordered inputs inline; gates with more than
three inputs use separate overflow storage. This capacity covers built-in gate arities and does not limit
the number of inputs or depend on the coordinate geometry. Hexagonal layouts also store larger input lists
in the overflow table. Names and truth-table payloads live outside object records.
Removing an object releases its overflow inputs; the object and connection pools retain capacity for reuse.
The live-object bitmap adds one bit per retained slot without enlarging object records.

Contiguous connection storage does not make each object's sink list contiguous. Reconnecting inputs reuses
free edge slots, so sink traversal follows indices that can span the pool. This representation supports local
routing edits without shifting neighboring adjacency lists. Analyses that need repeated dense logic traversal
can extract a network; the editable layout does not maintain a second adjacency representation.

Coordinates use a sparse flat hash map rather than an array proportional to the layout's frame. Names and
truth-table payloads live outside the records read by ordinary topology traversal, which needs neither payload.
The three inline input indices cover the built-in gate set without an allocation per gate. Object IDs keep
their slot and generation across moves; compacting slots would require a separate identity mapping.
