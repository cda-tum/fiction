(gate_design)=

# SiDB Gate Designer

Set `operational_params.timeout` in C++ or the `timeout` keyword in Python to limit a gate search in milliseconds. See {ref}`sidb_timeouts`
for defaults, exceptions, and cancellation limits.

::::{tab-set}
:sync-group: language

:::{tab-item} C++
:sync: cpp

**Header:** `fiction/technology/sidb/generators/design_gates.hpp`

```{doxygenstruct} fiction::sidb::generators::design_gates_stats
:members:
```

```{doxygenstruct} fiction::sidb::generators::design_gates_params
:members:
```

```{doxygenfunction} fiction::sidb::generators::design_gates(const layout& skeleton, const std::vector<fiction::synthesis::truth_table>& spec, const design_gates_params& params = {}, design_gates_stats* stats = nullptr)

```

:::

:::{tab-item} Python
:sync: python

The Python call releases the GIL and leaves the input skeleton unchanged.

```python
gates = design_sidb_gates(skeleton, specification, timeout=5_000)
```

```{eval-rst}
.. autoclass:: mnt.pyfiction.sidb.generators.design_sidb_gates_stats
   :members:

.. autoclass:: mnt.pyfiction.sidb.generators.design_sidb_gates_mode
   :members:

.. autoclass:: mnt.pyfiction.sidb.generators.termination_condition
   :members:
   :undoc-members:

.. autofunction:: mnt.pyfiction.sidb.generators.design_sidb_gates
```

:::

::::

## Example

Search for a two-input AND gate by adding one SiDB inside the canvas.
The skeleton supplies the input and output wires. The search returns at least one
operational layout; the number of returned layouts is not fixed.

::::{tab-set}
:sync-group: language

:::{tab-item} C++
:sync: cpp

```cpp
#include <fiction/technology/sidb/generators/design_gates.hpp>
#include <fiction/technology/sidb/layout.hpp>
#include <fiction/synthesis/truth_tables.hpp>
#include <cassert>
#include <initializer_list>
#include <iostream>

int main()
{
    namespace sidb = fiction::sidb;
    sidb::layout skeleton{};
    const auto place = [&](sidb::dot_tag tag, std::initializer_list<sidb::lattice_site> sites)
    {
        for (const auto& site : sites) { skeleton.assign_sidb(site, tag); }
    };
    place(sidb::dot_tag::INPUT, {{0, 0, 1}, {2, 1, 1}, {20, 0, 1}, {18, 1, 1}});
    place(sidb::dot_tag::NORMAL, {{4, 2, 1}, {6, 3, 1}, {14, 3, 1}, {16, 2, 1}, {10, 9, 1}});
    place(sidb::dot_tag::OUTPUT, {{10, 6, 0}, {10, 7, 0}});
    sidb::generators::design_gates_params params{};
    params.operational_params.sim_params.base = 2;
    params.operational_params.sim_params.mu_minus = -0.28;
    params.operational_params.timeout = 5'000;
    params.canvas = {{4, 4, 0}, {14, 5, 1}};
    params.number_of_canvas_sidbs = 1;
    const auto gates = sidb::generators::design_gates(skeleton, {fiction::synthesis::create_and_tt()}, params);
    assert(!gates.empty());
    std::cout << gates.size() << '\n';
}
```

:::

:::{tab-item} Python
:sync: python

```python
from mnt.pyfiction.sidb import lattice_site, sidb_dot_tag, sidb_layout
from mnt.pyfiction.sidb.generators import design_sidb_gates
from mnt.pyfiction.synthesis import create_and_tt

skeleton = sidb_layout()
for tag, sites in (
    (sidb_dot_tag.INPUT, ((0, 0, 1), (2, 1, 1), (20, 0, 1), (18, 1, 1))),
    (sidb_dot_tag.NORMAL, ((4, 2, 1), (6, 3, 1), (14, 3, 1), (16, 2, 1), (10, 9, 1))),
    (sidb_dot_tag.OUTPUT, ((10, 6, 0), (10, 7, 0))),
):
    for site in sites:
        skeleton.assign_sidb(lattice_site(*site), tag)
gates = design_sidb_gates(
    skeleton,
    [create_and_tt()],
    base=2,
    mu_minus=-0.28,
    canvas=(lattice_site(4, 4, 0), lattice_site(14, 5, 1)),
    number_of_canvas_sidbs=1,
    timeout=5_000,
)
assert gates
print(len(gates))
```

:::

::::
