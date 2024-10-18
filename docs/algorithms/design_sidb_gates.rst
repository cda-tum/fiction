SiDB Gate Designer
------------------

.. _gate_design:

.. tabs::
    .. tab:: C++
        **Header:** ``fiction/algorithms/physical_design/design_sidb_gates.hpp``

        .. doxygenstruct:: fiction::design_sidb_gates_metric_driven_simulated_annealing_params
           :members:
        .. doxygenenum:: fiction::design_sidb_gates_mode
           :members:
        .. doxygenstruct:: fiction::design_sidb_gates_params
           :members:
        .. doxygenstruct:: fiction::design_sidb_gates_stats
           :members:
        .. doxygenfunction:: fiction::design_sidb_gates
        .. doxygenfunction:: fiction::design_sidb_gates_metric_driven_simulated_annealing

    .. tab:: Python
        .. autoclass:: mnt.pyfiction.design_sidb_gates_mode
            :members:
        .. autoclass:: mnt.pyfiction.design_sidb_gates_params
            :members:
        .. autofunction:: mnt.pyfiction.design_sidb_gates
