Physical Simulation
-------------------

Write cell-level layouts into physical simulator files for various technologies.

QCADesigner
###########

**Header:** ``fiction/io/write_qca_layout.hpp``

.. doxygenfunction:: fiction::write_qca_layout(const Lyt& lyt, std::ostream& os, write_qca_layout_params ps = {})
.. doxygenfunction:: fiction::write_qca_layout(const Lyt& lyt, const std::string& filename, write_qca_layout_params ps = {})

QCA-STACK
#########

**Header:** ``fiction/io/write_fqca_layout.hpp``

.. doxygenfunction:: fiction::write_fqca_layout(const Lyt& lyt, std::ostream& os, write_fqca_layout_params ps = {})
.. doxygenfunction:: fiction::write_fqca_layout(const Lyt& lyt, const std::string& filename, write_fqca_layout_params ps = {})

**Header:** ``fiction/io/read_fqca_layout.hpp``

.. doxygenfunction:: fiction::read_fqca_layout(std::istream& is, const std::string& name = "")
.. doxygenfunction:: fiction::read_fqca_layout(const std::string& filename, const std::string& name = "")

SiQAD
#####

**Header:** ``fiction/io/write_sqd_layout.hpp``

.. doxygenfunction:: fiction::write_sqd_layout(const Lyt& lyt, std::ofstream& os)
.. doxygenfunction:: fiction::write_sqd_layout(const Lyt& lyt, const std::string& filename)
