SiDB Gate Designer
------------------

Automated Design of SiDB Gates
##############################

.. _gate_design:

.. tabs::
    .. tab:: C++
        **Header:** ``fiction/algorithms/physical_design/design_sidb_gates.hpp``

        .. doxygenstruct:: fiction::design_sidb_gates_stats
           :members:
        .. doxygenstruct:: fiction::design_sidb_gates_params
           :members:
        .. doxygenfunction:: fiction::design_sidb_gates

    .. tab:: Python
        .. autoclass:: mnt.pyfiction.design_sidb_gates_stats
            :members:
        .. autoclass:: mnt.pyfiction.design_sidb_gates_params_100
            :members:
        .. autoclass:: mnt.pyfiction.design_sidb_gates_params_111
            :members:
        .. doxygenenum:: mnt.pyfiction.design_sidb_gates_params_design_sidb_gates_mode_100
        .. doxygenenum:: mnt.pyfiction.design_sidb_gates_params_design_sidb_gates_mode_111
        .. doxygenenum:: mnt.pyfiction.design_sidb_gates_params_termination_condition_100
        .. doxygenenum:: mnt.pyfiction.design_sidb_gates_params_termination_condition_111
        .. autofunction:: mnt.pyfiction.design_sidb_gates

Post-Process Ordering of SiDB Gates
###################################

.. _designed_gate_ordering:

**Header:** ``fiction/algorithms/physical_design/compare_designed_sidb_gates.hpp``

.. doxygenstruct:: fiction::designed_sidb_gates
    :members:
.. doxygenclass:: fiction::designed_sidb_gate_comparator
    :members:
.. doxygentypedef:: fiction::designed_sidb_gates_ordering_recipe
.. doxygenfunction:: fiction::order_designed_sidb_gates


**Header:** ``fiction/algorithms/simulation/sidb/compare_by_ground_state_isolation.hpp``

.. doxygenclass:: fiction::compare_by_minimum_ground_state_isolation
    :members:
.. doxygenclass:: fiction::compare_by_average_ground_state_isolation
    :members: