//
// Created by Jan Drewniok on 28.12.23.
//

#ifndef FICTION_DEFECT_INFLUENCE_HPP
#define FICTION_DEFECT_INFLUENCE_HPP

#include "fiction/algorithms/iter/bdl_input_iterator.hpp"
#include "fiction/algorithms/simulation/sidb/is_operational.hpp"
#include "fiction/algorithms/simulation/sidb/quickexact.hpp"
#include "fiction/layouts/bounding_box.hpp"
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
#include <cstdint>
#include <cstdlib>
#include <optional>
#include <random>
#include <thread>
#include <unordered_set>
#include <utility>
#include <vector>

namespace fiction
{
/**
 * Parameters to determine the defect influence.
 *
 * @tparam CellType Type of the cell.
 */
template <typename CellType>
struct defect_influence_params
{
    enum class influence_definition : uint8_t
    {
        /**
         * Influence is considered as the ability to change the operational status of the layout.
         */
        OPERATIONALITY_CHANGE,
        /**
         * The influence is considered as the ability to change the ground state of the layout.
         */
        GROUND_STATE_CHANGE
    };
    /**
     * The defect to calculate the maximum defect influence distance for.
     */
    sidb_defect defect{};
    /**
     * Parameters for the `is_operational` algorithm.
     */
    is_operational_params operational_params{};
    /**
     * Area which is considered around the layout for additional defect scanning.
     * This describes the additional space around the bounding box of the layout.
     */
    CellType additional_scanning_area{CellType{50, 6}};
    /**
     * The definition of influence.
     */
    influence_definition influence_def{influence_definition::OPERATIONALITY_CHANGE};
};

/**
 * This defines whether the influence of a defect is present at a particular position
 * in the layout. It can be used to classify positions as having an influence or not.
 */
enum class defect_influence_status : uint8_t
{
    /**
     * This indicates that the defect is actively influencing the layout at this position.
     * It implies that some form of impact, such as a change in operational status or
     * ground state, is being caused by the defect at this position.
     */
    EXISTENT,
    /**
     * This indicates that the defect does not influence the layout at this position.
     * It implies that the layout remains unaffected by the defect at this location,
     * meaning there is no change in operational status or ground state.
     */
    NON_EXISTENT
};
/**
 * A defect influence domain defines for each defect position the influence of the defect on the layout.
 * Depending on the chosen definition of influence, this can either mean the operational status
 * or information on whether the ground state of the layout is changed due to the defect.
 */
template <typename Lyt>
struct defect_influence_domain
{
    /**
     * This stores for each defect position the condition of the layout.
     */
    locked_parallel_flat_hash_map<typename Lyt::cell, defect_influence_status> influence_information{};
};
/**
 * Statistics.
 */
struct defect_influence_stats
{
    /**
     * The total runtime of the defect influence computation.
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
     * Number of parameter combinations, for which the layout gets influenced.
     */
    std::size_t num_influencing_defect_positions{0};
    /**
     * Number of parameter combinations, for which the layout is not influenced.
     */
    std::size_t num_non_influencing_defect_positions{0};
};

namespace detail
{

template <typename Lyt>
class defect_influence_impl
{
  public:
    defect_influence_impl(const Lyt& lyt, const defect_influence_params<cell<Lyt>>& ps, defect_influence_stats& st) :
            layout{lyt},
            params{ps},
            stats{st}
    {
        determine_nw_se_cells();
    }

