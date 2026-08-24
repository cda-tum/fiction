Physical Properties
-------------------

Area Requirements
#################

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

MagCAD Magnet Count
###################

**Header:** ``fiction/technology/inml/magcad_magnet_count.hpp``

.. doxygenfunction:: fiction::inml::magcad_magnet_count
