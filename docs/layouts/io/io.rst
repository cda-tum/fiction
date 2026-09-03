Layout I/O
==========

Gate-level Layouts
------------------

Can be used to read gate-level layout files (``.fgl``) as offered by `MNT Bench <https://www.cda.cit.tum.de/mntbench/>`_.

.. tabs::
    .. tab:: C++

        **Header:** ``fiction/layouts/io/read_fgl_layout.hpp``

        .. doxygenfunction:: fiction::layouts::io::read_fgl_layout(std::istream& is, const std::string_view& name = "")
        .. doxygenfunction:: fiction::layouts::io::read_fgl_layout(Lyt& lyt, std::istream& is)
        .. doxygenfunction:: fiction::layouts::io::read_fgl_layout(const std::string_view& filename, const std::string_view& name = "")
        .. doxygenfunction:: fiction::layouts::io::read_fgl_layout(Lyt& lyt, const std::string_view& filename)

        .. doxygenclass:: fiction::layouts::io::fgl_parsing_error

    .. tab:: Python
        .. autofunction:: mnt.pyfiction.read_cartesian_fgl_layout
        .. autofunction:: mnt.pyfiction.read_shifted_cartesian_fgl_layout
        .. autofunction:: mnt.pyfiction.read_hexagonal_fgl_layout

        .. autoclass:: mnt.pyfiction.fgl_parsing_error
            :members:

Technology-independent Gate-level Layouts
-----------------------------------------

Can be used to generate gate-level layout files (``.fgl``) as offered by `MNT Bench <https://www.cda.cit.tum.de/mntbench/>`_.

.. tabs::
    .. tab:: C++
        **Header:** ``fiction/layouts/io/write_fgl_layout.hpp``

        .. doxygenfunction:: fiction::layouts::io::write_fgl_layout(const Lyt& lyt, std::ostream& os)
        .. doxygenfunction:: fiction::layouts::io::write_fgl_layout(const Lyt& lyt, const std::string_view& filename)


    .. tab:: Python
        .. autofunction:: mnt.pyfiction.write_fgl_layout


Layout Printing
---------------

**Header:** ``fiction/layouts/io/print_layout.hpp``

.. doxygenfunction:: fiction::layouts::io::print_gate_level_layout
.. doxygenfunction:: fiction::layouts::io::print_cell_level_layout
.. doxygenfunction:: fiction::layouts::io::print_sidb_layout
.. doxygenfunction:: fiction::layouts::io::print_layout

Graphviz (DOT) Drawers
----------------------

.. tabs::
    .. tab:: C++
        **Header:** ``fiction/layouts/io/layout_drawers.hpp``

        .. doxygenclass:: fiction::layouts::io::simple_gate_layout_tile_drawer
        .. doxygenclass:: fiction::layouts::io::gate_layout_cartesian_drawer
        .. doxygenclass:: fiction::layouts::io::gate_layout_shifted_cartesian_drawer
        .. doxygenclass:: fiction::layouts::io::gate_layout_hexagonal_drawer

        .. doxygenfunction:: fiction::layouts::io::write_dot_layout(const Lyt& lyt, std::ostream& os, const Drawer& drawer = {})
        .. doxygenfunction:: fiction::layouts::io::write_dot_layout(const Lyt& lyt, const std::string_view& filename, const Drawer& drawer = {})

    .. tab:: Python
        .. autofunction:: mnt.pyfiction.write_dot_layout
