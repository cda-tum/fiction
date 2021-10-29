//
// Created by marcel on 06.01.20.
//

#if (FICTION_Z3_SOLVER)

#ifndef FICTION_EXACT_HPP
#define FICTION_EXACT_HPP

#include "../io/print_layout.hpp"
#include "../layouts/clocking_scheme.hpp"
#include "../traits.hpp"
#include "../utils/debug/network_writer.hpp"
#include "fanout_substitution.hpp"
#include "iter/aspect_ratio_iterator.hpp"
#include "layout_utils.hpp"
#include "name_restoration.hpp"
#include "network_utils.hpp"

#include <fmt/format.h>
#include <mockturtle/traits.hpp>
#include <mockturtle/utils/node_map.hpp>
#include <mockturtle/utils/stopwatch.hpp>
#include <mockturtle/views/color_view.hpp>
#include <mockturtle/views/fanout_view.hpp>
#include <mockturtle/views/topo_view.hpp>
#if (PROGRESS_BARS)
#include <mockturtle/utils/progress_bar.hpp>
#endif

#include <z3++.h>

#include <algorithm>
#include <cassert>
#include <chrono>
#include <cstdint>
#include <future>
#include <memory>
#include <mutex>
#include <optional>
#include <set>
#include <thread>
#include <vector>

namespace fiction
{

template <typename Lyt>
struct exact_physical_design_params
{
    /**
     * Clocking scheme to be used.
     */
    std::shared_ptr<clocking_scheme<coordinate<Lyt>>> scheme =
        std::make_shared<clocking_scheme<coordinate<Lyt>>>(twoddwave_clocking<Lyt>());
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
    bool io_ports = true;  // TODO right now, this has to be true
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
            ps{p},
            pst{st}
    {
        mockturtle::names_view<topology_network> intermediate_ntk{
            fanout_substitution<mockturtle::names_view<topology_network>>(
                src, {fanout_substitution_params::substitution_strategy::BREADTH, ps.scheme->max_out_degree, 1ul})};

        // create PO nodes in the network
        intermediate_ntk.substitute_po_signals();

        ntk = std::make_shared<topology_ntk_t>(mockturtle::fanout_view{intermediate_ntk});

        lower_bound = static_cast<decltype(lower_bound)>(
            ntk->num_gates() + ntk->num_pis());  // TODO check lower bound minimum number tiles to use

        dit = aspect_ratio_iterator<aspect_ratio<Lyt>>{ps.fixed_size ? ps.fixed_size : lower_bound};
    }

    std::optional<Lyt> run()
    {
        if (ps.num_threads > 1)
        {
            return run_asynchronously();
        }
        else
        {
            return run_synchronously();
        }
    }

  private:
    using topology_ntk_t = mockturtle::topo_view<mockturtle::fanout_view<mockturtle::names_view<topology_network>>>;

    std::shared_ptr<topology_ntk_t> ntk;  // TODO incorporate clocking scheme degree into fanout-substitution

    exact_physical_design_params<Lyt> ps;
    exact_physical_design_stats&      pst;
    /**
     * Lower bound for the number of layout tiles.
     */
    uint16_t lower_bound{0};

    aspect_ratio_iterator<aspect_ratio<Lyt>> dit{0};

    /**
     * Aspect ratio of found result. Only interesting for asynchronous case.
     */
    std::optional<aspect_ratio<Lyt>> result_dimension;
    /**
     * Restricts access to the aspect_ratio_iterator and the result_dimension.
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
         * @param ctxp The context that is used in all solvers.
         * @param fgl The gate layout pointer that is going to contain the created layout.
         * @param c The configurations to respect in the SMT instance generation process.
         */
        smt_handler(ctx_ptr ctxp, Lyt& fgl, const topology_ntk_t& ntk,
                    const exact_physical_design_params<Lyt>& c) noexcept :
                ctx{std::move(ctxp)},
                layout{fgl},
                network{ntk},
                node2pos{ntk},
                //                hierarchy{std::make_shared<network_hierarchy>(network, false)}, // TODO hiearchy
                config{c}
        {}
        /**
         * Evaluates a given dimension regarding the stored configurations whether it can be skipped, i.e. does not
         * need to be explored by the SMT solver. The better this function is, the more UNSAT instances can be skipped
         * without losing the optimality guarantee. This function should never be overly restrictive!
         *
         * @param dim Dimension to evaluate.
         * @return True if dim can safely be skipped because it is UNSAT anyways.
         */
        bool skippable(const aspect_ratio<Lyt>& dim) const noexcept
        {
            // OPEN clocking optimization: rotated dimensions don't need to be explored
            if (!layout.is_regularly_clocked())
            {
                if (dim.x != dim.y && dim.x == layout.y() && dim.y == layout.x())
                    return true;
            }
            // ToPoliNano optimization: skip all dimensions where X <= levels + number of fan-outs - 1 and
            // Y < maximum of number of PIs and number of POs; this should not be too restrictive
            //            else if (config.topolinano)
            //            {
            //                if (dim.x <= hierarchy->height() + network->operation_count(operation::F1O2) - 1 ||
            //                    dim.y < std::max(network->num_pis(), network->num_pos()))
            //                    return true;
            //            }  // TODO hierarchy

            return false;
        }
        /**
         * Resizes the layout and creates a new solver checkpoint from where on the next incremental instance can be
         * generated.
         *
         * @param dim Current dimension to work on.
         */
        void update(const aspect_ratio<Lyt>& dim) noexcept
        {
            layout.resize({dim.x, dim.y, config.crossings ? 1 : 0});
            check_point = std::make_shared<solver_check_point>(fetch_solver(dim));
            ++lc;
            solver = check_point->state->solver;
        }
        /**
         * Sets the given timeout for the current solver.
         *
         * @param t Timeout in ms.
         */
        void set_timeout(const unsigned t)
        {
            z3::params p{*ctx};
            p.set("timeout", t);
            solver->set(p);
        }
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
        bool is_satisfiable()
        {
            generate_smt_instance();

            switch (solver->check(check_point->assumptions))
            {
                case z3::sat:
                {
                    // TODO out-of-solver constraints and maybe going back into solver

                    // optimize the generated result
                    if (auto opt = optimize(); opt != nullptr)
                    {
                        opt->check();
                        assign_layout(opt->get_model());
                    }
                    else
                    {
                        assign_layout(solver->get_model());
                    }

                    return true;
                }
                default:
                {
                    return false;
                }
            }
        }

