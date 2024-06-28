//
// Created by marcel on 06.01.20.
//

#ifndef FICTION_EXACT_HPP
#define FICTION_EXACT_HPP

#if (FICTION_Z3_SOLVER)

#include "fiction/algorithms/iter/aspect_ratio_iterator.hpp"
#include "fiction/algorithms/network_transformation/fanout_substitution.hpp"
#include "fiction/io/print_layout.hpp"
#include "fiction/layouts/clocking_scheme.hpp"
#include "fiction/technology/cell_ports.hpp"
#include "fiction/technology/sidb_surface_analysis.hpp"
#include "fiction/traits.hpp"
#include "fiction/utils/layout_utils.hpp"
#include "fiction/utils/name_utils.hpp"
#include "fiction/utils/network_utils.hpp"
#include "fiction/utils/placement_utils.hpp"
#include "fiction/utils/truth_table_utils.hpp"

#include <fmt/format.h>
#include <kitty/dynamic_truth_table.hpp>
#include <kitty/operations.hpp>
#include <mockturtle/traits.hpp>
#include <mockturtle/utils/node_map.hpp>
#include <mockturtle/utils/stopwatch.hpp>
#include <mockturtle/views/color_view.hpp>
#include <mockturtle/views/depth_view.hpp>
#include <mockturtle/views/fanout_view.hpp>
#include <mockturtle/views/topo_view.hpp>
#if (PROGRESS_BARS)
#include <mockturtle/utils/progress_bar.hpp>
#endif

#include <z3++.h>

