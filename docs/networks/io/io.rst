Network I/O
===========

Logic Networks
--------------

.. tabs::
    .. tab:: C++
        **Header:** ``fiction/networks/io/network_reader.hpp``

        .. doxygenclass:: fiction::networks::io::network_reader
           :members:

    .. tab:: Python
        .. autofunction:: mnt.pyfiction.read_technology_network

Graphviz (DOT) Drawers
----------------------

.. tabs::
    .. tab:: C++
        **Header:** ``fiction/networks/io/dot_drawers.hpp``

        .. doxygenclass:: fiction::networks::io::technology_dot_drawer
        .. doxygenclass:: fiction::networks::io::color_view_drawer
        .. doxygenclass:: fiction::networks::io::edge_color_view_drawer

        .. doxygenfunction:: fiction::networks::io::write_dot_network(const Ntk& ntk, std::ostream& os, const Drawer& drawer = {})
        .. doxygenfunction:: fiction::networks::io::write_dot_network(const Ntk& ntk, const std::string_view& filename, const Drawer& drawer = {})

    .. tab:: Python
        .. autofunction:: mnt.pyfiction.write_dot_network
