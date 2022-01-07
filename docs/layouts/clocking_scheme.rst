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

.. figure:: docs/img/columnar.png
   :width: 200

.. doxygenfunction:: fiction::columnar_clocking

Row
###

.. figure:: docs/img/row.png
   :width: 200

.. doxygenfunction:: fiction::row_clocking

2DDWave
#######

.. figure:: docs/img/2ddwave.png
   :width: 200

.. doxygenfunction:: fiction::twoddwave_clocking
.. doxygenfunction:: fiction::twoddwave_hex_clocking

USE
###

.. figure:: docs/img/use.png
   :width: 200

.. doxygenfunction:: fiction::use_clocking

RES
###

.. figure:: docs/img/res.png
   :width: 200

.. doxygenfunction:: fiction::res_clocking

ESP
###

.. figure:: docs/img/esp.png
   :width: 200

.. doxygenfunction:: fiction::esp_clocking

BANCS
#####

.. figure:: docs/img/bancs.png
   :width: 150

.. doxygenfunction:: fiction::bancs_clocking

Utility functions
-----------------

.. doxygenfunction:: fiction::ptr(clocking_scheme<clock_zone<Lyt>>&& scheme) noexcept
.. doxygenfunction:: fiction::is_linear_scheme(const clocking_scheme<clock_zone<Lyt>>& scheme) noexcept
.. doxygenfunction:: fiction::get_clocking_scheme(const std::string& name) noexcept