        /**
         * Stores the current solver state in the solver tree with dimension dim as key.
         *
         * @param dim Key to storing the current solver state.
         */
        void store_solver_state(const aspect_ratio<Lyt>& dim) noexcept
        {
            solver_tree[dim] = check_point->state;
        }

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
        Lyt& layout;
        /**
         * Logical specification for the layout.
         */
        const topology_ntk_t& network;
        /**
         * Maps nodes to tile positions.
         */
        mockturtle::node_map<branching_signal_container<Lyt, topology_ntk_t>, topology_ntk_t>
            node2pos;  // TODO include fanout_size
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
        z3::expr get_lit_e() noexcept
        {
            return ctx->bool_const(fmt::format("lit_e_{}", lc).c_str());
        }
        /**
         * Returns the lc-th southern assumption literal from the stored context.
         *
         * @return Southern literal.
         */
        z3::expr get_lit_s() noexcept
        {
            return ctx->bool_const(fmt::format("lit_s_{}", lc).c_str());
        }
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
        solver_check_point fetch_solver(const aspect_ratio<Lyt>& dim) noexcept
        {
            const auto create_assumptions = [this](const solver_state& state) -> z3::expr_vector
            {
                z3::expr_vector assumptions{*ctx};
                assumptions.push_back(state.lit.s);
                assumptions.push_back(state.lit.e);

                return assumptions;
            };

            // does a solver state for a layout of dimension of size x - 1 * y exist?
            if (auto it_x = solver_tree.find({dim.x - 1, dim.y}); it_x != solver_tree.end())
            {
                // gather additional y-tiles and updated tiles
                std::set<tile<Lyt>> added_tiles{}, updated_tiles{};
                for (decltype(dim.y) y = 0; y <= dim.y; ++y)
                {
                    added_tiles.emplace(tile<Lyt>{dim.x, y});
                    updated_tiles.emplace(tile<Lyt>{dim.x - 1, y});
                }

                // deep-copy solver state
                const auto   state     = it_x->second;
                solver_state new_state = {state->solver, {get_lit_e(), state->lit.s}};

                // reset eastern constraints
                new_state.solver->add(not state->lit.e);

                // remove solver
                solver_tree.erase(it_x);

                return {std::make_shared<solver_state>(new_state), added_tiles, updated_tiles,
                        create_assumptions(new_state)};
            }
            else
            {
                // does a solver state for a layout of dimension of size x * y - 1 exist?
                if (auto it_y = solver_tree.find({dim.x, dim.y - 1}); it_y != solver_tree.end())
                {
                    // gather additional x-tiles
                    std::set<tile<Lyt>> added_tiles{}, updated_tiles{};
                    for (decltype(dim.x) x = 0; x <= dim.x; ++x)
                    {
                        added_tiles.emplace(tile<Lyt>{x, dim.y});
                        updated_tiles.emplace(tile<Lyt>{x, dim.y - 1});
                    }

                    // deep-copy solver state
                    const auto   state     = it_y->second;
                    solver_state new_state = {state->solver, {state->lit.e, get_lit_s()}};

                    // reset southern constraints
                    new_state.solver->add(not state->lit.s);

                    // remove solver
                    solver_tree.erase(it_y);

                    return {std::make_shared<solver_state>(new_state), added_tiles, updated_tiles,
                            create_assumptions(new_state)};
                }
                else  // no existing solver state; create a new one
                {
                    // all tiles are additional ones
                    std::set<tile<Lyt>> added_tiles{};
                    for (decltype(dim.y) y = 0; y <= dim.y; ++y)
                    {
                        for (decltype(dim.x) x = 0; x <= dim.x; ++x) { added_tiles.emplace(tile<Lyt>{x, y}); }
                    }

                    // create new state
                    solver_state new_state{std::make_shared<z3::solver>(*ctx), {get_lit_e(), get_lit_s()}};

                    return {std::make_shared<solver_state>(new_state), added_tiles, {}, create_assumptions(new_state)};
                }
            }
        }
        /**
         * Checks whether a given tile belongs to the added tiles of the current solver check point.
         *
         * @param t Tile to check.
         * @return True iff t is contained in check_point->added_tiles.
         */
        bool is_added_tile(const tile<Lyt>& t) const noexcept
        {
            return check_point->added_tiles.count(t);
        }
        /**
         * Checks whether a given tile belongs to the updated tiles of the current solver check point.
         *
         * @param t Tile to check.
         * @return True iff t is contained in check_point->updated_tiles.
         */
        bool is_updated_tile(const tile<Lyt>& t) const noexcept
        {
            return check_point->updated_tiles.count(t);
        }
        /**
         * Returns true, iff config.io_ports is set to false and n is either a constant or PI or PO node in network.
         *
         * @param n Node in network.
         * @return True iff n is to be skipped in a loop due to it being a constant or an I/O and config.io_ports ==
         * false.
         */
        bool skip_const_or_io_node(const mockturtle::node<topology_ntk_t>& n) const noexcept
        {
            return network.is_constant(n) || ((network.is_pi(n) || network.is_po(n)) && !config.io_ports);
        }
        /**
         * Returns true, iff skip_const_or_io_node returns true for either source or target of the given edge..
         *
         * @param e Edge in network.
         * @return True iff e is to be skipped in a loop due to it having constant or I/O nodes while config.io_ports ==
         * false.
         */
        bool skip_const_or_io_edge(const mockturtle::edge<topology_ntk_t>& e) const noexcept
        {
            return skip_const_or_io_node(e.source) || skip_const_or_io_node(e.target);
        }
        /**
         * Applies a given function to all added and updated tiles in the current solver check point.
         *
         * @tparam Fn Functor type.
         * @param fn Unary function to apply to all added and updated tiles. Must receive a tile as parameter.
         */
        template <typename Fn>
        void apply_to_added_and_updated_tiles(Fn&& fn)
        {
            std::for_each(check_point->added_tiles.cbegin(), check_point->added_tiles.cend(), fn);
            std::for_each(check_point->updated_tiles.cbegin(), check_point->updated_tiles.cend(), fn);
        }

        uint32_t network_in_degree(const mockturtle::node<topology_ntk_t>& n) const noexcept
        {
            uint32_t degree{0};
            network.foreach_fanin(n,
                                  [this, &degree](const auto& fi)
                                  {
                                      if (const auto fn = network.get_node(fi); !skip_const_or_io_node(fn))
                                      {
                                          ++degree;
                                      }
                                  });
            return degree;
        }

