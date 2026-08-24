Network Utils
-------------

.. tabs::
    .. tab:: C++
        **Header:** ``fiction/networks/utils/network_utils.hpp``

        .. doxygenstruct:: mockturtle::edge

        .. doxygenfunction:: fiction::networks::utils::foreach_edge
        .. doxygenfunction:: fiction::networks::utils::foreach_outgoing_edge
        .. doxygenfunction:: fiction::networks::utils::foreach_incoming_edge
        .. doxygenfunction:: fiction::networks::utils::fanouts

        .. doxygenstruct:: fiction::networks::utils::fanin_container
           :members:

        .. doxygenfunction:: fiction::networks::utils::fanins
        .. doxygenfunction:: fiction::networks::utils::num_constant_fanins

        .. doxygenclass:: fiction::networks::utils::high_degree_fanin_exception

        .. doxygenfunction:: fiction::networks::utils::has_high_degree_fanin_nodes

        .. doxygenstruct:: fiction::networks::utils::fanin_edge_container
           :members:

        .. doxygenfunction:: fiction::networks::utils::fanin_edges

        .. doxygenfunction:: fiction::networks::utils::has_incoming_primary_input
        .. doxygenfunction:: fiction::networks::utils::all_incoming_edge_paths
        .. doxygenfunction:: fiction::networks::utils::inverse_levels

    .. tab:: Python
        .. autoclass:: mnt.pyfiction.high_degree_fanin_exception
            :members:
        .. autofunction:: mnt.pyfiction.has_high_degree_fanin_nodes


Truth Table Utils
-----------------

.. tabs::
    .. tab:: C++
        **Header:** ``fiction/networks/utils/truth_table_utils.hpp``

        .. doxygenfunction:: fiction::networks::utils::create_id_tt
        .. doxygenfunction:: fiction::networks::utils::create_not_tt
        .. doxygenfunction:: fiction::networks::utils::create_and_tt
        .. doxygenfunction:: fiction::networks::utils::create_or_tt
        .. doxygenfunction:: fiction::networks::utils::create_nand_tt
        .. doxygenfunction:: fiction::networks::utils::create_nor_tt
        .. doxygenfunction:: fiction::networks::utils::create_xor_tt
        .. doxygenfunction:: fiction::networks::utils::create_xnor_tt
        .. doxygenfunction:: fiction::networks::utils::create_lt_tt
        .. doxygenfunction:: fiction::networks::utils::create_gt_tt
        .. doxygenfunction:: fiction::networks::utils::create_le_tt
        .. doxygenfunction:: fiction::networks::utils::create_ge_tt
        .. doxygenfunction:: fiction::networks::utils::create_and3_tt
        .. doxygenfunction:: fiction::networks::utils::create_xor_and_tt
        .. doxygenfunction:: fiction::networks::utils::create_or_and_tt
        .. doxygenfunction:: fiction::networks::utils::create_onehot_tt
        .. doxygenfunction:: fiction::networks::utils::create_maj_tt
        .. doxygenfunction:: fiction::networks::utils::create_gamble_tt
        .. doxygenfunction:: fiction::networks::utils::create_dot_tt
        .. doxygenfunction:: fiction::networks::utils::create_ite_tt
        .. doxygenfunction:: fiction::networks::utils::create_and_xor_tt
        .. doxygenfunction:: fiction::networks::utils::create_xor3_tt
        .. doxygenfunction:: fiction::networks::utils::create_double_wire_tt
        .. doxygenfunction:: fiction::networks::utils::create_crossing_wire_tt
        .. doxygenfunction:: fiction::networks::utils::create_fan_out_tt
        .. doxygenfunction:: fiction::networks::utils::create_half_adder_tt

    .. tab:: Python
        .. autofunction:: mnt.pyfiction.create_id_tt
        .. autofunction:: mnt.pyfiction.create_not_tt
        .. autofunction:: mnt.pyfiction.create_and_tt
        .. autofunction:: mnt.pyfiction.create_or_tt
        .. autofunction:: mnt.pyfiction.create_nand_tt
        .. autofunction:: mnt.pyfiction.create_nor_tt
        .. autofunction:: mnt.pyfiction.create_xor_tt
        .. autofunction:: mnt.pyfiction.create_xnor_tt
        .. autofunction:: mnt.pyfiction.create_lt_tt
        .. autofunction:: mnt.pyfiction.create_gt_tt
        .. autofunction:: mnt.pyfiction.create_le_tt
        .. autofunction:: mnt.pyfiction.create_ge_tt
        .. autofunction:: mnt.pyfiction.create_and3_tt
        .. autofunction:: mnt.pyfiction.create_xor_and_tt
        .. autofunction:: mnt.pyfiction.create_or_and_tt
        .. autofunction:: mnt.pyfiction.create_onehot_tt
        .. autofunction:: mnt.pyfiction.create_maj_tt
        .. autofunction:: mnt.pyfiction.create_gamble_tt
        .. autofunction:: mnt.pyfiction.create_dot_tt
        .. autofunction:: mnt.pyfiction.create_ite_tt
        .. autofunction:: mnt.pyfiction.create_and_xor_tt
        .. autofunction:: mnt.pyfiction.create_xor3_tt
        .. autofunction:: mnt.pyfiction.create_double_wire_tt
        .. autofunction:: mnt.pyfiction.create_crossing_wire_tt
        .. autofunction:: mnt.pyfiction.create_fan_out_tt
        .. autofunction:: mnt.pyfiction.create_half_adder_tt


