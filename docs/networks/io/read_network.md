# Logic Networks

::::{tab-set}
:sync-group: language

:::{tab-item} C++
:sync: cpp

**Header:** `fiction/networks/io/network_reader.hpp`

```{doxygenfunction} fiction::networks::io::read_network
```

```{doxygenfunction} fiction::networks::io::read_networks
```

```{doxygenstruct} fiction::networks::io::network_read_batch
:members:
```

```{doxygenstruct} fiction::networks::io::network_read_failure
:members:
```

```{doxygenclass} fiction::networks::io::network_read_error
```

```cpp
#include <fiction/networks/io/network_reader.hpp>
#include <fiction/types.hpp>

const auto network = fiction::networks::io::read_network<fiction::networks::named_aig_network>("network.v");
```

:::

:::{tab-item} Python
:sync: python

```{eval-rst}
.. autofunction:: mnt.pyfiction.networks.io.read_network
```

:::

::::

Python network readers accept a keyword-only `network_type` class.
The default is `technology_network`; `aig_network`, `xag_network`, and `mig_network` select
their native representations. Pass the exact registered class; subclasses are not supported.
BLIF input supports `technology_network` only.

Both APIs infer Verilog (`.v`), binary AIGER (`.aig`), and BLIF (`.blif`) from the filename, ignoring case.
For Verilog, the reader constructs the network from the module named `top`.
BLIF reads the first flat model and accepts blank lines, comments, and continued declarations.
The `.model` and `.end` declarations are optional.
For an AIGER file with latches, a network type without registers exposes latch outputs as primary inputs
and next-state functions as primary outputs.
If parsing creates registers in the destination network, the reader reports a file failure.
An explicit format overrides the extension. Use `v`, `aig`, or `blif`, with an optional leading dot.
For a directory, `read_network` tries files in native filename order and stops at the first successful read.
The C++ `read_networks` function returns successful networks and reports file-level parsing and I/O failures.
Allocation failures can propagate to the caller. Directory reads visit regular files directly in the given directory.
They skip unrelated extensions when inferring the format.

```python
from mnt.pyfiction.networks import aig_network
from mnt.pyfiction.networks.io import read_network

network = read_network("network.v", network_type=aig_network)
```
