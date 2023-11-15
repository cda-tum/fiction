.. _exact:

SMT-based Exact Physical Design
-------------------------------

Utilizes the SMT solver `Z3 <https://github.com/Z3Prover/z3>`_ to generate minimal FCN gate-level layouts from logic
network specifications under constraints. This approach finds exact results but has a large runtime overhead.

.. tabs::
    .. tab:: C++
        **Header:** ``fiction/algorithms/physical_design/exact.hpp``

        .. doxygenstruct:: fiction::exact_physical_design_params
           :members:
        .. doxygenfunction:: fiction::exact
        .. doxygenfunction:: fiction::exact_with_blacklist

    .. tab:: Python
        .. autoclass:: fiction.pyfiction.exact_params
            :members:
        .. autofunction:: fiction.pyfiction.exact_cartesian
        .. autofunction:: fiction.pyfiction.exact_hexagonal