        uint32_t network_out_degree(const mockturtle::node<topology_ntk_t>& n) const noexcept
        {
            uint32_t degree{0};
            network.foreach_fanout(n,
                                   [this, &degree](const auto& fo)
                                   {
                                       if (const auto fn = network.get_node(fo); !skip_const_or_io_node(fn))
                                       {
                                           ++degree;
                                       }
                                   });
            return degree;
        }
        /**
         * Shortcut to the assumption literals.
         *
         * @return Reference to check_point->state->lit.
         */
        assumption_literals& lit() const noexcept
        {
            return check_point->state->lit;
        }
        /**
         * Returns a tv variable from the stored context representing that tile t has vertex v assigned.
         *
         * @param t Tile to be considered.
         * @param v Vertex to be considered.
         * @return tv variable from ctx.
         */
        z3::expr get_tv(const tile<Lyt>& t, const mockturtle::node<topology_ntk_t> v)
        {
            return ctx->bool_const(fmt::format("tv_({},{})_{}", t.x, t.y, v).c_str());
        }
        /**
         * Returns a te variable from the stored context representing that tile t has edge e assigned.
         *
         * @param t Tile to be considered.
         * @param e Edge to be considered.
         * @return te variable from ctx.
         */
        z3::expr get_te(const tile<Lyt>& t, const mockturtle::edge<topology_ntk_t>& e)
        {
            return ctx->bool_const(fmt::format("te_({},{})_({},{})", t.x, t.y, e.source, e.target).c_str());
        }
        /**
         * Returns a tc variable from the stored context representing that tile t1 and tile t2 are directly connected.
         *
         * @param t1 Tile 1 to be considered.
         * @param t2 Tile 2 to be considered.
         * @return tc variable from ctx.
         */
        z3::expr get_tc(const tile<Lyt>& t1, const tile<Lyt>& t2)
        {
            return ctx->bool_const(fmt::format("tc_({},{})_({},{})", t1.x, t1.y, t2.x, t2.y).c_str());
        }
        /**
         * Returns a tp variable from the stored context representing that a path from tile t1 to tile t2 exists.
         *
         * @param t1 Tile 1 to be considered.
         * @param t2 Tile 2 to be considered.
         * @return tp variable from ctx.
         */
        z3::expr get_tp(const tile<Lyt>& t1, const tile<Lyt>& t2)
        {
            return ctx->bool_const(fmt::format("tp_({},{})_({},{})", t1.x, t1.y, t2.x, t2.y).c_str());
        }
        /**
         * Returns a vcl variable from the stored context representing vertex v's (pi) clock number.
         *
         * @param v Vertex to be considered.
         * @return vcl variable from ctx.
         */
        z3::expr get_vcl(const mockturtle::node<topology_ntk_t> v)
        {
            return ctx->int_const(fmt::format("vcl_{}", v).c_str());
        }
        /**
         * Returns a tcl variable from the stored context representing tile t's clock number.
         *
         * @param t Tile to be considered.
         * @return tcl variable from ctx.
         */
        z3::expr get_tcl(const tile<Lyt>& t)
        {
            return ctx->int_const(fmt::format("tcl_({},{})", t.x, t.y).c_str());
        }
        /**
         * Returns a tl variable from the stored context representing tile t's clock latch delay in cycles.
         *
         * @param t Tile to be considered.
         * @return tl variable from ctx.
         */
        z3::expr get_tl(const tile<Lyt>& t)
        {
            return ctx->int_const(fmt::format("tl_({},{})", t.x, t.y).c_str());
        }
        /**
         * Helper function for generating an equality of an arbitrary number of expressions.
         *
         * @param v Vector of expressions to equalize.
         * @return Expression that represents the equality of all elements in v.
         */
        z3::expr mk_eq(const z3::expr_vector& v) const
        {
            z3::expr_vector eq{*ctx};
            for (int i = 1; static_cast<decltype(v.size())>(i) < v.size(); ++i) { eq.push_back(v[i - 1] == v[i]); }

            return z3::mk_and(eq);
        }
        /**
         * Helper function for generating an implication lit -> constraint where lit is the given assumption literal.
         *
         * @param constraint Constraint to be implied.
         * @param lit Assumption literal.
         * @return lit -> constraint.
         */
        z3::expr mk_as(const z3::expr& constraint, const z3::expr& lit) const
        {
            return z3::implies(lit, constraint);
        }
        /**
         * Helper function for generating an implication lit -> constraint where lit is the assumption literal
         * responsible for t, i.e. e if t is at eastern border, s if t is at southern border, and (e and s) if t is
         * the corner tile.
         *
         * @param constraint Constraint to be implied.
         * @param t Tile to consider for literal picking.
         * @return lit -> constraint.
         */
        z3::expr mk_as_if_se(const z3::expr& constraint, const tile<Lyt>& t) const
        {
            if (auto east = layout.is_eastern_border(t), south = layout.is_southern_border(t); east && south)
            {
                return mk_as(constraint, lit().e and lit().s);
            }
            else
            {
                if (east)
                {
                    return mk_as(constraint, lit().e);
                }
                else if (south)
                {
                    return mk_as(constraint, lit().s);
                }
                else
                {
                    return constraint;
                }
            }
        }
        /**
         * Constructs a series of expressions to evaluate how many tiles were occupied by a given edge. Therefore, all
         * te variables are translated to expressions of the form ite(te, 1, 0) which allows for applying z3::sum to
         * them. This is a work around because no such api function for pseudo boolean exists.
         *
         * @param e Edge to consider.
         * @param ve Vector of expressions to extend.
         */
        void tile_ite_counters(const mockturtle::edge<topology_ntk_t>& e, z3::expr_vector& ve)
        {
            z3::expr one  = ctx->real_val(1u);
            z3::expr zero = ctx->real_val(0u);

            z3::expr num_phases = ctx->real_val(static_cast<unsigned>(layout.num_clocks()));

            layout.foreach_ground_tile(
                [this, &e, &ve, &one, &zero, &num_phases](const auto& t)
                {
                    // an artificial latch variable counts as an extra 1 clock cycle (n clock phases)
                    if (config.synchronization_elements)
                    {
                        ve.push_back(z3::ite(get_te(t, e), get_tl(t) * num_phases + one, zero));
                    }
                    else
                    {
                        ve.push_back(z3::ite(get_te(t, e), one, zero));
                    }
                });
        }
        /**
         * Adds constraints to the solver to limit the number of elements that are going to be assigned to a tile to one
         * (vertex or edge) if no crossings are allowed. Otherwise, one vertex per tile or two edges per tile can be
         * placed.
         */
        void restrict_tile_elements()
        {
            for (const auto& t : check_point->added_tiles)
            {
                if (config.crossings)
                {
                    z3::expr_vector tv{*ctx};
                    network.foreach_node(
                        [this, &t, &tv](const auto& v)
                        {
                            if (!skip_const_or_io_node(v))
                            {
                                tv.push_back(get_tv(t, v));
                            }
                        });

                    if (!tv.empty())
                    {
                        // at most 1 vertex
                        solver->add(z3::atmost(tv, 1u));
                    }

                    z3::expr_vector te{*ctx};

                    foreach_edge(network,
                                 [this, &t, &te](const auto& e)
                                 {
                                     if (!skip_const_or_io_edge(e))
                                     {
                                         te.push_back(get_te(t, e));
                                     }
                                 });

                    if (!te.empty())
                    {
                        // at most 2 edges
                        solver->add(z3::atmost(te, 2u));
                    }

                    if (!tv.empty() && !te.empty())
                    {
                        // prevent the assignment of both vertices and edges to the same tile
                        solver->add(z3::implies(z3::atleast(tv, 1u), z3::atmost(te, 0)));
                        solver->add(z3::implies(z3::atleast(te, 1u), z3::atmost(tv, 0)));
                    }
                }
                else
                {
                    z3::expr_vector ve{*ctx};
                    network.foreach_node(
                        [this, &t, &ve](const auto& v)
                        {
                            if (!skip_const_or_io_node(v))
                            {
                                ve.push_back(get_tv(t, v));
                            }
                        });

                    foreach_edge(network,
                                 [this, &t, &ve](const auto& e)
                                 {
                                     if (!skip_const_or_io_edge(e))
                                     {
                                         ve.push_back(get_te(t, e));
                                     }
                                 });

                    if (!ve.empty())
                    {
                        // at most 1 vertex or edge
                        solver->add(z3::atmost(ve, 1u));
                    }
                }
            }
        }
        /**
         * Adds constraints to the solver to enforce that each vertex is placed exactly once on exactly one tile.
         */
        void restrict_vertices()
        {
            network.foreach_node(
                [this](const auto& n)
                {
                    if (!skip_const_or_io_node(n))
                    {
                        z3::expr_vector ve{*ctx};
                        layout.foreach_ground_tile([this, &n, &ve](const auto& t) { ve.push_back(get_tv(t, n)); });

                        // use a tracking literal to disable constraints in case of UNSAT
                        solver->add(mk_as(z3::atleast(ve, 1u), lit().e and lit().s));
                        solver->add(z3::atmost(ve, 1u));
                    }
                });
        }
        /**
         * Adds constraints to the solver to enforce that each clock zone variable has valid bounds of 0 <= cl <= C,
         * where C is the maximum clock number.
         */
        void restrict_clocks()
        {
            for (const auto& t : check_point->added_tiles)
            {
                auto cl = get_tcl(t);
                solver->add(ctx->int_val(0) <= cl);
                solver->add(cl < ctx->int_val(static_cast<unsigned>(layout.num_clocks())));
            }
        }
        /**
         * Adds constraints to the solver to enforce that a tile which was assigned with some vertex v has a successor
         * that is assigned to the adjacent vertex of v or an outgoing edge of v.
         */
        void define_adjacent_vertex_tiles()
        {
            apply_to_added_and_updated_tiles(
                [this](const auto& t)
                {
                    network.foreach_node(
                        [this, &t](const auto& v)
                        {
                            if (!skip_const_or_io_node(v))
                            {
                                auto tv = get_tv(t, v);

                                z3::expr_vector conj{*ctx};
                                foreach_outgoing_edge(
                                    network, v,
                                    [this, &t, &conj](const auto& ae)
                                    {
                                        if (!skip_const_or_io_edge(ae))
                                        {
                                            z3::expr_vector disj{*ctx};

                                            if (auto tgt = ae.target; layout.is_regularly_clocked())
                                            {
                                                layout.foreach_outgoing_clocked_zone(
                                                    t,
                                                    [this, &t, &disj, &tgt, &ae](const auto& at) {
                                                        disj.push_back((get_tv(at, tgt) or get_te(at, ae)) and
                                                                       get_tc(t, at));
                                                    });
                                            }
                                            else  // irregular clocking
                                            {
                                                layout.foreach_adjacent_tile(
                                                    t,
                                                    [this, &t, &disj, &tgt, &ae](const auto& at)
                                                    {
                                                        // clocks must differ by 1
                                                        auto mod = z3::mod(get_tcl(at) - get_tcl(t),
                                                                           layout.num_clocks()) == ctx->int_val(1);

                                                        disj.push_back(((get_tv(at, tgt) or get_te(at, ae)) and mod) and
                                                                       get_tc(t, at));
                                                    });
                                            }

                                            if (!disj.empty())
                                            {
                                                conj.push_back(z3::mk_or(disj));
                                            }
                                        }
                                    });

                                if (!conj.empty())
                                {
                                    solver->add(mk_as_if_se(z3::implies(tv, z3::mk_and(conj)), t));
                                }
                            }
                        });
                });
        }
        /**
         * Adds constraints to the solver to enforce that a tile which was assigned with some vertex v has a predecessor
         * that is assigned to the inversely adjacent vertex of v or an incoming edge of v.
         */
        void define_inv_adjacent_vertex_tiles()
        {
            apply_to_added_and_updated_tiles(
                [this](const auto& t)
                {
                    network.foreach_node(
                        [this, &t](const auto& v)
                        {
                            if (!skip_const_or_io_node(v))
                            {
                                auto tv = get_tv(t, v);

                                z3::expr_vector conj{*ctx};

                                foreach_incoming_edge(
                                    network, v,
                                    [this, &t, &conj](const auto& iae)
                                    {
                                        if (!skip_const_or_io_edge(iae))
                                        {
                                            z3::expr_vector disj{*ctx};

                                            if (auto src = iae.source; layout.is_regularly_clocked())
                                            {
                                                layout.foreach_incoming_clocked_zone(
                                                    t,
                                                    [this, &t, &disj, &src, &iae](const auto& iat) {
                                                        disj.push_back((get_tv(iat, src) or get_te(iat, iae)) and
                                                                       get_tc(iat, t));
                                                    });
                                            }
                                            else  // irregular clocking
                                            {
                                                layout.foreach_adjacent_tile(
                                                    t,
                                                    [this, &t, &disj, &src, &iae](const auto& iat)
                                                    {
                                                        // clocks must differ by 1
                                                        auto mod = z3::mod(get_tcl(t) - get_tcl(iat),
                                                                           layout.num_clocks()) == ctx->int_val(1);

                                                        disj.push_back(
                                                            ((get_tv(iat, src) or get_te(iat, iae)) and mod) and
                                                            get_tc(iat, t));
                                                    });
                                            }

                                            if (!disj.empty())
                                            {
                                                conj.push_back(z3::mk_or(disj));
                                            }
                                        }
                                    });

                                if (!conj.empty())
                                {
                                    solver->add(mk_as_if_se(z3::implies(tv, z3::mk_and(conj)), t));
                                }
                            }
                        });
                });
        }
        /**
         * Adds constraints to the solver to enforce that a tile that was assigned with some edge has a successor which
         * is assigned to the adjacent vertex or another edge.
         */
        void define_adjacent_edge_tiles()
        {
            apply_to_added_and_updated_tiles(
                [this](const auto& t)
                {
                    foreach_edge(
                        network,
                        [this, &t](const auto& e)
                        {
                            if (!skip_const_or_io_edge(e))
                            {
                                auto te = e.target;

                                z3::expr_vector disj{*ctx};

                                if (layout.is_regularly_clocked())
                                {
                                    layout.foreach_outgoing_clocked_zone(
                                        t, [this, &t, &e, &te, &disj](const auto& at)
                                        { disj.push_back((get_tv(at, te) or get_te(at, e)) and get_tc(t, at)); });
                                }
                                else  // irregular clocking
                                {
                                    layout.foreach_adjacent_tile(
                                        t,
                                        [this, &t, &e, &te, &disj](const auto& at)
                                        {
                                            // clocks must differ by 1
                                            auto mod = z3::mod(get_tcl(at) - get_tcl(t), layout.num_clocks()) ==
                                                       ctx->int_val(1);

                                            disj.push_back(((get_tv(at, te) or get_te(at, e)) and mod) and
                                                           get_tc(t, at));
                                        });
                                }

                                if (!disj.empty())
                                {
                                    solver->add(mk_as_if_se(z3::implies(get_te(t, e), z3::mk_or(disj)), t));
                                }
                            }
                        });
                });
        }
        /**
         * Adds constraints to the solver to enforce that a tile that was assigned with some edge has a predecessor
         * which is assigned to the inversely adjacent vertex or another edge.
         */
        void define_inv_adjacent_edge_tiles()
        {
            apply_to_added_and_updated_tiles(
                [this](const auto& t)
                {
                    foreach_edge(
                        network,
                        [this, &t](const auto& e)
                        {
                            if (!skip_const_or_io_edge(e))
                            {
                                auto se = e.source;

                                z3::expr_vector disj{*ctx};

                                if (layout.is_regularly_clocked())
                                {
                                    layout.foreach_incoming_clocked_zone(
                                        t, [this, &t, &e, &se, &disj](const auto& iat)
                                        { disj.push_back((get_tv(iat, se) or get_te(iat, e)) and get_tc(iat, t)); });
                                }
                                else  // irregular clocking
                                {
                                    layout.foreach_adjacent_tile(
                                        t,
                                        [this, &t, &e, &se, &disj](const auto& iat)
                                        {
                                            // clocks must differ by 1
                                            auto mod = z3::mod(get_tcl(t) - get_tcl(iat), layout.num_clocks()) ==
                                                       ctx->int_val(1);

                                            disj.push_back(((get_tv(iat, se) or get_te(iat, e)) and mod) and
                                                           get_tc(iat, t));
                                        });
                                }

                                if (!disj.empty())
                                {
                                    solver->add(mk_as_if_se(z3::implies(get_te(t, e), z3::mk_or(disj)), t));
                                }
                            }
                        });
                });
        }

