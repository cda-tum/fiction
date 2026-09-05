/*
 * Copyright (c) 2018 - 2023 Marcel Walter
 * Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
 * All rights reserved.
 *
 * SPDX-License-Identifier: MIT
 *
 * Licensed under the MIT License
 */

/**
 * @file
 * @brief Robustness of an SiDB layout against fabrication displacement of its dots.
 * @author Jan Drewniok (Drewniok)
 * @author Marcel Walter (marcelwa)
 */

#pragma once

#include "fiction/technology/sidb/lattice.hpp"
#include "fiction/technology/sidb/layout.hpp"
#include "fiction/technology/sidb/simulation/logic/is_operational.hpp"
#include "fiction/utils/math/combination_utils.hpp"
#include "fiction/utils/math/math_utils.hpp"

#include <kitty/traits.hpp>
#include <mockturtle/utils/stopwatch.hpp>

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <mutex>
#include <random>
#include <set>
#include <thread>
#include <utility>
#include <vector>

namespace fiction::sidb::simulation::defects
{

/**
 * The displacement robustness domain of an SiDB gate: every analyzed displaced layout together with its
 * operational status.
 */
struct displacement_robustness_domain
{
    /**
     * The displaced layouts and their operational status.
     */
    std::vector<std::pair<layout, logic::operational_status>> operational_values{};
};

/**
 * Parameters of the displacement robustness analysis.
 */
struct displacement_robustness_domain_params
{
    /**
     * Whether every displaced layout is analyzed or only a random share.
     */
    enum class displacement_analysis_mode : uint8_t
    {
        /**
         * All displaced layouts are analyzed.
         */
        EXHAUSTIVE,
        /**
         * A random share of the displaced layouts is analyzed.
         */
        RANDOM,
    };
    /**
     * Whether a displaced SiDB may leave its dimer.
     */
    enum class dimer_displacement_policy : uint8_t
    {
        /**
         * The SiDB stays on its dimer: only the two sites of the dimer are possible positions.
         */
        STAY_ON_ORIGINAL_DIMER,
        /**
         * The SiDB may be displaced across dimers.
         */
        ALLOW_OTHER_DIMER
    };
    /**
     * The analysis mode.
     */
    displacement_analysis_mode analysis_mode{displacement_analysis_mode::EXHAUSTIVE};
    /**
     * Share of the displaced layouts to analyze in `RANDOM` mode.
     */
    double percentage_of_analyzed_displaced_layouts{1.0};
    /**
     * Maximum displacement in columns and rows.
     */
    std::pair<uint64_t, uint64_t> displacement_variations = {1, 0};
    /**
     * Parameters of the operational check.
     */
    logic::is_operational_params operational_params{};
    /**
     * SiDBs that are not displaced.
     */
    std::set<lattice_site> fixed_sidbs{};
    /**
     * The dimer policy.
     */
    dimer_displacement_policy dimer_policy{dimer_displacement_policy::STAY_ON_ORIGINAL_DIMER};
    /**
     * Number of threads to use.
     */
    std::size_t number_of_threads{std::max(std::size_t{std::thread::hardware_concurrency()}, std::size_t{1})};
};

/**
 * Statistics of the displacement robustness analysis.
 */
struct displacement_robustness_domain_stats
{
    /**
     * Total runtime.
     */
    mockturtle::stopwatch<>::duration time_total{0};
    /**
     * Number of operational displaced layouts.
     */
    std::size_t num_operational_sidb_displacements{0};
    /**
     * Number of non-operational displaced layouts.
     */
    std::size_t num_non_operational_sidb_displacements{0};
};

namespace detail
{

/**
 * Implementation of the displacement robustness analysis. Displacements are measured in columns and rows
 * (`2y + z`), so a displacement of one row moves an SiDB to the other site of its dimer.
 *
 * @tparam TT Truth table type.
 */
template <typename TT>
class displacement_robustness_domain_impl
{
  public:
    /**
     * Constructor.
     *
     * @param lyt The operational gate layout.
     * @param spec The Boolean function(s) it implements.
     * @param ps Parameters.
     * @param st Statistics.
     */
    displacement_robustness_domain_impl(const layout& lyt, const std::vector<TT>& spec,
                                        displacement_robustness_domain_params ps,
                                        displacement_robustness_domain_stats& st) :
            layout_to_analyze{lyt},
            params{std::move(ps)},
            stats{st},
            truth_table{spec},
            generator(rd())
    {
        assert((logic::is_operational(layout_to_analyze, truth_table, params.operational_params).first ==
                logic::operational_status::OPERATIONAL) &&
               "The given layout is not a valid SiDB layout for the given Boolean function");

        if (params.analysis_mode == displacement_robustness_domain_params::displacement_analysis_mode::RANDOM)
        {
            assert(params.percentage_of_analyzed_displaced_layouts >= 0.0 &&
                   params.percentage_of_analyzed_displaced_layouts <= 1.0 &&
                   "percentage_of_analyzed_displaced_layouts must be between 0.0 and 1.0");
        }

        sidbs_of_the_original_layout.reserve(layout_to_analyze.num_cells());
        layout_to_analyze.foreach_cell([this](const auto& c) { sidbs_of_the_original_layout.push_back(c); });
    }
    /**
     * Generates the displaced layouts and checks each for operability.
     *
     * @return The displacement robustness domain.
     */
    displacement_robustness_domain determine_robustness_domain()
    {
        const mockturtle::stopwatch stop{stats.time_total};

        all_possible_sidb_displacements = calculate_all_possible_displacements_for_each_sidb();

        auto layouts = generate_valid_displaced_sidb_layouts();

        if (layouts.empty())
        {
            return displacement_robustness_domain{};
        }

        std::ranges::shuffle(layouts, generator);

        displacement_robustness_domain domain{};
        std::mutex                     mutex{};

        const auto check_operational_status = [this, &mutex, &domain](const layout& lyt)
        {
            const auto op_status = logic::is_operational(lyt, truth_table, params.operational_params);

            const std::scoped_lock lock{mutex};
            update_displacement_robustness_domain(domain, lyt, op_status.first);
        };

        const std::size_t requested_threads = std::max(params.number_of_threads, std::size_t{1});
        const std::size_t num_threads       = std::max(std::min(requested_threads, layouts.size()), std::size_t{1});
        const auto        slice_size        = (layouts.size() + num_threads - 1) / num_threads;

        std::vector<std::thread> threads{};
        threads.reserve(num_threads);

        for (std::size_t i = 0; i < num_threads; ++i)
        {
            const auto start = i * slice_size;
            const auto end   = std::min(start + slice_size, layouts.size());

            if (start >= end)
            {
                break;
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

        for (auto& thread : threads)
        {
            thread.join();
        }

        return domain;
    }
    /**
     * Estimates the probability that a fabricated gate is operational when a share of its SiDBs is displaced.
     *
     * @param fabrication_error_rate Share of the SiDBs that are displaced.
     * @return The probability.
     */
    [[nodiscard]] double determine_probability_of_fabricating_operational_gate(double fabrication_error_rate)
    {
        fabrication_error_rate = std::min(1.0, fabrication_error_rate);

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

        const auto all_combinations = utils::math::determine_all_combinations_of_distributing_k_entities_on_n_positions(
            number_of_displaced_sidbs, sidbs_of_the_original_layout.size());

        const auto max_tested = std::max(
            uint64_t{1}, static_cast<uint64_t>(static_cast<double>(all_combinations.size()) *
                                               std::min(params.percentage_of_analyzed_displaced_layouts, 1.0)));

        uint64_t tested = 0;

        for (const auto& displaced_indices : all_combinations)
        {
            if (tested >= max_tested)
            {
                break;
            }

            for (const auto& c : sidbs_of_the_original_layout)
            {
                params.fixed_sidbs.insert(c);
            }
            for (const auto i : displaced_indices)
            {
                params.fixed_sidbs.erase(sidbs_of_the_original_layout[i]);
            }

            static_cast<void>(determine_robustness_domain());

            ++tested;
        }

        return static_cast<double>(stats.num_operational_sidb_displacements) /
               static_cast<double>(stats.num_non_operational_sidb_displacements +
                                   stats.num_operational_sidb_displacements);
    }

  private:
    /**
     * The gate layout.
     */
    const layout& layout_to_analyze;
    /**
     * Parameters.
     */
    displacement_robustness_domain_params params;
    /**
     * Statistics.
     */
    displacement_robustness_domain_stats& stats;
    /**
     * The possible positions of every SiDB.
     */
    std::vector<std::vector<lattice_site>> all_possible_sidb_displacements{};
    /**
     * The SiDBs of the original layout.
     */
    std::vector<lattice_site> sidbs_of_the_original_layout{};
    /**
     * The Boolean function(s).
     */
    const std::vector<TT> truth_table;
    /**
     * Seed source.
     */
    std::random_device rd{};
    /**
     * Random generator for shuffling.
     */
    std::mt19937 generator;
    /**
     * Determines the possible positions of every SiDB: the fixed ones stay, the others may move by up to the
     * displacement variations in columns and rows, or stay on their dimer.
     *
     * @return One position list per SiDB.
     */
    [[nodiscard]] std::vector<std::vector<lattice_site>> calculate_all_possible_displacements_for_each_sidb()
    {
        std::vector<std::vector<lattice_site>> all{};
        all.reserve(layout_to_analyze.num_cells());

        const auto dx = static_cast<int32_t>(params.displacement_variations.first);
        const auto dy = static_cast<int32_t>(params.displacement_variations.second);

        layout_to_analyze.foreach_cell(
            [&](const auto& c)
            {
                if (params.fixed_sidbs.contains(c))
                {
                    all.push_back({c});
                    return;
                }

                const auto row = row_of(c);

                auto min_row = row - dy;
                auto max_row = row + dy;

                if (params.dimer_policy ==
                        displacement_robustness_domain_params::dimer_displacement_policy::STAY_ON_ORIGINAL_DIMER &&
                    dy > 0)
                {
                    min_row = int64_t{2} * c.y;
                    max_row = (int64_t{2} * c.y) + 1;
                }

                all.push_back(sites_in_area(site_at_row(c.x - dx, min_row), site_at_row(c.x + dx, max_row)));
            });

        return all;
    }
    /**
     * Combines the position lists into displaced layouts; combinations in which two SiDBs collide are dropped.
     *
     * @return The displaced layouts.
     */
    [[nodiscard]] std::vector<layout> generate_valid_displaced_sidb_layouts()
    {
        auto combinations = utils::math::cartesian_combinations(all_possible_sidb_displacements);
        std::ranges::shuffle(combinations, generator);

        std::vector<layout> layouts{};
        layouts.reserve(combinations.size());

        std::size_t num_generated = 0;
        auto        max_generated = combinations.size();

        if (params.analysis_mode == displacement_robustness_domain_params::displacement_analysis_mode::RANDOM)
        {
            max_generated =
                std::max(std::size_t{1},
                         static_cast<std::size_t>(static_cast<double>(combinations.size()) *
                                                  std::min(params.percentage_of_analyzed_displaced_layouts, 1.0)));
        }

        for (const auto& cell_displacements : combinations)
        {
            if (num_generated >= max_generated || (num_generated == 1 && max_generated == 1))
            {
                break;
            }

            layout displaced{layout_to_analyze.get_lattice()};

            for (std::size_t i = 0; i < cell_displacements.size(); ++i)
            {
                displaced.assign_cell_type(cell_displacements[i],
                                           layout_to_analyze.get_cell_type(sidbs_of_the_original_layout[i]));
            }

            if (displaced.num_cells() == layout_to_analyze.num_cells())
            {
                layouts.push_back(std::move(displaced));
            }

            ++num_generated;
        }

        return layouts;
    }
    /**
     * Records a verdict.
     *
     * @param domain The domain to extend.
     * @param lyt The displaced layout.
     * @param status Its operational status.
     */
    void update_displacement_robustness_domain(displacement_robustness_domain& domain, const layout& lyt,
                                               const logic::operational_status status)
    {
        domain.operational_values.emplace_back(lyt, status);

        if (status == logic::operational_status::OPERATIONAL)
        {
            ++stats.num_operational_sidb_displacements;
        }
        else
        {
            ++stats.num_non_operational_sidb_displacements;
        }
    }
};

}  // namespace detail

/**
 * Determines the displacement robustness domain of an SiDB gate: every SiDB that is not fixed is displaced by up to
 * the configured number of columns and rows, and every resulting layout is checked for operability.
 *
 * @tparam TT Truth table type.
 * @param lyt The operational gate layout.
 * @param spec The Boolean function(s) it implements.
 * @param params Parameters.
 * @param stats Statistics.
 * @return The displacement robustness domain.
 */
template <typename TT>
[[nodiscard]] displacement_robustness_domain
determine_displacement_robustness_domain(const layout& lyt, const std::vector<TT>& spec,
                                         const displacement_robustness_domain_params& params = {},
                                         displacement_robustness_domain_stats*        stats  = nullptr)
{
    static_assert(kitty::is_truth_table<TT>::value, "TT is not a truth table");

    displacement_robustness_domain_stats            st{};
    detail::displacement_robustness_domain_impl<TT> p{lyt, spec, params, st};

    const auto result = p.determine_robustness_domain();

    if (stats != nullptr)
    {
        *stats = st;
    }

    return result;
}
/**
 * Estimates the probability that a fabricated SiDB gate is operational when a share of its SiDBs is displaced:
 * for every combination of displaced SiDBs, the displaced layouts are checked for operability, and the share of
 * operational ones is the probability.
 *
 * @tparam TT Truth table type.
 * @param lyt The operational gate layout.
 * @param spec The Boolean function(s) it implements.
 * @param params Parameters.
 * @param fabrication_error_rate Share of the SiDBs that are displaced.
 * @return The probability.
 */
template <typename TT>
[[nodiscard]] double
determine_probability_of_fabricating_operational_gate(const layout& lyt, const std::vector<TT>& spec,
                                                      const displacement_robustness_domain_params& params = {},
                                                      const double fabrication_error_rate                 = 1.0)
{
    static_assert(kitty::is_truth_table<TT>::value, "TT is not a truth table");

    displacement_robustness_domain_stats            st{};
    detail::displacement_robustness_domain_impl<TT> p{lyt, spec, params, st};

    return p.determine_probability_of_fabricating_operational_gate(fabrication_error_rate);
}

}  // namespace fiction::sidb::simulation::defects
