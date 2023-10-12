//
// Created by marcel on 13.01.23.
//

#ifndef FICTION_SA_PLACEMENT_HPP
#define FICTION_SA_PLACEMENT_HPP

#include "fiction/algorithms/network_transformation/fanout_substitution.hpp"
#include "fiction/algorithms/optimization/simulated_annealing.hpp"
#include "fiction/io/print_layout.hpp"
#include "fiction/layouts/clocking_scheme.hpp"
#include "fiction/layouts/placement_layout.hpp"
#include "fiction/networks/technology_network.hpp"
#include "fiction/traits.hpp"
#include "fiction/utils/network_utils.hpp"

#include <mockturtle/utils/stopwatch.hpp>

#include <cstdint>
#include <cstdlib>
#include <string_view>

namespace fiction
{

/**
 * Parameters for the Simulated Annealing placer.
 */
struct sa_placement_params
{
    /**
     * Layout x-size.
     */
    std::size_t layout_size_x{10};
    /**
     * Layout y-size.
     */
    std::size_t layout_size_y{10};
    /**
     * Clocking scheme name.
     */
    std::string_view scheme{"2DDWave"};

    /**
     * Temperature schedule type.
     */
    temperature_schedule schedule{temperature_schedule::LINEAR};
    /**
     * Initial temperature.
     */
    double initial_temperature{1000};
    /**
     * Final temperature.
     */
    double final_temperature{1};
    /**
     * Number of iterations per temperature.
     */
    std::size_t number_of_cycles{10};
    /**
     * Number of parallel runs to pick the overall best result from.
     */
    std::size_t number_of_instances{100};
};
/**
 * Statistics for the Simulated Annealing placer.
 */
struct sa_placement_stats
{
    /**
     * Cost of the initial placement.
     */
    std::size_t initial_cost;
    /**
     * Cost of the final placement after optimization.
     */
    std::size_t final_cost;
    /**
     * Runtime measurement.
     */
    mockturtle::stopwatch<>::duration time_total{0};
};

namespace detail
{

template <typename Lyt>
class sa_placing_impl
{
  public:
    sa_placing_impl(const mockturtle::names_view<technology_network>& spec, const sa_placement_params& p,
                    sa_placement_stats& st) :
            ntk{spec},
            params{p},
            stats{st}
    {}

    Lyt run()
    {
        mockturtle::stopwatch stop{stats.time_total};

        //        const auto [result, cost] = multi_simulated_annealing(
        //            ps.initial_temperature, ps.final_temperature, ps.number_of_cycles, ps.number_of_instances,
        //            random_placement, placement_cost, linear_temperature_schedule, next_placement);

        assert(area(aspect_ratio<Lyt>{params.layout_size_x, params.layout_size_y}) >= ntk.num_gates() &&
               "provided layout dimensions are too small to host all gates from the given network");

        Lyt layout_sketch{aspect_ratio<Lyt>{params.layout_size_x, params.layout_size_y},
                          *get_clocking_scheme<Lyt>(params.scheme)};

        return perform_placement(layout_sketch);
    }

  private:
    /**
     * The network whose nodes are to be placed.
     */
    const mockturtle::names_view<technology_network> ntk;
    /**
     * Parameters.
     */
    const sa_placement_params& params;
    /**
     * Statistics.
     */
    sa_placement_stats& stats;

    /**
     *
     * @param layout_sketch
     * @return
     */
    [[nodiscard]] Lyt perform_placement(const Lyt& layout_sketch) noexcept
    {
        placement_layout initial_placement{layout_sketch, ntk};
        initial_placement.initialize_random_placement();

        const auto initial_layout = initial_placement.apply_placement();
        print_layout(initial_layout);
        std::cout << fmt::format("Initial cost: {}", initial_placement.net_cost()) << std::endl;

        // store initial cost
        stats.initial_cost = initial_placement.net_cost();

        const auto temperature_schedule = params.schedule == temperature_schedule::LINEAR ?
                                              linear_temperature_schedule :
                                              geometric_temperature_schedule;

        auto [optimized_placement, optimized_net_cost] = simulated_annealing<decltype(initial_placement)>(
            initial_placement, params.initial_temperature, params.final_temperature, params.number_of_cycles,
            [](const auto& lyt) { return static_cast<int64_t>(lyt.net_cost()); }, temperature_schedule,
            [](auto lyt)
            {
                lyt.swap_random_node_and_random_tile();
                return lyt;
            });

        stats.final_cost = static_cast<std::size_t>(optimized_net_cost);

        const auto final_layout = optimized_placement.apply_placement();
        print_layout(final_layout);
        std::cout << fmt::format("Final cost: {}", optimized_placement.net_cost()) << std::endl;


        std::cout << fmt::format("Initial costs: {}, final costs: {}", stats.initial_cost, stats.final_cost) << std::endl;

        return final_layout;
    }
};

}  // namespace detail

template <typename Lyt, typename Ntk>
[[nodiscard]] Lyt sa_placement(const Ntk& ntk, const sa_placement_params& ps = {}, sa_placement_stats* pst = nullptr)
{
    static_assert(mockturtle::is_network_type_v<Ntk>, "Ntk is not a network type");
    static_assert(is_gate_level_layout_v<Lyt>, "Lyt is not a gate-level layout");

    // check for supported clocking scheme
    const auto scheme = get_clocking_scheme<Lyt>(ps.scheme);
    if (!scheme.has_value())
    {
        throw unsupported_clocking_scheme_exception(ps.scheme);
    }
    // check for input degree
    if (has_high_degree_fanin_nodes(ntk, scheme->max_in_degree))
    {
        throw high_degree_fanin_exception();
    }

    // transform the input network into a technology network and substitute high-degree fanouts
    mockturtle::names_view<technology_network> intermediate_ntk{
        fanout_substitution<mockturtle::names_view<technology_network>>(
            ntk, {fanout_substitution_params::substitution_strategy::BREADTH, scheme->max_out_degree, 1ul})};
    // substitute PO signals with PO nodes
    intermediate_ntk.substitute_po_signals();

    sa_placement_stats st{};

    detail::sa_placing_impl<Lyt> p{intermediate_ntk, ps, st};

    const auto result = p.run();

    if (pst)
    {
        *pst = st;
    }

    return result;
}

}  // namespace fiction

#endif  // FICTION_SA_PLACEMENT_HPP
