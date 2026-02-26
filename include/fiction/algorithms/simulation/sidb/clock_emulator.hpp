//
// Created by marcel on 26.02.26.
//

#ifndef FICTION_CLOCK_EMULATOR_HPP
#define FICTION_CLOCK_EMULATOR_HPP

#include <ctime>
#if (FICTION_ALGLIB_ENABLED)
#include "fiction/algorithms/simulation/sidb/clustercomplete.hpp"
#endif  // FICTION_ALGLIB_ENABLED
#include "fiction/algorithms/simulation/sidb/exhaustive_ground_state_simulation.hpp"
#include "fiction/algorithms/simulation/sidb/quickexact.hpp"
#include "fiction/algorithms/simulation/sidb/sidb_simulation_engine.hpp"
#include "fiction/algorithms/simulation/sidb/sidb_simulation_parameters.hpp"
#include "fiction/algorithms/simulation/sidb/sidb_simulation_result.hpp"
#include "fiction/io/print_layout.hpp"
#include "fiction/technology/cell_technologies.hpp"
#include "fiction/technology/sidb_charge_state.hpp"
#include "fiction/traits.hpp"

#include <any>
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
        // phases.
        for (const auto& result : clock_phase_results)
        {
            if (!result.charge_distributions.empty())
            {
                std::cout << "Clock phase: "
                          << std::to_string(
                                 std::any_cast<uint8_t>(result.additional_simulation_parameters.at("clock_phase")))
                          << "\n";
                print_layout(result.charge_distributions[0]);
                std::cout << "-----------------------------\n";
            }
        }
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

        // std::cout << "Initial layout:\n";
        // print_layout(layout);
        // std::cout << "-----------------------------\n";

        // perform clock simulation for each time step
        for (std::size_t i = 0; i < num_clock_phases; ++i)
        {
            // current clock phase is determined by the current time step modulo the maximum number of clock phases
            const auto current_clock_phase = static_cast<uint8_t>(i % layout.num_clocks());
            // extract only the SiDBs of the current clock phase and put them in a new layout
            const auto layout_of_clock_i = extract_sidbs_of_clock_phase(current_clock_phase);

            // std::cout << "Simulating clock phase " << current_clock_phase << "...\n";
            // print_layout(layout_of_clock_i);

            // TODO: assign the static charges of the previous simulation to the new layout

            // simulate the new layout with the user-selected simulation engine and parameters
            auto simulation_result = physical_simulation_of_layout(layout_of_clock_i);
            simulation_result.additional_simulation_parameters["clock_phase"] = current_clock_phase;
            // NOTE: the simulation can yield multiple ground states in case of degenerate states. In this case, we just
            // take the first ground state for the emulation result. This is a simplification that can be improved in
            // the future by taking all ground states and simulating the next clock phase for each of them. This would
            // lead to a tree of possible clock phase evolutions, which could be interesting to analyze.

            // remove all charge distributions from the simulation result except the first groundstate
            if (const auto groundstates = simulation_result.groundstates(); !groundstates.empty())
            {
                // std::cout << "Ground state:\n";
                // print_layout(groundstates[0]);

                simulation_result.charge_distributions = {groundstates[0]};
            }
            else
            {
                // std::cout << "No ground state found for clock phase " << current_clock_phase << "!\n";
            }

            // FIXME: This fails when the groundstate is empty, because there was no layout to simulate, i.e., no clocks
            // of phase i assigned. add all previously removed SiDBs back to the layout as neutral charges
            layout.foreach_cell(
                [this, &simulation_result, current_clock_phase](const auto& cell)
                {
                    auto& cds = simulation_result.charge_distributions[0];
                    if (layout.get_clock_number(cell) != current_clock_phase)
                    {
                        cds.assign_cell_type(cell, layout.get_cell_type(cell),
                                             false);  // TODO: refactor bool as enum class
                        cds.assign_charge_state(cell, sidb_charge_state::NEUTRAL, charge_index_mode::KEEP_CHARGE_INDEX);
                        // TODO also restore cell modes, cell names, etc.
                    }
                });

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
     * @brief Extracts the SiDBs of the current clock phase and puts them in a new layout.
     *
     * @param clock_phase The current clock phase index.
     * @return A new layout containing only the SiDBs of the current clock phase.
     */
    [[nodiscard]] Lyt extract_sidbs_of_clock_phase(const std::size_t clock_phase) const
    {
        auto new_layout = layout.clone();

        layout.foreach_cell(
            [&new_layout, clock_phase](const auto& cell)
            {
                if (new_layout.get_clock_number(cell) != clock_phase)
                {
                    new_layout.assign_cell_type(cell, sidb_technology::EMPTY);
                }
            });

        return new_layout;
    }
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
 * @throw std::invalid_argument if an unsupported simulation engine is selected via the parameters.
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
