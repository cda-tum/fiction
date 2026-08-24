.. _clocking schemes:

Clocking Schemes
================

**Header:** ``fiction/layouts/clocking_scheme.hpp``

Clocking is one of the most important differences between conventional CMOS technologies and the FCN domain. Clocking
directs information flow and provides a notion of timing for both combinational and sequential circuits alike. Usually,
clocking information is assigned to the coordinates of a layout by the means of a clocking scheme.

This header provides the ``clocking_scheme`` class that defines general floor plans of clock numbers for
:ref:`clocked layouts<clocked layout>` as well as pre-defined schemes from the literature.

.. doxygenclass:: fiction::layouts::clocking_scheme
   :members:

Pre-defined schemes
-------------------

Open
####

.. doxygenfunction:: fiction::layouts::open_clocking

Columnar
########

.. figure:: /_static/columnar.png
   :width: 200

.. doxygenfunction:: fiction::layouts::columnar_clocking

Row
###

.. figure:: /_static/row.png
   :width: 200

.. doxygenfunction:: fiction::layouts::row_clocking

2DDWave
#######

.. figure:: /_static/2ddwave.png
   :width: 200

.. doxygenfunction:: fiction::layouts::twoddwave_clocking
.. doxygenfunction:: fiction::layouts::twoddwave_hex_clocking

USE
###

.. figure:: /_static/use.png
   :width: 200

.. doxygenfunction:: fiction::layouts::use_clocking

RES
###

.. figure:: /_static/res.png
   :width: 200

.. doxygenfunction:: fiction::layouts::res_clocking

ESR
###

.. figure:: /_static/esr.png
   :width: 200

.. doxygenfunction:: fiction::layouts::esr_clocking

CFE
###

.. figure:: /_static/cfe.png
   :width: 200

.. doxygenfunction:: fiction::layouts::cfe_clocking

Ripple
######

.. figure:: /_static/ripple.png
   :width: 200

.. doxygenfunction:: fiction::layouts::ripple_clocking

SRS
###

.. figure:: /_static/srs.png
   :width: 200

.. doxygenfunction:: fiction::layouts::srs_clocking

BANCS
#####

.. figure:: /_static/bancs.png
   :width: 150

.. doxygenfunction:: fiction::layouts::bancs_clocking

Utility functions
-----------------

.. doxygenfunction:: fiction::layouts::ptr(clocking_scheme<clock_zone<Lyt>>&& scheme) noexcept
.. doxygenfunction:: fiction::layouts::is_linear_scheme(const clocking_scheme<clock_zone<Lyt>>& scheme) noexcept
.. doxygenfunction:: fiction::layouts::get_clocking_scheme(const std::string_view& name) noexcept
