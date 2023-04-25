Physical Simulation
-------------------

Write cell-level layouts into physical simulator files for various technologies.

QCADesigner
###########

**Header:** ``fiction/io/write_qca_layout.hpp``

.. doxygenstruct:: fiction::write_qca_layout_params
   :members:

.. doxygenfunction:: fiction::write_qca_layout(const Lyt& lyt, std::ostream& os, write_qca_layout_params ps = {})
.. doxygenfunction:: fiction::write_qca_layout(const Lyt& lyt, const std::string_view& filename, write_qca_layout_params ps = {})

QCA-STACK
#########

**Header:** ``fiction/io/write_fqca_layout.hpp``

.. doxygenstruct:: fiction::write_fqca_layout_params
   :members:

.. doxygenfunction:: fiction::write_fqca_layout(const Lyt& lyt, std::ostream& os, write_fqca_layout_params ps = {})
.. doxygenfunction:: fiction::write_fqca_layout(const Lyt& lyt, const std::string_view& filename, write_fqca_layout_params ps = {})

.. doxygenclass:: fiction::out_of_cell_names_exception

**Header:** ``fiction/io/read_fqca_layout.hpp``

.. doxygenfunction:: fiction::read_fqca_layout(std::istream& is, const std::string_view& name = "")
.. doxygenfunction:: fiction::read_fqca_layout(const std::string_view& filename, const std::string_view& name = "")

ToPoliNano
##########

**Header:** ``fiction/io/write_qcc_layout.hpp``

.. doxygenstruct:: fiction::write_qcc_layout_params
   :members:

.. doxygenfunction:: fiction::write_qcc_layout(const Lyt& lyt, std::ostream& os, write_qcc_layout_params ps = {})
.. doxygenfunction:: fiction::write_qcc_layout(const Lyt& lyt, const std::string_view& filename, write_qcc_layout_params ps = {})

MagCAD & SCERPA
###############

**Header:** ``fiction/io/write_qll_layout.hpp``

.. doxygenfunction:: fiction::write_qll_layout(const Lyt& lyt, std::ostream& os)
.. doxygenfunction:: fiction::write_qll_layout(const Lyt& lyt, const std::string_view& filename)

SiQAD
#####

**Header:** ``fiction/io/write_sqd_layout.hpp``

.. doxygenfunction:: fiction::write_sqd_layout(const Lyt& lyt, std::ostream& os)
.. doxygenfunction:: fiction::write_sqd_layout(const Lyt& lyt, const std::string_view& filename)

**Header:** ``fiction/io/write_sqd_sim_result.hpp``

.. doxygenfunction:: fiction::write_sqd_sim_result(const sidb_simulation_result<Lyt>& sim_result, std::ostream& os)
.. doxygenfunction:: fiction::write_sqd_sim_result(const sidb_simulation_result<Lyt>& sim_result, const std::string_view& filename)

**Header:** ``fiction/io/read_sqd_layout.hpp``

.. doxygenfunction:: fiction::read_sqd_layout(std::istream& is, const std::string_view& name = "")
.. doxygenfunction:: fiction::read_sqd_layout(Lyt& lyt, std::istream& is)
.. doxygenfunction:: fiction::read_sqd_layout(const std::string_view& filename, const std::string_view& name = "")
.. doxygenfunction:: fiction::read_sqd_layout(Lyt& lyt, const std::string_view& filename)

.. doxygenclass:: fiction::sqd_parsing_error
