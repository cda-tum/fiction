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
#include "fiction/io/print_layout.hpp"
#include "fiction/technology/cell_technologies.hpp"
#include "fiction/technology/sidb_charge_state.hpp"
#include "fiction/technology/sidb_defect_surface.hpp"
#include "fiction/technology/sidb_defects.hpp"
#include "fiction/traits.hpp"

#include <fmt/format.h>

#include <algorithm>
#include <any>
#include <cassert>
#include <chrono>
#include <cstdint>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <thread>
#include <type_traits>
#include <utility>
#include <vector>

namespace fiction
{

/**
 * @brief Result type for the clock emulation algorithm.
 *
 * Holds the simulation results for each clock phase, and provides utilities
 * for printing and animating them on the console.
 *
 * @tparam Lyt SiDB cell-level layout type.
 */
template <typename Lyt>
struct clock_emulator_result
{
    /// Simulation results for each clock phase.
    std::vector<sidb_simulation_result<Lyt>> clock_phase_results{};

    /**
     * @brief Prints all clock phase results to the console sequentially.
     *
     * Each phase is printed with a header showing the clock phase index, the charge distribution, and a separator.
     */
    void pretty_print() const
    {
        for (std::size_t i = 0; i < clock_phase_results.size(); ++i)
        {
            const auto& result = clock_phase_results[i];

            if (!result.charge_distributions.empty())
            {
                const auto phase = std::any_cast<uint8_t>(result.additional_simulation_parameters.at("clock_phase"));

                std::cout << fmt::format("Step {}/{} — Clock phase: {}\n", i + 1, clock_phase_results.size(), phase);
                print_layout(result.charge_distributions[0]);
                std::cout << std::string(80, '-') << '\n';
            }
        }
    }
    /**
     * @brief Animates the clock phase results on the console by overwriting the previous frame.
     *
     * Uses ANSI escape codes to move the cursor up and overwrite the previous output,
     * creating a terminal animation effect. A configurable delay between frames controls playback speed.
     *
     * @param delay_ms Delay between frames in milliseconds (default: 500).
     * @param repetitions Number of times to loop through the full animation (default: 1).
     */
    void animate(const std::size_t delay_ms = 500, const std::size_t repetitions = 1) const
    {
        if (clock_phase_results.empty() || repetitions == 0)
        {
            return;
        }

        // number of lines in the previous frame (used to overwrite)
        std::size_t previous_line_count = 0;
        // global step counter across all repetitions
        std::size_t step        = 0;
        const auto  total_steps = clock_phase_results.size() * repetitions;

        for (std::size_t r = 0; r < repetitions; ++r)
        {
            for (std::size_t i = 0; i < clock_phase_results.size(); ++i)
            {
                const auto& result = clock_phase_results[i];

                if (result.charge_distributions.empty())
                {
                    ++step;
                    continue;
                }

                // render the frame to a string to count lines
                std::ostringstream frame_stream;

                const auto phase = std::any_cast<uint8_t>(result.additional_simulation_parameters.at("clock_phase"));

                frame_stream << fmt::format("Step {}/{} — Clock phase: {}\n", step + 1, total_steps, phase);
                print_layout(result.charge_distributions[0], frame_stream);

                const auto frame = frame_stream.str();

                // count lines in the frame
                const auto line_count = static_cast<std::size_t>(std::count(frame.begin(), frame.end(), '\n'));

                // move cursor up to overwrite the previous frame (skip for the first frame)
                if (previous_line_count > 0)
                {
                    std::cout << fmt::format("\033[{}A\033[J", previous_line_count);
                }

                std::cout << frame << std::flush;
                previous_line_count = line_count;

                ++step;

                // wait before the next frame (skip after the very last one)
                if (step < total_steps)
                {
                    std::this_thread::sleep_for(std::chrono::milliseconds(delay_ms));
                }
            }
        }

        std::cout << '\n';
    }
};

/// Parameter bundle for the clock emulation algorithm.
struct clock_emulator_params
{
    /// Physical simulation parameters used during clock emulation.
    sidb_simulation_parameters sim_params{};
    /**
     * @brief The simulation engine to be used for each step of the clock emulation.
     * @warning ExGS and QuickSim are not supported in clock emulation, because they do not support static charges.
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
    /// Simulation layout type: wraps in defect surface only if Lyt isn't already one, then adds charge distributions.
    using sim_lyt =
        charge_distribution_surface<std::conditional_t<is_sidb_defect_surface_v<Lyt>, Lyt, sidb_defect_surface<Lyt>>>;
    /// Simulation result type for the simulation layout.
    using sim_result_t = sidb_simulation_result<sim_lyt>;
    /// Phase layout type: a charge distribution surface on top of the simulation layout.
    using phase_lyt = charge_distribution_surface<sim_lyt>;
    /// A collection of cell-charge-state pairs, used to transfer charges between clock phases.
    using cell_charge_assignments = std::vector<std::pair<cell<sim_lyt>, sidb_charge_state>>;

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
    [[nodiscard]] clock_emulator_result<sim_lyt> run()
    {
        // reserve space for each clock phase result
        emulation_result.clock_phase_results.reserve(num_clock_phases);

        cell_charge_assignments charges_from_previous_phase{};

        // perform clock simulation for each time step
        for (std::size_t i = 0; i < num_clock_phases; ++i)
        {
            // current clock phase is determined by the current time step modulo the maximum number of clock phases
            const auto current_clock_phase = static_cast<uint8_t>(i % layout.num_clocks());
            // extract only the SiDBs of the current clock phase and put them in a new layout
            auto layout_of_clock_i = extract_sidbs_of_clock_phase(current_clock_phase);

            // assign the static charges of the previous simulation to the new layout as proxy defects
            const auto proxy_defect_cells = assign_charges_as_defects(layout_of_clock_i, charges_from_previous_phase);

            // simulate the new layout with the user-selected simulation engine and parameters
            auto simulation_result = physical_simulation_of_layout(layout_of_clock_i);
            simulation_result.additional_simulation_parameters["clock_phase"] = current_clock_phase;
            // NOTE: the simulation can yield multiple ground states in case of degenerate states. In this case, we just
            // take the first ground state for the emulation result. This is a simplification that can be improved in
            // the future by taking all ground states and simulating the next clock phase for each of them. This would
            // lead to a tree of possible clock phase evolutions, which could be interesting to analyze.

            phase_lyt phase_solution{sim_lyt{sidb_defect_surface<Lyt>{layout.clone()}, params.sim_params}};

            if (const auto groundstates = simulation_result.groundstates(); !groundstates.empty())
            {
                phase_solution = groundstates[0];
            }
            else
            {
                phase_solution.assign_all_charge_states(sidb_charge_state::NEUTRAL);
            }

            phase_lyt full_phase_layout{sim_lyt{sidb_defect_surface<Lyt>{layout.clone()}, params.sim_params}};
            full_phase_layout.assign_all_charge_states(sidb_charge_state::NEUTRAL);

            // keep previous phase charges as real SiDB charges in the final result layout
            apply_charge_states(full_phase_layout, charges_from_previous_phase);

            // keep current phase simulated charges as real SiDB charges in the final result layout
            layout.foreach_cell(
                [&full_phase_layout, &phase_solution, this, current_clock_phase](const auto& cell)
                {
                    if (layout.get_clock_number(cell) == current_clock_phase)
                    {
                        full_phase_layout.assign_charge_state(cell, phase_solution.get_charge_state(cell),
                                                              charge_index_mode::KEEP_CHARGE_INDEX);
                    }
                });

            // remove only the proxy defects that were injected for simulation, preserving any pre-existing defects
            remove_proxy_defects(full_phase_layout, proxy_defect_cells);

            full_phase_layout.update_after_charge_change();

            // store only current phase charges for defect injection in the next phase
            charges_from_previous_phase = extract_non_neutral_charges_of_phase(full_phase_layout, current_clock_phase);
            simulation_result.charge_distributions.clear();
            simulation_result.charge_distributions.emplace_back(full_phase_layout);

            // add the simulation result of the current clock phase to the emulation result
            emulation_result.clock_phase_results.push_back(simulation_result);
        }

        return emulation_result;
    }

  private:
    /// Layout to emulate clocking behavior of.
    const Lyt& layout;
    /// Clock emulation parameters.
    clock_emulator_params params{};
    /// The number of clock phases to simulate.
    std::size_t num_clock_phases{0};
    /// Struct to gather simulation results.
    clock_emulator_result<sim_lyt> emulation_result{};

    /**
     * @brief Extracts the SiDBs of the current clock phase and puts them in a new layout.
     *
     * @param clock_phase The current clock phase index.
     * @return A new layout containing only the SiDBs of the current clock phase.
     */
    [[nodiscard]] sim_lyt extract_sidbs_of_clock_phase(const std::size_t clock_phase) const
    {
        auto new_layout = sim_lyt{sidb_defect_surface<Lyt>{layout.clone()}, params.sim_params};

        layout.foreach_cell(
            [&new_layout, clock_phase](const auto& cell)
            {
                if (new_layout.get_clock_number(cell) != clock_phase)
                {
                    new_layout.assign_cell_type(cell, sidb_technology::EMPTY);
                }
            });

        new_layout.assign_all_charge_states(sidb_charge_state::NEUTRAL);

        return new_layout;
    }
    /**
     * @brief Assigns the given charge states as proxy defects to the given layout.
     *
     * @param lyt The layout to assign charges to.
     * @param charges The charge states to assign.
     * @return The cells where proxy defects were placed (for later removal).
     */
    [[nodiscard]] std::vector<cell<sim_lyt>> assign_charges_as_defects(sim_lyt&                       lyt,
                                                                       const cell_charge_assignments& charges) const
    {
        std::vector<cell<sim_lyt>> proxy_cells;
        proxy_cells.reserve(charges.size());

        for (const auto& [cell, charge_state] : charges)
        {
            const auto defect =
                sidb_defect{sidb_defect_type::DB, static_cast<int8_t>(charge_state_to_sign(charge_state)),
                            params.sim_params.epsilon_r, params.sim_params.lambda_tf};
            lyt.assign_sidb_defect(cell, defect);
            proxy_cells.push_back(cell);
        }

        return proxy_cells;
    }
    /**
     * @brief Applies the given charge states to the given layout.
     *
     * @param lyt The layout to apply charges to.
     * @param charges The charge states to apply.
     */
    void apply_charge_states(phase_lyt& lyt, const cell_charge_assignments& charges) const
    {
        for (const auto& [cell, charge_state] : charges)
        {
            lyt.assign_charge_state(cell, charge_state, charge_index_mode::KEEP_CHARGE_INDEX);
        }
    }
    /**
     * @brief Extracts all non-neutral charge states of a given clock phase.
     *
     * @param lyt The layout to extract charges from.
     * @param clock_phase The clock phase to extract charges from.
     * @return The extracted charge states.
     */
    [[nodiscard]] cell_charge_assignments extract_non_neutral_charges_of_phase(const phase_lyt& lyt,
                                                                               const uint8_t    clock_phase) const
    {
        cell_charge_assignments phase_charges{};

        layout.foreach_cell(
            [this, &lyt, &phase_charges, clock_phase](const auto& cell)
            {
                if (layout.get_clock_number(cell) == clock_phase)
                {
                    const auto charge_state = lyt.get_charge_state(cell);
                    if (charge_state != sidb_charge_state::NEUTRAL && charge_state != sidb_charge_state::NONE)
                    {
                        phase_charges.emplace_back(cell, charge_state);
                    }
                }
            });

        return phase_charges;
    }
    /**
     * @brief Removes only the proxy defects that were injected by assign_charges_as_defects.
     *
     * Pre-existing defects on the input layout are preserved.
     *
     * @param lyt The layout to remove proxy defects from.
     * @param proxy_cells The cells where proxy defects were placed.
     */
    void remove_proxy_defects(phase_lyt& lyt, const std::vector<cell<sim_lyt>>& proxy_cells) const
    {
        for (const auto& cell : proxy_cells)
        {
            lyt.assign_sidb_defect(cell, sidb_defect{sidb_defect_type::NONE});
        }
    }
    /**
     * Conduct physical simulation of the given SiDB layout with a user-selected simulation engine.
     *
     * @param lyt SiDB layout to simulate.
     * @return Simulation results.
     */
    [[nodiscard]] sim_result_t physical_simulation_of_layout(const sim_lyt& lyt)
    {
        if (params.sim_engine == sidb_simulation_engine::QUICKEXACT)
        {
            // perform QuickExact exact simulation
            const quickexact_params<cell<sim_lyt>> quickexact_params{params.sim_params};
            return quickexact(lyt, quickexact_params);
        }
#if (FICTION_ALGLIB_ENABLED)
        if (params.sim_engine == sidb_simulation_engine::CLUSTERCOMPLETE)
        {
            // perform ClusterComplete exact simulation
            const clustercomplete_params<cell<sim_lyt>> cc_params{params.sim_params};
            return clustercomplete(lyt, cc_params);
        }
#endif  // FICTION_ALGLIB_ENABLED

        throw std::invalid_argument(fmt::format("The selected simulation engine '{}' cannot be used for clock "
                                                "emulation, because it does not support static charges.",
                                                sidb_simulation_engine_name(params.sim_engine))
                                        .c_str());
    }
};

}  // namespace detail

