//
// Created by marcel on 07.03.23.
//

#if (FICTION_Z3_SOLVER)

#ifndef FICTION_EXACT_SIM_HPP
#define FICTION_EXACT_SIM_HPP

#include "fiction/algorithms/simulation/sidb/sidb_simulation_parameters.hpp"
#include "fiction/technology/charge_distribution_surface.hpp"
#include "fiction/traits.hpp"

#include <mockturtle/utils/stopwatch.hpp>
#include <z3++.h>

#include <cstdint>
#include <vector>

namespace fiction
{

struct exact_sidb_simulation_parameters
{
    /**
     * General parameters for the simulation of the physical SiDB system.
     */
    sidb_simulation_parameters phys_params{};
    /**
     * Flag to indicate whether all physically valid layouts should be enumerated. If set to `false`, only the one with
     * the minimum system energy is returned.
     */
    bool enumerate_all_physically_valid_layouts{false};
    /**
     * Flag to indicate whether the population stability should be computed outside the solver. If set to `true`, the
     * check is performed combinatorically on results yielded by the solver. Should a layout be found to be unstable,
     * this configuration is excluded from the search space and the solver is called again.
     */
    bool compute_population_stability_outside_the_solver{false};
    /**
     * Flag to indicate whether the configuration stability should be computed outside the solver. If set to `true`, the
     * check is performed combinatorically on results yielded by the solver. Should a layout be found to be unstable,
     * this configuration is excluded from the search space and the solver is called again.
     */
    bool compute_configuration_stability_outside_the_solver{false};
};

template <typename Lyt>
struct exact_sidb_simulation_stats
{
    /**
     * Total simulation runtime.
     */
    mockturtle::stopwatch<>::duration time_total{};
    /**
     * All physically valid charge layouts.
     */
    std::vector<charge_distribution_surface<Lyt>> valid_lyts{};
};

namespace detail
{

template <typename Lyt>
class exact_ground_state_simulation_impl
{
  public:
    exact_ground_state_simulation_impl(const Lyt& lyt, const exact_sidb_simulation_parameters& p,
                                       exact_sidb_simulation_stats<Lyt>& st) noexcept :
            charge_lyt{lyt},
            params{p},
            stats{st}
    {}

  private:
    /**
     * The charge distribution surface of the input layout.
     */
    charge_distribution_surface<Lyt> charge_lyt;
    /**
     * The simulation parameters.
     */
    const exact_sidb_simulation_parameters& params;
    /**
     * The simulation statistics.
     */
    exact_sidb_simulation_stats<Lyt>& stats;

    /**
     * The Z3 context.
     */
    z3::context ctx;
};

}  // namespace detail

template <typename Lyt>
void exact_ground_state_simulation(const Lyt& lyt, const exact_sidb_simulation_params& ps = {},
                                   exact_sidb_simulation_stats<Lyt>* pst = nullptr) noexcept
{
    static_assert(is_cell_level_layout_v<Lyt>, "Lyt is not a cell-level layout");
    static_assert(has_sidb_tech_v<Lyt>, "Lyt is not an SiDB layout");

    exact_sidb_simulation_stats<Lyt> st{};

    exact_ground_state_simulation_impl<Lyt> impl{lyt, ps, st}.run();

    if (pst)
    {
        *pst = st;
    }
}

}  // namespace fiction

#endif  // FICTION_EXACT_SIM_HPP

#endif  // FICTION_Z3_SOLVER