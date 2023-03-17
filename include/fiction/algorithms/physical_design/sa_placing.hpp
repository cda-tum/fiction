//
// Created by marcel on 13.01.23.
//

#ifndef FICTION_SA_PLACING_HPP
#define FICTION_SA_PLACING_HPP

#include "fiction/algorithms/optimization/simulated_annealing.hpp"
#include "fiction/layouts/clocking_scheme.hpp"
#include "fiction/layouts/placement_layout.hpp"
#include "fiction/traits.hpp"
#include "fiction/utils/network_utils.hpp"

#include <mockturtle/utils/stopwatch.hpp>

#include <cstdint>
#include <random>
#include <string>

namespace fiction
{

/**
 * Parameters for the Simulated Annealing placer.
 */
struct sa_placing_params
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
    std::string scheme{"2DDWave"};

    /**
     * Initial temperature.
     */
    std::size_t initial_temperature{1000};
    /**
     * Final temperature.
     */
    std::size_t final_temperature{1};
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
struct sa_placing_stats
{
    /**
     * Runtime measurement.
     */
    mockturtle::stopwatch<>::duration time_total{0};
};

namespace detail
{

template <typename Lyt, typename Ntk>
class sa_placing_impl
{
  public:
    sa_placing_impl(const Ntk& ntk, const sa_placing_params& p, sa_placing_stats& s) :
            ntk{ntk},
            ps{p},
            pst{s},
            scheme{*get_clocking_scheme<Lyt>(ps.scheme)}
    {}

    Lyt run()
    {
        mockturtle::stopwatch stop{pst.time_total};

        const auto [result, cost] = multi_simulated_annealing(
            ps.initial_temperature, ps.final_temperature, ps.number_of_cycles, ps.number_of_instances, random_placement,
            placement_cost, linear_temperature_schedule, next_placement);

        // TODO check if layout is large enough
    }

  private:
    /**
     * The network whose nodes are to be placed.
     */
    const Ntk& ntk;
    /**
     * Parameters.
     */
    const sa_placing_params& ps;
    /**
     * Statistics.
     */
    sa_placing_stats& pst;

    /**
     * Clocking scheme.
     */
    const clocking_scheme<Lyt>& scheme;

    /**
     * Create a layout of the specified size with the specified clocking scheme and randomly place the gates of the
     * network.
     */
    Lyt random_placement()
    {
        static std::mt19937_64 generator(std::random_device{}());

        std::uniform_int_distribution<uint64_t> distribution_x(0, ps.layout_size_x);
        std::uniform_int_distribution<uint64_t> distribution_y(0, ps.layout_size_y);

        Lyt lyt{ps.layout_size_x, ps.layout_size_y, scheme};

        ntk.foreach_node(
            [&](auto const& n)
            {
                if (ntk.is_constant(n))
                {
                    return;
                }

                for (;;)
                {
                    coordinate<Lyt> coord{distribution_x(generator), distribution_y(generator)};

                    if (lyt.is_empty_tile(coord))
                    {
                        lyt.place_gate(n, coord);

                        break;

                        // TODO we need a placer_layout
                    }
                }
            });

        return lyt;
    }
};

}  // namespace detail

template <typename Lyt, typename Ntk>
[[nodiscard]] Lyt sa_placing(const Ntk& ntk, const sa_placing_params& ps = {}, sa_placing_stats* pst = nullptr)
{
    static_assert(mockturtle::is_network_type_v<Ntk>, "Ntk is not a network type");
    static_assert(is_gate_level_layout_v<Lyt>, "Lyt is not a gate-level layout");

    if (const auto clocking_scheme = get_clocking_scheme<Lyt>(ps.scheme); !clocking_scheme.has_value())
    {
        // TODO throw unsupported_clocking_scheme_exception();
    }
    // check for input degree
    else if (has_high_degree_fanin_nodes(ntk, clocking_scheme->max_in_degree))
    {
        throw high_degree_fanin_exception();
    }

    sa_placing_stats st{};

    detail::sa_placing_impl<Lyt, Ntk> p{ntk, ps, st};

    const auto result = p.run();

    if (pst)
    {
        *pst = st;
    }

    return result;
}

}  // namespace fiction

#endif  // FICTION_SA_PLACING_HPP
