Physical Properties
-------------------

Area Requirements
#################

.. tabs::
    .. tab:: C++
        **Header:** ``fiction/technology/area.hpp``

        .. doxygenfunction:: fiction::area(const Lyt& lyt, area_params<technology<Lyt>, AreaType>& ps = {}, area_stats<AreaType>* pst = nullptr)
        .. doxygenfunction:: fiction::area(const bounding_box_2d<Lyt>& bb, area_params<technology<Lyt>, AreaType>& ps = {}, area_stats<AreaType>* pst = nullptr)

    .. tab:: Python
        .. autofunction:: mnt.pyfiction.area

MagCAD Magnet Count
###################

**Header:** ``fiction/technology/magcad_magnet_count.hpp``

.. doxygenfunction:: fiction::magcad_magnet_count
