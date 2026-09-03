.. _fcn-gate-libraries:

FCN
===

.. toctree::
   :maxdepth: 1

   io/io


Abstract Gate Library
---------------------

**Header:** ``fiction/technology/fcn/gate_library.hpp``

.. doxygenclass:: fiction::fcn::gate_library
   :members:

.. doxygenclass:: fiction::fcn::unsupported_gate_type_exception
    :members:
.. doxygenclass:: fiction::fcn::unsupported_gate_orientation_exception
    :members:

**Header:** ``fiction/technology/fcn/cell_ports.hpp``

.. doxygenstruct:: fiction::fcn::port_position
   :members:
.. doxygenstruct:: fiction::fcn::port_direction
   :members:
.. doxygenstruct:: fiction::fcn::port_list
   :members:

Area Requirements
-----------------

.. tabs::
    .. tab:: C++
        **Header:** ``fiction/technology/fcn/area.hpp``

        .. doxygenstruct:: fiction::fcn::area_params
            :members:
        .. doxygenstruct:: fiction::fcn::area_stats
            :members:

        .. doxygenfunction:: fiction::fcn::area(const Lyt& lyt, const area_params<technology<Lyt>>& ps = {}, area_stats* pst = nullptr)
        .. doxygenfunction:: fiction::fcn::area(const layouts::bounding_box_2d<Lyt>& bb, const area_params<technology<Lyt>>& ps = {}, area_stats* pst = nullptr)

    .. tab:: Python
        .. autofunction:: mnt.pyfiction.area
