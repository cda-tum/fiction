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
        .. autoclass:: mnt.pyfiction.write_qca_layout_params
            :members:
        .. autofunction:: mnt.pyfiction.write_qca_layout


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
        .. autoclass:: mnt.pyfiction.write_fqca_layout_params
            :members:
        .. autofunction:: mnt.pyfiction.write_fqca_layout

        .. autoclass:: mnt.pyfiction.out_of_cell_names_exception
            :members:

.. tabs::
    .. tab:: C++
        **Header:** ``fiction/io/read_fqca_layout.hpp``

        .. doxygenfunction:: fiction::read_fqca_layout(std::istream& is, const std::string_view& name = "")
        .. doxygenfunction:: fiction::read_fqca_layout(const std::string_view& filename, const std::string_view& name = "")

        .. doxygenclass:: fiction::unsupported_character_exception
           :members:
        .. doxygenclass:: fiction::undefined_cell_label_exception
           :members:
        .. doxygenclass:: fiction::unrecognized_cell_definition_exception
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
        **Header:** ``fiction/io/write_qcc_layout.hpp``

        .. doxygenstruct:: fiction::write_qcc_layout_params
           :members:

        .. doxygenfunction:: fiction::write_qcc_layout(const Lyt& lyt, std::ostream& os, write_qcc_layout_params ps = {})
        .. doxygenfunction:: fiction::write_qcc_layout(const Lyt& lyt, const std::string_view& filename, write_qcc_layout_params ps = {})

    .. tab:: Python
        .. autofunction:: mnt.pyfiction.write_qcc_layout


MagCAD & SCERPA
###############

.. tabs::
    .. tab:: C++
        **Header:** ``fiction/io/write_qll_layout.hpp``

        .. doxygenfunction:: fiction::write_qll_layout(const Lyt& lyt, std::ostream& os)
        .. doxygenfunction:: fiction::write_qll_layout(const Lyt& lyt, const std::string_view& filename)

    .. tab:: Python
        .. autofunction:: mnt.pyfiction.write_qll_layout


SiQAD
#####

.. tabs::
    .. tab:: C++
        **Header:** ``fiction/io/write_sqd_layout.hpp``

        .. doxygenfunction:: fiction::write_sqd_layout(const Lyt& lyt, std::ostream& os)
        .. doxygenfunction:: fiction::write_sqd_layout(const Lyt& lyt, const std::string_view& filename)

        **Header:** ``fiction/io/write_sqd_sim_result.hpp``

        .. doxygenfunction:: fiction::write_sqd_sim_result(const sidb_simulation_result<Lyt>& sim_result, std::ostream& os)
        .. doxygenfunction:: fiction::write_sqd_sim_result(const sidb_simulation_result<Lyt>& sim_result, const std::string_view& filename)

        **Header:** ``fiction/io/write_location_and_ground_state.hpp``

        .. doxygenfunction:: fiction::write_location_and_ground_state(const sidb_simulation_result<Lyt>& sim_result, std::ostream& os)
        .. doxygenfunction:: fiction::write_location_and_ground_state(const sidb_simulation_result<Lyt>& sim_result, const std::string_view& filename)

        **Header:** ``fiction/io/read_sqd_layout.hpp``

        .. doxygenfunction:: fiction::read_sqd_layout(std::istream& is, const std::string_view& name = "")
        .. doxygenfunction:: fiction::read_sqd_layout(Lyt& lyt, std::istream& is)
        .. doxygenfunction:: fiction::read_sqd_layout(const std::string_view& filename, const std::string_view& name = "")
        .. doxygenfunction:: fiction::read_sqd_layout(Lyt& lyt, const std::string_view& filename)

        .. doxygenclass:: fiction::sqd_parsing_error

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
        **Header:** ``fiction/io/write_operational_domain.hpp``

        .. doxygenstruct:: fiction::write_operational_domain_params
           :members:
        .. doxygenfunction:: fiction::write_operational_domain(const OpDomain& opdom, std::ostream& os,
                              const write_operational_domain_params& params = {})
        .. doxygenfunction:: fiction::write_operational_domain(const OpDomain& opdom, const std::string_view& filename,
                              const write_operational_domain_params& params = {})

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
        **Header:** ``fiction/io/write_fgl_layout.hpp``

        .. doxygenfunction:: fiction::write_fgl_layout(const Lyt& lyt, std::ostream& os)
        .. doxygenfunction:: fiction::write_fgl_layout(const Lyt& lyt, const std::string_view& filename)

        .. doxygenclass:: fiction::fgl_parsing_error
           :members:

    .. tab:: Python
        .. autofunction:: mnt.pyfiction.write_fgl_layout

        .. autoclass:: mnt.pyfiction.fgl_parsing_error
            :members:
