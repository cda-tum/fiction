# Network Transformation

## Example

Read a two-input AND network, convert its representation, and check its truth table.
Map the network to the supplied gate set, place it, and compare the resulting layout.

::::{tab-set}
:sync-group: language

:::{tab-item} C++
:sync: cpp

```cpp
#include <fiction/layouts/cartesian_layout.hpp>
#include <fiction/layouts/gate_layout.hpp>
#include <fiction/networks/technology_network.hpp>
#include <fiction/physical_design/orthogonal.hpp>
#include <fiction/synthesis/network_conversion.hpp>
#include <fiction/synthesis/technology_mapping.hpp>
#include <fiction/verification/count_gate_types.hpp>
#include <fiction/verification/equivalence_checking.hpp>
#include <mockturtle/networks/aig.hpp>
#include <cassert>

int main()
{
    fiction::networks::technology_network network{};
    const auto a = network.create_pi();
    const auto b = network.create_pi();
    network.create_po(network.create_and(a, b));
    const auto aig = fiction::synthesis::convert_network<mockturtle::aig_network>(network);
    const auto mapped = fiction::synthesis::technology_mapping(aig, fiction::synthesis::and_or_not());
    using layout = fiction::layouts::gate_layout<fiction::layouts::cartesian_layout>;
    const auto placed = fiction::physical_design::orthogonal<layout>(mapped);
    assert(fiction::verification::equivalence_checking(network, placed) == fiction::verification::eq_type::STRONG);
    fiction::verification::count_gate_types_stats counts{};
    fiction::verification::count_gate_types(placed, &counts);
    assert(counts.num_and2 == 1);
}
```

:::

:::{tab-item} Python
:sync: python

```python
from pathlib import Path
from tempfile import TemporaryDirectory

from mnt.pyfiction.networks import simulate_outputs
from mnt.pyfiction.networks.io import read_network
from mnt.pyfiction.physical_design import orthogonal
from mnt.pyfiction.synthesis import (
    and_or_not,
    convert_network,
    network_target,
    technology_mapping,
)
from mnt.pyfiction.verification import count_gate_types, eq_type, equivalence_checking

with TemporaryDirectory() as directory:
    source = Path(directory) / "and.v"
    source.write_text(
        "module top(a, b, f);\ninput a, b;\noutput f;\nassign f = a & b;\nendmodule\n",
        encoding="utf-8",
    )
    network = read_network(str(source))

aig = convert_network(network, network_target.AIG)
assert simulate_outputs(aig) == [("f", [True, False, False, False])]
mapped = technology_mapping(aig, **and_or_not())
layout = orthogonal(mapped)
assert equivalence_checking(network, layout) == eq_type.STRONG
assert count_gate_types(layout).num_and2 == 1
```

:::

::::

Simulation lists truth-table bits from the highest input assignment to the lowest.
`simulate_outputs` preserves output order and duplicate output names; `simulate` returns
an output-name dictionary, so duplicate names overwrite earlier entries.

(network-conversion)=

## Network Conversion

::::{tab-set}
:sync-group: language

:::{tab-item} C++
:sync: cpp

**Header:** `fiction/synthesis/network_conversion.hpp`

```{doxygenfunction} fiction::synthesis::convert_network
```

:::

:::{tab-item} Python
:sync: python

```{eval-rst}
.. autoclass:: mnt.pyfiction.synthesis.network_target
   :members:

.. autofunction:: mnt.pyfiction.synthesis.convert_network
```

:::

::::

## Network Balancing

::::{tab-set}
:sync-group: language

:::{tab-item} C++
:sync: cpp

**Header:** `fiction/synthesis/network_balancing.hpp`

```{doxygenstruct} fiction::synthesis::network_balancing_params
:members:
```

```{doxygenfunction} fiction::synthesis::is_balanced
```

```{doxygenfunction} fiction::synthesis::network_balancing

```

:::

:::{tab-item} Python
:sync: python

```{eval-rst}
.. autofunction:: mnt.pyfiction.synthesis.network_balancing

.. autofunction:: mnt.pyfiction.synthesis.is_balanced
```

:::

::::

## Fanout Substitution

::::{tab-set}
:sync-group: language

:::{tab-item} C++
:sync: cpp

**Header:** `fiction/synthesis/fanout_substitution.hpp`

```{doxygenstruct} fiction::synthesis::fanout_substitution_params
:members:
```

```{doxygenfunction} fiction::synthesis::is_fanout_substituted
```

```{doxygenfunction} fiction::synthesis::fanout_substitution

```

:::

:::{tab-item} Python
:sync: python

```{eval-rst}
.. autoclass:: mnt.pyfiction.synthesis.substitution_strategy
   :members:

.. autofunction:: mnt.pyfiction.synthesis.fanout_substitution

.. autofunction:: mnt.pyfiction.synthesis.is_fanout_substituted
```

:::

::::

## Technology Mapping

::::{tab-set}
:sync-group: language

:::{tab-item} C++
:sync: cpp

