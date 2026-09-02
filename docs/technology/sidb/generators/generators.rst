SiDB Generators
===============

Random SiDB Layout Generator
----------------------------

.. tabs::
    .. tab:: C++
        **Header:** ``fiction/technology/sidb/generators/random_layout_generator.hpp``

        .. doxygenstruct:: fiction::sidb::generators::generate_random_layout_params
           :members:
        .. doxygenfunction:: fiction::sidb::generators::generate_random_layout
        .. doxygenfunction:: fiction::sidb::generators::generate_multiple_random_layouts

    .. tab:: Python
        .. autoclass:: mnt.pyfiction.generate_random_sidb_layout_params
            :members:
        .. autofunction:: mnt.pyfiction.generate_random_sidb_layout
        .. autofunction:: mnt.pyfiction.generate_multiple_random_sidb_layouts

Is SiDB gate design deemed impossible
-------------------------------------

**Header:** ``fiction/technology/sidb/generators/is_gate_design_impossible.hpp``

.. doxygenstruct:: fiction::sidb::generators::is_gate_design_impossible_params
   :members:
.. doxygenfunction:: fiction::sidb::generators::is_gate_design_impossible

.. toctree::
   :maxdepth: 1

   design_gates
   on_the_fly_circuit_design