    /**
     * This function simulates for each position in the area (spanned by `nw_cell` and `se_cell`) if the existence of a
     * defect influences the layout.
     *
     * @param step_size The step size used to sample defect positions in the grid. Only positions with x and y
     * coordinates divisible by `step_size` will be checked.
     * @return The defect influence domain.
     */
    template <typename TT = tt>
    [[nodiscard]] defect_influence_domain<Lyt>
    grid_search(const std::size_t step_size, const std::optional<std::vector<TT>>& spec = std::nullopt) noexcept
    {
        mockturtle::stopwatch stop{stats.time_total};
        const auto            all_possible_defect_positions = all_coordinates_in_spanned_area(nw_cell, se_cell);
        const std::size_t     num_positions                 = all_possible_defect_positions.size();

        const auto number_of_threads = std::min(num_threads, num_positions);

        // calculate the size of each slice
        const auto slice_size = (num_positions + number_of_threads - 1) / number_of_threads;

        std::vector<std::thread> threads{};
        threads.reserve(number_of_threads);

        // launch threads, each with its own slice of random step points
        for (auto t = 0ul; t < number_of_threads; ++t)
        {
            const auto start = t * slice_size;
            const auto end   = std::min(start + slice_size, num_positions);

            if (start >= end)
            {
                break;  // no more work to distribute
            }

            threads.emplace_back(
                [this, start, end, &all_possible_defect_positions, &step_size, &spec]
                {
                    for (auto i = start; i < end; ++i)
                    {
                        // this ensures that the defects are evenly distributed in a grid-like pattern
                        if (static_cast<std::size_t>(std::abs(all_possible_defect_positions[i].x)) % step_size == 0 &&
                            static_cast<std::size_t>(std::abs(all_possible_defect_positions[i].y)) % step_size == 0)
                        {
                            does_defect_influence(spec, all_possible_defect_positions[i]);
                        }
                    }
                });
        }

        for (auto& thread : threads)
        {
            if (thread.joinable())
            {
                thread.join();
            }
        }

        log_stats();

        return influence_domain;
    }

    /**
     * This function checks for a certain number of random positions (given by `samples`) in the area (spanned by
     * `nw_cell` and `se_cell`) if the existence of a defect leads to an influence of the layout.
     *
     * @param samples The number of positions to sample. The actual number of iterations may be less than the total
     * number of positions or the `samples` value.
     * @return The defect influence domain.
     */
    template <typename TT = tt>
    [[nodiscard]] defect_influence_domain<Lyt>
    random_sampling(const std::size_t samples, const std::optional<std::vector<TT>>& spec = std::nullopt) noexcept
    {
        mockturtle::stopwatch stop{stats.time_total};

        // Get all possible defect positions within the grid spanned by nw_cell and se_cell
        auto all_possible_defect_positions = all_coordinates_in_spanned_area(nw_cell, se_cell);

        // Shuffle the vector using std::shuffle
        std::shuffle(all_possible_defect_positions.begin(), all_possible_defect_positions.end(), generator);

        // Determine how many positions to sample (use the smaller of samples or the total number of positions)
        const auto min_iterations = std::min(all_possible_defect_positions.size(), samples);

        const auto number_of_threads = std::min(num_threads, min_iterations);

        // calculate the size of each slice
        const auto slice_size = (min_iterations + number_of_threads - 1) / number_of_threads;

        std::vector<std::thread> threads{};
        threads.reserve(number_of_threads);

        // launch threads, each with its own slice of random step points
        for (auto t = 0ul; t < number_of_threads; ++t)
        {
            const auto start = t * slice_size;
            const auto end   = std::min(start + slice_size, min_iterations);

            if (start >= end)
            {
                break;  // no more work to distribute
            }

            threads.emplace_back(
                [this, start, end, &all_possible_defect_positions, &spec]
                {
                    for (auto i = start; i < end; ++i)
                    {
                        does_defect_influence(spec, all_possible_defect_positions[i]);
                    }
                });
        }

        for (auto& thread : threads)
        {
            if (thread.joinable())
            {
                thread.join();
            }
        }

        log_stats();  // Log the statistics after processing

        // Return the computed defect influence domain
        return influence_domain;
    }

