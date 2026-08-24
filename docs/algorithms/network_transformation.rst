Network Conversion
------------------

**Header:** ``fiction/synthesis/network_conversion.hpp``

.. doxygenfunction:: fiction::synthesis::convert_network

Network Balancing
------------------

.. tabs::
    .. tab:: C++
        **Header:** ``fiction/synthesis/network_balancing.hpp``

        .. doxygenstruct:: fiction::synthesis::network_balancing_params
           :members:
        .. doxygenfunction:: fiction::synthesis::network_balancing

    .. tab:: Python
        .. autoclass:: mnt.pyfiction.network_balancing_params
            :members:
        .. autofunction:: mnt.pyfiction.network_balancing
        .. autofunction:: mnt.pyfiction.is_balanced

Fanout Substitution
-------------------

.. tabs::
    .. tab:: C++
        **Header:** ``fiction/synthesis/fanout_substitution.hpp``

        .. doxygenstruct:: fiction::synthesis::fanout_substitution_params
            :members:
        .. doxygenfunction:: fiction::synthesis::fanout_substitution

    .. tab:: Python
        .. autoclass:: mnt.pyfiction.fanout_substitution_params
            :members:
        .. autoclass:: mnt.pyfiction.substitution_strategy
            :members:
        .. autofunction:: mnt.pyfiction.fanout_substitution
        .. autofunction:: mnt.pyfiction.is_fanout_substituted


Technology Mapping
------------------

.. tabs::
    .. tab:: C++
        **Header:** ``fiction/synthesis/technology_mapping.hpp``

        .. doxygenclass:: fiction::synthesis::missing_required_gates_exception
           :members:
        .. doxygenstruct:: fiction::synthesis::technology_mapping_params
           :members:
        .. doxygenfunction:: fiction::synthesis::and_or_not
        .. doxygenfunction:: fiction::synthesis::and_or_not_maj
        .. doxygenfunction:: fiction::synthesis::all_standard_2_input_functions
        .. doxygenfunction:: fiction::synthesis::all_standard_3_input_functions
        .. doxygenfunction:: fiction::synthesis::all_supported_standard_functions
        .. doxygenstruct:: fiction::synthesis::technology_mapping_stats
           :members:
        .. doxygenfunction:: fiction::synthesis::technology_mapping

    .. tab:: Python
        .. autoclass:: mnt.pyfiction.missing_required_gates_exception
            :members:
        .. autoclass:: mnt.pyfiction.technology_mapping_params
            :members:
        .. autofunction:: mnt.pyfiction.and_or_not
        .. autofunction:: mnt.pyfiction.and_or_not_maj
        .. autofunction:: mnt.pyfiction.all_standard_2_input_functions
        .. autofunction:: mnt.pyfiction.all_standard_3_input_functions
        .. autofunction:: mnt.pyfiction.all_supported_standard_functions
        .. autoclass:: mnt.pyfiction.technology_mapping_stats
            :members:
        .. autofunction:: mnt.pyfiction.technology_mapping

Delete Virtual PIs
------------------

.. tabs::
    .. tab:: C++
        **Header:** ``fiction/synthesis/delete_virtual_pis.hpp``

        .. doxygenfunction:: fiction::synthesis::delete_virtual_pis
