SMT-based Exact Physical Design
===============================

**Header:** ``fiction/algorithms/physical_design/exact.hpp``

Utilizes the SMT solver `Z3 <https://github.com/Z3Prover/z3>`_ to generate minimal FCN gate-level layouts from logic
network specifications under constraints. This approach finds exact results but has a large runtime overhead.

.. doxygenfunction:: fiction::exact

