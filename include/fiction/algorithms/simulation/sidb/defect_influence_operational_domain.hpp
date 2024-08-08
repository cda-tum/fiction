//
// Created by Jan Drewniok on 28.12.23.
//

#ifndef FICTION_DEFECT_INFLUENCE_OPERATIONAL_DOMAIN_HPP
#define FICTION_DEFECT_INFLUENCE_OPERATIONAL_DOMAIN_HPP

#include "fiction/algorithms/simulation/sidb/is_operational.hpp"
#include "fiction/algorithms/simulation/sidb/maximum_defect_influence_position_and_distance.hpp"
#include "fiction/layouts/bounding_box.hpp"
#include "fiction/layouts/coordinates.hpp"
#include "fiction/technology/sidb_defect_surface.hpp"
#include "fiction/technology/sidb_defects.hpp"
#include "fiction/utils/layout_utils.hpp"
#include "fiction/utils/phmap_utils.hpp"

#include <kitty/traits.hpp>
#include <mockturtle/utils/stopwatch.hpp>

#include <atomic>
#include <cassert>
#include <cstdint>
#include <cstdlib>
#include <optional>
#include <random>
#include <vector>

namespace fiction
{
/**
 * Parameters to determine the defect influence operational domain.
 */
struct defect_influence_operational_domain_params
{
    /**
     * Parameters to simulate the influence of the atomic defect.
     */
    maximum_defect_influence_position_and_distance_params defect_influence_params{};
    /**
     * Parameters for the `is_operational` algorithm.
     */
    is_operational_params operational_params{};
};
/**
 * A defect operational domain defines the positions where a specific atomic defect may exist along with corresponding
 * information indicating whether the SiDB layout remains logically operational. Logically operational implies that the
 * layout's ground state aligns with a designated Boolean function at the layout's outputs for all feasible input
 * combinations. This implementation assumes the presence of \f$ n \f$ BDL input wires and a single BDL output wire for
 * a given layout. Any algorithm for computing the defect influence operational domain iterates through all \f$ 2^n \f$
 * input combinations, evaluating the layout's output behavior based on the specified Boolean function. The layout is
 * classified as operational for a particular defect position only if its output behavior is correct across all
 * input combinations.
 */
template <typename Lyt>
struct defect_influence_operational_domain
{
    locked_parallel_flat_hash_map<typename Lyt::cell, operational_status> operational_values{};
};
/**
 * Statistics.
 */
struct defect_influence_operational_domain_stats
{
    /**
     * The total runtime of the operational domain computation.
     */
    mockturtle::stopwatch<>::duration time_total{0};
    /**
     * Number of simulator invocations.
     */
    std::size_t num_simulator_invocations{0};
    /**
     * Number of evaluated parameter combinations.
     */
    std::size_t num_evaluated_defect_positions{0};
    /**
     * Number of parameter combinations, for which the layout is operational.
     */
    std::size_t num_operational_defect_positions{0};
    /**
     * Number of parameter combinations, for which the layout is non-operational.
     */
    std::size_t num_non_operational_defect_positions{0};
};

namespace detail
{

template <typename Lyt, typename TT>
class defect_influence_operational_domain_impl
{
  public:
    defect_influence_operational_domain_impl(const Lyt& lyt, const std::vector<TT>& tt,
                                             const defect_influence_operational_domain_params& ps,
                                             defect_influence_operational_domain_stats&        st) :
            layout{lyt},
            truth_table{tt},
            params{ps},
            stats{st}
    {
        determine_nw_se_cells();
    }

    [[nodiscard]] defect_influence_operational_domain<Lyt> grid_search(const std::size_t& step_size) noexcept
    {
        mockturtle::stopwatch stop{stats.time_total};
        const auto            all_possible_defect_positions = all_coordinates_in_spanned_area(nw_cell, se_cell);

        // Iterate through all possible defect positions, but make sure that the position is distributed on a grid
        // defined by `step_size'.
        for (std::size_t i = 0; i < all_possible_defect_positions.size(); i += step_size)
        {
            if (static_cast<std::size_t>(std::abs(all_possible_defect_positions[i].x)) % step_size == 0 &&
                static_cast<std::size_t>(std::abs(all_possible_defect_positions[i].y)) % step_size == 0)
            {
                is_defect_position_operational(all_possible_defect_positions[i]);
            }
        }

        log_stats();

        return defect_operational_domain;
    }

