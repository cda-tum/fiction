# Logic Simulation Example

Sweep relative permittivity at two points and write the classified domain as CSV.
This layout produces two `NON_OPERATIONAL` entries: a completed sweep does not imply
that the layout implements the identity function.

::::{tab-set}
:sync-group: language

:::{tab-item} C++
:sync: cpp

```cpp
#include <fiction/technology/sidb/layout.hpp>
#include <fiction/technology/sidb/simulation/logic/operational_domain.hpp>
#include <fiction/technology/sidb/simulation/io/write_operational_domain.hpp>
#include <fiction/synthesis/truth_tables.hpp>
#include <cassert>
#include <initializer_list>
#include <iostream>

int main()
{
    namespace sidb = fiction::sidb;
    namespace logic = sidb::simulation::logic;
    sidb::layout wire{};
    const auto place = [&](sidb::dot_tag tag, std::initializer_list<int> xs)
    {
        for (const auto x : xs) { wire.assign_sidb({x, 0, 0}, tag); }
    };
    place(sidb::dot_tag::INPUT, {0, 2});
    place(sidb::dot_tag::NORMAL, {6, 8, 18});
    place(sidb::dot_tag::OUTPUT, {12, 14});
    logic::operational_domain_params params{};
    params.operational_params.sim_params.base = 2;
    params.operational_params.timeout = 5'000;
    params.sweep_dimensions = {{logic::sweep_parameter::EPSILON_R, 5.5, 5.6, 0.1}};
    const auto domain = logic::operational_domain_grid_search(wire, {fiction::synthesis::create_id_tt()}, params);
    assert(domain.size() == 2);
    sidb::simulation::io::write_operational_domain(domain, std::cout);
}
```

:::

:::{tab-item} Python
:sync: python

```python
from mnt.pyfiction.sidb import lattice_site, sidb_dot_tag, sidb_layout
from mnt.pyfiction.sidb.simulation.io import write_operational_domain_to_string
from mnt.pyfiction.sidb.simulation.logic import (
    operational_domain_grid_search,
    operational_domain_value_range,
    sweep_parameter,
)
from mnt.pyfiction.synthesis import create_id_tt

wire = sidb_layout()
for tag, xs in (
    (sidb_dot_tag.INPUT, (0, 2)),
    (sidb_dot_tag.NORMAL, (6, 8, 18)),
    (sidb_dot_tag.OUTPUT, (12, 14)),
):
    for x in xs:
        wire.assign_sidb(lattice_site(x, 0, 0), tag)
axis = operational_domain_value_range(sweep_parameter.EPSILON_R, 5.5, 5.6, 0.1)
domain = operational_domain_grid_search(wire, [create_id_tt()], base=2, sweep_dimensions=[axis], timeout=5_000)
assert len(domain) == 2
print(write_operational_domain_to_string(domain))
```

:::

::::