#include <algorithm>
#include <atomic>
#include <cassert>
#include <chrono>
#include <cstdint>
#include <functional>
#include <future>
#include <memory>
#include <mutex>
#include <optional>
#include <string>
#include <thread>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace fiction
{

/**
 * Target technologies.
 */
enum class technology_constraints : uint8_t
{
    /**
     * No technology-specific constraints.
     */
    NONE = 0,
    /**
     * ToPoLiNano technology-specific constraints.
     */
    TOPOLINANO
};
/**
 * Parameters for the exact physical design algorithm.
 */
struct exact_physical_design_params
{
    /**
     * Clocking scheme to be used.
     */
    std::string scheme = "2DDWave";
    /**
     * Number of total tiles to use as an upper bound.
     *
     * @note If `upper_bound_area` and (either) `upper_bound_x` or `upper_bound_y` are set, the imposed search space
     * restrictions are cumulative. E.g., if `upper_bound_area == 20` and `upper_bound_x == 4`, all aspect ratios with
     * an x-dimension of more than 4 *and* a total area of more than 20 will be skipped.
     */
    uint16_t upper_bound_area = std::numeric_limits<uint16_t>::max();
    /**
     * Number of tiles to use as an upper bound in x direction.
     */
    uint16_t upper_bound_x = std::numeric_limits<uint16_t>::max();
    /**
     * Number of tiles to use as an upper bound in y direction.
     */
    uint16_t upper_bound_y = std::numeric_limits<uint16_t>::max();
    /**
     * Exclusively investigate aspect ratios that conform with the restrictions imposed by the upper bound options.
     * E.g., if `fixed_size == true` *and* `upper_bound_area == 20`, only aspect ratios with exactly 20 tiles will be
     * examined. Restricted imposed by the `upper_bound_x` and `upper_bound_y` flags additionally apply.
     */
    bool fixed_size = false;
    /**
     * Number of threads to use for exploring the possible aspect ratios.
     *
     * @note This is an unstable beta feature.
     */
    std::size_t num_threads = 1ul;
    /**
     * Flag to indicate that crossings may be used.
     */
    bool crossings = false;
    /**
     * Flag to indicate that I/Os should be realized by designated wire segments (preferred).
     */
    bool io_pins = true;  // TODO right now, this has to be true
    /**
     * Flag to indicate that I/Os should be placed at the layout's border.
     */
    bool border_io = false;
    /**
     * Flag to indicate that artificial clock latch delays should be used to balance paths (runtime expensive!).
     */
    bool synchronization_elements = false;
    /**
     * Flag to indicate that straight inverters should be used over bend ones.
     */
    bool straight_inverters = false;
    /**
     * Flag to indicate that a discrepancy in fan-in path lengths is allowed (reduces runtime!).
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
    /**
     * Technology-specific constraints that are only to be added for a certain target technology.
     */
    technology_constraints technology_specifics = technology_constraints::NONE;
};
/**
 * Statistics.
 */
struct exact_physical_design_stats
{
    mockturtle::stopwatch<>::duration time_total{0};

    uint64_t x_size{0ull}, y_size{0ull};
    uint64_t num_gates{0ull}, num_wires{0ull};

    uint32_t num_aspect_ratios{0ul};

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

template <typename Lyt>
class exact_impl
{
  public:
    exact_impl(mockturtle::names_view<technology_network>& src, const exact_physical_design_params& p,
               exact_physical_design_stats& st, const surface_black_list<Lyt, port_direction>& sbl = {}) :
            ps{p},
            pst{st},
            scheme{*get_clocking_scheme<Lyt>(ps.scheme)},
            black_list{sbl}
    {
        // create PO nodes in the network
        src.substitute_po_signals();

        ntk = std::make_shared<topology_ntk_t>(mockturtle::fanout_view{src});

        lower_bound = static_cast<decltype(lower_bound)>(ntk->num_gates() + ntk->num_pis());

        // NOLINTNEXTLINE(*-prefer-member-initializer)
        ari = aspect_ratio_iterator<typename Lyt::aspect_ratio>{
            ps.fixed_size ? std::min(static_cast<uint64_t>(ps.upper_bound_area),
                                     static_cast<uint64_t>(ps.upper_bound_x * ps.upper_bound_y)) :
                            static_cast<uint64_t>(lower_bound)};
    }

    std::optional<Lyt> run()
    {
        if (ps.num_threads > 1)
        {
            return run_asynchronously();
        }

        return run_synchronously();
    }

  private:
    /**
     * Network type for internal handling. Converting the input network to this type ensures the availability of all
     * necessary member functions.
     */
    using topology_ntk_t = mockturtle::topo_view<mockturtle::fanout_view<mockturtle::names_view<technology_network>>>;
    /**
     * Specification network.
     */
    std::shared_ptr<topology_ntk_t> ntk;
    /**
     * Parameters.
     */
    exact_physical_design_params ps;
    /**
     * Statistics.
     */
    exact_physical_design_stats& pst;
    /**
     * The utilized clocking scheme.
     */
    clocking_scheme<tile<Lyt>> scheme;
    /**
     * Maps tiles to blacklisted gate types via their truth tables and port information.
     */
    const surface_black_list<Lyt, port_direction> black_list;
    /**
     * Lower bound for the number of layout tiles.
     */
    uint16_t lower_bound{0u};
    /**
     * Iterator for the factorization of possible aspect ratios.
     */
    aspect_ratio_iterator<typename Lyt::aspect_ratio> ari{0};
    /**
     * Aspect ratio of found result. Only needed for the asynchronous case.
     */
    std::optional<typename Lyt::aspect_ratio> result_aspect_ratio;
    /**
     * Restricts access to the aspect_ratio_iterator and the result_aspect_ratio.
     */
    std::mutex ari_mutex{}, rar_mutex{};

    using ctx_ptr      = std::shared_ptr<z3::context>;
    using solver_ptr   = std::shared_ptr<z3::solver>;
    using optimize_ptr = std::shared_ptr<z3::optimize>;

    /**
     * Sub-class to exact to handle construction of SMT instances as well as house-keeping like storing solver
     * states across incremental calls etc. Multiple handlers can be created in order to explore possible aspect ratios
     * in parallel.
     */
    class smt_handler
    {
      public:
        /**
         * Standard constructor.
         *
         * @param ctxp The context that is used in all solvers.
         * @param lyt The empty gate-level layout that is going to contain the created layout.
         * @param ps The parameters to respect in the SMT instance generation process.
         */
        smt_handler(ctx_ptr ctxp, Lyt& lyt, const topology_ntk_t& ntk, const exact_physical_design_params& ps,
                    const surface_black_list<Lyt, port_direction>& sbl) noexcept :
                ctx{std::move(ctxp)},
                layout{lyt},
                network{ntk},
                params{ps},
                black_list{sbl},
                node2pos{ntk},
                depth_ntk{ntk},
                inv_levels{inverse_levels(ntk)}
        {}
        /**
         * Evaluates a given aspect ratio regarding the stored configurations whether it can be skipped, i.e., does not
         * need to be explored by the SMT solver. The better this function is at predicting unsatisfying inputs, the
         * more UNSAT instances can be skipped without losing the optimality guarantee. This function should never be
         * overly restrictive!
         *
         * @param ar Aspect ratio to evaluate.
         * @return `true` if ar can safely be skipped because it is UNSAT anyway.
         */
        [[nodiscard]] bool skippable(const typename Lyt::aspect_ratio& ar) const noexcept
        {
            // skip aspect ratios that extend beyond the specified upper bounds
            if ((ar.x + 1) * (ar.y + 1) > params.upper_bound_area || ar.x >= params.upper_bound_x ||
                ar.y >= params.upper_bound_y)
            {
                return true;
            }
            // OPEN clocking optimization
            if (!layout.is_regularly_clocked())
            {
                // rotated aspect ratios don't need to be explored
                if (ar.x != ar.y && ar.x == layout.y() && ar.y == layout.x())
                {
                    return true;
                }
            }
            // Columnar clocking optimization
            else if (layout.is_clocking_scheme(clock_name::COLUMNAR))
            {
                // skip all aspect ratios that are too shallow for the network's depth
                if (ar.x < depth_ntk.depth())
                {
                    return true;
                }
                // if border I/Os are enforced, skip all aspect ratios that are too narrow for hosting all I/Os
                if (params.border_io && ar.y < std::max(network.num_pis(), network.num_pos()) - 1)
                {
                    return true;
                }
            }
            // Row clocking optimization
            else if (layout.is_clocking_scheme(clock_name::ROW))
            {
                // skip all aspect ratios that are too shallow for the network's depth
                if (ar.y < depth_ntk.depth())
                {
                    return true;
                }
                // if border I/Os are enforced, skip all aspect ratios that are too narrow for hosting all I/Os
                if (params.border_io && ar.x < std::max(network.num_pis(), network.num_pos()) - 1)
                {
                    return true;
                }
            }

            return false;
        }
        /**
         * Resizes the layout and creates a new solver checkpoint from where on the next incremental instance can be
         * generated.
         *
         * @param ar Current aspect ratio to work on.
         */
        void update(const typename Lyt::aspect_ratio& ar) noexcept
        {
            layout.resize({ar.x, ar.y, params.crossings ? 1 : 0});
            check_point = std::make_shared<solver_check_point>(fetch_solver(ar));
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
         * returns then true.
         *
         * @return `true` iff the instance generated for the current configuration is SAT.
         */
        [[nodiscard]] bool is_satisfiable()
        {
            generate_smt_instance();

            if (const auto z3_result = solver->check(check_point->assumptions); z3_result == z3::sat)
            {
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

            return false;
        }
        /**
         * Stores the current solver state in the solver tree with aspect ratio ar as key.
         *
         * @param ar Key to storing the current solver state.
         */
        void store_solver_state(const typename Lyt::aspect_ratio& ar) noexcept
        {
            solver_tree[ar] = check_point->state;
        }
        /**
         * Returns a statistics object from the current solver state.
         *
         * @return Statistics.
         */
        [[nodiscard]] z3::stats get_solver_statistics() const
        {
            return solver->statistics();
        }

      private:
        /**
         * During incremental SMT calls, several created assertions need to be reformulated. To still be able to reuse
         * information like learned clauses and lemmas, the push/pop strategy is not employed. Instead, assumption
         * literals are used that track, i.e., imply, certain assertions and can be negated in order to deactivate them.
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
         * tiles have been added in contrast to the last generation and which tiles got new neighbors, i.e., have been
         * updated. Additionally, a container for assumptions, i.e., assertions that are only valid in this and only
         * this run, is needed. All of that is packaged in a solver check point.
         */
        struct solver_check_point  // NOLINT: assumptions cannot be default-initialized
        {
            /**
             * Solver and watched literals.
             */
            state_ptr state;
            /**
             * added_tiles contains only the newly added tiles in eastern or southern direction. updated_tiles instead
             * contains the column (eastern) or row (southern) of tiles that used to be at the border but is not anymore
             * now. In these tiles, certain assertions change so that their previous assertions need to be reformulated.
             */
            std::unordered_set<typename Lyt::tile> added_tiles, updated_tiles;
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
         * Configurations specifying layout restrictions. Used in instance generation among other places.
         */
        const exact_physical_design_params params;
        /**
         * Maps tiles to blacklisted gate types via their truth tables and port information.
         */
        const surface_black_list<Lyt, port_direction>& black_list;
        /**
         * Maps nodes to tile positions when creating the layout from the SMT model.
         */
        mockturtle::node_map<
            branching_signal_container<
                Lyt, topology_ntk_t, Lyt::max_fanin_size>  // this currently assumes that max_fanin_size is equal to the
                                                           // maximum fanout size of a layout type. Should the maximum
                                                           // fanout size exceed max_fanin_size, this will break
            ,
            topology_ntk_t>
            node2pos;
        /**
         * Mapping of levels to nodes used for symmetry breaking.
         */
        const mockturtle::depth_view<topology_ntk_t> depth_ntk;
        /**
         * Mapping of inverse levels to nodes used for symmetry breaking.
         */
        const std::vector<uint32_t> inv_levels;
        /**
         * Assumption literal counter.
         */
        std::size_t lc = 0ul;
        /**
         * Tree-like structure mapping already examined aspect ratios to respective solver states for later reuse.
         * Due to the rather suboptimal exploration strategy of factorizing tile counts, multiple solver states need to
         * be kept. It would always be easiest to simply add an entire row or column to the layout but that way, several
         * tile counts are skipped. E.g. by exploring 4 x 5 after 4 x 4, one would go directly from 16 tiles to 20
         * tiles. If the optimal layout can be found at 18 tiles, it would be missed. Therefore, the exploration
         * strategy using factorizations is kept and several solvers are employed that can be reused at a later point.
         * In the example, the 4 x 4 solver would be stored and revisited when 4 x 5 is to be explored.
         */
        std::unordered_map<typename Lyt::aspect_ratio, state_ptr> solver_tree{};
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
        [[nodiscard]] z3::expr get_lit_e()
        {
            return ctx->bool_const(fmt::format("lit_e_{}", lc).c_str());
        }
        /**
         * Returns the lc-th southern assumption literal from the stored context.
         *
         * @return Southern literal.
         */
        [[nodiscard]] z3::expr get_lit_s()
        {
            return ctx->bool_const(fmt::format("lit_s_{}", lc).c_str());
        }
        /**
         * Accesses the solver tree and looks for a solver state that is associated with an aspect ratio smaller by 1
         * row or column than given aspect ratio. The found one is returned together with the tiles that are new to this
         * solver.
         *
         * If no such solver could be found, a new solver is created from the context given.
         *
         * @param ar aspect ratio of size x * y.
         * @return Solver state associated with an aspect ratio of size x - 1 * y or x * y - 1 and, additionally, the
         * tiles new to the solver. If no such solver is available, a new one is created.
         */
        [[nodiscard]] solver_check_point fetch_solver(const typename Lyt::aspect_ratio& ar)
        {
            const auto create_assumptions = [this](const solver_state& state) -> z3::expr_vector
            {
                z3::expr_vector assumptions{*ctx};
                assumptions.push_back(state.lit.s);
                assumptions.push_back(state.lit.e);

                return assumptions;
            };

            // does a solver state for a layout of aspect ratio of size x - 1 * y exist?
            if (const auto it_x = solver_tree.find({ar.x - 1, ar.y}); it_x != solver_tree.end())
            {
                // gather additional y-tiles and updated tiles
                std::unordered_set<typename Lyt::tile> added_tiles{}, updated_tiles{};
                for (decltype(ar.y) y = 0; y <= ar.y; ++y)
                {
                    added_tiles.emplace(ar.x, y);
                    updated_tiles.emplace(ar.x - 1, y);
                }

                // deep-copy solver state
                const auto   state     = it_x->second;
                solver_state new_state = {state->solver, {get_lit_e(), state->lit.s}};

                // reset eastern constraints
                new_state.solver->add(!(state->lit.e));

                // remove solver
                solver_tree.erase(it_x);

                return {std::make_shared<solver_state>(new_state), added_tiles, updated_tiles,
                        create_assumptions(new_state)};
            }
            // does a solver state for a layout of aspect ratio of size x * y - 1 exist?
            if (const auto it_y = solver_tree.find({ar.x, ar.y - 1}); it_y != solver_tree.end())
            {
                // gather additional x-tiles
                std::unordered_set<typename Lyt::tile> added_tiles{}, updated_tiles{};
                for (decltype(ar.x) x = 0; x <= ar.x; ++x)
                {
                    added_tiles.emplace(x, ar.y);
                    updated_tiles.emplace(x, ar.y - 1);
                }

                // deep-copy solver state
                const auto   state     = it_y->second;
                solver_state new_state = {state->solver, {state->lit.e, get_lit_s()}};

                // reset southern constraints
                new_state.solver->add(!(state->lit.s));

                // remove solver
                solver_tree.erase(it_y);

                return {std::make_shared<solver_state>(new_state), added_tiles, updated_tiles,
                        create_assumptions(new_state)};
            }
            // no existing solver state; create a new one
            // all tiles are additional ones
            std::unordered_set<typename Lyt::tile> added_tiles{};
            for (decltype(ar.y) y = 0; y <= ar.y; ++y)
            {
                for (decltype(ar.x) x = 0; x <= ar.x; ++x)
                {
                    added_tiles.emplace(x, y);
                }
            }

            // create new state
            solver_state new_state{std::make_shared<z3::solver>(*ctx), {get_lit_e(), get_lit_s()}};

            return {std::make_shared<solver_state>(new_state), added_tiles, {}, create_assumptions(new_state)};
        }
        /**
         * Checks whether a given tile belongs to the added tiles of the current solver check point.
         *
         * @param t Tile to check.
         * @return `true` iff t is contained in check_point->added_tiles.
         */
        [[nodiscard]] bool is_added_tile(const typename Lyt::tile& t) const noexcept
        {
            return check_point->added_tiles.count(t) != 0;
        }
        /**
         * Checks whether a given tile belongs to the updated tiles of the current solver check point.
         *
         * @param t Tile to check.
         * @return `true` iff t is contained in check_point->updated_tiles.
         */
        [[nodiscard]] bool is_updated_tile(const typename Lyt::tile& t) const noexcept
        {
            return check_point->updated_tiles.count(t) != 0;
        }
        /**
         * Returns true, iff params.io_ports is set to false and n is either a constant or PI or PO node in network.
         *
         * @param n Node in network.
         * @return `true` iff n is to be skipped in a loop due to it being a constant or an I/O and params.io_ports ==
         * false.
         */
        [[nodiscard]] bool skip_const_or_io_node(const mockturtle::node<topology_ntk_t>& n) const noexcept
        {
            return network.is_constant(n) || ((network.is_pi(n) || network.is_po(n)) && !params.io_pins);
        }
        /**
         * Returns true, iff skip_const_or_io_node returns true for either source or target of the given edge..
         *
         * @param e Edge in network.
         * @return `true` iff e is to be skipped in a loop due to it having constant or I/O nodes while params.io_ports
         * == false.
         */
        [[nodiscard]] bool skip_const_or_io_edge(const mockturtle::edge<topology_ntk_t>& e) const noexcept
        {
            return skip_const_or_io_node(e.source) || skip_const_or_io_node(e.target);
        }
        /**
         * Applies a given function to all added tiles in the current solver check point.
         *
         * @tparam Fn Functor type.
         * @param fn Unary function to apply to all added tiles. Must receive a tile as parameter.
         */
        template <typename Fn>
        void apply_to_added_tiles(Fn&& fn)
        {
            std::for_each(check_point->added_tiles.cbegin(), check_point->added_tiles.cend(), fn);
        }
        /**
         * Applies a given function to all updated tiles in the current solver check point.
         *
         * @tparam Fn Functor type.
         * @param fn Unary function to apply to all updated tiles. Must receive a tile as parameter.
         */
        template <typename Fn>
        void apply_to_updated_tiles(Fn&& fn)
        {
            std::for_each(check_point->updated_tiles.cbegin(), check_point->updated_tiles.cend(), fn);
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
            apply_to_added_tiles(fn);
            apply_to_updated_tiles(fn);
        }
        /**
         * Determines the number of child nodes to some given node n in the stored logic network, not counting constants
         * and not counting primary inputs if params.io_pins is not set.
         *
         * @param n Node in the stored network.
         * @return Number of incoming nodes to n.
         */
        [[nodiscard]] uint32_t network_in_degree(const mockturtle::node<topology_ntk_t>& n) const noexcept
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
        /**
         * Determines the number of parent nodes to some given node n in the stored logic network, not counting
         * constants and not counting primary outputs if params.io_pins is not set.
         *
         * @param n Node in the stored network.
         * @return Number of outgoing nodes of n.
         */
        [[nodiscard]] uint32_t network_out_degree(const mockturtle::node<topology_ntk_t>& n) const noexcept
        {
            uint32_t degree{0};
            network.foreach_fanout(n,
                                   [this, &degree](const auto& fn)
                                   {
                                       if (!skip_const_or_io_node(fn))
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
        [[nodiscard]] assumption_literals& lit() const noexcept
        {
            return check_point->state->lit;
        }
        /**
         * Returns a tn variable from the stored context representing that tile t has node n assigned.
         *
         * @param t Tile to be considered.
         * @param n Node to be considered.
         * @return tn variable from ctx.
         */
        [[nodiscard]] z3::expr get_tn(const typename Lyt::tile& t, const mockturtle::node<topology_ntk_t>& n)
        {
            return ctx->bool_const(fmt::format("tn_({},{})_{}", t.x, t.y, n).c_str());
        }
        /**
         * Returns a te variable from the stored context representing that tile t has edge e assigned.
         *
         * @param t Tile to be considered.
         * @param e Edge to be considered.
         * @return te variable from ctx.
         */
        [[nodiscard]] z3::expr get_te(const typename Lyt::tile& t, const mockturtle::edge<topology_ntk_t>& e)
        {
            return ctx->bool_const(fmt::format("te_({},{})_({},{})", t.x, t.y, e.source, e.target).c_str());
        }
        /**
         * Returns a tc variable from the stored context representing that information flows from tile t1 to tile t2.
         *
         * @param t1 Tile 1 to be considered.
         * @param t2 Tile 2 to be considered that is adjacent to t1.
         * @return tc variable from ctx.
         */
        [[nodiscard]] z3::expr get_tc(const typename Lyt::tile& t1, const typename Lyt::tile& t2)
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
        [[nodiscard]] z3::expr get_tp(const typename Lyt::tile& t1, const typename Lyt::tile& t2)
        {
            return ctx->bool_const(fmt::format("tp_({},{})_({},{})", t1.x, t1.y, t2.x, t2.y).c_str());
        }
        /**
         * Returns an ncl variable from the stored context representing node n's (pi) clock number.
         *
         * @param n Node to be considered.
         * @return ncl variable from ctx.
         */
        [[nodiscard]] z3::expr get_ncl(const mockturtle::node<topology_ntk_t>& n, const unsigned clk)
        {
            return ctx->bool_const(fmt::format("ncl_{}_{}", n, clk).c_str());
        }
        /**
         * Returns a tcl variable from the stored context representing tile t's clock number.
         *
         * @param t Tile to be considered.
         * @return tcl variable from ctx.
         */
        [[nodiscard]] z3::expr get_tcl(const typename Lyt::tile& t, const unsigned clk)
        {
            return ctx->bool_const(fmt::format("tcl_({},{})_{}", t.x, t.y, clk).c_str());
        }
        /**
         * Returns a tse variable from the stored context representing tile t's synchronization element delay in cycles.
         *
         * @param t Tile to be considered.
         * @return tse variable from ctx.
         */
        [[nodiscard]] z3::expr get_tse(const typename Lyt::tile& t)
        {
            return ctx->int_const(fmt::format("tse_({},{})", t.x, t.y).c_str());
        }
        /**
         * Helper function for generating an equality of an arbitrary number of expressions.
         *
         * @param v Vector of expressions to equalize.
         * @return Expression that represents the equality of all elements in v.
         */
        [[nodiscard]] z3::expr mk_eq(const z3::expr_vector& v) const
        {
            z3::expr_vector eq{*ctx};
            for (int i = 1; static_cast<decltype(v.size())>(i) < v.size(); ++i)
            {
                eq.push_back(v[static_cast<unsigned int>(i - 1)] == v[static_cast<unsigned int>(i)]);
            }

            return z3::mk_and(eq);
        }
        /**
         * Helper function for generating an implication lit -> constraint where lit is the given assumption literal.
         *
         * @param constraint Constraint to be implied.
         * @param lit Assumption literal.
         * @return lit -> constraint.
         */
        [[nodiscard]] z3::expr mk_as(const z3::expr& constraint, const z3::expr& lit) const
        {
            return z3::implies(lit, constraint);
        }
        /**
         * Helper function for generating an implication lit -> constraint where lit is the assumption literal
         * responsible for t, i.e., e if t is at eastern border, s if t is at southern border, and (e and s) if t is
         * the corner tile.
         *
         * @param constraint Constraint to be implied.
         * @param t Tile to consider for literal picking.
         * @return lit -> constraint.
         */
        [[nodiscard]] z3::expr mk_as_if_se(const z3::expr& constraint, const typename Lyt::tile& t) const
        {
            const auto east = layout.is_at_eastern_border(t), south = layout.is_at_southern_border(t);

            if (east && south)
            {
                return mk_as(constraint, lit().e && lit().s);
            }
            if (east)
            {
                return mk_as(constraint, lit().e);
            }
            if (south)
            {
                return mk_as(constraint, lit().s);
            }

            return constraint;
        }
        /**
         * Helper function to create an expression that assigns a matching clocking to a tile given its outgoing tile.
         * This function is only needed for irregular clocking schemes.
         *
         * @param t1 Tile.
         * @param t2 Tile outgoing to t1 given its dataflow.
         * @return An expression of the form tcl(t1, 0) --> tcl(t2, 1) and tcl(t1, 1) --> tcl(t2, 2) and ... up to the
         * number of clock phases in the layout.
         */
        [[nodiscard]] z3::expr mk_clk_mod(const typename Lyt::tile& t1, const typename Lyt::tile& t2)
        {
            z3::expr_vector clk_mod{*ctx};

            for (auto i = 0u; i < layout.num_clocks(); ++i)
            {
                clk_mod.push_back(z3::implies(get_tcl(t1, i), get_tcl(t2, (i + 1) % layout.num_clocks())));
            }

            return z3::mk_and(clk_mod);
        }
        /**
         * Helper function to create an expression that maps from an ncl variable to a real value during a solver call.
         *
         * @param n Network node.
         * @return An expression of the form ite(ncl == 0, 0, ite(ncl == 1, 1, ...)) up to the number of clock phases in
         * the layout.
         */
        [[nodiscard]] z3::expr ncl_to_real(const mockturtle::node<topology_ntk_t>& n)
        {
            const std::function<z3::expr(const unsigned i)> ncl_ite_chain = [&, this](const auto i) -> z3::expr
            {
                if (i == layout.num_clocks())
                {
                    return ctx->real_val(i);
                }

                return z3::ite(get_ncl(n, i), ctx->real_val(i), ncl_ite_chain(i + 1));
            };

            return ncl_ite_chain(0u);
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
            const z3::expr zero = ctx->real_val(0u);
            const z3::expr one  = ctx->real_val(1u);

            const z3::expr num_phases = ctx->real_val(static_cast<unsigned>(layout.num_clocks()));

            layout.foreach_ground_tile(
                [this, &e, &ve, &one, &zero, &num_phases](const auto& t)
                {
                    // an artificial latch variable counts as an extra 1 clock cycle (n clock phases)
                    if (has_synchronization_elements_v<Lyt> && params.synchronization_elements && !params.desynchronize)
                    {
                        ve.push_back(z3::ite(get_te(t, e), get_tse(t) * num_phases + one, zero));
                    }
                    else
                    {
                        ve.push_back(z3::ite(get_te(t, e), one, zero));
                    }
                });
        }
        /**
         * Adds constraints to the solver to limit the number of elements that are going to be assigned to a tile to one
         * (node or edge) if no crossings are allowed. Otherwise, one node per tile or two edges per tile can be
         * placed.
         */
        void restrict_tile_elements()
        {
            apply_to_added_tiles(
                [this](const auto& t)
                {
                    if (params.crossings)
                    {
                        z3::expr_vector tn{*ctx};
                        network.foreach_node(
                            [this, &t, &tn](const auto& n)
                            {
                                if (!skip_const_or_io_node(n))
                                {
                                    tn.push_back(get_tn(t, n));
                                }
                            });

                        if (!tn.empty())
                        {
                            // at most 1 node
                            solver->add(z3::atmost(tn, 1u));
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

                        if (!tn.empty() && !te.empty())
                        {
                            // prevent the assignment of both vertices and edges to the same tile
                            solver->add(z3::implies(z3::atleast(tn, 1u), z3::atmost(te, 0)));
                            solver->add(z3::implies(z3::atleast(te, 1u), z3::atmost(tn, 0)));
                        }
                    }
                    else
                    {
                        z3::expr_vector tn{*ctx};
                        network.foreach_node(
                            [this, &t, &tn](const auto& n)
                            {
                                if (!skip_const_or_io_node(n))
                                {
                                    tn.push_back(get_tn(t, n));
                                }
                            });

                        foreach_edge(network,
                                     [this, &t, &tn](const auto& e)
                                     {
                                         if (!skip_const_or_io_edge(e))
                                         {
                                             tn.push_back(get_te(t, e));
                                         }
                                     });

                        if (!tn.empty())
                        {
                            // at most 1 node or edge
                            solver->add(z3::atmost(tn, 1u));
                        }
                    }
                });
        }
        /**
         * Adds constraints to the solver to enforce that each node is placed exactly once on exactly one tile.
         */
        void restrict_vertices()
        {
            network.foreach_node(
                [this](const auto& n)
                {
                    if (!skip_const_or_io_node(n))
                    {
                        z3::expr_vector tn{*ctx};
                        layout.foreach_ground_tile([this, &n, &tn](const auto& t) { tn.push_back(get_tn(t, n)); });

                        // use a tracking literal to disable constraints in case of UNSAT
                        solver->add(mk_as(z3::atleast(tn, 1u), lit().e && lit().s));
                        solver->add(z3::atmost(tn, 1u));
                    }
                });
        }
        /**
         * Adds constraints to the solver to enforce that each clock zone variable has valid bounds of 0 <= cl <= C,
         * where C is the maximum clock number. Uses a one-hot encoding.
         */
        void restrict_clocks()
        {
            apply_to_added_tiles(
                [this](const auto& t)
                {
                    // an evaluation has shown that it is in fact one magnitude slower to use bounded int_consts for tcl
                    // variables and even slower to use disjoint real_consts. Most likely, the performance benefit of
                    // ints over reals is due to Z3's quantifier-free finite domain (QF_FD) solver.
                    // TL;DR one-hot encoding rules!
                    z3::expr_vector tcl{*ctx};
                    for (auto i = 0u; i < layout.num_clocks(); ++i)
                    {
                        tcl.push_back(get_tcl(t, i));
                    }
                    solver->add(z3::atleast(tcl, 1) && z3::atmost(tcl, 1));
                });
        }
        /**
         * Adds constraints to the solver to enforce that a tile which was assigned with some node n has a successor
         * that is assigned to the adjacent node of n or an outgoing edge of n.
         */
        void define_gate_fanout_tiles()
        {
            apply_to_added_and_updated_tiles(
                [this](const auto& t)
                {
                    network.foreach_node(
                        [this, &t](const auto& n)
                        {
                            if (!skip_const_or_io_node(n))
                            {
                                const auto tn = get_tn(t, n);

                                z3::expr_vector conj{*ctx};
                                foreach_outgoing_edge(
                                    network, n,
                                    [this, &t, &conj](const auto& ae)
                                    {
                                        if (!skip_const_or_io_edge(ae))
                                        {
                                            z3::expr_vector disj{*ctx};

                                            if (const auto tgt = ae.target; layout.is_regularly_clocked())
                                            {
                                                layout.foreach_outgoing_clocked_zone(
                                                    t,
                                                    [this, &t, &disj, &tgt, &ae](const auto& at) {
                                                        disj.push_back((get_tn(at, tgt) || get_te(at, ae)) &&
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
                                                        const auto mod = mk_clk_mod(t, at);

                                                        disj.push_back(((get_tn(at, tgt) || get_te(at, ae)) && mod) &&
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
                                    solver->add(mk_as_if_se(z3::implies(tn, z3::mk_and(conj)), t));
                                }
                            }
                        });
                });
        }
        /**
         * Adds constraints to the solver to enforce that a tile which was assigned with some node n has a predecessor
         * that is assigned to the inversely adjacent node of n or an incoming edge of n.
         */
        void define_gate_fanin_tiles()
        {
            apply_to_added_and_updated_tiles(
                [this](const auto& t)
                {
                    network.foreach_node(
                        [this, &t](const auto& n)
                        {
                            if (!skip_const_or_io_node(n))
                            {
                                const auto tn = get_tn(t, n);

                                z3::expr_vector conj{*ctx};

                                foreach_incoming_edge(
                                    network, n,
                                    [this, &t, &conj](const auto& iae)
                                    {
                                        if (!skip_const_or_io_edge(iae))
                                        {
                                            z3::expr_vector disj{*ctx};

                                            if (const auto src = iae.source; layout.is_regularly_clocked())
                                            {
                                                layout.foreach_incoming_clocked_zone(
                                                    t,
                                                    [this, &t, &disj, &src, &iae](const auto& iat) {
                                                        disj.push_back((get_tn(iat, src) || get_te(iat, iae)) &&
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
                                                        const auto mod = mk_clk_mod(iat, t);

                                                        disj.push_back(
                                                            ((get_tn(iat, src) || get_te(iat, iae)) && mod) &&
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
                                    solver->add(mk_as_if_se(z3::implies(tn, z3::mk_and(conj)), t));
                                }
                            }
                        });
                });
        }
        /**
         * Adds constraints to the solver to enforce that a tile that was assigned with some edge has a successor which
         * is assigned to the adjacent node or another edge.
         */
        void define_wire_fanout_tiles()
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
                                const auto te = e.target;

                                z3::expr_vector disj{*ctx};

                                if (layout.is_regularly_clocked())
                                {
                                    layout.foreach_outgoing_clocked_zone(
                                        t, [this, &t, &e, &te, &disj](const auto& at)
                                        { disj.push_back((get_tn(at, te) || get_te(at, e)) && get_tc(t, at)); });
                                }
                                else  // irregular clocking
                                {
                                    layout.foreach_adjacent_tile(
                                        t,
                                        [this, &t, &e, &te, &disj](const auto& at)
                                        {
                                            // clocks must differ by 1
                                            const auto mod = mk_clk_mod(t, at);

                                            disj.push_back(((get_tn(at, te) || get_te(at, e)) && mod) && get_tc(t, at));
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
         * which is assigned to the inversely adjacent node or another edge.
         */
        void define_wire_fanin_tiles()
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
                                const auto se = e.source;

                                z3::expr_vector disj{*ctx};

                                if (layout.is_regularly_clocked())
                                {
                                    layout.foreach_incoming_clocked_zone(
                                        t, [this, &t, &e, &se, &disj](const auto& iat)
                                        { disj.push_back((get_tn(iat, se) || get_te(iat, e)) && get_tc(iat, t)); });
                                }
                                else  // irregular clocking
                                {
                                    layout.foreach_adjacent_tile(t,
                                                                 [this, &t, &e, &se, &disj](const auto& iat)
                                                                 {
                                                                     // clocks must differ by 1
                                                                     const auto mod = mk_clk_mod(iat, t);

                                                                     disj.push_back(
                                                                         ((get_tn(iat, se) || get_te(iat, e)) && mod) &&
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
                                                    z3::implies(get_tp(t1, t2) && get_tp(t2, t3), get_tp(t1, t3)));
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
            apply_to_added_tiles([this](const auto& t) { solver->add(!(get_tp(t, t))); });
        }
        /**
         * Adds constraints to the solver to ensure that the cl variable of primary input pi is set to the clock zone
         * value of the tile pi is assigned to. Necessary to be taken into account for path lengths.
         */
        void assign_pi_clockings()
        {
            const auto assign = [this](const auto n) -> void
            {
                apply_to_added_tiles(
                    [this, &n](const auto& t)
                    {
                        if (layout.is_regularly_clocked())
                        {
                            solver->add(z3::implies(get_tn(t, n),
                                                    get_ncl(n, static_cast<unsigned>(layout.get_clock_number(t)))));
                        }
                        else  // irregular clocking
                        {
                            for (auto i = 0u; i < layout.num_clocks(); ++i)
                            {
                                solver->add(z3::implies(get_tn(t, n), get_ncl(n, i) == get_tcl(t, i)));
                            }
                        }
                    });
            };

            if (!(params.border_io && is_linear_scheme<Lyt>(layout.get_clocking_scheme())))
            {
                // ensure that exactly one ncl variable is set for each node
                network.foreach_node(
                    [this](const auto& n)
                    {
                        z3::expr_vector ncl{*ctx};
                        for (auto i = 0u; i < layout.num_clocks(); ++i)
                        {
                            ncl.push_back(get_ncl(n, i));
                        }
                        solver->add(z3::atleast(ncl, 1) && z3::atmost(ncl, 1));
                    });

                if (params.io_pins)
                {
                    network.foreach_pi(assign);
                }
                else
                {
                    network.foreach_pi(
                        [this, &assign](const auto& pi)
                        {
                            network.foreach_fanout(pi,
                                                   [this, &assign](const auto& fn)
                                                   {
                                                       if (!skip_const_or_io_node(fn))
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
         * in the layout modulo timing, i.e., plus the clock zone assigned to their PIs.
         */
        void global_synchronization()
        {
            // restrict PIs to the first c x c tiles of the layout
            const auto restrict_2ddwave_entry_tiles = [this](const auto& pi)
            {
                apply_to_added_tiles(
                    [this, &pi](const auto& t)
                    {
                        if (t.x > layout.num_clocks() - 1u || t.y > layout.num_clocks() - 1u)
                        {
                            solver->add(!(get_tn(t, pi)));
                        }
                    });
            };

            const auto define_length = [this](const mockturtle::node<topology_ntk_t>& n)
            {
                const auto paths = all_incoming_edge_paths(network, n);

                const auto longest_path = std::max_element(
                    paths.cbegin(), paths.cend(), [](const auto& p1, const auto& p2) { return p1.size() < p2.size(); });

                if (longest_path == paths.cend())
                {
                    return;
                }

                z3::expr_vector all_path_lengths{*ctx};
                for (const auto& p : paths)
                {
                    z3::expr_vector path_length{*ctx};

                    // respect number of vertices as an offset to path length
                    // this works because every node must be placed
                    if (const auto offset = static_cast<int>(p.size() - longest_path->size()); offset)
                    {
                        path_length.push_back(ctx->real_val(offset));
                    }

                    for (const auto& e : p)
                    {
                        // respect clock zone of PI if one is involved
                        if (const auto src = e.source; params.io_pins && network.is_pi(src))
                        {
                            path_length.push_back(ncl_to_real(src));
                        }
                        else if (!params.io_pins)
                        {
                            if (has_incoming_primary_input(network, src))
                            {
                                path_length.push_back(ncl_to_real(src));
                            }
                        }

                        tile_ite_counters(e, path_length);
                    }
                    all_path_lengths.push_back(z3::sum(path_length));
                }

                // use a tracking literal to disable constraints in case of UNSAT
                solver->add(mk_as(mk_eq(all_path_lengths), lit().e && lit().s));
            };

            // much simpler but equisatisfiable version of the constraint for 2DDWave clocking with border I/Os
            if (params.border_io && (layout.is_clocking_scheme(clock_name::TWODDWAVE) ||
                                     layout.is_clocking_scheme(clock_name::TWODDWAVE_HEX)))
            {
                if (params.io_pins)
                {
                    network.foreach_pi([&restrict_2ddwave_entry_tiles](const auto& pi)
                                       { restrict_2ddwave_entry_tiles(pi); });
                }
                else
                {
                    network.foreach_pi(
                        [this, &restrict_2ddwave_entry_tiles](const auto& pi) {
                            network.foreach_fanout(pi, [&restrict_2ddwave_entry_tiles](const auto& fn)
                                                   { restrict_2ddwave_entry_tiles(fn); });
                        });
                }
            }
            else if (params.border_io &&
                     (layout.is_clocking_scheme(clock_name::COLUMNAR) || layout.is_clocking_scheme(clock_name::ROW)))
            {
                // Columnar and row clocking scheme don't need the path length constraints when border pins are enabled
            }
            // all other configurations get expensive path length constraints
            else
            {
                if (params.io_pins)
                {
                    network.foreach_po([&define_length](const auto& po) { define_length(po); });
                }
                else
                {
                    network.foreach_po(
                        [this, &define_length](const auto& po) {
                            network.foreach_fanin(po, [this, &define_length](const auto& fi)
                                                  { define_length(network.get_node(fi)); });
                        });
                }
            }
        }
        /**
         * Adds constraints to the solver to prevent edges or vertices to be assigned to tiles with an insufficient
         * number of predecessors/successors. Symmetry breaking constraints.
         */
        void prevent_insufficiencies()
        {
            layout.foreach_ground_tile(
                [this](const auto& t)
                {
                    if (layout.is_at_eastern_border(t) || layout.is_at_southern_border(t) || is_updated_tile(t))
                    {
                        if (layout.is_regularly_clocked())
                        {
                            network.foreach_node(
                                [this, &t](const auto& n)
                                {
                                    if (!skip_const_or_io_node(n))
                                    {
                                        // if node n has more adjacent or inversely adjacent elements than tile t
                                        if (layout.out_degree(t) < network_out_degree(n) ||
                                            layout.in_degree(t) < network_in_degree(n))
                                        {
                                            // if t is at eastern/southern border, its adjacencies might change
                                            if (layout.is_at_eastern_border(t) || layout.is_at_southern_border(t))
                                            {
                                                // add restriction as assumption only
                                                check_point->assumptions.push_back(!(get_tn(t, n)));
                                            }
                                            else  // t is an updated tile
                                            {
                                                // add hard constraint
                                                solver->add(!(get_tn(t, n)));
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
                                                     if (layout.is_at_eastern_border(t) ||
                                                         layout.is_at_southern_border(t))
                                                     {
                                                         // add restriction as assumption only
                                                         check_point->assumptions.push_back(!(get_te(t, e)));
                                                     }
                                                     else if (is_updated_tile(t))  // nothing's about to change here
                                                     {
                                                         // add hard constraint
                                                         solver->add(!(get_te(t, e)));
                                                     }
                                                 }
                                             }
                                         });
                        }
                        else  // irregular clocking
                        {
                            const auto tile_degree = num_adjacent_coordinates(layout, t);

                            network.foreach_node(
                                [this, &t, &tile_degree](const auto& n)
                                {
                                    if (!skip_const_or_io_node(n))
                                    {
                                        // in an irregular clocking scheme, not so strict restrictions can be made
                                        if (tile_degree < network_out_degree(n) + network_in_degree(n))
                                        {
                                            // if t is at eastern/southern border, its adjacencies might change
                                            if (layout.is_at_eastern_border(t) || layout.is_at_southern_border(t))
                                            {
                                                // add restriction as assumption only
                                                check_point->assumptions.push_back(!(get_tn(t, n)));
                                            }
                                            else  // nothing's about to change here
                                            {
                                                // add hard constraint
                                                solver->add(!(get_tn(t, n)));
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
                                                                 const auto tc = get_tc(t, at);
                                                                 acc.push_back(tc);
                                                                 tcc.push_back(tc);
                                                             });

                        layout.foreach_incoming_clocked_zone(t,
                                                             [this, &t, &tcc, &iacc](const auto& iat)
                                                             {
                                                                 const auto itc = get_tc(iat, t);
                                                                 iacc.push_back(itc);
                                                                 tcc.push_back(itc);
                                                             });
                    }
                    else  // irregular clocking
                    {
                        layout.foreach_adjacent_tile(t,
                                                     [this, &t, &tcc, &acc, &iacc](const auto& at)
                                                     {
                                                         const auto tc = get_tc(t, at);
                                                         acc.push_back(tc);
                                                         tcc.push_back(tc);

                                                         const auto itc = get_tc(at, t);
                                                         iacc.push_back(itc);
                                                         tcc.push_back(itc);
                                                     });
                    }

                    z3::expr_vector ow{*ctx};

                    network.foreach_node(
                        [this, &t, &acc, &iacc, &ow](const auto& n)
                        {
                            if (!skip_const_or_io_node(n))
                            {
                                const auto tn   = get_tn(t, n);
                                const auto aon  = network_out_degree(n);
                                const auto iaon = network_in_degree(n);

                                ow.push_back(tn);

                                // if node n is assigned to a tile, the number of connections need to correspond
                                if (!acc.empty())
                                {
                                    solver->add(
                                        mk_as_if_se(z3::implies(tn, z3::atleast(acc, aon) && z3::atmost(acc, aon)), t));
                                }
                                if (!iacc.empty())
                                {
                                    solver->add(mk_as_if_se(
                                        z3::implies(tn, z3::atleast(iacc, iaon) && z3::atmost(iacc, iaon)), t));
                                }
                            }
                        });

                    z3::expr_vector wg{*ctx};
                    foreach_edge(network,
                                 [this, &t, &ow, &wg](const auto& e)
                                 {
                                     if (!skip_const_or_io_edge(e))
                                     {
                                         const auto te = get_te(t, e);
                                         ow.push_back(te);
                                         wg.push_back(te);
                                     }
                                 });

                    // if there is any edge assigned to a tile, the number of connections need to correspond
                    if (!wg.empty())
                    {
                        if (!acc.empty())
                        {
                            solver->add(mk_as_if_se(z3::implies(z3::atleast(wg, 1u) && z3::atmost(wg, 1u),
                                                                z3::atleast(acc, 1u) && z3::atmost(acc, 1u)),
                                                    t));
                        }
                        if (!iacc.empty())
                        {
                            solver->add(mk_as_if_se(z3::implies(z3::atleast(wg, 1u) && z3::atmost(wg, 1u),
                                                                z3::atleast(iacc, 1u) && z3::atmost(iacc, 1u)),
                                                    t));
                        }

                        // if crossings are allowed, there must be exactly four connections (one in each direction) for
                        // two assigned edges
                        if (params.crossings)
                        {
                            // don't assign two edges to a tile that lacks connectivity
                            if (acc.size() < 2 || iacc.size() < 2)
                            {
                                solver->add(mk_as_if_se(z3::atmost(wg, 1u), t));
                            }
                            else if (wg.size() >= 2)  // it is pointless to check an expression vector of size < 2 for
                                                      // at least 2 set variables
                            {
                                if (!acc.empty())
                                {
                                    solver->add(mk_as_if_se(z3::implies(z3::atleast(wg, 2u) && z3::atmost(wg, 2u),
                                                                        z3::atleast(acc, 2u) && z3::atmost(acc, 2u)),
                                                            t));
                                }
                                if (!iacc.empty())
                                {
                                    solver->add(mk_as_if_se(z3::implies(z3::atleast(wg, 2u) && z3::atmost(wg, 2u),
                                                                        z3::atleast(iacc, 2u) && z3::atmost(iacc, 2u)),
                                                            t));
                                }
                            }
                        }
                    }

                    // if tile t is empty, there must not be any connection from or to tile t established
                    if (ow.size() > 1 &&
                        !tcc.empty())  // test for > 1 to exclude single-node networks from this constraint
                    {
                        solver->add(mk_as_if_se(z3::atmost(ow, 0u) == z3::atmost(tcc, 0u), t));
                    }
                });  // function call to added and updated tiles done

            // lacking connections for irregular clocking
            if (!layout.is_regularly_clocked())
            {
                z3::expr_vector ccp{*ctx};

                // iterate over added_tiles only here to not duplicate constraints
                apply_to_added_tiles(
                    [this](const auto& t)
                    {
                        z3::expr_vector ow{*ctx};
                        network.foreach_node(
                            [this, &t, &ow](const auto& n)
                            {
                                if (!skip_const_or_io_node(n))
                                {
                                    ow.push_back(get_tn(t, n));
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
                            solver->add(z3::implies(z3::atmost(ow, 0u), get_tcl(t, 0)));
                        }
                    });
            }
        }
        /**
         * Adds constraints to the solver to prohibit certain node placements based on the network hierarchy if the
         * clocking scheme is feed-back-free. Symmetry breaking constraints.
         */
        void utilize_hierarchical_information()
        {
            // symmetry breaking for columnar clocking
            if (layout.is_clocking_scheme(clock_name::COLUMNAR))
            {
                // restrict node placement according to the hierarchy level
                if (params.io_pins && params.border_io)
                {
                    network.foreach_node(
                        [this](const auto& n)
                        {
                            if (!skip_const_or_io_node(n))
                            {
                                const auto l  = depth_ntk.level(n);
                                const auto il = inv_levels[network.node_to_index(n)];

                                // cannot be placed with too little distance to western border
                                for (auto column = 0u;
                                     column < std::min(static_cast<decltype(layout.y())>(l), layout.x()); ++column)
                                {
                                    for (auto row = 0u; row <= layout.y(); ++row)
                                    {
                                        if (const auto t = typename Lyt::tile{column, row}; is_added_tile(t))
                                        {
                                            solver->add(!(get_tn(t, n)));

                                            // same for the outgoing edges
                                            foreach_outgoing_edge(network, n,
                                                                  [this, &t](const auto& e)
                                                                  {
                                                                      if (!skip_const_or_io_edge(e))
                                                                      {
                                                                          solver->add(!(get_te(t, e)));
                                                                      }
                                                                  });
                                        }
                                    }
                                }

                                // cannot be placed with too little distance to eastern border
                                for (auto column = layout.x() - il + 1; column < layout.x(); ++column)
                                {
                                    for (auto row = 0u; row <= layout.y(); ++row)
                                    {
                                        const auto t = typename Lyt::tile{column, row};

                                        // use assumptions here because the south-east corner moves away in the
                                        // following iterations
                                        check_point->assumptions.push_back(!(get_tn(t, n)));

                                        // same for the incoming edges
                                        foreach_incoming_edge(network, n,
                                                              [this, &t](const auto& e)
                                                              {
                                                                  if (!skip_const_or_io_edge(e))
                                                                  {
                                                                      check_point->assumptions.push_back(
                                                                          !(get_te(t, e)));
                                                                  }
                                                              });
                                    }
                                }
                            }
                        });
                }
            }
            // symmetry breaking for row clocking
            if (layout.is_clocking_scheme(clock_name::ROW))
            {
                // restrict node placement according to the hierarchy level
                if (params.io_pins && params.border_io)
                {
                    network.foreach_node(
                        [this](const auto& n)
                        {
                            if (!skip_const_or_io_node(n))
                            {
                                const auto l  = depth_ntk.level(n);
                                const auto il = inv_levels[network.node_to_index(n)];

                                // cannot be placed with too little distance to northern border
                                for (auto row = 0u; row < std::min(static_cast<decltype(layout.y())>(l), layout.y());
                                     ++row)
                                {
                                    for (auto column = 0u; column <= layout.x(); ++column)
                                    {
                                        if (const auto t = typename Lyt::tile{column, row}; is_added_tile(t))
                                        {
                                            solver->add(!(get_tn(t, n)));

                                            // same for the outgoing edges
                                            foreach_outgoing_edge(network, n,
                                                                  [this, &t](const auto& e)
                                                                  {
                                                                      if (!skip_const_or_io_edge(e))
                                                                      {
                                                                          solver->add(!(get_te(t, e)));
                                                                      }
                                                                  });
                                        }
                                    }
                                }

                                // cannot be placed with too little distance to southern border
                                for (auto row = layout.y() - il + 1; row < layout.y(); ++row)
                                {
                                    for (auto column = 0u; column <= layout.x(); ++column)
                                    {
                                        const auto t = typename Lyt::tile{column, row};

                                        // use assumptions here because the south-east corner moves away in the
                                        // following iterations
                                        check_point->assumptions.push_back(!(get_tn(t, n)));

                                        // same for the incoming edges
                                        foreach_incoming_edge(network, n,
                                                              [this, &t](const auto& e)
                                                              {
                                                                  if (!skip_const_or_io_edge(e))
                                                                  {
                                                                      check_point->assumptions.push_back(
                                                                          !(get_te(t, e)));
                                                                  }
                                                              });
                                    }
                                }
                            }
                        });
                }
            }
            // symmetry breaking for 2DDWave clocking
            else if (layout.is_clocking_scheme(clock_name::TWODDWAVE))
            {
                // restrict node placement according to its hierarchy level
                if (params.io_pins && params.border_io)
                {
                    network.foreach_node(
                        [this](const auto& n)
                        {
                            if (!skip_const_or_io_node(n))
                            {
                                const auto l  = depth_ntk.level(n);
                                const auto il = inv_levels[network.node_to_index(n)];

                                // cannot be placed with too little distance to north-west corner
                                apply_to_added_tiles(
                                    [this, &n, &l, &il](const auto& t)
                                    {
                                        if (t.x + t.y < static_cast<decltype(t.x + t.y)>(l))
                                        {
                                            solver->add(!(get_tn(t, n)));

                                            // same for the outgoing edges
                                            foreach_outgoing_edge(network, n, [this, &t](const auto& e)
                                                                  { solver->add(!(get_te(t, e))); });
                                        }
                                        // cannot be placed with too little distance to south-east corner
                                        if (layout.x() - t.x + layout.y() - t.y < il)
                                        {
                                            // use assumptions here because the south-east corner moves away in the
                                            // following iterations
                                            check_point->assumptions.push_back(!(get_tn(t, n)));

                                            // same for the incoming edges
                                            foreach_incoming_edge(
                                                network, n, [this, &t](const auto& e)
                                                { check_point->assumptions.push_back(!(get_te(t, e))); });
                                        }
                                    });
                            }
                        });
                }
            }
        }
        /**
         * Adds constraints to the solver to position the primary inputs and primary outputs at the layout's borders.
         */
        void enforce_border_io()
        {
            const auto assign_border = [this](const auto& n)
            {
                apply_to_added_and_updated_tiles(
                    [this, &n](const auto& t)
                    {
                        if (!layout.is_at_any_border(t))
                        {
                            solver->add(!(get_tn(t, n)));
                        }
                    });
            };

            const auto assign_north = [this](const auto& n)
            {
                // no need to iterate over updated_tiles, because nothing changes there
                apply_to_added_tiles(
                    [this, &n](const auto& t)
                    {
                        if (!layout.is_at_northern_border(t))
                        {
                            solver->add(!(get_tn(t, n)));
                        }
                    });
            };

            const auto assign_west = [this](const auto& n)
            {
                // no need to iterate over updated_tiles, because nothing changes there
                apply_to_added_tiles(
                    [this, &n](const auto& t)
                    {
                        if (!layout.is_at_western_border(t))
                        {
                            solver->add(!(get_tn(t, n)));
                        }
                    });
            };

            const auto assign_east = [this](const auto& n)
            {
                apply_to_added_and_updated_tiles(
                    [this, &n](const auto& t)
                    {
                        if (!layout.is_at_eastern_border(t))
                        {
                            solver->add(!(get_tn(t, n)));
                        }
                    });
            };

            const auto assign_south = [this](const auto& n)
            {
                apply_to_added_and_updated_tiles(
                    [this, &n](const auto& t)
                    {
                        if (!layout.is_at_southern_border(t))
                        {
                            solver->add(!(get_tn(t, n)));
                        }
                    });
            };

            if (params.io_pins)
            {
                network.foreach_pi(
                    [this, &assign_north, &assign_west, &assign_border](const auto& pi)
                    {
                        layout.is_clocking_scheme(clock_name::COLUMNAR) ? assign_west(pi) :
                        layout.is_clocking_scheme(clock_name::ROW)      ? assign_north(pi) :
                                                                          assign_border(pi);
                    });
                network.foreach_po(
                    [this, &assign_east, &assign_south, &assign_border](const auto& po)
                    {
                        layout.is_clocking_scheme(clock_name::COLUMNAR) ? assign_east(network.get_node(po)) :
                        layout.is_clocking_scheme(clock_name::ROW)      ? assign_south(network.get_node(po)) :
                                                                          assign_border(network.get_node(po));
                    });
            }
            else
            {
                network.foreach_pi(
                    [this, &assign_north, &assign_west, &assign_border](const auto& pi)
                    {
                        network.foreach_fanout(pi,
                                               [this, &assign_north, &assign_west, &assign_border](const auto& fon)
                                               {
                                                   if (!skip_const_or_io_node(fon))
                                                   {
                                                       layout.is_clocking_scheme(clock_name::COLUMNAR) ?
                                                           assign_west(fon) :
                                                       layout.is_clocking_scheme(clock_name::ROW) ? assign_north(fon) :
                                                                                                    assign_border(fon);
                                                   }
                                               });
                    });

                network.foreach_po(
                    [this, &assign_east, &assign_south, &assign_border](const auto& po)
                    {
                        network.foreach_fanin(
                            po,
                            [this, &assign_east, &assign_south, &assign_border](const auto& fi)
                            {
                                if (const auto fin = network.get_node(fi); !skip_const_or_io_node(fin))
                                {
                                    layout.is_clocking_scheme(clock_name::COLUMNAR) ? assign_east(fin) :
                                    layout.is_clocking_scheme(clock_name::ROW)      ? assign_south(fin) :
                                                                                      assign_border(fin);
                                }
                            });
                    });
            }
        }
        /**
         * Adds constraints to the solver to enforce that no bent inverters are used.
         */
        void enforce_straight_inverters()
        {
            if constexpr (has_foreach_adjacent_opposite_tiles_v<Lyt>)
            {
                apply_to_added_and_updated_tiles(
                    [this](const auto& t)
                    {
                        network.foreach_node(
                            [this, &t](const auto& inv)
                            {
                                // skip all operations except for inverters
                                if (network.is_inv(inv))
                                {
                                    // I/Os inverters are always straight, so they can be skipped as well
                                    if (!skip_const_or_io_node(inv))
                                    {
                                        // vector to store possible direction combinations
                                        z3::expr_vector ve{*ctx};

                                        layout.foreach_adjacent_opposite_tiles(
                                            t,
                                            [this, &t, &ve](const auto& cp)
                                            {
                                                const auto &t1 = cp.first, t2 = cp.second;

                                                if ((layout.is_incoming_clocked(t, t1) &&
                                                     layout.is_outgoing_clocked(t, t2)) ||
                                                    !layout.is_regularly_clocked())
                                                {
                                                    ve.push_back(get_tc(t1, t) && get_tc(t, t2));
                                                }
                                                if ((layout.is_incoming_clocked(t, t2) &&
                                                     layout.is_outgoing_clocked(t, t1)) ||
                                                    !layout.is_regularly_clocked())
                                                {
                                                    ve.push_back(get_tc(t2, t) && get_tc(t, t1));
                                                }
                                            });

                                        if (!ve.empty())
                                        {
                                            // inverter can be placed here; enforce any of the direction combinations
                                            // found possible above
                                            solver->add(mk_as_if_se(z3::implies(get_tn(t, inv), z3::mk_or(ve)), t));
                                        }
                                        else
                                        {
                                            // inverter cannot be placed here, add constraint to avoid this case and
                                            // speed up solving
                                            solver->add(mk_as_if_se(!(get_tn(t, inv)), t));
                                        }
                                    }
                                }
                            });
                    });
            }
        }
        /**
         * Adds constraints to the solver to prevent negative valued synchronization elements and that gate tiles cannot
         * be assigned synchronization elements.
         */
        void restrict_synchronization_elements()
        {
            if constexpr (has_synchronization_elements_v<Lyt>)
            {
                const auto zero = ctx->int_val(0u);

                apply_to_added_tiles(
                    [this, &zero](const auto& t)
                    {
                        // synchronization elements must be positive
                        const auto l = get_tse(t);
                        solver->add(l >= zero);

                        // tiles without wires cannot be synchronization elements
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
                    });
            }
        }
        /**
         * Adds constraints to the solver to enforce technology-specific restrictions.
         */
        void technology_specific_constraints()
        {
            if (params.technology_specifics == technology_constraints::TOPOLINANO)
            {
                // fan-outs (couplers) cannot be followed by fan-outs and can only have inputs from their north-western
                // tiles
                network.foreach_gate(
                    [this](const auto& fon)
                    {
                        if (network.is_fanout(fon))
                        {
                            // prohibit succeeding fan-outs
                            network.foreach_fanout(
                                fon,
                                [this, &fon](const auto& afon)
                                {
                                    if (network.is_fanout(afon))
                                    {
                                        layout.foreach_ground_tile(
                                            [this, &fon, &afon](const auto& t)
                                            {
                                                layout.foreach_outgoing_clocked_zone(
                                                    t,
                                                    [this, &fon, &afon, &t](const auto& at)
                                                    {
                                                        if (is_added_tile(at))
                                                        {
                                                            solver->add(
                                                                z3::implies(get_tn(t, fon), !(get_tn(at, afon))));
                                                        }
                                                    });
                                            });
                                    }
                                });

                            // do not argue about fan-outs without predecessors in the following
                            if (network_in_degree(fon) != 0)
                            {
                                // enforce north-western input
                                apply_to_added_tiles(
                                    [this, &fon](const auto& t)
                                    {
                                        // if fo gets placed here, its predecessor must be on the north-western tile
                                        if (const auto nw = layout.north_west(t); nw != t)
                                        {
                                            solver->add(z3::implies(get_tn(t, fon), get_tc(nw, t)));

                                            // additionally, no crossing can precede a fan-out
                                            z3::expr_vector wv{*ctx};
                                            foreach_edge(network,
                                                         [this, &nw, &wv](const auto& e)
                                                         {
                                                             if (!skip_const_or_io_edge(e))
                                                             {
                                                                 wv.push_back(get_te(nw, e));
                                                             }
                                                         });

                                            solver->add(z3::implies(get_tn(t, fon), z3::atmost(wv, 1u)));
                                        }
                                        // if tile t doesn't have a north-western adjacent tile, fon cannot be placed
                                        // here
                                        else
                                        {
                                            solver->add(!(get_tn(t, fon)));
                                        }
                                    });
                            }
                        }
                    });

                network.foreach_gate(
                    [this](const auto& n1)
                    {
                        // AND/OR/MAJ gates cannot be followed directly by an AND/OR/MAJ gate or a fan-out (coupler)
                        // additionally, if straight inverters are enforced, AND/OR/MAJ cannot be followed by NOT either
                        if (network.is_and(n1) || network.is_or(n1) || network.is_maj(n1))
                        {
                            network.foreach_fanout(
                                n1,
                                [this, &n1](const auto& n2)
                                {
                                    // only argue about AND/OR/MAJ/fan-out and, additionally, about NOT if straight
                                    // inverters are enforced
                                    if (network.is_and(n2) || network.is_or(n2) || network.is_maj(n2) ||
                                        network.is_fanout(n2) || (network.is_inv(n2) && params.straight_inverters))
                                    {
                                        layout.foreach_ground_tile(
                                            [this, &n1, &n2](const auto& t)
                                            {
                                                layout.foreach_outgoing_clocked_zone(
                                                    t,
                                                    [this, &n1, &n2, &t](const auto& at)
                                                    {
                                                        if (is_added_tile(at))
                                                        {
                                                            solver->add(z3::implies(get_tn(t, n1), !(get_tn(at, n2))));
                                                        }
                                                    });
                                            });
                                    }
                                });

                            // AND/OR/MAJ gates cannot be followed by a south-eastern connection
                            // and cannot be followed by a crossing directly
                            layout.foreach_ground_tile(
                                [this, &n1](const auto& t)
                                {
                                    if (auto ne = layout.north_east(t); ne == t)
                                    {
                                        // no north-eastern tile, do not place v1 here
                                        check_point->assumptions.push_back(!(get_tn(t, n1)));
                                    }
                                    else
                                    {
                                        // north-eastern tile exists, do not create a crossing here
                                        z3::expr_vector wv{*ctx};
                                        foreach_edge(network,
                                                     [this, &ne, &wv](const auto& e)
                                                     {
                                                         if (!skip_const_or_io_edge(e))
                                                         {
                                                             wv.push_back(get_te(ne, e));
                                                         }
                                                     });

                                        solver->add(z3::implies(get_tn(t, n1), z3::atmost(wv, 1u)));
                                    }
                                    if (auto se = layout.south_east(t); se != t)
                                    {
                                        // south-eastern tile exists, do not route a connection here
                                        if (is_added_tile(se))
                                        {
                                            solver->add(z3::implies(get_tn(t, n1), !(get_tc(t, se))));
                                        }
                                    }
                                });
                        }
                    });
            }
            // more target technology constraints go here
        }
        /**
         * Adds constraints to the solver to enforce blacklisting of certain gates.
         */
        void black_list_gates()  // TODO take advantage of incremental solving
        {
            const auto gather_black_list_expr = [this](const auto& port, const auto& t) noexcept
            {
                z3::expr_vector iop{*ctx};

                for (const auto& i : port.inp)
                {
                    iop.push_back(!(get_tc(port_direction_to_coordinate(layout, t, i), t)));
                }
                for (const auto& o : port.out)
                {
                    iop.push_back(!(get_tc(t, port_direction_to_coordinate(layout, t, o))));
                }

                return iop;
            };

            // the identity function as a truth table
            const auto identity = create_id_tt();

            // for each tile-functions pair
            for (const auto& [tile, exclusions] : black_list)
            {
                for (const auto& [gate, port_list] : exclusions)
                {
                    network.foreach_node(
                        [this, &gather_black_list_expr, &t = tile, &tt = gate, &ports = port_list](const auto& n)
                        {
                            if (!skip_const_or_io_node(n))
                            {
                                if (kitty::equal(tt, network.node_function(n)))
                                {
                                    if (ports.empty())
                                    {
                                        solver->add(!(get_tn(t, n)));
                                    }
                                    else
                                    {
                                        for (const auto& p : ports)
                                        {
                                            solver->add(
                                                z3::implies(get_tn(t, n), z3::mk_and(gather_black_list_expr(p, t))));
                                        }
                                    }
                                }
                            }
                        });

                    // truth table represents the identity; wires need to be additionally excluded
                    if (kitty::equal(gate, identity))
                    {
                        foreach_edge(network,
                                     [this, &gather_black_list_expr, &t = tile, &ports = port_list](const auto& e)
                                     {
                                         if (!skip_const_or_io_edge(e))
                                         {
                                             if (ports.empty())
                                             {
                                                 solver->add(!(get_te(t, e)));
                                             }
                                             for (const auto& p : ports)
                                             {
                                                 solver->add(z3::implies(get_te(t, e),
                                                                         z3::mk_and(gather_black_list_expr(p, t))));
                                             }
                                         }
                                     });
                    }
                }
            }
        }
        /**
         * Adds constraints to the given optimize to minimize the number of crossing tiles to use.
         *
         * @param optimize Pointer to an z3::optimize to add constraints to.
         */
        void minimize_wires(const optimize_ptr& optimize)
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
        void minimize_crossings(const optimize_ptr& optimize)
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
         * Adds constraints to the given optimize to enforce that the overall sum of synchronization element values
         * should be minimized.
         *
         * @param optimize Pointer to an z3::optimize to add constraints to.
         */
        void minimize_synchronization_elements(const optimize_ptr& optimize)
        {
            if constexpr (has_synchronization_elements_v<Lyt>)
            {
                z3::expr_vector se_counter{*ctx};
                layout.foreach_ground_tile([this, &se_counter](const auto& t) { se_counter.push_back(get_tse(t)); });

                optimize->minimize(z3::sum(se_counter));
            }
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
            define_gate_fanout_tiles();
            define_gate_fanin_tiles();
            define_wire_fanout_tiles();
            define_wire_fanin_tiles();

            // global synchronization constraints
            if (!params.desynchronize)
            {
                assign_pi_clockings();
                global_synchronization();
            }

            // open clocking scheme constraints
            if (!layout.is_regularly_clocked())
            {
                restrict_clocks();
            }

            // path/cycle constraints
            if (!is_linear_scheme<Lyt>(layout.get_clocking_scheme()))  // linear schemes; no cycles by definition
            {
                establish_sub_paths();
                establish_transitive_paths();
                eliminate_cycles();
            }

            // I/O pin constraints
            if (params.border_io)
            {
                enforce_border_io();
            }

            // straight inverter constraints
            if (params.straight_inverters)
            {
                enforce_straight_inverters();
            }

            // synchronization element constraints
            if (params.synchronization_elements && !params.desynchronize)
            {
                restrict_synchronization_elements();
            }

            // technology-specific constraints
            technology_specific_constraints();
            // blacklisting constraints
            black_list_gates();

            // symmetry breaking constraints
            prevent_insufficiencies();
            define_number_of_connections();
            utilize_hierarchical_information();
        }
        /**
         * Creates and returns a z3::optimize if optimization criteria were set by the configuration. The optimize gets
         * passed all constraints from the current solver and the respective optimization constraints are added to it,
         * too.
         */
        [[nodiscard]] optimize_ptr optimize()
        {
            const auto wires = params.minimize_wires, cross = params.minimize_crossings,
                       se = params.synchronization_elements && !params.desynchronize;
            if (!wires && !cross && !se)
            {
                return nullptr;
            }

            // non-const to automatically move optimizer via RVO
            auto optimizer = std::make_shared<z3::optimize>(*ctx);

            // add all solver constraints
            for (const auto& e : solver->assertions())
            {
                optimizer->add(e);
            }

            // add assumptions as constraints, too, because optimize::check with assumptions is broken
            for (const auto& e : check_point->assumptions)
            {
                optimizer->add(e);
            }

            // wire minimization constraints
            if (wires)
            {
                minimize_wires(optimizer);
            }

            // crossing minimization constraints
            if (cross)
            {
                minimize_crossings(optimizer);
            }

            // clock latches minimization constraints
            if (se)
            {
                minimize_synchronization_elements(optimizer);
            }

            return optimizer;
        }
        /**
         * Places a primary output pin represented by node n of the stored network onto tile t in the stored layout.
         *
         * @param t Tile to place the PO pin.
         * @param n Node in the stored network representing a PO.
         */
        void place_output(const typename Lyt::tile& t, const mockturtle::node<topology_ntk_t>& n)
        {
            const auto output_signal = network.make_signal(fanins(network, n).fanin_nodes[0]);

            layout.create_po(node2pos[output_signal][n], "", t);
        }
        /**
         * If an open clocking scheme was provided, this function extracts the clocking information from the given model
         * and assigns the respective clock zones to the stored layout.
         *
         * @param model Satisfying model to the generated instance.
         */
        void assign_layout_clocking(const z3::model& model)
        {
            // assign clock zones to the stored layout when using an open clocking scheme
            if (!layout.is_regularly_clocked())
            {
                layout.foreach_ground_tile(
                    [this, &model](const auto& t)
                    {
                        for (auto i = 0u; i < layout.num_clocks(); ++i)
                        {
                            if (model.eval(get_tcl(t, i), true).bool_value() == Z3_L_TRUE)
                            {
                                // assign clock number to tile t
                                layout.assign_clock_number(t, static_cast<typename Lyt::clock_number_t>(i));
                                // and to the tile above
                                layout.assign_clock_number(layout.above(t),
                                                           static_cast<typename Lyt::clock_number_t>(i));
                                // NOTE if this algorithm is ever to be extended for stacked FCN, this function needs to
                                // assign the clock zone to all tiles in the z direction or the clocking lookup must
                                // only consider the ground tile
                            }
                        }
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
        void route(const typename Lyt::tile& t, const mockturtle::edge<topology_ntk_t>& e, const z3::model& model)
        {
            layout.foreach_outgoing_clocked_zone(
                t,
                [this, &t, &e, &model](const auto& at)
                {
                    // if e got assigned to at according to the model together with a
                    // set connection variable between t and at
                    if (model.eval(get_te(at, e)).bool_value() == Z3_L_TRUE &&
                        model.eval(get_tc(t, at)).bool_value() == Z3_L_TRUE)
                    {
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
            // from now on, a clocking scheme is assigned and no distinction between regular and irregular clocking
            // must be made

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
                                if (model.eval(get_tn(t, n)).bool_value() == Z3_L_TRUE)
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
                                                           [this, &model, &n, &t, &lyt_signal](const auto& fon)
                                                           {
                                                               if (!skip_const_or_io_node(fon))
                                                               {
                                                                   // store the signal as branch towards fn
                                                                   node2pos[n].update_branch(fon, lyt_signal);

                                                                   mockturtle::edge<topology_ntk_t> e{n, fon};

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
                                if (model.eval(get_tn(t, pon)).bool_value() == Z3_L_TRUE)
                                {
                                    place_output(t, pon);
                                }
                            });
                    }
                });

            // assign synchronization elements if there were any in use
            if constexpr (has_synchronization_elements_v<Lyt>)
            {
                if (params.synchronization_elements)
                {
                    layout.foreach_ground_tile(
                        [this, &model](const auto& t)
                        {
                            layout.assign_synchronization_element(
                                t,
                                static_cast<typename Lyt::sync_elem_t>(model.eval(get_tse(t), true).get_numeral_int()));
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
    void update_timeout(smt_handler& handler, const mockturtle::stopwatch<>::duration& time) const
    {
        const auto time_elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(time).count();
        const auto time_left = (ps.timeout - time_elapsed > 0 ? static_cast<unsigned>(ps.timeout - time_elapsed) : 0u);

        if (!time_left)
        {
            throw z3::exception("timeout");
        }

        handler.set_timeout(time_left);
    }
    /**
     * Contains a context pointer and a currently worked on aspect ratio and can be shared between multiple worker
     * threads so that they can notify each other via context interrupts based on their individual results, i.e., a
     * thread that found a result at aspect ratio x * y can interrupt all other threads that are working on larger
     * layout sizes.
     */
    struct thread_info
    {
        /**
         * Pointer to a context.
         */
        ctx_ptr ctx;
        /**
         * Currently examined layout aspect ratio.
         */
        typename Lyt::aspect_ratio worker_aspect_ratio;
    };
    /**
     * Thread function for the asynchronous solving strategy. It registers its own context in the given list of
     * thread_info objects and starts exploring the search space. It fetches the next aspect ratio to work on from the
     * global aspect ratio iterator which is protected by a mutex. When a result is found, other threads that are
     * currently working on larger layout aspect ratios are interrupted while smaller ones may finish running.
     *
     * @param t_num Thread's identifier.
     * @param ti_list Pointer to a list of shared thread info that the threads use for communication.
     * @return A found layout or nullptr if being interrupted.
     */
    [[nodiscard]] std::optional<Lyt> explore_asynchronously(const unsigned                                   t_num,
                                                            const std::shared_ptr<std::vector<thread_info>>& ti_list)
    {
        const auto ctx = std::make_shared<z3::context>();

        Lyt layout{{}, scheme};

        smt_handler handler{ctx, layout, *ntk, ps, black_list};
        (*ti_list)[t_num].ctx = ctx;

        while (true)
        {
            typename Lyt::aspect_ratio ar;

            // mutually exclusive access to the aspect ratio iterator
            {
                std::lock_guard<std::mutex> guard(ari_mutex);

                ++ari;
                ar = *ari;  // operations ++ and * are split to prevent a vector copy construction

                // log the examination of a new aspect ratio
                pst.num_aspect_ratios++;
            }

            if ((ar.x + 1) * (ar.y + 1) > ps.upper_bound_area || (ar.x >= ps.upper_bound_x && ar.y >= ps.upper_bound_y))
            {
                return std::nullopt;
            }

            if (handler.skippable(ar))
            {
                continue;
            }

            // mutually exclusive access to the result aspect ratio
            {
                std::lock_guard<std::mutex> guard(rar_mutex);

                // a result is available already
                if (result_aspect_ratio)
                {
                    // stop working if its area is smaller or equal to the one currently at hand
                    if (area(*result_aspect_ratio) <= area(ar))
                    {
                        return std::nullopt;
                    }
                }
            }

            // update aspect ratio in the thread_info list and the handler
            (*ti_list)[t_num].worker_aspect_ratio = ar;
            handler.update(ar);

            try
            {
                mockturtle::stopwatch stop{pst.time_total};

                if (handler.is_satisfiable())  // found a layout
                {
                    // mutually exclusive access to the result_aspect_ratio
                    {
                        std::lock_guard<std::mutex> guard(rar_mutex);

                        // update the result_aspect_ratio if there is none
                        if (!result_aspect_ratio)
                        {
                            result_aspect_ratio = ar;
                        }
                        else  // or if the own one is smaller
                        {
                            if (area(*result_aspect_ratio) > area(ar))
                            {
                                result_aspect_ratio = ar;
                            }
                            else
                            {
                                return std::nullopt;
                            }
                        }
                    }

                    // interrupt other threads that are working on higher aspect ratios
                    for (const auto& ti : *ti_list)
                    {
                        if (area(ar) <= area(ti.worker_aspect_ratio))
                        {
                            ti.ctx->interrupt();
                        }
                    }

                    return layout;
                }
                // no layout with this aspect ratio possible
                // One could assume that interrupting other threads that are working on real smaller (not bigger
                // in any dimension) layouts could be beneficial here. However, testing revealed that this code
                // was hardly ever triggered and if it was, it impacted performance negatively because no solver
                // state could be stored that could positively influence performance of later SMT calls

                handler.store_solver_state(ar);
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
     * Launches params.num_threads threads and evaluates their return statements.
     *
     * @return A placed and routed gate-level layout or std::nullopt in case a timeout or an upper bound was reached.
     */
    [[nodiscard]] std::optional<Lyt> run_asynchronously()
    {
        std::cout << "You have called an unstable beta feature that might crash." << std::endl;

        Lyt layout{{}, scheme};

        {
            mockturtle::stopwatch stop{pst.time_total};

            using fut_layout = std::future<std::optional<Lyt>>;
            std::vector<fut_layout> fut(ps.num_threads);

            const auto ti_list = std::make_shared<std::vector<thread_info>>(ps.num_threads);

#if (PROGRESS_BARS)
            mockturtle::progress_bar thread_bar("[i] examining layout aspect ratios using {} threads");
            thread_bar(ps.num_threads);

            auto post_toggle = false;

            mockturtle::progress_bar post_bar(
                "[i] some layout has been found; waiting for threads examining smaller aspect ratios to terminate");
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

            if (result_aspect_ratio)
            {
                const auto result_ar_val = *result_aspect_ratio;
                // extract the layout from the futures
                for (auto& f : fut)
                {
                    if (auto l = f.get(); l.has_value())
                    {
                        // in case multiple returned, get the actual winner
                        if ((*l).x() == result_ar_val.x && (*l).y() == result_ar_val.y)
                        {
                            layout = *l;
                        }
                    }
                }
            }
        }

        if (result_aspect_ratio.has_value())
        {
            // statistical information
            pst.x_size    = layout.x() + 1;
            pst.y_size    = layout.y() + 1;
            pst.num_gates = layout.num_gates();
            pst.num_wires = layout.num_wires();

            return layout;
        }

        return std::nullopt;
    }
    /**
     * Does the same as explore_asynchronously but without thread synchronization overhead.
     *
     * @return A placed and routed gate-level layout or std::nullopt in case a timeout or an upper bound was reached.
     */
    [[nodiscard]] std::optional<Lyt> run_synchronously() noexcept
    {
        Lyt layout{{}, scheme};

        smt_handler handler{std::make_shared<z3::context>(), layout, *ntk, ps, black_list};

        const auto upper_bound = std::min(static_cast<uint64_t>(ps.upper_bound_area),
                                          static_cast<uint64_t>(ps.upper_bound_x * ps.upper_bound_y));

        for (; ari <= upper_bound; ++ari)  // <= to prevent overflow
        {

#if (PROGRESS_BARS)
            mockturtle::progress_bar bar("[i] examining layout aspect ratios: {:>2} × {:<2}");
#endif

            auto ar = *ari;

            // log the examination of a new aspect ratio
            pst.num_aspect_ratios++;

            if (handler.skippable(ar))
            {
                continue;
            }

#if (PROGRESS_BARS)
            bar(ar.x + 1, ar.y + 1);
#endif

            handler.update(ar);

            try
            {
                const auto sat =
                    mockturtle::call_with_stopwatch(pst.time_total, [&handler] { return handler.is_satisfiable(); });

                if (sat)
                {
                    // statistical information
                    pst.x_size    = layout.x() + 1;
                    pst.y_size    = layout.y() + 1;
                    pst.num_gates = layout.num_gates();
                    pst.num_wires = layout.num_wires();

                    return layout;
                }

                handler.store_solver_state(ar);

                update_timeout(handler, pst.time_total);
            }
            catch (const z3::exception&)
            {
                return std::nullopt;
            }
        }

        return std::nullopt;
    }
};

}  // namespace detail

/**
 * An exact placement & routing approach using SMT solving as originally proposed in \"An Exact Method for Design
 * Exploration of Quantum-dot Cellular Automata\" by M. Walter, R. Wille, D. Große, F. Sill Torres, and R. Drechsler in
 * DATE 2018. A more extensive description can be found in \"Design Automation for Field-coupled Nanotechnologies\" by
 * M. Walter, R. Wille, F. Sill Torres, and R. Drechsler published by Springer Nature in 2022.
 *
 * Via incremental SMT calls, an optimal gate-level layout for a given logic network will be found under constraints.
 * Starting with \f$n\f$ tiles, where \f$n\f$ is the number of logic network nodes, each possible layout aspect
 * ratio will be examined by factorization and tested for routability with the SMT solver Z3. When no upper bound is
 * given, this approach will run until it finds a solution to the placement & routing problem instance.
 *
 * Note that there a combinations of constraints for which no valid solution under the given parameters exist for the
 * given logic network. Such combinations cannot be detected automatically. It is, thus, recommended to always set a
 * timeout. Recommended settings include the use of I/O pins located at the layout borders for better integration. Most
 * networks are not realizable without crossings enabled. Specifying a regular clocking scheme SIGNIFICANTLY speeds up
 * the process. 2DDWave allows for the strictest constraints and, thereby, finds a solution the quickest. However, for
 * high input degree networks, no valid solution exists when border I/Os are to be used unless global synchronization is
 * disabled. Generally, solutions are found the fastest with the following settings: Crossings enabled,
 * de-synchronization enabled, and 2DDWave clocking given. Multi-threading can sometimes speed up the process,
 * especially for large networks. Note that the more threads are being used, the less information can be shared across
 * the individual solver runs which destroys the benefits of incremental solving and thereby, comparatively, slows down
 * each run.
 *
 * The SMT instance works with a single layer of variables even though it is possible to allow crossings in the
 * solution. The reduced number of variables saves a considerable amount of runtime. That's why
 * `layout.foreach_ground_tile()` is used even though the model will be mapped to a 3-dimensional layout afterwards.
 * Generally, the algorithm incorporates quite a few encoding optimizations to be as performant as possible on various
 * layout topologies and clocking schemes.
 *
 * The approach applies to any data structures that implement the necessary functions to comply with `is_network_type`
 * and `is_gate_level_layout`, respectively. It is, thereby, mostly technology-independent but can make certain
 * assumptions if needed, for instance for ToPoliNano-compliant circuits.
 *
 * This approach requires the Z3 SMT solver to be installed on the system. Due to this circumstance, it is excluded from
 * (CLI) compilation by default. To enable it, pass `-DFICTION_Z3=ON` to the cmake call.
 *
 * May throw a high_degree_fanin_exception if `ntk` contains any node with a fan-in too large to be handled by the
 * specified clocking scheme.
 *
 * @tparam Lyt Desired gate-level layout type.
 * @tparam Ntk Network type that acts as specification.
 * @param ntk The network that is to place and route.
 * @param ps Parameters.
 * @param pst Statistics.
 * @return A gate-level layout of type `Lyt` that implements `ntk` as an FCN circuit if one is found under the given
 * parameters; `std::nullopt`, otherwise.
 */
template <typename Lyt, typename Ntk>
std::optional<Lyt> exact(const Ntk& ntk, const exact_physical_design_params& ps = {},
                         exact_physical_design_stats* pst = nullptr)
{
    static_assert(is_gate_level_layout_v<Lyt>, "Lyt is not a gate-level layout");
    static_assert(is_tile_based_layout_v<Lyt>, "Lyt is not a tile-based layout");
    static_assert(mockturtle::is_network_type_v<Ntk>,
                  "Ntk is not a network type");  // Ntk is being converted to a technology_network anyway, therefore,
                                                 // this is the only relevant check here

    const auto clocking_scheme = get_clocking_scheme<Lyt>(ps.scheme);

    if (!clocking_scheme.has_value())
    {
        throw unsupported_clocking_scheme_exception();
    }
    // check for input degree
    else if (has_high_degree_fanin_nodes(ntk, clocking_scheme->max_in_degree))
    {
        throw high_degree_fanin_exception();
    }

    if constexpr (!fiction::has_foreach_adjacent_opposite_tiles_v<Lyt>)
    {
        if (ps.straight_inverters)
        {
            std::cout << "[w] Lyt does not implement the foreach_adjacent_opposite_tiles function; straight inverters "
                         "cannot be guaranteed"
                      << std::endl;
        }
    }
    if constexpr (!fiction::has_synchronization_elements_v<Lyt>)
    {
        if (ps.synchronization_elements)
        {
            std::cout << "[w] Lyt does not support synchronization elements; not using them" << std::endl;
        }
    }

    mockturtle::names_view<technology_network> intermediate_ntk{
        fanout_substitution<mockturtle::names_view<technology_network>>(
            ntk, {fanout_substitution_params::substitution_strategy::BREADTH, clocking_scheme->max_out_degree, 1ul})};

    exact_physical_design_stats st{};

    detail::exact_impl<Lyt> p{intermediate_ntk, ps, st};

    auto result = p.run();

    if (pst)
    {
        *pst = st;
    }

    return result;
}
/**
 * The same as `exact` but with a black list of tiles that are not allowed to be used to a specified set of Boolean
 * functions and their orientations. For example, a black list could be used to exclude the use of a tile only for an
 * AND gate of a certain rotation, but not for other gates. This is useful if a tile is known to be faulty or if it is
 * known to be used for a different purpose.
 *
 * @tparam Lyt Desired gate-level layout type.
 * @tparam Ntk Network type that acts as specification.
 * @param ntk The network that is to place and route.
 * @param black_list The black list of tiles and their gate orientations.
 * @param ps Parameters.
 * @param pst Statistics.
 * @return A gate-level layout of type `Lyt` that implements `ntk` as an FCN circuit if one is found under the given
 * parameters; `std::nullopt`, otherwise.
 */
template <typename Lyt, typename Ntk>
std::optional<Lyt> exact_with_blacklist(const Ntk& ntk, const surface_black_list<Lyt, port_direction>& black_list,
                                        exact_physical_design_params ps  = {},
                                        exact_physical_design_stats* pst = nullptr)
{
    static_assert(is_gate_level_layout_v<Lyt>, "Lyt is not a gate-level layout");
    static_assert(is_tile_based_layout_v<Lyt>, "Lyt is not a tile-based layout");
    static_assert(mockturtle::is_network_type_v<Ntk>,
                  "Ntk is not a network type");  // Ntk is being converted to a technology_network anyway, therefore,
                                                 // this is the only relevant check here

    const auto clocking_scheme = get_clocking_scheme<Lyt>(ps.scheme);

    if (!clocking_scheme.has_value())
    {
        throw unsupported_clocking_scheme_exception();
    }
    // check for input degree
    else if (has_high_degree_fanin_nodes(ntk, clocking_scheme->max_in_degree))
    {
        throw high_degree_fanin_exception();
    }

    if constexpr (!fiction::has_foreach_adjacent_opposite_tiles_v<Lyt>)
    {
        if (ps.straight_inverters)
        {
            std::cout << "[w] Lyt does not implement the foreach_adjacent_opposite_tiles function; straight inverters "
                         "cannot be guaranteed"
                      << std::endl;
        }
    }
    if constexpr (!fiction::has_synchronization_elements_v<Lyt>)
    {
        if (ps.synchronization_elements)
        {
            std::cout << "[w] Lyt does not support synchronization elements; not using them" << std::endl;
        }
    }

    mockturtle::names_view<technology_network> intermediate_ntk{
        fanout_substitution<mockturtle::names_view<technology_network>>(
            ntk, {fanout_substitution_params::substitution_strategy::BREADTH, clocking_scheme->max_out_degree, 1ul})};

    exact_physical_design_stats st{};

    detail::exact_impl<Lyt> p{intermediate_ntk, ps, st, black_list};

    auto result = p.run();

    if (pst)
    {
        *pst = st;
    }

    return result;
}

}  // namespace fiction

#endif  // FICTION_Z3_SOLVER

#endif  // FICTION_EXACT_HPP
