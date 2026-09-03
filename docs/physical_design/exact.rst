.. _exact:

SMT-based Exact Physical Design
-------------------------------

Utilizes the SMT solver `Z3 <https://github.com/Z3Prover/z3>`_ to generate minimal FCN gate-level layouts from logic
network specifications under constraints. This approach finds exact results but has a large runtime overhead.

.. tabs::
    .. tab:: C++
        **Header:** ``fiction/physical_design/exact.hpp``

        .. doxygenstruct:: fiction::physical_design::exact_physical_design_params
           :members:
        .. doxygenstruct:: fiction::physical_design::exact_physical_design_stats
           :members:
        .. doxygenfunction:: fiction::physical_design::exact(const Ntk& ntk, const exact_physical_design_params& ps = {}, exact_physical_design_stats *pst = nullptr)
        .. doxygenfunction:: fiction::physical_design::exact_with_blacklist(const Ntk& ntk, const sidb::surface_black_list<Lyt, fcn::port_direction>& black_list, exact_physical_design_params ps  = {}, exact_physical_design_stats* pst = nullptr)

    .. tab:: Python
        .. autoclass:: mnt.pyfiction.exact_params
            :members:
        .. autofunction:: mnt.pyfiction.exact_cartesian
        .. autofunction:: mnt.pyfiction.exact_hexagonal
