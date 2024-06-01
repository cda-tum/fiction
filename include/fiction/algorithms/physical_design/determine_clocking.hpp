//
// Created by marcel on 20.01.24.
//

#ifndef FICTION_DETERMINE_CLOCKING_HPP
#define FICTION_DETERMINE_CLOCKING_HPP

#include "fiction/traits.hpp"

#include <bill/sat/cardinality.hpp>
#include <bill/sat/interface/common.hpp>
#include <bill/sat/interface/types.hpp>
#include <bill/sat/solver.hpp>
#include <bill/sat/tseytin.hpp>
#include <fmt/format.h>
#include <mockturtle/traits.hpp>
#include <mockturtle/utils/cost_functions.hpp>
#include <mockturtle/utils/stopwatch.hpp>
#include <mockturtle/views/depth_view.hpp>

#include <algorithm>
#include <functional>
#include <iostream>
#include <ostream>
#include <unordered_map>
#include <utility>
#include <vector>

namespace fiction
{

/**
 * Parameters for the `determine_clocking` algorithm.
 */
struct determine_clocking_params
{
    /**
     * The SAT solver to use.
     */
    bill::solvers sat_engine = bill::solvers::bsat2;
};
/**
 * Statistics for the `determine_clocking` algorithm.
 */
struct determine_clocking_stats
{
    /**
     * Total runtime.
     */
    mockturtle::stopwatch<>::duration time_total{0};
    /**
     * Reports the statistics to the given output stream.
     *
     * @param out The output stream to report to.
     */
    void report(std::ostream& out = std::cout) const
    {
        out << fmt::format("[i] total time = {:.2f} secs\n", mockturtle::to_seconds(time_total));
    }
};

namespace detail
{

template <typename Lyt, bill::solvers SolverType = bill::solvers::ghack>
class sat_clocking_handler
{
  public:
    /**
     * Default constructor.
     */
    explicit sat_clocking_handler(Lyt& lyt) : layout{lyt}, number_of_clocks{layout.num_clocks()}
    {
        // for each non-empty tile
        layout.foreach_node(
            [this](const auto& n)
            {
                // skip constants
                if (layout.is_constant(n))
                {
                    return;
                }

                const auto t = layout.get_tile(n);

                // for each possible clock number
                for (typename Lyt::clock_number_t clk = 0; clk < number_of_clocks; ++clk)
                {
                    variables[{t, clk}] = solver.add_variable();
                }
            });
    }
    /**
     * Determines clock numbers for the layout.
     *
     * Constructs a SAT instance and passes it to a solver to find a valid clocking scheme.
     *
     * @return `true` iff a valid clocking scheme could be found.
     */
    bool determine_clocks() noexcept
    {
        at_least_one_clock_number_per_tile();
        at_most_one_clock_number_per_tile();
        exclude_clock_assignments_that_violate_information_flow();
        ensure_same_clock_number_on_crossing_tiles();
        symmetry_breaking();

        // pass to the solver
        if (const auto sat_result = solver.solve(); sat_result == bill::result::states::satisfiable)
        {
            // extract model and assign clock numbers
            assign_clock_numbers(solver.get_model().model());
            return true;
        }

        // SAT instance was not satisfiable
        return false;
    }

  private:
    /**
     * The layout to clock.
     */
    Lyt& layout;
    /**
     * Number of clocks in layout's clocking scheme.
     */
    const typename Lyt::clock_number_t number_of_clocks;
    /**
     * The solver used to find a solution to the clocking problem.
     */
    bill::solver<SolverType> solver{};
    /**
     * Alias for a tile-clock number pair.
     */
    using tile_clock_number = std::pair<tile<Lyt>, typename Lyt::clock_number_t>;
    /**
     * Stores all variables.
     */
    std::unordered_map<tile_clock_number, bill::var_type> variables{};