        /**
         * Adds constraints to the solver to map established connections between single tiles to sub-paths. They are
         * spanned transitively by the next set of constraints.
         */
        void establish_sub_paths()
        {
            layout.foreach_ground_tile(
                [this](const auto& t)
                {
                    if (layout.is_regularly_clocked())
                    {
                        layout.foreach_outgoing_clocked_zone(t,
                                                             [this, &t](const auto& at)
                                                             {
                                                                 // if neither t nor at are in added_tiles, the
                                                                 // constraint exists already
                                                                 if (is_added_tile(t) || is_added_tile(at))
                                                                 {
                                                                     solver->add(
                                                                         z3::implies(get_tc(t, at), get_tp(t, at)));
                                                                 }
                                                             });
                    }
                    else  // irregular clocking
                    {
                        layout.foreach_adjacent_tile(t,
                                                     [this, &t](const auto& at)
                                                     {
                                                         // if neither t nor at are in added_tiles, the constraint
                                                         // exists already
                                                         if (is_added_tile(t) || is_added_tile(at))
                                                         {
                                                             solver->add(z3::implies(get_tc(t, at), get_tp(t, at)));
                                                         }
                                                     });
                    }
                });
        }
        /**
         * Adds constraints to the solver to expand the formerly created sub-paths transitively.
         */
        void establish_transitive_paths()
        {
            layout.foreach_ground_tile(
                [this](const auto& t1)
                {
                    layout.foreach_ground_tile(
                        [this, &t1](const auto& t2)
                        {
                            // skip instances where t1 == t2
                            if (t1 != t2)
                            {
                                layout.foreach_ground_tile(
                                    [this, &t1, &t2](const auto& t3)
                                    {
                                        // skip instances where t2 == t3
                                        if (t2 != t3)
                                        {
                                            // if neither t1 nor t2 nor t3 are in added_tiles, the constraint exists
                                            // already in the solver and does not need to be added
                                            if (is_added_tile(t1) || is_added_tile(t2) || is_added_tile(t3))
                                            {
                                                solver->add(
                                                    z3::implies(get_tp(t1, t2) and get_tp(t2, t3), get_tp(t1, t3)));
                                            }
                                        }
                                    });
                            }
                        });
                });
        }
        /**
         * Adds constraints to the solver to prohibit cycles that loop back information. To this end, the formerly
         * established paths are used. Without this constraint, useless wire loops appear.
         */
        void eliminate_cycles()
        {
            for (const auto& t : check_point->added_tiles) { solver->add(not get_tp(t, t)); }
        }
        /**
         * Adds constraints to the solver to ensure that the cl variable of primary input pi is set to the clock zone
         * value of the tile pi is assigned to. Necessary to be taken into account for path lengths.
         */
        void assign_pi_clockings()
        {
            auto assign = [this](const auto v) -> void
            {
                auto cl = get_vcl(v);
                for (const auto& t : check_point->added_tiles)
                {
                    if (layout.is_regularly_clocked())
                    {
                        solver->add(z3::implies(get_tv(t, v),
                                                cl == ctx->int_val(static_cast<unsigned>(layout.get_clock_number(t)))));
                    }
                    else  // irregular clocking
                    {
                        solver->add(z3::implies(get_tv(t, v), cl == get_tcl(t)));
                    }
                }
            };

            if (!((layout.is_clocking_scheme(clock_name::twoddwave) ||
                   layout.is_clocking_scheme(clock_name::twoddwave_hex)) &&
                  config.border_io))
            {
                if (config.io_ports)
                {
                    network.foreach_pi(assign);
                }
                else
                {
                    network.foreach_pi(
                        [this, &assign](const auto& pi)
                        {
                            network.foreach_fanout(pi,
                                                   [this, &assign](const auto& fo)
                                                   {
                                                       if (const auto fn = network.get_node(fo);
                                                           !skip_const_or_io_node(fo))
                                                       {
                                                           assign(fn);
                                                       }
                                                   });
                        });
                }
            }
        }
        /**
         * Adds constraints to the solver to ensure that fan-in paths to the same tile need to have the same length
         * in the layout modulo timing, i.e. plus the clock zone assigned to their PIs.
         */
        void global_synchronization();
        /**
         * Adds constraints to the solver to prevent edges or vertices to be assigned to tiles with an insufficient
         * number of predecessors/successors. Symmetry breaking constraints.
         */
        void prevent_insufficiencies()
        {
            // TODO was foreach_tile (why would that be correct?)
            layout.foreach_ground_tile(
                [this](const auto& t)
                {
                    if (layout.is_eastern_border(t) || layout.is_southern_border(t) || is_updated_tile(t))
                    {
                        if (layout.is_regularly_clocked())
                        {
                            network.foreach_node(
                                [this, &t](const auto& v)
                                {
                                    if (!skip_const_or_io_node(v))
                                    {
                                        // if vertex v has more adjacent or inversely adjacent elements than tile t
                                        if (layout.out_degree(t) < network_out_degree(v) ||
                                            layout.in_degree(t) < network_in_degree(v))
                                        {
                                            // if t is at eastern/southern border, its adjacencies might change
                                            if (layout.is_eastern_border(t) || layout.is_southern_border(t))
                                            {
                                                // add restriction as assumption only
                                                check_point->assumptions.push_back(not get_tv(t, v));
                                            }
                                            else if (is_updated_tile(
                                                         t))  // nothing's about to change here // TODO müsste es nicht
                                                              // reichen, hier else zu verwenden?
                                            {
                                                // add hard constraint
                                                solver->add(not get_tv(t, v));
                                            }
                                        }
                                    }
                                });

                            foreach_edge(network,
                                         [this, &t](const auto& e)
                                         {
                                             if (!skip_const_or_io_edge(e))
                                             {
                                                 // if tile t has no adjacent or inversely adjacent tiles
                                                 if (layout.out_degree(t) == 0 || layout.in_degree(t) == 0)
                                                 {
                                                     // if t is at eastern/southern border, its adjacencies might change
                                                     if (layout.is_eastern_border(t) || layout.is_southern_border(t))
                                                     {
                                                         // add restriction as assumption only
                                                         check_point->assumptions.push_back(not get_te(t, e));
                                                     }
                                                     else if (is_updated_tile(t))  // nothing's about to change here
                                                     {
                                                         // add hard constraint
                                                         solver->add(not get_te(t, e));
                                                     }
                                                 }
                                             }
                                         });
                        }
                        else  // irregular clocking
                        {
                            // TODO make a function for this
                            auto tile_degree = layout.template adjacent_tiles<std::set<tile<Lyt>>>(t).size();

                            network.foreach_node(
                                [this, &t, &tile_degree](const auto& v)
                                {
                                    if (!skip_const_or_io_node(v))
                                    {
                                        // in an irregular clocking scheme, not so strict restrictions can be made
                                        if (tile_degree < network_out_degree(v) + network_in_degree(v))
                                        {
                                            // if t is at eastern/southern border, its adjacencies might change
                                            if (layout.is_eastern_border(t) || layout.is_southern_border(t))
                                            {
                                                // add restriction as assumption only
                                                check_point->assumptions.push_back(not get_tv(t, v));
                                            }
                                            else  // nothing's about to change here
                                            {
                                                // add hard constraint
                                                solver->add(not get_tv(t, v));
                                            }
                                        }
                                    }
                                });
                            // cannot restrict wires because each tile has degree >= 2 in an irregular clocking
                        }
                    }
                });
        }
        /**
         * Adds constraints to the solver to define the number of connection variables to be set for each tile, i.e.
         * empty tiles are not allowed to have connections at all, edges need to have one ingoing and one outgoing
         * connection and so on. Symmetry breaking constraints.
         */
        void define_number_of_connections()
        {
            apply_to_added_and_updated_tiles(
                [this](const auto& t)
                {
                    z3::expr_vector tcc{*ctx};

                    // collect (inverse) connection variables
                    z3::expr_vector acc{*ctx};
                    z3::expr_vector iacc{*ctx};
                    if (layout.is_regularly_clocked())
                    {
                        layout.foreach_outgoing_clocked_zone(t,
                                                             [this, &t, &tcc, &acc](const auto& at)
                                                             {
                                                                 auto tc = get_tc(t, at);
                                                                 acc.push_back(tc);
                                                                 tcc.push_back(tc);
                                                             });

                        layout.foreach_incoming_clocked_zone(t,
                                                             [this, &t, &tcc, &iacc](const auto& iat)
                                                             {
                                                                 auto itc = get_tc(iat, t);
                                                                 iacc.push_back(itc);
                                                                 tcc.push_back(itc);
                                                             });
                    }
                    else  // irregular clocking
                    {
                        layout.foreach_adjacent_tile(t,
                                                     [this, &t, &tcc, &acc, &iacc](const auto& at)
                                                     {
                                                         auto tc = get_tc(t, at);
                                                         acc.push_back(tc);
                                                         tcc.push_back(tc);

                                                         auto itc = get_tc(at, t);
                                                         iacc.push_back(itc);
                                                         tcc.push_back(itc);
                                                     });
                    }

                    z3::expr_vector ow{*ctx};

                    network.foreach_node(
                        [this, &t, &acc, &iacc, &ow](const auto& v)
                        {
                            if (!skip_const_or_io_node(v))
                            {
                                auto tv   = get_tv(t, v);
                                auto aon  = network_out_degree(v);
                                auto iaon = network_in_degree(v);

                                ow.push_back(tv);

                                // if vertex v is assigned to a tile, the number of connections need to correspond
                                if (!acc.empty())
                                {
                                    solver->add(mk_as_if_se(
                                        z3::implies(tv, z3::atleast(acc, aon) and z3::atmost(acc, aon)), t));
                                }
                                if (!iacc.empty())
                                {
                                    solver->add(mk_as_if_se(
                                        z3::implies(tv, z3::atleast(iacc, iaon) and z3::atmost(iacc, iaon)), t));
                                }
                            }
                        });

                    z3::expr_vector wv{*ctx};
                    foreach_edge(network,
                                 [this, &t, &ow, &wv](const auto& e)
                                 {
                                     if (!skip_const_or_io_edge(e))
                                     {
                                         auto te = get_te(t, e);
                                         ow.push_back(te);
                                         wv.push_back(te);
                                     }
                                 });

                    // if there is any edge assigned to a tile, the number of connections need to correspond
                    if (!wv.empty())
                    {
                        if (!acc.empty())
                        {
                            solver->add(mk_as_if_se(z3::implies(z3::atleast(wv, 1u) and z3::atmost(wv, 1u),
                                                                z3::atleast(acc, 1u) and z3::atmost(acc, 1u)),
                                                    t));
                        }
                        if (!iacc.empty())
                        {
                            solver->add(mk_as_if_se(z3::implies(z3::atleast(wv, 1u) and z3::atmost(wv, 1u),
                                                                z3::atleast(iacc, 1u) and z3::atmost(iacc, 1u)),
                                                    t));
                        }

                        // if crossings are allowed, there must be exactly four connections (one in each direction) for
                        // two assigned edges
                        if (config.crossings)
                        {
                            // don't assign two edges to a tile that lacks connectivity
                            if (acc.size() < 2 || iacc.size() < 2)
                            {
                                solver->add(mk_as_if_se(z3::atmost(wv, 1u), t));
                            }
                            else if (wv.size() >= 2)  // it is pointless to check an expression vector of size < 2 for
                                                      // at least 2 set variables
                            {
                                if (!acc.empty())
                                {
                                    solver->add(mk_as_if_se(z3::implies(z3::atleast(wv, 2u) and z3::atmost(wv, 2u),
                                                                        z3::atleast(acc, 2u) and z3::atmost(acc, 2u)),
                                                            t));
                                }
                                if (!iacc.empty())
                                {
                                    solver->add(mk_as_if_se(z3::implies(z3::atleast(wv, 2u) and z3::atmost(wv, 2u),
                                                                        z3::atleast(iacc, 2u) and z3::atmost(iacc, 2u)),
                                                            t));
                                }
                            }
                        }
                    }

                    // if tile t is empty, there must not be any connection from or to tile t established
                    if (ow.size() > 1 &&
                        !tcc.empty())  // test for > 1 to exclude single-vertex networks from this constraint
                    {
                        solver->add(mk_as_if_se(z3::atmost(ow, 0u) == z3::atmost(tcc, 0u), t));
                    }
                });  // function call to added and updated tiles done

            // lacking connections for irregular clocking
            if (!layout.is_regularly_clocked())
            {
                z3::expr_vector ccp{*ctx};

                // iterate over added_tiles only here to not duplicate constraints
                for (const auto& t : check_point->added_tiles)
                {
                    z3::expr_vector ow{*ctx};
                    network.foreach_node(
                        [this, &t, &ow](const auto& v)
                        {
                            if (!skip_const_or_io_node(v))
                            {
                                ow.push_back(get_tv(t, v));
                            }
                        });
                    foreach_edge(network,
                                 [this, &t, &ow](const auto& e)
                                 {
                                     if (!skip_const_or_io_edge(e))
                                     {
                                         ow.push_back(get_te(t, e));
                                     }
                                 });

                    // if tile t is empty, the clock number does not matter and can be fixed to 0
                    if (!ow.empty())
                    {
                        solver->add(z3::implies(z3::atmost(ow, 0u), get_tcl(t) == ctx->int_val(0)));
                    }
                }
            }
        }
        /**
         * Adds constraints to the solver to prohibit certain vertex placements based on the network hierarchy if the
         * clocking scheme is feed-back-free. Symmetry breaking constraints.
         */
        void utilize_hierarchical_information();
        /**
         * Adds constraints to the solver to position the primary inputs and primary outputs at the layout's borders.
         */
        void enforce_border_io()
        {
            auto assign_border = [this](const auto& v)
            {
                apply_to_added_and_updated_tiles(
                    [this, &v](const auto& t)
                    {
                        if (!layout.is_border(t))
                        {
                            solver->add(not get_tv(t, v));
                        }
                    });
            };

            auto assign_west = [this](const auto& v)
            {
                // no need to iterate over updated_tiles, because nothing changes there
                for (const auto& t : check_point->added_tiles)
                {
                    if (!layout.is_western_border(t))
                    {
                        solver->add(not get_tv(t, v));
                    }
                }
            };

            auto assign_east = [this](const auto& v)
            {
                apply_to_added_and_updated_tiles(
                    [this, &v](const auto& t)
                    {
                        if (!layout.is_eastern_border(t))
                        {
                            solver->add(not get_tv(t, v));
                        }
                    });
            };

            if (config.io_ports)
            {
                network.foreach_pi(
                    [this, &assign_west, &assign_border](const auto& pi)
                    { layout.is_clocking_scheme(clock_name::columnar) ? assign_west(pi) : assign_border(pi); });
                network.foreach_po(
                    [this, &assign_east, &assign_border](const auto& po)
                    {
                        layout.is_clocking_scheme(clock_name::columnar) ? assign_east(network.get_node(po)) :
                                                                          assign_border(network.get_node(po));
                    });
            }
            else
            {
                network.foreach_pi(
                    [this, &assign_west, &assign_border](const auto& pi)
                    {
                        network.foreach_fanout(pi,
                                               [this, &assign_west, &assign_border](const auto& fo)
                                               {
                                                   if (const auto v = network.get_node(fo); !skip_const_or_io_node(v))
                                                   {
                                                       layout.is_clocking_scheme(clock_name::columnar) ?
                                                           assign_west(v) :
                                                           assign_border(v);
                                                   }
                                               });
                    });

                network.foreach_po(
                    [this, &assign_east, &assign_border](const auto& po)
                    {
                        network.foreach_fanin(po,
                                              [this, &assign_east, &assign_border](const auto& fi)
                                              {
                                                  if (const auto v = network.get_node(fi); !skip_const_or_io_node(v))
                                                  {
                                                      layout.is_clocking_scheme(clock_name::columnar) ?
                                                          assign_east(v) :
                                                          assign_border(v);
                                                  }
                                              });
                    });
            }
        }
        /**
         * Adds constraints to the solver to enforce that no bent inverters are used.
         */
        void enforce_straight_inverters();
        /**
         * Adds constraints to the solver to prevent negative valued clock latches and that vertex tiles cannot be
         * latches.
         */
        void restrict_synchronization_elements()
        {
            auto zero = ctx->int_val(0u);

            for (const auto& t : check_point->added_tiles)
            {
                // latches must be positive
                auto l = get_tl(t);
                solver->add(l >= zero);

                // tiles without wires cannot have latches
                z3::expr_vector te{*ctx};

                foreach_edge(network,
                             [this, &t, &te](const auto& e)
                             {
                                 if (!skip_const_or_io_edge(e))
                                 {
                                     te.push_back(get_te(t, e));
                                 }
                             });

                solver->add(z3::implies(z3::atmost(te, 0u), l == zero));
            }
        }
        /**
         * Adds constraints to the solver to enforce topology-specific restrictions.
         */
        void topology_specific_constraints();
        /**
         * Adds constraints to the given optimize to minimize the number of crossing tiles to use.
         *
         * @param optimize Pointer to an z3::optimize to add constraints to.
         */
        void minimize_wires(optimize_ptr optimize)
        {
            z3::expr_vector wire_counter{*ctx};
            layout.foreach_ground_tile(
                [this, &wire_counter](const auto& t)
                {
                    foreach_edge(network,
                                 [this, &wire_counter, &t](const auto& e)
                                 {
                                     if (!skip_const_or_io_edge(e))
                                     {
                                         wire_counter.push_back(
                                             z3::ite(get_te(t, e), ctx->real_val(1u), ctx->real_val(0u)));
                                     }
                                 });
                });

            optimize->minimize(z3::sum(wire_counter));
        }
        /**
         * Adds constraints to the given optimize to minimize the number of crossing tiles to use.
         *
         * @param optimize Pointer to an z3::optimize to add constraints to.
         */
        void minimize_crossings(optimize_ptr optimize)
        {
            z3::expr_vector crossings_counter{*ctx};
            layout.foreach_ground_tile(
                [this, &crossings_counter](const auto& t)
                {
                    z3::expr_vector wv{*ctx};
                    foreach_edge(network,
                                 [this, &t, &wv](const auto& e)
                                 {
                                     if (!skip_const_or_io_edge(e))
                                     {
                                         wv.push_back(get_te(t, e));
                                     }
                                 });

                    crossings_counter.push_back(z3::ite(z3::atleast(wv, 2u), ctx->real_val(1u), ctx->real_val(0u)));
                });

            optimize->minimize(z3::sum(crossings_counter));
        }
        /**
         * Adds constraints to the given optimize to enforce that the overall sum of latch values should be minimized.
         *
         * @param optimize Pointer to an z3::optimize to add constraints to.
         */
        void minimize_synchronization_elements(optimize_ptr optimize)
        {
            z3::expr_vector latch_counter{*ctx};
            layout.foreach_ground_tile([this, &latch_counter](const auto& t) { latch_counter.push_back(get_tl(t)); });

            optimize->minimize(z3::sum(latch_counter));
        }
        /**
         * Generates the SMT instance by calling the constraint generating functions.
         */
        void generate_smt_instance()
        {
            // placement constraints
            restrict_tile_elements();
            restrict_vertices();

            // local synchronization constraints
            define_adjacent_vertex_tiles();
            define_inv_adjacent_vertex_tiles();
            define_adjacent_edge_tiles();
            define_inv_adjacent_edge_tiles();

            // global synchronization constraints
            if (!config.desynchronize && !(layout.is_clocking_scheme(clock_name::twoddwave) ||
                                           layout.is_clocking_scheme(clock_name::twoddwave_hex)))
            {
                assign_pi_clockings();
                //                global_synchronization();
            }

            // open clocking scheme constraints
            if (!layout.is_regularly_clocked())
            {
                restrict_clocks();
            }

            // path/cycle constraints
            if (!(layout.is_clocking_scheme(clock_name::columnar) || layout.is_clocking_scheme(clock_name::twoddwave) ||
                  layout.is_clocking_scheme(clock_name::twoddwave_hex)))  // linear schemes; no cycles by definition
            {
                establish_sub_paths();
                establish_transitive_paths();
                eliminate_cycles();
            }

            // I/O pin constraints
            if (config.border_io)
            {
                enforce_border_io();
            }

            // straight inverter constraints
            //            if (config.straight_inverters)
            //            {
            //                enforce_straight_inverters();
            //            }

            // clock latch constraints
            //            if (config.synchronization_elements && !config.desynchronize)
            //            {
            //                restrict_synchronization_elements();
            //            }

            // topology-specific constraints
            //            topology_specific_constraints();

            // symmetry breaking constraints
            prevent_insufficiencies();
            define_number_of_connections();
            //            utilize_hierarchical_information();
        }
        /**
         * Creates and returns a z3::optimize if optimization criteria were set by the configuration. The optimize gets
         * passed all constraints from the current solver and the respective optimization constraints are added to it,
         * too.
         */
        optimize_ptr optimize()
        {
            if (auto wires = config.minimize_wires, cross = config.minimize_crossings,
                latch = config.synchronization_elements && !config.desynchronize;
                !wires && !cross && !latch)
            {
                return nullptr;
            }
            else
            {
                auto optimize = std::make_shared<z3::optimize>(*ctx);

                // add all solver constraints
                for (const auto& e : solver->assertions()) { optimize->add(e); }

                // add assumptions as constraints, too, because optimize::check with assumptions is broken
                for (const auto& e : check_point->assumptions) { optimize->add(e); }

                // wire minimization constraints
                if (wires)
                {
                    minimize_wires(optimize);
                }

                // crossing minimization constraints
                if (cross)
                {
                    minimize_crossings(optimize);
                }

                // clock latches minimization constraints
                if (latch)
                {
                    minimize_synchronization_elements(optimize);
                }

                return optimize;
            }
        }

