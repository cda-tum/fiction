.. _exact:

SMT-based Exact Physical Design
-------------------------------

**Header:** ``fiction/algorithms/physical_design/exact.hpp``

Utilizes the SMT solver `Z3 <https://github.com/Z3Prover/z3>`_ to generate minimal FCN gate-level layouts from logic
network specifications under constraints. This approach finds exact results but has a large runtime overhead.

.. doxygenstruct:: fiction::exact_physical_design_params
   :members:
.. doxygenfunction:: fiction::exact(const Ntk& ntk, const exact_physical_design_params<Lyt>& ps = {}, exact_physical_design_stats* pst = nullptr)
