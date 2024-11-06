//
// Created by Jan Drewniok on 28.12.23.
//

#ifndef FICTION_DEFECT_OPERATIONAL_DOMAIN_HPP
#define FICTION_DEFECT_OPERATIONAL_DOMAIN_HPP

#include "fiction/algorithms/simulation/sidb/is_operational.hpp"
#include "fiction/algorithms/simulation/sidb/maximum_defect_influence_position_and_distance.hpp"
#include "fiction/layouts/bounding_box.hpp"
#include "fiction/layouts/coordinates.hpp"
#include "fiction/technology/sidb_defect_surface.hpp"
#include "fiction/technology/sidb_defects.hpp"
#include "fiction/traits.hpp"
#include "fiction/utils/layout_utils.hpp"
#include "fiction/utils/phmap_utils.hpp"

#include <kitty/traits.hpp>
#include <mockturtle/utils/stopwatch.hpp>

#include <algorithm>
#include <atomic>
#include <cassert>
#include <cstdlib>
#include <future>
#include <optional>
#include <random>
#include <thread>
#include <unordered_set>
#include <vector>

namespace fiction
{
/**
 * Parameters to determine the defect operational domain.
 */
struct defect_operational_domain_params
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
 * a given layout. Any algorithm for computing the defect operational domain iterates through all \f$ 2^n \f$
 * input combinations, evaluating the layout's output behavior based on the specified Boolean function. The layout is
 * classified as operational for a particular defect position only if its output behavior is correct across all
 * input combinations.
 */
template <typename Lyt>
struct defect_operational_domain
{
    locked_parallel_flat_hash_map<typename Lyt::cell, operational_status> operational_values{};
};
/**
 * Statistics.
 */
struct defect_operational_domain_stats
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
class defect_operational_domain_impl
{
  public:
    defect_operational_domain_impl(const Lyt& lyt, const std::vector<TT>& tt,
                                   const defect_operational_domain_params& ps, defect_operational_domain_stats& st) :
            layout{lyt},
            truth_table{tt},
            params{ps},
            stats{st}
    {
        determine_nw_se_cells();
    }

    /**
     * This function divides the search space (defined by `nw_cell` and `se_cell`) into chunks, each processed by a
     * different thread. The search checks if defect positions in the grid are operational based on a given step size.
     * Each thread processes a chunk of positions in parallel to improve performance.
     *
     * @param step_size The step size used to sample defect positions in the grid. Only positions with x and y
     * coordinates divisible by `step_size` will be checked for being operational.
     * @return A `defect_operational_domain<Lyt>` object representing the operational domain of the defects.
     */
    [[nodiscard]] defect_operational_domain<Lyt> grid_search(const std::size_t& step_size) noexcept
    {
        mockturtle::stopwatch stop{stats.time_total};
        const auto            all_possible_defect_positions = all_coordinates_in_spanned_area(nw_cell, se_cell);
        const std::size_t     num_positions                 = all_possible_defect_positions.size();

        const auto num_threads = std::thread::hardware_concurrency();  // Get the number of hardware threads

        // Determine the chunk size (each thread will process a chunk of positions)
        const std::size_t chunk_size = (num_positions + num_threads - 1) / num_threads;  // Distribute positions evenly

        // Define a lambda function that processes a chunk of defect positions
        auto process_chunk = [&](const std::size_t& start, const std::size_t& end)
        {
            for (std::size_t i = start; i < end; i += step_size)
            {
                if (static_cast<std::size_t>(std::abs(all_possible_defect_positions[i].x)) % step_size == 0 &&
                    static_cast<std::size_t>(std::abs(all_possible_defect_positions[i].y)) % step_size == 0)
                {
                    is_defect_position_operational(all_possible_defect_positions[i]);
                }
            }
        };

        std::vector<std::thread> threads{};
        threads.reserve(num_threads);

        for (std::size_t start = 0; start < num_positions; start += chunk_size)
        {
            const std::size_t end = std::min(start + chunk_size, num_positions);
            threads.emplace_back(process_chunk, start, end);
        }

        // Wait for all threads to complete
        for (auto& thread : threads)
        {
            if (thread.joinable())
            {
                thread.join();
            }
        }

        log_stats();

        return defect_op_domain;
    }

