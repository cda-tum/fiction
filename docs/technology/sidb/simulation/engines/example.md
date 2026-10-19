# Simulation Engine Example

Simulate three SiDBs and inspect a minimum-energy charge distribution.
`groundstates()` returns all distinct charge distributions with the minimum energy;
several distributions can share that energy. Energies use electronvolts.

::::{tab-set}
:sync-group: language

:::{tab-item} C++
:sync: cpp

```cpp
#include <fiction/technology/sidb/layout.hpp>
#include <fiction/technology/sidb/simulation/engines/quickexact.hpp>
#include <cassert>
#include <iostream>

int main()
{
    namespace sidb = fiction::sidb;
    sidb::layout layout{};
    for (int x = 0; x < 3; ++x)
    {
        layout.assign_sidb({x, 0, 0});
    }
    sidb::simulation::engines::quickexact_params params{};
    params.sim_params.base = 2;
    params.sim_params.mu_minus = -0.25;
    const auto result = sidb::simulation::engines::quickexact(layout, params);
    const auto states = result.groundstates();
    assert(!states.empty());
    std::cout << states.front().energy() << ' '
              << static_cast<int>(states.front().get_charge_state({0, 0, 0})) << '\n';
}
```

:::

:::{tab-item} Python
:sync: python

```python
from mnt.pyfiction.sidb import lattice_site, sidb_dot_tag, sidb_layout
from mnt.pyfiction.sidb.simulation.engines import quickexact

layout = sidb_layout()
for x in range(3):
    layout.assign_sidb(lattice_site(x, 0, 0), sidb_dot_tag.NORMAL)
result = quickexact(layout, base=2, mu_minus=-0.25)
states = result.groundstates()
assert states
print(states[0].energy(), states[0].get_charge_state(lattice_site(0, 0, 0)))
```

:::

::::
