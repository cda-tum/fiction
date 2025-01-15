.. _on_the_fly_design:

SiDB Circuit Design Algorithm in the Presence of Atomic Defects
---------------------------------------------------------------

This algorithm is designed to create SiDB circuits on a clocked surface, accommodating the presence of atomic defects.

1. **Blacklist Generation**:

   Initially, a blacklist of gate-tile pairs is generated. This blacklist is based on the locations of neutrally charged atomic defects and their overlap with the I/O pins of the SiDB skeletons.

2. **Gate-Level Layout Design**:

   Using the generated blacklist, a gate-level layout is designed with the ``exact`` algorithm. This process involves:

   - **Valid Layout Found**:

     If a valid gate-level layout is found, the corresponding gates are implemented with SiDBs.

   - **Invalid Layout**:

     If a valid layout is not found, the blacklist is updated, and the placement and routing process is repeated.

This iterative approach ensures that the designed SiDB circuits can effectively handle defects present on the surface.


**Header:** ``fiction/algorithms/physical_design/on_the_fly_circuit_design.hpp``

.. doxygenstruct:: fiction::on_the_fly_sidb_circuit_design_on_defective_surface_params
   :members:
.. doxygenstruct:: fiction::on_the_fly_sidb_circuit_design_params
   :members:
.. doxygenstruct:: fiction::on_the_fly_sidb_circuit_design_on_defective_surface_stats
    :members:
.. doxygenfunction:: fiction::on_the_fly_sidb_circuit_design_on_defective_surface
.. doxygenfunction:: fiction::on_the_fly_sidb_circuit_design
