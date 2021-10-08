//
// Created by marcel on 06.01.20.
//

#if (FICTION_Z3_SOLVER)

#ifndef FICTION_EXACT_HPP
#define FICTION_EXACT_HPP

#include "../algorithms/name_restoration.hpp"
#include "../io/print_layout.hpp"
#include "../layouts/clocking_scheme.hpp"
#include "../traits.hpp"
#include "../utils/debug/network_writer.hpp"
#include "fanout_substitution.hpp"

#include <fmt/format.h>
#include <mockturtle/traits.hpp>
#include <mockturtle/utils/node_map.hpp>
#include <mockturtle/utils/progress_bar.hpp>
#include <mockturtle/utils/stopwatch.hpp>
#include <mockturtle/views/color_view.hpp>
#include <mockturtle/views/fanout_view.hpp>
#include <mockturtle/views/topo_view.hpp>

#include <z3++.h>

#include <algorithm>
#include <chrono>
#include <cstdint>
#include <future>
#include <memory>
#include <mutex>
#include <optional>
#include <set>
#include <thread>
#include <vector>

namespace mockturtle
{
template <typename Ntk>
struct edge
{
    node<Ntk> source, target;
};
}  // namespace mockturtle

namespace fiction
{

template <typename Lyt>
struct exact_physical_design_params
{
    /**
     * Clocking scheme to be used.
     */
    std::shared_ptr<clocking_scheme<coordinate<Lyt>>> scheme =
        std::make_shared<clocking_scheme<coordinate<Lyt>>>(twoddwave_4_clocking<Lyt>());
    /**
     * Number of tiles to use.
     */
    uint16_t upper_bound = std::numeric_limits<uint16_t>::max();
    /**
     * Use just one fixed tile size.
     */
    uint16_t fixed_size = 0ul;
    /**
     * Number of threads to use for exploring the possible dimensions.
     */
    std::size_t num_threads = 1ul;
    /**
     * Flag to indicate that crossings should be used.
     */
    bool crossings = false;
    /**
     * Flag to indicate that designated wires should be routed to balance I/O port paths.
     */
    bool io_ports = false;
    /**
     * Flag to indicate that I/Os should be placed at the grid's border.
     */
    bool border_io = false;
    /**
     * Flag to indicate that artificial clock latch delays should be used to balance paths.
     */
    bool synchronization_elements = false;
    /**
     * Flag to indicate that only straight inverters should be used (instead of bend ones).
     */
    bool straight_inverters = false;
    /**
     * Flag to indicate that a discrepancy in fan-in path lengths was allowed.
     */
    bool desynchronize = false;
    /**
     * Flag to indicate that the number of used crossing tiles should be minimized.
     */
    bool minimize_wires = false;
    /**
     * Flag to indicate that the number of used crossing tiles should be minimized.
     */
    bool minimize_crossings = false;
    /**
     * Sets a timeout in ms for the solving process. Standard is 4294967 seconds as defined by Z3.
     */
    unsigned timeout = 4294967u;
};

struct exact_physical_design_stats
{
    mockturtle::stopwatch<>::duration time_total{0};

    uint64_t x_size{0ull}, y_size{0ull};
    uint64_t num_gates{0ull}, num_wires{0ull};

    void report(std::ostream& out = std::cout) const
    {
        out << fmt::format("[i] total time  = {:.2f} secs\n", mockturtle::to_seconds(time_total));
        out << fmt::format("[i] layout size = {} × {}\n", x_size, y_size);
        out << fmt::format("[i] num. gates  = {}\n", num_gates);
        out << fmt::format("[i] num. wires  = {}\n", num_wires);
    }
};

namespace detail
{

template <typename Lyt, typename Ntk>
class exact_impl
{
  public:
    exact_impl(const Ntk& src, const exact_physical_design_params<Lyt>& p, exact_physical_design_stats& st) :
            ntk{mockturtle::fanout_view{fanout_substitution<mockturtle::names_view<topology_network>>(
                src)}},  // TODO incorporate clocking scheme degree into fanout-substitution
            ps{p},
            pst{st},
            node2pos{ntk},
            lower_bound{static_cast<decltype(lower_bound)>(ntk.num_gates() + ntk.num_pis() + ntk.num_pos())}  // minimum number tiles to use
    //            dit{ps.fixed_size ? ps.fixed_size : lower_bound}
    {}

