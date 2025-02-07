//
// Created by Willem Lambooy on 05/02/2025.
//

#ifndef COMPARE_DESIGNED_SIDB_GATES_HPP
#define COMPARE_DESIGNED_SIDB_GATES_HPP

#include "fiction/technology/charge_distribution_surface.hpp"

#include <cassert>
#include <cstdint>
#include <memory>
#include <optional>
#include <vector>

namespace fiction
{

template <typename Lyt>
struct designed_sidb_gates
{
    using simulation_results_per_input = std::vector<std::vector<charge_distribution_surface<Lyt>>>;

    std::vector<Lyt> gate_layouts;

    std::optional<std::vector<simulation_results_per_input>> simulation_results;
};

// every instance of a designed_sidb_gates_comparator should implement operator<, operator==, and have a sensitivity
// parameter
template <typename Lyt>
class designed_sidb_gates_comparator
{
  public:
    struct sidb_gate_design
    {
        Lyt                                                             gate_design;
        typename designed_sidb_gates<Lyt>::simulation_results_per_input simulation_results_per_input;
    };

             designed_sidb_gates_comparator() = delete;
    virtual ~designed_sidb_gates_comparator() = default;

    designed_sidb_gates_comparator& operator=(const designed_sidb_gates_comparator& other) noexcept = default;
    designed_sidb_gates_comparator& operator=(designed_sidb_gates_comparator&& other) noexcept      = default;

    explicit designed_sidb_gates_comparator(const double sens) noexcept : sensitivity{sens} {}

    [[nodiscard]] virtual bool operator()(const sidb_gate_design& lhs, const sidb_gate_design& rhs) const noexcept = 0;
    [[nodiscard]] virtual bool equals(const sidb_gate_design& lhs, const sidb_gate_design& rhs) const noexcept     = 0;

  protected:
    double sensitivity;
};

// use pointers to prevent slicing
template <typename Lyt>
using designed_sidb_gates_ordering_recipe = std::vector<std::shared_ptr<designed_sidb_gates_comparator<Lyt>>>;

template <typename Lyt>
void order_designed_sidb_gates(const designed_sidb_gates_ordering_recipe<Lyt>& recipe,
                               designed_sidb_gates<Lyt>&                       designed_gates) noexcept
{
    assert(designed_gates.simulation_results.has_value() &&
           "Simulation results must be available for gate design ordering");

    std::vector<typename designed_sidb_gates_comparator<Lyt>::sidb_gate_design> gate_designs_for_assessment{};
    gate_designs_for_assessment.reserve(designed_gates.gate_layouts.size());

    // pair the gate layouts with the associated simulation results using move
    for (uint64_t i = 0; i < designed_gates.gate_layouts.size(); ++i)
    {
        gate_designs_for_assessment.emplace_back(typename designed_sidb_gates_comparator<Lyt>::sidb_gate_design{
            std::move(designed_gates.gate_layouts.at(i)), std::move(designed_gates.simulation_results.value().at(i))});
    }

    // clear the gate_layouts vector so that we may reenter the elements in order later
    designed_gates.gate_layouts.clear();

    // perform ordering procedure using the given recipe
    std::sort(gate_designs_for_assessment.begin(), gate_designs_for_assessment.end(),
              [&](const auto& lhs, const auto& rhs) noexcept
              {
                  for (uint64_t i = 0; i < recipe.size() - 1; ++i)
                  {
                      if (!recipe.at(i)->equals(lhs, rhs))
                      {
                          return (*recipe.at(i))(lhs, rhs);
                      }
                  }

                  return (*recipe.at(recipe.size() - 1))(lhs, rhs);
              });

    // put the designed gate layouts back in order
    for (auto&& [gate_design, _] : gate_designs_for_assessment)
    {
        designed_gates.gate_layouts.emplace_back(std::move(gate_design));
    }
}

}  // namespace fiction

#endif  // COMPARE_DESIGNED_SIDB_GATES_HPP
