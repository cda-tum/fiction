//
// Created by marcel on 26.04.18.
//

#ifndef FICTION_FCN_GATE_LAYOUT_H
#define FICTION_FCN_GATE_LAYOUT_H

#include "fcn_layout.h"
#include "logic_network.h"
#include "directions.h"
#include <boost/optional.hpp>
#include <boost/bimap.hpp>
#include <boost/bimap/unordered_set_of.hpp>

/**
 * Represents layouts of field coupled nanocomputing (FCN) devices on a gate level abstraction. Inherits from fcn_layout
 * so it is a 3-dimensional grid-like structure as well. Faces are called tiles in a gate layout. Tiles can be occupied
 * by either an operation like AND, OR, NOT, fan-outs, special PI/PO ports or buffers (which make the tile a gate tile),
 * or multiple edges (which makes the tile a wire tile). All operations and edges are associated with a logic_network
 * (vertices, edges). Via direction maps, each tile and each wire within can be assigned input and output directions to
 * neighboring tiles to define information flow (in combination with clocking scheme). There are functions available to
 * access and trace those information flows.
 *
 * To iterate over all tiles of a fcn_gate_layout l, use
 *  for (auto&& t : l.tiles())
 *  {
 *      ...
 *  }
 *
 * There are more functions returning ranges of tiles. They work analogously.
 *
 * Sets of primary inputs and primary outputs mark tiles which have special connectors to the outside world. Note that
 * there are designated operations in logic_networks called operation::PI and operation::PO. Assigning those to tiles
 * leads to out-of-gate connectors. Though, also normal gate tiles can be PI/PO by setting the flags in the
 * assign_logic_vertex function. Then, the PI/PO is located directly in the gate. Note that this may or may not be
 * working in real devices depending on the used technological implementation. Note also that this version cannot
 * distinguish between multiple input or output ports so that it leads to errors when using non-symmetric functions as
 * gate-tiles. Though it is no restriction for classical MAJ, AND, OR, NOT, fan-out functions. Technically, PI/POs can
 * also be detected by a lack of input/output connections.
 *
 * It is not necessary (in fact, it leads to errors) to assign constant nodes to MAJ tiles to create AND, OR. Those do
 * have constant nodes inherently and can be assigned to tiles directly. No MAJ needed.
 */