/**
 * @brief Emulates clocking behavior of a given SiDB layout.
 *
 * @warning ExGS and QuickSim cannot be used as simulation engines for clock emulation, because they do not support
 * static charges.
 *
 * @tparam Lyt SiDB cell-level layout type.
 * @param lyt The layout to emulate clocks on.
 * @param num_clock_phases The number of clock phases to simulate.
 * @param ps Clock emulation parameters.
 * @return Placeholder simulation result for the scaffold.
 * @throw std::invalid_argument if an unsupported simulation engine is selected via the parameters.
 */
template <typename Lyt>
[[nodiscard]] clock_emulator_result<
    charge_distribution_surface<std::conditional_t<is_sidb_defect_surface_v<Lyt>, Lyt, sidb_defect_surface<Lyt>>>>
emulate_clocks(const Lyt& lyt, const std::size_t num_clock_phases,
               const clock_emulator_params& ps = clock_emulator_params{})
{
    static_assert(is_cell_level_layout_v<Lyt>, "Lyt is not a cell-level layout");
    static_assert(has_sidb_technology_v<Lyt>, "Lyt must be an SiDB layout");

    detail::clock_emulator_impl<Lyt> impl{lyt, num_clock_phases, ps};

    return impl.run();
}

}  // namespace fiction

#endif  // FICTION_CLOCK_EMULATOR_HPP
