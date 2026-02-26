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
#include "fiction/technology/sidb_defect_surface.hpp"
#include "fiction/technology/sidb_defects.hpp"
#include "fiction/traits.hpp"

#include <fmt/format.h>

#include <any>
#include <cassert>
#include <cstdint>
#include <iostream>
#include <stdexcept>
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
                std::cout << fmt::format(
                    "Clock phase: {}\n",
                    std::any_cast<uint8_t>(result.additional_simulation_parameters.at("clock_phase")));
                print_layout(result.charge_distributions[0]);
                std::cout << fmt::format(
                    "{}", "------------------------------------------------------------------------------\n");
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
    using sim_lyt      = charge_distribution_surface<sidb_defect_surface<Lyt>>;
    using sim_result_t = sidb_simulation_result<sim_lyt>;
    using phase_lyt    = charge_distribution_surface<sim_lyt>;

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

        std::cout << fmt::format("{}", "Initial layout:\n");
        print_layout(layout);
        std::cout << fmt::format("{}",
                                 "------------------------------------------------------------------------------\n");

        std::vector<std::pair<cell<sim_lyt>, sidb_charge_state>> charges_from_previous_phase{};

        // perform clock simulation for each time step
        for (std::size_t i = 0; i < num_clock_phases; ++i)
        {
            // current clock phase is determined by the current time step modulo the maximum number of clock phases
            const auto current_clock_phase = static_cast<uint8_t>(i % layout.num_clocks());
            // extract only the SiDBs of the current clock phase and put them in a new layout
            auto layout_of_clock_i = extract_sidbs_of_clock_phase(current_clock_phase);

            // assign the static charges of the previous simulation to the new layout
            assign_charges_as_defects(layout_of_clock_i, charges_from_previous_phase);

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

            // ensure all temporary defects used for simulation are removed from the returned layout
            remove_all_defects(full_phase_layout);

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
    /**
     * @brief Layout to emulate clocking behavior of.
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

        // // print the charge state of each cell in the new layout for debugging
        // new_layout.foreach_cell(
        //     [&new_layout](const auto& cell)
        //     {
        //         std::cout << fmt::format("Cell {}: charge state {}\n", cell,
        //                                  charge_state_to_sign(new_layout.get_charge_state(cell)));
        //     });

        return new_layout;
    }
    /**
     * @brief Assigns the given charge states as defects to the given layout.
     *
     * @param lyt The layout to assign charges to.
     * @param charges The charge states to assign.
     */
    void assign_charges_as_defects(sim_lyt&                                                        lyt,
                                   const std::vector<std::pair<cell<sim_lyt>, sidb_charge_state>>& charges) const
    {
        for (const auto& [cell, charge_state] : charges)
        {
            const auto defect =
                sidb_defect{sidb_defect_type::DB, static_cast<int8_t>(charge_state_to_sign(charge_state)),
                            params.sim_params.epsilon_r, params.sim_params.lambda_tf};
            lyt.assign_sidb_defect(cell, defect);
        }
    }
    /**
     * @brief Applies the given charge states to the given layout.
     *
     * @param lyt The layout to apply charges to.
     * @param charges The charge states to apply.
     */
    void apply_charge_states(phase_lyt&                                                      lyt,
                             const std::vector<std::pair<cell<sim_lyt>, sidb_charge_state>>& charges) const
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
    [[nodiscard]] std::vector<std::pair<cell<sim_lyt>, sidb_charge_state>>
    extract_non_neutral_charges_of_phase(const phase_lyt& lyt, const uint8_t clock_phase) const
    {
        auto phase_charges = std::vector<std::pair<cell<sim_lyt>, sidb_charge_state>>{};

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
     * @brief Removes all defects from the given layout.
     *
     * @param lyt The layout to remove defects from.
     */
    void remove_all_defects(phase_lyt& lyt) const
    {
        auto defect_cells = std::vector<cell<sim_lyt>>{};

        lyt.foreach_sidb_defect(
            [&defect_cells](const auto& cd)
            {
                if (const auto& [cell, defect] = cd;
                    defect.type != sidb_defect_type::NONE && is_charged_defect_type(defect))
                {
                    defect_cells.push_back(cell);
                }
            });

        for (const auto& cell : defect_cells)
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
[[nodiscard]] clock_emulator_result<charge_distribution_surface<sidb_defect_surface<Lyt>>>
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