    /**
     * This function applies contour tracing to identify the boundary (contour) between influencing and
     * non-influencing defect positions for a given SiDB layout.
     *
     * The algorithm leverages the concept of a screened Coulomb potential, where the electrostatic interaction weakens
     * as distance increases. If a defect at a position p allows the SiDB layout to remain operational, then defects
     * further from the layout are also likely to maintain operability. Conversely, defects closer to the layout may
     * cause it to fail. This behavior allows for efficient contour tracing of the transition between operational and
     * non-operational states.
     *
     * The process is as follows:
     * 1. **Initialization**: Randomly select `samples` initial defect positions several nanometers away
     *    from the layout where the layout is known to be operational.
     * 2. **Contour Tracing**: For each position, perform a defect-aware physical simulation to identify adjacent
     *    positions along the x-axis that change the operational state of the layout. This marks the starting
     *    point of the contour.
     * 3. **Contour Following**: Trace the contour of operational positions until the starting point is reached again,
     *    thereby closing the contour.
     * 4. **Repetition**: Repeat steps 1-3 for multiple initial heights to identify additional contours, since
     *    multiple operational-to-non-operational boundaries may exist.
     *
     * @param samples The number of random initial positions used to identify and trace operational
     *                and non-operational defect boundaries. Higher values increase the chance of capturing all
     *                relevant contours but increase computation time.
     * @return The defect operational domain.
     */
    template <typename TT = tt>
    [[nodiscard]] defect_influence_domain<Lyt>
    quicktrace(const std::size_t samples, const std::optional<std::vector<TT>>& spec = std::nullopt) noexcept
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
            const auto operational_starting_point = find_operational_defect_position_at_left_side(spec);

            // if no operational point was found within the specified number of samples, return
            if (!operational_starting_point.has_value())
            {
                return influence_domain;
            }

            // check if the starting point has already been sampled
            if (starting_points.find(*operational_starting_point) != starting_points.cend())
            {
                continue;
            }

            // add an operational starting point to the set of starting points
            starting_points.insert(*operational_starting_point);

            // the layout has to be defect-free.
            assert(layout.num_defects() == 0 && "An atomic defect is added");

            // find an operational point on the contour starting from the randomly determined starting point
            const auto contour_starting_p =
                find_last_operational_defect_position_moving_right(spec, *operational_starting_point);

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

            // if the backtrack point is not part of the neighborhood, continue with the next starting point
            if (std::find(current_neighborhood.cbegin(), current_neighborhood.cend(), backtrack_point) ==
                current_neighborhood.cend())
            {
                continue;
            }

            auto next_point = current_contour_point == backtrack_point ?
                                  current_neighborhood.front() :
                                  next_clockwise_point(current_neighborhood, backtrack_point);