    std::optional<Lyt> run()
    {
        // measure run time
        mockturtle::stopwatch stop{pst.time_total};

        Lyt layout{};

        // restore possibly set signal names
        restore_names(ntk, layout, node2pos);

        // statistical information
        pst.x_size    = layout.x() + 1;
        pst.y_size    = layout.y() + 1;
        pst.num_gates = layout.num_gates();
        pst.num_wires = layout.num_wires();

        return layout;
    }

  private:
    mockturtle::topo_view<mockturtle::fanout_view<mockturtle::names_view<topology_network>>> ntk;

    exact_physical_design_params<Lyt> ps;
    exact_physical_design_stats&      pst;

    mockturtle::node_map<mockturtle::signal<Lyt>, decltype(ntk)> node2pos;
    /**
     * Lower bound for the number of layout tiles.
     */
    uint16_t lower_bound;

    //    dimension_iterator dit{0};

    /**
     * Aspect ratio of found result. Only interesting for asynchronous case.
     */
    std::optional<aspect_ratio<Lyt>> result_dimension;
    /**
     * Restricts access to the dimension_iterator and the result_dimension.
     */
    std::mutex dit_mutex{}, rd_mutex{};

    using ctx_ptr      = std::shared_ptr<z3::context>;
    using solver_ptr   = std::shared_ptr<z3::solver>;
    using optimize_ptr = std::shared_ptr<z3::optimize>;

    /**
     * Sub-class to exact to handle construction of SMT instances as well as house-keeping like storing solver
     * states across incremental calls etc. Multiple handlers can be created in order to explore possible dimensions in
     * parallel.
     */
    class smt_handler
    {
      public:
        /**
         * Standard constructor.
         *
         * @param ctx The context that is used in all solvers.
         * @param fgl The gate layout pointer that is going to contain the created layout.
         * @param c The configurations to respect in the SMT instance generation process.
         */
        smt_handler(ctx_ptr ctx, const Lyt& fgl, const exact_physical_design_params<Lyt>& c) noexcept;
        /**
         * Evaluates a given dimension regarding the stored configurations whether it can be skipped, i.e. does not
         * need to be explored by the SMT solver. The better this function is, the more UNSAT instances can be skipped
         * without losing the optimality guarantee. This function should never be overly restrictive!
         *
         * @param dim Dimension to evaluate.
         * @return True if dim can safely be skipped because it is UNSAT anyways.
         */
        bool skippable(const aspect_ratio<Lyt>& dim) const noexcept;
        /**
         * Resizes the layout and creates a new solver checkpoint from where on the next incremental instance can be
         * generated.
         *
         * @param dim Current dimension to work on.
         */
        void update(const aspect_ratio<Lyt>& dim) noexcept;
        /**
         * Sets the given timeout for the current solver.
         *
         * @param t Timeout in ms.
         */
        void set_timeout(const unsigned t);
        /**
         * Generates the SMT instance for the current solver check point and runs the solver check. In case the instance
         * was satisfiable, all constraints are moved to an z3::optimize if optimization criteria were specified. This
         * way, no unnecessary optimization constraints need to be generated over and over for UNSAT instances.
         *
         * If the instance was found SAT on both levels, a layout is extract from the model and stored. The function
         * returns true.
         *
         * @return true iff the instance generated for the current configuration is SAT.
         */
        bool is_satisfiable();
        /**
         * Stores the current solver state in the solver tree with dimension dim as key.
         *
         * @param dim Key to storing the current solver state.
         */
        void store_solver_state(const aspect_ratio<Lyt>& dim) noexcept;

