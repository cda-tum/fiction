//
// Created by marcel on 09.03.17.
//

#ifndef FICTION_EXACT_PR_H
#define FICTION_EXACT_PR_H

#include "place_route.h"
#include "exact_pr_config.h"
#include <boost/format.hpp>
#include <z3++.h>

/**
 * An exact P&R approach using SMT solving. This class handles the incremental solver calls and
 * evaluation of the model. The pr_result holds extra statistical information about the solving
 * process.
 *
 * The SMT instance works with a single layer of variables even though it is possible to allow
 * crossings in the solution. The reduced number of variables saves a considerable amount of
 * runtime. That's why most for-loops iterate over layout->get_ground_layer_tiles() even
 * though it will be mapped to a 3-dimensional layout afterwards.
 */
class exact_pr : public place_route
{
public:
    /**
     * Standard constructor.
     *
     * @param ln Logic network.
     * @param config Configuration object storing all the bounds, flags, and so on.
     */
    exact_pr(logic_network_ptr ln, exact_pr_config&& config);
    /**
     * Default Destructor.
     */
    ~exact_pr() override = default;
    /**
     * Copy constructor is not available.
     */
    exact_pr(const exact_pr& rhs) = delete;
    /**
     * Move constructor is not available.
     */
    exact_pr(exact_pr&& rhs) = delete;
    /**
     * Assignment operator is not available.
     */
    exact_pr& operator=(const exact_pr& rhs) = delete;
    /**
     * Move assignment operator is not available.
     */
    exact_pr& operator=(exact_pr&& rhs) = delete;
    /**
     * Starts the P&R process. Builds the SMT instance and tries to fulfill it. The layout size will be
     * incremented after each UNSAT result until the given upper bound is reached.
     *
     * Returns a PRResult eventually.
     *
     * @return PRResult containing placed and routed layout as well as some statistical information.
     */
    place_route::pr_result perform_place_and_route() override;

private:
    /**
     * Shortcuts for types.
     */
    using layout_tile  = fcn_gate_layout::tile;
    using logic_vertex = logic_network::vertex;
    using logic_edge   = logic_network::edge;
    /**
     * Shortcuts for indices.
     */
    using layout_tile_index  = fcn_gate_layout::tile_index;
    using logic_vertex_index = logic_network::vertex_index;
    using logic_edge_index   = std::pair<logic_vertex_index, logic_vertex_index>;
    /**
     * Shortcuts for hashes.
     */
    using tile_vertex_hash = boost::hash<std::pair<layout_tile_index, logic_vertex_index>>;
    using tile_edge_hash   = boost::hash<std::pair<layout_tile_index, logic_edge_index>>;
    using tile_tile_hash   = boost::hash<std::pair<layout_tile_index, layout_tile_index>>;
    /**
     * Alias for an std::vector of z3::expr as a workaround because one cannot extract z3::expr from std::unordered_map
     * because of a missing standard constructor.
     */ 
    using z3_expr_proxy = std::vector<z3::expr>;
    /**
     * Alias for a map to access Z3 constants associated with layout tiles.
     */
    using tile_vertex_map = std::unordered_map<std::pair<layout_tile_index, logic_vertex_index>, z3_expr_proxy, tile_vertex_hash>;
    /**
     * Alias for a map to access Z3 constants associated with layout tile edges.
     */
    using tile_edge_map = std::unordered_map<std::pair<layout_tile_index, logic_edge_index>, z3_expr_proxy, tile_edge_hash>;
    /**
     * Alias for a map to access Z3 constants associated with layout tile connections.
     */
    using tile_connection_map = std::unordered_map<std::pair<layout_tile_index, layout_tile_index>, z3_expr_proxy, tile_tile_hash>;
    /**
     * Alias for a map to access Z3 constants associated with layout tile paths.
     */
    using tile_path_map = std::unordered_map<std::pair<layout_tile_index, layout_tile_index>, z3_expr_proxy, tile_tile_hash>;
    /**
     * Alias for a map to access Z3 constants associated with input vertex clocking zones.
     */
    using vertex_clock_map = std::unordered_map<logic_vertex_index, z3_expr_proxy>;
    /**
     * Alias for a map to access Z3 constants associated with tile clock zones. Used to symbolically represent an open
     * clocking so that the SMT solver is free to assign clock zones.
     */
    using tile_clock_map = std::unordered_map<layout_tile_index, z3_expr_proxy>;
    /**
     * Alias for a map to access Z3 constants associated with tile latches used to balance fan-in paths by an
     * artificial clock delay.
     */
    using tile_latch_map = std::unordered_map<layout_tile_index, z3_expr_proxy>;

