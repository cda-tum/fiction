Input Parsing
-------------

Read logic networks, truth tables, or FCN layouts from file into data types.

Logic Networks
##############

.. tabs::
    .. tab:: C++
        **Header:** ``fiction/io/network_reader.hpp``

        .. doxygenclass:: fiction::network_reader
           :members:

    .. tab:: Python
        .. autofunction:: mnt.pyfiction.read_technology_network


Truth Tables
############

**Header:** ``fiction/io/tt_reader.hpp``

.. doxygenclass:: fiction::tt_reader
   :members:


Gate-level Layouts
##################

Can be used to read gate-level layout files (``.fgl``) as offered by `MNT Bench <https://www.cda.cit.tum.de/mntbench/>`_.

.. tabs::
    .. tab:: C++

        **Header:** ``fiction/io/read_fgl_layout.hpp``

        .. doxygenfunction:: fiction::read_fgl_layout(std::istream& is, const std::string_view& name = "")
        .. doxygenfunction:: fiction::read_fgl_layout(Lyt& lyt, std::istream& is)
        .. doxygenfunction:: fiction::read_fgl_layout(const std::string_view& filename, const std::string_view& name = "")
        .. doxygenfunction:: fiction::read_fgl_layout(Lyt& lyt, const std::string_view& filename)

        .. doxygenclass:: fiction::fgl_parsing_error

    .. tab:: Python
        .. autofunction:: mnt.pyfiction.read_cartesian_fgl_layout
        .. autofunction:: mnt.pyfiction.read_shifted_cartesian_fgl_layout
        .. autofunction:: mnt.pyfiction.read_hexagonal_fgl_layout

        .. autoclass:: mnt.pyfiction.fgl_parsing_error
            :members:


Cell-level Layouts
##################

.. tabs::
    .. tab:: C++

        **Header:** ``fiction/io/read_sqd_layout.hpp``

        .. doxygenfunction:: fiction::read_sqd_layout(std::istream& is, const std::string_view& name = "")
        .. doxygenfunction:: fiction::read_sqd_layout(Lyt& lyt, std::istream& is)
        .. doxygenfunction:: fiction::read_sqd_layout(const std::string_view& filename, const std::string_view& name = "")
        .. doxygenfunction:: fiction::read_sqd_layout(Lyt& lyt, const std::string_view& filename)

        .. doxygenclass:: fiction::sqd_parsing_error

    .. tab:: Python
        .. autofunction:: mnt.pyfiction.read_sqd_layout_100
        .. autofunction:: mnt.pyfiction.read_sqd_layout_111

        .. autoclass:: mnt.pyfiction.sqd_parsing_error
            :members:

.. tabs::
    .. tab:: C++
        **Header:** ``fiction/io/read_fqca_layout.hpp``

        .. doxygenfunction:: fiction::read_fqca_layout(std::istream& is, const std::string_view& name = "")
        .. doxygenfunction:: fiction::read_fqca_layout(const std::string_view& filename, const std::string_view& name = "")

        .. doxygenclass:: fiction::unsupported_character_exception
        .. doxygenclass:: fiction::undefined_cell_label_exception
        .. doxygenclass:: fiction::unrecognized_cell_definition_exception

    .. tab:: Python
        .. autofunction:: mnt.pyfiction.read_fqca_layout


SiDB Surface Defects
####################

**Header:** ``fiction/io/read_sidb_surface_defects.hpp``

.. doxygenfunction:: fiction::read_sidb_surface_defects(std::istream& is, const std::string_view& name = "")
.. doxygenfunction:: fiction::read_sidb_surface_defects(const std::string_view& filename, const std::string_view& name = "")

.. doxygenclass:: fiction::unsupported_defect_index_exception
.. doxygenclass:: fiction::missing_sidb_position_exception