      private:
        /**
         * During incremental SMT calls, several created assertions need to be reformulated. To still be able to reuse
         * information like learned clauses and lemmas, the push/pop strategy is not employed. Instead, assumption
         * literals are used that track, i.e. imply, certain assertions and can be negated in order to deactivate them.
         * Over incremental calls, the layout size grows either in eastern or southern direction. Assertions to
         * deactivate affect tiles sitting at the growing borders as they certainly get new neighbors. To not deactivate
         * both at once if not needed, there is one assertion literal tracking eastern growth and one tracking southern
         * growth. The corner tile is implied by their conjunction.
         */
        struct assumption_literals
        {
            /**
             * Watched literals for eastern and southern constraints which are used to reformulate them.
             */
            z3::expr e, s;
        };
        /**
         * Incremental solver calls require some kind of continuous solver state that can be extended. This state
         * involves a pointer to the solver alongside its assumption literals.
         */
        struct solver_state
        {
            /**
             * The actual stored solver.
             */
            solver_ptr solver;
            /**
             * Watched literals for eastern and southern constraints which are used to reformulate them.
             */
            assumption_literals lit;
        };
        /**
         * Alias for a pointer to a solver state.
         */
        using state_ptr = std::shared_ptr<solver_state>;
        /**
         * To reuse solver states, more information is necessary in the SMT instance generation process. Namely, which
         * tiles have been added in contrast to the last generation and which tiles got new neighbors, i.e. have been
         * updated. Additionally, a container for assumptions, i.e., assertions that are only valid in this and only
         * this run, is needed. All of that is packaged in a solver check point.
         */
        struct solver_check_point
        {
            /**
             * Solver and watched literals.
             */
            state_ptr state;
            /**
             * added_tiles contains only the newly added tiles in eastern or southern direction.
             * updated_tiles instead contains the column (eastern) or row (southern) of tiles that used to be at the
             * border but is not anymore now. In these tiles, certain assertions change so that their assertions need to
             * be reformulated.
             */
            std::set<tile<Lyt>> added_tiles, updated_tiles;
            /**
             * Assumptions that are true for only one solver run. Always includes the assumption literal that did not
             * change.
             */
            z3::expr_vector assumptions;
        };
        /**
         * Alias for a pointer to a solver check point.
         */
        using solver_check_point_ptr = std::shared_ptr<solver_check_point>;
        /**
         * The context used for all solvers.
         */
        const ctx_ptr ctx;
        /**
         * The sketch that later contains the layout generated from a model.
         */
        const Lyt& layout;
        /**
         * Logical specification for the layout.
         */
        const Ntk& network;
        //        /**
        //         * Network hierarchy used for symmetry breaking.
        //         */
        //        network_hierarchy_ptr hierarchy;
        /**
         * Configurations specifying layout restrictions. Used in instance generation among other places.
         */
        const exact_physical_design_params<Lyt> config;
        /**
         * Assumption literal counter.
         */
        std::size_t lc = 0ul;
        /**
         * Tree-like structure mapping already examined dimensions to respective solver states for later reuse.
         * Due to the rather suboptimal exploration strategy of factorizing tile counts, multiple solver states need to
         * be kept. It would always be easiest to simply add an entire row or column to the layout but that way, several
         * tile counts are skipped. E.g. by exploring 4 x 5 after 4 x 4, one would go directly from 16 tiles to 20
         * tiles. If the optimal layout can be found at 18 tiles, it would be missed. Therefore, the exploration
         * strategy using factorizations is kept and several solvers are employed that can be reused at a later point.
         * In the example, the 4 x 4 solver would be stored and revisited when 4 x 5 is to be explored.
         */
        std::map<aspect_ratio<Lyt>, state_ptr> solver_tree{};
        /**
         * Current solver checkpoint extracted from the solver tree.
         */
        solver_check_point_ptr check_point;
        /**
         * Shortcut to the solver stored in check_point.
         */
        solver_ptr solver;
        /**
         * Returns the lc-th eastern assumption literal from the stored context.
         *
         * @return Eastern literal.
         */
        z3::expr get_lit_e() noexcept;
        /**
         * Returns the lc-th southern assumption literal from the stored context.
         *
         * @return Southern literal.
         */
        z3::expr get_lit_s() noexcept;
        /**
         * Accesses the solver tree and looks for a solver state that is associated with a dimension smaller by 1 row or
         * column than given dim. The found one is returned together with the tiles that are new to this solver.
         *
         * If no such solver could be found, a new solver is created from the context given.
         *
         * @param dim Dimension of size x * y.
         * @return Solver state associated with a dimension of size x - 1 * y or x * y - 1 and, additionally, the tiles
         *                new to the solver. If no such solver is available, a new one is created.
         */
        solver_check_point fetch_solver(const aspect_ratio<Lyt>& dim) noexcept;
        /**
         * Checks whether a given tile belongs to the added tiles of the current solver check point.
         *
         * @param t Tile to check.
         * @return True iff t is contained in check_point->added_tiles.
         */
        bool is_added_tile(const tile<Lyt>& t) const noexcept;
        /**
         * Checks whether a given tile belongs to the updated tiles of the current solver check point.
         *
         * @param t Tile to check.
         * @return True iff t is contained in check_point->updated_tiles.
         */
        bool is_updated_tile(const tile<Lyt>& t) const noexcept;
        /**
         * Shortcut to the assumption literals.
         *
         * @return Reference to check_point->state->lit.
         */
        assumption_literals& lit() const noexcept;
        /**
         * Returns a tv variable from the stored context representing that tile t has vertex v assigned.
         *
         * @param t Tile to be considered.
         * @param v Vertex to be considered.
         * @return tv variable from ctx.
         */
        z3::expr get_tv(const tile<Lyt>& t, const mockturtle::node<Ntk> v) noexcept;
        /**
         * Returns a te variable from the stored context representing that tile t has edge e assigned.
         *
         * @param t Tile to be considered.
         * @param e Edge to be considered.
         * @return te variable from ctx.
         */
        z3::expr get_te(const tile<Lyt>& t, const mockturtle::edge<Ntk>& e) noexcept;
        /**
         * Returns a tc variable from the stored context representing that tile t1 and tile t2 are directly connected.
         *
         * @param t1 Tile 1 to be considered.
         * @param t2 Tile 2 to be considered.
         * @return tc variable from ctx.
         */
        z3::expr get_tc(const tile<Lyt>& t1, const tile<Lyt>& t2) noexcept;
        /**
         * Returns a tp variable from the stored context representing that a path from tile t1 to tile t2 exists.
         *
         * @param t1 Tile 1 to be considered.
         * @param t2 Tile 2 to be considered.
         * @return tp variable from ctx.
         */
        z3::expr get_tp(const tile<Lyt>& t1, const tile<Lyt>& t2) noexcept;
        /**
         * Returns a vcl variable from the stored context representing vertex v's (pi) clock number.
         *
         * @param v Vertex to be considered.
         * @return vcl variable from ctx.
         */
        z3::expr get_vcl(const mockturtle::node<Ntk> v) noexcept;
        /**
         * Returns a tcl variable from the stored context representing tile t's clock number.
         *
         * @param t Tile to be considered.
         * @return tcl variable from ctx.
         */
        z3::expr get_tcl(const tile<Lyt>& t) noexcept;
        /**
         * Returns a tl variable from the stored context representing tile t's clock latch delay in cycles.
         *
         * @param t Tile to be considered.
         * @return tl variable from ctx.
         */
        z3::expr get_tl(const tile<Lyt>& t) noexcept;
        /**
         * Helper function for generating an equality of an arbitrary number of expressions.
         *
         * @param v Vector of expressions to equalize.
         * @return Expression that represents the equality of all elements in v.
         */
        z3::expr mk_eq(const z3::expr_vector& v) const noexcept;
        /**
         * Helper function for generating an implication lit -> constraint where lit is the given assumption literal.
         *
         * @param constraint Constraint to be implied.
         * @param lit Assumption literal.
         * @return lit -> constraint.
         */
        z3::expr mk_as(const z3::expr& constraint, const z3::expr& lit) const noexcept;
        /**
         * Helper function for generating an implication lit -> constraint where lit is the assumption literal
         * responsible for t, i.e. e if t is at eastern border, s if t is at southern border, and (e and s) if t is
         * the corner tile.
         *
         * @param constraint Constraint to be implied.
         * @param t Tile to consider for literal picking.
         * @return lit -> constraint.
         */
        z3::expr mk_as_if_se(const z3::expr& constraint, const tile<Lyt>& t) const noexcept;
        /**
         * Constructs a series of expressions to evaluate how many tiles were occupied by a given edge. Therefore, all
         * te variables are translated to expressions of the form ite(te, 1, 0) which allows for applying z3::sum to
         * them. This is a work around because no such api function for pseudo boolean exists.
         *
         * @param e Edge to consider.
         * @param ve Vector of expressions to extend.
         */
        void tile_ite_counters(const mockturtle::edge<Ntk>& e, z3::expr_vector& ve) noexcept;
        /**
         * Adds constraints to the solver to limit the number of elements that are going to be assigned to a tile to one
         * (vertex or edge) if no crossings are allowed. Otherwise, one vertex per tile or two edges per tile can be
         * placed.
         */
        void restrict_tile_elements() noexcept;
        /**
         * Adds constraints to the solver to enforce that each vertex is placed exactly once on exactly one tile.
         */
        void restrict_vertices() noexcept;
        /**
         * Adds constraints to the solver to enforce that each clock zone variable has valid bounds of 0 <= cl <= C,
         * where C is the maximum clock number.
         */
        void restrict_clocks() noexcept;
        /**
         * Adds constraints to the solver to enforce that a tile which was assigned with some vertex v has a successor
         * that is assigned to the adjacent vertex of v or an outgoing edge of v.
         */
        void define_adjacent_vertex_tiles() noexcept;
        /**
         * Adds constraints to the solver to enforce that a tile which was assigned with some vertex v has a predecessor
         * that is assigned to the inversely adjacent vertex of v or an incoming edge of v.
         */
        void define_inv_adjacent_vertex_tiles() noexcept;
        /**
         * Adds constraints to the solver to enforce that a tile that was assigned with some edge has a successor which
         * is assigned to the adjacent vertex or another edge.
         */
        void define_adjacent_edge_tiles() noexcept;
        /**
         * Adds constraints to the solver to enforce that a tile that was assigned with some edge has a predecessor
         * which is assigned to the inversely adjacent vertex or another edge.
         */
        void define_inv_adjacent_edge_tiles() noexcept;
        /**
         * Adds constraints to the solver to map established connections between single tiles to sub-paths. They are
         * spanned transitively by the next set of constraints.
         */
        void establish_sub_paths() noexcept;
        /**
         * Adds constraints to the solver to expand the formerly created sub-paths transitively.
         */
        void establish_transitive_paths() noexcept;
        /**
         * Adds constraints to the solver to prohibit cycles that loop back information. To this end, the formerly
         * established paths are used. Without this constraint, useless wire loops appear.
         */
        void eliminate_cycles() noexcept;
        /**
         * Adds constraints to the solver to ensure that the cl variable of primary input pi is set to the clock zone
         * value of the tile pi is assigned to. Necessary to be taken into account for path lengths.
         */
        void assign_pi_clockings() noexcept;
        /**
         * Adds constraints to the solver to ensure that fan-in paths to the same tile need to have the same length
         * in the layout modulo timing, i.e. plus the clock zone assigned to their PIs.
         */
        void global_synchronization() noexcept;
        /**
         * Adds constraints to the solver to prevent edges or vertices to be assigned to tiles with an insufficient
         * number of predecessors/successors. Symmetry breaking constraints.
         */
        void prevent_insufficiencies() noexcept;
        /**
         * Adds constraints to the solver to define the number of connection variables to be set for each tile, i.e.
         * empty tiles are not allowed to have connections at all, edges need to have one ingoing and one outgoing
         * connection and so on. Symmetry breaking constraints.
         */
        void define_number_of_connections() noexcept;
        /**
         * Adds constraints to the solver to prohibit certain vertex placements based on the network hierarchy if the
         * clocking scheme is feed-back-free. Symmetry breaking constraints.
         */
        void utilize_hierarchical_information() noexcept;
        /**
         * Adds constraints to the solver to position the primary inputs and primary outputs at the layout's borders.
         */
        void enforce_border_io() noexcept;
        /**
         * Adds constraints to the solver to enforce that no bent inverters are used.
         */
        void enforce_straight_inverters() noexcept;
        /**
         * Adds constraints to the solver to prevent negative valued clock latches and that vertex tiles cannot be
         * latches.
         */
        void restrict_clock_latches() noexcept;
        /**
         * Adds constraints to the solver to enforce topology-specific restrictions.
         */
        void topology_specific_constraints() noexcept;
        /**
         * Adds constraints to the given optimize to minimize the number of crossing tiles to use.
         *
         * @param optimize Pointer to an z3::optimize to add constraints to.
         */
        void minimize_wires(optimize_ptr optimize) noexcept;
        /**
         * Adds constraints to the given optimize to minimize the number of crossing tiles to use.
         *
         * @param optimize Pointer to an z3::optimize to add constraints to.
         */
        void minimize_crossings(optimize_ptr optimize) noexcept;
        /**
         * Adds constraints to the given optimize to enforce that the overall sum of latch values should be minimized.
         *
         * @param optimize Pointer to an z3::optimize to add constraints to.
         */
        void minimize_clock_latches(optimize_ptr optimize) noexcept;
        /**
         * Generates the SMT instance by calling the constraint generating functions.
         */
        void generate_smt_instance() noexcept;
        /**
         * Creates and returns a z3::optimize if optimization criteria were set by the configuration. The optimize gets
         * passed all constraints from the current solver and the respective optimization constraints are added to it,
         * too.
         */
        optimize_ptr optimize() noexcept;
        /**
         * Assigns vertices, edges and directions to the stored layout sketch with respect to the given model.
         *
         * @param model A satisfying assignment to the created variables under all created constraints that can be
         *              used to extract a layout description.
         */
        void assign_layout(const z3::model& model) noexcept;
    };

