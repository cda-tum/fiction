Simulation Engines
==================

Heuristic Ground State Simulation
---------------------------------

.. _quicksim:

.. tabs::
    .. tab:: C++
        **Header:** ``fiction/technology/sidb/simulation/engines/quicksim.hpp``

        .. doxygenstruct:: fiction::sidb::simulation::engines::quicksim_params
           :members:

        .. doxygenfunction:: fiction::sidb::simulation::engines::quicksim

    .. tab:: Python
        .. autoclass:: mnt.pyfiction.quicksim_params
            :members:

        .. autofunction:: mnt.pyfiction.quicksim

Exhaustive Ground State Simulation
----------------------------------

.. tabs::
    .. tab:: C++

        .. _quickexact:

        **Header:** ``fiction/technology/sidb/simulation/engines/quickexact.hpp``

        .. doxygenstruct:: fiction::sidb::simulation::engines::quickexact_params
           :members:
        .. doxygenfunction:: fiction::sidb::simulation::engines::quickexact

        .. _clustercomplete:

        **Header:** ``fiction/technology/sidb/simulation/engines/clustercomplete.hpp``

        .. doxygenstruct:: fiction::sidb::simulation::engines::clustercomplete_params
           :members:
        .. doxygenfunction:: fiction::sidb::simulation::engines::clustercomplete

        **Header:** ``fiction/technology/sidb/simulation/engines/exhaustive_ground_state_simulation.hpp``

        .. doxygenfunction:: fiction::sidb::simulation::engines::exhaustive_ground_state_simulation

    .. tab:: Python
        .. autoclass:: mnt.pyfiction.quickexact_params
            :members:
        .. autofunction:: mnt.pyfiction.quickexact
        .. autoclass:: mnt.pyfiction.clustercomplete_params
            :members:
        .. autofunction:: mnt.pyfiction.clustercomplete
        .. autofunction:: mnt.pyfiction.exhaustive_ground_state_simulation

Ground State Space Construction
-------------------------------

.. _ground_state_space:

**Header:** ``fiction/technology/sidb/simulation/engines/ground_state_space.hpp``

.. doxygenstruct:: fiction::sidb::simulation::engines::ground_state_space_results
   :members:
.. doxygenfunction:: fiction::sidb::simulation::engines::ground_state_space
