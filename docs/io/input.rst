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

**Header:** ``fiction/io/read_fqca_layout.hpp``

.. doxygenfunction:: fiction::read_fqca_layout(std::istream& is, const std::string& name = "")
.. doxygenfunction:: fiction::read_fqca_layout(const std::string& filename, const std::string& name = "")

.. doxygenclass:: fiction::unsupported_character_exception
.. doxygenclass:: fiction::undefined_cell_label_exception
.. doxygenclass:: fiction::unrecognized_cell_definition_exception