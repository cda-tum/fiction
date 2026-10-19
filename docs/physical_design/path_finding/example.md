# Path Finding Example

Find a forward path through a 2DDWave-clocked Cartesian layout.
The returned path includes both endpoints.

::::{tab-set}
:sync-group: language

:::{tab-item} C++
:sync: cpp

```cpp
#include <fiction/layouts/cartesian_layout.hpp>
#include <fiction/layouts/gate_layout.hpp>
#include <fiction/layouts/clocking_scheme.hpp>
#include <fiction/physical_design/path_finding/a_star.hpp>
#include <cassert>

int main()
{
    using layout = fiction::layouts::gate_layout<fiction::layouts::cartesian_layout>;
    layout lyt{{5, 5}, fiction::layouts::clocking::twoddwave()};
    const auto path = fiction::physical_design::path_finding::a_star<
        fiction::physical_design::layout_coordinate_path>(lyt, {{0, 0}, {2, 2}});
    assert(path.size() == 5);
}
```

:::

:::{tab-item} Python
:sync: python

```python
from mnt.pyfiction.layouts import cartesian_gate_layout, coordinate
from mnt.pyfiction.physical_design.path_finding import a_star

layout = cartesian_gate_layout((5, 5), "2DDWave")
path = a_star(layout, coordinate(0, 0), coordinate(2, 2))
assert len(path) == 5
```

:::

::::
