//
// Created by Jan Drewniok on 21.03.24.
//

#ifndef FICTION_DETERMINE_DISPLACEMENT_ROBUSTNESS_HPP
#define FICTION_DETERMINE_DISPLACEMENT_ROBUSTNESS_HPP

#include "fiction/algorithms/simulation/sidb/is_operational.hpp"
#include "fiction/layouts/coordinates.hpp"
#include "fiction/traits.hpp"
#include "fiction/utils/layout_utils.hpp"
#include "fiction/utils/math_utils.hpp"

#include <fmt/format.h>
#include <mockturtle/utils/stopwatch.hpp>

#include <algorithm>
#include <cassert>
#include <cstdint>
#include <cstdlib>
#include <limits>
#include <mutex>
#include <random>
#include <set>
#include <thread>
#include <utility>
#include <vector>

namespace fiction
{

/**
 * A displacement robustness domain encompasses SiDB layouts obtained from an original SiDB layout by displacing
 * individual SiDBs. These layouts remain valid SiDB gate implementations if they fulfill the required logic.
 */
template <typename Lyt>
struct displacement_robustness_domain
{
    /**
     * Represents a domain of displacement robustness for layouts resulting from applying a displacement to a given SiDB
     * layout.
     *
     * @note The original layout is not stored.
     */
    std::vector<std::pair<Lyt, operational_status>> operational_values{};
};
/**
 * Parameters for the `determine_displacement_robustness_domain` and
 * `determine_propability_of_fabricating_operational_gate` algorithm.
 *
 * @param Lyt SiDB cell-level layout type.
 */
template <typename Lyt>
struct displacement_robustness_domain_params
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
    /**
     * This parameter defines the mode of the displacement analysis. The default value is `ALL_POSSIBLE_DISPLACEMENTS`.
     * For faster simulation results, `RANDOM_SAMPLING` can be used.
     */
    displacement_analysis_mode analysis_mode{displacement_analysis_mode::ALL_POSSIBLE_DISPLACEMENTS};
    /**
     * This parameter defines the percentage of all possible displaced SiDB layouts that are analyzed. The default value
     * is 1.0.
     */
    double percentage_of_analyzed_displaced_layouts{1.0};
    /**
     * This parameter defines the percentage of all possible misplaced SiDB combinations that are analyzed. The default
     * value is 1.0.
     */
    double percentage_of_displaced_sidb_combinations{1.0};
    /**
     * The atomic displacement in x- and y-direction, respectively. The default value is (0, 0).
     */
    std::pair<uint64_t, uint64_t> displacement_variations = {0, 0};
    /**
     * Parameters to check the operation status.
     */
    is_operational_params operational_params{};
    /**
     * Cells/SiDBs of the layout which are not affected by variations.
     */
    std::set<cell<Lyt>> fixed_cells{};
    /**
     * This flag controls whether the displacement in the y-direction can lead to changes in the Si-dimer.
     */
    bool allow_dimer_change_in_y_direction = false;
};
/**
 * Statistics for the displacement robustness domain computation.
 */
struct displacement_robustness_domain_stats
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

