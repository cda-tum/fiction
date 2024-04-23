.. _post_layout_optimization:

Optimizing 2DDWave-clocked Cartesian Layouts
--------------------------------------------

.. _post_layout_opt:

This algorithm aims to decrease the overall layout area of a given 2DDWave-clocked Cartesian layout that has been
generated using either heuristic methods or machine learning techniques. It achieves this objective by strategically
repositioning gates within the layout, removing excess wiring using the ``wiring_reduction`` algorithm (cf. :ref:`wiring_reduction`), and effectively
relocating outputs to more favorable positions.

.. tabs::
    .. tab:: C++
        **Header:** ``fiction/algorithms/physical_design/post_layout_optimization.hpp``

        .. doxygenstruct:: fiction::post_layout_optimization_params
           :members:
        .. doxygenstruct:: fiction::post_layout_optimization_stats
           :members:
        .. doxygenfunction:: fiction::post_layout_optimization(const Lyt& lyt, post_layout_optimization_params ps = {}, post_layout_optimization_stats* pst = nullptr)

    .. tab:: Python
        .. autoclass:: mnt.pyfiction.post_layout_optimization_params
            :members:
        .. autoclass:: mnt.pyfiction.post_layout_optimization_stats
            :members:
        .. autofunction:: mnt.pyfiction.post_layout_optimization