class fcn_gate_layout : public fcn_layout
{
public:
    /**
     * Vertices of FCN gate layouts are called tiles.
     */
    using tile = vertex_t;
    /**
     * Same for tile_indices.
     */
    using tile_index = vertex_index_t;
    /**
     * Same for tile_paths.
     */
    using tile_path = vertex_path;
    /**
     * Granting access to private data members for fcn_cell_layout.
     */
    friend class fcn_cell_layout;
    /**
     * Standard constructor. Creates an FCN gate layout by the means of an array determining its size
     * as well as a clocking scheme defining its data flow possibilities.
     *
     * @param lengths 3-dimensional array defining sizes of each dimension (x, y, z) where z - 1 determines
     * the number of crossing layers.
     * NOTE: Due to a bug in the BGL, every dimension should have a minimum size of 2 to prevent SEGFAULTs.
     * See https://svn.boost.org/trac10/ticket/11735 for details.
     * @param clocking Clocking scheme defining possible data flow.
     * @param ln Pointer to a logic network whose elements should be assigned to this layout.
     */
    fcn_gate_layout(fcn_dimension_xyz&& lengths, fcn_clocking_scheme&& clocking, logic_network_ptr ln) noexcept;
    /**
     * Standard constructor. Creates an FCN gate layout by the means of an array determining its size
     * as well as a clocking scheme defining its data flow possibilities.
     *
     * @param lengths 2-dimensional array defining sizes of dimensions (x, y) where no crossing layer is given.
     * NOTE: Due to a bug in the BGL, every dimension should have a minimum size of 2 to prevent SEGFAULTs.
     * See https://svn.boost.org/trac10/ticket/11735 for details.
     * @param clocking Clocking scheme defining possible data flow.
     * @param ln Pointer to a logic network whose elements should be assigned to this layout.
     */
    fcn_gate_layout(fcn_dimension_xy&& lengths, fcn_clocking_scheme&& clocking, logic_network_ptr ln) noexcept;
    /**
     * Standard constructor. Creates an FCN gate layout by the means of an array determining its size
     * as well as a clocking scheme defining its data flow possibilities.
     *
     * @param lengths 2-dimensional array defining sizes of dimensions (x, y) where no crossing layer is given.
     * NOTE: Due to a bug in the BGL, every dimension should have a minimum size of 2 to prevent SEGFAULTs.
     * See https://svn.boost.org/trac10/ticket/11735 for details.
     * @param ln Pointer to a logic network whose elements should be assigned to this layout.
     */
    fcn_gate_layout(fcn_dimension_xy&& lengths, logic_network_ptr ln) noexcept;
    /**
     * Standard constructor. Creates a FCN gate layout of size 2 x 2 x 2 with the given clocking scheme.
     * NOTE: Due to a bug in the BGL, every dimension should have a minimum size of 2 to prevent SEGFAULTs.
     * See https://svn.boost.org/trac10/ticket/11735 for details.
     *
     * @param clocking Clocking scheme defining possible data flow.
     * @param ln Pointer to a logic network whose elements should be assigned to this layout.
     */
    fcn_gate_layout(fcn_clocking_scheme&& clocking, logic_network_ptr ln) noexcept;
    /**
     * Standard constructor. Creates a FCN gate layout of size 2 x 2 x 2 with an empty clocking.
     * NOTE: Due to a bug in the BGL, every dimension should have a minimum size of 2 to prevent SEGFAULTs.
     * See https://svn.boost.org/trac10/ticket/11735 for details.
     *
     * @param ln Pointer to a logic network whose elements should be assigned to this layout.
     */
    fcn_gate_layout(logic_network_ptr ln) noexcept;
    /**
     * Default copy constructor.
     */
    fcn_gate_layout(const fcn_gate_layout& fgl) noexcept = default;
    /**
     * Default move constructor.
     */
    fcn_gate_layout(fcn_gate_layout&& fgl) noexcept = default;
    /**
     * Default destructor.
     */
    ~fcn_gate_layout() = default;
    /**
     * Default assignment operator.
     */
    fcn_gate_layout& operator=(const fcn_gate_layout& rhs) noexcept = default;
    /**
     * Default move operator.
     */
    fcn_gate_layout& operator=(fcn_gate_layout&& rhs) noexcept = default;