namespace detail
{

template <typename Lyt, typename TT>
class displacement_robustness_domain_impl
{
  public:
    /**
     * Standard constructor. Initializes the layout, the truth table, the parameters, and the statistics.
     *
     * @param lyt SiDB cell-level layout to be evaluated regarding displacement robustness.
     * @param spec Expected Boolean function of the layout given as a multi-output truth table.
     * @param ps Parameters for the displacement robustness computation.
     * @param st Statistics related to the displacement robustness computation.
     */
    displacement_robustness_domain_impl(const Lyt& lyt, const std::vector<TT>& spec,
                                        const displacement_robustness_domain_params<Lyt>& ps,
                                        displacement_robustness_domain_stats&             st) noexcept :
            layout{lyt},
            params{ps},
            stats{st},
            truth_table{spec},
            generator(rd())
    {
        for ([[maybe_unused]] const auto& c : params.fixed_cells)
        {
            assert(!layout.is_empty_cell(c) && "Not all fixed cells are part of the layout");
        }

        assert(
            (is_operational(layout, truth_table, params.operational_params).first == operational_status::OPERATIONAL) &&
            "The given layout is not a valid SiDB gate implementation");

        sidbs_of_the_original_layout.reserve(layout.num_cells());
        layout.foreach_cell([&](const auto& c) { sidbs_of_the_original_layout.push_back(c); });
        calculate_permitted_displacements_for_each_sidb();
    };
    /**
     * This function calculates the robustness domain of the SiDB gate based on the provided truth table specification
     * and displacement robustness computation parameters.
     */
    displacement_robustness_domain<Lyt> determine_robustness_domain()
    {
        mockturtle::stopwatch stop{stats.time_total};
        calculate_permitted_displacements_for_each_sidb();
        // Shuffle the layouts vector randomly
        std::vector<Lyt> layouts{};

        layouts = generate_valid_displaced_sidb_layouts();

        std::shuffle(layouts.begin(), layouts.end(), generator);

        displacement_robustness_domain<Lyt> domain{};

        const auto status_original_layout = is_operational(layout, truth_table, params.operational_params).first;

        domain.operational_values.emplace_back(layout, status_original_layout);

        if (status_original_layout == operational_status::OPERATIONAL)
        {
            stats.num_operational_sidb_displacements++;
        }
        else
        {
            stats.num_non_operational_sidb_displacements++;
        }

        std::mutex mutex_to_protect_shared_resources;

        const auto check_operation_status =
            [this, &mutex_to_protect_shared_resources, &domain](const std::vector<Lyt>& layouts) noexcept
        {
            for (const auto& lyt : layouts)
            {
                const auto op_status = is_operational(lyt, truth_table, params.operational_params);
                {
                    const std::lock_guard lock_domain{mutex_to_protect_shared_resources};
                    // Store the operational status in the domain
                    domain.operational_values.emplace_back(lyt, op_status.first);
                    if (op_status.first == operational_status::OPERATIONAL)
                    {
                        stats.num_operational_sidb_displacements++;
                    }
                    else
                    {
                        stats.num_non_operational_sidb_displacements++;
                    }
                }
            }
        };

        const std::size_t num_threads = std::max(static_cast<std::size_t>(std::thread::hardware_concurrency()),
                                                 static_cast<std::size_t>(layouts.size()));
        const std::size_t chunk_size  = layouts.size() / num_threads;

        std::vector<std::thread> threads;
        threads.reserve(num_threads);

        // Start threads to process layouts in parallel
        auto layouts_start = layouts.begin();
        auto layouts_end   = layouts_start;
        for (std::size_t i = 0; i < num_threads - 1; ++i)
        {
            std::advance(layouts_end, chunk_size);
            threads.emplace_back(check_operation_status, std::vector<Lyt>{layouts_start, layouts_end});
            layouts_start = layouts_end;
        }
        // Remaining layouts are assigned to the last thread
        threads.emplace_back(check_operation_status, std::vector<Lyt>{layouts_start, layouts.end()});

        // Wait for all threads to finish
        for (auto& thread : threads)
        {
            thread.join();
        }

        return domain;
    }