    /**
     * Arguments, flags, and options for the P&R process stored in one configuration object.
     */
    const exact_pr_config config;
    /**
     * Lower bound for the number of layout tiles.
     */
    const unsigned lower_bound;
    /**
     * Context for all Z3 variables used by this instance.
     */
    z3::context ctx{};
    /**
     * SMT solver (utilizing optimize because of performance reasons).
     */
    z3::optimize solver;
    /**
     * Maps layout tiles and vertices to Z3 constants.
     */
    tile_vertex_map tv_map{};
    /**
     * Maps layout tiles and edges to Z3 constants.
     */
    tile_edge_map te_map{};
    /**
     * Maps layout tile connections to Z3 constants.
     */
    tile_connection_map tc_map{};
    /**
     * Maps layout tile paths to Z3 constants.
     */
    tile_path_map tp_map{};
    /**
     * Maps input vertex clock zones to Z3 constants.
     */
    vertex_clock_map vcl_map{};
    /**
     * Maps layout tiles clock zones to Z3 constants.
     */
    tile_clock_map tcl_map{};
    /**
     * Maps layout tile latches to Z3 constants.
     */
    tile_latch_map tl_map{};

    /**
     * Sets the given timeout for the solver.
     *
     * @param t Timeout in ms.
     */
    void set_timeout(const unsigned t);
    /**
     * Initializes tv_map using layout, netlist and context.
     *
     * Creates t * v many variables.
     */
    void initialize_tv_map();
    /**
     * Initializes te_map using layout, netlist and context.
     *
     * Creates t * e many variables.
     */
    void initialize_te_map();
    /**
     * Initializes tc_map using layout and context.
     *
     * Creates one variable for each connection on the layout.
     */
    void initialize_tc_map();
    /**
     * Initializes tp_map using layout and context.
     *
     * Creates t^2 many variables.
     */
    void initialize_tp_map();
    /**
     * Initializes vcl_map using netlist and context.
     *
     * Creates v many variables.
     */
    void initialize_vcl_map();
    /**
     * Initializes tcl_map using layout and context.
     *
     * Creates t many variables.
     */
    void initialize_tcl_map();
    /**
     * Initializes tl_map using layout and context.
     *
     * Creates t many variables
     */
    void initialize_tl_map();
    /**
     * Workaround helper function for getting the variable from a tile_vertex_map
     * corresponding to a given tile and vertex.
     *
     * @param t tile to be considered.
     * @param v vertex to be considered.
     * @return First (and only) element from the vector in map found by the keys.
     */
    z3::expr get_tv(const layout_tile& t, const logic_vertex v);
    /**
     * Workaround helper function for getting the variable from a tile_edge_map
     * corresponding to a given tile and edge.
     *
     * @param t tile to be considered.
     * @param e edge to be considered.
     * @return First (and only) element from the vector in map found by the keys.
     */
    z3::expr get_te(const layout_tile& t, const logic_edge& e);
    /**
     * Workaround helper function for getting the variable from a tile_connection_map
     * corresponding to two given tiles.
     *
     * @param t1 First tile to be considered.
     * @param t2 Second tile to be considered.
     * @return First (and only) element from the vector in map found by the keys.
     */
    z3::expr get_tc(const layout_tile& t1, const layout_tile& t2);
    /**
     * Workaround helper function for getting the variable from a tile_path_map
     * corresponding to two given tiles.
     *
     * @param t1 First tile to be considered.
     * @param t2 Second tile to be considered.
     * @return First (and only) element from the vector in map found by the keys.
     */
    z3::expr get_tp(const layout_tile& t1, const layout_tile& t2);
    /**
     * Workaround helper function for getting the variable from a vertex_clock_map
     * corresponding to a given vertex.
     *
     * @param v vertex to be considered.
     * @return First (and only) element from the vector in map found by the key.
     */
    z3::expr get_vcl(const logic_vertex v);
    /**
     * Workaround helper function for getting the variable from a tile_clock_map
     * corresponding to a given tile.
     *
     * @param t tile to be considered.
     * @return First (and only) element from the vector in map found by the key.
     */
    z3::expr get_tcl(const layout_tile& t);
    /**
     * Workaround helper function for getting the variable from a tile_latch_map
     * corresponding to a given tile.
     *
     * @param t tile to be considered.
     * @return First (and only) element from the vector in map found by the key.
     */
    z3::expr get_tl(const layout_tile& t);
    /**
     * Helper function for generating an equality of an arbitrary number of expressions.
     *
     * @param v Vector of expressions to equal.
     * @return Expression that represents the equality of all elements in v.
     */
    z3::expr mk_eq(const z3::expr_vector& v) const noexcept;
    /**
     * Constructs a series of expressions to evaluate how many tiles were occupied by a
     * given edge. Therefore, all te variables are translated to expressions like
     * ite(te, 1, 0) which allows for applying z3::sum to them. This is a work around because
     * no such api function for pseudo boolean exists.
     *
     * @param e edge to check for.
     * @param ve Vector of expressions to extend.
     */
    void tile_ite_counters(const logic_edge& e, z3::expr_vector& ve) noexcept;
    /**
     * Adds constraints to the solver to limit the number of elements that are going to be
     * assigned to a tile to one (vertex or edge) if no crossings are allowed.
     * Otherwise, one vertex per tile or two edges per tile can be placed.
     */
    void restrict_tile_elements();
    /**
     * Adds constraints to the solver to enforce that each vertex is placed exactly once
     * on exactly one tile.
     */
    void restrict_vertices();
    /**
     * Adds constraints to the solver to enforce that each clock zone variable has valid
     * bounds of 0 <= cl <= 3.
     */
    void restrict_clocks();
    /**
     * Adds constraints to the solver to enforce that latches have to be positive, that vertex tiles cannot be latches,
     * and that the overall sum of latch values should be minimized.
     */
    void restrict_latches();
    /**
     * Adds constraints to the solver to enforce that a tile which was assigned with some
     * vertex has a successor which is assigned to the adjacent vertex or an outgoing
     * edge.
     */
    void define_adjacent_vertex_tiles();
    /**
     * Adds constraints to the solver to enforce that a tile which was assigned with some
     * vertex has a predecessor which is assigned to the inversely adjacent vertex or an
     * ingoing edge.
     */
    void define_inv_adjacent_vertex_tiles();
    /**
     * Adds constraints to the solver to enforce that a tile which was assigned with some
     * edge has a successor which is assigned to the adjacent vertex or another edge.
     */
    void define_adjacent_edge_tiles();
    /**
     * Adds constraints to the solver to enforce that a tile which was assigned with some
     * edge has a predecessor which is assigned to the inversely adjacent vertex or another
     * edge.
     */
    void define_inv_adjacent_edge_tiles();
    /**
     * Adds constraints to the solver to map so far established connections between single
     * tiles to sub-paths. They are spanned transitively by the next set of constraints.
     */
    void establish_sub_paths();
    /**
     * Adds constraints to the solver to expand the formerly created sub-paths transitively.
     */
    void establish_transitive_paths();
    /**
     * Adds constraints to the solver to forbid cycles that loop back information. Therefore,
     * the formerly established paths are used.
     */
    void eliminate_cycles();
    /**
     * Adds constraints to the solver to ensure that the cl variable of primary input pi
     * is set to the clocking zone value of the tile pi is assigned to.
     */
    void assign_pi_clockings();
    /**
     * Adds constraints to the solver to ensure that fan-in paths to the same tile need
     * to have the same length modulo timing.
     */
    void fanin_length();
    /**
     * Adds constraints to the solver to prevent edges or vertices to be assigned to tiles
     * with an insufficient number of predecessors/successors.
     */
    void prevent_insufficiencies();
    /**
     * Adds constraints to the solver to define the exact number of connection variables to be set
     * for each tile, i.e. empty tiles are not allowed to have connections at all, edges need to have
     * one ingoing and one outgoing connection and so on.
     */
    void define_number_of_connections();
    /**
     * Adds constraints to the solver to position the primary inputs and primary outputs at
     * the layout's borders.
     */
    void enforce_border_io();
    /**
     * Adds constraints to the solver to limit the number of wire elements that are allowed
     * to be used to connect two vertices in the layout.
     */
    void limit_wire_length();
    /**
     * Adds constraints to the solver to limit the number of crossing tiles to be used.
     */
    void limit_crossings();
    /**
     * Generates the SMT instance and adds it to the solver.
     */
    void generate_smt_instance();
    /**
     * Assigns vertices, edges and directions to the stored layout with respect to the found model.
     */
    void assign_layout();
};


#endif //FICTION_EXACT_PR_H
