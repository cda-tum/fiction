# Simulation and Random Generation

```{eval-rst}
.. autofunction:: mnt.pyfiction.networks.simulate

.. autofunction:: mnt.pyfiction.networks.simulate_outputs

.. autofunction:: mnt.pyfiction.networks.random_network

```

Random network generation accepts a keyword-only `network_type` class.
The default is `technology_network`; `aig_network`, `xag_network`, and `mig_network` select
their native representations. Pass the exact registered class; subclasses are not supported.

```python
from mnt.pyfiction.networks import aig_network, random_network

random = random_network(3, 4, 42, network_type=aig_network)
```