        void place_output(const tile<Lyt>& t, const mockturtle::node<topology_ntk_t>& n)
        {
            const auto output_signal = network.make_signal(fanins(network, n).fanin_nodes[0]);

            layout.create_po(node2pos[output_signal][n], "", t);
        }

        void assign_layout_clocking(const z3::model& model)
        {
            // assign clock zones to tiles of open schemes
            if (!layout.is_regularly_clocked())
            {
                layout.foreach_ground_tile(
                    [this, &model](const auto& t)
                    {
                        layout.assign_clock_number(t, static_cast<typename Lyt::clock_number_t>(
                                                          model.eval(get_tcl(t), true).get_numeral_int()));
                    });
            }
        }
        /**
         * Starting from t, all outgoing clocked tiles are recursively considered and checked against the given model.
         * Consequently, e is routed through all tiles with a match in model.
         *
         * @param t Initial tile to start recursion from (not included in model evaluations).
         * @param e Edge to check for.
         */
        void route(const tile<Lyt>& t, const mockturtle::edge<topology_ntk_t>& e, const z3::model& model)
        {
            //            std::cout << fmt::format("Routing ({},{}) starting on {}", e.source, e.target, t) <<
            //            std::endl;

            layout.foreach_outgoing_clocked_zone(
                t,
                [this, &t, &e, &model](const auto& at)
                {
                    // if e got assigned to at according to the model together with a
                    // set connection variable between t and at
                    if (model.eval(get_te(at, e)).bool_value() == Z3_L_TRUE &&
                        model.eval(get_tc(t, at)).bool_value() == Z3_L_TRUE)
                    {
                        //                        std::cout << fmt::format("assigning ({},{}) to {} with incoming signal
                        //                        {}", e.source, e.target,
                        //                                                 at,
                        //                                                 static_cast<tile<Lyt>>(node2pos[e.source][e.target]))
                        //                                  << std::endl;

                        // assign wire segment to at and save its position as the
                        // signal lookup for e's source node
                        node2pos[e.source].update_branch(
                            e.target, layout.create_buf(node2pos[e.source][e.target],
                                                        layout.is_empty_tile(at) ? at : layout.above(at)));

                        // recursion call
                        route(at, e, model);

                        // quit loop since the wire should not split
                        return false;
                    }

                    // no wire path was found yet; continue looping
                    return true;
                });
        }
        /**
         * Assigns vertices, edges and directions to the stored layout sketch with respect to the given model.
         *
         * @param model A satisfying assignment to the created variables under all created constraints that can be
         *              used to extract a layout description.
         */
        void assign_layout(const z3::model& model)
        {
            assign_layout_clocking(model);
            // from now on, a clocking scheme is assigned and no distinction between regular and irregular must be made

            //            std::ofstream assertions_file{"assertions.txt"};
            //
            //            assertions_file << layout.x() << " x " << layout.y() << std::endl;
            //            assertions_file << solver->assertions() << std::endl;
            //
            //            assertions_file.close();
            //
            //            std::ofstream model_file{"model.txt"};
            //
            //            model_file << model << std::endl;
            //
            //            model_file.close();

            const auto pis = reserve_input_nodes(layout, network);

            // network is topologically sorted, therefore, foreach_node ensures conflict-free traversal
            network.foreach_node(
                [this, &model, &pis](const auto& n)
                {
                    if (!skip_const_or_io_node(n))
                    {
                        // find the tile where n is placed
                        layout.foreach_ground_tile(
                            [this, &model, &pis, &n](const auto& t)
                            {
                                // was node n placed on tile t according to the model?
                                if (model.eval(get_tv(t, n)).bool_value() == Z3_L_TRUE)
                                {
                                    mockturtle::signal<Lyt> lyt_signal;

                                    if (network.is_pi(n))
                                    {
                                        lyt_signal = layout.move_node(pis[n], t);
                                    }
                                    else if (network.is_po(n))
                                    {
                                        // skip POs since they are created in a second loop to preserve their order
                                        return false;
                                    }
                                    else
                                    {
                                        // assign n to t in layout and save the resulting signal
                                        lyt_signal = place(layout, t, network, n, node2pos);
                                    }

                                    // check n's outgoing edges
                                    network.foreach_fanout(n,
                                                           [this, &model, &n, &t, &lyt_signal](const auto& fo)
                                                           {
                                                               if (const auto fn = network.get_node(fo);
                                                                   !skip_const_or_io_node(fn))
                                                               {
                                                                   // store the signal as branch towards fn
                                                                   node2pos[n].update_branch(fn, lyt_signal);

                                                                   mockturtle::edge<topology_ntk_t> e{n, fn};

                                                                   // check t's outgoing clocked tiles since those
                                                                   // are the only ones where e could potentially
                                                                   // have been placed
                                                                   route(t, e, model);
                                                               }
                                                           });

                                    // node placed; stop looping
                                    return false;
                                }

                                // node not placed yet; keep looping
                                return true;
                            });
                    }
                });

            // place outputs
            network.foreach_po(
                [this, &model](const auto& po)
                {
                    if (const auto pon = network.get_node(po); !skip_const_or_io_node(pon))
                    {  // find the tile where n is placed
                        layout.foreach_ground_tile(
                            [this, &model, &pon](const auto& t)
                            {
                                // was node n placed on tile t according to the model?
                                if (model.eval(get_tv(t, pon)).bool_value() == Z3_L_TRUE)
                                {
                                    place_output(t, pon);
                                }
                            });
                    }
                });

            //            debug::write_dot_network(network);

            // adjust wires for ToPoliNano clocking as multi wires are supported
            //            if (config.topolinano)
            //            {
            //                for (auto&& t : layout->crossing_layers())
            //                {
            //                    for (auto& e : layout->get_logic_edges(t))
            //                    {
            //                        if (auto inp = layout->get_wire_inp_dirs(t, e), out = layout->get_wire_out_dirs(t,
            //                        e);
            //                            inp != layout::opposite(out))
            //                        {
            //                            // move edge to ground layer
            //                            auto g = layout_tile{t[X], t[Y], GROUND};
            //                            layout->dissociate_logic_edge(t, e);
            //                            layout->assign_logic_edge(g, e);
            //                            layout->assign_wire_inp_dir(g, e, inp);
            //                            layout->assign_wire_out_dir(g, e, out);
            //                        }
            //                    }
            //                }
            //            }

            // assign artificial latches if there were any in use
            if (config.synchronization_elements)
            {
                if constexpr (has_synchronization_elements_v<Lyt>)
                {
                    layout.foreach_ground_layer(
                        [this, &model](const auto& t)
                        {
                            layout.assign_synchronization_element(
                                t,
                                static_cast<typename Lyt::sync_elem_t>(model.eval(get_tl(t), true).get_numeral_int()));
                        });
                }
            }

            // restore possibly set signal names
            restore_names(network, layout, node2pos);
        }
    };

