Physical Properties
-------------------

Area Requirements
#################

.. tabs::
    .. tab:: C++
        **Header:** ``fiction/technology/area.hpp``

        .. doxygenstruct:: fiction::area_params
            :members:
        .. doxygenstruct:: fiction::area_stats
            :members:

        .. doxygenfunction:: fiction::area(const Lyt& lyt, const area_params<technology<Lyt>>& ps = {}, area_stats* pst = nullptr)
        .. doxygenfunction:: fiction::area(const bounding_box_2d<Lyt>& bb, const area_params<technology<Lyt>>& ps = {}, area_stats* pst = nullptr)

    .. tab:: Python
        .. autofunction:: mnt.pyfiction.area

MagCAD Magnet Count
###################

**Header:** ``fiction/technology/magcad_magnet_count.hpp``

.. doxygenfunction:: fiction::magcad_magnet_count
