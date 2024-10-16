Technology Network
==================

A logic network type that extends any ``mockturtle`` logic network. It enables copies of PIs, called virtual PIs.
Virtual PIs are created by passing an existing ``real_pi`` to the ``create_virtual_pi`` function. This function
calls the ``create_pi`` function of the extended network and stores the newly created ``virtual_pi`` in a node map,
which maps it to the originating ``real_pi``. Additionally, ``delete_virtual_pis`` returns a network with all
``virtual_pi`` nodes removed and reassigns the edges connected to them to the corresponding ``real_pi`` node stored in
the ``node_map``.

.. tabs::
    .. tab:: C++
        **Header:** ``fiction/networks/virtual_pi_network.hpp``

        .. doxygenclass:: fiction::virtual_pi_network
           :members:

    .. tab:: Python
        .. autoclass:: mnt.pyfiction.virtual_pi_network
           :members:
