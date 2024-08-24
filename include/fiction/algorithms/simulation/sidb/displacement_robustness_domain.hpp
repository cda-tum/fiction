//
// Created by Jan Drewniok on 21.03.24.
//

#ifndef FICTION_DISPLACEMENT_ROBUSTNESS_DOMAIN_HPP
#define FICTION_DISPLACEMENT_ROBUSTNESS_DOMAIN_HPP

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
#include <numeric>
#include <random>
#include <set>
#include <thread>
#include <utility>
#include <vector>

namespace fiction
{

/**
 * During fabrication, SiDBs may not align precisely with their intended atomic positions, resulting in displacement.
 * This means that an SiDB is fabricated close to the desired one, typically one or a few H-Si
 * positions away. Consequently, depending on the fabrication speed, a certain number of SiDBs may experience
 * displacement. To address and analyze this occurrence, we introduce the *Displacement Robustness Domain*. This domain
 * consists of SiDB layouts derived from an original layout, each showing displaced SiDBs, together with the
 * `operational` or `non-operational` status, based on the specified logic.
 */
template <typename Lyt>
struct displacement_robustness_domain
{
    /**
     * Represents a domain of displacement robustness for layouts resulting from applying a displacement to a given SiDB
     * layout.
     */
    std::vector<std::pair<Lyt, operational_status>> operational_values{};
};

/**
 * Parameters for the `determine_displacement_robustness_domain` and
 * `determine_probability_of_fabricating_operational_gate` algorithms.
 *
 * @param CellType SiDB layout cell type.
 */
template <typename CellType>
struct displacement_robustness_domain_params
{
    /**
     * Possible modes to determine the displacement robustness domain.
     */
    enum class displacement_analysis_mode
    {
        /**
         * All possible displacements are analyzed.
         */
        EXHAUSTIVE,
        /**
         * A certain amount of all possible displacements is analyzed randomly. Defined by
         * `percentage_of_analyzed_displaced_layouts`.
         */
        RANDOM,
    };
    /**
     * Specifies the allowed displacement range options for SiDB fabrication simulation.
     */
    enum class dimer_displacement_policy
    {
        /**
         * In this mode, any displacement of SiDBs must remain within the boundaries
         * of the initial dimer they are placed on.
         */
        STAY_ON_ORIGINAL_DIMER,
        /**
         * In this mode, SiDBs are allowed to be displaced from the original dimer
         * to any other dimer within the layout.
         */
        ALLOW_OTHER_DIMER
    };
    /**
     * This parameter defines the mode of the displacement. If `EXHAUSTIVE`, all possible displacements are analyzed.
     * Otherwise, a certain amount of all possible displacements is analyzed randomly.
     */
    displacement_analysis_mode analysis_mode{displacement_analysis_mode::EXHAUSTIVE};
    /**
     * This parameter defines the percentage of all possible displaced SiDB layouts that are analyzed. The default value
     * is 1.0 (100 %), which means that all possible displacements are covered.
     */
    double percentage_of_analyzed_displaced_layouts{1.0};
    /**
     * Possible displacement range of H-Si positions in the x- and y-directions.
     * The default value is (1, 0), which means that displacements of ±1 position in the x-direction are analyzed, with
     * no displacement in the y-direction.
     */
    std::pair<uint64_t, uint64_t> displacement_variations = {1, 0};
    /**
     * Parameters to check the operation status of the SiDB layout.
     */
    is_operational_params operational_params{};
    /**
     * SiDBs in the given layout which shall not be affected by variations.
     */
    std::set<CellType> fixed_sidbs{};
    /**
     * This flag controls whether the displacement in the y-direction can lead to changes in the Si dimer.
     */
    dimer_displacement_policy dimer_policy{dimer_displacement_policy::STAY_ON_ORIGINAL_DIMER};
};

/**
 * Statistics for the displacement robustness domain computation.
 */
struct displacement_robustness_domain_stats
{
    /**
     * Total runtime in seconds to determine the robustness of the passed SiDB layout.
     */
    mockturtle::stopwatch<>::duration time_total{0};
    /**
     * The number of operational SiDB layouts resulting from the given layout by displacements.
     */
    std::size_t num_operational_sidb_displacements{0};
    /**
     * The number of non-operational SiDB layouts resulting from the given layout by displacements.
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
                                        const displacement_robustness_domain_params<cell<Lyt>>& ps,
                                        displacement_robustness_domain_stats&                   st) noexcept :
            layout{lyt},
            params{ps},
            stats{st},
            truth_table{spec},
            generator(rd())
    {
        assert(
            (is_operational(layout, truth_table, params.operational_params).first == operational_status::OPERATIONAL) &&
            "The given layout is not a valid SiDB layout for the given Boolean function");

        if (params.analysis_mode ==
            displacement_robustness_domain_params<cell<Lyt>>::displacement_analysis_mode::RANDOM)
        {
            assert(params.percentage_of_analyzed_displaced_layouts >= 0.0 &&
                   params.percentage_of_analyzed_displaced_layouts <= 1.0 &&
                   "percentage_of_analyzed_displaced_layouts must be between 0.0 and 1.0");
        }

        sidbs_of_the_original_layout.reserve(layout.num_cells());
        layout.foreach_cell([this](const auto& c) { sidbs_of_the_original_layout.push_back(c); });
    };
    /**
     * This function calculates the robustness domain of the SiDB layout based on the provided truth table specification
     * and displacement robustness computation parameters.
     */
    displacement_robustness_domain<Lyt> determine_robustness_domain() noexcept
    {
        mockturtle::stopwatch stop{stats.time_total};
        all_possible_sidb_displacements = calculate_all_possible_displacements_for_each_sidb();

        auto layouts = generate_valid_displaced_sidb_layouts();

        if (layouts.empty())
        {
            return displacement_robustness_domain<Lyt>{};
        }

        // Shuffle the layouts vector to have random displaced layouts
        std::shuffle(layouts.begin(), layouts.end(), generator);

        displacement_robustness_domain<Lyt> domain{};

        std::mutex mutex_to_protect_displacement_robustness_domain{};

        const auto check_operational_status =
            [this, &mutex_to_protect_displacement_robustness_domain, &domain](const Lyt& lyt) noexcept
        {
            const auto op_status = is_operational(lyt, truth_table, params.operational_params);
            {
                const std::lock_guard lock_domain{mutex_to_protect_displacement_robustness_domain};
                update_displacement_robustness_domain(domain, lyt, op_status.first);
            }
        };

        const std::size_t num_threads = std::min(static_cast<std::size_t>(std::thread::hardware_concurrency()),
                                                 static_cast<std::size_t>(layouts.size()));

        // calculate the size of each slice
        const auto slice_size = (layouts.size() + num_threads - 1) / num_threads;

        std::vector<std::thread> threads{};
        threads.reserve(num_threads);

        // launch threads, each with its own slice of random step points
        for (auto i = 0ul; i < num_threads; ++i)
        {
            const auto start = i * slice_size;
            const auto end   = std::min(start + slice_size, layouts.size());

            if (start >= end)
            {
                break;  // no more work to distribute
            }

            threads.emplace_back(
                [start, end, &layouts, &check_operational_status]
                {
                    for (auto it = layouts.cbegin() + static_cast<int64_t>(start);
                         it != layouts.cbegin() + static_cast<int64_t>(end); ++it)
                    {
                        check_operational_status(*it);
                    }
                });
        }

        // wait for all threads to complete
        for (auto& thread : threads)
        {
            if (thread.joinable())
            {
                thread.join();
            }
        }

        return domain;
    }

    /**
     * The manufacturing error rate is highly dependent on the speed of the manufacturing process. Therefore, fast
     * fabrication requires SiDB layouts with high displacement tolerance to ensure functionality in the
     * presence of displacements. This function determines the probability of fabricating an operational SiDB layout for
     * a given fabrication error rate. If the fabrication error rate is 0.0 or negative, it means that the SiDB layout
     * is designed without displacement.
     *
     * @param fabrication_error_rate The fabrication error rate. For example, 0.1 describes that 10% of all manufactured
     *        SiDBs have a slight displacement.
     * @return Probability of fabricating a working SiDB gate implementation.
     */
    [[nodiscard]] double determine_probability_of_fabricating_operational_gate(double fabrication_error_rate)
    {
        // The maximum error rate is 1.0.
        fabrication_error_rate = std::min(1.0, fabrication_error_rate);
        // if the error rate is 0.0 or negative, it means that the SiDB gate implementation is designed without a
        // displacement. Hence, it works properly.
        if (fabrication_error_rate < std::numeric_limits<double>::epsilon())
        {
            return 1.0;
        }

        const auto number_of_displaced_sidbs =
            static_cast<uint64_t>(static_cast<double>(sidbs_of_the_original_layout.size()) * fabrication_error_rate);

        if (number_of_displaced_sidbs == 0)
        {
            assert(false && "The error rate is too small for the given SiDB layout. Hence, no SiDB is misplaced");
            return 1.0;
        }

        const auto all_combinations_of_fabricating_misplaced_sidbs =
            determine_all_combinations_of_distributing_k_entities_on_n_positions(number_of_displaced_sidbs,
                                                                                 sidbs_of_the_original_layout.size());

        const auto number_of_maximal_tested_misplaced_cell_combinations =
            std::max(uint64_t{1},
                     static_cast<uint64_t>(static_cast<double>(all_combinations_of_fabricating_misplaced_sidbs.size()) *
                                           std::min(params.percentage_of_analyzed_displaced_layouts, 1.0)));

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
                params.fixed_sidbs.insert(c);
            }
            for (const auto& c : fixed_c_indices)
            {
                const auto cells = sidbs_of_the_original_layout[c];
                params.fixed_sidbs.erase(cells);
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
     * The SiDB layout for which the displacement robustness calculation is performed.
     */
    const Lyt& layout;
    /**
     * The parameters for the displacement robustness computation.
     */
    displacement_robustness_domain_params<cell<Lyt>> params;
    /**
     * The statistics of the displacement robustness computation.
     */
    displacement_robustness_domain_stats& stats;
    /**
     * This stores all possible displacements for all SiDBs in the SiDB layout. This means e.g. the first vector
     * describes all possible positions of the first SiDB due to the allowed/possible displacements.
     */
    std::vector<std::vector<cell<Lyt>>> all_possible_sidb_displacements{};
    /**
     * SiDB positions of the originally given SiDB layout.
     */
    std::vector<cell<Lyt>> sidbs_of_the_original_layout{};
    /**
     * The logical specification of the layout.
     */
    const std::vector<TT>& truth_table;
    /**
     * Random device for obtaining seed for the random number generator.
     * Provides a source of quasi-non-deterministic pseudo-random numbers.
     */
    std::random_device rd{};
    /**
     * Mersenne Twister random number generator.
     * Generates high-quality pseudo-random numbers using a random seed from 'rd'.
     */
    std::mt19937 generator;
    /**
     * This function calculates all permitted displacements for each SiDB based on the specified allowed displacements.
     *
     * @return A vector containing all possible displacements for each SiDB.
     */
    [[nodiscard]] std::vector<std::vector<cell<Lyt>>> calculate_all_possible_displacements_for_each_sidb() noexcept
    {
        std::vector<std::vector<cell<Lyt>>> all_possible_sidb_misplacements = {};

        all_possible_sidb_misplacements.reserve(layout.num_cells());

        layout.foreach_cell(
            [&](const auto& c)
            {
                if constexpr (has_siqad_coord_v<Lyt>)
                {
                    auto new_pos_se = fiction::siqad::to_fiction_coord<cube::coord_t>(c);
                    auto new_pos_nw = fiction::siqad::to_fiction_coord<cube::coord_t>(c);
                    // the cell c is not a fixed cell, i.e., displacement is considered.

                    if (params.fixed_sidbs.find(c) == params.fixed_sidbs.cend())
                    {
                        new_pos_se.x += static_cast<decltype(new_pos_se.x)>(params.displacement_variations.first);
                        new_pos_nw.x -= static_cast<decltype(new_pos_nw.x)>(params.displacement_variations.first);

                        if (params.dimer_policy == displacement_robustness_domain_params<
                                                       cell<Lyt>>::dimer_displacement_policy::STAY_ON_ORIGINAL_DIMER &&
                            params.displacement_variations.second > 0)
                        {
                            new_pos_nw.y =
                                fiction::siqad::to_fiction_coord<cube::coord_t>(fiction::siqad::coord_t{c.x, c.y, 0}).y;
                            new_pos_se.y =
                                fiction::siqad::to_fiction_coord<cube::coord_t>(fiction::siqad::coord_t{c.x, c.y, 1}).y;
                        }
                        else
                        {
                            new_pos_se.y += static_cast<decltype(new_pos_se.y)>(params.displacement_variations.second);
                            new_pos_nw.y -= static_cast<decltype(new_pos_nw.y)>(params.displacement_variations.second);
                        }
                    }

                    const auto all_coord = all_coordinates_in_spanned_area<cell<Lyt>>(
                        fiction::siqad::to_siqad_coord(new_pos_nw), fiction::siqad::to_siqad_coord(new_pos_se));
                    all_possible_sidb_misplacements.push_back(all_coord);
                }

                else
                {
                    auto new_pos_se = c;
                    auto new_pos_nw = c;
                    // the cell c is not a fixed cell, i.e., displacement is considered.

                    // if the cell is not fixed, it can be displaced.
                    if (params.fixed_sidbs.find(c) == params.fixed_sidbs.cend())
                    {
                        // treating the x-coordinate
                        if constexpr (has_offset_ucoord_v<Lyt>)
                        {
                            // ensuring that the displacement does not exceed the boundaries of the layout/coordinate
                            // system
                            if (new_pos_nw.x <
                                static_cast<decltype(new_pos_se.x)>(params.displacement_variations.first))
                            {
                                new_pos_nw.x = 0;
                            }
                            else
                            {
                                new_pos_nw.x -=
                                    static_cast<decltype(new_pos_nw.x)>(params.displacement_variations.first);
                            }
                            new_pos_se.x += static_cast<decltype(new_pos_se.x)>(params.displacement_variations.first);
                        }
                        else
                        {
                            new_pos_nw.x -= static_cast<decltype(new_pos_nw.x)>(params.displacement_variations.first);
                            new_pos_se.x += static_cast<decltype(new_pos_se.x)>(params.displacement_variations.first);
                        }

                        // treating the y-coordinate
                        if (params.dimer_policy == displacement_robustness_domain_params<
                                                       cell<Lyt>>::dimer_displacement_policy::STAY_ON_ORIGINAL_DIMER &&
                            params.displacement_variations.second > 0)
                        {
                            auto new_pos_se_siqad = fiction::siqad::to_siqad_coord(c);
                            auto new_pos_nw_siqad = fiction::siqad::to_siqad_coord(c);

                            new_pos_se_siqad.z = 0;
                            new_pos_nw_siqad.z = 1;

                            new_pos_nw = fiction::siqad::to_fiction_coord<cell<Lyt>>(new_pos_nw_siqad);
                            new_pos_se = fiction::siqad::to_fiction_coord<cell<Lyt>>(new_pos_se_siqad);
                        }

                        else
                        {
                            if constexpr (has_offset_ucoord_v<Lyt>)
                            {
                                if (new_pos_nw.y <
                                    static_cast<decltype(new_pos_nw.y)>(params.displacement_variations.second))
                                {
                                    new_pos_nw.y = 0;
                                }
                                else
                                {
                                    new_pos_nw.y -=
                                        static_cast<decltype(new_pos_nw.y)>(params.displacement_variations.second);
                                }
                                new_pos_se.y +=
                                    static_cast<decltype(new_pos_se.y)>(params.displacement_variations.second);
                            }
                            else
                            {
                                new_pos_nw.y -=
                                    static_cast<decltype(new_pos_nw.y)>(params.displacement_variations.second);
                                new_pos_se.y +=
                                    static_cast<decltype(new_pos_se.y)>(params.displacement_variations.second);
                            }
                        }
                    }

                    const auto all_coord = all_coordinates_in_spanned_area<cell<Lyt>>(new_pos_se, new_pos_nw);
                    all_possible_sidb_misplacements.push_back(all_coord);
                }
            });

        return all_possible_sidb_misplacements;
    }
    /**
     * This function generates all SiDB layouts with displacements based on the original layout.
     * It filters out layouts where two or more SiDBs would be on the same spot due to displacement.
     *
     * @return A vector containing all valid SiDB layouts with displacements.
     */
    [[nodiscard]] std::vector<Lyt> generate_valid_displaced_sidb_layouts() noexcept
    {
        auto all_possible_sidb_displacement = cartesian_combinations(all_possible_sidb_displacements);

        std::shuffle(all_possible_sidb_displacement.begin(), all_possible_sidb_displacement.end(), generator);

        std::vector<Lyt> layouts{};
        layouts.reserve(all_possible_sidb_displacement.size());

        std::size_t number_of_layouts_with_displaced_sidbs = 0;

        auto max_number_of_layouts_with_displaced_sidbs = all_possible_sidb_displacement.size();

        if (params.analysis_mode ==
            displacement_robustness_domain_params<cell<Lyt>>::displacement_analysis_mode::RANDOM)
        {
            // the "1" is used so that at least one displaced layout is analyzed.
            max_number_of_layouts_with_displaced_sidbs =
                std::max(std::size_t{1},
                         static_cast<std::size_t>(static_cast<double>(all_possible_sidb_displacement.size()) *
                                                  std::min(params.percentage_of_analyzed_displaced_layouts, 1.0)));
        }

        for (const auto& cell_displacements : all_possible_sidb_displacement)
        {
            if (number_of_layouts_with_displaced_sidbs >= max_number_of_layouts_with_displaced_sidbs ||
                ((number_of_layouts_with_displaced_sidbs == 1) && (max_number_of_layouts_with_displaced_sidbs == 1)))
            {
                break;
            }

            Lyt displaced_lyt{};

            for (std::size_t i = 0; i < cell_displacements.size(); ++i)
            {
                displaced_lyt.assign_cell_type(cell_displacements[i],
                                               layout.get_cell_type(sidbs_of_the_original_layout[i]));
            }

            if (displaced_lyt.num_cells() == layout.num_cells())
            {
                layouts.push_back(displaced_lyt);
            }
            number_of_layouts_with_displaced_sidbs++;
        }

        return layouts;
    }
    /**
     * This function adds the provided layout and its corresponding operational status to the list of
     * operational values in the displacement robustness domain. Depending on the operational status,
     * it also updates the count of operational or non-operational SiDB displacements in the statistics.
     *
     * @tparam Lyt SiDB cell-layout type.
     * @param domain The displacement robustness domain to be updated.
     * @param lyt The SiDB layout to be added.
     * @param status The operational status of the provided layout.
     */
    void update_displacement_robustness_domain(displacement_robustness_domain<Lyt>& domain, const Lyt& lyt,
                                               const operational_status status) noexcept
    {
        domain.operational_values.emplace_back(lyt, status);

        if (status == operational_status::OPERATIONAL)
        {
            stats.num_operational_sidb_displacements++;
        }
        else
        {
            stats.num_non_operational_sidb_displacements++;
        }
    };
};

}  // namespace detail

/**
 * During fabrication, SiDBs may not align precisely with their intended atomic positions, resulting in displacement.
 * This means that an SiDB is fabricated close to the desired one, typically one or a few H-Si
 * positions away. Consequently, depending on the fabrication speed, a certain number of SiDBs may experience
 * displacement.
 *
 * This function determines the operational status of all possible displacements of the SiDBs of the given SiDB layout,
 * based on the provided truth table specification and displacement robustness computation parameters.
 * The number of displacements grows exponentially with the number of SiDBs. For small layouts, all displacements
 * can be analyzed. For larger layouts, random sampling can be applied, controllable by the `analysis_mode` and
 * `percentage_of_analyzed_displaced_layouts` in `params.
 *
 * @tparam Lyt The SiDB cell-level layout type.
 * @tparam TT Truth table type.
 * @param truth_table_spec Vector of truth table specifications.
 * @param params Parameters for the displacement robustness computation.
 * @param stats Statistics related to the displacement robustness computation.
 * @return The displacement robustness domain of the SiDB layout.
 */
template <typename Lyt, typename TT>
[[nodiscard]] displacement_robustness_domain<Lyt>
determine_displacement_robustness_domain(const Lyt& layout, const std::vector<TT>& spec,
                                         const displacement_robustness_domain_params<cell<Lyt>>& params = {},
                                         displacement_robustness_domain_stats*                   stats  = nullptr)
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
 * During fabrication, SiDBs may not align precisely with their intended atomic positions, resulting in displacement.
 * This means that an SiDB is fabricated close to the desired one, typically one or a few H-Si
 * positions away. The percentage of displaced SiDBs depends on the fabrication speed. Therefore, SiDB layouts with high
 * displacement tolerance are preferred to speed up the fabrication process.
 *
 * This function calculates the probability of
 * fabricating an operational SiDB layout for an originally given SiDB layout and a given fabrication error rate. A
 * fabrication error rate of 0.0 or negative indicates that the SiDB layout is designed without displacement.
 *
 * @tparam Lyt The SiDB cell-level layout type.
 * @tparam TT The type of the truth table.
 * @param layout The SiDB cell-level layout which is analyzed.
 * @param spec Vector of truth table specifications.
 * @param params Parameters for the displacement robustness computation.
 * @param fabrication_error_rate The fabrication error rate. For example, 0.1 describes that 10% of all manufactured
 *        SiDBs have a slight displacement.
 * @return The probability of fabricating an operational SiDB layout.
 */
template <typename Lyt, typename TT>
[[nodiscard]] double determine_probability_of_fabricating_operational_gate(
    const Lyt& layout, const std::vector<TT>& spec, const displacement_robustness_domain_params<cell<Lyt>>& params = {},
    const double fabrication_error_rate = 1.0)
{
    static_assert(is_cell_level_layout_v<Lyt>, "Lyt is not a cell-level layout");
    static_assert(has_sidb_technology_v<Lyt>, "Lyt is not an SiDB layout");

    displacement_robustness_domain_stats                 st{};
    detail::displacement_robustness_domain_impl<Lyt, TT> p{layout, spec, params, st};

    return p.determine_probability_of_fabricating_operational_gate(fabrication_error_rate);
}

}  // namespace fiction

#endif  // FICTION_DISPLACEMENT_ROBUSTNESS_DOMAIN_HPP