    /**
     * The manufacturing error rate is highly dependent on the speed of the manufacturing process. Therefore, fast
     * fabrication requires SiDB gates with high displacement tolerance to ensure functionality in the presence of
     * displacements. This function determines the probability of fabricating a working gate for a given SiDB gate and
     * fabrication error rate. If the fabrication error rate is 0.0 or negative, it means that the gate is designed
     * without displacement, hence it works properly.
     */
    [[nodiscard]] double determine_propability_of_fabricating_operational_gate(const double fabrication_error_rate)
    {
        // if the error rate is 0.0 or negative, it means that the gate is designed without a displacement.
        // Hence, it works properly.
        if (fabrication_error_rate < std::numeric_limits<double>::epsilon())
        {
            return 1.0;
        }

        const auto number_of_displaced_sidbs =
            static_cast<uint64_t>(static_cast<double>(sidbs_of_the_original_layout.size()) * fabrication_error_rate);
        if (number_of_displaced_sidbs == 0)
        {
            assert(false && "The error rate is too small for the given SiDB gate. Hence, no SiDB is misplaced");
            return 1.0;
        }
        const auto all_combinations_of_fabricating_misplaced_sidbs =
            determine_all_combinations_of_distributing_k_entities_on_n_positions(number_of_displaced_sidbs,
                                                                                 sidbs_of_the_original_layout.size());

        const auto number_of_maximal_tested_misplaced_cell_combinations =
            std::max(uint64_t{2},
                     static_cast<uint64_t>(static_cast<double>(all_combinations_of_fabricating_misplaced_sidbs.size()) *
                                           params.percentage_of_displaced_sidb_combinations));
        // the "2" is used so that at least one further displaced layout is analyzed in addition to the original SiDB
        // layout.

        uint64_t number_of_tested_misplaced_cell_combinations = 0;

        for (const auto& fixed_c_indices : all_combinations_of_fabricating_misplaced_sidbs)
        {
            if (number_of_tested_misplaced_cell_combinations >= number_of_maximal_tested_misplaced_cell_combinations)
            {
                break;
            }
            // all cells are fixed initially.
            for (const auto& c : sidbs_of_the_original_layout)
            {
                params.fixed_cells.insert(c);
            }
            for (const auto& c : fixed_c_indices)
            {
                const auto cells = sidbs_of_the_original_layout[c];
                params.fixed_cells.erase(cells);
            }

            determine_robustness_domain();

            number_of_tested_misplaced_cell_combinations++;
        }

        return static_cast<double>(stats.num_operational_sidb_displacements) /
               static_cast<double>(stats.num_non_operational_sidb_displacements +
                                   stats.num_operational_sidb_displacements);
    }