    [[nodiscard]] defect_influence_operational_domain<Lyt> random_sampling(const std::size_t samples) noexcept
    {
        mockturtle::stopwatch stop{stats.time_total};
        auto                  all_possible_defect_positions = all_coordinates_in_spanned_area(nw_cell, se_cell);

        std::random_device rd;
        std::mt19937       gen(rd());

        // Shuffle the vector using std::shuffle
        std::shuffle(all_possible_defect_positions.begin(), all_possible_defect_positions.end(), gen);

        const auto min_iterations = std::min(all_possible_defect_positions.size(), samples);

        for (auto i = 0u; i < min_iterations; ++i)
        {
            is_defect_position_operational(all_possible_defect_positions[i]);
        }

        log_stats();

        return defect_operational_domain;
    }

    [[nodiscard]] defect_influence_operational_domain<Lyt> contour_tracing(const std::size_t samples) noexcept
    {
        mockturtle::stopwatch stop{stats.time_total};

        const auto next_clockwise_point = [](std::vector<typename Lyt::cell>& neighborhood,
                                             const typename Lyt::cell&        backtrack) noexcept -> typename Lyt::cell
        {
            assert(std::find(neighborhood.cbegin(), neighborhood.cend(), backtrack) != neighborhood.cend() &&
                   "The backtrack point must be part of the neighborhood");

            while (neighborhood.back() != backtrack)
            {
                std::rotate(neighborhood.begin(), neighborhood.begin() + 1, neighborhood.end());
            }

            return neighborhood.front();
        };

        std::size_t number_of_random_start_positions = 0;
        while (number_of_random_start_positions < samples)
        {
            // first, perform random sampling to find an operational starting point
            const auto operational_starting_point = find_operational_defect_position_at_left_side();

            // if no operational point was found within the specified number of samples, return
            if (!operational_starting_point.has_value())
            {
                return defect_operational_domain;
            }

            // the layout hs to be defect-free.
            assert(layout.num_defects() == 0 && "An atomic defect is added");

            number_of_random_start_positions++;

            // find an operational point on the contour starting from the randomly determined starting point
            const auto contour_starting_point =
                find_last_operational_defect_position_moving_right(*operational_starting_point);

            // the layout hs to be defect-free.
            assert(layout.num_defects() == 0 && "An atomic defect is added");

            auto current_contour_point = contour_starting_point;
            auto backtrack_point       = current_contour_point.x == nw_cell.x ?
                                             current_contour_point :
                                             typename Lyt::cell{current_contour_point.x - 1, current_contour_point.y};

            auto current_neighborhood = moore_neighborhood(current_contour_point);
            auto next_point           = current_contour_point == backtrack_point ?
                                            current_neighborhood.front() :
                                            next_clockwise_point(current_neighborhood, backtrack_point);

            bool contour_goes_around_layout = false;

            uint64_t counter = 0;
            while (next_point != contour_starting_point && counter < 100000)
            {
                const auto operational_status = is_defect_position_operational(next_point);
                // check if the contour goes around the layout.
                if (next_point.x >= se_bb_layout.x && next_point.y >= se_bb_layout.y)
                {
                    contour_goes_around_layout = true;
                }

                assert(layout.num_defects() == 0 && "more than one defect");

                if (operational_status == operational_status::NON_OPERATIONAL || !layout.is_empty_cell(next_point))
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
                counter++;
            }
            number_of_random_start_positions++;
            if (!contour_goes_around_layout)
            {
                continue;
            }
            log_stats();
            return defect_operational_domain;
        }
        log_stats();

        return defect_operational_domain;
    }

