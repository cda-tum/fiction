//
// Created by marcel on 26.02.26.
//

#ifndef FICTION_CLOCK_EMULATOR_HPP
#define FICTION_CLOCK_EMULATOR_HPP

#if (FICTION_ALGLIB_ENABLED)
#include "fiction/algorithms/simulation/sidb/clustercomplete.hpp"
#endif  // FICTION_ALGLIB_ENABLED
#include "fiction/algorithms/simulation/sidb/exhaustive_ground_state_simulation.hpp"
#include "fiction/algorithms/simulation/sidb/quickexact.hpp"
#include "fiction/algorithms/simulation/sidb/sidb_simulation_engine.hpp"
#include "fiction/algorithms/simulation/sidb/sidb_simulation_parameters.hpp"
#include "fiction/algorithms/simulation/sidb/sidb_simulation_result.hpp"
#include "fiction/traits.hpp"

#include <cassert>
#include <cstdint>
#include <stdexcept>
#include <string>
#include <vector>

namespace fiction
{

template <typename Lyt>
struct clock_emulator_result
{
    std::vector<sidb_simulation_result<Lyt>> clock_phase_results{};

    void pretty_print() const
    {
        // TODO: print every clock phase result on the console with a small delay in between to simulate the clock
        // phases. This is just a placeholder for now.
    }
};

/**
 * @brief Parameter bundle for the clock emulation algorithm.
 */
struct clock_emulator_params
{
    /**
     * @brief Physical simulation parameters used during clock emulation.
     */
    sidb_simulation_parameters sim_params{};
    /**
     * @brief The simulation engine to be used for each step of the clock emulation.
     * @warning QuickSim is not supported in clock emulation, because it does not support static charges.
     */
    sidb_simulation_engine sim_engine{sidb_simulation_engine::QUICKEXACT};
};

namespace detail
{

/**
 * @brief Implementation class for the clock emulation algorithm.
 *
 * @tparam Lyt SiDB cell-level layout type.
 */
template <typename Lyt>
class clock_emulator_impl
{
  public:
    /**
     * @brief Constructs the clock emulation implementation.
     *
     * @param lyt Layout to emulate clocks on.
     * @param num_phases The number of clock phases to simulate.
     * @param ps Clock emulation parameters.
     */
    clock_emulator_impl(const Lyt& lyt, const std::size_t num_phases, const clock_emulator_params& ps) :
            layout{lyt},
            params{ps},
            num_clock_phases{num_phases}
    {}

    /**
     * @brief Executes the clock emulation algorithm.
     *
     * @return Placeholder simulation result for the scaffold.
     */
    [[nodiscard]] clock_emulator_result<Lyt> run()
    {
        // reserve space for each clock phase result
        emulation_result.clock_phase_results.reserve(num_clock_phases);

        // perform clock simulation for each time step
        for (std::size_t i = 0; i < num_clock_phases; ++i)
        {
            // TODO: extract the SiDBs of the current clock phase and put them in a new layout
            // TODO: assign the static charges of the previous simulation to the new layout

            // simulate the new layout
            const auto simulation_result = physical_simulation_of_layout(layout);

            // TODO: take the the charge states of the current simulation result and store them as static charges for
            // the next clock phase
            // TODO: assign those charges to a copy of the layout for the current simulation result
            // TODO: store the simulation result in the emulation result struct

            // FIXME: not correct yet!!
            emulation_result.clock_phase_results.push_back(simulation_result);
        }

        return emulation_result;
    }

  private:
    /**
     * @brief Layout to emulate clocks on.
     */
    const Lyt& layout;
    /**
     * @brief Clock emulation parameters.
     */
    clock_emulator_params params{};
    /**
     * @brief The number of clock phases to simulate.
     */
    std::size_t num_clock_phases{0};
    /**
     * @brief Struct to gather simulation results.
     */
    clock_emulator_result<Lyt> emulation_result{};

    /**
     * This function conducts physical simulation of the given SiDB layout.
     *
     * @param lyt SiDB layout to simulate.
     * @return Simulation results.
     */
    [[nodiscard]] sidb_simulation_result<Lyt> physical_simulation_of_layout(const Lyt& lyt)
    {
        if (params.sim_engine == sidb_simulation_engine::EXGS)
        {
            // perform exhaustive ground state simulation
            return exhaustive_ground_state_simulation(lyt, params.sim_params);
        }
        if (params.sim_engine == sidb_simulation_engine::QUICKEXACT)
        {
            // perform QuickExact exact simulation
            const quickexact_params<cell<Lyt>> quickexact_params{
                params.sim_params, fiction::quickexact_params<cell<Lyt>>::automatic_base_number_detection::OFF};
            return quickexact(lyt, quickexact_params);
        }
#if (FICTION_ALGLIB_ENABLED)
        if (params.sim_engine == sidb_simulation_engine::CLUSTERCOMPLETE)
        {
            // perform ClusterComplete exact simulation
            const clustercomplete_params<cell<Lyt>> cc_params{params.sim_params};
            return clustercomplete(lyt, cc_params);
        }
#endif  // FICTION_ALGLIB_ENABLED
        if (params.sim_engine == sidb_simulation_engine::QUICKSIM)
        {
            throw std::invalid_argument(
                "QuickSim is not supported for clock emulation, because it does not support static charges.");
        }

        assert(false && "unsupported simulation engine");

        return sidb_simulation_result<Lyt>{};
    }
};

}  // namespace detail

/**
 * @brief Emulates clocks for an SiDB layout.
 *
 * @warning QuickSim is not supported for clock emulation, because it does not support static charges.
 *
 * @tparam Lyt SiDB cell-level layout type.
 * @param lyt The layout to emulate clocks on.
 * @param num_clock_phases The number of clock phases to simulate.
 * @param ps Clock emulation parameters.
 * @return Placeholder simulation result for the scaffold.
 * @throw std::invalid_argument if an unsupported simulation engine is selected in the parameters.
 */
template <typename Lyt>
[[nodiscard]] clock_emulator_result<Lyt> emulate_clocks(const Lyt& lyt, const std::size_t num_clock_phases,
                                                        const clock_emulator_params& ps = clock_emulator_params{})
{
    static_assert(is_cell_level_layout_v<Lyt>, "Lyt is not a cell-level layout");
    static_assert(has_sidb_technology_v<Lyt>, "Lyt must be an SiDB layout");

    detail::clock_emulator_impl<Lyt> impl{lyt, num_clock_phases, ps};

    return impl.run();
}

}  // namespace fiction

#endif  // FICTION_CLOCK_EMULATOR_HPP
