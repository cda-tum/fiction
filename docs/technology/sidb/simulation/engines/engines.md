(simulation-engines)=

# Engines

(quicksim)=

## Heuristic Ground State Simulation

::::{tab-set}
:sync-group: language

:::{tab-item} C++
:sync: cpp

**Header:** `fiction/technology/sidb/simulation/engines/quicksim.hpp`

```{doxygenstruct} fiction::sidb::simulation::engines::quicksim_params
:members:
```

```{doxygenfunction} fiction::sidb::simulation::engines::quicksim(const layout& lyt, const quicksim_params& ps = quicksim_params{})

```

:::

:::{tab-item} Python
:sync: python

```{eval-rst}
.. autofunction:: mnt.pyfiction.sidb.simulation.engines.quicksim
```

:::

::::

## Exhaustive Ground State Simulation

(quickexact)=

### QuickExact

::::{tab-set}
:sync-group: language

:::{tab-item} C++
:sync: cpp

**Header:** `fiction/technology/sidb/simulation/engines/quickexact.hpp`

```{doxygenstruct} fiction::sidb::simulation::engines::quickexact_params
:members:
```

```{doxygenfunction} fiction::sidb::simulation::engines::quickexact(const layout& lyt, const quickexact_params& params = {})

```

:::

:::{tab-item} Python
:sync: python

```{eval-rst}
.. autoclass:: mnt.pyfiction.sidb.simulation.engines.automatic_base_number_detection
   :members:

.. autofunction:: mnt.pyfiction.sidb.simulation.engines.quickexact
```

:::

::::

(clustercomplete)=

### ClusterComplete

::::{tab-set}
:sync-group: language

:::{tab-item} C++
:sync: cpp

**Header:** `fiction/technology/sidb/simulation/engines/clustercomplete.hpp`

```{doxygenstruct} fiction::sidb::simulation::engines::clustercomplete_params
:members:
```

```{doxygenfunction} fiction::sidb::simulation::engines::clustercomplete(const layout& lyt, const clustercomplete_params& params = {})

```

:::

:::{tab-item} Python
:sync: python

```{eval-rst}
.. autoclass:: mnt.pyfiction.sidb.simulation.engines.ground_state_space_reporting
   :members:

.. autofunction:: mnt.pyfiction.sidb.simulation.engines.clustercomplete
```

:::

::::

### ExGS

::::{tab-set}
:sync-group: language

:::{tab-item} C++
:sync: cpp

**Header:** `fiction/technology/sidb/simulation/engines/exhaustive_ground_state_simulation.hpp`

```{doxygenfunction} fiction::sidb::simulation::engines::exhaustive_ground_state_simulation

```

:::

:::{tab-item} Python
:sync: python

```{eval-rst}
.. autofunction:: mnt.pyfiction.sidb.simulation.engines.exhaustive_ground_state_simulation
```

:::

::::

(ground_state_space)=

## Ground State Space Construction

**Header:** `fiction/technology/sidb/simulation/engines/ground_state_space.hpp`

```{doxygenstruct} fiction::sidb::simulation::engines::ground_state_space_params
:members:
```

```{doxygenstruct} fiction::sidb::simulation::engines::ground_state_space_results
:members:
```

```{doxygenfunction} fiction::sidb::simulation::engines::ground_state_space(const potential_landscape& land, const ground_state_space_params& params = {})
```

```{doxygenfunction} fiction::sidb::simulation::engines::ground_state_space(const layout& lyt, const ground_state_space_params& params = {})
```

## Example

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