**Header:** `fiction/synthesis/technology_mapping.hpp`

```{doxygenclass} fiction::synthesis::missing_required_gates_exception
:members:
```

```{doxygenstruct} fiction::synthesis::technology_mapping_params
:members:
```

```{doxygenfunction} fiction::synthesis::and_or_not

```

```{doxygenfunction} fiction::synthesis::and_or_not_maj

```

```{doxygenfunction} fiction::synthesis::all_2_input_functions
```

```{doxygenfunction} fiction::synthesis::all_standard_2_input_functions

```

```{doxygenfunction} fiction::synthesis::all_standard_3_input_functions

```

```{doxygenfunction} fiction::synthesis::all_supported_functions
```

```{doxygenfunction} fiction::synthesis::all_supported_standard_functions

```

```{doxygenstruct} fiction::synthesis::technology_mapping_stats
:members:
```

```{doxygenfunction} fiction::synthesis::technology_mapping

```

:::

:::{tab-item} Python
:sync: python

```{eval-rst}
.. autoclass:: mnt.pyfiction.synthesis.missing_required_gates_exception
   :members:

.. autofunction:: mnt.pyfiction.synthesis.and_or_not

.. autofunction:: mnt.pyfiction.synthesis.and_or_not_maj

.. autofunction:: mnt.pyfiction.synthesis.all_standard_2_input_functions

.. autofunction:: mnt.pyfiction.synthesis.all_standard_3_input_functions

.. autofunction:: mnt.pyfiction.synthesis.all_supported_standard_functions

.. autoclass:: mnt.pyfiction.synthesis.mapper_stats
   :members:

.. autoclass:: mnt.pyfiction.synthesis.technology_mapping_stats
   :members:

.. autofunction:: mnt.pyfiction.synthesis.technology_mapping
```

:::

::::

## Delete Virtual PIs

::::{tab-set}
:sync-group: language

:::{tab-item} C++
:sync: cpp

**Header:** `fiction/synthesis/delete_virtual_pis.hpp`

```{doxygenfunction} fiction::synthesis::delete_virtual_pis

```

:::

::::

## Truth Tables

Construct a truth table from binary, hexadecimal, or Boolean expressions.
The string forms list the most significant bit first.

::::{tab-set}
:sync-group: language

:::{tab-item} C++
:sync: cpp

```cpp
#include <fiction/synthesis/truth_tables.hpp>
#include <kitty/print.hpp>
#include <cassert>

int main()
{
    const auto table = fiction::synthesis::create_maj_tt();
    assert(kitty::to_binary(table) == "11101000");
    assert(kitty::to_hex(table) == "e8");
}
```

:::

:::{tab-item} Python
:sync: python

```python
from mnt.pyfiction.synthesis import create_maj_tt, dynamic_truth_table

table = dynamic_truth_table(3)
table.create_from_expression("<abc>")
assert table.to_binary() == "11101000"
assert table.to_hex() == create_maj_tt().to_hex() == "e8"
```

:::

::::

::::{tab-set}
:sync-group: language

:::{tab-item} C++
:sync: cpp

**Header:** `fiction/synthesis/truth_tables.hpp`

```{doxygenfunction} fiction::synthesis::evaluate_output
```

```{doxygenfunction} fiction::synthesis::create_id_tt

```

```{doxygenfunction} fiction::synthesis::create_not_tt

```

```{doxygenfunction} fiction::synthesis::create_and_tt

```

```{doxygenfunction} fiction::synthesis::create_or_tt

```

```{doxygenfunction} fiction::synthesis::create_nand_tt

```

```{doxygenfunction} fiction::synthesis::create_nor_tt

```

```{doxygenfunction} fiction::synthesis::create_xor_tt

```

```{doxygenfunction} fiction::synthesis::create_xnor_tt

```

```{doxygenfunction} fiction::synthesis::create_lt_tt

```

```{doxygenfunction} fiction::synthesis::create_gt_tt

```

```{doxygenfunction} fiction::synthesis::create_le_tt

```

```{doxygenfunction} fiction::synthesis::create_ge_tt

```

```{doxygenfunction} fiction::synthesis::create_and3_tt

```

```{doxygenfunction} fiction::synthesis::create_xor_and_tt

```

```{doxygenfunction} fiction::synthesis::create_or_and_tt

```

```{doxygenfunction} fiction::synthesis::create_onehot_tt

```

```{doxygenfunction} fiction::synthesis::create_maj_tt

```

```{doxygenfunction} fiction::synthesis::create_gamble_tt

```

```{doxygenfunction} fiction::synthesis::create_dot_tt

```

```{doxygenfunction} fiction::synthesis::create_ite_tt

```

```{doxygenfunction} fiction::synthesis::create_and_xor_tt

```

```{doxygenfunction} fiction::synthesis::create_xor3_tt

```

```{doxygenfunction} fiction::synthesis::create_double_wire_tt

```

```{doxygenfunction} fiction::synthesis::create_crossing_wire_tt

```

