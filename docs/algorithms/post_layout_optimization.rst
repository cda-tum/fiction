.. _post_layout_optimization:

Optimizing 2DDWave-clocked Cartesian Layouts
--------------------------------------------

.. _post_layout_opt:

**Header:** ``fiction/algorithms/physical_design/post_layout_optimization.hpp``

This algorithm aims to decrease the overall layout area of a given 2DDWave-clocked Cartesian layout that has been
generated using either heuristic methods or machine learning techniques. It achieves this objective by strategically
repositioning gates within the layout, removing excess wiring using the ``wiring_reduction`` algorithm (cf. :ref:`wiring_reduction`), and effectively
relocating outputs to more favorable positions.

.. doxygenstruct:: fiction::post_layout_optimization_stats
   :members:
.. doxygenfunction:: fiction::post_layout_optimization