  private:
    /**
     * This function determines the northwest and southeast cells based on the gate layout and the additional scan area
     * specified.
     */
    void determine_nw_se_cells() noexcept
    {
        // bounding box around the given layout to have north-west and south-east cells.
        const auto                              siqad_layout = convert_to_siqad_coordinates(layout);
        bounding_box_2d<decltype(siqad_layout)> bb{siqad_layout};

        auto nw = fiction::siqad::to_fiction_coord<cube::coord_t>(bb.get_min());  // north-west cell
        auto se = fiction::siqad::to_fiction_coord<cube::coord_t>(bb.get_max());  // south-east cell

        nw_bb_layout = nw;
        se_bb_layout = se;

        // shift nw and se cell by the additional scanning area to cover an area that is larger than the gate area.
        nw.x = nw.x - params.defect_influence_params.additional_scanning_area.first;
        nw.y = nw.y - params.defect_influence_params.additional_scanning_area.second;

        se.x = se.x + params.defect_influence_params.additional_scanning_area.first;
        se.y = se.y + params.defect_influence_params.additional_scanning_area.second;

        nw_cell = nw;
        se_cell = se;
    }
    /**
     * This function aims to identify an operational defect position within the layout. It does so by selecting a defect
     * position with the leftmost x-coordinate and a randomly selected y-coordinate limited the layout's bounding box.
     */
    [[nodiscard]] std::optional<typename Lyt::cell> find_operational_defect_position_at_left_side() noexcept
    {
        auto               starting_point = nw_cell;
        std::random_device rd;
        std::mt19937       gen(rd());
        // Create a distribution for generating random numbers within the specified range
        std::uniform_int_distribution<decltype(nw_bb_layout.y)> dist(nw_bb_layout.y, se_bb_layout.y);
        starting_point.y = dist(gen);
        layout.assign_sidb_defect(starting_point, params.defect_influence_params.defect);
        // determine the operational status
        const auto operational_value = is_defect_position_operational(starting_point);
        layout.assign_sidb_defect(starting_point, sidb_defect{sidb_defect_type::NONE});

        // if the parameter combination is operational, return its step values in x and y dimension
        if (operational_value == operational_status::OPERATIONAL)
        {
            return starting_point;
        }

        return std::nullopt;
    }

    /**
     * This function evaluates the operational status of the SiDB gate when a defect is placed at position `c`.
     *
     * @param c Position of the defect.
     */
    operational_status is_defect_position_operational(const typename Lyt::cell& c) noexcept
    {
        auto lyt_copy = layout.clone();
        // if the point has already been sampled, return the stored operational status
        if (const auto op_value = has_already_been_sampled(c); op_value.has_value())
        {
            return *op_value;
        }

        const auto operational = [this, &c]()
        {
            defect_operational_domain.operational_values[c] = operational_status::OPERATIONAL;

            return operational_status::OPERATIONAL;
        };

        const auto non_operational = [this, &c]()
        {
            defect_operational_domain.operational_values[c] = operational_status::NON_OPERATIONAL;

            return operational_status::NON_OPERATIONAL;
        };

        // increment the number of evaluated parameter combinations
        ++num_evaluated_defect_positions;

        if (!lyt_copy.is_empty_cell(c))
        {
            return non_operational();
        }

        lyt_copy.assign_sidb_defect(c, params.defect_influence_params.defect);
        const auto& [status, sim_calls] = is_operational(lyt_copy, truth_table, params.operational_params);
        lyt_copy.assign_sidb_defect(c, sidb_defect{sidb_defect_type::NONE});
        num_simulator_invocations += sim_calls;

        if (status == operational_status::NON_OPERATIONAL)
        {
            return non_operational();
        }

        // if we made it here, the layout is operational
        return operational();
    }

    /**
     * This function verifies whether the layout has already been analyzed for the specified defect position `c`.
     *
     * @param c Position of the defect.
     */
    [[nodiscard]] inline std::optional<operational_status>
    has_already_been_sampled(const typename Lyt::cell& c) const noexcept
    {
        if (const auto it = defect_operational_domain.operational_values.find(c);
            it != defect_operational_domain.operational_values.cend())
        {
            return it->second;
        }

        return std::nullopt;
    }

