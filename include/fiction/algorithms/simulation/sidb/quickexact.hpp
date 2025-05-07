//
// Created by Jan Drewniok on 18.12.22.
//

#ifndef FICTION_QUICKEXACT_HPP
#define FICTION_QUICKEXACT_HPP

#include "fiction/algorithms/iter/gray_code_iterator.hpp"
#include "fiction/algorithms/simulation/sidb/sidb_simulation_engine.hpp"
#include "fiction/algorithms/simulation/sidb/sidb_simulation_parameters.hpp"
#include "fiction/algorithms/simulation/sidb/sidb_simulation_result.hpp"
#include "fiction/layouts/coordinates.hpp"
#include "fiction/technology/charge_distribution_surface.hpp"
#include "fiction/technology/sidb_charge_state.hpp"
#include "fiction/technology/sidb_defects.hpp"
#include "fiction/traits.hpp"

#include <mockturtle/utils/stopwatch.hpp>

#include <algorithm>
#include <cstdint>
#include <unordered_map>
#include <vector>

namespace fiction
{

/**
 * This struct stores the parameters for the *QuickExact* algorithm.
 *
 * @tparam CellType Cell type.
 */
template <typename CellType = offset::ucoord_t>
struct quickexact_params
{
    /**
     * Modes to use for the *QuickExact* algorithm.
     */
    enum class automatic_base_number_detection : uint8_t
    {
        /**
         * Simulation is conducted with the required base number (i.e., if positively charged SiDBs can occur, three
         * state simulation is conducted).
         */
        ON,
        /**
         * The base number from the physical parameter is used for the simulation.
         */
        OFF
    };
    /**
     * All parameters for physical SiDB simulations.
     */
    sidb_simulation_parameters simulation_parameters{};
    /**
     * If `ON`, *QuickExact* checks which base number is required for the simulation, i.e., whether 3-state is
     * necessary or 2-state simulation is sufficient.
     */
    automatic_base_number_detection base_number_detection = automatic_base_number_detection::ON;
    /**
     * Local external electrostatic potentials (e.g., locally applied electrodes).
     */
    std::unordered_map<CellType, double> local_external_potential = {};
    /**
     * Global external electrostatic potential. Value is applied on each cell in the layout.
     */
    double global_potential = 0;
};

namespace detail
{

template <typename Lyt>
class quickexact_impl
{
  public:
    quickexact_impl(const Lyt& lyt, const quickexact_params<cell<Lyt>>& parameter) :
            layout{lyt.clone()},
            charge_lyt{lyt},
            params{parameter}
    {
        charge_lyt.set_sidb_simulation_engine(sidb_simulation_engine::QUICKEXACT);
        charge_lyt.assign_all_charge_states(sidb_charge_state::NEGATIVE);
        charge_lyt.assign_physical_parameters(parameter.simulation_parameters);
    }

