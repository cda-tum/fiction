Design Rule Violations (DRVs)
-----------------------------

.. tabs::
    .. tab:: C++
        **Header:** ``fiction/verification/design_rule_violations.hpp``

        .. doxygenstruct:: fiction::verification::gate_level_drv_params
           :members:
        .. doxygenstruct:: fiction::verification::gate_level_drv_stats
           :members:
        .. doxygenfunction:: fiction::verification::gate_level_drvs

    .. tab:: Python
        .. autoclass:: mnt.pyfiction.gate_level_drv_params
            :members:
        .. autofunction:: mnt.pyfiction.gate_level_drvs

.. _equiv:

Equivalence Checking
--------------------

.. tabs::
    .. tab:: C++
        **Header:** ``fiction/verification/equivalence_checking.hpp``

        .. doxygenenum:: fiction::verification::eq_type
        .. doxygenstruct:: fiction::verification::equivalence_checking_stats
           :members:
        .. doxygenfunction:: fiction::verification::equivalence_checking

    .. tab:: Python
        .. autoclass:: mnt.pyfiction.eq_type
            :members:
        .. autofunction:: mnt.pyfiction.equivalence_checking

Virtual Miter
-------------

.. tabs::
    .. tab:: C++
        **Header:** ``fiction/verification/virtual_miter.hpp``

        .. doxygenfunction:: fiction::verification::virtual_miter
