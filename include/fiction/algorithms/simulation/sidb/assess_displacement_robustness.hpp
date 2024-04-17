//
// Created by Jan Drewniok on 21.03.24.
//

#ifndef FICTION_ASSESS_DISPLACEMENT_ROBUSTNESS_HPP
#define FICTION_ASSESS_DISPLACEMENT_ROBUSTNESS_HPP

#include "fiction/algorithms/simulation/sidb/is_operational.hpp"
#include "fiction/algorithms/simulation/sidb/sidb_simulation_engine.hpp"
#include "fiction/io/print_layout.hpp"
#include "fiction/layouts/coordinates.hpp"
#include "fiction/traits.hpp"
#include "fiction/utils/layout_utils.hpp"
#include "fiction/utils/math_utils.hpp"

#include <mockturtle/utils/stopwatch.hpp>

#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <random>
#include <set>
#include <utility>
#include <vector>

namespace fiction
{
/**
 * A displacement robustness domain encompasses SiDB layouts obtained from an original SiDB gate by displacing
 * individual SiDBs. These layouts remain valid gate implementations if they fulfill the required logic.
 */
template <typename Lyt>
struct displacement_robustness_domain
{
    /**
     * Represents a domain of displacement robustness for layouts resulting from applying a displacement to a given gate
     * layout.
     */
    std::vector<std::pair<Lyt, operational_status>> operational_values{};
};
/**
 * Parameters for the `assess_sidb_gate_displacement_robustness` algorithm.
 *
 * @param Lyt SiDB cell-level layout Type.
 */
template <typename Lyt>
struct sidb_gate_displacement_robustness_params
{
    /**
     * Possible operational status of a layout.
     */
    enum class displacement_analysis_mode
    {
        /**
         * The layout is operational.
         */
        ALL_POSSIBLE_DISPLACEMENTS,
        /**
         * The layout is non-operational.
         */
        RANDOM_SAMPLING,
    };
    displacement_analysis_mode mode{displacement_analysis_mode::ALL_POSSIBLE_DISPLACEMENTS};
    /**
     * This parameter defines the percentage of all possible misplacements that are analyzed.
     */
    double random_sampling_percentage{0.1};
    /**
     * The atomic displacement in x- and y-direction, respectively. The default value is (1, 1).
     */
    std::pair<uint64_t, uint64_t> displacement_variations = {0, 0};
    /**
     * The simulation engine used.
     */
    sidb_simulation_engine sim_engine{sidb_simulation_engine::QUICKEXACT};
    /**
     * Parameters to check the operation status.
     */
    is_operational_params operational_params{};
    /**
     * Cells/SiDBs of the layout which are not affected by variations.
     */
    std::set<cell<Lyt>> fixed_cells{};
};

struct displacement_robustness_stats
{
    /**
     * Total runtime in sec. to determine the robustness of the SiDB gate.
     */
    mockturtle::stopwatch<>::duration time_total{0};
    /**
     * The number of operational SiDB gates resulting from the given layout by displacements.
     */
    std::size_t num_operational_sidb_displacements{0};
    /**
     * The number of non-operational SiDB gates resulting from the given layout by displacements.
     */
    std::size_t num_non_operational_sidb_displacements{0};
};

/**
 * Counts the number of operational SiDB gates in the given displacement robustness domain.
 *
 * @tparam Lyt The SiDB cell-level layout type.
 * @param domain The displacement robustness domain to analyze.
 * @return The number of operational layouts within the domain.
 */
template <typename Lyt>
std::size_t num_operational_layouts(const displacement_robustness_domain<Lyt>& domain)
{
    return static_cast<std::size_t>(std::count_if(domain.operational_values.begin(), domain.operational_values.end(),
                                                  [](const auto& robust)
                                                  { return robust.second == operational_status::OPERATIONAL; }));
}

/**
 * Counts the number of non-operational SiDB gates in the given displacement robustness domain.
 *
 * @tparam Lyt The SiDB cell-level layout type.
 * @param domain The displacement robustness domain to analyze.
 * @return The number of non-operational layouts within the domain.
 */
template <typename Lyt>
std::size_t num_non_operational_layouts(const displacement_robustness_domain<Lyt>& domain)
{
    return static_cast<std::size_t>(std::count_if(domain.operational_values.begin(), domain.operational_values.end(),
                                                  [](const auto& robust)
                                                  { return robust.second == operational_status::NON_OPERATIONAL; }));
}

namespace detail
{

template <typename Lyt, typename TT>
class displacement_robustness_domain_impl
{
  public:
    displacement_robustness_domain_impl(const Lyt& lyt, const std::vector<TT>& spec,
                                        sidb_gate_displacement_robustness_params<Lyt>& ps,
                                        displacement_robustness_stats&                 st) noexcept :
            layout{lyt},
            params{ps},
            stats{st},
            truth_table{spec}
    {
        original_layout_cells.reserve(layout.num_cells());
        for (const auto& c : params.fixed_cells)
        {
            assert(!layout.is_empty_cell(c) && "Not all fixed cells are part of the layout");
        }

        assert(
            (is_operational(layout, truth_table, params.operational_params).first == operational_status::OPERATIONAL) &&
            "The given layout is not a valid SiDb gate implementation");

        layout.foreach_cell([&](const auto& c) { original_layout_cells.push_back(c); });
        determine_all_cell_displacements();
    };