Layout Utils
------------

.. tabs::
    .. tab:: C++
        **Header:** ``fiction/layouts/utils/layout_utils.hpp``

        .. doxygenfunction:: fiction::layouts::utils::num_adjacent_coordinates
        .. doxygenfunction:: fiction::layouts::utils::relative_to_absolute_cell_position
        .. doxygenfunction:: fiction::layouts::utils::port_direction_to_coordinate
        .. doxygenfunction:: fiction::layouts::utils::normalize_layout_coordinates
        .. doxygenfunction:: fiction::layouts::utils::convert_layout_to_siqad_coordinates
        .. doxygenfunction:: fiction::layouts::utils::convert_layout_to_fiction_coordinates
        .. doxygenfunction:: fiction::layouts::utils::random_coordinate
        .. doxygenfunction:: fiction::layouts::utils::all_coordinates_in_spanned_area

    .. tab:: Python
        .. autofunction:: mnt.pyfiction.num_adjacent_coordinates
        .. autofunction:: mnt.pyfiction.normalize_layout_coordinates
        .. autofunction:: mnt.pyfiction.convert_layout_to_siqad_coordinates
        .. autofunction:: mnt.pyfiction.random_coordinate


Map utils
---------

.. tabs::
    .. tab:: C++
        **Header:** ``fiction/utils/stl/map_utils.hpp``

        .. doxygenfunction:: fiction::utils::stl::find_key_with_tolerance


Placement Utils
---------------

.. tabs::
    .. tab:: C++
        **Header:** ``fiction/physical_design/utils/placement_utils.hpp``

        .. doxygenfunction:: fiction::physical_design::utils::reserve_input_nodes
        .. doxygenfunction:: fiction::physical_design::utils::place(Lyt& lyt, const tile<Lyt>& t, const Ntk& ntk, const mockturtle::node<Ntk>& n) noexcept
        .. doxygenfunction:: fiction::physical_design::utils::place(Lyt& lyt, const tile<Lyt>& t, const Ntk& ntk, const mockturtle::node<Ntk>& n, const mockturtle::signal<Lyt>& a) noexcept
        .. doxygenfunction:: fiction::physical_design::utils::place(Lyt& lyt, const tile<Lyt>& t, const Ntk& ntk, const mockturtle::node<Ntk>& n, const mockturtle::signal<Lyt>& a, const mockturtle::signal<Lyt>& b, const std::optional<bool>& c = std::nullopt) noexcept
        .. doxygenfunction:: fiction::physical_design::utils::place(Lyt& lyt, const tile<Lyt>& t, const Ntk& ntk, const mockturtle::node<Ntk>& n, const mockturtle::signal<Lyt>& a, const mockturtle::signal<Lyt>& b, const mockturtle::signal<Lyt>& c) noexcept
        .. doxygenfunction:: fiction::physical_design::utils::place(Lyt& lyt, const tile<Lyt>& t, const Ntk& ntk, const mockturtle::node<Ntk>& n, const mockturtle::node_map<mockturtle::signal<Lyt>, Ntk>& node2pos) noexcept

        .. doxygenstruct:: fiction::physical_design::utils::branching_signal_container
           :members:

        .. doxygenfunction:: fiction::physical_design::utils::place(Lyt& lyt, const tile<Lyt>& t, const Ntk& ntk, const mockturtle::node<Ntk>& n, const mockturtle::node_map<branching_signal_container<Lyt, Ntk, fanout_size>, Ntk>& node2pos) noexcept

    .. tab:: Python
        .. autofunction:: mnt.pyfiction.reserve_input_nodes
        .. autofunction:: mnt.pyfiction.place


Routing Utils
-------------

