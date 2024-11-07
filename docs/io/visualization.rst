Visualization
-------------

Visualization of logic networks and FCN layouts.

Layout Printing
###############

**Header:** ``fiction/io/print_layout.hpp``

.. doxygenfunction:: fiction::print_gate_level_layout
.. doxygenfunction:: fiction::print_cell_level_layout
.. doxygenfunction:: fiction::print_sidb_layout
.. doxygenfunction:: fiction::print_layout

Graphviz (DOT) Drawers
######################

**Header:** ``fiction/io/dot_drawers.hpp``

.. doxygenclass:: fiction::technology_dot_drawer
.. doxygenclass:: fiction::color_view_drawer
.. doxygenclass:: fiction::edge_color_view_drawer
.. doxygenclass:: fiction::simple_gate_layout_tile_drawer
.. doxygenclass:: fiction::gate_layout_cartesian_drawer
.. doxygenclass:: fiction::gate_layout_shifted_cartesian_drawer
.. doxygenclass:: fiction::gate_layout_hexagonal_drawer


.. tabs::
    .. tab:: C++
        .. doxygenfunction:: fiction::write_dot_layout(const Lyt& lyt, std::ostream& os, const Drawer& drawer = {})
        .. doxygenfunction:: fiction::write_dot_layout(const Lyt& lyt, const std::string_view& filename, const Drawer& drawer = {})

    .. tab:: Python
        .. autofunction:: mnt.pyfiction.write_dot_layout

SVG Images
##########

.. tabs::
    .. tab:: C++
        **Header:** ``fiction/io/write_svg_layout.hpp``

        .. doxygenstruct:: fiction::write_qca_layout_svg_params
           :members:

        .. doxygenfunction:: fiction::write_qca_layout_svg(const Lyt& lyt, std::ostream& os, write_qca_layout_svg_params ps = {})
        .. doxygenfunction:: fiction::write_qca_layout_svg(const Lyt& lyt, const std::string_view& filename, write_qca_layout_svg_params ps = {})

        .. doxygenclass:: fiction::unsupported_cell_type_exception

        **Header:** ``fiction/io/write_sidb_layout_to_svg_in_siqad_style.hpp``

        .. doxygenstruct:: fiction::color_mode
           :members:

        .. doxygenstruct:: fiction::write_sidb_layout_svg_in_siqad_style_params
           :members:

        .. doxygenfunction:: fiction::write_sidb_layout_to_svg_in_siqad_style(const Lyt& lyt, std::ostream& os,
                                                 const write_sidb_layout_svg_in_siqad_style_params& ps = {})
        .. doxygenfunction:: fiction::write_sidb_layout_to_svg_in_siqad_style(const Lyt& lyt, const std::string_view& filename,
                                             const write_sidb_layout_svg_in_siqad_style_params& ps = {})
        .. doxygenclass:: fiction::write_sidb_layout_to_svg_and_return_as_string(const Lyt& lyt, const write_sidb_layout_svg_in_siqad_style_params& ps = {})

        .. doxygenclass:: fiction::unsupported_cell_type_exception

    .. tab:: Python
        .. autoclass:: mnt.pyfiction.write_qca_layout_svg_params
            :members:
        .. autofunction:: mnt.pyfiction.write_qca_layout_svg
