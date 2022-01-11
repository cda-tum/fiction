Synchronization Element Layout
==============================

**Header:** ``fiction/layouts/synchronization_element_layout.hpp``

The synchronization element layout can be layered on top of any clocked layout type at compile time to extend its
functionality by a notion of `synchronization elements <https://ieeexplore.ieee.org/abstract/document/8626294>`_ (SEs).
SEs are a technology extension that proposes to utilize the external clock generator to produce additional asymmetric
clock signals with extended hold phases. Assigned to a clock zone, they would transform it into a latch-like device that
stalls information over multiple full clock cycles. Such clock zones are called SEs.

.. doxygenclass:: fiction::synchronization_element_layout
   :members:
