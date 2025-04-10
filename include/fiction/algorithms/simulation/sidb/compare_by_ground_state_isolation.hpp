//
// Created by Willem Lambooy on 05.02.2025.
//

#ifndef COMPARE_BY_GROUND_STATE_ISOLATION
#define COMPARE_BY_GROUND_STATE_ISOLATION

#include "fiction/algorithms/physical_design/compare_designed_sidb_gates.hpp"
#include "fiction/technology/charge_distribution_surface.hpp"

#include <btree.h>

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <iterator>
#include <limits>
#include <vector>

namespace fiction
{

namespace detail
{

/**
 * Obtains the energetic ground state isolation; i.e., the energetic difference between the ground state and the first
 * excited state.
 *
 * @tparam Lyt SiDB cell-level layout type.
 * @param sim_res Simulation results as a vector of physically valid charge distributions.
 * @return The energetic difference between the ground state and the first excited state.
 */
template <typename Lyt>
[[nodiscard]] double get_ground_state_isolation(const std::vector<charge_distribution_surface<Lyt>>& sim_res) noexcept
{
    if (sim_res.size() == 1)
    {
        return std::numeric_limits<double>::infinity();
    }

    double ground_state_energy        = std::numeric_limits<double>::infinity();
    double first_excited_state_energy = std::numeric_limits<double>::infinity();

    for (const charge_distribution_surface<Lyt>& cds : sim_res)
    {
        const double energy = cds.get_electrostatic_potential_energy();

        if (energy - ground_state_energy < 0)
        {
            ground_state_energy = energy;
            continue;
        }

        if (energy - first_excited_state_energy < 0)
        {
            first_excited_state_energy = energy;
        }
    }

    return ground_state_energy - first_excited_state_energy;
}

}  // namespace detail

/**
 * This comparator for designed SiDB gates implements comparison by minimum ground state isolation over all input
 * combinations.
 *
 * @tparam Lyt SiDB cell-level layout type.
 */
template <typename Lyt>
class compare_by_minimum_ground_state_isolation final : public designed_sidb_gate_comparator<Lyt>
{
  public:
    using typename designed_sidb_gate_comparator<Lyt>::sidb_gate_design;
    /**
     * Standard constructor.
     *
     * @param sens The sensitivity parameter determines when an energetic difference it big enough to judge the
     * left-hand right-hand side as not equal.
     */
    explicit compare_by_minimum_ground_state_isolation(
        const double sens = std::numeric_limits<double>::epsilon()) noexcept :
            designed_sidb_gate_comparator<Lyt>{sens}
    {}
    /**
     * Implements strict comparison by minimum ground state isolation.
     *
     * @param lhs Left-hand side.
     * @param rhs Right-hand side.
     * @return True if and only if the left-hand side is strictly below the right-hand side.
     */
    [[nodiscard]] bool operator()(const sidb_gate_design& lhs, const sidb_gate_design& rhs) const noexcept override
    {
        return minimum_ground_state_isolation_over_all_inputs(lhs) <
               minimum_ground_state_isolation_over_all_inputs(rhs);
    }
    /**
     * Implements equality comparison for minimum ground state isolation.
     *
     * @param lhs Left-hand side.
     * @param rhs Right-hand side.
     * @return True if and only if the left-hand side is equal to the right-hand side as determined by the sensitivity
     * parameter.
     */
    [[nodiscard]] bool equals(const sidb_gate_design& lhs, const sidb_gate_design& rhs) const noexcept override
    {
        return std::abs(minimum_ground_state_isolation_over_all_inputs(lhs) -
                        minimum_ground_state_isolation_over_all_inputs(rhs)) < this->sensitivity;
    }

  private:
    /**
     * Obtains the minimum ground state isolation over all input for the given SiDB gate design.
     *
     * @param gate_design The SiDB gate design object with simulation results for each input to obtain the minimum
     * ground state isolation over all inputs of.
     * @return The minimum ground state isolation over all inputs.
     */
    [[nodiscard]] double
    minimum_ground_state_isolation_over_all_inputs(const sidb_gate_design& gate_design) const noexcept
    {
        std::vector<double> ground_state_isolations{};
        ground_state_isolations.reserve(gate_design.simulation_results_per_input.size());

        std::transform(gate_design.simulation_results_per_input.cbegin(),
                       gate_design.simulation_results_per_input.cend(), std::back_inserter(ground_state_isolations),
                       [](const auto& sim_res) { return detail::get_ground_state_isolation(sim_res); });

        return *std::min_element(ground_state_isolations.cbegin(), ground_state_isolations.cend());
    }
};
/**
 * This comparator for designed SiDB gates implements comparison by average ground state isolation over all input
 * combinations.
 *
 * @tparam Lyt SiDB cell-level layout type.
 */
template <typename Lyt>
class compare_by_average_ground_state_isolation final : public designed_sidb_gate_comparator<Lyt>
{
  public:
    using typename designed_sidb_gate_comparator<Lyt>::sidb_gate_design;
    /**
     * Standard constructor.
     *
     * @param sens The sensitivity parameter determines when an energetic difference it big enough to judge the
     * left-hand right-hand side as not equal.
     */
    explicit compare_by_average_ground_state_isolation(
        const double sens = std::numeric_limits<double>::epsilon()) noexcept :
            designed_sidb_gate_comparator<Lyt>{sens}
    {}
    /**
     * Implements strict comparison by average ground state isolation.
     *
     * @param lhs Left-hand side.
     * @param rhs Right-hand side.
     * @return True if and only if the left-hand side is strictly below the right-hand side.
     */
    [[nodiscard]] bool operator()(const sidb_gate_design& lhs, const sidb_gate_design& rhs) const noexcept override
    {
        return average_ground_state_isolation_over_all_inputs(lhs) <
               average_ground_state_isolation_over_all_inputs(rhs);
    }
    /**
     * Implements equality comparison for average ground state isolation.
     *
     * @param lhs Left-hand side.
     * @param rhs Right-hand side.
     * @return True if and only if the left-hand side is equal to the right-hand side as determined by the sensitivity
     * parameter.
     */
    [[nodiscard]] bool equals(const sidb_gate_design& lhs, const sidb_gate_design& rhs) const noexcept override
    {
        return std::abs(average_ground_state_isolation_over_all_inputs(lhs) -
                        average_ground_state_isolation_over_all_inputs(rhs)) < this->sensitivity;
    }

  private:
    /**
     * Obtains the average ground state isolation over all input for the given SiDB gate design.
     *
     * @param gate_design The SiDB gate design object with simulation results for each input to obtain the average
     * ground state isolation over all inputs of.
     * @return The average ground state isolation over all inputs.
     */
    [[nodiscard]] double
    average_ground_state_isolation_over_all_inputs(const sidb_gate_design& gate_design) const noexcept
    {
        uint64_t count = 0;

        double accumulated_ground_state_isolation = 0.0;

        for (const auto& sim_res : gate_design.simulation_results_per_input)
        {
            if (sim_res.size() == 1)
            {
                continue;
            }

            accumulated_ground_state_isolation += detail::get_ground_state_isolation(sim_res);

            ++count;
        }

        return accumulated_ground_state_isolation / static_cast<double>(count);
    }
};

}  // namespace fiction

#endif  // COMPARE_BY_GROUND_STATE_ISOLATION