    [[nodiscard]] displacement_robustness_domain<Lyt> determine_robustness_domain() noexcept
    {
        mockturtle::stopwatch stop{stats.time_total};
        determine_all_cell_displacements();
        // Shuffle the layouts vector randomly
        std::random_device rd;
        std::mt19937       g(rd());  // Initialize random number generator with a seed
        auto               layouts = generate_all_displacement_sidb_layouts();
        std::shuffle(layouts.begin(), layouts.end(), g);

        double percentage_of_layouts = 1.0;
        if (params.mode == sidb_gate_displacement_robustness_params<Lyt>::displacement_analysis_mode::RANDOM_SAMPLING)
        {
            percentage_of_layouts = params.random_sampling_percentage;
        }

        assert(percentage_of_layouts >= 0.0 && percentage_of_layouts <= 1.0 &&
               "Percentage of layouts must be between 0.0 and 1.0");

        displacement_robustness_domain<Lyt> domain{};
        for (auto i = 0u; i < static_cast<uint64_t>(layouts.size() * percentage_of_layouts); i++)
        {
            // Assess the operational status of the new layout
            //            print_sidb_layout(std::cout, layouts[i]);
            //            std::cout << "----------------------------------------------\n";
            const auto op_status = is_operational(layouts[i], truth_table, params.operational_params);

            // Store the operational status in the domain
            domain.operational_values.emplace_back(layouts[i], op_status.first);
            if (op_status.first == operational_status::OPERATIONAL)
            {
                stats.num_operational_sidb_displacements++;
            }
            else
            {
                stats.num_non_operational_sidb_displacements++;
            }
        }
        return domain;
    }

    [[nodiscard]] double determine_robustness_domain_p_error_rate(const double error_p) noexcept
    {
        const auto all_combinations = determine_all_combinations_of_distributing_k_entities_on_n_positions(
            static_cast<uint64_t>(original_layout_cells.size() * error_p), original_layout_cells.size());

        std::size_t num_operational_combinations     = 0;
        std::size_t num_non_operational_combinations = 0;

        uint64_t counter = 0;
        std::cout << fmt::format("All combination: {}", all_combinations.size()) << std::endl;

        for (const auto& fixed_c_indices : all_combinations)
        {
            if (counter >= all_combinations.size() * 0.1)
            {
                break;
            }
            for (const auto& c : original_layout_cells)
            {
                params.fixed_cells.insert(c);
            }
            for (const auto& c : fixed_c_indices)
            {
                const auto cells = original_layout_cells[c];
                params.fixed_cells.erase(cells);
            }
            determine_all_cell_displacements();

            counter++;
            if (counter % 100 == 0)
            {
                std::cout << counter << std::endl;
            }
            const auto robustness_domain = determine_robustness_domain();
            // std::cout << mockturtle::to_seconds(stats.time_total) << std::endl;
            num_non_operational_combinations += num_non_operational_layouts(robustness_domain);
            num_operational_combinations += num_operational_layouts(robustness_domain);
        }

        return static_cast<double>(num_non_operational_combinations) /
               static_cast<double>(num_operational_combinations + num_non_operational_combinations);
    }

