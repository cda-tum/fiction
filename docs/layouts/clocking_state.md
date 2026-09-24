# Clocking State

Gate and cell layouts own `clocking::state<Coordinate>` objects in their storage.
The state contains a clocking scheme, its overrides, and synchronization delays.
It has no layout base or geometry parameter. Copying the state copies its contents independently.

Layouts provide the public clocking methods and perform geometry traversal. A clock zone
is a tile: in a gate-level layout, a tile usually holds one gate or wire; in a cell-level
layout, a tile is a region of `get_tile_size_x()` by `get_tile_size_y()` cells that one
clock signal governs on every layer.

Synchronization elements (SEs) {cite:p}`nano2018_1` use the external clock generator to
produce additional asymmetric clock signals with extended Hold phases. A nonzero
synchronization delay turns a clock zone into such a latch-like SE, which extends the Hold
phase by that many full clock cycles. Distinct zones can exchange information when their
phases follow each other or either zone is an SE. A zone cannot feed itself.

**Header:** `fiction/layouts/clocking_state.hpp`

```{doxygenclass} fiction::layouts::clocking::state
:members:
```