```{doxygenfunction} fiction::synthesis::create_fan_out_tt

```

```{doxygenfunction} fiction::synthesis::create_half_adder_tt

```

:::

:::{tab-item} Python
:sync: python

```{eval-rst}
.. autoclass:: mnt.pyfiction.synthesis.dynamic_truth_table
   :members:

.. autofunction:: mnt.pyfiction.synthesis.create_id_tt

.. autofunction:: mnt.pyfiction.synthesis.create_not_tt

.. autofunction:: mnt.pyfiction.synthesis.create_and_tt

.. autofunction:: mnt.pyfiction.synthesis.create_or_tt

.. autofunction:: mnt.pyfiction.synthesis.create_nand_tt

.. autofunction:: mnt.pyfiction.synthesis.create_nor_tt

.. autofunction:: mnt.pyfiction.synthesis.create_xor_tt

.. autofunction:: mnt.pyfiction.synthesis.create_xnor_tt

.. autofunction:: mnt.pyfiction.synthesis.create_lt_tt

.. autofunction:: mnt.pyfiction.synthesis.create_gt_tt

.. autofunction:: mnt.pyfiction.synthesis.create_le_tt

.. autofunction:: mnt.pyfiction.synthesis.create_ge_tt

.. autofunction:: mnt.pyfiction.synthesis.create_and3_tt

.. autofunction:: mnt.pyfiction.synthesis.create_xor_and_tt

.. autofunction:: mnt.pyfiction.synthesis.create_or_and_tt

.. autofunction:: mnt.pyfiction.synthesis.create_onehot_tt

.. autofunction:: mnt.pyfiction.synthesis.create_maj_tt

.. autofunction:: mnt.pyfiction.synthesis.create_gamble_tt

.. autofunction:: mnt.pyfiction.synthesis.create_dot_tt

.. autofunction:: mnt.pyfiction.synthesis.create_ite_tt

.. autofunction:: mnt.pyfiction.synthesis.create_and_xor_tt

.. autofunction:: mnt.pyfiction.synthesis.create_xor3_tt

.. autofunction:: mnt.pyfiction.synthesis.create_double_wire_tt

.. autofunction:: mnt.pyfiction.synthesis.create_crossing_wire_tt

.. autofunction:: mnt.pyfiction.synthesis.create_fan_out_tt

.. autofunction:: mnt.pyfiction.synthesis.create_half_adder_tt
```

:::

::::

## Technology Mapping Library

**Header:** `fiction/synthesis/technology_mapping_library.hpp`

```{doxygenvariable} fiction::synthesis::GATE_ZERO
```

```{doxygenvariable} fiction::synthesis::GATE_ONE
```

```{doxygenvariable} fiction::synthesis::GATE_BUF
```

```{doxygenvariable} fiction::synthesis::GATE_INV
```

```{doxygenvariable} fiction::synthesis::GATE_AND2
```

```{doxygenvariable} fiction::synthesis::GATE_NAND2
```

```{doxygenvariable} fiction::synthesis::GATE_OR2
```

```{doxygenvariable} fiction::synthesis::GATE_NOR2
```

```{doxygenvariable} fiction::synthesis::GATE_XOR2
```

```{doxygenvariable} fiction::synthesis::GATE_XNOR2
```

```{doxygenvariable} fiction::synthesis::GATE_LT2
```

```{doxygenvariable} fiction::synthesis::GATE_GT2
```

```{doxygenvariable} fiction::synthesis::GATE_LE2
```

```{doxygenvariable} fiction::synthesis::GATE_GE2
```

```{doxygenvariable} fiction::synthesis::GATE_AND3
```

```{doxygenvariable} fiction::synthesis::DECAY_AND3
```

```{doxygenvariable} fiction::synthesis::GATE_XOR_AND
```

```{doxygenvariable} fiction::synthesis::DECAY_XOR_AND
```

```{doxygenvariable} fiction::synthesis::GATE_OR_AND
```

```{doxygenvariable} fiction::synthesis::DECAY_OR_AND
```

```{doxygenvariable} fiction::synthesis::GATE_ONEHOT
```

```{doxygenvariable} fiction::synthesis::DECAY_ONEHOT
```

```{doxygenvariable} fiction::synthesis::GATE_MAJ3
```

```{doxygenvariable} fiction::synthesis::DECAY_MAJ3
```

```{doxygenvariable} fiction::synthesis::GATE_GAMBLE
```

```{doxygenvariable} fiction::synthesis::DECAY_GAMBLE
```

```{doxygenvariable} fiction::synthesis::GATE_DOT
```

```{doxygenvariable} fiction::synthesis::DECAY_DOT
```

```{doxygenvariable} fiction::synthesis::GATE_MUX
```

```{doxygenvariable} fiction::synthesis::DECAY_MUX
```

```{doxygenvariable} fiction::synthesis::GATE_AND_XOR
```

```{doxygenvariable} fiction::synthesis::DECAY_AND_XOR
```