    sidb_simulation_result<Lyt> run() noexcept
    {
        result.algorithm_name        = "QuickExact";
        result.simulation_parameters = params.simulation_parameters;
        result.additional_simulation_parameters.emplace("global_potential", params.global_potential);

        mockturtle::stopwatch<>::duration time_counter{};
        {
            const mockturtle::stopwatch stop{time_counter};

            initialize_charge_layout();

            // Determine if three state simulation (i.e., positively charged SiDBs can occur) is required.
            required_simulation_base_number base_number =
                (params.base_number_detection == quickexact_params<cell<Lyt>>::automatic_base_number_detection::ON &&
                 charge_lyt.is_three_state_simulation_required()) ||
                        (params.base_number_detection ==
                             quickexact_params<cell<Lyt>>::automatic_base_number_detection::OFF &&
                         params.simulation_parameters.base == 3) ?
                    required_simulation_base_number::THREE :
                    required_simulation_base_number::TWO;

            // If the layout has at least two SiDBs, all SiDBs that have to be negatively charged are erased from the
            // layout.
            if (number_of_sidbs > 1)
            {
                generate_layout_without_negative_sidbs();

                // If the layout consists of SiDBs that do not need to be negatively charged.
                if (!all_sidbs_in_lyt_without_negative_preassigned_ones.empty())
                {
                    if constexpr (has_get_sidb_defect_v<Lyt>)
                    {
                        charge_distribution_surface charge_layout{layout.clone()};
                        conduct_simulation(charge_layout, base_number);
                    }
                    else
                    {
                        charge_distribution_surface charge_layout{layout.clone()};
                        conduct_simulation(charge_layout, base_number);
                    }
                }

                // If the layout consists of only pre-assigned negatively charged SiDBs
                // (i.e., only SiDBs that are far away from each other).
                else if (all_sidbs_in_lyt_without_negative_preassigned_ones.empty())
                {
                    result.charge_distributions.emplace_back(charge_lyt);
                }
            }
            // If there is only one SiDB in the layout, this single SiDB can be neutrally or even positively charged due
            // to external potentials or defects.
            else if (number_of_sidbs == 1)
            {
                if (base_number == required_simulation_base_number::THREE)
                {
                    charge_lyt.assign_base_number(3);
                }
                else
                {
                    charge_lyt.assign_base_number(2);
                }

                // A check is performed to see if the charge index is still below the maximum charge index. If not, the
                // charge index is increased and the corresponding charge distribution is checked for physical validity.
                while (charge_lyt.get_charge_index_and_base().first < charge_lyt.get_max_charge_index())
                {
                    if (charge_lyt.is_physically_valid())
                    {
                        result.charge_distributions.emplace_back(charge_lyt);
                    }

                    charge_lyt.increase_charge_index_by_one(
                        dependent_cell_mode::VARIABLE);  // `dependent_cell_mode::VARIABLE` allows that the charge state
                                                         // of the dependent cell is automatically changed based on the
                                                         // new charge distribution.
                }

                if (charge_lyt.is_physically_valid())
                {
                    result.charge_distributions.emplace_back(charge_lyt);
                }
            }

            for (const auto& cell : preassigned_negative_sidbs)
            {
                layout.assign_cell_type(cell, Lyt::cell_type::NORMAL);
            }
        }

        result.simulation_runtime = time_counter;

        return result;
    }

  private:
    /**
     * Layout to simulate.
     */
    Lyt layout;
    /**
     * Charge distribution surface.
     */
    charge_distribution_surface<Lyt> charge_lyt{};
    /**
     * Parameters used for the simulation.
     */
    quickexact_params<cell<Lyt>> params{};
    /**
     * Indices of all SiDBs that are pre-assigned to be negatively charged in a physically valid layout.
     */
    std::vector<uint64_t> preassigned_negative_sidb_indices{};
    /**
     * All SiDBs that are pre-assigned to be negatively charged in a physically valid layout.
     */
    std::vector<typename Lyt::cell> preassigned_negative_sidbs{};
    /**
     * All SiDBs of the layout but without the negatively-charged SiDBs.
     */
    std::vector<typename Lyt::cell> all_sidbs_in_lyt_without_negative_preassigned_ones{};
    /**
     * Number of SiDBs of the input layout.
     */
    uint64_t number_of_sidbs{};
    /**
     * Simulation results.
     */
    sidb_simulation_result<Lyt> result{};
    /**
     * Base number required for the correct physical simulation.
     */
    enum class required_simulation_base_number : uint8_t
    {
        /**
         * Two state simulation (i.e., negative and neutral) is sufficient.
         */
        TWO,
        /**
         * Three state simulation (i.e., negative, neutral, and positive) is required.
         */
        THREE
    };
    /**
     * This function initializes the charge layout with necessary parameters, and conducts
     * the physical simulation based on whether a three-state simulation is required.
     *
     * @tparam ChargeLyt Type of the charge distribution surface.
     * @param base_number `THREE` if a three-state simulation is required, `TWO` otherwise.
     */
    template <typename ChargeLyt>
    void conduct_simulation(ChargeLyt& charge_layout, const required_simulation_base_number base_number) noexcept
    {
        static_assert(is_cell_level_layout_v<ChargeLyt>, "ChargeLyt is not a cell-level layout");
        static_assert(has_sidb_technology_v<ChargeLyt>, "ChargeLyt is not an SiDB layout");
        static_assert(is_charge_distribution_surface_v<ChargeLyt>, "ChargeLyt is not a charge distribution surface");

        if (base_number == required_simulation_base_number::THREE)
        {
            charge_lyt.assign_base_number(3);
        }
        else
        {
            charge_lyt.assign_base_number(2);
        }
        charge_layout.assign_physical_parameters(params.simulation_parameters);
        charge_layout.assign_all_charge_states(sidb_charge_state::NEUTRAL);
        charge_layout.update_after_charge_change(dependent_cell_mode::FIXED);
        charge_layout.assign_dependent_cell(all_sidbs_in_lyt_without_negative_preassigned_ones[0]);

        charge_layout.assign_local_external_potential(params.local_external_potential);
        charge_layout.assign_global_external_potential(params.global_potential);

        // IMPORTANT: The pre-assigned negatively charged SiDBs (they have to be negatively charged to
        // fulfill the population stability) are considered as negatively charged defects in the layout.
        // Hence, there are no "real" defects assigned, but in order to set some SiDBs with a fixed
        // negative charge, this way of implementation is chosen.
        for (const auto& cell : preassigned_negative_sidbs)
        {
            charge_layout.add_sidb_defect_to_potential_landscape(
                cell, sidb_defect{sidb_defect_type::UNKNOWN, -1, charge_layout.get_simulation_params().epsilon_r,
                                  charge_layout.get_simulation_params().lambda_tf});
        }

        // Update all local potentials, system energy, and physical validity. The flag is set to
        // `dependent_cell_mode::VARIABLE` to allow the dependent cell to change its charge state based on the N-1 SiDBs
        // to fulfill the local population stability at its position.
        charge_layout.update_after_charge_change(dependent_cell_mode::VARIABLE);

        if (base_number == required_simulation_base_number::TWO)
        {
            result.additional_simulation_parameters.emplace("base_number", uint64_t{2});
            two_state_simulation(charge_layout);
        }
        // If positively charged SiDBs can occur in the layout, 3-state simulation is conducted.
        else
        {
            result.additional_simulation_parameters.emplace("base_number", uint64_t{3});
            three_state_simulation(charge_layout);
        }
    }

    /**
     * This function conducts 2-state physical simulation (negative, neutral).
     *
     * @tparam ChargeLyt Type of the charge distribution surface.
     * @param charge_layout Initialized charge layout.
     */
    template <typename ChargeLyt>
    void two_state_simulation(ChargeLyt& charge_layout) noexcept
    {
        static_assert(is_cell_level_layout_v<ChargeLyt>, "ChargeLyt is not a cell-level layout");
        static_assert(has_sidb_technology_v<ChargeLyt>, "ChargeLyt is not an SiDB layout");
        static_assert(is_charge_distribution_surface_v<ChargeLyt>, "ChargeLyt is not a charge distribution surface");

        charge_layout.assign_base_number(2);
        uint64_t previous_charge_index = 0;

        gray_code_iterator gci{0};

        for (gci = 0; gci <= charge_layout.get_max_charge_index(); ++gci)
        {
            charge_layout.assign_charge_index_by_gray_code(*gci, previous_charge_index, dependent_cell_mode::VARIABLE,
                                                           energy_calculation::KEEP_OLD_ENERGY_VALUE,
                                                           charge_distribution_history::CONSIDER);

            previous_charge_index = *gci;

            if (charge_layout.is_physically_valid())
            {
                result.charge_distributions.emplace_back(charge_layout);
            }
        }

        // The cells of the pre-assigned negatively charged SiDBs are added to the cell level layout.
        for (const auto& cell : preassigned_negative_sidbs)
        {
            layout.assign_cell_type(cell, Lyt::cell_type::NORMAL);
        }
    }
    /**
     * This function conducts 3-state physical simulation (negative, neutral, positive).
     *
     * @tparam ChargeLyt Type of the charge distribution surface.
     * @param charge_layout Initialized charge layout.
     */
    template <typename ChargeLyt>
    void three_state_simulation(ChargeLyt& charge_layout) noexcept
    {
        static_assert(is_cell_level_layout_v<ChargeLyt>, "ChargeLyt is not a cell-level layout");
        static_assert(has_sidb_technology_v<ChargeLyt>, "ChargeLyt is not an SiDB layout");
        static_assert(is_charge_distribution_surface_v<ChargeLyt>, "ChargeLyt is not a charge distribution surface");

        charge_layout.assign_all_charge_states(sidb_charge_state::NEGATIVE);
        charge_layout.update_after_charge_change();
        // Not executed to detect if 3-state simulation is required, but to detect the SiDBs that could be positively
        // charged (important to speed up the simulation).
        charge_layout.is_three_state_simulation_required();
        charge_layout.update_after_charge_change(dependent_cell_mode::VARIABLE);

        while (charge_layout.get_charge_index_and_base().first < charge_layout.get_max_charge_index())
        {
            while (charge_layout.get_charge_index_of_sub_layout() < charge_layout.get_max_charge_index_sub_layout())
            {
                if (charge_layout.is_physically_valid())
                {
                    result.charge_distributions.emplace_back(charge_layout);
                }

                charge_layout.increase_charge_index_of_sub_layout_by_one(
                    dependent_cell_mode::VARIABLE, energy_calculation::KEEP_OLD_ENERGY_VALUE,
                    charge_distribution_history::CONSIDER);  // `dependent_cell_mode::VARIABLE` allows that the
                                                             // charge state of the dependent cell is automatically
                                                             // changed based on the new charge distribution.
            }

            if (charge_layout.is_physically_valid())
            {
                result.charge_distributions.emplace_back(charge_layout);
            }

            if (charge_layout.get_max_charge_index_sub_layout() != 0)
            {
                charge_layout.reset_charge_index_sub_layout();
            }

            charge_layout.increase_charge_index_by_one(
                dependent_cell_mode::VARIABLE, energy_calculation::KEEP_OLD_ENERGY_VALUE,
                charge_distribution_history::NEGLECT);  // `dependent_cell_mode::VARIABLE` allows that the charge
                                                        // state of the dependent cell is automatically changed
                                                        // based on the new charge distribution.
        }

        // charge configurations of the sublayout are iterated
        while (charge_layout.get_charge_index_of_sub_layout() < charge_layout.get_max_charge_index_sub_layout())
        {
            if (charge_layout.is_physically_valid())
            {
                result.charge_distributions.emplace_back(charge_layout);
            }

            charge_layout.increase_charge_index_of_sub_layout_by_one(dependent_cell_mode::VARIABLE,
                                                                     energy_calculation::KEEP_OLD_ENERGY_VALUE,
                                                                     charge_distribution_history::CONSIDER);
        }

        if (charge_layout.is_physically_valid())
        {
            result.charge_distributions.emplace_back(charge_layout);
        }

        for (const auto& cell : preassigned_negative_sidbs)
        {
            layout.assign_cell_type(cell, Lyt::cell_type::NORMAL);
        }
    }
    /**
     * This function is responsible for preparing the charge layout and relevant data structures for the simulation.
     *
     * This function initializes the charge layout within the context of the current simulation. It performs the
     * following tasks:
     *
     * - If the provided layout type `Lyt` supports a `foreach_sidb_defect` method, it iterates through each
     *   defect in the layout.
     *   - If a defect is found, it adds the SiDB defect to the potential landscape.
     * - It assigns the local external potential from the `params.local_external_potential` configuration to the charge
     * layout.
     * - It assigns the global external potential from `params.global_potential` to the charge layout.
     */
    void initialize_charge_layout() noexcept
    {
        // If Lyt is already a charge distribution surface, the layout was copied directly to the charge layout,
        // and initialization was skipped. Therefore, the electrostatic influence of the defects is needed.
        if constexpr (is_sidb_defect_surface_v<Lyt> && is_charge_distribution_surface_v<Lyt>)
        {
            layout.foreach_sidb_defect(
                [this](const auto& cd)
                {
                    const auto& [cell, defect] = cd;

                    if (defect.type != sidb_defect_type::NONE && is_charged_defect_type(cd.second))
                    {
                        charge_lyt.add_sidb_defect_to_potential_landscape(cell, layout.get_sidb_defect(cell));
                    }
                });
        }

        charge_lyt.assign_local_external_potential(params.local_external_potential);
        charge_lyt.assign_global_external_potential(params.global_potential);

        preassigned_negative_sidb_indices = charge_lyt.negative_sidb_detection();
        preassigned_negative_sidbs.reserve(preassigned_negative_sidb_indices.size());

        all_sidbs_in_lyt_without_negative_preassigned_ones = charge_lyt.get_sidb_order();
        // store the number of SiDBs, since the number of active cells changes during simulation.
        number_of_sidbs = charge_lyt.num_cells();
    }
    /**
     *
     * This function is used to generate a layout without the SiDBs that are pre-assigned to be negatively charged in a
     * physically-valid layout.
     */
    void generate_layout_without_negative_sidbs() noexcept
    {
        for (const auto& index : preassigned_negative_sidb_indices)
        {
            const auto cell = charge_lyt.index_to_cell(index);
            preassigned_negative_sidbs.push_back(cell);
            layout.assign_cell_type(cell, Lyt::cell_type::EMPTY);
        }

        // all pre-assigned negatively charged SiDBs are erased from the
        // all_sidbs_in_lyt_without_negative_preassigned_ones vector.
        all_sidbs_in_lyt_without_negative_preassigned_ones.erase(
            std::remove_if(all_sidbs_in_lyt_without_negative_preassigned_ones.begin(),
                           all_sidbs_in_lyt_without_negative_preassigned_ones.end(),
                           [this](const auto& n)
                           {
                               return std::find(preassigned_negative_sidbs.cbegin(), preassigned_negative_sidbs.cend(),
                                                n) != preassigned_negative_sidbs.cend();
                           }),
            all_sidbs_in_lyt_without_negative_preassigned_ones.cend());
    }
};

}  // namespace detail

/**
 * *QuickExact* is a quick and exact physical simulation algorithm designed specifically for SiDB layouts. It was
 * proposed in \"The Need for Speed: Efficient Exact Simulation of Silicon Dangling Bond Logic\" by J. Drewniok, M.
 * Walter, and R. Wille in ASP-DAC 2024 (https://ieeexplore.ieee.org/document/10473946). It determines all physically
 * valid charge configurations of a given SiDB layout, providing a significant performance advantage of more than three
 * orders of magnitude over *ExGS*
 * (`exhaustive_ground_state_simulation`).
 *
 * The performance improvement of *QuickExact* can be attributed to the incorporation of three key ideas:
 *
 * 1. Advanced Negative SiDB Detection: *QuickExact* efficiently identifies SiDBs that require negative charges
 *    in a physically valid charge distribution. By pre-assigned them in advance, the search space is pruned
 *    by a factor of \f$2^k\f$, where k is the number of found SiDBs.
 *
 * 2. Dependent SiDB Selection: The algorithm selects a dependent SiDB, whose charge state is always derived
 *    from its n-1 neighbors. This dependency simplifies the computation process and contributes to the overall
 *    efficiency of *QuickExact*.
 *
 * 3. Gray Code Representation: *QuickExact* employs Gray code to represent and traverse through all charge
 *    configurations. By using Gray code, only one charge state changes at a time, making the computation
 *    of the local electrostatic potential easier.
 *
 * Additionally, *QuickExact* also considers global and local electrostatic potentials, as well as existing defects.
 * This holistic approach ensures an accurate representation of the physical behavior of the SiDB layout.
 *
 * In summary, *QuickExact* combines advanced SiDB charge detection, dependent SiDB selection, and the use of Gray code
 * to achieve outstanding performance and enable efficient simulations of SiDB layouts, even in scenarios where
 * positively-charged SiDBs occur due to small spacing.
 *
 * @tparam Lyt SiDB cell-level layout type.
 * @param lyt Layout to simulate.
 * @param params Parameter required for the simulation.
 * @return Simulation Results.
 */
template <typename Lyt>
[[nodiscard]] sidb_simulation_result<Lyt> quickexact(const Lyt&                          lyt,
                                                     const quickexact_params<cell<Lyt>>& params = {}) noexcept
{
    static_assert(is_cell_level_layout_v<Lyt>, "Lyt is not a cell-level layout");
    static_assert(has_sidb_technology_v<Lyt>, "Lyt is not an SiDB layout");

    detail::quickexact_impl<Lyt> p{lyt, params};

    return p.run();
}

}  // namespace fiction

#endif  // FICTION_QUICKEXACT_HPP