    /**
     * Adds constraints to the solver that enforce the assignment of at least one clock number per tile.
     */
    void at_least_one_clock_number_per_tile() noexcept
    {
        // for each non-empty tile
        layout.foreach_node(
            [this](const auto& n)
            {
                if (layout.is_constant(n))
                {
                    return;
                }

                const auto t = layout.get_tile(n);

                std::vector<bill::var_type> tc{};
                tc.reserve(number_of_clocks);

                // for each possible clock number
                for (typename Lyt::clock_number_t clk = 0; clk < number_of_clocks; ++clk)
                {
                    tc.push_back(variables[{t, clk}]);
                }

                bill::at_least_one(tc, solver);
            });
    }
    /**
     * Adds constraints to the solver that enforce the assignment of at most one clock number per tile.
     */
    void at_most_one_clock_number_per_tile() noexcept
    {
        // for each pair of clock numbers
        for (typename Lyt::clock_number_t c1 = 0; c1 < number_of_clocks; ++c1)
        {
            // use an optimization here: c2 > c1 instead of c2 != c1 to save half the clauses
            for (typename Lyt::clock_number_t c2 = c1 + 1; c2 < number_of_clocks; ++c2)
            {
                // for each non-empty tile
                layout.foreach_node(
                    [this, &c1, &c2](const auto& n)
                    {
                        if (layout.is_constant(n))
                        {
                            return;
                        }

                        const auto t = layout.get_tile(n);

                        // not tile has clock 1 OR not tile has clock 2
                        solver.add_clause({{bill::lit_type{variables[{t, c1}], bill::negative_polarity},
                                            bill::lit_type{variables[{t, c2}], bill::negative_polarity}}});
                    });
            }
        }
    }
    /**
     * Adds constraints to the solver that exclude the assignment of non-adjacently clocked tiles.
     */
    void exclude_clock_assignments_that_violate_information_flow() noexcept
    {
        // for each non-empty tile
        layout.foreach_node(
            [this](const auto& n)
            {
                if (layout.is_constant(n))
                {
                    return;
                }

                const auto t1 = layout.get_tile(n);

                // for each of t's predecessors (disregarding clocking)
                const auto incoming_tiles = layout.template incoming_data_flow<false>(t1);
                std::for_each(
                    incoming_tiles.cbegin(), incoming_tiles.cend(),
                    [this, &t1](const auto& t2)
                    {
                        // for each combination of possible clock numbers
                        for (typename Lyt::clock_number_t c1 = 0; c1 < number_of_clocks; ++c1)
                        {
                            for (typename Lyt::clock_number_t c2 = 0; c2 < number_of_clocks; ++c2)
                            {
                                // if c2 is not c1's incoming clock number
                                if (!(static_cast<typename Lyt::clock_number_t>((c2 + typename Lyt::clock_number_t{1}) %
                                                                                number_of_clocks) == c1))
                                {
                                    // not tile t1 has clock c1 OR not tile t2 has clock c2
                                    solver.add_clause({{bill::lit_type{variables[{t1, c1}], bill::negative_polarity},
                                                        bill::lit_type{variables[{t2, c2}], bill::negative_polarity}}});
                                }
                            }
                        }
                    });
            });
    }
    /**
     * Adds constraints to the solver that ensure the assignment of the same clock number to crossing tiles.
     */
    void ensure_same_clock_number_on_crossing_tiles() noexcept
    {
        // for each crossing wire
        layout.foreach_wire(
            [this](const auto& w)
            {
                const auto t = layout.get_tile(w);

                if (layout.is_ground_layer(t))
                {
                    return;
                }

                // fetch corresponding tile in ground layer
                const auto ground_t = layout.below(t);

                // for each possible clock number
                for (typename Lyt::clock_number_t clk = 0; clk < number_of_clocks; ++clk)
                {
                    // ensure that the clock number of both tiles is identical
                    solver.add_clause(
                        bill::add_tseytin_equals(solver, variables[{t, clk}], variables[{ground_t, clk}]));
                }
            });
    }
    /**
     * Adds constraints to the solver that help to speed up the solving process by breaking symmetries in the solution
     * space.
     */
    void symmetry_breaking() noexcept
    {
        // compute the critical path of the layout (unit cost for all nodes, no complemented edges, consider PI cost)
        mockturtle::depth_view depth_layout{layout, mockturtle::unit_cost<Lyt>(), {false, true}};

        const std::function<void(const mockturtle::node<Lyt>& n)> recurse =
            [this, &depth_layout, &recurse, clk = 0](const auto& n) mutable
        {
            const auto t = depth_layout.get_tile(n);

            // pre-assign tile t to clock number clk
            solver.add_clause(variables[{t, clk++ % number_of_clocks}]);

            depth_layout.foreach_fanout(n,
                                        [&depth_layout, &recurse](auto const& fon)
                                        {
                                            if (depth_layout.is_on_critical_path(fon))
                                            {
                                                recurse(fon);
                                                return false;  // terminate
                                            }

                                            return true;  // continue until fan-out on critical path is found
                                        });
        };

        // only for the PI on the critical path
        depth_layout.foreach_pi(
            [&depth_layout, &recurse](const auto& pi)
            {
                if (depth_layout.is_on_critical_path(pi))
                {
                    recurse(pi);
                    return false;  // terminate
                }

                return true;  // continue until PI on critical path is found
            });
    }
    /**
     * Assigns clock numbers to the layout based on the provided model.
     *
     * @param model The model to extract the clocking scheme from.
     */
    void assign_clock_numbers(const bill::result::model_type& model) noexcept
    {
        // for each non-empty tile
        layout.foreach_node(
            [this, &model](const auto& n)
            {
                if (layout.is_constant(n))
                {
                    return;
                }

                const auto t = layout.get_tile(n);

                // for each possible clock number
                for (typename Lyt::clock_number_t clk = 0; clk < number_of_clocks; ++clk)
                {
                    // if tile t is clocked with clock number clk
                    if (model.at(variables.at({t, clk})) == bill::lbool_type::true_)
                    {
                        layout.assign_clock_number(t, clk);
                        layout.assign_clock_number(layout.above(t), clk);
                        layout.assign_clock_number(layout.below(t), clk);
                    }
                }
            });
    }
};

template <typename Lyt>
class determine_clocking_impl
{
  public:
    determine_clocking_impl(Lyt& lyt, const determine_clocking_params& p, determine_clocking_stats& st) :
            layout{lyt},
            params{p},
            stats{st}
    {}

