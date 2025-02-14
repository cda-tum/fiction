//
// Created by Willem Lambooy on 05/02/2025.
//

#ifndef COMPARE_BY_ENERGETIC_GAP_HPP
#define COMPARE_BY_ENERGETIC_GAP_HPP

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

template <typename Lyt>
[[nodiscard]] double get_ground_state_isolation(const std::vector<charge_distribution_surface<Lyt>>& sim_res) noexcept
{
    if (sim_res.size() == 1)
    {
        return std::numeric_limits<double>::infinity();
    }

    phmap::btree_set<double> system_energies{};

    for (const charge_distribution_surface<Lyt>& cds : sim_res)
    {
        system_energies.insert(cds.get_system_energy());
    }

    return *system_energies.cbegin() - *std::next(system_energies.cbegin(), 1);
}

}  // namespace detail

template <typename Lyt>
class compare_by_minimum_ground_state_isolation final : public designed_sidb_gate_comparator<Lyt>
{
  public:
    using typename designed_sidb_gate_comparator<Lyt>::sidb_gate_design;

    explicit compare_by_minimum_ground_state_isolation(
        const double sens = std::numeric_limits<double>::epsilon()) noexcept :
            designed_sidb_gate_comparator<Lyt>{sens}
    {}

    [[nodiscard]] bool operator()(const sidb_gate_design& lhs, const sidb_gate_design& rhs) const noexcept override
    {
        return minimum_ground_state_isolation_over_all_inputs(lhs) <
               minimum_ground_state_isolation_over_all_inputs(rhs);
    }

    [[nodiscard]] bool equals(const sidb_gate_design& lhs, const sidb_gate_design& rhs) const noexcept override
    {
        return std::abs(minimum_ground_state_isolation_over_all_inputs(lhs) -
                        minimum_ground_state_isolation_over_all_inputs(rhs)) < this->sensitivity;
    }

  private:
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

template <typename Lyt>
class compare_by_average_ground_state_isolation final : public designed_sidb_gate_comparator<Lyt>
{
  public:
    using typename designed_sidb_gate_comparator<Lyt>::sidb_gate_design;

    explicit compare_by_average_ground_state_isolation(
        const double sens = std::numeric_limits<double>::epsilon()) noexcept :
            designed_sidb_gate_comparator<Lyt>{sens}
    {}

    [[nodiscard]] bool operator()(const sidb_gate_design& lhs, const sidb_gate_design& rhs) const noexcept override
    {
        return average_ground_state_isolation_over_all_inputs(lhs) <
               average_ground_state_isolation_over_all_inputs(rhs);
    }

    [[nodiscard]] bool equals(const sidb_gate_design& lhs, const sidb_gate_design& rhs) const noexcept override
    {
        return std::abs(average_ground_state_isolation_over_all_inputs(lhs) -
                        average_ground_state_isolation_over_all_inputs(rhs)) < this->sensitivity;
    }

  private:
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

#endif  // COMPARE_BY_ENERGETIC_GAP_HPP

//
// /**
//      * Performs a sorting operation on the designed gate layouts, putting those in front for which the energetic gap
//      * between the ground state and the first excited state is larger. For each designed gate layout, the minimum
//      * energetic gap is taken over each input. When the minima are equal for two designed gate layouts, the average
//      * energetic gap over each input is taken as a tiebreaker.
//      *
//      * @param designed_gate_layouts A vector of designed gate layouts to sort in place.
//      * @param sim_results_per_input_for_each_gate_design The simulation results for each input of each designed gate
//      * layout.
//      */
//     void sort_designed_gate_layouts_by_ground_state_isolation(
//         std::vector<Lyt>& designed_gate_layouts,
//         std::vector<std::vector<std::vector<charge_distribution_surface<Lyt>>>>
//                                                      sim_results_per_input_for_each_gate_design) const noexcept
//     {
//         // pair the two input vectors
//         std::vector<std::pair<Lyt, std::vector<std::vector<charge_distribution_surface<Lyt>>>>> pairs{};
//         pairs.reserve(designed_gate_layouts.size());
//
//         for (uint64_t i = 0; i < designed_gate_layouts.size(); ++i)
//         {
//             pairs.emplace_back(std::move(designed_gate_layouts.at(i)),
//                                std::move(sim_results_per_input_for_each_gate_design.at(i)));
//         }
//
//         // clear the designed_gate_layouts vector so that we may reenter the elements in order later
//         designed_gate_layouts.clear();
//
//         // sort all individual simulation results by system energy
//         for (auto& pair : pairs)
//         {
//             for (std::vector<charge_distribution_surface<Lyt>>& sim_res : pair.second)
//             {
//                 std::sort(sim_res.begin(), sim_res.end(), [](const auto& cds1, const auto& cds2) noexcept
//                           { return cds1.get_system_energy() < cds2.get_system_energy(); });
//             }
//         }
//
//         const auto get_ground_state_isolation =
//             [&](const std::vector<charge_distribution_surface<Lyt>>& sim_res) noexcept
//         {
//             return sim_res.size() == 1 ? std::numeric_limits<double>::infinity() :
//                                          sim_res.at(1).get_system_energy() - sim_res.at(0).get_system_energy();
//         };
//
//         const auto minimum_ground_state_isolation_for_all_inputs =
//             [&get_ground_state_isolation](
//                 const std::vector<std::vector<charge_distribution_surface<Lyt>>>& res_per_input) noexcept
//         {
//             std::vector<double> ground_state_isolations{};
//             std::transform(res_per_input.cbegin(), res_per_input.cend(), std::back_inserter(ground_state_isolations),
//                            get_ground_state_isolation);
//             return *std::min_element(ground_state_isolations.cbegin(), ground_state_isolations.cend());
//         };
//
//         const auto average_ground_state_isolation_for_all_inputs =
//             [&get_ground_state_isolation](
//                 const std::vector<std::vector<charge_distribution_surface<Lyt>>>& res_per_input) noexcept
//         {
//             uint64_t count = 0;
//
//             double accumulated_ground_state_isolation = 0.0;
//
//             for (const auto& sim_res : res_per_input)
//             {
//                 if (sim_res.size() == 1)
//                 {
//                     continue;
//                 }
//
//                 accumulated_ground_state_isolation += get_ground_state_isolation(sim_res);
//
//                 ++count;
//             }
//
//             return accumulated_ground_state_isolation / static_cast<double>(count);
//         };
//
//         // sort the pairs by minimum ground state isolation for each input
//         std::sort(pairs.begin(), pairs.end(),
//                   [&minimum_ground_state_isolation_for_all_inputs,
//                    &average_ground_state_isolation_for_all_inputs](const auto& lhs, const auto& rhs) noexcept
//                   {
//                       const double diff = minimum_ground_state_isolation_for_all_inputs(lhs.second) -
//                                           minimum_ground_state_isolation_for_all_inputs(rhs.second);
//
//                       // when minima are equal, take the average
//                       if (std::abs(diff) < std::numeric_limits<double>::epsilon())
//                       {
//                           return average_ground_state_isolation_for_all_inputs(lhs.second) >
//                                  average_ground_state_isolation_for_all_inputs(rhs.second);
//                       }
//
//                       return diff > 0.0;
//                   });
//
//         // put the designed gate layouts back in the sorted order
//         for (auto& pair : pairs)
//         {
//             designed_gate_layouts.emplace_back(pair.first);
//         }
//     }