  private:
    void determine_all_cell_displacements() noexcept
    {
        all_displacements_for_all_coordinates = {};
        all_displacements_for_all_coordinates.reserve(layout.num_cells());
        layout.foreach_cell(
            [&](const auto& c)
            {
                if constexpr (has_siqad_coord_v<Lyt>)
                {
                    auto new_pos_se = siqad::to_fiction_coord<cube::coord_t>(c);
                    auto new_pos_nw = siqad::to_fiction_coord<cube::coord_t>(c);

                    // the cell c is a fixed cell, i.e., no displacement is considered.
                    if (params.fixed_cells.find(c) == params.fixed_cells.end())
                    {
                        new_pos_se.x -= static_cast<decltype(new_pos_se.x)>(params.displacement_variations.first);
                        new_pos_se.y -= static_cast<decltype(new_pos_se.y)>(params.displacement_variations.second);
                        new_pos_nw.x += static_cast<decltype(new_pos_nw.x)>(params.displacement_variations.first);
                        new_pos_nw.y += static_cast<decltype(new_pos_nw.y)>(params.displacement_variations.second);
                    }
                    const auto all_coord = all_coordinates_in_spanned_area<cell<Lyt>>(
                        siqad::to_siqad_coord(new_pos_se), siqad::to_siqad_coord(new_pos_nw));
                    all_displacements_for_all_coordinates.push_back(all_coord);
                }
                else
                {
                    auto new_pos_se = c;
                    auto new_pos_nw = c;
                    if (params.fixed_cells.find(c) == params.fixed_cells.end())
                    {
                        new_pos_se.x -= params.displacement_variations.first;
                        new_pos_se.y -= params.displacement_variations.second;
                        new_pos_nw.x += params.displacement_variations.first;
                        new_pos_nw.y += params.displacement_variations.second;
                    }

                    const auto all_coord = all_coordinates_in_spanned_area<cell<Lyt>>(new_pos_se, new_pos_nw);
                    all_displacements_for_all_coordinates.push_back(all_coord);
                }
            });
    }
    /**
     * Recursively generates combinations of cell displacements for all coordinates.
     *
     * This function recursively generates combinations of cell displacements for all coordinates
     * based on the provided vector of displacement vectors. The resulting combinations are stored
     * in the 'result' vector.
     *
     * @tparam Lyt The SiDB cell-level layout type.
     * @param result The vector to store the generated combinations.
     * @param current_combination The current combination being constructed.
     * @param index The current index in the vector of displacement vectors.
     */
    void generate_combinations(std::vector<std::vector<cell<Lyt>>>& result, std::vector<cell<Lyt>>& current_combination,
                               std::size_t index) noexcept
    {
        if (index == all_displacements_for_all_coordinates.size())
        {
            result.push_back(current_combination);
            return;
        }

        for (const auto& c : all_displacements_for_all_coordinates[index])
        {
            current_combination.push_back(c);
            generate_combinations(result, current_combination, index + 1);
            current_combination.pop_back();
        }
    }

    /**
     * Generates all displacement SiDB layouts.
     *
     * This function generates all displacement SiDB layouts based on the provided vector
     * of displacement vectors and the original layout. It filters out layouts where two
     * or more SiDBs land on the same spot due to misplacement.
     *
     * @tparam Lyt The SiDB cell-level layout type.
     * @return A vector containing all valid displacement SiDB layouts.
     */
    [[nodiscard]] std::vector<Lyt> generate_all_displacement_sidb_layouts() noexcept
    {
        std::vector<std::vector<cell<Lyt>>> result{};
        std::vector<cell<Lyt>>              current_combination{};
        generate_combinations(result, current_combination, 0);

        std::vector<Lyt> layouts{};
        layouts.reserve(result.size());

        for (const auto& cell_displacements : result)
        {
            Lyt         lyt{};
            std::size_t identical_cells_to_original_layout = 0;
            for (std::size_t i = 0; i < cell_displacements.size(); ++i)
            {
                lyt.assign_cell_type(cell_displacements[i], layout.get_cell_type(original_layout_cells[i]));
                if (!layout.is_empty_cell(cell_displacements[i]))
                {
                    identical_cells_to_original_layout++;
                }
            }
            if (lyt.num_cells() == layout.num_cells() && identical_cells_to_original_layout != layout.num_cells())
            {
                layouts.push_back(lyt);
            }
        }
        return layouts;
    }