            while (next_point != contour_starting_point)
            {
                const auto defect_influence_status = does_defect_influence(spec, next_point);

                assert(layout.num_defects() == 0 && "more than one defect");

                if (defect_influence_status == defect_influence_status::EXISTENT || !layout.is_empty_cell(next_point))
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
     * The SiDB cell-level layout to investigate.
     */
    sidb_defect_surface<Lyt> layout{};
    /**
     * The parameters for the operational domain computation.
     */
    const defect_influence_params<cell<Lyt>>& params;
    /**
     * North-west cell.
     */
    typename Lyt::cell nw_cell{};
    /**
     * The north-west cell of the bounding box of the layout.
     */
    typename Lyt::cell nw_bb_layout{};
    /**
     * South-east cell.
     */
    typename Lyt::cell se_cell{};
    /**
     * The south-east cell of the bounding box of the layout.
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
    defect_influence_domain<Lyt> influence_domain{};
    /**
     * The statistics of the operational domain computation.
     */
    defect_influence_stats& stats;
    /**
     * Random number generator.
     */
    inline static std::mt19937_64 generator{std::random_device{}()};
    /**
     * Uniform distribution for the y-coordinate of the defect.
     */
    std::uniform_int_distribution<decltype(nw_cell.y)> dist{};
    /**
     * Number of simulator invocations.
     */
    std::atomic<std::size_t> num_simulator_invocations{0};
    /**
     * Number of evaluated defect positions.
     */
    std::atomic<std::size_t> num_evaluated_defect_positions{0};
    /**
     * Number of available hardware threads.
     */
    const std::size_t num_threads{std::thread::hardware_concurrency()};
    /**
     * This function determines the northwest and southeast cells based on the layout and the additional scan
     * area specified.
     */
    void determine_nw_se_cells() noexcept
    {
        // bounding box around the given layout to have north-west and south-east cells.
        bounding_box_2d bb{layout};

        auto nw = bb.get_min();  // north-west cell
        auto se = bb.get_max();  // south-east cell

        nw_bb_layout = nw;
        se_bb_layout = se;

        // shift nw and se cell by the additional scanning area to cover an area that is larger than the layout area.
        nw.x = nw.x - params.additional_scanning_area.x;
        nw.y = nw.y - params.additional_scanning_area.y;

        se.x = se.x + params.additional_scanning_area.x;
        se.y = se.y + params.additional_scanning_area.y;

        nw_cell = nw;
        se_cell = se;

        dist = std::uniform_int_distribution<decltype(nw_cell.y)>{nw_cell.y, se_cell.y};
    }
    /**
     * This function aims to identify an operational defect position within the layout. It does so by selecting a defect
     * position with the leftmost x-coordinate and a randomly selected y-coordinate limited the layout's bounding box.
     *
     * @return The operational defect position. If no operational defect position is found, `std::nullopt` is returned.
     */
    template <typename TT>
    [[nodiscard]] std::optional<typename Lyt::cell>
    find_operational_defect_position_at_left_side(const std::optional<std::vector<TT>>& spec) noexcept
    {
        auto starting_point = nw_cell;

        starting_point.y = dist(generator);

        layout.assign_sidb_defect(starting_point, params.defect);

        const auto operational_value = does_defect_influence(spec, starting_point);
        layout.assign_sidb_defect(starting_point, sidb_defect{sidb_defect_type::NONE});

        // if the parameter combination is operational, return its step values in x and y dimension
        if (operational_value == defect_influence_status::NON_EXISTENT)
        {
            return starting_point;
        }

        return std::nullopt;
    }
    /**
     * This function evaluates the operational status of the SiDB layout when a defect is placed at position `c`.
     *
     * @param c Position of the defect.
     */
    template <typename TT>
    defect_influence_status does_defect_influence(const std::optional<std::vector<TT>>& spec,
                                                  const typename Lyt::cell&             defect_cell) noexcept
    {
        // increment the number of evaluated parameter combinations
        ++num_evaluated_defect_positions;

        auto lyt_copy = layout.clone();

        // if the point has already been sampled, return the stored operational status
        if (const auto op_value = has_already_been_sampled(defect_cell); op_value.has_value())
        {
            return *op_value;
        }

        const auto non_influential = [this, &defect_cell]()
        {
            ++num_simulator_invocations;
            influence_domain.influence_information[defect_cell] = defect_influence_status::NON_EXISTENT;

            return defect_influence_status::NON_EXISTENT;
        };

        const auto influential = [this, &defect_cell]()
        {
            ++num_simulator_invocations;
            influence_domain.influence_information[defect_cell] = defect_influence_status::EXISTENT;

            return defect_influence_status::EXISTENT;
        };

        if (!lyt_copy.is_empty_cell(defect_cell))
        {
            return non_influential();
        }

        lyt_copy.assign_sidb_defect(defect_cell, params.defect);

        if (spec.has_value())
        {
            if (params.influence_def == defect_influence_params<cell<Lyt>>::influence_definition::OPERATIONALITY_CHANGE)
            {
                const auto [status, result] = is_operational(lyt_copy, spec.value(), params.operational_params);
                if (status == operational_status::OPERATIONAL)
                {
                    lyt_copy.assign_sidb_defect(defect_cell, sidb_defect{sidb_defect_type::NONE});
                    return non_influential();
                }
                lyt_copy.assign_sidb_defect(defect_cell, sidb_defect{sidb_defect_type::NONE});
                return influential();
            }

            if (params.influence_def == defect_influence_params<cell<Lyt>>::influence_definition::GROUND_STATE_CHANGE)
            {
                auto bii = bdl_input_iterator<Lyt>{lyt_copy, params.operational_params.input_bdl_iterator_params};

                // number of different input combinations
                for (auto i = 0u; i < spec.value().front().num_bits(); ++i, ++bii)
                {
                    ++num_simulator_invocations;
                    if (does_defect_influence_groundstate(*bii, defect_cell) == defect_influence_status::EXISTENT)
                    {
                        return influential();
                    }
                }
                return non_influential();
            }
        }

        else
        {
            if (params.influence_def == defect_influence_params<cell<Lyt>>::influence_definition::GROUND_STATE_CHANGE)
            {
                if (does_defect_influence_groundstate(lyt_copy, defect_cell) == defect_influence_status::EXISTENT)
                {
                    lyt_copy.assign_sidb_defect(defect_cell, sidb_defect{sidb_defect_type::NONE});
                    return influential();
                }
                return non_influential();
            }

            static_assert(true, "No truth table provided, but influence definition is not ground state change.");
        }

        return non_influential();
    }

    /**
     *
     */
    [[nodiscard]] defect_influence_status
    does_defect_influence_groundstate(const Lyt& lyt_without_defect, const typename Lyt::cell& defect_pos) noexcept
    {
        static_assert(!is_sidb_defect_surface_v<Lyt>, "Lyt should not be an SiDB defect surface");

        if (layout.is_empty())
        {
            return defect_influence_status::EXISTENT;
        }

        const quickexact_params<cell<Lyt>> qe_params{
            params.operational_params.simulation_parameters,
            quickexact_params<cell<Lyt>>::automatic_base_number_detection::OFF};

        mockturtle::stopwatch stop{stats.time_total};

        const auto simulation_results = quickexact(lyt_without_defect, qe_params);

        const auto ground_states = groundstate_from_simulation_results(simulation_results);

        if (lyt_without_defect.get_cell_type(defect_pos) == Lyt::technology::cell_type::EMPTY)
        {
            sidb_defect_surface<Lyt> lyt_defect{lyt_without_defect};

            lyt_defect.assign_sidb_defect(defect_pos, params.defect);

            if (can_positive_charge_occur(lyt_defect, params.operational_params.simulation_parameters))
            {
                return defect_influence_status::EXISTENT;
            }

            // conduct simulation with defect
            auto simulation_result_defect = quickexact(lyt_defect, qe_params);

            const auto ground_states_defect = groundstate_from_simulation_results(simulation_result_defect);

            if (ground_states.size() != ground_states_defect.size())
            {
                return defect_influence_status::EXISTENT;
            }

            for (const auto& gs_defect : ground_states_defect)
            {
                const auto same_ground_state_was_found = std::any_of(
                    ground_states.cbegin(), ground_states.cend(), [&gs_defect](const auto& gs)
                    { return gs.get_charge_index_and_base().first == gs_defect.get_charge_index_and_base().first; });

                if (!same_ground_state_was_found)
                {
                    return defect_influence_status::EXISTENT;
                }
            }

            return defect_influence_status::NON_EXISTENT;
        }

        // defect is placed on a non-empty cell
        return defect_influence_status::NON_EXISTENT;
    };
    /**
     * This function verifies whether the layout has already been analyzed for the specified defect position `c`.
     *
     * @param c Position of the defect.
     */
    [[nodiscard]] std::optional<defect_influence_status>
    has_already_been_sampled(const typename Lyt::cell& c) const noexcept
    {
        if (const auto it = influence_domain.influence_information.find(c);
            it != influence_domain.influence_information.cend())
        {
            return it->second;
        }

        return std::nullopt;
    }
    /**
     * This function identifies the most recent operational defect position while traversing from left to right
     * towards the SiDB layout.
     *
     * @param starting_defect_position The starting position of the defect, from which the traversal towards the
     * right is conducted while maintaining layout operability.
     * @return The last operational defect position. If no non-operational defect is found, `std::nullopt` is
     * returned.
     */
    template <typename TT>
    [[nodiscard]] std::optional<typename Lyt::cell>
    find_last_operational_defect_position_moving_right(const std::optional<std::vector<TT>>& spec,
                                                       const typename Lyt::cell& starting_defect_position) noexcept
    {
        auto latest_operational_defect_position = starting_defect_position;

        previous_defect_position = starting_defect_position;

        // move towards the left border of the parameter range
        for (auto x = starting_defect_position.x; x <= se_cell.x; x++)
        {
            previous_defect_position = current_defect_position;
            current_defect_position  = {x, starting_defect_position.y};

            layout.assign_sidb_defect(current_defect_position, params.defect);

            const auto influence_status = does_defect_influence(spec, current_defect_position);

            layout.assign_sidb_defect(current_defect_position, sidb_defect{sidb_defect_type::NONE});

            if (influence_status == defect_influence_status::NON_EXISTENT)
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
        stats.num_simulator_invocations      = num_simulator_invocations.load();
        stats.num_evaluated_defect_positions = num_evaluated_defect_positions.load();

        for (const auto& [param_point, status] : influence_domain.influence_information)
        {
            if (status == defect_influence_status::EXISTENT)
            {
                ++stats.num_influencing_defect_positions;
            }
            else
            {
                ++stats.num_non_influencing_defect_positions;
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
    }
};

};  // namespace detail

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
[[nodiscard]] defect_influence_domain<Lyt>
defect_influence_grid_search(const Lyt& lyt, const std::vector<TT>& spec,
                             const defect_influence_params<cell<Lyt>>& params = {}, const std::size_t step_size = 1,
                             defect_influence_stats* stats = nullptr)
{
    static_assert(is_cell_level_layout_v<Lyt>, "Lyt is not a cell-level layout");
    static_assert(has_sidb_technology_v<Lyt>, "Lyt is not an SiDB layout");
    static_assert(kitty::is_truth_table<TT>::value, "TT is not a truth table");
    static_assert(has_cube_coord_v<Lyt>, "Lyt is not based on cube coordinates");

    defect_influence_stats             st{};
    detail::defect_influence_impl<Lyt> p{lyt, params, st};

    const auto result = p.grid_search(step_size, std::optional{spec});

    if (stats)
    {
        *stats = st;
    }

    return result;
}

/**
 * @tparam Lyt SiDB cell-level layout type.
 * @param lyt Layout to compute the defect operational domain for.
 * @param step_size The parameter specifying the interval between consecutive defect positions to be evaluated.
 * @param params Defect operational domain computation parameters.
 * @param stats Statistics.
 * @return The defect operational domain of the layout.
 */
template <typename Lyt>
[[nodiscard]] defect_influence_domain<Lyt>
defect_influence_grid_search(const Lyt& lyt, const defect_influence_params<cell<Lyt>>& params = {},
                             const std::size_t step_size = 1, defect_influence_stats* stats = nullptr)
{
    static_assert(is_cell_level_layout_v<Lyt>, "Lyt is not a cell-level layout");
    static_assert(has_sidb_technology_v<Lyt>, "Lyt is not an SiDB layout");
    static_assert(has_cube_coord_v<Lyt>, "Lyt is not based on cube coordinates");

    defect_influence_stats             st{};
    detail::defect_influence_impl<Lyt> p{lyt, params, st};

    const auto result = p.grid_search(step_size);

    if (stats)
    {
        *stats = st;
    }

    return result;
}
/**
 * Computes the defect operational domain of the given SiDB cell-level layout. The defect operational domain is the
 * set of all defect positions for which the layout is logically operational. Logical operation is defined as the layout
 * implementing the given truth table. The input BDL pairs of the layout are assumed to be in the same order as the
 * inputs of the truth table.
 *
 * This algorithm uses random sampling to find a part of the defect operational domain that might not be
 * complete. It performs a total of `samples uniformly-distributed random samples within the specified area.
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
[[nodiscard]] defect_influence_domain<Lyt>
defect_influence_random_sampling(const Lyt& lyt, const std::vector<TT>& spec, std::size_t samples,
                                 const defect_influence_params<cell<Lyt>>& params = {},
                                 defect_influence_stats*                   stats  = nullptr)
{
    static_assert(is_cell_level_layout_v<Lyt>, "Lyt is not a cell-level layout");
    static_assert(has_sidb_technology_v<Lyt>, "Lyt is not an SiDB layout");
    static_assert(kitty::is_truth_table<TT>::value, "TT is not a truth table");
    static_assert(has_cube_coord_v<Lyt>, "Lyt is not based on cube coordinates");

    defect_influence_stats             st{};
    detail::defect_influence_impl<Lyt> p{lyt, params, st};

    const auto result = p.random_sampling(samples, std::optional{spec});

    if (stats)
    {
        *stats = st;
    }

    return result;
}

/**
 * @tparam Lyt SiDB cell-level layout type.
 * @param lyt Layout to compute the defect influence domain for.
 * @param samples Number of random samples to perform.
 * @param params Defect influence domain computation parameters.
 * @param stats Statistics.
 * @return The (partial) defect influence domain of the layout.
 */
template <typename Lyt>
[[nodiscard]] defect_influence_domain<Lyt>
defect_influence_random_sampling(const Lyt& lyt, std::size_t samples,
                                 const defect_influence_params<cell<Lyt>>& params = {},
                                 defect_influence_stats*                   stats  = nullptr)
{
    static_assert(is_cell_level_layout_v<Lyt>, "Lyt is not a cell-level layout");
    static_assert(has_sidb_technology_v<Lyt>, "Lyt is not an SiDB layout");
    static_assert(has_cube_coord_v<Lyt>, "Lyt is not based on cube coordinates");

    defect_influence_stats             st{};
    detail::defect_influence_impl<Lyt> p{lyt, params, st};

    const auto result = p.random_sampling(samples);

    if (stats)
    {
        *stats = st;
    }

    return result;
}

/**
 * Computes the defect influence domain of the given SiDB cell-level layout. The defect
 * influence domain is the set of defect positions together with the information if the layout is influenced. This means
 * depending on the definition that the layout changes its operationality status or its ground state.
 *
 * This algorithm uses contour tracing to identify non-influencing defect locations within the SiDB layout.
 * It starts by searching for defect locations on the left side (bounding_box + additional scanning area). The
 * y-coordinate for these positions is chosen randomly. The number of samples is determined by the `samples` parameter.
 *
 * Then, the algorithm moves each defect position to the right, searching for the influencing defect position before
 * finding the non-influencing one. This position is selected as the starting point for the contour trace. The contour
 * tracing process checks whether the contour includes the SiDB layout. If it does not, the next random sample point is
 * selected as the starting point and the process is repeated.
 *
 * @tparam Lyt SiDB cell-level layout type.
 * @tparam TT Truth table type.
 * @param lyt Layout to compute the defect influence domain for.
 * @param spec Expected Boolean function of the layout given as a multi-output truth table.
 * @param samples Number of samples to perform.
 * @param params Defect influence domain computation parameters.
 * @param stats Defect influence computation statistics.
 * @return The (partial) defect influence domain of the layout.
 */
template <typename Lyt, typename TT>
[[nodiscard]] defect_influence_domain<Lyt>
defect_influence_quicktrace(const Lyt& lyt, const std::vector<TT>& spec, const std::size_t samples,
                            const defect_influence_params<cell<Lyt>>& params = {},
                            defect_influence_stats*                   stats  = nullptr)
{
    static_assert(is_cell_level_layout_v<Lyt>, "Lyt is not a cell-level layout");
    static_assert(has_sidb_technology_v<Lyt>, "Lyt is not an SiDB layout");
    static_assert(has_cube_coord_v<Lyt>, "Lyt is not based on cube coordinates");

    defect_influence_stats             st{};
    detail::defect_influence_impl<Lyt> p{lyt, params, st};

    const auto result = p.quicktrace(samples, std::optional{spec});

    if (stats)
    {
        *stats = st;
    }

    return result;
}

/**
 * @tparam Lyt SiDB cell-level layout type.
 * @param lyt Layout to compute the defect influence domain for.
 * @param samples Number of samples to perform.
 * @param params Defect influence domain computation parameters.
 * @param stats Defect influence computation statistics.
 * @return The (partial) defect influence domain of the layout.
 */
template <typename Lyt>
[[nodiscard]] defect_influence_domain<Lyt>
defect_influence_quicktrace(const Lyt& lyt, const std::size_t samples,
                            const defect_influence_params<cell<Lyt>>& params = {},
                            defect_influence_stats*                   stats  = nullptr)
{
    static_assert(is_cell_level_layout_v<Lyt>, "Lyt is not a cell-level layout");
    static_assert(has_sidb_technology_v<Lyt>, "Lyt is not an SiDB layout");
    static_assert(has_cube_coord_v<Lyt>, "Lyt is not based on cube coordinates");

    defect_influence_stats             st{};
    detail::defect_influence_impl<Lyt> p{lyt, params, st};

    const auto result = p.quicktrace(samples);

    if (stats)
    {
        *stats = st;
    }

    return result;
}

}  // namespace fiction

#endif  // FICTION_DEFECT_INFLUENCE_HPP