  private:
    /**
     * This function calculates all permitted displacements for each SiDB based on the specified allowed displacements.
     */
    void calculate_permitted_displacements_for_each_sidb() noexcept
    {
        std::vector<std::vector<cell<Lyt>>> all_possible_sidb_misplacements = {};
        all_possible_sidb_misplacements.reserve(layout.num_cells());
        layout.foreach_cell(
            [&](const auto& c)
            {
                if constexpr (has_siqad_coord_v<Lyt>)
                {
                    auto new_pos_se = siqad::to_fiction_coord<cube::coord_t>(c);
                    auto new_pos_nw = siqad::to_fiction_coord<cube::coord_t>(c);
                    // the cell c is not a fixed cell, i.e., displacement is considered.
                    if (params.fixed_cells.find(c) == params.fixed_cells.end())
                    {
                        new_pos_se.x -= static_cast<decltype(new_pos_se.x)>(params.displacement_variations.first);
                        new_pos_nw.x += static_cast<decltype(new_pos_nw.x)>(params.displacement_variations.first);
                        if (params.allow_dimer_change_in_y_direction)
                        {
                            new_pos_nw.y = siqad::to_fiction_coord<cube::coord_t>(siqad::coord_t{c.x, c.y, 0}).y;
                            new_pos_se.y = siqad::to_fiction_coord<cube::coord_t>(siqad::coord_t{c.x, c.y, 1}).y;
                        }
                        new_pos_se.y -= static_cast<decltype(new_pos_se.y)>(params.displacement_variations.second);
                        new_pos_nw.y += static_cast<decltype(new_pos_nw.y)>(params.displacement_variations.second);
                    }
                    const auto all_coord = all_coordinates_in_spanned_area<cell<Lyt>>(
                        siqad::to_siqad_coord(new_pos_se), siqad::to_siqad_coord(new_pos_nw));
                    all_possible_sidb_misplacements.push_back(all_coord);
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
                    all_possible_sidb_misplacements.push_back(all_coord);
                }
            });
        all_possible_displacements_for_each_sidb = all_possible_sidb_misplacements;
    }
    /**
     * This is a helper function, which recursively generates combinations of cell/SiDB displacements for all SiDBs
     * based on the provided vector of displacement vectors.
     *
     * @param result The vector to store the generated combinations.
     * @param current_combination The current combination being constructed.
     * @param index The current index in the vector of displacement vectors.
     */
    void generate_combinations(std::vector<std::vector<cell<Lyt>>>& result, std::vector<cell<Lyt>>& current_combination,
                               std::size_t index) noexcept
    {
        if (index == all_possible_displacements_for_each_sidb.size())
        {
            result.push_back(current_combination);
            return;
        }

        for (const auto& c : all_possible_displacements_for_each_sidb[index])
        {
            current_combination.push_back(c);
            generate_combinations(result, current_combination, index + 1);
            current_combination.pop_back();
        }
    }
    /**
     * This function generates all SiDB layouts with displacements based on the original layout.
     * It filters out layouts where two or more SiDBs would be on the same spot due to displacement.
     *
     * @return A vector containing all valid SiDB layouts with displacements.
     */
    [[nodiscard]] std::vector<Lyt> generate_valid_displaced_sidb_layouts()
    {
        std::vector<std::vector<cell<Lyt>>> all_possible_sidb_displacement{};
        std::vector<cell<Lyt>>              current_combination{};
        generate_combinations(all_possible_sidb_displacement, current_combination, 0);

        std::shuffle(all_possible_sidb_displacement.begin(), all_possible_sidb_displacement.end(), generator);

        std::vector<Lyt> layouts{};
        layouts.reserve(all_possible_sidb_displacement.size());

        std::size_t number_of_layouts_with_displaced_sidbs = 0;

        double percentage_of_layouts = 1.0;
        if (params.analysis_mode ==
            displacement_robustness_domain_params<Lyt>::displacement_analysis_mode::RANDOM_SAMPLING)
        {
            percentage_of_layouts = params.percentage_of_analyzed_displaced_layouts;
        }

        assert(percentage_of_layouts >= 0.0 && percentage_of_layouts <= 1.0 &&
               "percentage_of_analyzed_displaced_layouts must be between 0.0 and 1.0");

        const auto max_number_of_layouts_with_displaced_sidbs =
            std::max(static_cast<std::size_t>(2),
                     static_cast<std::size_t>(percentage_of_layouts *
                                              static_cast<double>(all_possible_sidb_displacement.size())));
        // the "2" is used so that at least one further displaced layout is analyzed in addition to the original SiDB
        // layout.

        for (const auto& cell_displacements : all_possible_sidb_displacement)
        {
            if (number_of_layouts_with_displaced_sidbs >= max_number_of_layouts_with_displaced_sidbs)
            {
                break;
            }
            Lyt         lyt{};
            std::size_t identical_cells_to_original_layout = 0;
            for (std::size_t i = 0; i < cell_displacements.size(); ++i)
            {
                lyt.assign_cell_type(cell_displacements[i], layout.get_cell_type(sidbs_of_the_original_layout[i]));
                if (!layout.is_empty_cell(cell_displacements[i]))
                {
                    identical_cells_to_original_layout++;
                }
            }
            if (lyt.num_cells() == layout.num_cells() && identical_cells_to_original_layout != layout.num_cells())
            {
                layouts.push_back(lyt);
            }
            number_of_layouts_with_displaced_sidbs++;
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
    displacement_robustness_domain_params<Lyt> params;
    /**
     * The statistics of the displacement robustness computation.
     */
    displacement_robustness_domain_stats& stats;
    /**
     * This stores all possible displacements for all SiDBs in the SiDB layout. This means e.g. the first vector
     * describes all possible positions of the first SiDB due to the allowed/possible displacements.
     */
    std::vector<std::vector<cell<Lyt>>> all_possible_displacements_for_each_sidb{};
    /**
     * SiDB positions of the originally given SiDB layout.
     */
    std::vector<cell<Lyt>> sidbs_of_the_original_layout{};
    /**
     * The specification of the layout.
     */
    const std::vector<TT>& truth_table;
    /**
     * Random device for obtaining seed for the random number generator.
     * Provides a source of non-deterministic random numbers.
     */
    std::random_device rd;
    /**
     * Mersenne Twister random number generator.
     * Generates high-quality pseudo-random numbers using a random seed from 'rd'.
     */
    std::mt19937 generator;
};

}  // namespace detail

/**
 * This function determines the operational status of all possible displacements of the SiDBs of the given SiDB gate,
 * based on the provided truth table specification and displacement robustness computation parameters.
 * The number of displacements grows exponentially with the number of SiDBs. For small layouts, all displacements
 * can be analyzed. For larger layouts, random sampling can be applied, controllable by the `analysis_mode` and
 * `percentage_of_analyzed_displaced_layouts` in `params.
 *
 * @tparam Lyt The SiDB cell-level layout type.
 * @tparam TT The type of the truth table specifying the gate behavior.
 * @param truth_table_spec Vector of truth table specifications.
 * @param params Parameters for the displacement robustness computation.
 * @param stats Statistics related to the displacement robustness computation.
 * @return The displacement robustness domain of the SiDB gate.
 */
template <typename Lyt, typename TT>
[[nodiscard]] displacement_robustness_domain<Lyt>
determine_displacement_robustness_domain(const Lyt& layout, const std::vector<TT>& spec,
                                         const displacement_robustness_domain_params<Lyt>& params = {},
                                         displacement_robustness_domain_stats*             stats  = nullptr)
{
    static_assert(is_cell_level_layout_v<Lyt>, "Lyt is not a cell-level layout");
    static_assert(has_sidb_technology_v<Lyt>, "Lyt is not an SiDB layout");

    displacement_robustness_domain_stats                 st{};
    detail::displacement_robustness_domain_impl<Lyt, TT> p{layout, spec, params, st};

    const auto result = p.determine_robustness_domain();

    if (stats)
    {
        *stats = st;
    }

    return result;
}
/**
 * The manufacturing error rate is highly dependent on the speed of the manufacturing process. Therefore, fast
 * fabrication requires gates with high displacement tolerance to ensure functionality in the presence of displacements.
 * This function determines the probability of fabricating a working gate for a given SiDB gate and fabrication error
 * rate. If the fabrication error rate is 0.0 or negative, it means that the gate is designed without displacement,
 * hence it works properly.
 *
 * @tparam Lyt The SiDB cell-level layout type.
 * @tparam TT The type of the truth table specifying the gate behavior.
 * @param layout The SiDB cell-level layout which is analyzed.
 * @param spec Vector of truth table specifications.
 * @param params Parameters for the displacement robustness computation.
 * @param fabrication_error_rate The fabrication error rate. For example, 0.1 describes that 10% of all manufactured
 *        SiDBs have a slight displacement.
 * @return The displacement robustness of the SiDB gate.
 */
template <typename Lyt, typename TT>
[[nodiscard]] double
determine_propability_of_fabricating_operational_gate(const Lyt& layout, const std::vector<TT>& spec,
                                                      const displacement_robustness_domain_params<Lyt>& params = {},
                                                      const double fabrication_error_rate                      = 1.0)
{
    static_assert(is_cell_level_layout_v<Lyt>, "Lyt is not a cell-level layout");
    static_assert(has_sidb_technology_v<Lyt>, "Lyt is not an SiDB layout");

    displacement_robustness_domain_stats                 st{};
    detail::displacement_robustness_domain_impl<Lyt, TT> p{layout, spec, params, st};

    return p.determine_propability_of_fabricating_operational_gate(fabrication_error_rate);
}

}  // namespace fiction

#endif  // FICTION_DETERMINE_DISPLACEMENT_ROBUSTNESS_HPP