    /**
     * Function alias for get_vertices using perfect forwarding and the name tiles to fit naming in fcn_gate_layout.
     *
     * @tparam ARGS Stack of argument types.
     * @param args Stack of arguments.
     * @return get_vertices(args).
     */
    template <typename... ARGS>
    auto tiles(ARGS&& ... args) const noexcept
    {
        return get_vertices(std::forward<ARGS>(args)...);
    }
    /**
     * Returns a range of all surrounding tiles to t whose clock number plus latch number is one less modulo maximum
     * clock number and which are placed in the same layer, i.e. all tiles which can feed information to t.
     *
     * @param t Tile whose counterparts with incoming clocking are desired.
     * @return A range of all tiles that are clocked incoming to t.
     */
    auto incoming_clocked_tiles(const tile& t) const noexcept
    {
        return surrounding_2d(t) |
               iter::filter([&](const tile& _t)
                            {
                                if (!tile_clocking(t) || !tile_clocking(_t))
                                    return false;

                                return static_cast<fcn_clock::zone>((*tile_clocking(_t) + get_latch(_t) + 1) %
                                                                 clocking.num_clocks) == *tile_clocking(t);
                            });
    }
    /**
     * Returns a range of all surrounding tiles to t whose clock number plus latch number is one higher modulo maximum
     * clock number and which are placed in the same layer, i.e. all tiles which can be fed with information by t.
     *
     * @param t Tile whose counterparts with outgoing clocking are desired.
     * @return A range of all tiles that are clocked outgoing to t.
     */
    auto outgoing_clocked_tiles(const tile& t) const noexcept
    {
        return surrounding_2d(t) |
               iter::filter([&](const tile& _t)
                            {
                                if (!tile_clocking(t) || !tile_clocking(_t))
                                    return false;

                                return static_cast<fcn_clock::zone>((*tile_clocking(t) + get_latch(t) + 1) %
                                                       clocking.num_clocks) == *tile_clocking(_t);
                            });
    }
    /**
     * Returns the number of tiles that are able to pass information to the given tile t within the same layer, i.e. the
     * indegree of t if viewed from a graph representation perspective.
     *
     * @param t Tile whose indegree is desired.
     * @return Indegree of tile t.
     */
    auto in_degree(const tile& t) const noexcept
    {
        auto incoming = incoming_clocked_tiles(t);
        return std::distance(incoming.begin(), incoming.end());
    }
    /**
     * Returns the number of tiles within the same layer the given tile t is able to pass information to, i.e. the
     * outdegree of t if viewed from a graph representation perspective.
     *
     * @param t Tile whose outdegree is desired.
     * @return Outdegree of tile t.
     */
    auto out_degree(const tile& t) const noexcept
    {
        auto outgoing = outgoing_clocked_tiles(t);
        return std::distance(outgoing.begin(), outgoing.end());
    }
    /**
     * Returns whether the given tile t is located at the layout borders where x or y are either minimal or maximal.
     * This is equivalent to checking if the number of surrounding tiles in the same layer is less than 4.
     *
     * @param t Tile to check for border location.
     * @return True iff t is located at one of layout's borders, false otherwise.
     */
    bool is_border_tile(const tile& t) const noexcept;
    /**
     * Determines the given tile's clock number by consulting the stored clocking cutout in case of regular clockings,
     * and looks up in the clocking map if clocking is irregular. If no entry has been stored for irregular clockings
     * yet, boost::none is returned.
     *
     * @param t Tile whose clock number is desired.
     * @return Clock number of t.
     */
    boost::optional<fcn_clock::zone> tile_clocking(const tile& t) const noexcept;
    /**
     * Determines the given index' tile's clock number by consulting the stored clocking cutout in case of regular
     * clockings, and looks up in the clocking map if clocking is irregular. If no entry has been stored for irregular
     * clockings yet, boost::none is returned.
     *
     * @param t Tile index whose tile's clock number is desired.
     * @return Clock number of t's tile.
     */
    boost::optional<fcn_clock::zone> tile_clocking(const tile_index t) const noexcept;
    /**
     * Assigns tile t with logic_network::vertex v.
     *
     * @param t Tile which should be associated with vertex v.
     * @param v Vertex which should be assigned to tile t.
     */
    void assign_logic_vertex(const tile& t, const logic_network::vertex v, const bool pi = false, const bool po = false) noexcept;
    /**
     * Dissociates any logic_network::vertex previously assigned to tile t.
     *
     * @param t Tile which should be cleared.
     */
    void dissociate_logic_vertex(const tile& t) noexcept;
    /**
     * Returns the logic_network::vertex assigned to tile t iff there is one. Returns boost::none otherwise.
     *
     * @param t Tile whose assigned logic vertex is desired.
     * @return Vertex associated with t iff there is one, boost::none otherwise.
     */
    boost::optional<logic_network::vertex> get_logic_vertex(const tile& t) const noexcept;
    /**
     * Checks whether a logic vertex is assigned to given tile t at all.
     *
     * @param t Tile to consider.
     * @return True, iff there is a logic vertex assigned to t.
     */
    bool is_gate_tile(const tile& t) const noexcept;
    /**
     * Checks whether given logic vertex v is assigned to given tile t.
     *
     * @param t Tile to consider.
     * @param v Logic vertex to check t's assignment for.
     * @return True, iff there is logic vertex v assigned to t.
     */
    bool has_logic_vertex(const tile& t, const logic_network::vertex v) const noexcept;
    /**
     * Returns the tile to which logic_network::vertex v is assigned iff there is one. Returns boost::none otherwise.
     *
     * @param v Logic vertex whose tile is desired.
     * @return Tile associated with v iff there is one, boost::none otherwise.
     */
    boost::optional<tile> get_logic_tile(const logic_network::vertex v) const noexcept;
    /**
     * Assigns tile t with logic_network::edge e.
     *
     * @param t Tile which should be associated with edge e.
     * @param e Edge which should be assigned to tile t.
     */
    void assign_logic_edge(const tile& t, const logic_network::edge& e) noexcept;
    /**
     * Dissociates a logic_network::edge e previously assigned to tile t.
     *
     * @param t Tile to consider.
     * @param e Edge to dissociate from t.
     */
    void dissociate_logic_edge(const tile& t, const logic_network::edge& e) noexcept;
    /**
     * Dissociates all logic_network::edges previously assigned to tile t.
     *
     * @param t Tile to consider.
     */
    void dissociate_logic_edges(const tile& t) noexcept;
private:
    /**
     * Alias for a hash set of logic edges. (Kind of "forward declaration".)
     */
    using edge_set = std::unordered_set<logic_network::edge, boost::hash<logic_network::edge>>;
public:
    /**
     * Returns the logic_network::edges assigned to tile t. Returns an empty set if there are none.
     *
     * @param t Tile whose assigned logic edges are desired.
     * @return Edges associated with t.
     */
    edge_set get_logic_edges(const tile& t) const noexcept;
    /**
     * Checks whether one or more logic edges are assigned to given tile t at all.
     *
     * @param t Tile to consider.
     * @return True, iff there are one or more logic edges assigned to t.
     */
    bool is_wire_tile(const tile& t) const noexcept;
    /**
     * Checks whether the given logic edge e is assigned to the given tile t.
     *
     * @param t Tile to consider.
     * @param e Edge to check for assignment to t.
     * @return True, iff e is assigned to t.
     */
    bool has_logic_edge(const tile& t, const logic_network::edge& e) const noexcept;
    /**
     * Removes all assignments from the given tile t, i.e. logic vertices and edges, latches, and all directions.
     *
     * @param t Tile to clear.
     */
    void clear_tile(const tile& t) noexcept;
    /**
     * Checks for the existence of logic vertices or edges assigned to the given tile.
     *
     * @param t Tile to check for emptiness.
     * @return True, iff tile t has no logic vertex or logic edge assigned to it.
     */
    bool is_free_tile(const tile& t) const noexcept;
    /**
     * Returns a vector of tiles containing the ones with outgoing data flow with respect to the given tile t which have
     * a subsequent logic vertex or logic edge assigned with respect to the network.
     *
     * @param t Tile whose outgoing information flow tiles are desired.
     * @return A vector of all outgoing information flow tiles to t.
     */
    std::vector<tile> outgoing_information_flow_tiles(const tile& t) const noexcept;
    /**
     * Returns a vector of tiles containing the ones with incoming data flow with respect to the given tile t which have
     * a preceeding logic vertex or logic edge assigned with respect to the network.
     *
     * @param t Tile whose incoming information flow tiles are desired.
     * @return A vector of all incoming information flow tiles to t.
     */
    std::vector<tile> incoming_information_flow_tiles(const tile& t) const noexcept;
    /**
     * Returns the operation assigned to tile t. If no operation has been set, op::NONE is returned.
     * Note that op::W is returned if a logic edge has been assigned to t.
     *
     * @param t Tile whose assigned operation is desired.
     * @return Operation associated with t.
     */
    operation get_op(const tile& t) const noexcept;
    /**
     * Returns the number of tiles that are assigned with logic vertices.
     *
     * @return Number of gates in the layout.
     */
    auto gate_count() const noexcept
    {
        return v_map.size();
    }
    /**
     * Returns the number of tiles that are assigned with logic edges.
     *
     * @return Number of wires in the layout.
     */
    auto wire_count() const noexcept
    {
        return e_map.size();
    }
    /**
     * Returns the number of logic edges assigned to tiles above ground layer.
     *
     * @return Number of logic_network::edge assignments above ground layer.
     */
    auto crossing_count() const noexcept
    {
        return std::count_if(crossing_layers().begin(), crossing_layers().end(),
                             [&](const tile& _t){return !is_free_tile(_t);});
    }
    /**
     * Assigns tile t with input directions d. The given directions will be added to the existing ones.
     * Use this to assign directions to gates or tiles in general. If explicit wires should be assigned with directions,
     * use assign_wire_inp_dir instead!
     * Removes entry if d == fcn_layout::DIR_NONE.
     *
     * @param t Tile which should be associated with input directions d.
     * @param d Input directions which should be assigned to tile t.
     */
    void assign_tile_inp_dir(const tile& t, layout::directions d) noexcept;
    /**
     * Assigns tile t with input directions d for logic edge e. The given directions will be added to the existing ones.
     * Removes entry if d == fcn_layout::DIR_NONE.
     *
     * @param t Tile which should be associated with e's input directions d.
     * @param e Logic edge whose input directions d should be assigned to t.
     * @param d Input directions of e which should be assigned to tile t.
     */
    void assign_wire_inp_dir(const tile& t, const logic_network::edge& e, layout::directions d) noexcept;
    /**
     * Checks whether the given directions d are assigned to tile t as inputs. Returns true if all given directions
     * are assigned to t but does not check if t has more directions assigned to it.
     *
     * @param t Tile whose assigned input directions should be checked.
     * @param d Directions to check t for.
     * @return true iff d are input directions of t.
     */
    bool is_tile_inp_dir(const tile& t, const layout::directions& d) const noexcept;
    /**
     * Checks whether the given directions d are assigned to tile t's logic edge e as inputs. Returns true if all given
     * directions are assigned to t's e but does not check if e has more directions assigned to it. If e is not assigned
     * to t in the first place, false is returned.
     *
     * @param t Tile whose assigned input directions for e should be checked.
     * @param e Logic edge on tile t whose input directions should be checked.
     * @param d Directions to check t's e for.
     * @return True, iff d are input directions of t's e. False, if e is not assigned to t in the first place.
     */
    bool is_wire_inp_dir(const tile& t, const logic_network::edge& e, const layout::directions& d) const noexcept;
    /**
     * Returns the input directions of the given tile as a bitset. The other related functions should be preferred for
     * convenience reasons if possible.
     *
     * @param t Tile whose input directions are desired.
     * @return All assigned input directions to t as a bitset.
     */
    layout::directions get_tile_inp_dirs(const tile& t) const noexcept;
    /**
     * Returns the input directions of the given tile's logic edge as a bitset. The other related functions should be
     * preferred for convenience reasons if possible.
     *
     * @param t Tile to consider.
     * @param e Logic edge assigned to t whose input directions are desired.
     * @return All assigned input directions to t's e as a bitset. layout::DIR_NONE if e is not assigned to t.
     */
    layout::directions get_wire_inp_dirs(const tile& t, const logic_network::edge& e) const noexcept;
    /**
     * Assigns tile t with output directions d. The given directions will be added to the existing ones.
     * Use this to assign directions to gates or tiles in general. If explicit wires should be assigned with directions,
     * use assign_wire_out_dir instead!
     * Removes entry if d == layout::DIR_NONE.
     *
     * @param t Tile which should be associated with output directions d.
     * @param d Output directions which should be assigned to tile t.
     */
    void assign_tile_out_dir(const tile& t, layout::directions d) noexcept;
    /**
     * Assigns tile t with output directions d for logic edge e. The given directions will be added to the existing
     * ones. Removes entry if d == layout::DIR_NONE.
     *
     * @param t Tile which should be associated with e's output directions d.
     * @param e Logic edge whose output directions d should be assigned to t.
     * @param d Output directions of e which should be assigned to tile t.
     */
    void assign_wire_out_dir(const tile& t, const logic_network::edge& e, layout::directions d) noexcept;
    /**
     * Checks whether the given directions d are assigned to tile t as outputs. Returns true if all given directions
     * are assigned to t but does not check if t has more directions assigned to it.
     *
     * @param t Tile whose assigned output directions should be checked.
     * @param d Directions to check t for.
     * @return true iff d are output directions of t.
     */
    bool is_tile_out_dir(const tile& t, const layout::directions& d) const noexcept;
    /**
     * Checks whether the given directions d are assigned to tile t's logic edge e as outputs. Returns true if all given
     * directions are assigned to t's e but does not check if t's e has more directions assigned to it. Returns false if
     * e is not assigned to t in the first place.
     *
     * @param t Tile to consider.
     * @param e Logic edge on tile t whose output directions d should be checked.
     * @param d Directions to check t's e for.
     * @return true iff d are output directions of t's e. False if e is not assigned to t.
     */
    bool is_wire_out_dir(const tile& t, const logic_network::edge& e, const layout::directions& d) const noexcept;
    /**
     * Returns the output directions of the given tile as a bitset. The other related functions should be preferred for
     * convenience reasons if possible.
     *
     * @param t Tile whose output directions are desired.
     * @return All assigned output directions to t as a bitset.
     */
    layout::directions get_tile_out_dirs(const tile& t) const noexcept;
    /**
     * Returns the output directions of the given tile's logic edge as a bitset. The other related functions should be
     * preferred for convenience reasons if possible.
     *
     * @param t Tile to consider.
     * @param e Logic edge assigned to t whose output directions are desired.
     * @return All assigned output directions to t's e as a bitset. layout::DIR_NONE if e is not assigned to t.
     */
    layout::directions get_wire_out_dirs(const tile& t, const logic_network::edge& e) const noexcept;
    /**
     * Returns all the dirs which are neither input nor output to the given tile as a bitset.
     *
     * @param t Tile whose unused directions are desired.
     * @return All directions which are neither input nor output to t as a bitset.
     */
    layout::directions get_unused_tile_dirs(const tile& t) const noexcept;
    /**
     * Tests whether the given directions are all assigned as either inputs or outputs to the given tile.
     *
     * @param t Tile whose directions should be checked.
     * @param d directions to check t for.
     * @return true iff all directions in d are assigned to t as inputs or outputs.
     */
    bool is_tile_dir(const tile& t, const layout::directions& d) const noexcept;
    /**
     * Tests whether the given directions are all assigned as either inputs or outputs to the given tile's logic edge.
     *
     * @param t Tile to consider.
     * @param e Logic edge whose assigned directions d should be checked.
     * @param d directions to check t's e for.
     * @return true iff all directions in d are assigned to t as inputs or outputs. False if e is not assigned to t.
     */
    bool is_wire_dir(const tile& t, const logic_network::edge& e, const layout::directions& d) const noexcept;
    /**
     * Returns spatial connectivity of the two given tiles i.e. the direction in which t2 can be reached by t1.
     * t1 and t2 do not have to be located in the same layer and do not have to be adjacent.
     * If both X- and Y-location are different, i.e. they are not straightly reachable, DIR::NONE is returned.
     * Only possible return values are: DIR_N, DIR_E, DIR_S, DIR_W, DIR_NONE.
     *
     * @param t1 Source tile.
     * @param t2 Target tile.
     * @return Direction in which t2 can be straightly reached by t1, or DIR_NONE if that is impossible.
     */
    layout::directions get_bearing(const tile& t1, const tile& t2) const noexcept;
    /**
     * Returns whether or not given tile t is a PI port.
     *
     * @param t Tile whose assigned PI port should be checked.
     * @return true iff t is a PI port.
     */
    bool is_pi(const tile& t) const noexcept;
    /**
     * Returns a range of all tiles flagged as primary input in the layout.
     *
     * @return range_t of all primary input tiles.
     */
    auto get_pis() const noexcept
    {
        return range_t<primary_set::const_iterator>{std::make_pair(pi_set.begin(), pi_set.end())};
    }
    /**
     * Returns whether or not given tile t is a PO port.
     *
     * @param t Tile whose assigned PO port should be checked.
     * @return true iff t is a PO port.
     */
    bool is_po(const tile& t) const noexcept;
    /**
     * Returns a range of all tiles flagged as primary output in the layout.
     *
     * @return range_t of all primary output tiles.
     */
    auto get_pos() const noexcept
    {
        return range_t<primary_set::const_iterator>{std::make_pair(po_set.begin(), po_set.end())};
    }
    /**
     * Tries to fetch input port names of the given tile t. If t itself does not have a PI operation, its predecessors
     * are investigated to find port names.
     *
     * @param t Tile whose input port names are desired.
     * @return A vector of input port names to t.
     */
    std::vector<std::string> get_inp_names(const tile& t) const noexcept;
    /**
     * Tries to fetch output port names of the given tile t. If t itself does not have a PO operation, its predecessors
     * are investigated to find port names.
     *
     * @param t Tile whose output port names are desired.
     * @return A vector of output port names to t.
     */
    std::vector<std::string> get_out_names(const tile& t) const noexcept;
    /**
     * Returns the associated network's stored file path name.
     *
     * @return The associated network's stored file path name.
     */
    std::string get_name() const noexcept;
    /**
     * Represents a bounding box surrounding the elements assigned to the layout's tiles. Helps to determine the "real"
     * size of a layout.
     */
    struct bounding_box
    {
        /**
         * Standard constructor. Defines corner points and calculates size.
         */
        bounding_box(std::size_t min_x, std::size_t min_y, std::size_t max_x, std::size_t max_y)
                :
                min_x{min_x},
                min_y{min_y},
                max_x{max_x},
                max_y{max_y},
                x_size{max_x - min_x},
                y_size{max_y - min_y}
        {}
        /**
         * Corners of bounding box.
         */
        const std::size_t min_x, min_y, max_x, max_y;
        /**
         * Size in x- and y-dimension.
         */
        const std::size_t x_size, y_size;
        /**
         * Returns area of bounding box.
         *
         * @return x-size * y-size.
         */
        std::size_t area() const noexcept
        {
            return x_size * y_size;
        }
    };
    /**
     * Determines the layout's bounding box i.e. the area in which logic elements are placed. Helps to determine the
     * "real" size of a layout.
     *
     * @return Bounding box.
     */
    bounding_box determine_bounding_box() const noexcept;
    /**
     * Resizes the layout so that all empty rows and columns on the right and bottom side are removed respectively.
     * No rows and columns on the left and top are affected because that would change indices of all tiles and therefore
     * all maps would not work properly anymore.
     */
    void shrink_to_fit() noexcept;
    /**
     * Prints the assigned logic operations and edges to the given std::ostream channel. A textual representation of
     * assigned objects is used as provided by the type operations. Currently only one crossing layer can be represented
     * correctly. This is more of a debug function and unsuitable for large layouts.
     *
     * @param os An std::ostream channel to write the textual representation of this layout into.
     * @param io_color Flag to indicate features like PI/PO should be printed with color escape.
     * @param clk_color Flag to indicate that clock zones should be printed with color escape. Can look weird.
     */
    void write_layout(std::ostream& os = std::cout, bool io_color = true, bool clk_color = false) const noexcept;

private:
    /**
     * Logic network associated with the vertices assigned to layout's tiles.
     */
    logic_network_ptr network = nullptr;
    /**
     * Alias for a hashed bidirectional map that assigns logic vertices to the tiles and vice versa.
     */
    using vertex_map = boost::bimap<boost::bimaps::unordered_set_of<tile, boost::hash<tile>>,
            boost::bimaps::unordered_set_of<logic_network::vertex, boost::hash<logic_network::vertex>>>;
    /**
     * Stores mapping tile -> logic_network::vertex. Helper functions for access save memory.
     */
    vertex_map v_map{};
    /**
     * Alias for a hash map that assigns sets of logic edges to the tiles.
     */
    using edge_map = std::unordered_map<tile, edge_set, boost::hash<tile>>;
    /**
     * Stores mapping tile -> set of logic_network::edge. Note that several edges per tile are possible.
     * Helper functions for access save memory.
     */
    edge_map e_map{};
    /**
     * Alias for a hash map that assigns directions to the tiles.
     */
    using direction_map = std::unordered_map<tile, layout::directions, boost::hash<tile>>;
    /**
     * Stores mapping tile -> directions for inputs and outputs respectively.
     * Helper functions for access save memory.
     */
    direction_map inp_dir_map{}, out_dir_map{};
    /**
     * Alias for a hash map that assigns directions to pairs of tiles and logic_network::edges to allow for a complete
     * many-to-many relation.
     */
    using edge_direction_map = std::unordered_map<std::pair<tile, logic_network::edge>, layout::directions,
            boost::hash<std::pair<tile, logic_network::edge>>>;
    /**
     * Stores mapping std::pair<tile, logic_network::edge> to directions for inputs and outputs respectively. Since a
     * single tile can hold either one vertex or multiple edges, the direction set above is already complete for
     * vertices but needs extension for edges. inp_dir_map and out_dir_map furthermore hold combinations of all
     * available directions for the respective tiles.
     * Helper functions for access save memory.
     */
    edge_direction_map edge_inp_dir_map{}, edge_out_dir_map{};
    /**
     * Alias for a hash set that holds tiles to represent PI/PO ports.
     */
    using primary_set = std::unordered_set<tile, boost::hash<tile>>;
    /**
     * Stores tiles that are marked as PIs/POs. Helper functions for access save memory.
     */
    primary_set pi_set{}, po_set{};
};

using fcn_gate_layout_ptr = std::shared_ptr<fcn_gate_layout>;


#endif //FICTION_FCN_GATE_LAYOUT_H