    bool run()
    {
        // measure run time
        mockturtle::stopwatch stop{stats.time_total};

        if (layout.is_empty())
        {
            return true;
        }

        switch (params.sat_engine)
        {
            case bill::solvers::ghack:
            {
                return sat_clocking_handler<Lyt, bill::solvers::ghack>{layout}.determine_clocks();
            }
            case bill::solvers::glucose_41:
            {
                return sat_clocking_handler<Lyt, bill::solvers::glucose_41>{layout}.determine_clocks();
            }
            case bill::solvers::bsat2:
            {
                return sat_clocking_handler<Lyt, bill::solvers::bsat2>{layout}.determine_clocks();
            }
#if !defined(BILL_WINDOWS_PLATFORM)
            case bill::solvers::maple:
            {
                return sat_clocking_handler<Lyt, bill::solvers::maple>{layout}.determine_clocks();
            }
            case bill::solvers::bmcg:
            {
                return sat_clocking_handler<Lyt, bill::solvers::bmcg>{layout}.determine_clocks();
            }
#endif
            default:
            {
                return sat_clocking_handler<Lyt>{layout}.determine_clocks();
            }
        }
    }

  private:
    /**
     * The layout to assign clock numbers to.
     */
    Lyt& layout;
    /**
     * Parameters.
     */
    determine_clocking_params params;
    /**
     * Statistics.
     */
    determine_clocking_stats& stats;
};

}  // namespace detail

/**
 * Determines clock numbers for the given gate-level layout. This algorithm parses the layout's gate and wire
 * connections, disregarding any existing clocking information, and constructs a SAT instance to find a valid clock
 * number assignment under which the information flow is respected. It then assigns these clock numbers as an irregular
 * clock map to the given layout via the `assign_clock_number` function, overriding any existing clocking scheme.
 *
 * If no valid clock number assignment exists for `lyt`, this function returns `false` and does not modify `lyt`.
 *
 * @tparam Lyt Gate-level layout type.
 * @param lyt The gate-level layout to assign clock numbers to.
 * @param params Parameters.
 * @param stats Statistics.
 * @return `true` iff `lyt` could be successfully clocked via a valid clock number assignment.
 */
template <typename Lyt>
bool determine_clocking(Lyt& lyt, const determine_clocking_params& params = {},
                        determine_clocking_stats* stats = nullptr)
{
    static_assert(is_gate_level_layout_v<Lyt>, "Lyt is not a gate-level layout");

    determine_clocking_stats             st{};
    detail::determine_clocking_impl<Lyt> p{lyt, params, st};

    const auto result = p.run();

    if (stats)
    {
        *stats = st;
    }

    return result;
}

}  // namespace fiction

#endif  // FICTION_DETERMINE_CLOCKING_HPP
