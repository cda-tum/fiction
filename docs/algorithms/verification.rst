Design Rule Violations (DRVs)
-----------------------------

.. tabs::
    .. tab:: C++
        **Header:** ``fiction/algorithms/verification/design_rule_violations.hpp``

        .. doxygenstruct:: fiction::gate_level_drv_params
           :members:
        .. doxygenstruct:: fiction::gate_level_drv_stats
           :members:
        .. doxygenfunction:: fiction::gate_level_drvs

    .. tab:: Python
        .. autoclass:: mnt.pyfiction.gate_level_drv_params
            :members:
        .. autofunction:: mnt.pyfiction.gate_level_drvs

.. _equiv:

Equivalence Checking
--------------------

.. tabs::
    .. tab:: C++
        **Header:** ``fiction/algorithms/verification/equivalence_checking.hpp``

        .. doxygenenum:: fiction::eq_type
        .. doxygenstruct:: fiction::equivalence_checking_stats
           :members:
        .. doxygenfunction:: fiction::equivalence_checking

    .. tab:: Python
        .. autoclass:: mnt.pyfiction.eq_type
            :members:
        .. autofunction:: mnt.pyfiction.equivalence_checking

Virtual Miter
-------------

.. tabs::
    .. tab:: C++
        **Header:** ``fiction/algorithms/verification/virtual_miter.hpp``

        .. doxygenfunction:: fiction::virtual_miter
