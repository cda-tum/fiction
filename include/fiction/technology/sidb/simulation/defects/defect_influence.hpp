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
 * @brief Determines at which positions a defect disturbs an SiDB layout.
 * @author Jan Drewniok (Drewniok)
 * @author Marcel Walter (marcelwa)
 */

#pragma once

#include "fiction/technology/sidb/charge_distribution.hpp"
#include "fiction/technology/sidb/lattice.hpp"
#include "fiction/technology/sidb/layout.hpp"
#include "fiction/technology/sidb/model/defect.hpp"
#include "fiction/technology/sidb/simulation/analysis/can_positive_charges_occur.hpp"
#include "fiction/technology/sidb/simulation/domain.hpp"
#include "fiction/technology/sidb/simulation/engines/quickexact.hpp"
#include "fiction/technology/sidb/simulation/logic/bdl_input_iterator.hpp"
#include "fiction/technology/sidb/simulation/logic/is_operational.hpp"
#include "fiction/technology/sidb/technology.hpp"
#include "fiction/types.hpp"

#include <kitty/traits.hpp>
#include <mockturtle/utils/stopwatch.hpp>

#include <algorithm>
#include <atomic>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <limits>
#include <optional>
#include <random>
#include <stdexcept>
#include <thread>
#include <unordered_set>
#include <utility>
#include <vector>

namespace fiction::sidb::simulation::defects
{

/**
 * Parameters of the defect influence analysis.
 */
struct defect_influence_params
{
    /**
     * What counts as an influence of the defect on the layout.
     */
    enum class influence_definition : uint8_t
    {
        /**
         * The defect changes the operational status of the layout (a specification is required).
         */
        OPERATIONALITY_CHANGE,
        /**
         * The defect changes the ground state of the layout (for every input pattern if a specification is given).
         */
        GROUND_STATE_CHANGE
    };
    /**
     * The defect to place.
     */
    model::defect defect{};
    /**
     * Parameters of the operational check and the simulation.
     */
    logic::is_operational_params operational_params{};
    /**
     * The scanning area extends the layout's bounding box by this many columns and rows in every direction.
     */
    std::pair<int32_t, int32_t> additional_scanning_area{50, 6};
    /**
     * The influence definition.
     */
    influence_definition influence_def{influence_definition::OPERATIONALITY_CHANGE};
    /**
     * Number of threads to use.
     */
    std::size_t number_of_threads{std::max(std::size_t{std::thread::hardware_concurrency()}, std::size_t{1})};
};

/**
 * Whether a defect at a position influences the layout.
 */
enum class defect_influence_status : uint8_t
{
    /**
     * The defect influences the layout.
     */
    INFLUENTIAL,
    /**
     * The defect does not influence the layout.
     */
    NON_INFLUENTIAL
};

/**
 * The influence of a defect on a layout for every evaluated defect position.
 */
class defect_influence_domain : public domain<lattice_site, defect_influence_status>
{};

/**
 * Statistics of the defect influence analysis.
 */
struct defect_influence_stats
{
    /**
     * Total runtime.
     */
    mockturtle::stopwatch<>::duration time_total{0};
    /**
     * Number of simulator invocations.
     */
    std::size_t num_simulator_invocations{0};
    /**
     * Number of evaluated defect positions.
     */
    std::size_t num_evaluated_defect_positions{0};
    /**
     * Number of influencing defect positions.
     */
    std::size_t num_influencing_defect_positions{0};
    /**
     * Number of non-influencing defect positions.
     */
    std::size_t num_non_influencing_defect_positions{0};
};

namespace detail
{

/**
 * Implementation of the defect influence analysis. Defect positions are addressed by column and row (`2y + z`),
 * so the scanning area, the step size, and the neighborhoods of the contour trace are rectangular in rows.
 */
class defect_influence_impl
{
  public:
    /**
     * Constructor.
     *
     * @param lyt The layout to analyze.
     * @param ps Parameters.
     * @param st Statistics.
     */
    defect_influence_impl(const layout& lyt, const defect_influence_params& ps, defect_influence_stats& st) :
            layout_to_analyze{lyt},
            base_layout{lyt},
            params{ps},
            stats{st}
    {
        determine_scanning_area();
    }
    /**
     * Evaluates every position of the scanning area whose column and row are multiples of the step size.
     *
     * @tparam TT Truth table type.
     * @param step_size Step size.
     * @param spec The specification, if the influence definition needs one.
     * @return The defect influence domain.
     */
    template <typename TT = tt>
    [[nodiscard]] defect_influence_domain grid_search(const std::size_t                     step_size,
                                                      const std::optional<std::vector<TT>>& spec = std::nullopt)
    {
        if (step_size == 0)
        {
            throw std::invalid_argument{"step_size must be greater than zero"};
        }

        const mockturtle::stopwatch stop{stats.time_total};

        const auto positions = all_positions();

        run_in_parallel(positions.size(),
                        [this, &positions, step_size, &spec](const std::size_t i)
                        {
                            const auto& p = positions[i];

                            if (static_cast<std::size_t>(std::abs(int64_t{p.x})) % step_size == 0 &&
                                static_cast<std::size_t>(std::abs(row_of(p))) % step_size == 0)
                            {
                                is_defect_influential(spec, p);
                            }
                        });

        log_stats();

        return influence_domain;
    }
    /**
     * Evaluates randomly chosen positions of the scanning area.
     *
     * @tparam TT Truth table type.
     * @param samples Number of positions to evaluate.
     * @param spec The specification, if the influence definition needs one.
     * @return The defect influence domain.
     */
    template <typename TT = tt>
    [[nodiscard]] defect_influence_domain random_sampling(const std::size_t                     samples,
                                                          const std::optional<std::vector<TT>>& spec = std::nullopt)
    {
        const mockturtle::stopwatch stop{stats.time_total};

        auto positions = all_positions();
        std::ranges::shuffle(positions, generator);

        const auto num = std::min(positions.size(), samples);

        run_in_parallel(num,
                        [this, &positions, &spec](const std::size_t i) { is_defect_influential(spec, positions[i]); });

        log_stats();

        return influence_domain;
    }
    /**
     * *QuickTrace*: traces the contour of the influential region around the layout. From a non-influential position
     * at the left edge of the scanning area, the trace moves right until it hits the first influential position and
     * follows the boundary of the influential region clockwise from there.
     *
     * @tparam TT Truth table type.
     * @param samples Number of starting rows to try.
     * @param spec The specification, if the influence definition needs one.
     * @return The defect influence domain.
     */
    template <typename TT = tt>
    [[nodiscard]] defect_influence_domain quicktrace(const std::size_t                     samples,
                                                     const std::optional<std::vector<TT>>& spec = std::nullopt)
    {
        const mockturtle::stopwatch stop{stats.time_total};

        const auto next_clockwise_point = [](std::vector<lattice_site>& neighborhood,
                                             const lattice_site&        backtrack) noexcept -> lattice_site
        {
            assert(std::ranges::find(neighborhood, backtrack) != neighborhood.cend() &&
                   "The backtrack point must be part of the neighborhood");

            while (neighborhood.back() != backtrack)
            {
                std::ranges::rotate(neighborhood, neighborhood.begin() + 1);
            }

            return neighborhood.front();
        };

        std::unordered_set<lattice_site> starting_points{};

        for (std::size_t sample = 0; sample < samples; ++sample)
        {
            const auto operational_starting_point = find_non_influential_defect_position_at_left_side(spec);

            if (!operational_starting_point.has_value())
            {
                return influence_domain;
            }

            if (!starting_points.insert(*operational_starting_point).second)
            {
                continue;
            }

            const auto contour_starting_p =
                find_last_non_influential_defect_position_moving_right(spec, *operational_starting_point);

            if (!contour_starting_p.has_value())
            {
                continue;
            }

            const auto contour_starting_point = *contour_starting_p;

            auto current_contour_point = contour_starting_point;
            auto backtrack_point       = current_contour_point.x == nw_x ?
                                             current_contour_point :
                                             site_at_row(current_contour_point.x - 1, row_of(current_contour_point));

            auto current_neighborhood = moore_neighborhood(current_contour_point);

            if (std::ranges::find(current_neighborhood, backtrack_point) == current_neighborhood.cend())
            {
                continue;
            }

            auto next_point = current_contour_point == backtrack_point ?
                                  current_neighborhood.front() :
                                  next_clockwise_point(current_neighborhood, backtrack_point);

            while (next_point != contour_starting_point)
            {
                const auto status = is_defect_influential(spec, next_point);

                if (status == defect_influence_status::INFLUENTIAL)
                {
                    backtrack_point       = current_contour_point;
                    current_contour_point = next_point;
                }
                else
                {
                    backtrack_point = next_point;
                }

                current_neighborhood = moore_neighborhood(current_contour_point);
                next_point           = next_clockwise_point(current_neighborhood, backtrack_point);
            }
        }

        log_stats();

        return influence_domain;
    }

