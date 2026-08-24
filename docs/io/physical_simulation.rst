Physical Simulation
-------------------

Write cell-level layouts into physical simulator files for various technologies.

QCADesigner
###########

.. tabs::
    .. tab:: C++
        **Header:** ``fiction/technology/qca/io/write_qca_layout.hpp``

        .. doxygenstruct:: fiction::qca::io::write_qca_layout_params
           :members:

        .. doxygenfunction:: fiction::qca::io::write_qca_layout(const Lyt& lyt, std::ostream& os, write_qca_layout_params ps = {})
        .. doxygenfunction:: fiction::qca::io::write_qca_layout(const Lyt& lyt, const std::string_view& filename, write_qca_layout_params ps = {})

    .. tab:: Python
        .. autoclass:: mnt.pyfiction.write_qca_layout_params
            :members:
        .. autofunction:: mnt.pyfiction.write_qca_layout


QCA-STACK
#########

.. tabs::
    .. tab:: C++
        **Header:** ``fiction/technology/qca/io/write_fqca_layout.hpp``

        .. doxygenstruct:: fiction::qca::io::write_fqca_layout_params
           :members:

        .. doxygenfunction:: fiction::qca::io::write_fqca_layout(const Lyt& lyt, std::ostream& os, write_fqca_layout_params ps = {})
        .. doxygenfunction:: fiction::qca::io::write_fqca_layout(const Lyt& lyt, const std::string_view& filename, write_fqca_layout_params ps = {})

        .. doxygenclass:: fiction::qca::io::out_of_cell_names_exception

    .. tab:: Python
        .. autoclass:: mnt.pyfiction.write_fqca_layout_params
            :members:
        .. autofunction:: mnt.pyfiction.write_fqca_layout

        .. autoclass:: mnt.pyfiction.out_of_cell_names_exception
            :members:

.. tabs::
    .. tab:: C++
        **Header:** ``fiction/technology/qca/io/read_fqca_layout.hpp``

        .. doxygenfunction:: fiction::qca::io::read_fqca_layout(std::istream& is, const std::string_view& name = "")
        .. doxygenfunction:: fiction::qca::io::read_fqca_layout(const std::string_view& filename, const std::string_view& name = "")

        .. doxygenclass:: fiction::qca::io::unsupported_character_exception
           :members:
        .. doxygenclass:: fiction::qca::io::undefined_cell_label_exception
           :members:
        .. doxygenclass:: fiction::qca::io::unrecognized_cell_definition_exception
           :members:

    .. tab:: Python
        .. autofunction:: mnt.pyfiction.read_fqca_layout

        .. autoclass:: mnt.pyfiction.unsupported_character_exception
            :members:
        .. autoclass:: mnt.pyfiction.undefined_cell_label_exception
            :members:
        .. autoclass:: mnt.pyfiction.unrecognized_cell_definition_exception
            :members:


ToPoliNano
##########

.. tabs::
    .. tab:: C++
        **Header:** ``fiction/technology/inml/io/write_qcc_layout.hpp``

        .. doxygenstruct:: fiction::inml::io::write_qcc_layout_params
           :members:

        .. doxygenfunction:: fiction::inml::io::write_qcc_layout(const Lyt& lyt, std::ostream& os, write_qcc_layout_params ps = {})
        .. doxygenfunction:: fiction::inml::io::write_qcc_layout(const Lyt& lyt, const std::string_view& filename, write_qcc_layout_params ps = {})

    .. tab:: Python
        .. autofunction:: mnt.pyfiction.write_qcc_layout


MagCAD & SCERPA
###############

.. tabs::
    .. tab:: C++
        **Header:** ``fiction/technology/inml/io/write_qll_layout.hpp``

        .. doxygenfunction:: fiction::inml::io::write_qll_layout(const Lyt& lyt, std::ostream& os)
        .. doxygenfunction:: fiction::inml::io::write_qll_layout(const Lyt& lyt, const std::string_view& filename)

    .. tab:: Python
        .. autofunction:: mnt.pyfiction.write_qll_layout