.. tabs::
    .. tab:: C++
        **Header:** ``fiction/physical_design/utils/routing_utils.hpp``

        .. doxygenstruct:: fiction::physical_design::utils::routing_objective
           :members:

        .. doxygenclass:: fiction::physical_design::utils::layout_coordinate_path
           :members:

        .. doxygenclass:: fiction::physical_design::utils::path_collection
           :members:
        .. doxygenclass:: fiction::physical_design::utils::path_set
           :members:

        .. doxygenfunction:: fiction::physical_design::utils::is_crossable_wire

        .. doxygenfunction:: fiction::physical_design::utils::route_path

        .. doxygenfunction:: fiction::physical_design::utils::extract_routing_objectives
        .. doxygenfunction:: fiction::physical_design::utils::clear_routing

    .. tab:: Python
        .. autofunction:: mnt.pyfiction.is_crossable_wire
        .. autofunction:: mnt.pyfiction.route_path
        .. autofunction:: mnt.pyfiction.extract_routing_objectives
        .. autofunction:: mnt.pyfiction.clear_routing


Name Utils
----------

.. tabs::
    .. tab:: C++
        **Header:** ``fiction/networks/utils/name_utils.hpp``

        .. doxygenfunction:: fiction::networks::utils::get_name
        .. doxygenfunction:: fiction::networks::utils::set_name
        .. doxygenfunction:: fiction::networks::utils::restore_network_name
        .. doxygenfunction:: fiction::networks::utils::restore_input_names
        .. doxygenfunction:: fiction::networks::utils::restore_output_names
        .. doxygenfunction:: fiction::networks::utils::restore_signal_names(const NtkSrc& ntk_src, NtkDest& ntk_dest, const mockturtle::node_map<mockturtle::signal<NtkDest>, NtkSrc>& old2new) noexcept
        .. doxygenfunction:: fiction::networks::utils::restore_signal_names(const NtkSrc& ntk_src, NtkDest& ntk_dest, const mockturtle::node_map<branching_signal_container<NtkDest, NtkSrc, fanout_size>, NtkSrc>& old2new) noexcept
        .. doxygenfunction:: fiction::networks::utils::restore_names(const NtkSrc& ntk_src, NtkDest& ntk_dest) noexcept
        .. doxygenfunction:: fiction::networks::utils::restore_names(const NtkSrc& ntk_src, NtkDest& ntk_dest, mockturtle::node_map<T, NtkSrc>& old2new) noexcept

    .. tab:: Python
        .. autofunction:: mnt.pyfiction.get_name
        .. autofunction:: mnt.pyfiction.set_name


Array Utils
-----------

**Header:** ``fiction/utils/stl/array_utils.hpp``

.. doxygenfunction:: fiction::utils::stl::create_array
.. doxygenfunction:: fiction::utils::stl::convert_array
.. doxygenfunction:: fiction::utils::stl::convert_array_of_arrays


STL Extensions
--------------

**Header:** ``fiction/utils/stl/stl_utils.hpp``

.. doxygenfunction:: fiction::utils::stl::find_first_two_of
.. doxygenclass:: fiction::utils::stl::searchable_priority_queue
    :members:
.. doxygenfunction:: fiction::utils::stl::safe_localtime


Execution Policy Macros
-----------------------

**Header:** ``fiction/utils/stl/execution_utils.hpp``

Handling parallel STL algorithms is a bit cumbersome due to their platform dependence. The following macros are provided
to simplify the usage of parallel STL algorithms while CMake and some pre-processor magic take care of all the
boilerplate.

One can use the following macros to specify the execution policy for parallel STL algorithms in a (mostly)
platform-independent way::

    std::for_each(FICTION_EXECUTION_POLICY_PAR v.begin(), v.end(), lambda);
    //                                        ^ note the missing comma

If parallelism or execution policies are not available, this will expand to::

    std::for_each(v.begin(), v.end(), lambda);

.. note::
    Only include this header and do not include ``<execution>`` directly. This header will include ``<execution>`` if
    available and will define the macros accordingly.

.. doxygendefine:: FICTION_EXECUTION_POLICY_SEQ
.. doxygendefine:: FICTION_EXECUTION_POLICY_PAR
.. doxygendefine:: FICTION_EXECUTION_POLICY_PAR_UNSEQ


Hashing
-------

**Header:** ``fiction/utils/stl/hash.hpp``

This header defines implementations for ``std::hash`` for several data types.

.. doxygenfunction:: fiction::utils::stl::hash_combine


Math Utils
----------

**Header:** ``fiction/utils/math/math_utils.hpp``

.. doxygenfunction:: fiction::utils::math::round_to_n_decimal_places
.. doxygenfunction:: fiction::utils::math::integral_abs
.. doxygenfunction:: fiction::utils::math::binomial_coefficient
.. doxygenfunction:: fiction::utils::math::determine_all_combinations_of_distributing_k_entities_on_n_positions
.. doxygenfunction:: fiction::utils::math::cartesian_combinations


``phmap``
---------

**Header:** ``fiction/utils/stl/phmap_utils.hpp``

.. doxygentypedef:: fiction::utils::stl::locked_parallel_flat_hash_map
