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

```cpp
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
An explicit format overrides the extension. Use `v`, `aig`, or `blif`, with an optional leading dot.
For a directory, `read_network` tries files in filename order and stops at the first successful read.
The C++ `read_networks` function returns every successful network along with the failures. Directory reads
visit regular files directly in the given directory. They skip unrelated extensions when inferring the format.

```python
from mnt.pyfiction.networks import aig_network
from mnt.pyfiction.networks.io import read_network

network = read_network("network.v", network_type=aig_network)
```