SiQAD
#####

.. tabs::
    .. tab:: C++
        **Header:** ``fiction/technology/sidb/io/write_sqd_layout.hpp``

        .. doxygenfunction:: fiction::sidb::io::write_sqd_layout(const Lyt& lyt, std::ostream& os)
        .. doxygenfunction:: fiction::sidb::io::write_sqd_layout(const Lyt& lyt, const std::string_view& filename)

        **Header:** ``fiction/technology/sidb/io/write_sqd_sim_result.hpp``

        .. doxygenfunction:: fiction::sidb::io::write_sqd_sim_result(const sidb::simulation::result<Lyt>& sim_result, std::ostream& os)
        .. doxygenfunction:: fiction::sidb::io::write_sqd_sim_result(const sidb::simulation::result<Lyt>& sim_result, const std::string_view& filename)

        **Header:** ``fiction/technology/sidb/io/write_location_and_ground_state.hpp``

        .. doxygenfunction:: fiction::sidb::io::write_location_and_ground_state(const sidb::simulation::result<Lyt>& sim_result, std::ostream& os)
        .. doxygenfunction:: fiction::sidb::io::write_location_and_ground_state(const sidb::simulation::result<Lyt>& sim_result, const std::string_view& filename)

        **Header:** ``fiction/technology/sidb/io/read_sqd_layout.hpp``

        .. doxygenfunction:: fiction::sidb::io::read_sqd_layout(std::istream& is, const std::string_view& name = "")
        .. doxygenfunction:: fiction::sidb::io::read_sqd_layout(Lyt& lyt, std::istream& is)
        .. doxygenfunction:: fiction::sidb::io::read_sqd_layout(const std::string_view& filename, const std::string_view& name = "")
        .. doxygenfunction:: fiction::sidb::io::read_sqd_layout(Lyt& lyt, const std::string_view& filename)

        .. doxygenclass:: fiction::sidb::io::sqd_parsing_error

    .. tab:: Python
        .. autofunction:: mnt.pyfiction.write_sqd_layout
        .. autofunction:: mnt.pyfiction.write_sqd_sim_result
        .. autofunction:: mnt.pyfiction.read_sqd_layout_100
        .. autofunction:: mnt.pyfiction.read_sqd_layout_111

        .. autoclass:: mnt.pyfiction.sqd_parsing_error
            :members:


SiDB Operational Domain
#######################

.. tabs::
    .. tab:: C++
        **Header:** ``fiction/technology/sidb/io/write_operational_domain.hpp``

        .. doxygenstruct:: fiction::sidb::io::write_operational_domain_params
           :members:
        .. doxygenfunction:: fiction::sidb::io::write_operational_domain(const OpDomain&, std::ostream&, const write_operational_domain_params&)
        .. doxygenfunction:: fiction::sidb::io::write_operational_domain(const OpDomain&, const std::string_view&, const write_operational_domain_params&)

    .. tab:: Python
        .. autoclass:: mnt.pyfiction.write_operational_domain_params
            :members:
        .. autofunction:: mnt.pyfiction.write_operational_domain
        .. autofunction:: mnt.pyfiction.write_critical_temperature_domain

Technology-independent Gate-level Layouts
#########################################

Can be used to generate gate-level layout files (``.fgl``) as offered by `MNT Bench <https://www.cda.cit.tum.de/mntbench/>`_.

.. tabs::
    .. tab:: C++
        **Header:** ``fiction/layouts/io/write_fgl_layout.hpp``

        .. doxygenfunction:: fiction::layouts::io::write_fgl_layout(const Lyt& lyt, std::ostream& os)
        .. doxygenfunction:: fiction::layouts::io::write_fgl_layout(const Lyt& lyt, const std::string_view& filename)

        .. doxygenclass:: fiction::layouts::io::fgl_parsing_error
           :members:

    .. tab:: Python
        .. autofunction:: mnt.pyfiction.write_fgl_layout

        .. autoclass:: mnt.pyfiction.fgl_parsing_error
            :members:
