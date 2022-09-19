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

.. doxygenfunction:: fiction::read_sqd_layout(std::istream& is, const std::string& name = "")
.. doxygenfunction:: fiction::read_sqd_layout(Lyt& lyt, std::istream& is)
.. doxygenfunction:: fiction::read_sqd_layout(const std::string& filename, const std::string& name = "")
.. doxygenfunction:: fiction::read_sqd_layout(Lyt& lyt, const std::string& filename)

**Header:** ``fiction/io/read_fqca_layout.hpp``

.. doxygenfunction:: fiction::read_fqca_layout(std::istream& is, const std::string& name = "")
.. doxygenfunction:: fiction::read_fqca_layout(const std::string& filename, const std::string& name = "")
