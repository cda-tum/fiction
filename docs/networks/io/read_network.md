# Logic Networks

::::{tab-set}
:sync-group: language

:::{tab-item} C++
:sync: cpp

**Header:** `fiction/networks/io/network_reader.hpp`

```{doxygenclass} fiction::networks::io::network_reader
:members:
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

```python
from mnt.pyfiction.networks import aig_network
from mnt.pyfiction.networks.io import read_network

network = read_network("network.v", network_type=aig_network)
```
