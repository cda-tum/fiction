Network Conversion
------------------

**Header:** ``fiction/algorithms/network_transformation/network_conversion.hpp``

.. doxygenfunction:: fiction::convert_network

Network Balancing
------------------

.. tabs::
    .. tab:: C++
        **Header:** ``fiction/algorithms/network_transformation/network_balancing.hpp``

        .. doxygenstruct:: fiction::network_balancing_params
           :members:
        .. doxygenfunction:: fiction::network_balancing
    .. tab:: Python
        .. autoclass:: mnt.pyfiction.network_balancing_params
            :members:
        .. autofunction:: mnt.pyfiction.network_balancing
        .. autofunction:: mnt.pyfiction.is_balanced

Fanout Substitution
-------------------

.. tabs::
    .. tab:: C++
        **Header:** ``fiction/algorithms/network_transformation/fanout_substitution.hpp``

        .. doxygenstruct:: fiction::fanout_substitution_params
            :members:
        .. doxygenfunction:: fiction::fanout_substitution

    .. tab:: Python
        .. autoclass:: mnt.pyfiction.fanout_substitution_params
            :members:
        .. autoclass:: mnt.pyfiction.substitution_strategy
            :members:
        .. autofunction:: mnt.pyfiction.fanout_substitution
        .. autofunction:: mnt.pyfiction.is_fanout_substituted