  private:
    /**
     * The layout to analyze.
     */
    const layout layout_to_analyze;
    /**
     * The layout before placing the candidate defect.
     */
    const layout base_layout;
    /**
     * Parameters.
     */
    const defect_influence_params& params;
    /**
     * Statistics.
     */
    defect_influence_stats& stats;
    /**
     * Bounds of the scanning area in columns and rows.
     */
    int32_t nw_x{}, se_x{};
    int64_t nw_row{}, se_row{};
    /**
     * The defect influence domain under construction.
     */
    defect_influence_domain influence_domain{};
    /**
     * Random generator for the sampling.
     */
    inline static std::mt19937_64 generator{std::random_device{}()};
    /**
     * Number of simulator invocations.
     */
    std::atomic<std::size_t> num_simulator_invocations{0};
    /**
     * Number of evaluated defect positions.
     */
    std::atomic<std::size_t> num_evaluated_defect_positions{0};
    /**
     * Extends the layout's bounding box by the additional scanning area.
     */
    void determine_scanning_area() noexcept
    {
        const auto [nw, se] = layout_to_analyze.bounding_box();

        constexpr auto min_x   = int64_t{std::numeric_limits<int32_t>::min()};
        constexpr auto max_x   = int64_t{std::numeric_limits<int32_t>::max()};
        constexpr auto min_row = int64_t{2} * std::numeric_limits<int32_t>::min();
        constexpr auto max_row = (int64_t{2} * std::numeric_limits<int32_t>::max()) + 1;

        nw_x   = static_cast<int32_t>(std::clamp(int64_t{nw.x} - params.additional_scanning_area.first, min_x, max_x));
        se_x   = static_cast<int32_t>(std::clamp(int64_t{se.x} + params.additional_scanning_area.first, min_x, max_x));
        nw_row = std::clamp(row_of(nw) - params.additional_scanning_area.second, min_row, max_row);
        se_row = std::clamp(row_of(se) + params.additional_scanning_area.second, min_row, max_row);
    }
    /**
     * All positions of the scanning area in raster order.
     *
     * @return The positions.
     */
    [[nodiscard]] std::vector<lattice_site> all_positions() const
    {
        return sites_in_area(site_at_row(nw_x, nw_row), site_at_row(se_x, se_row));
    }
    /**
     * Runs `fn(i)` for `i` in `[0, n)` on the configured number of threads.
     *
     * @tparam Fn Callable type.
     * @param n Number of indices.
     * @param fn The function to run.
     */
    template <typename Fn>
    void run_in_parallel(const std::size_t n, const Fn& fn) const
    {
        const auto number_of_threads =
            std::max(std::min(std::max(params.number_of_threads, std::size_t{1}), n), std::size_t{1});
        const auto slice_size = (n + number_of_threads - 1) / number_of_threads;

        std::vector<std::thread> threads{};
        threads.reserve(number_of_threads);

        for (std::size_t t = 0; t < number_of_threads; ++t)
        {
            const auto start = t * slice_size;
            const auto end   = std::min(start + slice_size, n);

            if (start >= end)
            {
                break;
            }

            threads.emplace_back(
                [start, end, &fn]
                {
                    for (auto i = start; i < end; ++i)
                    {
                        fn(i);
                    }
                });
        }

        for (auto& thread : threads)
        {
            thread.join();
        }
    }
    /**
     * Picks a random row at the left edge of the scanning area and returns it if a defect there is not influential.
     *
     * @tparam TT Truth table type.
     * @param spec The specification.
     * @return The position, or `std::nullopt` if the defect is influential there.
     */
    template <typename TT>
    [[nodiscard]] std::optional<lattice_site>
    find_non_influential_defect_position_at_left_side(const std::optional<std::vector<TT>>& spec)
    {
        std::uniform_int_distribution<int64_t> dist{nw_row, se_row};

        const auto starting_point = site_at_row(nw_x, dist(generator));

        if (is_defect_influential(spec, starting_point) == defect_influence_status::NON_INFLUENTIAL)
        {
            return starting_point;
        }

        return std::nullopt;
    }
    /**
     * Determines whether a defect at `defect_cell` influences the layout and records the verdict.
     *
     * @tparam TT Truth table type.
     * @param spec The specification.
     * @param defect_cell The defect position.
     * @return The verdict.
     */
    template <typename TT>
    defect_influence_status is_defect_influential(const std::optional<std::vector<TT>>& spec,
                                                  const lattice_site&                   defect_cell)
    {
        ++num_evaluated_defect_positions;

        if (const auto op_value = influence_domain.contains(defect_cell); op_value.has_value())
        {
            return std::get<0>(*op_value);
        }

        const auto non_influential = [this, &defect_cell]()
        {
            ++num_simulator_invocations;
            influence_domain.add_value(defect_cell, {defect_influence_status::NON_INFLUENTIAL});

            return defect_influence_status::NON_INFLUENTIAL;
        };

        const auto influential = [this, &defect_cell]()
        {
            ++num_simulator_invocations;
            influence_domain.add_value(defect_cell, {defect_influence_status::INFLUENTIAL});

            return defect_influence_status::INFLUENTIAL;
        };

        // a new defect cannot sit on an SiDB or replace an existing defect
        if (!layout_to_analyze.is_empty_cell(defect_cell) ||
            layout_to_analyze.get_defect(defect_cell).type != model::defect_type::NONE)
        {
            return non_influential();
        }

        if (spec.has_value())
        {
            if (params.influence_def == defect_influence_params::influence_definition::OPERATIONALITY_CHANGE)
            {
                auto lyt_copy = layout_to_analyze;
                lyt_copy.assign_defect(defect_cell, params.defect);

                const auto [status, result] = logic::is_operational(lyt_copy, *spec, params.operational_params);

                return status == logic::operational_status::OPERATIONAL ? non_influential() : influential();
            }

            // ground state change for every input pattern; the defect is added by the comparison
            logic::bdl_input_iterator bii{base_layout, params.operational_params.input_bdl_iterator_params};

            for (auto i = 0u; i < spec->front().num_bits(); ++i, ++bii)
            {
                ++num_simulator_invocations;

                if (does_defect_influence_groundstate(*bii, defect_cell) == defect_influence_status::INFLUENTIAL)
                {
                    return influential();
                }
            }

            return non_influential();
        }

        if (params.influence_def == defect_influence_params::influence_definition::GROUND_STATE_CHANGE)
        {
            return does_defect_influence_groundstate(base_layout, defect_cell) == defect_influence_status::INFLUENTIAL ?
                       influential() :
                       non_influential();
        }

        return non_influential();
    }
    /**
     * Compares the ground states of a layout with and without the defect.
     *
     * @param lyt_without_candidate The layout without the candidate defect.
     * @param defect_pos The defect position.
     * @return Whether the defect changes the ground state.
     */
    [[nodiscard]] defect_influence_status does_defect_influence_groundstate(const layout&       lyt_without_candidate,
                                                                            const lattice_site& defect_pos)
    {
        if (layout_to_analyze.is_empty())
        {
            return defect_influence_status::NON_INFLUENTIAL;
        }

        if (lyt_without_candidate.get_cell_type(defect_pos) != sidb_technology::cell_type::EMPTY)
        {
            return defect_influence_status::NON_INFLUENTIAL;
        }

        const engines::quickexact_params qe_params{
            .sim_params            = params.operational_params.sim_params,
            .base_number_detection = engines::quickexact_params::automatic_base_number_detection::OFF};

        const auto ground_states = engines::quickexact(lyt_without_candidate, qe_params).groundstates();

        auto lyt_defect = lyt_without_candidate;
        lyt_defect.assign_defect(defect_pos, params.defect);

        if (analysis::can_positive_charges_occur(lyt_defect, params.operational_params.sim_params))
        {
            return defect_influence_status::INFLUENTIAL;
        }

        const auto ground_states_defect = engines::quickexact(lyt_defect, qe_params).groundstates();

        if (ground_states.size() != ground_states_defect.size())
        {
            return defect_influence_status::INFLUENTIAL;
        }

        const auto base = params.operational_params.sim_params.base;

        for (const auto& gs_defect : ground_states_defect)
        {
            const auto index = gs_defect.charge_index(base);

            if (!std::ranges::any_of(ground_states,
                                     [index, base](const auto& gs) { return gs.charge_index(base) == index; }))
            {
                return defect_influence_status::INFLUENTIAL;
            }
        }

        return defect_influence_status::NON_INFLUENTIAL;
    }
    /**
     * Moves right from a non-influential position until the defect becomes influential.
     *
     * @tparam TT Truth table type.
     * @param spec The specification.
     * @param starting_defect_position The non-influential starting position.
     * @return The last non-influential position before the influential region, or `std::nullopt` if none is hit.
     */
    template <typename TT>
    [[nodiscard]] std::optional<lattice_site>
    find_last_non_influential_defect_position_moving_right(const std::optional<std::vector<TT>>& spec,
                                                           const lattice_site& starting_defect_position)
    {
        const auto row = row_of(starting_defect_position);

        auto latest_non_influential = starting_defect_position;
        auto previous               = starting_defect_position;
        auto current                = starting_defect_position;

        for (auto x = starting_defect_position.x; x <= se_x; ++x)
        {
            previous = current;
            current  = site_at_row(x, row);

            if (is_defect_influential(spec, current) == defect_influence_status::NON_INFLUENTIAL)
            {
                latest_non_influential = current;
            }
            else
            {
                return previous;
            }
        }

        if (current == latest_non_influential)
        {
            return std::nullopt;
        }

        return latest_non_influential;
    }
    /**
     * Writes the counters into the statistics.
     */
    void log_stats() const noexcept
    {
        stats.num_simulator_invocations      = num_simulator_invocations.load();
        stats.num_evaluated_defect_positions = num_evaluated_defect_positions.load();

        influence_domain.for_each(
            [this](const auto& defect_pos [[maybe_unused]], const auto& status)
            {
                if (std::get<0>(status) == defect_influence_status::INFLUENTIAL)
                {
                    ++stats.num_influencing_defect_positions;
                }
                else
                {
                    ++stats.num_non_influencing_defect_positions;
                }
            });
    }
    /**
     * The empty positions in the Moore neighborhood of `c` within the scanning area, in clockwise order starting
     * east.
     *
     * @param c The position.
     * @return The neighbors.
     */
    [[nodiscard]] std::vector<lattice_site> moore_neighborhood(const lattice_site& c) const
    {
        std::vector<lattice_site> neighbors{};
        neighbors.reserve(8);

        const auto x   = c.x;
        const auto row = row_of(c);

        const auto decr_x   = (x - 1 >= nw_x) ? x - 1 : x;
        const auto incr_x   = (x + 1 <= se_x) ? x + 1 : x;
        const auto decr_row = (row - 1 >= nw_row) ? row - 1 : row;
        const auto incr_row = (row + 1 <= se_row) ? row + 1 : row;

        const auto add = [this, &neighbors](const int32_t nx, const int64_t nrow)
        {
            if (const auto s = site_at_row(nx, nrow); layout_to_analyze.is_empty_cell(s))
            {
                neighbors.push_back(s);
            }
        };

        if (x != incr_x)
        {
            add(incr_x, row);
        }
        if (x != incr_x && row != decr_row)
        {
            add(incr_x, decr_row);
        }
        if (row != decr_row)
        {
            add(x, decr_row);
        }
        if (x != decr_x && row != decr_row)
        {
            add(decr_x, decr_row);
        }
        if (x != decr_x)
        {
            add(decr_x, row);
        }
        if (x != decr_x && row != incr_row)
        {
            add(decr_x, incr_row);
        }
        if (row != incr_row)
        {
            add(x, incr_row);
        }
        if (x != incr_x && row != incr_row)
        {
            add(incr_x, incr_row);
        }

        return neighbors;
    }
};

}  // namespace detail

/**
 * Determines the influence of a defect on the operational status of an SiDB gate by placing the defect at every
 * position of a grid over the scanning area and checking whether the gate still implements its Boolean function
 * (or, with `GROUND_STATE_CHANGE`, whether the ground state of any input pattern changes).
 *
 * @tparam TT Truth table type.
 * @param lyt The gate layout.
 * @param spec The Boolean function(s) it implements.
 * @param params Parameters.
 * @param step_size Only positions whose column and row are multiples of this are evaluated.
 * @param stats Statistics.
 * @return The defect influence domain.
 * @throws std::invalid_argument if `step_size` is zero.
 * @throws std::invalid_argument if `spec` is empty.
 */
template <typename TT>
[[nodiscard]] defect_influence_domain
defect_influence_grid_search(const layout& lyt, const std::vector<TT>& spec, const defect_influence_params& params = {},
                             const std::size_t step_size = 1, defect_influence_stats* stats = nullptr)
{
    static_assert(kitty::is_truth_table<TT>::value, "TT is not a truth table");

    if (spec.empty())
    {
        throw std::invalid_argument{"spec must not be empty"};
    }

    defect_influence_stats        st{};
    detail::defect_influence_impl p{lyt, params, st};

    const auto result = p.grid_search(step_size, std::optional{spec});

    if (stats != nullptr)
    {
        *stats = st;
    }

    return result;
}
/**
 * Determines the influence of a defect on the ground state of an SiDB layout by placing the defect at every position
 * of a grid over the scanning area.
 *
 * @param lyt The layout.
 * @param params Parameters; the influence definition has to be `GROUND_STATE_CHANGE`.
 * @param step_size Only positions whose column and row are multiples of this are evaluated.
 * @param stats Statistics.
 * @return The defect influence domain.
 * @throws std::invalid_argument if `step_size` is zero.
 */
[[nodiscard]] inline defect_influence_domain defect_influence_grid_search(const layout&                  lyt,
                                                                          const defect_influence_params& params    = {},
                                                                          const std::size_t              step_size = 1,
                                                                          defect_influence_stats* stats = nullptr)
{
    defect_influence_stats        st{};
    detail::defect_influence_impl p{lyt, params, st};

    const auto result = p.grid_search(step_size);

    if (stats != nullptr)
    {
        *stats = st;
    }

    return result;
}
/**
 * Like `defect_influence_grid_search`, but evaluates randomly chosen positions of the scanning area.
 *
 * @tparam TT Truth table type.
 * @param lyt The gate layout.
 * @param spec The Boolean function(s) it implements.
 * @param samples Number of positions to evaluate.
 * @param params Parameters.
 * @param stats Statistics.
 * @return The defect influence domain.
 * @throws std::invalid_argument if `spec` is empty.
 */
template <typename TT>
[[nodiscard]] defect_influence_domain
defect_influence_random_sampling(const layout& lyt, const std::vector<TT>& spec, const std::size_t samples,
                                 const defect_influence_params& params = {}, defect_influence_stats* stats = nullptr)
{
    static_assert(kitty::is_truth_table<TT>::value, "TT is not a truth table");

    if (spec.empty())
    {
        throw std::invalid_argument{"spec must not be empty"};
    }

    defect_influence_stats        st{};
    detail::defect_influence_impl p{lyt, params, st};

    const auto result = p.random_sampling(samples, std::optional{spec});

    if (stats != nullptr)
    {
        *stats = st;
    }

    return result;
}
/**
 * Like `defect_influence_grid_search` without a specification, but evaluates randomly chosen positions.
 *
 * @param lyt The layout.
 * @param samples Number of positions to evaluate.
 * @param params Parameters; the influence definition has to be `GROUND_STATE_CHANGE`.
 * @param stats Statistics.
 * @return The defect influence domain.
 */
[[nodiscard]] inline defect_influence_domain
defect_influence_random_sampling(const layout& lyt, const std::size_t samples,
                                 const defect_influence_params& params = {}, defect_influence_stats* stats = nullptr)
{
    defect_influence_stats        st{};
    detail::defect_influence_impl p{lyt, params, st};

    const auto result = p.random_sampling(samples);

    if (stats != nullptr)
    {
        *stats = st;
    }

    return result;
}
/**
 * *QuickTrace*: traces the contour of the region in which a defect influences an SiDB gate, which needs far fewer
 * evaluations than a grid search.
 *
 * @tparam TT Truth table type.
 * @param lyt The gate layout.
 * @param spec The Boolean function(s) it implements.
 * @param samples Number of starting rows to try.
 * @param params Parameters.
 * @param stats Statistics.
 * @return The defect influence domain.
 * @throws std::invalid_argument if `spec` is empty.
 */
template <typename TT>
[[nodiscard]] defect_influence_domain
defect_influence_quicktrace(const layout& lyt, const std::vector<TT>& spec, const std::size_t samples,
                            const defect_influence_params& params = {}, defect_influence_stats* stats = nullptr)
{
    static_assert(kitty::is_truth_table<TT>::value, "TT is not a truth table");

    if (spec.empty())
    {
        throw std::invalid_argument{"spec must not be empty"};
    }

    defect_influence_stats        st{};
    detail::defect_influence_impl p{lyt, params, st};

    const auto result = p.quicktrace(samples, std::optional{spec});

    if (stats != nullptr)
    {
        *stats = st;
    }

    return result;
}
/**
 * *QuickTrace* without a specification: traces the contour of the region in which a defect changes the ground state
 * of an SiDB layout.
 *
 * @param lyt The layout.
 * @param samples Number of starting rows to try.
 * @param params Parameters; the influence definition has to be `GROUND_STATE_CHANGE`.
 * @param stats Statistics.
 * @return The defect influence domain.
 */
[[nodiscard]] inline defect_influence_domain defect_influence_quicktrace(const layout& lyt, const std::size_t samples,
                                                                         const defect_influence_params& params = {},
                                                                         defect_influence_stats*        stats = nullptr)
{
    defect_influence_stats        st{};
    detail::defect_influence_impl p{lyt, params, st};

    const auto result = p.quicktrace(samples);

    if (stats != nullptr)
    {
        *stats = st;
    }

    return result;
}

}  // namespace fiction::sidb::simulation::defects