    /**
     * Calculates the time left for solving by subtracting the time passed from the configured timeout and updates
     * Z3's timeout accordingly.
     *
     * @param handler Handler whose timeout is to be updated.
     * @param time Time passed since beginning of the solving process.
     */
    void update_timeout(smt_handler& handler, mockturtle::stopwatch<>::duration time) const
    {
        auto time_elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(time).count();
        auto time_left    = (ps.timeout - time_elapsed > 0 ? static_cast<unsigned>(ps.timeout - time_elapsed) : 0u);

        if (!time_left)
        {
            throw z3::exception("timeout");
        }

        handler.set_timeout(time_left);
    }
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
    std::optional<Lyt> explore_asynchronously(const unsigned t_num, std::shared_ptr<std::vector<thread_info>> ti_list)
    {
        auto ctx = std::make_shared<z3::context>();

        Lyt layout{{}, *ps.scheme};

        smt_handler handler{ctx, layout, *ntk, ps};
        (*ti_list)[t_num].ctx = ctx;

        while (true)
        {
            aspect_ratio<Lyt> dimension;

            // mutually exclusive access to the dimension iterator
            {
                std::lock_guard<std::mutex> guard(dit_mutex);

                ++dit;
                dimension = *dit;  // operations ++ and * are split to prevent a vector copy construction
            }

            if (area(dimension) > ps.upper_bound)
            {
                return std::nullopt;
            }

            if (handler.skippable(dimension))
            {
                continue;
            }

            // mutually exclusive access to the result dimension
            {
                std::lock_guard<std::mutex> guard(rd_mutex);

                // a result is available already
                if (result_dimension)
                {
                    // stop working if its area is smaller or equal to the one currently at hand
                    if (area(*result_dimension) <= area(dimension))
                    {
                        return std::nullopt;
                    }
                }
            }

            // update dimension in the thread_info list and the handler
            (*ti_list)[t_num].worker_dimension = dimension;
            handler.update(dimension);

            try
            {
                mockturtle::stopwatch stop{pst.time_total};

                if (handler.is_satisfiable())  // found a layout
                {
                    // mutually exclusive access to the result_dimension
                    {
                        std::lock_guard<std::mutex> guard(rd_mutex);

                        // update the result_dimension if there is none
                        if (!result_dimension)
                        {
                            result_dimension = dimension;
                        }
                        else  // or if the own one is smaller
                        {
                            if (area(*result_dimension) > area(dimension))
                            {
                                result_dimension = dimension;
                            }
                            else
                            {
                                return std::nullopt;
                            }
                        }
                    }

                    // interrupt other threads that are working on higher dimensions
                    for (const auto& ti : *ti_list)
                    {
                        if (area(dimension) <= area(ti.worker_dimension))
                        {
                            ti.ctx->interrupt();
                        }
                    }

                    return layout;
                }
                else  // no layout with this dimension possible
                {
                    // One could assume that interrupting other threads that are working on real smaller (not bigger in
                    // any dimension) layouts could be beneficial here. However, testing revealed that this code was
                    // hardly ever triggered and if it was, it impacted performance negatively because no solver state
                    // could be stored that could positively influence performance of later SMT calls

                    handler.store_solver_state(dimension);
                }
            }
            catch (const z3::exception&)  // timed out or interrupted
            {
                return std::nullopt;
            }

            update_timeout(handler, pst.time_total);
        }

        // unreachable code, but compiler complains if it's not there
        return std::nullopt;
    }
    /**
     * Launches config.num_threads threads and evaluates their return statements.
     *
     * @return Physical design result including statistical information.
     */
    std::optional<Lyt> run_asynchronously()
    {
        Lyt layout{{}, *ps.scheme};

        {
            mockturtle::stopwatch stop{pst.time_total};

            using fut_layout = std::future<std::optional<Lyt>>;
            std::vector<fut_layout> fut(ps.num_threads);

            auto ti_list = std::make_shared<std::vector<thread_info>>(ps.num_threads);

#if (PROGRESS_BARS)
            mockturtle::progress_bar thread_bar("[i] examining layout dimensions using {} threads");
            thread_bar(ps.num_threads);

            auto post_toggle = false;

            mockturtle::progress_bar post_bar(
                "[i] some layout has been found; waiting for threads examining smaller dimensions to terminate");
#endif

            for (auto i = 0u; i < ps.num_threads; ++i)
            {
                fut[i] = std::async(std::launch::async, &exact_impl::explore_asynchronously, this, i, ti_list);
            }

            // wait for all tasks to finish running (can be made much prettier in C++20...)
            for (auto still_running = true; still_running;)
            {
                still_running = false;
                for (auto i = 0u; i < ps.num_threads; ++i)
                {
                    using namespace std::chrono_literals;
                    if (fut[i].wait_for(10ms) == std::future_status::timeout)
                    {
                        still_running = true;
                    }
#if (PROGRESS_BARS)
                    else if (!post_toggle)
                    {
                        thread_bar.done();
                        post_bar(true);
                        post_toggle = true;
                    }
#endif
                }
            }

            if (result_dimension)
            {
                auto result_dim_val = *result_dimension;
                // extract the layout from the futures
                for (auto& f : fut)
                {
                    if (auto l = f.get(); l.has_value())
                    {
                        // in case multiple returned, get the actual winner
                        if ((*l).x() == result_dim_val.x && (*l).y() == result_dim_val.y)
                        {
                            layout = *l;
                        }
                    }
                }
            }
        }

        // TODO verify that this makes sense
        if (result_dimension.has_value())
        {
            // statistical information
            pst.x_size    = layout.x() + 1;
            pst.y_size    = layout.y() + 1;
            pst.num_gates = layout.num_gates();
            pst.num_wires = layout.num_wires();

            // TODO more statistics?
            return layout;
        }
        else
        {
            return std::nullopt;
        }
    }
    /**
     * Does the same as explore_asynchronously but without thread synchronization overhead.
     *
     * @return Physical design result including statistical information.
     */
    std::optional<Lyt> run_synchronously() noexcept
    {
        Lyt layout{{}, *ps.scheme};

        smt_handler handler{std::make_shared<z3::context>(), layout, *ntk, ps};

        for (; dit <= ps.upper_bound; ++dit)  // <= to prevent overflow
        {

#if (PROGRESS_BARS)
            mockturtle::progress_bar bar("[i] examining layout dimensions: {:>2} × {:<2}");
#endif

            auto dimension = *dit;

            if (handler.skippable(dimension))
                continue;

#if (PROGRESS_BARS)
            bar(dimension.x + 1, dimension.y + 1);
#endif

            handler.update(dimension);

            try
            {
                auto sat =
                    mockturtle::call_with_stopwatch(pst.time_total, [&handler] { return handler.is_satisfiable(); });

                if (sat)
                {
                    // statistical information
                    pst.x_size    = layout.x() + 1;
                    pst.y_size    = layout.y() + 1;
                    pst.num_gates = layout.num_gates();
                    pst.num_wires = layout.num_wires();

                    // TODO more statistics?

                    return layout;
                }
                else
                {
                    handler.store_solver_state(dimension);
                }

                update_timeout(handler, pst.time_total);
            }
            catch (const z3::exception&)
            {
                // TODO timeout exception?
                return std::nullopt;
            }
        }

        return std::nullopt;
    }
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

    // check for input degree
    if (has_high_degree_fanin_nodes(ntk, ps.scheme->max_in_degree))
    {
        throw high_degree_fanin_exception();
    }

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
