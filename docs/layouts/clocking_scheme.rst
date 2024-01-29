.. _clocking schemes:

Clocking Schemes
================

**Header:** ``fiction/layouts/clocking_scheme.hpp``

Clocking is one of the most important differences between conventional CMOS technologies and the FCN domain. Clocking
directs information flow and provides a notion of timing for both combinational and sequential circuits alike. Usually,
clocking information is assigned to the coordinates of a layout by the means of a clocking scheme.

This header provides the ``clocking_scheme`` class that defines general floor plans of clock numbers for
:ref:`clocked layouts<clocked layout>` as well as pre-defined schemes from the literature.

.. doxygenclass:: fiction::clocking_scheme
   :members:

Pre-defined schemes
-------------------

Open
####

.. doxygenfunction:: fiction::open_clocking

Columnar
########

.. figure:: /_static/columnar.png
   :width: 200

.. doxygenfunction:: fiction::columnar_clocking

Row
###

.. figure:: /_static/row.png
   :width: 200

.. doxygenfunction:: fiction::row_clocking

2DDWave
#######

.. figure:: /_static/2ddwave.png
   :width: 200

.. doxygenfunction:: fiction::twoddwave_clocking
.. doxygenfunction:: fiction::twoddwave_hex_clocking

USE
###

.. figure:: /_static/use.png
   :width: 200

.. doxygenfunction:: fiction::use_clocking

RES
###

.. figure:: /_static/res.png
   :width: 200

.. doxygenfunction:: fiction::res_clocking

ESR
###

.. figure:: /_static/esr.png
   :width: 200

.. doxygenfunction:: fiction::esr_clocking

CFE
###

.. figure:: /_static/cfe.png
   :width: 200

.. doxygenfunction:: fiction::cfe_clocking

Ripple
######

.. figure:: /_static/ripple.png
   :width: 200

.. doxygenfunction:: fiction::ripple_clocking

BANCS
#####

.. figure:: /_static/bancs.png
   :width: 150

.. doxygenfunction:: fiction::bancs_clocking

Utility functions
-----------------

.. doxygenfunction:: fiction::ptr(clocking_scheme<clock_zone<Lyt>>&& scheme) noexcept
.. doxygenfunction:: fiction::is_linear_scheme(const clocking_scheme<clock_zone<Lyt>>& scheme) noexcept
.. doxygenfunction:: fiction::get_clocking_scheme(const std::string_view& name) noexcept
