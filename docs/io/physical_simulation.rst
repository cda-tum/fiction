Physical Simulation
-------------------

Write cell-level layouts into physical simulator files for various technologies.

QCADesigner
###########

.. tabs::
    .. tab:: C++
        **Header:** ``fiction/io/write_qca_layout.hpp``

        .. doxygenstruct:: fiction::write_qca_layout_params
           :members:

        .. doxygenfunction:: fiction::write_qca_layout(const Lyt& lyt, std::ostream& os, write_qca_layout_params ps = {})
        .. doxygenfunction:: fiction::write_qca_layout(const Lyt& lyt, const std::string_view& filename, write_qca_layout_params ps = {})

    .. tab:: Python
        .. autoclass:: fiction.pyfiction.write_qca_layout_params
            :members:
        .. autofunction:: fiction.pyfiction.write_qca_layout

QCA-STACK
#########

.. tabs::
    .. tab:: C++
        **Header:** ``fiction/io/write_fqca_layout.hpp``

        .. doxygenstruct:: fiction::write_fqca_layout_params
           :members:

        .. doxygenfunction:: fiction::write_fqca_layout(const Lyt& lyt, std::ostream& os, write_fqca_layout_params ps = {})
        .. doxygenfunction:: fiction::write_fqca_layout(const Lyt& lyt, const std::string_view& filename, write_fqca_layout_params ps = {})

        .. doxygenclass:: fiction::out_of_cell_names_exception

    .. tab:: Python
        .. autoclass:: fiction.pyfiction.write_fqca_layout_params
            :members:
        .. autofunction:: fiction.pyfiction.write_fqca_layout

.. tabs::
    .. tab:: C++
        **Header:** ``fiction/io/read_fqca_layout.hpp``

        .. doxygenfunction:: fiction::read_fqca_layout(std::istream& is, const std::string_view& name = "")
        .. doxygenfunction:: fiction::read_fqca_layout(const std::string_view& filename, const std::string_view& name = "")

    .. tab:: Python
        .. autofunction:: fiction.pyfiction.read_fqca_layout

ToPoliNano
##########

.. tabs::
    .. tab:: C++
        **Header:** ``fiction/io/write_qcc_layout.hpp``

        .. doxygenstruct:: fiction::write_qcc_layout_params
           :members:

        .. doxygenfunction:: fiction::write_qcc_layout(const Lyt& lyt, std::ostream& os, write_qcc_layout_params ps = {})
        .. doxygenfunction:: fiction::write_qcc_layout(const Lyt& lyt, const std::string_view& filename, write_qcc_layout_params ps = {})

    .. tab:: Python
        .. autofunction:: fiction.pyfiction.write_qcc_layout

MagCAD & SCERPA
###############

.. tabs::
    .. tab:: C++
        **Header:** ``fiction/io/write_qll_layout.hpp``

        .. doxygenfunction:: fiction::write_qll_layout(const Lyt& lyt, std::ostream& os)
        .. doxygenfunction:: fiction::write_qll_layout(const Lyt& lyt, const std::string_view& filename)

    .. tab:: Python
        .. autofunction:: fiction.pyfiction.write_qll_layout

SiQAD
#####

.. tabs::
    .. tab:: C++
        **Header:** ``fiction/io/write_sqd_layout.hpp``

        .. doxygenfunction:: fiction::write_sqd_layout(const Lyt& lyt, std::ostream& os)
        .. doxygenfunction:: fiction::write_sqd_layout(const Lyt& lyt, const std::string_view& filename)

    .. tab:: Python
        .. autofunction:: fiction.pyfiction.write_sqd_layout

**Header:** ``fiction/io/read_sqd_layout.hpp``

.. doxygenfunction:: fiction::read_sqd_layout(std::istream& is, const std::string_view& name = "")
.. doxygenfunction:: fiction::read_sqd_layout(Lyt& lyt, std::istream& is)
.. doxygenfunction:: fiction::read_sqd_layout(const std::string_view& filename, const std::string_view& name = "")
.. doxygenfunction:: fiction::read_sqd_layout(Lyt& lyt, const std::string_view& filename)

.. doxygenclass:: fiction::sqd_parsing_error