    /**
     * This function identifies the most recent operational defect position while traversing from left to right towards
     * the SiDB gate.
     *
     * @param starting_defect_position The starting position of the defect, from which the traversal towards the right
     * is conducted while maintaining gate operability.
     * @return The last operational defect position.
     */
    [[nodiscard]] typename Lyt::cell
    find_last_operational_defect_position_moving_right(const typename Lyt::cell& starting_defect_position) noexcept
    {
        auto latest_operational_defect_position = starting_defect_position;
        // layout.assign_sidb_defect(starting_defect_position, params.defect_influence_params.defect);
        previous_defect_position = starting_defect_position;

        // move towards the left border of the parameter range
        for (auto x = starting_defect_position.x; x <= se_bb_layout.x; x++)
        {
            previous_defect_position = current_defect_position;
            current_defect_position  = {x, starting_defect_position.y};

            layout.assign_sidb_defect(current_defect_position, params.defect_influence_params.defect);

            const auto operational_status = is_defect_position_operational(current_defect_position);

            layout.assign_sidb_defect(current_defect_position, sidb_defect{sidb_defect_type::NONE});

            if (operational_status == operational_status::OPERATIONAL)
            {
                latest_operational_defect_position = current_defect_position;
            }
            else
            {
                return previous_defect_position;
            }
        }

        return latest_operational_defect_position;
    }
    /**
     * Helper function that writes the the statistics of the defect operational domain computation to the statistics
     * object.
     */
    void log_stats() const noexcept
    {
        stats.num_simulator_invocations      = num_simulator_invocations;
        stats.num_evaluated_defect_positions = num_evaluated_defect_positions;

        for (const auto& [param_point, status] : defect_operational_domain.operational_values)
        {
            if (status == operational_status::OPERATIONAL)
            {
                ++stats.num_operational_defect_positions;
            }
            else
            {
                ++stats.num_non_operational_defect_positions;
            }
        }
    }
    /**
     * Computes the Moore neighborhood of a given cell within the SiDB layout.
     * The Moore neighborhood consists of the eight cells surrounding the central cell
     * in horizontal, vertical, and diagonal directions.
     *
     * @param c The cell for which the Moore neighborhood is computed.
     * @return A vector containing the cells in the Moore neighborhood that are empty.
     *         If a cell is outside the layout boundaries or occupied, it is not included in the result.
     */
    [[nodiscard]] std::vector<typename Lyt::cell> moore_neighborhood(const typename Lyt::cell& c) const noexcept
    {
        std::vector<typename Lyt::cell> neighbors{};
        neighbors.reserve(8);

        const auto& moore = c;

        const auto decr_x = (moore.x - 1 >= nw_cell.x) ? moore.x - 1 : moore.x;
        const auto incr_x = (moore.x + 1 <= se_cell.x) ? moore.x + 1 : moore.x;
        const auto decr_y = (moore.y - 1 >= nw_cell.y) ? moore.y - 1 : moore.y;
        const auto incr_y = (moore.y + 1 <= se_cell.y) ? moore.y + 1 : moore.y;

        // add neighbors in clockwise direction

        // right
        if (moore.x != incr_x)
        {
            if (layout.is_empty_cell({incr_x, moore.y}))
            {
                neighbors.emplace_back(incr_x, moore.y);
            }
        }
        // lower-right
        if (moore.x != incr_x && moore.y != decr_y)
        {
            if (layout.is_empty_cell({incr_x, decr_y}))
            {
                neighbors.emplace_back(incr_x, decr_y);
            }
        }
        // down
        if (moore.y != decr_y)
        {
            if (layout.is_empty_cell({moore.x, decr_y}))
            {
                neighbors.emplace_back(moore.x, decr_y);
            }
        }
        // lower-left
        if (moore.x != decr_x && moore.y != decr_y)
        {
            if (layout.is_empty_cell({decr_x, decr_y}))
            {
                neighbors.emplace_back(decr_x, decr_y);
            }
        }
        // left
        if (moore.x != decr_x)
        {
            if (layout.is_empty_cell({decr_x, moore.y}))
            {
                neighbors.emplace_back(decr_x, moore.y);
            }
        }
        // upper-left
        if (moore.x != decr_x && moore.y != incr_y)
        {
            if (layout.is_empty_cell({decr_x, incr_y}))
            {
                neighbors.emplace_back(decr_x, incr_y);
            }
        }
        // up
        if (moore.y != incr_y)
        {
            if (layout.is_empty_cell({moore.x, incr_y}))
            {
                neighbors.emplace_back(moore.x, incr_y);
            }
        }
        // upper-right
        if (moore.x != incr_x && moore.y != incr_y)
        {
            if (layout.is_empty_cell({incr_x, incr_y}))
            {
                neighbors.emplace_back(incr_x, incr_y);
            }
        }

        return neighbors;
    };
    /**
     * The SiDB cell-level layout to investigate.
     */
    sidb_defect_surface<Lyt> layout{};
    /**
     * The specification of the layout.
     */
    const std::vector<TT>& truth_table;
    /**
     * The parameters for the operational domain computation.
     */
    const defect_influence_operational_domain_params& params;
    /**
     * North-west cell.
     */
    typename Lyt::cell nw_cell{};
    /**
     * The north-west bounding box of the layout.
     */
    typename Lyt::cell nw_bb_layout{};
    /**
     * South-east cell.
     */
    typename Lyt::cell se_cell{};
    /**
     * The south-east bounding box of the layout.
     */
    typename Lyt::cell se_bb_layout{};
    /**
     * The current defect position.
     */
    typename Lyt::cell current_defect_position{};
    /**
     * The previous defect position.
     */
    typename Lyt::cell previous_defect_position{};
    /**
     * The operational domain of the layout.
     */
    defect_influence_operational_domain<Lyt> defect_operational_domain{};
    /**
     * The statistics of the operational domain computation.
     */
    defect_influence_operational_domain_stats& stats;
    /**
     * Number of simulator invocations.
     */
    std::atomic<std::size_t> num_simulator_invocations{0};
    /**
     * Number of evaluated defect positions.
     */
    std::atomic<std::size_t> num_evaluated_defect_positions{0};
};

}  // namespace detail

/**
 * Computes the defect influence operational domain of the given SiDB layout. The defect influence
 * operational domain is the set of all defect positions for which the layout is logically operational. Logical
 * operation is defined as the layout implementing the given truth table.
 *
 * This algorithm uses a grid search to determine the defect influence operational domain. The grid search is performed
 * by exhaustively sweeping all possible atomic defect positions in x and y dimensions.
 *
 * @tparam Lyt SiDB cell-level layout type.
 * @tparam TT Truth table type.
 * @param lyt Layout to compute the defect influence operational domain for.
 * @param spec Expected Boolean function of the layout given as a multi-output truth table.
 * @param step_size The parameter specifying the interval between consecutive defect positions to be evaluated.
 * @param params Defect influence operational domain computation parameters.
 * @param stats Statistics.
 * @return The defect influence operational domain of the layout.
 */
template <typename Lyt, typename TT>
defect_influence_operational_domain<Lyt>
defect_influence_operational_domain_grid_search(const Lyt& lyt, const std::vector<TT>& spec,
                                                const std::size_t                                 step_size = 1,
                                                const defect_influence_operational_domain_params& params    = {},
                                                defect_influence_operational_domain_stats*        stats     = nullptr)
{
    static_assert(is_cell_level_layout_v<Lyt>, "Lyt is not a cell-level layout");
    static_assert(has_sidb_technology_v<Lyt>, "Lyt is not an SiDB layout");
    static_assert(kitty::is_truth_table<TT>::value, "TT is not a truth table");
    static_assert(has_cube_coord_v<Lyt> || has_siqad_coord_v<Lyt>, "Lyt is not based on cube coordinates");

    defect_influence_operational_domain_stats                 st{};
    detail::defect_influence_operational_domain_impl<Lyt, TT> p{lyt, spec, params, st};

    const auto result = p.grid_search(step_size);

    if (stats)
    {
        *stats = st;
    }

    return result;
}
/**
 * Computes the `defect influence operational domain` of the given SiDB cell-level layout. The defect influence
 * operational domain is the set of all defect positions for which the layout is logically operational. Logical
 * operation is defined as the layout implementing the given truth table. The input BDL pairs of the layout are assumed
 * to be in the same order as the inputs of the truth table.
 *
 * This algorithm uses random sampling to find a part of the defect influence operational domain that might not be
 * complete. It performs a total of `samples` uniformly-distributed random samples within the specified area.
 *
 * @tparam Lyt SiDB cell-level layout type.
 * @tparam TT Truth table type.
 * @param lyt Layout to compute the defect influence operational domain for.
 * @param spec Expected Boolean function of the layout given as a multi-output truth table.
 * @param samples Number of random samples to perform.
 * @param params Defect influence operational domain computation parameters.
 * @param stats Statistics.
 * @return The (partial) defect influence operational domain of the layout.
 */
template <typename Lyt, typename TT>
defect_influence_operational_domain<Lyt>
defect_influence_operational_domain_random_sampling(const Lyt& lyt, const std::vector<TT>& spec, std::size_t samples,
                                                    const defect_influence_operational_domain_params& params = {},
                                                    defect_influence_operational_domain_stats*        stats  = nullptr)
{
    static_assert(is_cell_level_layout_v<Lyt>, "Lyt is not a cell-level layout");
    static_assert(has_sidb_technology_v<Lyt>, "Lyt is not an SiDB layout");
    static_assert(kitty::is_truth_table<TT>::value, "TT is not a truth table");
    static_assert(has_cube_coord_v<Lyt>, "Lyt is not based on cube coordinates");

    defect_influence_operational_domain_stats                 st{};
    detail::defect_influence_operational_domain_impl<Lyt, TT> p{lyt, spec, params, st};

    const auto result = p.random_sampling(samples);

    if (stats)
    {
        *stats = st;
    }

    return result;
}

/**
 * Computes the defect influence operational domain of the given SiDB cell-level layout. The defect influence
 * operational domain is the set of all defect positions for which the layout is logically operational. Logical
 * operation is defined as the layout implementing the given truth table. The input BDL pairs of the layout are assumed
 * to be in the same order as the inputs of the truth table.
 *
 * This algorithm uses contour tracing to identify operational defect locations within the SiDB gate layout.
 * It starts by searching for defect locations on the left side of the bounding box, with an additional distance
 * of the SiDB gate where the SiDB gate remains operational. The y-coordinate for these positions is chosen
 * randomly. The number of samples is determined by the `samples` parameter.
 *
 * Then the algorithm moves each defect position to the right, searching for the last operational defect position. This
 * position is selected as the starting point for the contour trace. The contour tracing process checks whether the
 * contour includes the SiDB layout. If it does not, the next random sample point is is selected as the starting point
 * and the process is repeated.
 *
 * @Note This algorithm is an approximation to determine the defect influence operational domain. Therefore, it
 * is recommended to analyze the result afterwards to assess whether the contour reflects the desired contour.
 *
 * @tparam Lyt SiDB cell-level layout type.
 * @tparam TT Truth table type.
 * @param lyt Layout to compute the defect influence operational domain for.
 * @param spec Expected Boolean function of the layout given as a multi-output truth table.
 * @param samples Number of samples to perform.
 * @param params Defect influence operational domain computation parameters.
 * @param stats Operational domain computation statistics.
 * @return The (partial) defect influence operational domain of the layout.
 */
template <typename Lyt, typename TT>
defect_influence_operational_domain<Lyt>
defect_influence_operational_domain_contour_tracing(const Lyt& lyt, const std::vector<TT>& spec,
                                                    const std::size_t                                 samples,
                                                    const defect_influence_operational_domain_params& params = {},
                                                    defect_influence_operational_domain_stats*        stats  = nullptr)
{
    static_assert(is_cell_level_layout_v<Lyt>, "Lyt is not a cell-level layout");
    static_assert(has_sidb_technology_v<Lyt>, "Lyt is not an SiDB layout");
    static_assert(kitty::is_truth_table<TT>::value, "TT is not a truth table");
    static_assert(has_cube_coord_v<Lyt>, "Lyt is not based on cube coordinates");

    defect_influence_operational_domain_stats                 st{};
    detail::defect_influence_operational_domain_impl<Lyt, TT> p{lyt, spec, params, st};

    const auto result = p.contour_tracing(samples);

    if (stats)
    {
        *stats = st;
    }

    return result;
}

}  // namespace fiction

#endif  // FICTION_DEFECT_INFLUENCE_OPERATIONAL_DOMAIN_HPP