    /**
     * The SiDB cell-level layout/SiDB gate for which the displacement robustness calculation is performed.
     */
    const Lyt& layout;
    /**
     * The parameters for the displacement robustness computation.
     */
    sidb_gate_displacement_robustness_params<Lyt>& params;
    /**
     * The statistics of the displacement robustness computation.
     */
    displacement_robustness_stats& stats;
    /**
     * This stores all displacements for all coordinates in the SiDB gate. This means e.g. the first vector describes
     * all positions of the first SiDB.
     */
    std::vector<std::vector<cell<Lyt>>> all_displacements_for_all_coordinates{};
    /**
     * SiDB positions of the initial SiDB layout.
     */
    std::vector<cell<Lyt>> original_layout_cells{};
    /**
     * The specification of the layout.
     */
    const std::vector<TT>& truth_table;
};

}  // namespace detail

/**
 * This function analyzes the operational status of all possible displacements of the SiDBs in the SiDB gate,
 * based on the provided truth table specification and displacement robustness computation parameters.
 *
 * @tparam Lyt SiDB cell-level layout type.
 * @tparam TT The type of the truth table specifying the gate behavior.
 * @param spec Vector of truth table specifications.
 * @param params The parameters for the displacement robustness computation.
 * @param stats Statistics related to the displacement robustness computation.
 * @return The displacement robustness of the SiDB gate.
 */
template <typename Lyt, typename TT>
[[nodiscard]] displacement_robustness_domain<Lyt>
determine_sidb_gate_robustness_domain(const Lyt& layout, const std::vector<TT>& spec,
                                      sidb_gate_displacement_robustness_params<Lyt>& params,
                                      displacement_robustness_stats*                 stats = nullptr)
{
    static_assert(is_cell_level_layout_v<Lyt>, "Lyt is not a cell-level layout");
    static_assert(has_sidb_technology_v<Lyt>, "Lyt is not an SiDB layout");

    displacement_robustness_stats                        st{};
    detail::displacement_robustness_domain_impl<Lyt, TT> p{layout, spec, params, st};

    const auto result = p.determine_robustness_domain();

    if (stats)
    {
        *stats = st;
    }
    return result;
}

/**
 * This function analyzes the operational status of all possible displacements of the SiDBs in the SiDB gate,
 * based on the provided truth table specification and displacement robustness computation parameters.
 *
 * @tparam Lyt SiDB cell-level layout type.
 * @tparam TT The type of the truth table specifying the gate behavior.
 * @param spec Vector of truth table specifications.
 * @param params The parameters for the displacement robustness computation.
 * @param stats Statistics related to the displacement robustness computation.
 * @return The displacement robustness of the SiDB gate.
 */
template <typename Lyt, typename TT>
[[nodiscard]] double
determine_sidb_gate_robustness_domain_p_error_rate(const Lyt& layout, const std::vector<TT>& spec,
                                                   sidb_gate_displacement_robustness_params<Lyt>& params,
                                                   double                                         error_p)
{
    static_assert(is_cell_level_layout_v<Lyt>, "Lyt is not a cell-level layout");
    static_assert(has_sidb_technology_v<Lyt>, "Lyt is not an SiDB layout");

    displacement_robustness_stats                        st{};
    detail::displacement_robustness_domain_impl<Lyt, TT> p{layout, spec, params, st};

    return p.determine_robustness_domain_p_error_rate(error_p);
}

}  // namespace fiction

#endif  // FICTION_ASSESS_DISPLACEMENT_ROBUSTNESS_HPP
