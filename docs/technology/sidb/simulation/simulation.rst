SiDB Simulation
===============

.. toctree::
   :maxdepth: 1

   analysis/analysis
   defects/defects
   engines/engines
   io/io
   logic/logic


Simulation Result
-----------------

.. tabs::
    .. tab:: C++
        **Header:** ``fiction/technology/sidb/simulation/result.hpp``

        .. doxygenstruct:: fiction::sidb::simulation::result
           :members:

    .. tab:: Python
        .. autoclass:: mnt.pyfiction.sidb_simulation_result_100
            :members:
        .. autoclass:: mnt.pyfiction.sidb_simulation_result_111
            :members:

Engine Selectors
----------------

.. tabs::
    .. tab:: C++
        **Header:** ``fiction/technology/sidb/simulation/engine.hpp``

        .. doxygenenum:: fiction::sidb::simulation::engine
        .. doxygenenum:: fiction::sidb::simulation::exact_engine
        .. doxygenenum:: fiction::sidb::simulation::heuristic_engine
        .. doxygenfunction:: fiction::sidb::simulation::engine_name

    .. tab:: Python
        .. autoclass:: mnt.pyfiction.sidb_simulation_engine
            :members:
        .. autoclass:: mnt.pyfiction.exact_sidb_simulation_engine
            :members:
        .. autoclass:: mnt.pyfiction.heuristic_sidb_simulation_engine
            :members:
        .. autofunction:: mnt.pyfiction.sidb_simulation_engine_name

Simulation Equivalence Checking
-------------------------------

.. tabs::
    .. tab:: C++
       **Header:** ``fiction/technology/sidb/simulation/check_simulation_results_for_equivalence.hpp``

        .. doxygenfunction:: fiction::sidb::simulation::check_simulation_results_for_equivalence

    .. tab:: Python
        .. autofunction:: mnt.pyfiction.check_simulation_results_for_equivalence

Ground State Check
------------------

.. tabs::
    .. tab:: C++
        **Header:** ``fiction/technology/sidb/simulation/is_ground_state.hpp``

        .. doxygenfunction:: fiction::sidb::simulation::is_ground_state

    .. tab:: Python
        .. autofunction:: mnt.pyfiction.is_ground_state

Simulation Domain
-----------------

**Header:** ``fiction/technology/sidb/simulation/domain.hpp``

.. doxygenclass:: fiction::sidb::simulation::domain
   :members:
