Reducing Wiring in 2DDWave-clocked Cartesian Layouts
----------------------------------------------------

**Header:** ``fiction/algorithms/physical_design/wiring_reduction.hpp``

This algorithm performs wiring reduction on a 2DDWave clocking scheme layout by iteratively
finding paths with excess wiring and deleting them. The process continues until no further wires can be deleted.

.. doxygenstruct:: fiction::wiring_reduction_stats
   :members:
.. doxygenfunction:: fiction::wiring_reduction
