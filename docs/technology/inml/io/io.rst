iNML I/O
========

ToPoliNano
----------

.. tabs::
    .. tab:: C++
        **Header:** ``fiction/technology/inml/io/write_qcc_layout.hpp``

        .. doxygenstruct:: fiction::inml::io::write_qcc_layout_params
           :members:

        .. doxygenfunction:: fiction::inml::io::write_qcc_layout(const Lyt& lyt, std::ostream& os, write_qcc_layout_params ps = {})
        .. doxygenfunction:: fiction::inml::io::write_qcc_layout(const Lyt& lyt, const std::string_view& filename, write_qcc_layout_params ps = {})

    .. tab:: Python
        .. autofunction:: mnt.pyfiction.write_qcc_layout
