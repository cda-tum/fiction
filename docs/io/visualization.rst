Visualization
-------------

Visualization of logic networks and FCN layouts.

Layout Printing
###############

**Header:** ``fiction/layouts/io/print_layout.hpp``

.. doxygenfunction:: fiction::layouts::io::print_gate_level_layout
.. doxygenfunction:: fiction::layouts::io::print_cell_level_layout
.. doxygenfunction:: fiction::layouts::io::print_sidb_layout
.. doxygenfunction:: fiction::layouts::io::print_layout

Graphviz (DOT) Drawers
######################

**Header:** ``fiction/networks/io/dot_drawers.hpp``

.. doxygenclass:: fiction::networks::io::technology_dot_drawer
.. doxygenclass:: fiction::networks::io::color_view_drawer
.. doxygenclass:: fiction::networks::io::edge_color_view_drawer
.. doxygenclass:: fiction::layouts::io::simple_gate_layout_tile_drawer
.. doxygenclass:: fiction::layouts::io::gate_layout_cartesian_drawer
.. doxygenclass:: fiction::layouts::io::gate_layout_shifted_cartesian_drawer
.. doxygenclass:: fiction::layouts::io::gate_layout_hexagonal_drawer


.. tabs::
    .. tab:: C++
        .. doxygenfunction:: fiction::layouts::io::write_dot_layout(const Lyt& lyt, std::ostream& os, const Drawer& drawer = {})
        .. doxygenfunction:: fiction::layouts::io::write_dot_layout(const Lyt& lyt, const std::string_view& filename, const Drawer& drawer = {})

    .. tab:: Python
        .. autofunction:: mnt.pyfiction.write_dot_layout

SVG Images
##########

.. tabs::
    .. tab:: C++
        **Header:** ``fiction/layouts/io/write_svg_layout.hpp``

        .. doxygenstruct:: fiction::layouts::io::write_qca_layout_svg_params
           :members:

        .. doxygenfunction:: fiction::layouts::io::write_qca_layout_svg(const Lyt& lyt, std::ostream& os, const write_qca_layout_svg_params& ps = {})
        .. doxygenfunction:: fiction::layouts::io::write_qca_layout_svg(const Lyt& lyt, const std::string_view& filename, const write_qca_layout_svg_params& ps = {})

        .. doxygenclass:: fiction::layouts::io::unsupported_cell_type_exception

        .. doxygenstruct:: fiction::layouts::io::write_sidb_layout_svg_params
           :members:

        .. doxygenfunction:: fiction::layouts::io::write_sidb_layout_svg(const Lyt& lyt, std::ostream& os, const write_sidb_layout_svg_params& ps = {})
        .. doxygenfunction:: fiction::layouts::io::write_sidb_layout_svg(const Lyt& lyt, const std::string_view& filename, const write_sidb_layout_svg_params& ps = {})

        .. doxygenfunction:: fiction::layouts::io::write_mol_qca_layout_svg(const Lyt& lyt, std::ostream& os, const write_qca_layout_svg_params& ps = {})
        .. doxygenfunction:: fiction::layouts::io::write_mol_qca_layout_svg(const Lyt& lyt, const std::string_view& filename, const write_qca_layout_svg_params& ps = {})


    .. tab:: Python
        .. autoclass:: mnt.pyfiction.write_qca_layout_svg_params
            :members:
        .. autofunction:: mnt.pyfiction.write_qca_layout_svg

        .. autoclass:: mnt.pyfiction.write_sidb_layout_svg_params
            :members:
        .. autofunction:: mnt.pyfiction.write_sidb_layout_svg
        .. autofunction:: mnt.pyfiction.write_sidb_layout_svg_to_string
        .. autofunction:: mnt.pyfiction.write_mol_qca_layout_svg
