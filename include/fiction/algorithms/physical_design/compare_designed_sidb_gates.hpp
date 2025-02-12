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

/**
 * This struct is used to store designed gate layouts, optionally along with their respective simulation results for
 * each input.
 *
 * @tparam Lyt SiDB cell-level layout
 */
template <typename Lyt>
struct designed_sidb_gates
{
    /**
     * Simulation results per input is a vector of simulation results (which is a vector of charge distribution
     * surfaces) that occur in the order of the bit representation of the respectively associated inputs.
     */
    using simulation_results_per_input = std::vector<std::vector<charge_distribution_surface<Lyt>>>;
    /**
     * The designed SiDB gate layouts are stored here.
     */
    std::vector<Lyt> gate_layouts;
    /**
     * Optionally, the respectively associated simulation results for each input are stored here.
     */
    std::optional<std::vector<simulation_results_per_input>> simulation_results;
};

// every instance of a designed_sidb_gates_comparator should implement operator<, operator==, and have a sensitivity
// parameter
template <typename Lyt>
class designed_sidb_gate_comparator
{
  public:
    struct sidb_gate_design
    {
        Lyt                                                             gate_design;
        typename designed_sidb_gates<Lyt>::simulation_results_per_input simulation_results_per_input;
    };

    designed_sidb_gate_comparator()          = delete;
    virtual ~designed_sidb_gate_comparator() = default;

    designed_sidb_gate_comparator& operator=(const designed_sidb_gate_comparator& other) noexcept = default;
    designed_sidb_gate_comparator& operator=(designed_sidb_gate_comparator&& other) noexcept      = default;

    explicit designed_sidb_gate_comparator(const double sens) noexcept : sensitivity{sens} {}

    [[nodiscard]] virtual bool operator()(const sidb_gate_design& lhs, const sidb_gate_design& rhs) const noexcept = 0;
    [[nodiscard]] virtual bool equals(const sidb_gate_design& lhs, const sidb_gate_design& rhs) const noexcept     = 0;

  protected:
    double sensitivity;
};

/**
 * An ordering recipe for designed SiDB gates is a vector of pointers to designed SiDB gate comparators. Pointers are
 * used to prevent slicing.
 *
 * @tparam Lyt SiDB cell-level layout.
 */
template <typename Lyt>
using designed_sidb_gates_ordering_recipe = std::vector<std::shared_ptr<designed_sidb_gate_comparator<Lyt>>>;

/**
 * The designed SiDB gates are ordered inplace according to the given ordering recipe. Comparators that occur earlier in
 * the recipe have a higher precedence. Two designed gates are compared using the recipe as follows: iterating through
 * the comparators in the order of precedence, the `equals` function is invoked. When the current comparator judges the
 * two gate implementations to be equal, we move on to the next comparator. This proceeds until one comparator judges
 * non-equality, in which case `operator()` is invoked, which implements `<`. If all comparators judge the two gate
 * implementations to be equal, `operator()` is invoked on the last in the recipe.
 *
 * @tparam Lyt SiDB cell-level layout.
 * @param recipe A list of comparators that compose a recipe for determining an ordering of the designed SiDB gates.
 * @param designed_gates The gates that were designed that are to be ordered by the given ordering recipe.
 */
template <typename Lyt>
void order_designed_sidb_gates(const designed_sidb_gates_ordering_recipe<Lyt>& recipe,
                               designed_sidb_gates<Lyt>&                       designed_gates) noexcept
{
    assert(designed_gates.simulation_results.has_value() &&
           "Simulation results must be available for gate design ordering");

    std::vector<typename designed_sidb_gate_comparator<Lyt>::sidb_gate_design> gate_designs_for_assessment{};
    gate_designs_for_assessment.reserve(designed_gates.gate_layouts.size());

    // pair the gate layouts with the associated simulation results using move
    for (uint64_t i = 0; i < designed_gates.gate_layouts.size(); ++i)
    {
        gate_designs_for_assessment.emplace_back(typename designed_sidb_gate_comparator<Lyt>::sidb_gate_design{
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
