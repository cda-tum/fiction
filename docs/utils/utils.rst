Network Utils
-------------

**Header:** ``fiction/utils/network_utils.hpp``

.. doxygenstruct:: mockturtle::edge

.. doxygenfunction:: fiction::foreach_edge
.. doxygenfunction:: fiction::foreach_outgoing_edge
.. doxygenfunction:: fiction::foreach_incoming_edge
.. doxygenfunction:: fiction::fanouts

.. doxygenstruct:: fiction::fanin_container
   :members:

.. doxygenfunction:: fiction::fanins
.. doxygenfunction:: fiction::num_constant_fanins

.. doxygenclass:: fiction::high_degree_fanin_exception

.. doxygenfunction:: fiction::has_high_degree_fanin_nodes

.. doxygenstruct:: fiction::fanin_edge_container
   :members:

.. doxygenfunction:: fiction::fanin_edges

.. doxygenfunction:: fiction::has_incoming_primary_input
.. doxygenfunction:: fiction::all_incoming_edge_paths
.. doxygenfunction:: fiction::inverse_levels


Truth Table Utils
-----------------

**Header:** ``fiction/utils/truth_table_utils.hpp``

.. doxygenfunction:: fiction::create_id_tt
.. doxygenfunction:: fiction::create_not_tt
.. doxygenfunction:: fiction::create_and_tt
.. doxygenfunction:: fiction::create_or_tt
.. doxygenfunction:: fiction::create_nand_tt
.. doxygenfunction:: fiction::create_nor_tt
.. doxygenfunction:: fiction::create_xor_tt
.. doxygenfunction:: fiction::create_xnor_tt
.. doxygenfunction:: fiction::create_maj_tt


Layout Utils
------------

**Header:** ``fiction/utils/layout_utils.hpp``

.. doxygenfunction:: fiction::num_adjacent_coordinates
.. doxygenfunction:: fiction::relative_to_absolute_cell_position
.. doxygenfunction:: fiction::port_direction_to_coordinate


Placement Utils
---------------

**Header:** ``fiction/utils/placement_utils.hpp``

.. doxygenfunction:: fiction::reserve_input_nodes
.. doxygenfunction:: fiction::place(Lyt& lyt, const tile<Lyt>& t, const Ntk& ntk, const mockturtle::node<Ntk>& n) noexcept
.. doxygenfunction:: fiction::place(Lyt& lyt, const tile<Lyt>& t, const Ntk& ntk, const mockturtle::node<Ntk>& n, const mockturtle::signal<Lyt>& a) noexcept
.. doxygenfunction:: fiction::place(Lyt& lyt, const tile<Lyt>& t, const Ntk& ntk, const mockturtle::node<Ntk>& n, const mockturtle::signal<Lyt>& a, const mockturtle::signal<Lyt>& b, const std::optional<bool>& c = std::nullopt) noexcept
.. doxygenfunction:: fiction::place(Lyt& lyt, const tile<Lyt>& t, const Ntk& ntk, const mockturtle::node<Ntk>& n, const mockturtle::signal<Lyt>& a, const mockturtle::signal<Lyt>& b, const mockturtle::signal<Lyt>& c) noexcept
.. doxygenfunction:: fiction::place(Lyt& lyt, const tile<Lyt>& t, const Ntk& ntk, const mockturtle::node<Ntk>& n, const mockturtle::node_map<mockturtle::signal<Lyt>, Ntk>& node2pos) noexcept

.. doxygenstruct:: fiction::branching_signal_container
   :members:

.. doxygenfunction:: fiction::place(Lyt& lyt, const tile<Lyt>& t, const Ntk& ntk, const mockturtle::node<Ntk>& n, const mockturtle::node_map<branching_signal_container<Lyt, Ntk, fanout_size>, Ntk>& node2pos) noexcept


Routing Utils
-------------

**Header:** ``fiction/utils/routing_utils.hpp``

.. doxygenstruct:: fiction::routing_objective
   :members:

.. doxygenclass:: fiction::layout_coordinate_path
   :members:

.. doxygenclass:: fiction::path_collection
   :members:
.. doxygenclass:: fiction::path_set
   :members:

.. doxygenfunction:: fiction::is_crossable_wire
.. doxygenfunction:: fiction::route_path
.. doxygenfunction:: fiction::extract_routing_objectives
.. doxygenfunction:: fiction::clear_routing


Name Utils
----------

**Header:** ``fiction/utils/name_utils.hpp``

.. doxygenfunction:: fiction::get_name
.. doxygenfunction:: fiction::set_name
.. doxygenfunction:: fiction::restore_network_name
.. doxygenfunction:: fiction::restore_input_names
.. doxygenfunction:: fiction::restore_output_names
.. doxygenfunction:: fiction::restore_signal_names(const NtkSrc& ntk_src, NtkDest& ntk_dest, const mockturtle::node_map<mockturtle::signal<NtkDest>, NtkSrc>& old2new) noexcept
.. doxygenfunction:: fiction::restore_signal_names(const NtkSrc& ntk_src, NtkDest& ntk_dest, const mockturtle::node_map<branching_signal_container<NtkDest, NtkSrc, fanout_size>, NtkSrc>& old2new) noexcept
.. doxygenfunction:: fiction::restore_names(const NtkSrc& ntk_src, NtkDest& ntk_dest) noexcept
.. doxygenfunction:: fiction::restore_names(const NtkSrc& ntk_src, NtkDest& ntk_dest, mockturtle::node_map<T, NtkSrc>& old2new) noexcept


Array Utils
-----------

**Header:** ``fiction/utils/array_utils.hpp``

.. doxygenfunction:: fiction::create_array
.. doxygenfunction:: fiction::convert_array
.. doxygenfunction:: fiction::convert_array_of_arrays


STL Extensions
--------------

**Header:** ``fiction/utils/stl_utils.hpp``

.. doxygenfunction:: fiction::find_first_two_of

.. doxygenclass:: fiction::searchable_priority_queue


Ranges
------

**Header:** ``fiction/utils/range.hpp``

.. doxygenstruct:: fiction::range_t
   :members:


Hashing
-------

**Header:** ``fiction/utils/hash.hpp``

.. doxygenfunction:: fiction::hash_combine