    /**
     * Calculates the time left for solving by subtracting the time passed from the configured timeout and updates
     * Z3's timeout accordingly.
     *
     * @param handler Handler whose timeout is to be updated.
     * @param time Time passed since beginning of the solving process.
     */
    void update_timeout(smt_handler& handler, mockturtle::stopwatch<>::duration time) const;
    /**
     * Contains a context pointer and a currently worked on dimension and can be shared between multiple worker threads
     * so that they can notify each other via context interrupts based on their individual results, i.e. a thread that
     * found a result at dimension x * y can interrupt all other threads that are working on larger layout sizes.
     */
    struct thread_info
    {
        /**
         * Pointer to a context.
         */
        ctx_ptr ctx;
        /**
         * Currently examined layout dimension.
         */
        aspect_ratio<Lyt> worker_dimension;
    };
    /**
     * Thread function for the asynchronous solving strategy. It registers its own context in the given list of
     * thread_info objects and starts exploring the search space. It fetches the next dimension to work on from the
     * global dimension iterator which is protected by a mutex. When a result is found, other threads that are currently
     * working on larger layout dimensions are interrupted while smaller ones may finish running.
     *
     * @param t_num Thread's identifier.
     * @param ti_list Pointer to a list of shared thread info that the threads use for communication.
     * @return A found layout or nullptr if being interrupted.
     */
    Lyt explore_asynchronously(const unsigned t_num, std::shared_ptr<std::vector<thread_info>> ti_list) noexcept;
    /**
     * Launches config.num_threads threads and evaluates their return statements.
     *
     * @return Physical design result including statistical information.
     */
    std::optional<Lyt> run_asynchronously() noexcept;
    /**
     * Does the same as explore_asynchronously but without thread synchronization overhead.
     *
     * @return Physical design result including statistical information.
     */
    std::optional<Lyt> run_synchronously() noexcept;
};

}  // namespace detail

/**
 * An exact physical design approach using SMT solving. This class provides the interface via which the technique
 * can be called from the CLI. The instance generation happens in its sub-class smt_handler.
 *
 * Via incremental SMT calls, an optimal placement & routing for a given logic network will be found. Starting with n,
 * each possible layout dimension in n tiles will be examined by factorization and tested for routability with the SMT
 * solver z3. The number n thereby is the lower bound which is equal to the number of vertices in the given logic
 * network. When no upper bound is given, this approach will run until it finds a solution to the placement & routing
 * problem instance under all given constraints. Note that there a combinations of constraints for which no valid
 * solution under the given parameters exist for the given logic network. Recommended settings include the use of
 * I/O pins located at the layout borders for better integration. Most networks are not realizable without crossings
 * enabled. Specifying a clocking scheme SIGNIFICANTLY speeds up the process. 2DDWave allows for the strictest
 * constraints and thereby finds a solution the quickest. However, for high input degree networks, no valid solution
 * exists when border I/Os are to be used unless global synchronization is disabled. Generally, solutions are found
 * the fastest with the following settings: Crossings enabled, de-synchronization enabled, and 2DDWave clocking given.
 * Multi-threading can sometimes speed up the process especially for large networks. Note that the more threads are
 * being used, the less information can be shared across the individual solver runs which destroys the benefits of
 * incremental solving and thereby, comparatively, slows down each run.
 *
 * The SMT instance works with a single layer of variables even though it is possible to allow crossings in the
 * solution. The reduced number of variables saves a considerable amount of runtime. That's why most for-loops iterate
 * over layout->get_ground_layer_tiles() even though the model will be mapped to a 3-dimensional layout afterwards.
 */
template <typename Lyt, typename Ntk>
std::optional<Lyt> exact(const Ntk& ntk, exact_physical_design_params<Lyt> ps = {},
                         exact_physical_design_stats* pst = nullptr)
{
    static_assert(is_gate_level_layout_v<Lyt>, "Lyt is not a gate-level layout");
    static_assert(mockturtle::is_network_type_v<Ntk>,
                  "Ntk is not a network type");  // Ntk is being converted to a topology_network anyways, therefore,
                                                 // this is the only relevant check here

    exact_physical_design_stats  st{};
    detail::exact_impl<Lyt, Ntk> p{ntk, ps, st};

    auto result = p.run();

    if (pst)
    {
        *pst = st;
    }

    return result;
}

}  // namespace fiction

#endif  // FICTION_EXACT_HPP

#endif  // FICTION_Z3_SOLVER