    /**
     * This function performs random sampling of defect positions from a grid area (spanned by `nw_cell` and `se_cell`).
     * The positions are shuffled and divided into chunks, which are processed by different threads to check if each
     * defect position is operational. Each thread handles a subset of the defect positions to improve performance.
     *
     * @param samples The number of positions to sample. The actual number of iterations will be the smaller of
     *                the total number of positions or the `samples` value.
     * @return A `defect_operational_domain<Lyt>` object representing the operational domain of the defects.
     *         The return value is marked [[nodiscard]], meaning it must be used by the caller.
     */
    [[nodiscard]] defect_operational_domain<Lyt> random_sampling(const std::size_t samples) noexcept
    {
        mockturtle::stopwatch stop{stats.time_total};  // Start the stopwatch for performance measurement

        // Get all possible defect positions within the grid spanned by nw_cell and se_cell
        auto all_possible_defect_positions = all_coordinates_in_spanned_area(nw_cell, se_cell);

        // Initialize random number generator
        std::random_device rd;
        std::mt19937       gen(rd());

        // Shuffle the vector using std::shuffle
        std::shuffle(all_possible_defect_positions.begin(), all_possible_defect_positions.end(), gen);

        // Determine how many positions to sample (use the smaller of samples or the total number of positions)
        const auto min_iterations = std::min(all_possible_defect_positions.size(), samples);

        // Get the number of hardware threads available
        const auto num_threads = std::thread::hardware_concurrency();

        // Calculate the chunk size for each thread to process
        std::size_t chunk_size = (min_iterations + num_threads - 1) / num_threads;

        // Define the lambda function that processes a chunk of sampled defect positions
        auto process_chunk = [&](std::size_t start, std::size_t end)
        {
            for (std::size_t i = start; i < end; ++i)
            {
                is_defect_position_operational(all_possible_defect_positions[i]);
            }
        };

        // Create a vector to hold futures for the threads
        std::vector<std::future<void>> futures{};

        // Launch threads to process chunks of defect positions in parallel
        for (std::size_t start = 0; start < min_iterations; start += chunk_size)
        {
            std::size_t end = std::min(start + chunk_size, min_iterations);
            futures.emplace_back(std::async(std::launch::async, process_chunk, start, end));
        }

        // Wait for all threads to complete their execution
        for (auto& future : futures)
        {
            future.get();  // Block until the thread completes
        }

        log_stats();  // Log the statistics after processing

        // Return the computed operational domain
        return defect_op_domain;
    }

    [[nodiscard]] defect_operational_domain<Lyt> quicktrace(const std::size_t samples) noexcept
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

        std::unordered_set<cell<Lyt>> starting_points{};

        std::size_t sample_counter = 0;

        while (sample_counter < samples)
        {
            sample_counter++;

            // first, perform random sampling to find an operational starting point
            const auto operational_starting_point = find_operational_defect_position_at_left_side();

            // if no operational point was found within the specified number of samples, return
            if (!operational_starting_point.has_value())
            {
                return defect_op_domain;
            }

            // check if the starting point has already been sampled
            if (starting_points.find(*operational_starting_point) != starting_points.cend())
            {
                continue;
            }

            // add operatioal starting point to the set of starting points
            starting_points.insert(*operational_starting_point);

            // the layout hs to be defect-free.
            assert(layout.num_defects() == 0 && "An atomic defect is added");

            // find an operational point on the contour starting from the randomly determined starting point
            const auto contour_starting_p =
                find_last_operational_defect_position_moving_right(*operational_starting_point);

            if (!contour_starting_p.has_value())
            {
                continue;
            }

            const auto contour_starting_point = *contour_starting_p;

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

            while (next_point != contour_starting_point)
            {
                const auto operational_status = is_defect_position_operational(next_point);

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
            }
        }
        log_stats();

        return defect_op_domain;
    }

  private:
    /**
     * This function determines the northwest and southeast cells based on the gate layout and the additional scan area
     * specified.
     */
    void determine_nw_se_cells() noexcept
    {
        // bounding box around the given layout to have north-west and south-east cells.
        const auto                              siqad_layout = convert_layout_to_siqad_coordinates(layout);
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
        std::uniform_int_distribution<decltype(nw_cell.y)> dist(nw_cell.y, se_cell.y);
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
            defect_op_domain.operational_values[c] = operational_status::OPERATIONAL;

            return operational_status::OPERATIONAL;
        };

        const auto non_operational = [this, &c]()
        {
            defect_op_domain.operational_values[c] = operational_status::NON_OPERATIONAL;

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
    [[nodiscard]] std::optional<operational_status> has_already_been_sampled(const typename Lyt::cell& c) const noexcept
    {
        if (const auto it = defect_op_domain.operational_values.find(c);
            it != defect_op_domain.operational_values.cend())
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
     * @return The last operational defect position. If no non-operational defect is found, std::nullopt is returned.
     */
    [[nodiscard]] std::optional<typename Lyt::cell>
    find_last_operational_defect_position_moving_right(const typename Lyt::cell& starting_defect_position) noexcept
    {
        auto latest_operational_defect_position = starting_defect_position;
        // layout.assign_sidb_defect(starting_defect_position, params.defect_influence_params.defect);
        previous_defect_position = starting_defect_position;

        // move towards the left border of the parameter range
        for (auto x = starting_defect_position.x; x <= se_cell.x; x++)
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

        if (current_defect_position == latest_operational_defect_position)
        {
            return std::nullopt;
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

        for (const auto& [param_point, status] : defect_op_domain.operational_values)
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
    const defect_operational_domain_params& params;
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
    defect_operational_domain<Lyt> defect_op_domain{};
    /**
     * The statistics of the operational domain computation.
     */
    defect_operational_domain_stats& stats;
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
 * Computes the defect operational domain of the given SiDB layout. The defect operational domain is the set of all
 * defect positions for which the layout is logically operational. Logical operation is defined as the layout
 * implementing the given truth table.
 *
 * This algorithm uses a grid search to determine the defect operational domain. The grid search is performed
 * by exhaustively sweeping all possible atomic defect positions in x and y dimensions.
 *
 * @tparam Lyt SiDB cell-level layout type.
 * @tparam TT Truth table type.
 * @param lyt Layout to compute the defect operational domain for.
 * @param spec Expected Boolean function of the layout given as a multi-output truth table.
 * @param step_size The parameter specifying the interval between consecutive defect positions to be evaluated.
 * @param params Defect operational domain computation parameters.
 * @param stats Statistics.
 * @return The defect operational domain of the layout.
 */
template <typename Lyt, typename TT>
defect_operational_domain<Lyt>
defect_operational_domain_grid_search(const Lyt& lyt, const std::vector<TT>& spec, const std::size_t step_size = 1,
                                      const defect_operational_domain_params& params = {},
                                      defect_operational_domain_stats*        stats  = nullptr)
{
    static_assert(is_cell_level_layout_v<Lyt>, "Lyt is not a cell-level layout");
    static_assert(has_sidb_technology_v<Lyt>, "Lyt is not an SiDB layout");
    static_assert(kitty::is_truth_table<TT>::value, "TT is not a truth table");
    static_assert(has_cube_coord_v<Lyt>);

    defect_operational_domain_stats                 st{};
    detail::defect_operational_domain_impl<Lyt, TT> p{lyt, spec, params, st};

    const auto result = p.grid_search(step_size);

    if (stats)
    {
        *stats = st;
    }

    return result;
}
/**
 * Computes the `defect operational domain` of the given SiDB cell-level layout. The defect operational domain is the
 * set of all defect positions for which the layout is logically operational. Logical operation is defined as the layout
 * implementing the given truth table. The input BDL pairs of the layout are assumed to be in the same order as the
 * inputs of the truth table.
 *
 * This algorithm uses random sampling to find a part of the defect operational domain that might not be
 * complete. It performs a total of `samples` uniformly-distributed random samples within the specified area.
 *
 * @tparam Lyt SiDB cell-level layout type.
 * @tparam TT Truth table type.
 * @param lyt Layout to compute the defect operational domain for.
 * @param spec Expected Boolean function of the layout given as a multi-output truth table.
 * @param samples Number of random samples to perform.
 * @param params Defect operational domain computation parameters.
 * @param stats Statistics.
 * @return The (partial) defect operational domain of the layout.
 */
template <typename Lyt, typename TT>
defect_operational_domain<Lyt>
defect_operational_domain_random_sampling(const Lyt& lyt, const std::vector<TT>& spec, std::size_t samples,
                                          const defect_operational_domain_params& params = {},
                                          defect_operational_domain_stats*        stats  = nullptr)
{
    static_assert(is_cell_level_layout_v<Lyt>, "Lyt is not a cell-level layout");
    static_assert(has_sidb_technology_v<Lyt>, "Lyt is not an SiDB layout");
    static_assert(kitty::is_truth_table<TT>::value, "TT is not a truth table");
    static_assert(has_cube_coord_v<Lyt>, "Lyt is not based on cube coordinates");

    defect_operational_domain_stats                 st{};
    detail::defect_operational_domain_impl<Lyt, TT> p{lyt, spec, params, st};

    const auto result = p.random_sampling(samples);

    if (stats)
    {
        *stats = st;
    }

    return result;
}

/**
 * Computes the defect operational domain of the given SiDB cell-level layout. The defect
 * operational domain is the set of all defect positions for which the layout is logically operational. Logical
 * operation is defined as the layout implementing the given truth table. The input BDL pairs of the layout are assumed
 * to be in the same order as the inputs of the truth table.
 *
 * This algorithm uses contour tracing to identify operational defect locations within the SiDB gate layout.
 * It starts by searching for defect locations on the left side (bounding_box + additional scanning area). The
 * y-coordinate for these positions is chosen randomly. The number of samples is determined by the `samples` parameter.
 *
 * Then the algorithm moves each defect position to the right, searching for the last operational defect position. This
 * position is selected as the starting point for the contour trace. The contour tracing process checks whether the
 * contour includes the SiDB layout. If it does not, the next random sample point is selected as the starting point
 * and the process is repeated.
 *
 * @tparam Lyt SiDB cell-level layout type.
 * @tparam TT Truth table type.
 * @param lyt Layout to compute the defect operational domain for.
 * @param spec Expected Boolean function of the layout given as a multi-output truth table.
 * @param samples Number of samples to perform.
 * @param params Defect operational domain computation parameters.
 * @param stats Operational domain computation statistics.
 * @return The (partial) defect operational domain of the layout.
 */
template <typename Lyt, typename TT>
defect_operational_domain<Lyt> defect_operational_domain_quicktrace(const Lyt& lyt, const std::vector<TT>& spec,
                                                                    const std::size_t                       samples,
                                                                    const defect_operational_domain_params& params = {},
                                                                    defect_operational_domain_stats* stats = nullptr)
{
    static_assert(is_cell_level_layout_v<Lyt>, "Lyt is not a cell-level layout");
    static_assert(has_sidb_technology_v<Lyt>, "Lyt is not an SiDB layout");
    static_assert(kitty::is_truth_table<TT>::value, "TT is not a truth table");
    static_assert(has_cube_coord_v<Lyt>, "Lyt is not based on cube coordinates");

    defect_operational_domain_stats                 st{};
    detail::defect_operational_domain_impl<Lyt, TT> p{lyt, spec, params, st};

    const auto result = p.quicktrace(samples);

    if (stats)
    {
        *stats = st;
    }

    return result;
}

}  // namespace fiction

#endif  // FICTION_DEFECT_OPERATIONAL_DOMAIN_HPP
