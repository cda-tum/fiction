Network Utils
-------------

.. tabs::
    .. tab:: C++
        **Header:** ``fiction/networks/network_utils.hpp``

        .. doxygenstruct:: mockturtle::edge

        .. doxygenfunction:: fiction::networks::foreach_edge
        .. doxygenfunction:: fiction::networks::foreach_outgoing_edge
        .. doxygenfunction:: fiction::networks::foreach_incoming_edge
        .. doxygenfunction:: fiction::networks::fanouts

        .. doxygenstruct:: fiction::networks::fanin_container
           :members:

        .. doxygenfunction:: fiction::networks::fanins
        .. doxygenfunction:: fiction::networks::num_constant_fanins

        .. doxygenclass:: fiction::networks::high_degree_fanin_exception

        .. doxygenfunction:: fiction::networks::has_high_degree_fanin_nodes

        .. doxygenstruct:: fiction::networks::fanin_edge_container
           :members:

        .. doxygenfunction:: fiction::networks::fanin_edges

        .. doxygenfunction:: fiction::networks::has_incoming_primary_input
        .. doxygenfunction:: fiction::networks::all_incoming_edge_paths
        .. doxygenfunction:: fiction::networks::inverse_levels

    .. tab:: Python
        .. autoclass:: mnt.pyfiction.high_degree_fanin_exception
            :members:
        .. autofunction:: mnt.pyfiction.has_high_degree_fanin_nodes


Layout Utils
------------

.. tabs::
    .. tab:: C++
        **Header:** ``fiction/layouts/layout_utils.hpp``

        .. doxygenfunction:: fiction::layouts::num_adjacent_coordinates
        .. doxygenfunction:: fiction::layouts::relative_to_absolute_cell_position
        .. doxygenfunction:: fiction::layouts::port_direction_to_coordinate
        .. doxygenfunction:: fiction::layouts::normalize_layout_coordinates
        .. doxygenfunction:: fiction::layouts::convert_layout_to_siqad_coordinates
        .. doxygenfunction:: fiction::layouts::convert_layout_to_fiction_coordinates
        .. doxygenfunction:: fiction::layouts::random_coordinate
        .. doxygenfunction:: fiction::layouts::all_coordinates_in_spanned_area

    .. tab:: Python
        .. autofunction:: mnt.pyfiction.num_adjacent_coordinates
        .. autofunction:: mnt.pyfiction.normalize_layout_coordinates
        .. autofunction:: mnt.pyfiction.convert_layout_to_siqad_coordinates
        .. autofunction:: mnt.pyfiction.random_coordinate


Map utils
---------

.. tabs::
    .. tab:: C++
        **Header:** ``fiction/utils/math/map_utils.hpp``

        .. doxygenfunction:: fiction::utils::math::find_key_with_tolerance


Placement Utils
---------------

.. tabs::
    .. tab:: C++
        **Header:** ``fiction/physical_design/placement_utils.hpp``

        .. doxygenfunction:: fiction::physical_design::reserve_input_nodes
        .. doxygenfunction:: fiction::physical_design::place(Lyt& lyt, const tile<Lyt>& t, const Ntk& ntk, const mockturtle::node<Ntk>& n) noexcept
        .. doxygenfunction:: fiction::physical_design::place(Lyt& lyt, const tile<Lyt>& t, const Ntk& ntk, const mockturtle::node<Ntk>& n, const mockturtle::signal<Lyt>& a) noexcept
        .. doxygenfunction:: fiction::physical_design::place(Lyt& lyt, const tile<Lyt>& t, const Ntk& ntk, const mockturtle::node<Ntk>& n, const mockturtle::signal<Lyt>& a, const mockturtle::signal<Lyt>& b, const std::optional<bool>& c = std::nullopt) noexcept
        .. doxygenfunction:: fiction::physical_design::place(Lyt& lyt, const tile<Lyt>& t, const Ntk& ntk, const mockturtle::node<Ntk>& n, const mockturtle::signal<Lyt>& a, const mockturtle::signal<Lyt>& b, const mockturtle::signal<Lyt>& c) noexcept
        .. doxygenfunction:: fiction::physical_design::place(Lyt& lyt, const tile<Lyt>& t, const Ntk& ntk, const mockturtle::node<Ntk>& n, const mockturtle::node_map<mockturtle::signal<Lyt>, Ntk>& node2pos) noexcept

        .. doxygenstruct:: fiction::physical_design::branching_signal_container
           :members:

        .. doxygenfunction:: fiction::physical_design::place(Lyt& lyt, const tile<Lyt>& t, const Ntk& ntk, const mockturtle::node<Ntk>& n, const mockturtle::node_map<branching_signal_container<Lyt, Ntk, fanout_size>, Ntk>& node2pos) noexcept

    .. tab:: Python
        .. autofunction:: mnt.pyfiction.reserve_input_nodes
        .. autofunction:: mnt.pyfiction.place


Routing Utils
-------------

.. tabs::
    .. tab:: C++
        **Header:** ``fiction/physical_design/routing_utils.hpp``

        .. doxygenstruct:: fiction::physical_design::routing_objective
           :members:

        .. doxygenclass:: fiction::physical_design::layout_coordinate_path
           :members:

        .. doxygenclass:: fiction::physical_design::path_collection
           :members:
        .. doxygenclass:: fiction::physical_design::path_set
           :members:

        .. doxygenfunction:: fiction::physical_design::is_crossable_wire

        .. doxygenfunction:: fiction::physical_design::route_path

        .. doxygenfunction:: fiction::physical_design::extract_routing_objectives
        .. doxygenfunction:: fiction::physical_design::clear_routing

    .. tab:: Python
        .. autofunction:: mnt.pyfiction.is_crossable_wire
        .. autofunction:: mnt.pyfiction.route_path
        .. autofunction:: mnt.pyfiction.extract_routing_objectives
        .. autofunction:: mnt.pyfiction.clear_routing


Name Utils
----------

.. tabs::
    .. tab:: C++
        **Header:** ``fiction/networks/name_utils.hpp``

        .. doxygenfunction:: fiction::networks::get_name
        .. doxygenfunction:: fiction::networks::set_name
        .. doxygenfunction:: fiction::networks::restore_network_name
        .. doxygenfunction:: fiction::networks::restore_input_names
        .. doxygenfunction:: fiction::networks::restore_output_names
        .. doxygenfunction:: fiction::networks::restore_signal_names(const NtkSrc& ntk_src, NtkDest& ntk_dest, const mockturtle::node_map<mockturtle::signal<NtkDest>, NtkSrc>& old2new) noexcept
        .. doxygenfunction:: fiction::networks::restore_signal_names(const NtkSrc& ntk_src, NtkDest& ntk_dest, const mockturtle::node_map<branching_signal_container<NtkDest, NtkSrc, fanout_size>, NtkSrc>& old2new) noexcept
        .. doxygenfunction:: fiction::networks::restore_names(const NtkSrc& ntk_src, NtkDest& ntk_dest) noexcept
        .. doxygenfunction:: fiction::networks::restore_names(const NtkSrc& ntk_src, NtkDest& ntk_dest, mockturtle::node_map<T, NtkSrc>& old2new) noexcept

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
