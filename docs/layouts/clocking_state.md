# Clocking State

Gate and cell layouts own `clocking::state<Coordinate>` objects in their storage.
The state contains a clocking scheme, its overrides, and synchronization delays.
It has no layout base or geometry parameter. Copying the state copies its contents independently.

Layouts provide the public clocking methods and perform geometry traversal. A nonzero
synchronization delay extends the Hold phase by that many full clock cycles. Distinct
zones can exchange information when their phases follow each other or either zone is a
synchronization element. A zone cannot feed itself.

**Header:** `fiction/layouts/clocking_state.hpp`

```{doxygenclass} fiction::layouts::clocking::state
:members:
```
