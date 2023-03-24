Input Parsing
-------------

Read logic networks, truth tables, or FCN layouts from file into data types.

Logic Networks
##############

**Header:** ``fiction/io/network_reader.hpp``

.. doxygenclass:: fiction::network_reader
   :members:

Truth Tables
############

**Header:** ``fiction/io/tt_reader.hpp``

.. doxygenclass:: fiction::tt_reader
   :members:

Cell-level Layouts
##################

**Header:** ``fiction/io/read_sqd_layout.hpp``

.. doxygenfunction:: fiction::read_sqd_layout(std::istream& is, const std::string_view& name = "")
.. doxygenfunction:: fiction::read_sqd_layout(Lyt& lyt, std::istream& is)
.. doxygenfunction:: fiction::read_sqd_layout(const std::string_view& filename, const std::string_view& name = "")
.. doxygenfunction:: fiction::read_sqd_layout(Lyt& lyt, const std::string_view& filename)

.. doxygenclass:: fiction::sqd_parsing_error

**Header:** ``fiction/io/read_fqca_layout.hpp``

.. doxygenfunction:: fiction::read_fqca_layout(std::istream& is, const std::string_view& name = "")
.. doxygenfunction:: fiction::read_fqca_layout(const std::string_view& filename, const std::string_view& name = "")

.. doxygenclass:: fiction::unsupported_character_exception
.. doxygenclass:: fiction::undefined_cell_label_exception
.. doxygenclass:: fiction::unrecognized_cell_definition_exception

SiDB Surface Defects
####################

**Header:** ``fiction/io/read_sidb_surface_defects.hpp``

.. doxygenfunction:: fiction::read_sidb_surface_defects(std::istream& is, const std::string_view& name = "")
.. doxygenfunction:: fiction::read_sidb_surface_defects(const std::string_view& filename, const std::string_view& name = "")

.. doxygenclass:: fiction::unsupported_defect_index_exception
.. doxygenclass:: fiction::missing_sidb_position_exception
